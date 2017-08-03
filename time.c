#include "time.h"

int Time( tm *time1)
{
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	time_t It;
	It = time(NULL);
	localtime_r(&It, time1);
	return 0;
}
