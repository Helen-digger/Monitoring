
CC= gcc -std=c99
PRF :=$(pwd)
CFLAGS   = -Wall
#SOURC:=$( find $DIR -name '*.c')
all: udpserver.o udpclient.o
	$(CC) -Wall -o udpserver udpserver.c DieWithError.c 
	$(CC) -Wall -o udpclient udpclient.c DieWithError.c uuid.c cpustat.c memstat.c
clean:
	rm -f udpclient udpserver  cpu *.o *.a *.so 



