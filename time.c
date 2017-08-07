#include "time.h"

int Time(struct tm *time1)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	time_t It;
	It = time(NULL);
	localtime_r((const time_t *)&It, time1);
	return 0;
}
