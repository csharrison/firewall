FEXEC = build/firewall
FSRC = src/collections/*.c src/packet/*  src/random/* src/*.c -lm
TEXEC = build/test
TSRC = src/test/*.c src/collections/skip_list.c src/collections/R.c src/packet/*.c src/random/*.c -lm

CFLAGS= -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align
CFLAGS +=   -Winline -Wfloat-equal -Wnested-externs
CFLAGS +=   -pedantic -std=c11
CFLAGS +=   -D_GNU_SOURCE -g  -pg
CFLAGS +=   -iquotesrc/include -pthread
CFLAGS +=   -lrt

CC = gcc-4.9

firewall:
	$(CC) $(CFLAGS) -o $(FEXEC) $(FSRC)

test:
	$(CC) $(CFLAGS) -o $(TEXEC) $(TSRC)

clean:
	rm -f *.o $(TEXEC)
