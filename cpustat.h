#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
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

int ProcessList_scan(double * val);
