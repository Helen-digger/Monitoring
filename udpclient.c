#include "protocol.h"

int get_srv_ip(sk_t * sk)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	sk_t sk_bcast;
	SK_INIT(sk_bcast, htonl(INADDR_ANY), htonl(INADDR_ANY), BCAST_PORT, 0);
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	//for (;;)
	//{
		unsigned int sk_size = sizeof(sk_bcast.AnsAddr);
		//if (0 > (
		sk_bcast.rcv_size = recvfrom(sk_bcast.s_in,
		                                      0,//(void *) sk->IP,
		                                      0,//sizeof(sk->IP),
		                                      0,
		                                      (struct sockaddr *) &sk_bcast.AnsAddr,
	                                          &sk_size);//))
		//{
			//printf("size %d IP(%s)\n", sk_bcast.rcv_size, sk->IP);
			///memset(sk->IP, 0, sizeof(sk->IP));
			strncpy(sk->IP, inet_ntoa(sk_bcast.AnsAddr.sin_addr), sizeof(sk->IP));
			printf("sk_size %u inet_ntoa(%s)\n", sk_size, sk->IP);
			//break;
		//	fprintf(stderr, "'%s': recvfrom() failed!\n", __func__);
		//	return -1;
		//}
		//sleep(1);
	//}
	close(sk_bcast.s_in);
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	return 0;
}

int client_build_msg(PC_stat * cl_stat)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	if (0 != CPU_info(&cl_stat->cpu_stat))
	{
		fprintf(stderr, "'%s': ProcessList() failed!\n", __func__);
		return -1;
	}
	printf("%s\n", __func__);
	if (0 != Mem_Info(&cl_stat->mem_stat)) 
	{
		fprintf(stderr, "'%s': Mem_Info() failed!\n", __func__);
		return -1;
	}
	printf("%s\n", __func__);
	if (0 != Uuid(cl_stat->client_id))
	{
		fprintf(stderr,"'%s': Uuid() failed!\n", __func__);
		return -1;
	}
	//printf("%s\n", __func__);
	/*if (0!=GetIP(cl_stat->IP))
    {
		fprintf(stderr,"'%s': IP() failed!\n", __func__);
		return -1;
	}
	printf("%s\n", __func__);*/
    if (0!=MAC_address(cl_stat->mac))
            {
		fprintf(stderr,"'%s': Mac() failed!\n", __func__);
		return -1;
	}
	printf("%s\n", __func__);
	if (0 != Load(&cl_stat->avginfo))
	{
		fprintf(stderr,"'%s': Load() failed!\n", __func__);
		return -1;
	}
	printf("%s\n", __func__);
	if (0!=Time(&cl_stat->time1))
            {
		fprintf(stderr,"'%s': Time() failed!\n", __func__);
		return -1;
	}
	return 0;
}

int client_handle_msg(sk_t * sk, PC_stat * cl_stat, server_ans * ans)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	if ( sizeof(PC_stat) != sendto(sk->s_in,
	                               (void*)cl_stat,
	                               sizeof(PC_stat),
	                               0,
	                               (struct sockaddr *) &sk->RemoteAddr,
	                               sizeof(sk->RemoteAddr)))
	{
		//fprintf(stderr, "'%s': sendto() failed!\n", __func__);
		printf("%s 1: ", __func__); perror("sendto() failed");
		return -1;
	}

	unsigned int sk_size = sizeof(sk->AnsAddr);
	if ( sizeof(server_ans) !=  (recvfrom(sk->s_in,
	                                      ((void*)ans),
	                                      sizeof(server_ans),
	                                      0,
	                                      (struct sockaddr *) &sk->AnsAddr,
	                                      &sk_size)))
	{
		fprintf(stderr, "'%s': recvfrom() failed!\n", __func__);
		return -1;
	}
	/*DOTO проверить отправителя и сообщение*/
	/*if (sk->AnsAddr.sin_addr.s_addr != inet_addr(sk->IP))
	{
		fprintf(stderr,"'%s': received a packet from unknown source.\n", __func__);
		return -1;
	}*/

	return 0;
}

int main(int argc, char *argv[])
{	
	pid_t pid;
	pid = getpid ();
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
	
	printf ("PID: %d\n", pid);

	printf("%s init vars %s\n", "client", (errno ? strerror(errno) : "ok"));

	SK_INIT(sk, htonl(INADDR_ANY), inet_addr(sk.IP), CLIENT_PORT, SERVER_PORT);

	printf("%s init vars %s\n", "client", (errno ? strerror(errno) : "ok"));

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
	getchar ();
	close(sk.s_in);

	return (0);
}
