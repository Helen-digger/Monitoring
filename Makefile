
CC= gcc -std=c99
PRF :=$(pwd)
CFLAGS   = -Wall
#SOURC:=$( find $DIR -name '*.c')
all: udpserver.o udpclient.o
	$(CC) -Wall -o udpserver udpserver.c protocol.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c
	$(CC) -Wall -o udpclient udpclient.c protocol.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c loadavg.c
clean:
	rm -f udpclient udpserver  cpu *.o *.a *.so 



