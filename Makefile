TEXEC = build/test
TSRC = src/test/*.c src/collections/skip_list.c src/collections/R.c

CFLAGS= -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align
CFLAGS +=   -Winline -Wfloat-equal -Wnested-externs
CFLAGS +=   -pedantic -std=c11
CFLAGS +=   -D_GNU_SOURCE -g
CFLAGS +=   -iquotesrc/include -pthread

CC = /usr/bin/gcc-4.9

test:
	$(CC) $(CFLAGS) -o $(TEXEC) $(TSRC)

clean:
	rm -f *.o $(TEXEC)
