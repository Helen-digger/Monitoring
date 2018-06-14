#include "protocol.h"

int send_cast(sk_t sk)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	if (0 < (sk.snd_size = sendto(sk.s_in, 0, 0, 0,
		                          (struct sockaddr *) &sk.RemoteAddr,
		                          sizeof(sk.RemoteAddr))))
	{
		printf("%s sendto() failed %s\n", __func__, (errno ? strerror(errno) : "ok"));
		return -1;
	}
	return 0;
}

int handle_client(sk_t * sk, PC_stat * buffer, server_ans * ans)
{
	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/

	unsigned int cliAddrLen = sizeof(sk->AnsAddr);
	if (sizeof(PC_stat) != (sk->rcv_size = recvfrom(sk->s_in,
							                        (void*)buffer,
							                        sizeof(PC_stat),
							                        0,
							                        (struct sockaddr *) &sk->AnsAddr,
							                        &cliAddrLen)))
	{
		fprintf(stderr, "<%s> receive data from client failed, recvfrom rc(%d), errno = %d(%s)", __func__,
		        sk->rcv_size, errno, strerror(errno));
		return -1;
	}

	if (sk->rcv_size== sizeof(PC_stat))
	{
		ans->ans = SUCCESS;

		if (sizeof(server_ans) != (sk->snd_size = sendto(sk->s_in,
		                                                 (void*)ans,
		                                                 sizeof(server_ans),
		                                                 0,
		                                                 (struct sockaddr *) &sk->AnsAddr,
		                                                 sizeof(sk->AnsAddr))))
		{
			fprintf(stderr, "<%s> send answer to client failed, sendto rc(%d), errno = %d(%s)", __func__,
			        sk->snd_size, errno, strerror(errno));
			return -1;
		}
	}

	else
	{
		printf("'%s': server: recived wrong msg!!!\n", __func__);
		ans->ans = FAILED;

		if (sizeof(server_ans) != (sk->snd_size = sendto(sk->s_in,
		                                                 (void*)ans,
		                                                 sizeof(server_ans),
		                                                 0,
		                                                 (struct sockaddr *) &sk->AnsAddr,
		                                                 sizeof(sk->AnsAddr))))
		{
			fprintf(stderr, "<%s> send answer to client failed, sendto rc(%d), errno = %d(%s)", __func__,
			        sk->snd_size, errno, strerror(errno));
			return -1;
		}
	}

	return 0;
}

int PC_statToFile(PC_stat buffer, int flag)
{
	/*printf("%s fopen %s\n", __func__, (errno ? strerror(errno) : "ok"));*/
	if (0 == flag)
	{
	printf("\t%s\n", buffer.client_id);
	printf("\t%f\n", buffer.cpu_stat);
	printf("%32lu\n %32lu \n %32lu \n %32lu\n %32lu\n %32lu\n %32lu\n %32lu\n\n", 
			buffer.mem_stat.MemTotal, 
			buffer.mem_stat.MemFree, 
			buffer.mem_stat.MemAvailable,
			buffer.mem_stat.Buffers, 
			buffer.mem_stat.Cached,
			buffer.mem_stat.SwapCached, 
			buffer.mem_stat.Active,
			buffer.mem_stat.Inactive);
	printf("%s\n",buffer.mac);

	printf("cpu now :%f 5:%f 15:%f \n",
	       buffer.avginfo.avg_now,
	       buffer.avginfo.avg_five,
	       buffer.avginfo.avg_fifteen
	       );
	 printf("%s", asctime((const struct tm *)&buffer.time1));
	}
	else
	{
		char *filename=(char *)malloc(64);   
		filename=buffer.client_id;           
		FILE * file = fopen(filename, "a+"); 
		
		if (file == NULL) 
		{
			printf("%s fopen %s\n", __func__, (errno ? strerror(errno) : "ok"));
		}
		else 
		{
			fprintf(file,"%s\n", asctime((const struct tm *)&buffer.time1));
			fprintf(file,"UUID: %s\n", buffer.client_id);
			fprintf(file,"CPU_USAGE%f\n", buffer.cpu_stat);
			fprintf(file, "MemTotal %32lu\nMemFree %32lu\nMemAvailable %32lu\nBuffers %32lu\nCached %32lu\nSwapCached %32lu\nActive %32lu\nInactive %32lu\n\n", 
					buffer.mem_stat.MemTotal, 
					buffer.mem_stat.MemFree, 
					buffer.mem_stat.MemAvailable,
					buffer.mem_stat.Buffers, 
					buffer.mem_stat.Cached,
					buffer.mem_stat.SwapCached, 
					buffer.mem_stat.Active,
					buffer.mem_stat.Inactive);
			fprintf(file,"cpu now :%f 5:%f 15:%f \n",
					buffer.avginfo.avg_now,
					buffer.avginfo.avg_five,
					buffer.avginfo.avg_fifteen);
			fprintf(file,"MAC %s\n",buffer.mac);
			//fprintf(file,"IP %s\n", buffer.IP);
			fclose(file);
		}
	}
	return 0;
}

static int srv_init(srv_t * this, int argc, const char * arg)
{
	if (NULL == this) return -1;

	this->rl_cast = SRV_CAST_RL;

#if defined(MCAST)
	if (0) if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [IP_BCAST]\n", arg);
		return -1;
	}
	SK_INIT_MCAST_SRV(this->sk_cast, MCAST_ADDR);
#elif defined(BCAST)
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [IP_BCAST]\n", arg);
		return -1;
	}
	SK_INIT_BCAST(this->sk_cast, arg);
#endif

	memset(&this->buffer,  0, sizeof(this->buffer));
	memset(&this->ans_all, 0, sizeof(this->ans_all));
	SK_INIT(this->sk, htonl(INADDR_ANY), htonl(INADDR_ANY), SERVER_PORT, CLIENT_PORT);

	printf("%s init vars %s\n", "server", (errno ? strerror(errno) : "ok"));

	if (0 != Uuid(this->ans_all.server_id))
	{
		fprintf(stderr,"'%s': Uuid() failed!\n", __func__);
		return -1;
	}

	this->poll_fd[0].fd      = this->sk.s_in;
	this->poll_fd[0].events  = POLLIN;
	this->poll_fd[0].revents = 0;
	this->poll_rc = 0;

	return 0;
}

static void srv_deinit(srv_t * this)
{
	if (NULL == this) return;

	if (0 <= this->sk.s_in)
		close(this->sk.s_in);
	if (0 <= this->sk_cast.s_in)
		close(this->sk_cast.s_in);

	return;
}

static int srv_main_loop(srv_t * this)
{
	if (NULL == this) return -1;

	for (;;) /* Run forever */
	{
		if (tbf_rl(&this->rl_cast)) send_cast(this->sk_cast);

		this->poll_rc = poll(this->poll_fd, 1, SRV_LISTEN_T);
		switch (this->poll_rc)
		{
			case 0:
			{
				break;
			}
			case 1:
			{
				if (0 != handle_client(&this->sk, &this->buffer, &this->ans_all))
				{
					fprintf(stderr, "'%s': handle_client() failed!\n", __func__);
					return -1;
				}
				//PC_statToFile(buffer, 0);
				PC_statToFile(this->buffer, 1);
				break;
			}
			default:
			{
				fprintf(stderr, "<%s> poll failed rc(%d), errno = %d(%s)", __func__, this->poll_rc, errno, strerror(errno));
				return -1;
			}
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	static srv_t srv_data;

	if (0 != srv_init(&srv_data, argc, argv[0]))
	{
		fprintf(stderr, "'%s':srv_init failed!\n", __func__);
		return -1;
	}

	if (0 != srv_main_loop(&srv_data))
	{
		fprintf(stderr, "'%s':srv_main_loop failed!\n", __func__);
		return -1;
	}

	srv_deinit(&srv_data);

	return 0;
}
