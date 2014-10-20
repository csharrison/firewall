#include "containers.h"
#define LEFTO 1
#define RIGHTO 2
#define INSIDEO 3
#define OUTSIDEO 4
#define LEFTNO 5
#define RIGHTNO 6

char _range_overlaps(addr_t a_s, addr_t a_e, addr_t b_s, addr_t b_e);
void _node_tear_down(node_t *node, char free_next);
node_t *_node_setup(addr_t begin, addr_t end, int height);
void _link_node(node_t *node, node_t **succs, node_t *pred);

int _find(skip_list_t *sl, addr_t begin, addr_t end, node_t **preds, node_t **succs, int *overlaps);
int _random_level() ;

skip_list_t *skip_list_setup() {
	// seed the random number generator
	srand((unsigned int) time(NULL));
	skip_list_t *sl = malloc(sizeof(skip_list_t));
	node_t *tail = _node_setup(MAX_ADDR, MAX_ADDR, 0);
	node_t *head = _node_setup(0, 0, MAX_LEVEL + 1);
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
		_node_tear_down(cur, 1);
		cur = next;
	}
	_node_tear_down(sl->tail, 1);
	pthread_rwlock_destroy(sl->rwlock);
	free(sl);
}

char skip_list_contains(skip_list_t *sl, addr_t addr) {
	node_t *preds[MAX_LEVEL + 1];
	node_t *succs[MAX_LEVEL + 1];
	int level = _find(sl, addr, addr, preds, succs, NULL);
	if (level != -1) {
		return 1;
	}
	return 0;
}

char skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end) {
	node_t *preds[MAX_LEVEL + 1];
	node_t *succs[MAX_LEVEL + 1];

	int top_level = _random_level();
	int flevel;
	if((flevel = _find(sl, begin, end, preds, succs, NULL)) != -1) {
		return 0;
	}

	// merge along the lowest level
	addr_t tb = begin;
	addr_t te = end;
	int height = top_level;

	node_t *succ = succs[0];
	node_t *n = preds[0]->next[0];
	while(n != succ) {
		node_t *next = n->next[0];
		tb = tb < n->begin ? tb : n->begin;
		te = te < n->end ? n->end : te;
		height = n->height > height ? n->height : height;
		_node_tear_down(n, 1);
		n = next;
	}
	node_t *new_node = _node_setup(tb, te, height);

	// link up the new node
	int level;
	for (level = 0; level <= height; level++) {
		preds[level]->next[level] = new_node;
		new_node->next[level] = succs[level];
		// no overlap w/ succ
		assert(_range_overlaps(begin, end, succs[level]->begin, succs[level]->end) == LEFTNO);
	}
	return 1;
}

void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end) {
	node_t *preds[MAX_LEVEL + 1];
	node_t *succs[MAX_LEVEL + 1];
	int overlaps[MAX_LEVEL + 1];

	_find(sl, begin, end, preds, succs, overlaps);

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

			node_t *new_node = _node_setup(sb, se, curr->height);
			curr->begin = fb;
			curr->end = fe;

			_link_node(new_node, curr->next, curr);
		} else if(overlap == OUTSIDEO) {
			_link_node(pred, curr->next, NULL);
			_node_tear_down(curr, 0);
		} else {
			curr->begin = end > curr->begin ?  end : curr->begin;
			curr->end = begin < curr->end ? begin : curr->end;
			pred = curr;
		}
		curr = next;
	}
}

int _find(skip_list_t *sl, addr_t begin, addr_t end, node_t **preds, node_t **succs, int *overlaps) {
	int found = -1;
	node_t *pred = sl->head;
	int level;
	for(level = MAX_LEVEL; level >= 0; level--) {
		node_t *cur = pred->next[level];
		assert(cur != NULL);
		int overlap;
		int last_overlap = 0;
		while((overlap = _range_overlaps(begin, end, cur->begin, cur->end)) != LEFTNO) {
			// this ensures pred never overlaps
			if(overlap == RIGHTNO) pred = cur;
			cur = cur->next[level];
			assert(cur != NULL);
			last_overlap = overlap;
		}

		if (found == -1 && last_overlap == INSIDEO) {
			found = level;
		}
		preds[level] = pred;
		succs[level] = cur;
		if (overlaps) overlaps[level] = overlap;
	}
	return found;
}

void _link_node(node_t *node, node_t **succs, node_t *pred) {
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

node_t *_node_setup(addr_t begin, addr_t end, int height) {
	node_t *node = malloc(sizeof(node_t));
	node->begin = begin;
	node->end = end;

	node_t **next = NULL;
	if (height > 0) {
		next = malloc(sizeof(node_t *) * (unsigned int)(height + 1));
		memset(next, 0, sizeof(node_t *) * (unsigned int)(height + 1));
	}
	assert(height == 0 || next != NULL);

	node->next = next;
	node->height = height;
	return node;
}

void _node_tear_down(node_t *node, char free_next) {
	if (free_next) free(node->next);
	free(node);
}

char _range_overlaps(addr_t a_s, addr_t a_e, addr_t b_s, addr_t b_e) {
	if (a_s < b_s && a_e > b_e) {
		// CASE 1: right side of a overlaps
		return RIGHTO;
	} else if(a_s < b_e && a_e > b_e) {
		// CASE 2: left side of a overlap
		return LEFTO;
	} else if(a_s >= b_s && a_e <= b_e) {
		// CASE 3: b includes all of a
		return INSIDEO;
	} else if(a_s < b_s && a_e > b_e) {
		// CASE 4: a includes all of b
		return OUTSIDEO;
	} else if(a_e < b_s) {
		// CASE 5: a left of b
		return LEFTNO;
	} else if(a_s > b_e) {
		// CASE 6: a right of b
		return RIGHTNO;
	}
	assert("should take care of all cases");
	return 0;
}

int _random_level() {
	int r = rand();
	int i;
	int three_fourths = 1610612735;
	printf("%d %d\n", r, RAND_MAX);
	if (r < three_fourths) return 1;

	int c = three_fourths;
	for (i = 2; i < MAX_LEVEL; i++) {
		int diff = RAND_MAX - c;
		if (r < c + diff/2) return i;
		c = c + diff/2;
	}
	return MAX_LEVEL;
}

void skip_list_print(skip_list_t *sl) {
	node_t *curr = sl->head;

	while(1) {
		printf("(%5u, %5u):", curr->begin, curr->end);
		int level;
		for(level = 0; level < curr->height; level++) {
			printf("#");
		}
		printf("\n");
		if (curr == sl->tail) break;
		curr = curr->next[0];
	}
	printf("\n");
}