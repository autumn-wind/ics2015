#include "nemu.h"

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
	if(cpu.PE == 0) {
		return addr;
	}
	else {
		uint32_t base, limit;
		switch(sreg) {
			case ES:
				base = cpu.es.base;
				limit = cpu.es.limit;
				break;
			case CS:
				base = cpu.cs.base;
				limit = cpu.cs.limit;
				break;
			case SS:
				base = cpu.ss.base;
				limit = cpu.ss.limit;
				break;
			case DS:
				base = cpu.ds.base;
				limit = cpu.ds.limit;
				break;
			default:
				assert(0);break;
		}
		assert(addr <= limit);
		return base + addr;
	}
}

