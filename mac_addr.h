#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define IFNAMSIZ1 32
#define LEN_MAC 32

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
 struct ifconf {
                      int                 ifc_len; /* size of buffer */
                      union {
                          char           *ifc_buf; /* buffer address */
                          struct ifreq1   *ifc_req; /* array of structures */
                      };
                };

int MAC_address(char * mac);

