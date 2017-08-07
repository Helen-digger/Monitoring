#include "protocol.h"

/* Select() params
	 * int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
	 * FD_SET(int fd, fd_set *set);
	 * FD_CLR(int fd, fd_set *set);
	 * FD_ISSET(int fd, fd_set *set);
	 * FD_ZERO(fd_set *set);
	*/

int send_bcast(sk_t sk)
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
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	unsigned int cliAddrLen = sizeof(sk->AnsAddr);
	if (sizeof(PC_stat) != (sk->rcv_size = recvfrom(sk->s_in,
							                        (void*)buffer,
							                        sizeof(PC_stat),
							                        0,
							                        (struct sockaddr *) &sk->AnsAddr,
							                        &cliAddrLen)))
	{
		fprintf(stderr, "'%s': recvfrom() failed!\n", __func__);
		return -1;
	}

	if (sk->rcv_size== sizeof(PC_stat))
	{
		ans->ans = SUCCESS;

		if (sizeof(server_ans) != sendto(sk->s_in,
		                                 (void*)ans,
		                                 sizeof(server_ans),
		                                 0,
		                                 (struct sockaddr *) &sk->AnsAddr,
		                                 sizeof(sk->AnsAddr)))
		{
			//fprintf(stderr, "'%s': sendto() failed!\n", __func__);
			printf("%s 1: ", __func__); perror("sendto() failed!:");
			return -1;
		}

	}

	else
	{
		printf("'%s': server: recived wrong msg!!!\n", __func__);

		ans->ans = FAILED;

		if (sizeof(server_ans) != sendto(sk->s_in,
			                             (void*)ans,
			                             sizeof(server_ans),
			                             0,
			                             (struct sockaddr *) &sk->AnsAddr,
			                             sizeof(sk->AnsAddr)))
		{
			//fprintf(stderr, "'%s': sendto() failed!\n", __func__);
			printf("%s 2: ", __func__); perror("sendto() failed!:");
			return -1;
		}
		printf("%s3\n", __func__);
	}
	return 0;
}

int PC_statToFile(PC_stat buffer, int flag)
{
	printf("%s fopen %s\n", __func__, (errno ? strerror(errno) : "ok"));
	if (0 == flag)
	{
	printf("\t%s\n", buffer.client_id);
	printf("\t%f\n", buffer.cpu_stat);
	printf("%32lu\n  %32lu \n %32lu \n  %32lu \n  %32lu \n  %32lu \n  %32lu \n %32lu \n\n", 
			buffer.mem_stat.MemTotal, 
			buffer.mem_stat.MemFree, 
			buffer.mem_stat.MemAvailable,
			buffer.mem_stat.Buffers, 
			buffer.mem_stat.Cached,
			buffer.mem_stat.SwapCached, 
			buffer.mem_stat.Active,
			buffer.mem_stat.Inactive);
	//printf("\t%s\n",buffer.mac);
	//printf("\t%s\n", buffer.IP );
	//buffer.time_str=(asctime(&buffer.time));
	printf("cpu now :%f 5:%f 15:%f \n",
	       buffer.avginfo.avg_now,
	       buffer.avginfo.avg_five,
	       buffer.avginfo.avg_fifteen
	       //avginfo->proccess
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
			fprintf(file,"%s\n", buffer.client_id);
			fprintf(file,"%f\n", buffer.cpu_stat);
			fprintf(file, "%32lu\n  %32lu \n %32lu \n  %32lu \n  %32lu \n  %32lu \n  %32lu \n %32lu \n\n", 
					buffer.mem_stat.MemTotal, 
					buffer.mem_stat.MemFree, 
					buffer.mem_stat.MemAvailable,
					buffer.mem_stat.Buffers, 
					buffer.mem_stat.Cached,
					buffer.mem_stat.SwapCached, 
					buffer.mem_stat.Active,
					buffer.mem_stat.Inactive);
			fclose(file);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	//if (list_ifaces()) return -1;
	sk_t       sk, sk_bcast;
	tbf        rl_bcast       = SRV_BCAST_RL;
	PC_stat    buffer;
	server_ans ans_all;
	int        error;

	memset(&buffer,  0, sizeof(PC_stat));
	memset(&ans_all, 0, sizeof(server_ans));
	SK_INIT(sk, htonl(INADDR_ANY), htonl(INADDR_ANY), SERVER_PORT, CLIENT_PORT);
	SK_INIT_BCAST(sk_bcast);

	printf("%s init vars %s\n", "server", (errno ? strerror(errno) : "ok"));

	if (0 != Uuid(ans_all.server_id))
	{
		fprintf(stderr,"'%s': Uuid() failed!\n", __func__);
		return -1;
	}

	for (;;) /* Run forever */
	{
		if (tbf_rl(&rl_bcast)) send_bcast(sk_bcast);
		if (isReadable(sk.s_in, &error, SRV_LISTEN_T))
		{
			if (0 != handle_client(&sk, &buffer, &ans_all))
			{
				fprintf(stderr, "'%s': handle_client() failed!\n", __func__);
				return -1;
			}
			else
			{
				PC_statToFile(buffer, 0);
				PC_statToFile(buffer, 1);
			}
		}
	}

	close(sk.s_in);
	close(sk_bcast.s_in);

	return 0;
}
