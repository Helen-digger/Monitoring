#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "protocol.h"
#include <sys/time.h> /* select() */ 
#include <sys/select.h>//use select() for multiplexing
#include <sys/ioctl.h>
#define SERVER_ID 12211
#define SOCKET_ERROR -1
#define BroadcastPort 2049
/* Select() params
     * int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
     * FD_SET(int fd, fd_set *set);
     * FD_CLR(int fd, fd_set *set);
     * FD_ISSET(int fd, fd_set *set);
     * FD_ZERO(fd_set *set);
    */

int isReadable(int sock,int * error,int timeOut) { // milliseconds
  fd_set socketReadSet;
  FD_ZERO(&socketReadSet);
  FD_SET(sock,&socketReadSet);
  struct timeval tv;
  if (timeOut) {
    tv.tv_sec  = timeOut / 1000;
    tv.tv_usec = (timeOut % 1000) * 1000;
  } else {
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
  } // if
  if (select(sock+1,&socketReadSet,0,0,&tv) == SOCKET_ERROR) {
    *error = 1;
    return 0;
  } // if
  *error = 0;
  return FD_ISSET(sock,&socketReadSet) != 0;
} /* isReadable */

void sendBroad(char *dstIP, char *localIP)
{
	int so; /* Socket */
	struct sockaddr_in broadcastAddr; /* Broadcast address */
	int broadcastPermission; /* Socket opt to set permission to broadcast */
	unsigned int localIPLen; /* Length of string to broadcast */

	/* Create socket for sending/receiving datagrams */
	if ((so = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	perror("socket() failed");

	/* Set socket to allow broadcast */
	broadcastPermission = 1;
	if (setsockopt(so, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
	sizeof(broadcastPermission)) < 0)
	perror("setsockopt() failed");

	/* Construct local address structure */
	memset(&broadcastAddr, 0, sizeof(broadcastAddr)); /* Zero out structure */
	broadcastAddr.sin_family = AF_INET; /* Internet address family */
	broadcastAddr.sin_addr.s_addr = inet_addr(dstIP); /* Broadcast IP address */
	broadcastAddr.sin_port = htons(BroadcastPort); /* Broadcast port */

	localIPLen = strlen(localIP); /* Find length of localIP */
	int j;
	for (j=0; j<1; j++) //doesnt mean anything so far, not important
	{
	/* Broadcast localIP in datagram to clients */
	if (sendto(so, localIP, localIPLen, 0, (struct sockaddr *) 
	&broadcastAddr, sizeof(broadcastAddr)) != localIPLen)
	perror("sendto() sent a different number of bytes than expected");

	}
	close(so);
}

void DieWithError(char *errorMessage);  /* External error handling function */



int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    //unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
   struct timeval       timeout;
   int   error, timeOut;

   // void sendBroad(char *dstIP, char *localIP);

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

	timeOut = 100; // ms

  	
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(SERVER_PORT);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");
    timeout.tv_sec  = 3 * 60;
    timeout.tv_usec = 0;

    PC_stat buffer;

    memset(&buffer, 0, sizeof(PC_stat));   /* Zero out structure */
    server_ans ans_all;
    memset(&ans_all, 0, sizeof(server_ans));   /* Zero out structure */
	
	

    for (;;) /* Run forever */
    {

    	 if (isReadable(sock,&error,timeOut)) 
    	 {
	    		//printf("\n");
	        /* Set the size of the in-out parameter */
	        cliAddrLen = sizeof(echoClntAddr);
	        

	        /* Block until receive message from a client */
	        if ((recvMsgSize = recvfrom(sock,(void*)&buffer , sizeof(PC_stat), 0,
	            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
	            DieWithError("recvfrom() failed");
	        //printf("%d\n",recvMsgSize );
	         //printf("%d\t%f\n", buffer.ID_client, buffer.cpu_stat );

	        /*  проверяем сообщение от клиента в зависимости от результата отправляем ответ */
	        if (recvMsgSize== sizeof(PC_stat))
	        {   timeOut = 100; // ms
	  		 	

	          printf("ID клиента:\t%s\n", buffer.ID_client);
	          printf("Загрузка процессора:\t%f\n", buffer.cpu_stat);
              printf("Состояние оперативной памяти:\n MemTotal:%32lu kB\n MemFree: %32lu kB\n MemAvailable: %32lu kB\n Buffers: %32lu kB\n Cached: %32lu kB\n SwapCached: %32lu kB\n Active: %32lu kB\n Inactive: %32lu kB\n\n", 
                 buffer.MemList.MemTotal, 
                 buffer.MemList.MemFree, 
                 buffer.MemList.MemAvailable,
                 buffer.MemList.Buffers, 
                 buffer.MemList.Cached,
                 buffer.MemList.SwapCached, 
                 buffer.MemList.Active,
                 buffer.MemList.Inactive);

	            ans_all.server_id=SERVER_ID;
	            ans_all.client_id=buffer.ID_client;
	            ans_all.ans=SUCCESS;

	            if (sendto(sock,(void*)&ans_all, sizeof(server_ans), 0, 
	                (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sizeof(server_ans))
	            DieWithError("sendto() sent a different number of bytes than expected");
	            //else(  printf("%lf\n", ans_all));
	       
	        }


	        else 
	        {
	            printf(" error: поврежденно сообщение " );

	            ans_all.server_id=SERVER_ID;
	            ans_all.client_id=0;
	            ans_all.ans=FAILED;

	            if (sendto(sock,(void*)&ans_all, sizeof(server_ans), 0, 
	                (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sizeof(server_ans))
	            DieWithError("sendto() sent a different number of bytes than expected");
	        }
        }
        else printf(".");
    	
    /* NOT REACHED */}
}
