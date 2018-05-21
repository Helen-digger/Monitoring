#include <stdio.h>    //printf
#include <string.h>   //strncpy
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>   //ifreq
#include <unistd.h>   //close
#include <netinet/in.h>
//#define IFNAMSIZ 16
#define LEN_MAC 18


/*
struct ifmap1 
{
	unsigned long   mem_start;
	unsigned long   mem_end;
	unsigned short  base_addr;
	unsigned char   irq;
	unsigned char   dma;
	unsigned char   port;
};

struct ifreq1 {
	char ifr_name[IFNAMSIZ1]; 
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
		struct ifmap1     ifr_map;
		char             ifr_slave[IFNAMSIZ1];
		char             ifr_newname[IFNAMSIZ1];
		char           * ifr_data;
	};
};
*/
int MAC_address(char * mac, char * ifname);

//int list_ifaces();
