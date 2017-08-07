#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#ifndef PROCDIR
#define PROCDIR "/proc"
#endif
#ifndef PROCMEMINFOFILE
#define PROCMEMINFOFILE PROCDIR "/meminfo"
#endif
#define N 8

typedef struct
{
	unsigned long int MemTotal;
	unsigned long int MemFree;
	unsigned long int MemAvailable;
	unsigned long int Buffers;
	unsigned long int Cached;
	unsigned long int SwapCached;
	unsigned long int Active;
	unsigned long int Inactive;
} Memlist;

int Mem_Info(Memlist * meminf);
