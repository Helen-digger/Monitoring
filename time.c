#include "time.h"

int Time( tm *ptr)
{
time_t It;
It = time(NULL);
ptr = localtime(&It);
printf(asctime(ptr));
return 0;
}
