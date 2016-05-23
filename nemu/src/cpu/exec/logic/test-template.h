#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	unsigned int mask = 0;
	switch(DATA_BYTE) {
		case 1:	mask = 0x000000FF;break;
		case 2: mask = 0x0000FFFF;break;
		case 4: mask = 0xFFFFFFFF;break;
		default:assert(0);
	}
	unsigned long long left = (&ops_decoded.dest)->val & mask;
	unsigned long long right = (&ops_decoded.src)->val & mask;
	unsigned long long result = left & right;
	cpu.CF = 0;
	cpu.OF = 0;
	set_ZF(result, DATA_BYTE);
	set_SF(result, DATA_BYTE);
}

make_instr_helper(i2r)
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
