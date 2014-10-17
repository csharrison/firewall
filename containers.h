set_t *skip_list_setup();
void skip_list_tear_down(set_t *set);
char skip_list_contains(set_t *set, addr_t addr);
void skip_list_add_range(set_t *set, addr_t begin, addr_t end);
void skip_list_remove_range(set_t *set, addr_t beign, addr_t end);



set_t *list_setup();
void list_tear_down(set_t *set);
char list_contains(set_t *set, addr_t addr);
void list_add_range(set_t *set, addr_t begin, addr_t end);
void list_remove_range(set_t *set, addr_t beign, addr_t end);



