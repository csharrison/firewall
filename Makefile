TEXEC = test
TSRC = src/test/*.c src/containers.c

CFLAGS= -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align
CFLAGS +=   -Winline -Wfloat-equal -Wnested-externs
CFLAGS +=   -pedantic -std=c99
CFLAGS +=   -D_GNU_SOURCE -g
CFLAGS += -iquotesrc/include -pthread

CC = /usr/bin/gcc

.PHONY: all test clean

test:
	$(CC) $(CFLAGS) -o $(TEXEC) $(TSRC)

clean:
	rm -f *.o $(TEXEC)