#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	if(!free_)
		return NULL;
	WP *new = free_;
	free_ = free_->next;
	new->next = head;
	head = new;
	return new;
}

bool free_wp(int no) {
	WP *pre = NULL, *p = head;
	while(p && p->NO != no) {
		pre = p;
		p = p->next;
	}
	if(!p)	return false;
	if(!pre)
		head = p->next;
	else
		pre->next = p->next;
	p->next = free_;
	free_ = p;
	return true;
}

void print_watchpoints() {
	if(!head) {
		printf("No watchpoints.\n");
		return;
	}
	WP *p = head;
	printf("Num\tWhat\n");
	while(p) {
		printf("%3d\t%s\n", p->NO, p->str);
		p = p->next;
	}
}

int check_watchpoints() {
	int changed = 0;
	WP *p = head;
	while(p) {
		bool success;
		uint32_t new_val = expr(p->str, &success);
		if(!success) {
			assert(0);
		}
		if(new_val != p->old_val){
			printf("watchpoint %d: %s\n", p->NO, p->str);
			printf("old value: %u\n", p->old_val);
			printf("new value: %u\n", new_val);
			printf("\n");
			p->old_val = new_val;
			changed += 1;
		}
		p = p->next;
	}
	return changed;
}



