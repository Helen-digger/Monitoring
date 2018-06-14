#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <poll.h>

#include "memstat.h"
#include "cpustat.h"
#include "uuid.h"
#include "tbf.h"
#include "id_client.h"
#include "get_addr.h"
#include "time.h"
#include "loadavg.h"


#define SOCKET_ERROR        -1

#define SERVER_PORT         2048
#define SRV_CAST_RL (tbf) {.rate = 1, .burst = 1}
#define CLT_SEND_RL (tbf) {.rate = 1, .burst = 5}

#define MCAST_TTL           64
#define MCAST_ADDR          "239.0.0.1"
#define MCAST_PORT          4092
#define BCAST_PORT          4091
#define SRV_LISTEN_T        3000
#define ANS_TIMEOUT         5000
#define MISED_ANS_COUNT_MAX 10

#define CLIENT_PORT         2049

#define IP4_STR_LEN         16
#define TIME_MAX            32

typedef enum
{
	FAILED= -1,
	SUCCESS= 1
} ans_code;

typedef struct 
{
	char server_id[ID_LEN];
	char serv_IP[IP4_STR_LEN];
	ans_code ans;
} server_ans;

typedef struct 
{
	char client_id[ID_LEN];
	char mac[LEN_MAC];
	//char IP[IP4_STR_LEN];
	double cpu_stat;
	Memlist mem_stat;
	struct tm time1;
	char time_str[TIME_MAX];
	AVGList avginfo;
} PC_stat;

typedef struct
{
	int                s_in;
	struct sockaddr_in LocalAddr;
	struct sockaddr_in RemoteAddr;
	struct sockaddr_in AnsAddr;
	int                snd_size;
	char               IP[IP4_STR_LEN];
	int                rcv_size;
	int                ans_code;
} sk_t;

typedef struct
{
	sk_t          sk;
	sk_t          sk_cast;
	tbf           rl_cast;
	PC_stat       buffer;
	server_ans    ans_all;
	struct pollfd poll_fd[1];
	int           poll_rc;
} srv_t;

typedef struct
{
	sk_t          sk;
	pid_t         pid;
	tbf           snd_rl;
	PC_stat       client_status;
	server_ans    ans_me;
	char          iface[IFNAMSIZ];
	struct pollfd poll_fd[1];
	int           poll_rc;
	int           missed_ans;
} clt_t;

#define SK_INIT(_sk_, _l_ip_, _r_ip_, _lport_, _rport_)                           \
do {                                                                              \
	if (0 > (_sk_.s_in = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))               \
	{                                                                             \
		fprintf(stderr, "'%s': macro 'SK_INIT': socket() failed!\n", __func__);   \
		return -1;                                                                \
	}                                                                             \
	memset(&_sk_.AnsAddr, 0, sizeof(_sk_.AnsAddr));                               \
	memset(&_sk_.LocalAddr, 0, sizeof(_sk_.LocalAddr));                           \
	_sk_.LocalAddr.sin_family      = AF_INET;                                     \
	_sk_.LocalAddr.sin_addr.s_addr = _l_ip_;                                      \
	_sk_.LocalAddr.sin_port        = htons(_lport_);                              \
	memset(&_sk_.RemoteAddr, 0, sizeof(_sk_.RemoteAddr));                         \
	_sk_.RemoteAddr.sin_family      = AF_INET;                                    \
	_sk_.RemoteAddr.sin_addr.s_addr = _r_ip_;                                     \
	_sk_.RemoteAddr.sin_port        = htons(_rport_);                             \
	int optval = 1;                                                               \
	setsockopt(_sk_.s_in, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));     \
	if (0 > bind(_sk_.s_in,                                                       \
	             (const struct sockaddr *) &_sk_.LocalAddr,                       \
	             sizeof(_sk_.LocalAddr)))                                         \
	{                                                                             \
		fprintf(stderr, "'%s' :macro 'SK_INIT': bind() failed!\n", __func__);     \
		perror(": ");                                                             \
		return -1;                                                                \
	}                                                                             \
} while (0)

#define SK_INIT_MCAST_CLT(_sk_, _l_ip_, _r_ip_, _lport_, _rport_)                               \
do {                                                                                            \
	if (0 > (_sk_.s_in = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))                             \
	{                                                                                           \
		fprintf(stderr, "'%s': macro 'SK_INIT_MCAST_CLT': socket() failed!\n", __func__);       \
		return -1;                                                                              \
	}                                                                                           \
	memset(&_sk_.AnsAddr, 0, sizeof(_sk_.AnsAddr));                                             \
	memset(&_sk_.LocalAddr, 0, sizeof(_sk_.LocalAddr));                                         \
	_sk_.LocalAddr.sin_family      = AF_INET;                                                   \
	_sk_.LocalAddr.sin_addr.s_addr = _l_ip_;                                                    \
	_sk_.LocalAddr.sin_port        = htons(_lport_);                                            \
	memset(&_sk_.RemoteAddr, 0, sizeof(_sk_.RemoteAddr));                                       \
	_sk_.RemoteAddr.sin_family      = AF_INET;                                                  \
	_sk_.RemoteAddr.sin_addr.s_addr = _r_ip_;                                                   \
	_sk_.RemoteAddr.sin_port        = htons(_rport_);                                           \
	struct ip_mreq mreq;                                                                        \
	memset(&mreq, 0, sizeof(struct ip_mreq));                                                   \
	mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);                                          \
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);                                              \
	if (0 > setsockopt(_sk_.s_in,                                                               \
	                   IPPROTO_IP,                                                              \
	                   IP_ADD_MEMBERSHIP,                                                       \
	                   (void *) &mreq,                                                          \
	                   sizeof(struct ip_mreq)))                                                 \
	{                                                                                           \
		fprintf(stderr, "'%s' :macro 'SK_INIT_MCAST_CLT': setsockopt() failed!\n", __func__);   \
		return -1;                                                                              \
	}                                                                                           \
	if (0 > bind(_sk_.s_in,                                                                     \
	             (const struct sockaddr *) &_sk_.LocalAddr,                                     \
	             sizeof(_sk_.LocalAddr)))                                                       \
	{                                                                                           \
		fprintf(stderr, "'%s' :macro 'SK_INIT_MCAST_CLT': bind() failed!\n", __func__);         \
		perror(": ");                                                                           \
		return -1;                                                                              \
	}                                                                                           \
} while (0)

#define SK_INIT_BCAST(_sk_, _ip_bcast_)                                                     \
do {                                                                                        \
	if (0 > (_sk_.s_in = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))                         \
	{                                                                                       \
		fprintf(stderr, "'%s': macro 'SK_INIT_BCAST': socket() failed!\n", __func__);       \
		return -1;                                                                          \
	}                                                                                       \
	int broadcastPermission = 1;                                                            \
	if (0 > setsockopt(_sk_.s_in,                                                           \
	                   SOL_SOCKET,                                                          \
	                   SO_BROADCAST,                                                        \
	                   (void *) &broadcastPermission,                                       \
	                   sizeof(broadcastPermission)))                                        \
	{                                                                                       \
		fprintf(stderr, "'%s' :macro 'SK_INIT_BCAST': setsockopt() failed!\n", __func__);   \
		return -1;                                                                          \
	}                                                                                       \
	memset(&_sk_.RemoteAddr, 0, sizeof(_sk_.RemoteAddr));                                   \
	_sk_.RemoteAddr.sin_family      = AF_INET;                                              \
	_sk_.RemoteAddr.sin_addr.s_addr = inet_addr(_ip_bcast_);                                \
	_sk_.RemoteAddr.sin_port        = htons(BCAST_PORT);                                    \
} while (0)     //htonl(INADDR_BROADCAST);

#define SK_INIT_MCAST_SRV(_sk_, _ip_mcast_)                                                 \
do {                                                                                        \
	if (0 > (_sk_.s_in = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))                         \
	{                                                                                       \
		fprintf(stderr, "'%s': macro 'SK_INIT_MCAST': socket() failed!\n", __func__);       \
		return -1;                                                                          \
	}                                                                                       \
	int ttl = MCAST_TTL;                                                                    \
	if (0 > setsockopt(_sk_.s_in,                                                           \
	                   IPPROTO_IP,                                                          \
	                   IP_MULTICAST_TTL,                                                    \
	                   (void *) &ttl,                                                       \
	                   sizeof(ttl)))                                                        \
	{                                                                                       \
		fprintf(stderr, "'%s' :macro 'SK_INIT_MCAST': setsockopt() failed!\n", __func__);   \
		return -1;                                                                          \
	}                                                                                       \
	memset(&_sk_.RemoteAddr, 0, sizeof(_sk_.RemoteAddr));                                   \
	_sk_.RemoteAddr.sin_family      = AF_INET;                                              \
	_sk_.RemoteAddr.sin_addr.s_addr = inet_addr(_ip_mcast_);                                \
	_sk_.RemoteAddr.sin_port        = htons(MCAST_PORT);                                    \
} while (0)
