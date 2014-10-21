#include "containers.h"

#define LEFTO 1
#define RIGHTO 2
#define INSIDEO 3
#define OUTSIDEO 4
#define LEFTNO 5
#define RIGHTNO 6
#define EQUALO 7

#define MAX(x,y) ((x) < (y) ? (y) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

char _range_overlaps(addr_t a_s, addr_t a_e, addr_t b_s, addr_t b_e);
void _node_tear_down(node_t *node, char free_next);
node_t *_node_setup(addr_t begin, addr_t end, int height);
void _link_node(node_t *node, node_t *replacing, node_t *pred);
void _link_from_arrs(node_t *node, node_t **preds, node_t **succs);
int _find(skip_list_t *sl, addr_t begin, addr_t end, node_t **preds, node_t **succs, int *overlaps);
int _random_level() ;
int _random_level_at_most(int l);

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

int skip_list_contains(skip_list_t *sl, addr_t addr) {
	pthread_rwlock_rdlock(sl->rwlock);
	int level = _find(sl, addr, addr, NULL, NULL, NULL);
	pthread_rwlock_unlock(sl->rwlock);
	return level == -1 ? 0 : 1;
}

int skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end) {
	pthread_rwlock_wrlock(sl->rwlock);

	node_t *preds[MAX_LEVEL];
	node_t *succs[MAX_LEVEL];

	int top_level = _random_level();
	int r = 0;
	if((_find(sl, begin, end, preds, succs, NULL)) != -1) goto unlock;

	// merge along the lowest level
	addr_t tb = begin;
	addr_t te = end;
	int height = top_level;
	node_t *n = preds[0]->next[0];
	while(n != succs[0]) {
		node_t *next = n->next[0];
		tb = MIN(tb, n->begin);
		te = MAX(te, n->end);
		height = MAX(height, n->height);
		_node_tear_down(n, 1);
		n = next;
	}
	node_t *new_node = _node_setup(tb, te, height);
	_link_from_arrs(new_node, preds, succs);
	r = 1;
unlock:
	pthread_rwlock_unlock(sl->rwlock);
	return r;
}

void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end) {
	// hack - never delete head / tail
	begin = MAX(1, begin);
	end = MIN(end, MAX_ADDR - 1);
	pthread_rwlock_wrlock(sl->rwlock);

	node_t *preds[MAX_LEVEL];
	node_t *succs[MAX_LEVEL];
	int overlaps[MAX_LEVEL];

	_find(sl, begin, end, preds, succs, overlaps);

	node_t *pred = preds[0];
	node_t *curr = pred->next[0];
	node_t *succ = succs[0];
	while(curr != succ) {
		node_t *next = curr->next[0];
		int overlap = _range_overlaps(begin, end, curr->begin, curr->end);
		if (overlap == INSIDEO) {
			// new nodes (curr->begin, begin) and (end, curr->end)
			curr->end = begin;
			node_t *new_node = _node_setup(end, curr->end, _random_level_at_most(curr->height));
			_link_node(new_node, curr, curr);
		} else if(overlap == OUTSIDEO || overlap == EQUALO) {
			// totally destroy it
			_link_node(pred, curr, NULL);
			_node_tear_down(curr, 0);
		} else {
			// shrink it
			curr->begin = MAX(curr->begin, end);
			curr->end = MIN(curr->end, begin);
			pred = curr;
		}
		curr = next;
	}
	pthread_rwlock_unlock(sl->rwlock);
}

int _find(skip_list_t *sl, addr_t begin, addr_t end, node_t **preds, node_t **succs, int *overlaps) {
	int found = -1;
	node_t *pred = sl->head;
	int level;
	for(level = MAX_LEVEL - 1; level >= 0; level--) {
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

		if (found == -1 && (last_overlap == INSIDEO || last_overlap == EQUALO)) {
			found = level;
		}
		if (preds) preds[level] = pred;
		if (succs) succs[level] = cur;
		if (overlaps) overlaps[level] = overlap;
	}
	return found;
}

void _link_node(node_t *node, node_t *replacing, node_t *pred) {
	int level;
	node_t **succs = replacing->next;
	int copy_height = MIN(node->height, replacing->height);
	for(level = 0; level < copy_height; level++) {
		if (succs != NULL) {
			node->next[level] = succs[level];
		}
	}
	if (pred != NULL) {
		for(level = 0; level < node->height; level++) {
			pred->next[level] = node;
		}
	}
}

void _link_from_arrs(node_t *node, node_t **preds, node_t **succs) {
	int level;
	for (level = 0; level < node->height; level++) {
		preds[level]->next[level] = node;
		node->next[level] = succs[level];
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
	// TODO: maybe memory leak if free_next false
	if (free_next) free(node->next);
	free(node);
}

char _range_overlaps(addr_t a_s, addr_t a_e, addr_t b_s, addr_t b_e) {
	if (a_s < b_s && a_e > b_s && a_e < b_e) {
		// CASE 1: right side of a overlaps b
		return RIGHTO;
	} else if(a_s > b_s && a_s < b_e && a_e > b_e) {
		// CASE 2: left side of a overlap
		return LEFTO;
	} else if(a_s == b_s && a_e == b_e) {
		// CASE 7: EQUALOOOOAUAUAOAOD
		return EQUALO;
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
	int three_fourths = 1610612735; // wha huh? (3/4)*RAND_MAX
	if (r < three_fourths) return 1;

	int c = three_fourths;
	for (i = 2; i < MAX_LEVEL; i++) {
		int diff = RAND_MAX - c;
		if (r < c + diff/2) return i;
		c = c + diff/2;
	}
	return MAX_LEVEL - 1;
}

int _random_level_at_most(int l) {
	int level = _random_level();
	return MIN(level, l);
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