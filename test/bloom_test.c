#include "../bloom.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#define BUF_SIZE 4096

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: bftest <path>\n");
		return 1;
	}

	bfilter_t *bf = bf_setup((1 << 21), 2);
	int fd = open(argv[1], O_RDONLY);

	if (fd < 0) return 1;


	char *buf = (char *)malloc(BUF_SIZE);
	memset(buf, 0, BUF_SIZE);
	ssize_t r;
	while((r = read(fd, buf, BUF_SIZE))) {
		
		char *c = buf;
		char *end;
		while (1) {
			while(*c == ' ' || *c == '\n') c++;
			end = c;
			while(*end != '\0' && *end != '\n' && *end != ' ') end++;

			if (*end == '\0' ) break;
			*end = '\0';
			bf_add_member(c, bf);
			assert(bf_is_member(c, bf));
			c = end + 1;
		}

		memset(buf, 0, BUF_SIZE);
	}
	printf("Try the dictionary!\n");
	printf(">> ");
	while(fgets(buf, BUF_SIZE, stdin)) {
		char *c = strpbrk(buf, "\n");
		if (c) *c = '\0';
		if (*buf == '\0') break;
		if (!strncmp(buf, "print", 5)) {
			bf_stat(bf);
		} else if (bf_is_member(buf, bf)) {
			printf("%s is a word\n", buf);
		} else {
			printf("%s is not a word\n", buf);
		}
		printf(">> ");
	}

}
