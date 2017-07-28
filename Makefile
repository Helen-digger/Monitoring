
CC= gcc -std=c99
PRF :=$(pwd)
CFLAGS   = -Wall
#SOURC:=$( find $DIR -name '*.c')
all: udpserver.o udpclient.o
	$(CC) -Wall -o udpserver udpserver.c DieWithError.c uuid.c tbf.c
	$(CC) -Wall -o udpclient udpclient.c DieWithError.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c
clean:
	rm -f udpclient udpserver  cpu *.o *.a *.so 



