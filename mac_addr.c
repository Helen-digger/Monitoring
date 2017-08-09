#include "mac_addr.h"
#include "id_client.h"
#include <errno.h>

/*int GetIP(char * address)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	int sock_fd;
	struct sockaddr_in *addr;
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	printf("Введите активный иннтерфейс\n");
	char name_interface[IFNAMSIZ];
	 scanf("%s", name_interface);
	sock_fd = socket( AF_INET , SOCK_DGRAM , 0 );
	strcpy( ifr.ifr_name , name_interface);			 //enp8s0
	ioctl( sock_fd , SIOCGIFADDR , &ifr );
	addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	//address=inet_ntoa(addr->sin_addr);
	strcpy( address , inet_ntoa(addr->sin_addr) );
	close( sock_fd );

	return 0;
}*/

int MAC_address(char * mac, char * ifname)
{
int fd;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	memset(mac, 0, LEN_MAC);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy( ifr.ifr_name , ifname, IFNAMSIZ);

		ioctl(fd, SIOCGIFHWADDR, &ifr); 
		//mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		// addr1 = (struct sockaddr_in *)&(ifr.ifr_addr);
		//strcpy( mac , inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_hwaddr))->sin_addr) );
		//display mac address
		unsigned char hwa[6];
		memcpy(hwa, ifr.ifr_hwaddr.sa_data, 6);
		snprintf(mac, LEN_MAC,"%2X:%2X:%2X:%2X:%2X:%2X" ,
				 hwa[0], hwa[1], hwa[2], hwa[3], hwa[4], hwa[5]);

	close(fd);
	return 0;
}

/*int list_ifaces()
{
	char		  buf[1024];
	struct ifconf ifc;
	struct ifreq *ifr;
	struct ifreq  ifr_buf;
	int           sck;
	int           nInterfaces;
	int           i;

/* Get a socket handle. 
	if(0 > (sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)))
	{
		perror("socket");
		return 1;
	}

/* Query available interfaces. 
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(0 > ioctl(sck, SIOCGIFCONF, &ifc))
	{
		perror("ioctl(SIOCGIFCONF)");
		return 1;
	}
	close(sck);
/*
	IFF_UP            Interface is running.
	IFF_BROADCAST     Valid broadcast address set.
	IFF_DEBUG         Internal debugging flag.
	IFF_LOOPBACK      Interface is a loopback interface.
	IFF_POINTOPOINT   Interface is a point-to-point link.
	IFF_RUNNING       Resources allocated.
	IFF_NOARP         No arp protocol, L2 destination address not set.
	IFF_PROMISC       Interface is in promiscuous mode.
	IFF_NOTRAILERS    Avoid use of trailers.
	IFF_ALLMULTI      Receive all multicast packets.
	IFF_MASTER        Master of a load balancing bundle.
	IFF_SLAVE         Slave of a load balancing bundle.
	IFF_MULTICAST     Supports multicast
	IFF_PORTSEL       Is able to select media type via ifmap.
	IFF_AUTOMEDIA     Auto media selection active.

	IFF_DYNAMIC       The addresses are lost when the interface goes down.
	IFF_LOWER_UP      Driver signals L1 up (since Linux 2.6.17)
	IFF_DORMANT       Driver signals dormant (since Linux 2.6.17)
	IFF_ECHO          Echo sent packets (since Linux 2.6.25)

	IFF_802_1Q_VLAN      Interface is 802.1Q VLAN device.
	IFF_EBRIDGE          Interface is Ethernet bridging device.
	IFF_SLAVE_INACTIVE   Interface is inactive bonding slave.
	IFF_MASTER_8023AD    Interface is 802.3ad bonding master.
	IFF_MASTER_ALB       Interface is balanced-alb bonding master.
	IFF_BONDING          Interface is a bonding master or slave.
	IFF_SLAVE_NEEDARP    Interface needs ARPs for validation.
	IFF_ISATAP           Interface is RFC4214 ISATAP interface.
*/
/* Iterate through the list of interfaces. 
	if(0 > (sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)))
	{
		perror("socket");
		return 1;
	}
	ifr		 = ifc.ifc_req;
	nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
	for(i = 0; i < nInterfaces; i++)
	{
		struct ifreq *item = &ifr[i];
		memcpy(&ifr_buf, item, sizeof(struct ifreq));

		if(0 > ioctl(sck, SIOCGIFPFLAGS, item))
		{
			perror("ioctl(SIOCGIFPFLAGS)");
			return 1;
		}
		printf("%10s %10X  %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
		item->ifr_name,
		item->ifr_flags,
			(item->ifr_flags & IFF_UP),
			(item->ifr_flags & IFF_BROADCAST),
			(item->ifr_flags & IFF_DEBUG),
			(item->ifr_flags & IFF_LOOPBACK),
			(item->ifr_flags & IFF_POINTOPOINT),

			(item->ifr_flags & IFF_RUNNING),
			(item->ifr_flags & IFF_NOARP),
			(item->ifr_flags & IFF_PROMISC),
			(item->ifr_flags & IFF_NOTRAILERS),
			(item->ifr_flags & IFF_ALLMULTI),

			(item->ifr_flags & IFF_MASTER),
			(item->ifr_flags & IFF_SLAVE),
			(item->ifr_flags & IFF_MULTICAST),
			(item->ifr_flags & IFF_PORTSEL),
			(item->ifr_flags & IFF_AUTOMEDIA),

			(item->ifr_flags & IFF_DYNAMIC),
			(item->ifr_flags & IFF_LOWER_UP),
			(item->ifr_flags & IFF_DORMANT),
			(item->ifr_flags & IFF_ECHO));


		if (! (item->ifr_flags & IFF_LOOPBACK))
			{
				printf("%s: IP %s",
			   item->ifr_name,
			   inet_ntoa(((struct sockaddr_in *)&item->ifr_addr)->sin_addr));
			}


		/* Get the broadcast address (added by Eric) 
		if(ioctl(sck, SIOCGIFBRDADDR, item) >= 0)
			printf(", BROADCAST %s", inet_ntoa(((struct sockaddr_in *)&item->ifr_broadaddr)->sin_addr));
		printf("\n");
	}
	close(sck);
	return 0;
} */