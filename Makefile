
CC= gcc -std=c99
PRF :=$(pwd)
CFLAGS   = -Wall -Wformat-security -Wextra -Werror -D_XOPEN_SOURCE=500
SERVER_SRC = udpserver.c protocol.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c
SERVER_DST = udpserver
CLIENT_SRC = udpclient.c protocol.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c loadavg.c
CLIENT_DST = udpclient
#SOURC:=$( find $DIR -name '*.c')
all: clean udpserver.o udpclient.o
	$(CC) $(CFLAGS) -o $(SERVER_DST) $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_DST) $(CLIENT_SRC)
clean:
	rm -f udpclient udpserver  cpu *.o *.a *.so
