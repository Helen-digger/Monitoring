#include "get_addr.h"
#include "id_client.h"
#include <errno.h>

int GetIP(char * address, char * name_interface)
{
	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/
	int sock_fd = -1;
	int rc = 0;
	struct sockaddr_in *addr;
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));

	scanf("%s", name_interface);
	if (0 > (sock_fd = socket( AF_INET , SOCK_DGRAM , 0 )))
	{
		fprintf(stderr, "<%s> open control socket failed socket rc(%d), errno = %d(%s)", __func__,
		                 sock_fd, errno, strerror(errno));
		return -1;
	}

	strncpy( ifr.ifr_name , name_interface, IFNAMSIZ);

	if (0 > (rc = ioctl( sock_fd , SIOCGIFADDR , &ifr )))
	{
		fprintf(stderr, "<%s> get IP addr for iface '%s' failed fscanf rc(%d), errno = %d(%s)", __func__,
		                 name_interface, rc, errno, strerror(errno));
		return -1;
	}

	addr = (struct sockaddr_in *)&(ifr.ifr_addr);

	strncpy( address , inet_ntoa(addr->sin_addr), LEN_IP);

	if (0 > sock_fd)
		close(sock_fd);

	return 0;
}

int MAC_address(char * mac, char * ifname)
{
	int fd = -1;
	int rc = 0;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	memset(mac, 0, LEN_MAC);

	if (0 > (fd = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		fprintf(stderr, "<%s> open control socket failed socket rc(%d), errno = %d(%s)", __func__,
		                 fd, errno, strerror(errno));
		return -1;
	}

	strncpy( ifr.ifr_name , ifname, IFNAMSIZ);

	if (0 > (rc = ioctl(fd, SIOCGIFHWADDR, &ifr)))
	{
		fprintf(stderr, "<%s> get MAC addr for iface '%s' failed fscanf rc(%d), errno = %d(%s)", __func__,
		                 ifname, rc, errno, strerror(errno));
		return -1;
	}

	unsigned char hwa[6];
	memcpy(hwa, ifr.ifr_hwaddr.sa_data, 6);

	if (0 >= snprintf(mac, LEN_MAC,"%2X:%2X:%2X:%2X:%2X:%2X",
			 hwa[0], hwa[1], hwa[2], hwa[3], hwa[4], hwa[5]))
	{
		fprintf(stderr, "<%s> build MAC addr string for iface '%s' failed snprintf rc(%d), errno = %d(%s)", __func__,
		                 ifname, rc, errno, strerror(errno));
		return -1;
	}

	if (0 >= fd)
		close(fd);

	return 0;
}
