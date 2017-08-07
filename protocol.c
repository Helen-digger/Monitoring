#include "protocol.h"

int isReadable(int sock, int * error, int timeOut)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	// milliseconds
	fd_set socketReadSet;
	FD_ZERO(&socketReadSet);
	FD_SET(sock, &socketReadSet);
	struct timeval tv;
	if (timeOut)
	{
		tv.tv_sec  = timeOut / 1000;
		tv.tv_usec = (timeOut % 1000) * 1000;
	}
	else
	{
		tv.tv_sec  = 0;
		tv.tv_usec = 0;
	}

	if (select(sock+1, &socketReadSet, 0, 0, &tv) == SOCKET_ERROR)
	{
		*error = 1;
		return 0;
	}

	*error = 0;
	return FD_ISSET(sock, &socketReadSet) != 0;
}
