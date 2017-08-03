#include "memstat.h"
#include "cpustat.h"
#include "uuid.h"
#include "tbf.h"
#include "id_client.h"
#include "mac_addr.h"
#include "time.h"
#include <errno.h>

#define SERVER_PORT 2048
#define BCAST_PORT  4091
#define CLIENT_PORT 2049
#define IP4_STR_LEN 16
#define TIME_MAX 32

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
	char IP[IP4_STR_LEN];
	double cpu_stat;
	Memlist mem_stat;
	tm time1;
	char time_str[TIME_MAX];
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

#define SK_INIT_BCAST(_sk_)                                                                 \
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
	_sk_.RemoteAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);                              \
	_sk_.RemoteAddr.sin_port        = htons(BCAST_PORT);                                    \
} while (0)

void DieWithError(char *errorMessage);
