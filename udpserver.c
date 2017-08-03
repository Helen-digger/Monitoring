#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include "protocol.h"

#define SOCKET_ERROR -1

/* Select() params
	 * int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
	 * FD_SET(int fd, fd_set *set);
	 * FD_CLR(int fd, fd_set *set);
	 * FD_ISSET(int fd, fd_set *set);
	 * FD_ZERO(fd_set *set);
	*/

int send_bcast(sk_t sk)
{
	printf("%s IP %s\n", __func__, sk.IP);
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	if (7 >= (sk.snd_size = sendto(sk.s_in,
		                           (void*)sk.IP,
		                           sizeof(sk.IP),
		                           0,
		                           (struct sockaddr *) &sk.RemoteAddr,
		                           sizeof(sk.RemoteAddr))))
	{
		printf("ip %s\n", sk.IP);
		fprintf(stderr, "'%s': sendto() failed!\n", __func__);
		perror("ERROR:");
		return -1;
	}
	//else {printf("%s %d\n", sk.IP, sk.snd_size); return 1;}
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	return 0;
}



int handle_client(sk_t * sk, PC_stat * buffer, server_ans * ans)
{
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
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

int isReadable(int sock, int * error, int timeOut) 
{
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	// milliseconds
	fd_set socketReadSet;
	FD_ZERO(&socketReadSet);
	FD_SET(sock, &socketReadSet);
	struct timeval tv;
	if (timeOut)
	{
		tv.tv_sec  = timeOut / 1000;
		tv.tv_usec = (timeOut % 1000) * 1000;
	}
	else
	{
		tv.tv_sec  = 0;
		tv.tv_usec = 0;
	}

	if (select(sock+1, &socketReadSet, 0, 0, &tv) == SOCKET_ERROR)
	{
		*error = 1;
		return 0;
	}

	*error = 0;
	return FD_ISSET(sock, &socketReadSet) != 0;
}

int PC_statToFile(PC_stat buffer, int flag)
{
	printf("%s fopen %s\n", __func__, errno ? strerror(errno) : "ok");
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
	printf("\t%s\n",buffer.mac);
	printf("\t%s\n", buffer.IP );
	//buffer.time_str=(asctime(&buffer.time));
	 printf("%s", asctime(&buffer.time1));
	}
	else
	{
		
		char *filename=(char *)malloc(64);   
		filename=buffer.client_id;           
		FILE * file = fopen(filename, "a+");
		if (file == NULL) 
		{
			printf("%s fopen %s\n", __func__, errno ? strerror(errno) : "ok");
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

	sk_t       sk, sk_bcast;
	tbf        rl_bcast       = {.rate = 1, .burst = 1};
	PC_stat    buffer;
	server_ans ans_all;
	int        error, timeOut;

	memset(&buffer,  0, sizeof(PC_stat));
	memset(&ans_all, 0, sizeof(server_ans));
	SK_INIT(sk, htonl(INADDR_ANY), htonl(INADDR_ANY), SERVER_PORT, CLIENT_PORT);
	SK_INIT_BCAST(sk_bcast);

	printf("%s init vars %s\n", "server", errno ? strerror(errno) : "ok");

	if (0 != Uuid(ans_all.server_id))
	{
		fprintf(stderr,"'%s': Uuid() failed!\n", __func__);
		return -1;
	}
	//char myip[IP4_STR_LEN];
	if (0 !=  GetIP(sk_bcast.IP))
	{
		fprintf(stderr,"'%s': IP() failed!\n", __func__);
		return -1;
	}
	timeOut = 3000; // ms
	for (;;) /* Run forever */
	{
		if (tbf_rl(&rl_bcast)) send_bcast(sk_bcast);
		timeOut = 3000; // ms
		if (isReadable(sk.s_in, &error, timeOut)) 
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
