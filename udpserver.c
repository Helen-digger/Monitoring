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
	printf("%s\n", __func__);
	if (7 >= (sk.snd_size = sendto(sk.sock,
					                           (void*)sk.IP,
					                           sizeof(sk.IP),
					                           0,
					                           (struct sockaddr *) &sk.SrvAddr,
					                           sizeof(sk.SrvAddr))))
	{
		printf("ip %s\n", sk.IP);
		fprintf(stderr, "'%s': sendto() failed!\n", __func__);
		perror("ERROR:");
		return -1;
	}
	//else {printf("%s %d\n", sk.IP, sk.snd_size); return 1;}
	return 0;
}



int handle_client(sk_t * sk, PC_stat * buffer, server_ans * ans)
{
	printf("%s\n", __func__);
	unsigned int cliAddrLen = sizeof(sk->ClntAddr);
	if (sizeof(PC_stat) != (sk->rcv_size = recvfrom(sk->sock,
			                        (void*)buffer,
			                        sizeof(PC_stat),
			                        0,
			                        (struct sockaddr *) &sk->ClntAddr,
			                        &cliAddrLen)))
	{
		DieWithError("recvfrom() failed");
	}
	printf("%s1\n", __func__);
	if (sk->rcv_size== sizeof(PC_stat))
	{	
		ans->ans = SUCCESS;

		if (sizeof(server_ans) != sendto(sk->sock,
		                                 (void*)ans,
		                                 sizeof(server_ans),
		                                 0,
		                                 (struct sockaddr *) &sk->ClntAddr,
		                                 sizeof(sk->ClntAddr)))
		{
			fprintf(stderr, "'%s': sendto() failed!\n", __func__);
			return -1;
		}
		printf("%s2\n", __func__);
	}

	else 
	{
		printf(" error: поврежденно сообщение " );

		ans->ans = FAILED;

		if (sizeof(server_ans) != sendto(sk->sock,
			                             (void*)ans,
			                             sizeof(server_ans),
			                             0,
			                             (struct sockaddr *) &sk->ClntAddr,
			                             sizeof(sk->ClntAddr)))
		{
			fprintf(stderr, "'%s': sendto() failed!\n", __func__);
			return -1;
		}
		printf("%s3\n", __func__);
	}
	return 0;
}

int isReadable(int sock, int * error, int timeOut) 
{
	printf("%s\n", __func__);
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

int main(int argc, char *argv[])
{
	sk_t       sk, sk_bcast;
	tbf        rl_bcast       = {.rate = 1, .burst = 1};
	PC_stat    buffer;
	server_ans ans_all;
	int        error, timeOut;

	memset(&buffer,  0, sizeof(PC_stat));
	memset(&ans_all, 0, sizeof(server_ans));
	SK_INIT(sk, PF_INET, SOCK_DGRAM, IPPROTO_UDP, AF_INET, htonl(INADDR_ANY), SERVER_PORT);
	SK_INIT_BCAST(sk_bcast, PF_INET, SOCK_DGRAM, IPPROTO_UDP, AF_INET, htonl(INADDR_ANY), BCAST_PORT);
	
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
	timeOut = 500; // ms
	for (;;) /* Run forever */
	{
		if (tbf_rl(&rl_bcast)) send_bcast(sk_bcast);

		//if (isReadable(sk.sock, &error, timeOut)) 
		{

			if (0 != handle_client(&sk, &buffer, &ans_all))
			{
				fprintf(stderr, "'%s': handle_client() failed!\n", __func__);
				return -1;
			}
			else
			{


					printf("\t%s\n", buffer.client_id);
            		printf("\t%f\n", buffer.cpu_stat);
              		printf( "%32lu\n  %32lu \n %32lu \n  %32lu \n  %32lu \n  %32lu \n  %32lu \n %32lu \n\n", 
                 		buffer.mem_stat.MemTotal, 
                 		buffer.mem_stat.MemFree, 
                		buffer.mem_stat.MemAvailable,
                 		buffer.mem_stat.Buffers, 
                 		buffer.mem_stat.Cached,
                 		buffer.mem_stat.SwapCached, 
                 		buffer.mem_stat.Active,
                 		buffer.mem_stat.Inactive);
			/*char *filename=(char *)malloc(64);
			filename=buffer.client_id;
    		FILE * file = fopen(filename, "a+");
   				if (file == NULL) 
   				{
      				perror("Cannot open 11");
  				}
  				else 
  				{
  					fwrite(&buffer, sizeof(buffer)+1, 1, file);

      				fprintf(file,"\t%s\n", buffer.client_id);
            		fprintf(file,"\t%f\n", buffer.cpu_stat);
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
  				}*/
  			} 
 
		}
	}

	close(sk.sock);
	close(sk_bcast.sock);

	return 0;
}
