#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

#define DATA_BYTE 1
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "mov-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(mov_i2r)
make_helper_v(mov_i2rm)
make_helper_v(mov_r2rm)
make_helper_v(mov_rm2r)
make_helper_v(mov_a2moffs)
make_helper_v(mov_moffs2a)

make_helper_v(cmova_rm2r)
make_helper_v(cmove_rm2r)
make_helper_v(cmovs_rm2r)
make_helper_v(cmovg_rm2r)
make_helper_v(cmovge_rm2r)
make_helper_v(cmovns_rm2r)
make_helper_v(cmovne_rm2r)

make_helper(mov_rm2sreg_w) {
	Operand rm, reg;
	rm.size = 2;
	int len = read_ModR_M(eip + 1, &rm, &reg);
	uint16_t seg_desc_offset = rm.val & 0xFFF8;
	assert(seg_desc_offset < cpu.gdtr.limit);
	uint32_t seg_desc[2];
	seg_desc[0] = lnaddr_read(cpu.gdtr.base + seg_desc_offset, 4);
	seg_desc[1] = lnaddr_read(cpu.gdtr.base + seg_desc_offset + 4, 4);
	uint32_t seg_base_addr = ((seg_desc[0] & 0xFFFF0000) >> 16) | ((seg_desc[1] & 0x000000FF) << 16) | (seg_desc[1] & 0xFF000000);
	uint32_t seg_limit = (seg_desc[0] & 0x0000FFFF) | (seg_desc[1] & 0x000F0000);
	if(seg_desc[1] & 0x00800000)
		seg_limit = (seg_limit << 12) | 0x00000FFF;

	switch(reg.reg) {
		case ES:	cpu.es.selector = rm.val;
					cpu.es.base = seg_base_addr;
					cpu.es.limit = seg_limit;
					break;
		case CS:	cpu.cs.selector = rm.val;
					cpu.cs.base = seg_base_addr;
					cpu.cs.limit = seg_limit;
					break;
		case SS:	cpu.ss.selector = rm.val;
					cpu.ss.base = seg_base_addr;
					cpu.ss.limit = seg_limit;
					break;
		case DS:	cpu.ds.selector = rm.val;
					cpu.ds.base = seg_base_addr;
					cpu.ds.limit = seg_limit;
					break;
		default:	assert(0);break;
	}
	return 1 + len;
}
