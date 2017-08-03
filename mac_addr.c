#include "mac_addr.h"
#include "id_client.h"
#include <errno.h>

/*int MAC_address(char * mac)
{
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	int fd, rc = 0;
	struct ifreq ifr1;

	memset(&ifr1, 0, sizeof(ifr1));

	if (0 > (fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)))
	{
		printf("%s: socket()", __func__); perror("");
		return -1;
	}

	if (0 > strncpy( ifr1.ifr_name , "wlp7s0" , 32))
	{
		printf("%s strncpy()\n", __func__); perror("");
		return -1;
	}

	ifr1.ifr_addr.sa_family = AF_INET;

	if (0 > (rc = ioctl(fd, SIOCGIFHWADDR, &ifr1)))
	{
		printf("%s ioctl() rc %d \n", __func__, rc); perror("");
		return -1;
	}

#if 0
	//mac = (unsigned char *)ifr1.ifr_hwaddr.sa_data;
	char umac[7]; memset(umac, 0, sizeof(umac));
	//strncpy(umac, inet_ntoa(((struct sockaddr_in *)&(ifr1.ifr_addr))->sin_addr), sizeof(umac));
	//snprintf(umac, 7, "%s",

	printf("lladdr %6s\n", inet_ntoa(((struct sockaddr_in *)&(ifr1.ifr_addr))->sin_addr));


	printf("Mac : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" ,
			umac[0], umac[1], umac[2], umac[3], umac[4], umac[5]);
#endif
	close(fd);
	return 0;
}
*/
int GetIP(char * address)
{
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	int sock_fd;
	struct sockaddr_in *addr;
	//char *address;
	struct ifreq ifr;
//	char iface[] = "wlp7s0";

	memset(&ifr, 0, sizeof(ifr));

	sock_fd = socket( AF_INET , SOCK_DGRAM , 0 );
	strcpy( ifr.ifr_name , "wlp7s0" );             //enp8s0
	ioctl( sock_fd , SIOCGIFADDR , &ifr );
	addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	//address=inet_ntoa(addr->sin_addr);
	strcpy( address , inet_ntoa(addr->sin_addr) );
	close( sock_fd );

	return 0;
}

int MAC_address(char * mac)
{   
    
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *addr1;
    //char *iface = "wlp7s0";

    memset(&ifr, 0, sizeof(ifr));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy( ifr.ifr_name , "wlp7s0" );
    //ifr.ifr_addr.sa_family = AF_INET;
    //strncpy(ifr.ifr_name , iface , IFNAMSIZ1-1);

        ioctl(fd, SIOCGIFHWADDR, &ifr); 
        //mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
       // addr1 = (struct sockaddr_in *)&(ifr.ifr_addr);
        //strcpy( mac , inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_hwaddr))->sin_addr) );
        //display mac address
        unsigned char hwa[6];
        memcpy(hwa, ifr.ifr_hwaddr.sa_data, 6);
        printf("%s Mac : %X:%X:%X:%X:%X:%X\n" ,
               __func__, hwa[0], hwa[1], hwa[2], hwa[3], hwa[4], hwa[5]);
        /*printf("Mac : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" ,
               ifr.ifr_hwaddr.sa_data[0], ifr.ifr_hwaddr.sa_data[1], ifr.ifr_hwaddr.sa_data[2],
               ifr.ifr_hwaddr.sa_data[3], ifr.ifr_hwaddr.sa_data[4], ifr.ifr_hwaddr.sa_data[5]);*/
    

    close(fd);

    return 0;
}
