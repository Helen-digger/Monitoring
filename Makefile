
CC= gcc -std=c99
PRF :=$(pwd)
CFLAGS   = -Wall -Wformat-security -Wextra -Werror -D_GNU_SOURCE 
SERVER_SRC = srv_mond.c uuid.c tbf.c
SERVER_DST = srv_mond
CLIENT_SRC = clt_mond.c uuid.c tbf.c cpustat.c memstat.c get_addr.c time.c loadavg.c
CLIENT_DST = clt_mond

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
	rm -f clt_mond srv_mond  cpu *.o *.a *.so
