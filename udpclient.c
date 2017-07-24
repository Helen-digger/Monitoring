#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"

int get_srv_ip(sk_t * sk)
{
	//printf("%s\n", __func__);
	sk_t sk_bcast;
	SK_INIT(sk_bcast, PF_INET, SOCK_DGRAM, IPPROTO_UDP, AF_INET, htonl(INADDR_ANY), BCAST_PORT);

	//for (;;)
	//{
		//if (0 > (
		sk_bcast.rcv_size = recvfrom(sk_bcast.sock,
		                                      (void *) sk->IP,
		                                      sizeof(sk->IP),
		                                      0, NULL, 0);//))
		//{
			printf("size %d IP(%s)\n", sk_bcast.rcv_size, sk->IP);
			//break;
		//	fprintf(stderr, "'%s': recvfrom() failed!\n", __func__);
		//	return -1;
		//}
		//sleep(1);
	//}

	close(sk_bcast.sock);

	return 0;
}

int client_build_msg(PC_stat * cl_stat)
{
	printf("%s\n", __func__);
	if (0 != CPU_info(&cl_stat->cpu_stat))
	{
		fprintf(stderr, "'%s': ProcessList() failed!\n", __func__);
		return -1;
	}
	if (0 != Mem_Info(&cl_stat->mem_stat)) 
	{
		fprintf(stderr, "'%s': Mem_Info() failed!\n", __func__);
		return -1;
	}
	if (0 != Uuid(cl_stat->client_id))
	{
		fprintf(stderr,"'%s': Uuid() failed!\n", __func__);
		return -1;
	}
	if (0!=GetIP(cl_stat->IP))
    {
		fprintf(stderr,"'%s': IP() failed!\n", __func__);
		return -1;
	}
    if (0!=MAC_address(cl_stat->mac))
            {
		fprintf(stderr,"'%s': Mac() failed!\n", __func__);
		return -1;
	}
	return 0;
}

int client_handle_msg(sk_t * sk, PC_stat * cl_stat, server_ans * ans)
{
	printf("%s 0\n", __func__);
	if ( sizeof(PC_stat) != sendto(sk->sock,
	                               (void*)cl_stat,
	                               sizeof(PC_stat),
	                               0,
	                               (struct sockaddr *) &sk->SrvAddr,
	                               sizeof(sk->SrvAddr)))
	{
		fprintf(stderr, "'%s': sendto() failed!\n", __func__);
		return -1;
	}
	printf("%s 1\n", __func__);
	unsigned int sk_size = sizeof(sk->ClntAddr);
	if ( sizeof(server_ans) !=  (recvfrom(sk->sock,
	                                      ((void*)ans),
	                                      sizeof(server_ans),
	                                      0,
	                                      (struct sockaddr *) &sk->ClntAddr,
	                                      &sk_size)))
	{
		fprintf(stderr, "'%s': recvfrom() failed!\n", __func__);
		return -1;
	}
	printf("%s 2\n", __func__);
	if (sk->SrvAddr.sin_addr.s_addr != inet_addr(sk->IP))
	{
		fprintf(stderr,"'%s': received a packet from unknown source.\n", __func__);
		return -1;
	}
	printf("%s return\n", __func__);
	return 0;
}

int main(int argc, char *argv[])
{
	tbf snd_rl = (tbf) {.rate = 1, .burst = 1};
	sk_t sk;

	PC_stat client_status;
	server_ans ans_me;

	memset(&client_status, 0, sizeof(PC_stat));
	memset(&ans_me, 0, sizeof(server_ans));

	if (0 != get_srv_ip(&sk))
	{
		fprintf(stderr, "'%s': get_srv_ip() failed!\n", __func__);
		return -1;
	}
	
	SK_INIT(sk, PF_INET, SOCK_DGRAM, IPPROTO_UDP, AF_INET, inet_addr(sk.IP), CLIENT_PORT);
	for(;;)
	{
		if (!tbf_rl(&snd_rl)) continue;

		if (0 != client_build_msg(&client_status))
		{
			fprintf(stderr, "'%s': client_build_msg() failed!\n", __func__);
			return -1;
		}

		if (0 != client_handle_msg(&sk, &client_status, &ans_me))
		{
			fprintf(stderr, "'%s': client_handle_msg() failed!\n", __func__);
			return -1;
		}
	}
	close(sk.sock);

	return (0);
}
