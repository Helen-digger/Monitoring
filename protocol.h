#include "memstat.h"
#include "cpustat.h"
//#include "id_machine.h"
#include "uuid.h"

#define SERVER_PORT 2048
#define ID_LEN  64

typedef enum  {
    FAILED=-1,
    SUCCESS=1
}ans_code;

typedef struct 
{
    int server_id;
    char client_id;
    ans_code ans;
}server_ans;



typedef struct 
{ 
	char ID_client[ID_LEN];
 	double cpu_stat;
    struct {
    	unsigned  long int MemTotal ;
		unsigned  long int MemFree;
		unsigned  long int MemAvailable;
		unsigned  long int Buffers;
		unsigned  long int Cached;
		unsigned  long int SwapCached;
		unsigned  long int Active;
		unsigned  long int Inactive;
    	} MemList;
} PC_stat;
