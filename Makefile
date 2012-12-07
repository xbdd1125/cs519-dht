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

test: server client
	@./server 1 > /dev/null &
	@./server 2 > /dev/null &
	@./server 3 > /dev/null &
	@./server 4 > /dev/null &
	@./client write localhost 4444 111111 hello
	@./client write localhost 4446 222222 world
	@./client write localhost 4444 333333 this
	@./client write localhost 4446 444444 is
	@./client write localhost 4444 555555 a
	@./client wirte localhost 4446 666666 test
	@./client read  localhost 4446 111111
	@./client read  localhost 4444 222222
	@./client kill  localhost 4446

clean:
	@-rm *.o server client > /dev/null 2>&1
