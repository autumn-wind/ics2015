#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	unsigned int old_val;
	char str[EXPR_LEN];

	/* TODO: Add more members if necessary */
} WP;

void init_wp_list();
WP* new_wp();
bool free_wp();
void print_watchpoints();
int check_watchpoints();

#endif
