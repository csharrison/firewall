#include "containers.h"
#define LEFTO 1
#define RIGHTO 2
#define INSIDEO 3
#define OUTSIDEO 4
#define LEFTNO 5
#define RIGHTNO 6
/* Set */

// seed the random number generator
srand(time(NULL));

skip_list_t *skip_list_setup() {
	skip_list_t *sl = malloc(sizeof(skip_list_t));
	node_t *tail = node_setup(MAX_ADDR, MAX_ADDR, 0);
	node_t *head = node_setup(0, 0, MAX_LEVEL);
	pthread_rwlock_t *rwlock = (pthread_rwlock_t *)malloc(sizeof(pthread_rwlock_t));
	if ((pthread_rwlock_init(rwlock, NULL))) {
		perror("pthread_rwlock_init");
		return NULL;
	}

	int i;
	for(i = 0; i < head->height; i++) {
		head->next[i] = tail;
	}

	sl->head = head;
	sl->tail = tail;
	sl->rwlock = rwlock;
	return sl;
}

void skip_list_tear_down(skip_list_t *sl) {
	node_t *cur = sl->head;
	while(cur != sl->tail) {
		node_t *next = cur->next[0];
		node_tear_down(cur, 1);
		cur = next;
	}
	free(sl->preds);
	pthread_rwlock_destroy(sl->rwlock);
	free(sl);
}

int find(skip_list_t *sl, addr_t begin, addr_t end, node_t *preds, node_t *succs, int *overlaps) {
	int found = -1;
	node_t *pred = sl->head;
	int level;
	for(level = MAX_LEVEL; level >= 0; level--) {
		node_t *cur = pred->next[level];
		assert(cur);
		int overlap;
		while((overlap=_range_overlaps(begin, end, cur->begin, cur->end)) != LEFTNO) {
			// this ensures pred never overlaps
			if(overlap == RIGHTNO) pred = cur;
			cur = pred->next[level];
		}

		if (found == -1 && overlap == INSIDEO) {
			found = level;
		}
		preds[level] = pred;
		succs[level] = cur;
		overlaps[level] = overlap;
	}
	return found;
}

char skip_list_contains(skip_list_t *sl, addr_t addr) {
	node_t preds[MAX_LEVEL];
	node_t succs[MAX_LEVEL];
	int level = find(sl, addr, addr, preds, succs, NULL);

	node_t *f = succs[level];
	if (level != -1 && f->fully_linked && !f->removed) {
		return 1;
	}
	return 0;
}

char skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end) {
	node_t preds[MAX_LEVEL];
	node_t succs[MAX_LEVEL];
	// TODO: NORMALIZE RANDOMLEVEL
	int top_level = rand() % MAX_LEVEL;
	int flevel;
	if((flevel = find(sl, begin, end, preds, succs, NULL)) != -1)
		return 0;
	}

	node_t *new_node = node_setup(begin, end, top_level);

	// link up the new node
	int level;
	for (level = 0; level < top_level; level++) {
		preds[level]->next[level] = new_node;
		new_node->next[level] = succs[level];
		// no overlap w/ succ
		assert(_range_overlaps(begin, end, succs[level]->begin, succs[level]->end) == LEFTNO);
	}

	// merge along the lowest level
	addr_t tb = begin;
	addr_t te = end;

	node_t *succ = succs[0];
	node_t *n = preds[0]->next[0];
	while(n != succ) {
		node_t *next = n->next[0];
		tb = tb < n->begin ? tb : n->begin;
		te = te < succ->end ? n->end : te;
		node_tear_down(n, 1);
		n = next;
	}
	new_node->begin = tb;
	new_node->end = te;
	return 1;
}

void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end) {
	node_t preds[MAX_LEVEL];
	node_t succs[MAX_LEVEL];
	int overlaps[MAX_LEVEL];

	int flevel =  = find(sl, begin, end, preds, succs, overlaps);

	node_t *pred = preds[0];
	node_t *curr = pred->next[0];
	node_t *succ = succs[0];
	while(curr != succ) {
		node_t *next = curr->next[0];
		int overlap = _range_overlaps(begin, end, curr->begin, curr->end);
		if (overlap == INSIDEO) {
			// create a new node
			addr_t fb = curr->begin;
			addr_t fe = begin;
			addr_t sb = end;
			addr_t se = curr->end;

			node_t *new_node = node_setup(sb, se, curr->height);
			curr->begin = fb;
			curr->end = fe;

			_link_node(new_node, curr->next, curr);
		} else if(overlap == OUTSIDEO) {
			_link_node(pred, curr->next, NULL);
			node_tear_down(curr, 0);
		} else {
			cur->begin = end > cur->begin ?  end : cur->begin;
			cur->end = begin < cur->end ? begin : cur->end;
			pred = curr;
		}
		curr = next;
	}
}

void _link_node(node_t *node, node_t *succs, node_t *pred) {
	int level;
	for(level = 0; level < node->height; level++) {
		if (succs != NULL) {
			node->next[level] = succs[level];
		}
		if(pred != NULL) {
			// used only when we are splitting a region into 2 in remove_range
			pred->next[level] = node;
		}
	}
}

node_t *node_setup(addr_t begin, addr_t end, int height) {
	node_t *node = malloc(sizeof(node_t));
	node->removed = 0;
	node->fully_linked = 0;
	node->begin = begin;
	node->end = end;

	node_t *next = NULL;
	if (height) {
		next = malloc(sizeof(node_t) * height);
		memset(0, next, sizeof(node_t) * height);
	}

	node->next = next;
	node->height = height;
	return node;
}

void node_tear_down(node_t *node, char free_next) {
	if (freee_next) free(node->next);
	free(node);
}

char _range_overlaps(addr_t a_s, addr_t a_e, addr_t b_s, addr_t b_e) {
	if (a_s < b_s && a_e <= b_e) {
		// CASE 1: right side of a overlaps
		return RIGHTO
	} else if(a_s < b_e && a_e > b_e) {
		// CASE 2: left side of a overlap
		return LEFTO
	} else if(a_s >= b_s && a_e <= b_e) {
		// CASE 3: b includes all of a
		return INSIDEO
	} else if(a_s < b_s && a_e > b_e) {
		// CASE 4: a includes all of b
		return OUTSIDEO
	} else if(a_e < b_s)
		// CASE 5: a left of b
		return LEFTNO;
	} else if(a_s > b_e) {
		// CASE 6: a right of b
		return RIGHTNO;
	}
	assert("should take care of all cases");
	return 0;
}