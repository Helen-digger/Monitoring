
CC= gcc -std=c99
PRF :=$(pwd)
CFLAGS   = -Wall -Wformat-security -Wextra -Werror -D_GNU_SOURCE 
SERVER_SRC = udpserver.c protocol.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c
SERVER_DST = udpserver
CLIENT_SRC = udpclient.c protocol.c uuid.c tbf.c cpustat.c memstat.c mac_addr.c time.c loadavg.c
CLIENT_DST = udpclient
#SOURC:=$( find $DIR -name '*.c')
all: clean mcast
mcast: clean
	$(CC) $(CFLAGS) -DMCAST=1 -o $(SERVER_DST) $(SERVER_SRC)
	$(CC) $(CFLAGS) -DMCAST=1 -o $(CLIENT_DST) $(CLIENT_SRC)
bcast: clean
	$(CC) $(CFLAGS) -DBCAST=1 -o $(SERVER_DST) $(SERVER_SRC)
	$(CC) $(CFLAGS) -DBCAST=1 -o $(CLIENT_DST) $(CLIENT_SRC)
nocast: clean
	$(CC) $(CFLAGS) -o $(SERVER_DST) $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_DST) $(CLIENT_SRC)
clean:
	rm -f udpclient udpserver  cpu *.o *.a *.so
