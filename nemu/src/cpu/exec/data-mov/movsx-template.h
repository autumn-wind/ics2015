#include "cpu/exec/template-start.h"

#if DATA_BYTE == 2 || DATA_BYTE == 4

#define instr movsxb
static void do_execute() {
	unsigned int temp = op_src->val;
	if(temp & (0x1 << (1 * 8 - 1))) {
		temp = temp | 0xFFFFFF00;
	}
	else {
		temp = temp & 0x000000FF;
	}
	op_src->val = temp;

	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(rm2r)
#undef instr

#endif


#if DATA_BYTE == 4

#define instr movsxw
static void do_execute() {
	unsigned int temp = op_src->val;
	if(temp & (0x1 << (2 * 8 - 1))) {
		temp = temp | 0xFFFF0000;
	}
	else {
		temp = temp & 0x0000FFFF;
	}
	op_src->val = temp;

	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(rm2r)
#undef instr

#endif

#include "cpu/exec/template-end.h"
