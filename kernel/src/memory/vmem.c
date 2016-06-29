#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

/* Use the function to get the start address of user page directory. */
inline PDE* get_updir();

static PTE vmptable[SCR_SIZE / PAGE_SIZE] align_to_page; //vedio memory page tables

void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	/*panic("please implement me!\n");*/

	PDE *updir = get_updir();

	/*make pde*/
	PTE *uptable = vmptable;
	uint32_t pframe_addr = VMEM_ADDR & 0xFFFFF000;
	uint32_t pde_idx = pframe_addr >> 22;
	for(; pframe_addr < VMEM_ADDR + SCR_SIZE; pframe_addr += PT_SIZE) {
		updir[pde_idx].val = make_pde(va_to_pa(uptable));
		uptable += NR_PTE;
		pde_idx++;
	}

	/*make pte*/
	uptable = vmptable;
	pframe_addr = VMEM_ADDR & 0xFFFFF000;
	uint32_t pte_idx = (pframe_addr & 0x003FF000) >> 12;
	for(; pframe_addr < VMEM_ADDR + SCR_SIZE; pframe_addr += PAGE_SIZE) {
		uptable[pte_idx].val = make_pte(pframe_addr);
		pte_idx++;
	}
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

