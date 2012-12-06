all: server

CFLAGS = -g
LDFLAGS = -lcrypto -lssl -ldb

server: config.o key.o store.o

config.o: config.c config.h
key.o: key.c key.h
store.o: store.c store.h

clean:
	-rm *.o server
