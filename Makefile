all: server client

CFLAGS = -g
LDFLAGS = -lcrypto -lssl -ldb

server: config.o key.o store.o socket.o

client: client_api.o socket.o

config.o: config.c config.h
key.o: key.c key.h
store.o: store.c store.h
socket.o: socket.c socket.h
client_api.o: client_api.c client_api.h

clean:
	-rm *.o server client
