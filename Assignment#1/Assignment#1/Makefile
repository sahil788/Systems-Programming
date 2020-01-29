CC=g++
CFLAGS=-I
CFLAGS+=-Wall
FILES=netMonitor.cpp
FILES1+=intfMonitor.cpp

netMonitor: .
	$(CC) $(CFLAGS) -o netMonitor $(FILES)

intfMonitor: .
	$(CC) $(CFLAGS) -o intfMonitor $(FILES1)

clean:
	rm -f *.o netMonitor intfMonitor

all: netMonitor intfMonitor
