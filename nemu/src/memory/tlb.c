#include "nemu.h"
#include <stdlib.h>
#include <time.h>

#ifdef USE_TLB

TLB_LINE tlb[TLB_LINE_NR];

void tlb_init() {
	int i;
	for(i = 0; i < TLB_LINE_NR; ++i)
		tlb[i].valid = 0;
}

/*int32_t tlb_search(lnaddr_t addr) {*/
	/*int i;*/
	/*for(i = 0; i < TLB_LINE_NR; ++i) {*/
		/*if(tlb[i].valid)*/
			/*if(tlb[i].tag == (addr >> 12))*/
				/*[>tlb hit<]*/
				/*return i;*/
	/*}*/
	/*return -1;*/
/*}*/

int32_t tlb_search(lnaddr_t addr) {
	int i = addr >> 12;
	if(tlb[i].valid)
		return i;
	else
		return -1;
}

/*void tlb_fill(lnaddr_t addr, uint32_t pte) {*/
	/*int i;*/
	/*for(i = 0; i < TLB_LINE_NR; ++i) {*/
		/*if(tlb[i].valid == 0)*/
			/*break;*/
	/*}*/
	/*if(i >= TLB_LINE_NR) {*/
		/*[>no free line, must drop one before filling new<]*/
		/*srand((uint32_t)time(NULL));*/
		/*uint32_t random_line = rand() & (TLB_LINE_NR - 1);*/
		/*tlb[random_line].pte = pte;*/
		/*tlb[random_line].tag = (addr >> 12);*/
		/*tlb[random_line].valid = 1;*/
	/*}*/
	/*else {*/
		/*[>free line found, fill directly<]*/
		/*tlb[i].pte = pte;*/
		/*tlb[i].tag = (addr >> 12);*/
		/*tlb[i].valid = 1;*/
	/*}*/
/*}*/

void tlb_fill(lnaddr_t addr, uint32_t pte) {
	int i = addr >> 12;
	tlb[i].pte = pte;
	tlb[i].valid = 1;
}

#endif
