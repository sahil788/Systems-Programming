CC=g++
CFLAGS=-I
CFLAGS+=-Wall
FILES=server.cpp
FILES1=client.cpp

server: .
	$(CC) $(CFLAGS) -o server $(FILES)

client: .
	$(CC) $(CFLAGS) -o client $(FILES1)

clean:
	rm -f *.o server client

all: server client