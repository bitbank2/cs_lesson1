CFLAGS=-c -ggdb -Wall -O0
LIBS= -lm

all: cs1

cs1: main.o
	$(CC) main.o $(LIBS) -g -o cs1

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *o cs1

