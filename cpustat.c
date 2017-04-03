#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#ifndef PROCDIR
#define PROCDIR "/proc"
#endif
#ifndef PROCSTATFILE
#define PROCSTATFILE PROCDIR "/stat"
#endif
#ifndef PROCMEMINFOFILE
#define PROCMEMINFOFILE PROCDIR "/meminfo"
#endif
#define INTERVAL 250000
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


int ProcessList_scan(double * val) {
	unsigned long long int PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, totald, idled;
	double CPU_Percentage; 
	FILE * file = fopen(PROCSTATFILE, "r");
   if (file == NULL) {
      perror("Cannot open " PROCSTATFILE);
   }

   fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &prev.user, &prev.nice, &prev.system, &prev.idle, &prev.iowait, &prev.irq, &prev.softirq, &prev.steal);
    printf("cpu %llu %llu %llu %llu %llu %llu %llu %llu\n", 
    	prev.user, 
    	prev.nice, 
    	prev.system, 
    	prev.idle, 
    	prev.iowait, 
    	prev.irq, 
    	prev.softirq,
    	prev.steal);
    fclose(file); 
    usleep(INTERVAL);
    file = fopen(PROCSTATFILE, "r");
   if (file == NULL) {
       perror("Cannot open " PROCSTATFILE);
   }

   fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", 
   	&this.user,
    &this.nice,
     &this.system, 
     &this.idle, 
     &this.iowait, 
     &this.irq, 
     &this.softirq, 
     &this.steal);
   printf("cpu %llu %llu %llu %llu %llu %llu %llu %llu\n", 
   		this.user,
    	this.nice,
    	this.system, 
    	this.idle, 
    	this.iowait, 
    	this.irq, 
    	this.softirq, 
    	this.steal);
    fclose(file);
    puts("heloo");
   PrevIdle=prev.idle+prev.iowait;
   Idle=this.idle+this.iowait;

   PrevNonIdle = prev.user +
   				 prev.nice +
   				 prev.system +
   				 prev.irq +
   				 prev.softirq +
   				 prev.steal;
   NonIdle     = this.user +
   				 this.nice +
   				 this.system +
   				 this.irq +
   				 this.softirq +
   				 this.steal;

   PrevTotal = PrevIdle + PrevNonIdle;
   Total= Idle + NonIdle;
   totald= Total - PrevTotal;
   idled=Idle-PrevIdle;
   
   printf("%llu\n", totald);
   printf("%llu\n", totald - idled);
   *val = (100.0 * (totald - idled))/totald;
   printf("%f\n", *val);
   return (0);
}
	

int main(int argc, char *argv[]) {
	double cpu;
 ProcessList_scan(&cpu);
 printf("%lf\n", cpu);
 
 return (0);
}
 	



