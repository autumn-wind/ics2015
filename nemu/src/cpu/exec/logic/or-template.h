#include "cpu/exec/template-start.h"

#define instr or

static void do_execute () {
	DATA_TYPE result2 = op_dest->val | op_src->val;
	OPERAND_W(op_dest, result2);

	/* TODO: Update EFLAGS. */
	unsigned int mask = 0;
	switch(DATA_BYTE) {
		case 1:	mask = 0x000000FF;break;
		case 2: mask = 0x0000FFFF;break;
		case 4: mask = 0xFFFFFFFF;break;
		default:assert(0);
	}
	unsigned long long left = (&ops_decoded.dest)->val & mask;
	unsigned long long right = (&ops_decoded.src)->val & mask;
	unsigned long long result = left | right;
	set_ZF(result, DATA_BYTE);
	set_SF(result, DATA_BYTE);
	cpu.CF = 0;
	cpu.OF = 0;

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
