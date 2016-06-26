#include "nemu.h"
#include "tlb.h"

hwaddr_t page_translate(lnaddr_t addr) {
	assert(!(cpu.PE == 0 && cpu.PG == 1));
	if(cpu.PE == 0 || cpu.PG == 0) {
		return addr;
	}

	page_table_entry pde;
	page_table_entry pte;
	uint32_t page_offset = (addr & 0x00000FFF);

	int32_t tlb_idx = tlb_search(addr);

	if(tlb_idx != -1) {
		/*tlb hit*/
		assert(tlb_idx < TLB_LINE_NR);
		pte.val = tlb[tlb_idx].pte;
	}
	else {
		/*tlb not hit, must page walk*/
		uint32_t pd_idx = (addr & 0xFFC00000) >> 22;
		uint32_t pg_idx = (addr & 0x003FF000) >> 12;

		hwaddr_t pd_base = cpu.cr3.page_directory_base << 12;

		pde.val = hwaddr_read(pd_base + (pd_idx << 2), 4);
		assert(pde.present);
		hwaddr_t pg_base = pde.page_frame_base << 12;

		pte.val = hwaddr_read(pg_base + (pg_idx << 2), 4);
		/*fill tlb a new line*/
		tlb_fill(addr, pte.val);
	}

	assert(pte.present);
	hwaddr_t page_base = pte.page_frame_base << 12;

	return page_base + page_offset;
}
