#include <stdio.h>    //printf
#include <string.h>   //strncpy
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>   //ifreq
#include <unistd.h>   //close
#include <netinet/in.h>
#define LEN_MAC 18
#define LEN_IP  64

int MAC_address(char * mac, char * ifname);
