#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE result2 = op_src->val - 1;
	OPERAND_W(op_src, result2);

	/* TODO: Update EFLAGS. */
	unsigned int mask = 0;
	switch(DATA_BYTE) {
		case 1:	mask = 0x000000FF;break;
		case 2: mask = 0x0000FFFF;break;
		case 4: mask = 0xFFFFFFFF;break;
		default:assert(0);
	}
	unsigned long long left = (&ops_decoded.src)->val & mask;
	unsigned long long result = left - 1;
	set_CF(result, DATA_BYTE);
	set_ZF(result, DATA_BYTE);
	set_OF(left, 1, result, DATA_BYTE, 1);
	set_SF(result, DATA_BYTE);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
