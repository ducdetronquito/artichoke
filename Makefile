PROGRAM=main
OBJECTS=
CFLAGS= -g -Wall -O3 
CFLAGS=-L/usr/lib -lmyhtml
CC=c99

test:
	$(CC) main.c artichoke.c $(CFLAGS) $(CFLAGS) -o test

clean:
	rm test
