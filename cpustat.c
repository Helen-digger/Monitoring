


#include "cpustat.h"



int main(int argc, char *argv[]) {
    double cpu;
    ProcessList_scan(&cpu);
    printf("%lf\n", cpu);
 
 return (0);
}

	



