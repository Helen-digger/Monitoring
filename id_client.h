#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
//#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
//#include <linux/if.h>
#include <linux/if_tun.h>
#ifdef __linux__
#include <linux/if.h>
#else
#include <net/if.h>
#endif


/*struct ifmap 
{
	unsigned long   mem_start;
	unsigned long   mem_end;
	unsigned short  base_addr;
	unsigned char   irq;
	unsigned char   dma;
	unsigned char   port;
};

struct ifreq {
	char ifr_name[IFNAMSIZ]; // Interface name 
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
};*/

int GetIP(char * address);
