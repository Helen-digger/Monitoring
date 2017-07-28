#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define IFNAMSIZ 16

struct ifmap 
{
	unsigned long   mem_start;
	unsigned long   mem_end;
	unsigned short  base_addr;
	unsigned char   irq;
	unsigned char   dma;
	unsigned char   port;
};

struct ifreq {
	char ifr_name[IFNAMSIZ]; /* Interface name */
	union
	{
		struct sockaddr  ifr_addr;
		struct sockaddr  ifr_dstaddr;
		struct sockaddr  ifr_broadaddr;
		struct sockaddr  ifr_netmask;
		struct sockaddr  ifr_hwaddr;
		short            ifr_flags;
		int              ifr_ifindex;
		int              ifr_metric;
		int              ifr_mtu;
		struct ifmap     ifr_map;
		char             ifr_slave[IFNAMSIZ];
		char             ifr_newname[IFNAMSIZ];
		char           * ifr_data;
	};
};


int GetIP(char * address)
{
	printf("%s\n", __func__);
	int sock_fd;
	struct sockaddr_in *addr;
	//char *address;
	struct ifreq ifr;
//	char iface[] = "wlp7s0";

	memset(&ifr, 0, sizeof(ifr));

	sock_fd = socket( AF_INET , SOCK_DGRAM , 0 );
	strcpy( ifr.ifr_name , "wlp7s0" );             //wlp7s0
	ioctl( sock_fd , SIOCGIFADDR , &ifr );
	addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	//address=inet_ntoa(addr->sin_addr);
	strcpy( address , inet_ntoa(addr->sin_addr) );
	//printf("inet addr:%s\n",address);
	close( sock_fd );

	return 0;
}
//MAC_address(char * m);
