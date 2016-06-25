#include "nemu.h"

hwaddr_t page_translate(lnaddr_t addr) {
	assert(!(cpu.PE == 0 && cpu.PG == 1));
	if(cpu.PE == 0 || cpu.PG == 0) {
		return addr;
	}

	uint32_t pd_idx = (addr & 0xFFC00000) >> 22;
	uint32_t pg_idx = (addr & 0x003FF000) >> 12;
	uint32_t page_offset = (addr & 0x00000FFF);

	hwaddr_t pd_base = cpu.cr3.page_directory_base << 12;

	page_table_entry pde;
	pde.val = hwaddr_read(pd_base + (pd_idx << 2), 4);
	assert(pde.present);
	hwaddr_t pg_base = pde.page_frame_base << 12;

	page_table_entry pte;
	pte.val = hwaddr_read(pg_base + (pg_idx << 2), 4);
	assert(pte.present);
	hwaddr_t page_base = pte.page_frame_base << 12;

	return page_base + page_offset;
}
