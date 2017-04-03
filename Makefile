cpustat:cpustat.o
	gcc -std=c99 cpustat.o -o cpustat
cpustat.o:cpustat.c
	gcc -std=c99 -c cpustat.c
clean:	
	rm -f cpustat cpustat.o