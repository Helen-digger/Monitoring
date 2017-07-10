#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "protocol.h"
#define SERVER_ID 12211



void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    //unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(SERVER_PORT);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    PC_stat buffer;

    memset(&buffer, 0, sizeof(PC_stat));   /* Zero out structure */
    server_ans ans_all;
    memset(&ans_all, 0, sizeof(server_ans));   /* Zero out structure */
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock,(void*)&buffer , sizeof(server_ans), 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");

        /*  проверяем сообщение от клиента в зависимости от результата отправляем ответ */
        if (recvMsgSize== sizeof(PC_stat))
        {
            printf("%d\t%f\n", buffer.ID_client, buffer.cpu_stat );

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
    /* NOT REACHED */
}
