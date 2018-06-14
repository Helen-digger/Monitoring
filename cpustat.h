#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define INTERVAL 250000
#ifndef PROCDIR
#define PROCDIR "/proc"
#endif
#ifndef PROCSTATFILE
#define PROCSTATFILE PROCDIR "/stat"
#endif
#ifndef PROCMEMINFOFILE
#define PROCMEMINFOFILE PROCDIR "/meminfo"
#endif

typedef struct
{
	unsigned long long int user;
	unsigned long long int nice;
	unsigned long long int system;
	unsigned long long int idle;
	unsigned long long int iowait;
	unsigned long long int irq;
	unsigned long long int softirq;
	unsigned long long int steal;
} ProcessList;

ProcessList this;
ProcessList prev;

struct tm *localtime_r(const time_t *timep, struct tm *result);

int CPU_info(double * val);
