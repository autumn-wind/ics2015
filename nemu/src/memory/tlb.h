#ifndef __TLB_H__
#define __TLB_H__

int32_t tlb_search(lnaddr_t addr);
void tlb_fill(lnaddr_t addr, uint32_t pte);

extern TLB_LINE tlb[TLB_LINE_NR];

#endif
