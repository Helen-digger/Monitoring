#include "mac_addr.h"

int MAC_address(char * mac)
{   
    
    int fd;
    struct ifreq1 ifr1;
    
    //char *iface = "wlp7s0";

    memset(&ifr1, 0, sizeof(ifr1));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy( ifr1.ifr_name , "wlp7s0" );
    //ifr1.ifr_addr.sa_family = AF_INET;
    //strncpy(ifr1.ifr_name , iface , IFNAMSIZ1-1);

        ioctl(fd, SIOCGIFHWADDR, &ifr1); 
        mac = (unsigned char *)ifr1.ifr_hwaddr.sa_data;
       // addr1 = (struct sockaddr_in *)&(ifr1.ifr_addr);
        //strcpy( mac , inet_ntoa(addr1->sin_addr) );
        //display mac address
        printf("Mac : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    

    close(fd);

    return 0;
}
