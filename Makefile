all: server client

CFLAGS = -g
LDFLAGS = -lcrypto -lssl -ldb

server: config.o key.o store.o socket.o

client: socket.o

config.o: config.c config.h
key.o: key.c key.h
store.o: store.c store.h
socket.o: socket.c socket.h

clean:
	-rm *.o server client
