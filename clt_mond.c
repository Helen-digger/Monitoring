#include "protocol.h"

int get_srv_ip(sk_t * sk)
{
	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/
	sk_t sk_cast;
#if defined(MCAST)
	SK_INIT_MCAST_CLT(sk_cast, htonl(INADDR_ANY), htonl(INADDR_ANY), MCAST_PORT, 0);
#elif defined(BCAST)
	SK_INIT(sk_cast, htonl(INADDR_ANY), htonl(INADDR_ANY), BCAST_PORT, 0);
#endif

	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/
	unsigned int sk_size = sizeof(sk_cast.AnsAddr);
	sk_cast.rcv_size = recvfrom(sk_cast.s_in,
	                                      0,
	                                      0,
	                                      0,
	                                      (struct sockaddr *) &sk_cast.AnsAddr,
	                                      &sk_size);

	strncpy(sk->IP, inet_ntoa(sk_cast.AnsAddr.sin_addr), sizeof(sk->IP));
	/*printf("sk_size %u inet_ntoa(%s)\n", sk_size, sk->IP);*/

	close(sk_cast.s_in);
	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/

	usleep(rand()%ANS_TIMEOUT);

	return 0;
}

int client_build_msg(clt_t * this)
{
	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/
	if (0 != CPU_info(&this->client_status.cpu_stat))
	{
		fprintf(stderr, "'%s': ProcessList() failed!\n", __func__);
		return -1;
	}
	/*printf("%s\n", __func__);*/
	if (0 != Mem_Info(&this->client_status.mem_stat)) 
	{
		fprintf(stderr, "'%s': Mem_Info() failed!\n", __func__);
		return -1;
	}
	/*printf("%s\n", __func__);*/
	if (0 != Uuid(this->client_status.client_id))
	{
		fprintf(stderr,"'%s': Uuid() failed!\n", __func__);
		return -1;
	}
	/*printf("%s\n", __func__);*/
	/*if (0!=GetIP(this->client_status.IP))
	{
		fprintf(stderr,"'%s': IP() failed!\n", __func__);
		return -1;
	}*/
	/*printf("%s\n", __func__);*/
	if (0!=MAC_address(this->client_status.mac, this->iface))
	{
		fprintf(stderr,"'%s': Mac() failed!\n", __func__);
		return -1;
	}
	/*printf("%s\n", __func__);*/
	if (0 != Load(&this->client_status.avginfo))
	{
		fprintf(stderr,"'%s': Load() failed!\n", __func__);
		return -1;
	}
	/*printf("%s\n", __func__);*/
	if (0 != Time(&this->client_status.time1))
	{
		fprintf(stderr,"'%s': Time() failed!\n", __func__);
		return -1;
	}
	return 0;
}

int client_handle_msg(clt_t * this)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	int rc = 0;
	if ( sizeof(PC_stat) != (rc = sendto(this->sk.s_in,
	                                     (void*)&this->client_status,
	                                     sizeof(this->client_status),
	                                     0,
	                                     (struct sockaddr *) &this->sk.RemoteAddr,
	                                     sizeof(this->sk.RemoteAddr))))
	{
		fprintf(stderr, "<%s> send data to server failed, sendto rc(%d), errno = %d(%s)", __func__,
		                rc, errno, strerror(errno));
		return -1;
	}

	this->poll_rc = poll(this->poll_fd, 1, ANS_TIMEOUT);

	switch (this->poll_rc)
	{
		case 0:
		{
			this->missed_ans++;
			fprintf(stdout, "'%s': timeout!\n", __func__);
			if (MISED_ANS_COUNT_MAX <= this->missed_ans)
			{
				if (0 != get_srv_ip(&this->sk))
				{
					fprintf(stderr, "'%s': get_srv_ip() failed!\n", __func__);
					return -1;
				}
			}

			return 0;
		}
		case 1:
		{
			unsigned int sk_size = sizeof(this->sk.AnsAddr);
			if ( sizeof(server_ans) !=  (rc = recvfrom(this->sk.s_in,
			                                      ((void*)&this->ans_me),
			                                      sizeof(this->ans_me),
			                                      0,
			                                      (struct sockaddr *) &this->sk.AnsAddr,
			                                      &sk_size)))
			{
				fprintf(stderr, "<%s> send data to server failed, sendto rc(%d), errno = %d(%s)",
				                __func__, rc, errno, strerror(errno));
				return -1;
			}
			this->missed_ans = 0;

			return 0;
		}
		default:
		{
			fprintf(stderr, "<%s> poll failed rc(%d), errno = %d(%s)", __func__,
			                 this->poll_rc, errno, strerror(errno));
			return -1;
		}
	}

	return -1;
}

static int clt_init(clt_t * this, int argc, const char * args[])
{
	if (NULL == this) return -1;

	srand(time(NULL));

	this->pid = getpid ();
	this->snd_rl = CLT_SEND_RL;
	this->missed_ans = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [IFNAME]\n", args[0]);
		return -1;
	}
	strncpy(this->iface, args[1], sizeof(this->iface));

	memset(&this->client_status, 0, sizeof(this->client_status));
	memset(&this->ans_me, 0, sizeof(this->ans_me));

	if (0 != get_srv_ip(&this->sk))
	{
		fprintf(stderr, "'%s': get_srv_ip() failed!\n", __func__);
		return -1;
	}

	/*printf ("PID: %d\n", this->pid);*/

	/*printf("%s init vars %s\n", __func__, (errno ? strerror(errno) : "ok"));*/

	SK_INIT(this->sk, htonl(INADDR_ANY), inet_addr(this->sk.IP), CLIENT_PORT, SERVER_PORT);

	/*printf("%s init vars %s\n", __func__, (errno ? strerror(errno) : "ok"));*/

	this->poll_fd[0].fd      = this->sk.s_in;
	this->poll_fd[0].events  = POLLIN;
	this->poll_fd[0].revents = 0;
	this->poll_rc = 0;

	return 0;
}

static void clt_deinit(clt_t * this)
{
	if (NULL == this) return;

	if (0 <= this->sk.s_in)
		close(this->sk.s_in);

	return;
}

static int clt_main_loop(clt_t * this)
{
	if (NULL == this) return -1;

	for(;;)
	{
		if (!tbf_rl(&this->snd_rl)) continue;

		if (0 != client_build_msg(this))
		{
			fprintf(stderr, "'%s': client_build_msg() failed!\n", __func__);
			return -1;
		}

		if (0 != client_handle_msg(this))
		{
			fprintf(stderr, "'%s': client_handle_msg() failed!\n", __func__);
			return -1;
		}
	}

	return 0;
}

int main(int argc, const char * argv[])
{
	static clt_t clt_data;

	if (0 != clt_init(&clt_data, argc, argv))
	{
		fprintf(stderr, "'%s':clt_init failed!\n", __func__);
		return -1;
	}

	if (0 != clt_main_loop(&clt_data))
	{
		fprintf(stderr, "'%s':clt_main_loop failed!\n", __func__);
		return -1;
	}

	clt_deinit(&clt_data);

	return (0);
}
