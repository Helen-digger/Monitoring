#include "memstat.h"
#include "cpustat.h"
#include "uuid.h"
#include "tbf.h"
#include "id_client.h"
#include "mac_addr.h"

#define SERVER_PORT 2048
#define BCAST_PORT  4091
#define CLIENT_PORT 2049
#define IP4_STR_LEN 16

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
} PC_stat;

typedef struct
{
	int sock;
	struct sockaddr_in SrvAddr;
	struct sockaddr_in ClntAddr;
	int                snd_size;
	char               IP[IP4_STR_LEN];
	int                rcv_size;
	int                ans_code;
} sk_t;

#define SK_INIT(_sk_, _proto_f_, _sk_t_, _proto_, _addr_f_, _ip_, _port_)                   \
do {                                                                                        \
	if (0 > (_sk_.sock = socket(_proto_f_, _sk_t_, _proto_)))                               \
	{                                                                                       \
		fprintf(stderr, "'%s': macro 'SK_INIT': socket() failed!\n", __func__);             \
		return -1;                                                                          \
	}                                                                                       \
	memset(&_sk_.SrvAddr, 0, sizeof(_sk_.SrvAddr));                                         \
	_sk_.SrvAddr.sin_family      = _addr_f_;                                                \
	_sk_.SrvAddr.sin_addr.s_addr = _ip_;                                                    \
	_sk_.SrvAddr.sin_port        = htons(_port_);                                           \
	if (0 > bind(_sk_.sock, (const struct sockaddr *) &_sk_.SrvAddr, sizeof(_sk_.SrvAddr))) \
	{                                                                                       \
		fprintf(stderr, "'%s' :macro 'SK_INIT': bind() failed!\n", __func__);               \
		perror("Произошла ошибка: ");                                                       \
		return -1;                                                                          \
	}                                                                                       \
} while (0)

#define SK_INIT_BCAST(_sk_, _proto_f_, _sk_t_, _proto_, _addr_f_, _ip_, _port_)             \
do {                                                                                        \
	if (0 > (_sk_.sock = socket(_proto_f_, _sk_t_, _proto_)))                               \
	{                                                                                       \
		fprintf(stderr, "'%s': macro 'SK_INIT_BCAST': socket() failed!\n", __func__);       \
		return -1;                                                                          \
	}                                                                                       \
	int broadcastPermission = 1;                                                            \
	if (0 > setsockopt(_sk_.sock,                                                           \
	                   SOL_SOCKET,                                                          \
	                   SO_BROADCAST,                                                        \
	                   (void *) &broadcastPermission,                                       \
	                   sizeof(broadcastPermission)))                                        \
	{                                                                                       \
		fprintf(stderr, "'%s' :macro 'SK_INIT_BCAST': setsockopt() failed!\n", __func__);   \
		return -1;                                                                          \
	}                                                                                       \
	memset(&_sk_.SrvAddr, 0, sizeof(_sk_.SrvAddr));                                         \
	_sk_.SrvAddr.sin_family      = _addr_f_;                                                \
	_sk_.SrvAddr.sin_addr.s_addr = _ip_;                                                    \
	_sk_.SrvAddr.sin_port        = htons(_port_);                                           \
} while (0)

void DieWithError(char *errorMessage);
