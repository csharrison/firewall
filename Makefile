TEXEC = test
TSRC = src/test/*.c src/containers.c src/R.c

CFLAGS= -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align
CFLAGS +=   -Winline -Wfloat-equal -Wnested-externs
CFLAGS +=   -pedantic -std=c11
CFLAGS +=   -D_GNU_SOURCE -g
CFLAGS +=   -iquotesrc/include -pthread

CC = /usr/bin/gcc-4.9

.PHONY: all test clean

test:
	$(CC) $(CFLAGS) -o $(TEXEC) $(TSRC)

clean:
	rm -f *.o $(TEXEC)
