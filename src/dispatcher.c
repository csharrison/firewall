#include "dispatcher.h"
#define MAX_INFLIGHT 256

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("usage: firewall <num reader threads> <num writer threads>\n");
		exit(1);
	}
	int num_readers = atoi(argv[1]);
	int num_writers = atoi(argv[2]);

	pthread_t *treaders = malloc(sizeof(pthread_t) * (unsigned int)num_readers);
	pthread_t *twriters = malloc(sizeof(pthread_t) * (unsigned int)num_writers);
	reader_info_t **ireaders = malloc(sizeof(reader_info_t *) * (unsigned int)num_readers);
	writer_info_t **iwriters = malloc(sizeof(writer_info_t *) * (unsigned int)num_writers);

	png_t *png = png_setup();
	r_t *r = r_setup(MAX_ADDR);
	hist_t *hist = hist_setup();

	for (int i = 0; i < num_readers; i++) {
		reader_info_t *reader = reader_setup(MAX_INFLIGHT, png, r, hist);
		ireaders[i] = reader;
		pthread_create(&treaders[i], NULL, reader_start, (void *)reader);
	}
	for (int i = 0; i < num_writers; i++) {
		writer_info_t *w = writer_setup(MAX_INFLIGHT, png, r);
		iwriters[i] = w;
		pthread_create(&twriters[i], NULL, writer_start, (void *)w);
	}

	pgen_t *pgen = setup_packet_gen(NULL);

	while (1) {
		cpacket_t *cp = NULL;
		dpacket_t *dp = NULL;
		get_packet(pgen, &dp, &cp);
		assert(cp != NULL || dp != NULL);
		if (dp != NULL) {
			reader_info_t *r = ireaders[dp->seed % num_readers];
			reader_send_packet(r, dp);
		} else {
			writer_info_t *w = iwriters[cp->address % num_writers];
			writer_send_packet(w, cp);
		}
	}

	for (int i = 0; i < num_readers; i++) {
		reader_tear_down(ireaders[i]);
	}
	for (int i = 0; i < num_writers; i++) {
		writer_tear_down(iwriters[i]);
	}
	return 0;
}