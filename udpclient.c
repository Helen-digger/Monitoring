#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "protocol.h"

//#define IP 192.168.2.4
//#define ID 1111
typedef struct {
  time_t timestamp;		/* Last update */
  unsigned long long int count;			/* Available tokens */
  unsigned short int burst;			/* Max number of tokens */
  unsigned short int rate;				/* Rate of replenishment */
  unsigned short int mark;				/* Whether last op was limited */
} tbf;

//Если rate = 1, а burst = 10, то раз в 10 секунд фунция будет возвращать 1 (при этом mark будет выведено на экран и занулено)
//Если запросы происходят чаще раз в 10 секунд, функция запишет в mark сколько раз вернула 0(количество подавленных действий)
int tbf_rl(tbf * rl)
{
	time_t now = time(NULL);
	time_t delta = now - rl->timestamp;
	if ((0 <= delta) && (delta < rl->burst))
	{
		rl->count++;
	}
	else
	{
		if (rl->mark)
		{
			printf("%d count of  suppressed actions\n", rl->mark);
			rl->mark = 0;
		}
		rl->timestamp = now;
		rl->count = 0;
		return 1;
	}

	if (rl->count >= rl->rate)
	{
		rl->mark++;
		return 0;
	}
}

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *IP; 
    
     
    fprintf(stderr,"Usage: %s <Server IP>", argv[0]);
    IP = argv[1];
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(IP);  /* Server IP address */
    echoServAddr.sin_port   = htons(SERVER_PORT);     /* Server port */

    PC_stat client_status;


    memset(&client_status, 0, sizeof(PC_stat));
    //client_status.ID_client=ID;

    server_ans ans_me;
    memset(&ans_me, 0, sizeof(server_ans));   /* Zero out structure */

    tbf rl = (tbf) {.rate = 1, .burst = 10};

	for(;;)
	{   
		if (!tbf_rl(&rl)) continue;
 
    	if (0!=Uuid(&client_status.ID_client))
            DieWithError("ProcessList failed!!!!!");
        if (0!=ProcessList_scan(&client_status.cpu_stat))
            DieWithError("ProcessList failed!!!!!");
        if (0!=MemInfo(&client_status.MemList))
            DieWithError("MemList failed!!!!!");

        /* Send the string to the server */
        if (sendto(sock, (void*)&client_status, sizeof(PC_stat), 0, (struct sockaddr *)
           &echoServAddr, sizeof(echoServAddr)) != sizeof(PC_stat))
           DieWithError("sendto() sent a different number of bytes than expected");
        /* Recv a response */
    
        fromSize = sizeof(fromAddr);
        if ((recvfrom(sock, ((void*)&ans_me), sizeof(server_ans), 0, 
            (struct sockaddr *) &fromAddr, &fromSize)) !=  sizeof(server_ans))
            DieWithError("recvfrom() failed");

        if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
            fprintf(stderr,"Error: received a packet from unknown source.\n");
            exit(1);
        }
	}
	close(sock);
    return (0);
}
