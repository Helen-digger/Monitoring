#include "mac_addr.h"

int MAC_address(char * mac)
{   
    printf("%s\n", __func__);
    struct ifreq1 ifr1;
    struct ifconf ifc;
    char buf[32];
    int success = 0;
    struct sockaddr_in *addr;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { /* handle error*/ };
    printf("%s1\n", __func__);

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */ }
    printf("%s2\n", __func__);

    struct ifreq1* it = ifc.ifc_req;
    const struct ifreq1* const end = it + (ifc.ifc_len / sizeof(struct ifreq1));

    for (; it != end; ++it) {
        strcpy(ifr1.ifr_name, "wlp7s0");
        strcpy(ifr1.ifr_name, it->ifr_name);
        printf("%s3\n", __func__);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr1) == 0) {
            printf("%s4\n", __func__);
            //if (! (ifr1.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr1) == 0) {
                    printf("%s5\n", __func__);
                    success = 1;
                    break;
               // }
            }
        }
        else { /* handle error */ }
    }
printf("%s6\n", __func__);
    unsigned char mac_address[6];
    printf("%s\n", mac_address);

    if (success) memcpy(mac_address, ifr1.ifr_hwaddr.sa_data, 6);
    return 0;
}
