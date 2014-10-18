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
	node_t *tail = node_setup(MAX_ADDR, 0, 0);
	node_t *head = node_setup(0, 0, MAX_LEVEL);

	int i;
	for(i = 0; i < head->height; i++) {
		head->next[i] = tail;
	}

	sl->head = head;
	sl->tail = tail;
	return sl;
}

void skip_list_tear_down(skip_list_t *sl) {
	node_t *cur = sl->head;
	while(cur != sl->tail) {
		node_t *next = cur->next[0];
		node_tear_down(cur);
		cur = next;
	}
	free(sl->preds);
}

int find(skip_list_t *sl, addr_t begin, addr_t end, node_t *preds, node_t *succs) {
	int found = -1;
	node_t *pred = sl->head;
	int level;
	for(level = MAX_LEVEL; level >= 0; level--) {
		node_t *cur = pred->next[level];
		int overlap;
		while((overlap=_range_overlaps(begin, end, cur->begin, cur->end)) == LEFTNO) {
			pred = cur;
			cur = pred->next[level];
		}

		if (found == -1 && overlap == INSIDEO) {
			found = level;
		}
		preds[level] = pred;
		succs[level] = cur;
	}
	return found;
}

char skip_list_contains(skip_list_t *sl, addr_t addr) {
	node_t preds[MAX_LEVEL];
	node_t succs[MAX_LEVEL];
	int level = find(sl, addr, preds, succs, NULL);

	node_t *f = succs[level];
	if (level != -1 && f->fully_linked && !f->removed) {
		return 1;
	}
	return 0;
}

int lock_and_validate(node_t *preds, node_t *succs, int top_level, node_t *victim, int *highest_locked) {
	// if victim is set, we are doing deletion
	int level;
	node_t *prev_pred = NULL;
	for(level = 0; level <= top_level; level++) {
		pred = preds[level];
		succ = succs[level];

		if (pred != prev_pred) {
			// so we don't double lock / unlock
			pthread_rw_lock(pred->lock);
			prev_pred = pred;
		}
		*highest_locked = level;
		char invalid;
		if (victim == NULL) {
			invalid = pred->removed || succ->removed || pred->next[level] != succ;
		} else {
			invalid = pred->removed || (pred->next[level] != victim);
		}
		// invalid... :/ try again
		if (invalid) return 0;
	}
	return 1;
}

void unlock_preds(node_t *preds, int highest_locked) {
	node_t *prev_pred = NULL;
	int level;
	for(level = 0; level <= highest_locked; level++) {
		node_t *pred = preds[level];
		if (pred != prev_pred) {
			pthread_rw_unlock(preds[level]->lock);
			prev_pred = pred;
		}
	}
}

char skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end) {
	node_t preds[MAX_LEVEL];
	node_t succs[MAX_LEVEL];

	// TODO: NORMALIZE RANDOMLEVEL
	int top_level = rand() % MAX_LEVEL;
	int done = 0;
	int overlap;
	while (!done) {
		int flevel = find(sl, begin, end, preds, succs);
		if (flevel != -1) {
			// the full range is included in the skip list
			node_t *found = succs[flevel];
			if (found->removed) {
				// but its about to be deleted, so try again
				continue;
			} else {
				// just return as soon as we're in a valid state
				while (! node->fully_linked) {}
				return 0;
			}
		}

		// the full range is not in the skip list
		int highest_locked;
		if((lock_and_validate(preds, succs, top_level, NULL, &highest_locked))) {
			node_t *new_node = node_setup(begin, end, top_level);
			for (level = 0; level <= top_level; level++) {
				// TODO: potentially destroy succ if we can merge
				new_node->next[level] = succs[level];
				preds[level]->next[level] = new_node;
			}
			new_node->fully_linked = 1; // LINEARIZATION POINT
			done = 1; // stop looping
		}
		unlock_preds(preds, highest_locked);
	}
}

void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end) {
	node_t preds[MAX_LEVEL];
	node_t succs[MAX_LEVEL];
	node_t *victim = NULL;
	char is_marked = 0;
	int top_level = -1;
	int done = 0;
	while (!done) {
		int flevel = find(sl, begin, end, preds, succs);
		if (flevel != -1) victim = succs[flevel];

		if (is_marked ||
			 (flevel != -1 &&
			  victim->fully_linked &&
			  victim->height == flevel &&
			  !victim->removed)) {
			if (! is_marked) {
				top_level = victim->height;
				pthread_rw_lock(victim->lock);
				if (victim->removed) {
					pthread_rw_unlock(victim->lock);
					return 0; //already deleted
				}
				victim->removed = 1; // mark for deletion
				is_marked = 1;
			}

			int highest_locked;
			if(lock_and_validate(preds, succs, top_level, victim, &highest_locked)) {
				for (level = top_level; level >= 0; level--) {
					preds[level]->next[level] = victim->next[level];
				}
				pthread_rw_unlock(victim->lock);
				done = 1;
			}
			unlock_preds(preds, highest_locked);
		} else {
			return 0;
		}
	}
}

int update_node(skip_list_t *sl, node_t *node, addr_t begin, addr_t end) {
	// better not be any overlefts to the left of node
	// so we only worry about overlaps to the right of node
	_merge_node(node, begin, end);

	int level;
	node_t *prev_cur = NULL;
	for(level = 0; level < node->height; level++) {
		node_t *cur = node->next[level];
		if (cur == prev_cur) continue;

		char overlap;
		while (cur && (overlap = _range_overlaps(tb, te, cur->begin, cur->end)) != LEFTNO) {
			assert(overlap != RIGHTNO);
			_merge_node(node, cur->begin, cur->end);
			// delete cur cause we overlap baby
			_delete_node(cur, node);
			cur = node->next[level];
		}
		prev_cur = cur; // no overlap or null
	}
}

// returns whether we successfully merged
void _merge_node(node_t *node, addr_t begin, addr_t end) {
	addr_t nb = node->begin;
	addr_t ne = node->end;

	node->begin = nb < begin ? nb : begin;
	node->end = ne < end ? end : ne;
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

void node_tear_down(node_t *node) {
	free(node->next);
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