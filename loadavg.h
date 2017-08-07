#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define INTERVAL 250000
#ifndef PROCDIR
#define PROCDIR "/proc"
#endif
#ifndef PROCLOADFILE
#define PROCLOADFILE PROCDIR "/loadavg"
#endif


typedef struct
{
	double avg_now;
	float avg_five;
	float avg_fifteen;
	float proccess;
	int proc_pid;
} AVGList;

int Load(AVGList * avginfo);

