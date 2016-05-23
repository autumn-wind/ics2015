#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	OPERAND_W(op_dest, op_dest->val + op_src->val + cpu.CF);
	print_asm_template2();

	unsigned int mask = 0;
	switch(DATA_BYTE) {
		case 1:	mask = 0x000000FF;break;
		case 2: mask = 0x0000FFFF;break;
		case 4: mask = 0xFFFFFFFF;break;
		default:assert(0);
	}
	unsigned long long left = (&ops_decoded.dest)->val & mask;
	unsigned long long right = (&ops_decoded.src)->val & mask;
	unsigned long long result = left + right + cpu.CF;
	set_CF(result, DATA_BYTE);
	set_ZF(result, DATA_BYTE);
	set_OF(left, right, result, DATA_BYTE, 0);
	set_SF(result, DATA_BYTE);
}

make_instr_helper(r2rm)
make_instr_helper(si2rm)

#include "cpu/exec/template-end.h"
