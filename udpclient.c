#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "cpustat.h"
#include "protocol.h"
//#define IP 192.168.2.4
#define ID 1111

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
   // unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *IP;                    /* IP address of server */
    //char *echoString;                /* String to send to echo server */
    //char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    //int echoStringLen;               /* Length of string to echo */
    //int respStringLen;               /* Length of received response */
 
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
    client_status.ID_client=ID;

    server_ans ans_me;
    memset(&ans_me, 0, sizeof(server_ans));   /* Zero out structure */

    for(;;)
    {   
        
        if (0!=ProcessList_scan(&client_status.cpu_stat))
            DieWithError("ProcessList failed!!!!!");

        /* Send the string to the server */
        if (sendto(sock, &client_status, sizeof(PC_stat), 0, (struct sockaddr *)
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
    exit(0);
}
