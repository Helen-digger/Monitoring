#include "cpustat.h"
int CPU_info(double * val)
{
	printf("%s\n", __func__);
	unsigned long long int PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, totald, idled;
	FILE * file = fopen(PROCSTATFILE, "r");
	if (file == NULL) 
	{
		perror("Cannot open " PROCSTATFILE);
		return -1;
	}

	fscanf(file,
	       "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
	       &prev.user,
	       &prev.nice,
	       &prev.system,
	       &prev.idle,
	       &prev.iowait,
	       &prev.irq,
	       &prev.softirq,
	       &prev.steal);

	fclose(file); 
	usleep(INTERVAL);
	file = fopen(PROCSTATFILE, "r");
	if (file == NULL) 
	{
		perror("Cannot open " PROCSTATFILE);
		return -1;
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

	fclose(file);

	PrevIdle=prev.idle+prev.iowait;
	Idle=this.idle+this.iowait;

	PrevNonIdle = prev.user +
	              prev.nice +
	              prev.system +
	              prev.irq +
	              prev.softirq +
	              prev.steal;

	NonIdle	 = this.user +
	           this.nice +
	           this.system +
	           this.irq +
	           this.softirq +
	           this.steal;

	PrevTotal = PrevIdle + PrevNonIdle;
	Total= Idle + NonIdle;
	totald= Total - PrevTotal;
	idled=Idle-PrevIdle;

	*val = (100.0 * (totald - idled))/totald;

	return 0;
}
