#include "cpu/exec/template-start.h"

#define instr sbb

static void do_execute() {
	//uint32_t left = (&ops_decoded.dest)->val;
	//uint32_t right = (&ops_decoded.src)->val;
	//set_CF(left, right, 1);
	unsigned long long left = (&ops_decoded.dest)->val;
	unsigned long long right = (&ops_decoded.src)->val + cpu.CF;
	unsigned long long result = left - right;

	set_CF(result, DATA_BYTE);
	set_ZF(result);
	set_OF(left, right, result, DATA_BYTE, 1);
	set_SF(result, DATA_BYTE);

	OPERAND_W(op_dest, op_dest->val - op_src->val - cpu.CF);
	print_asm_template2();
}

make_instr_helper(si2rm)
make_instr_helper(i2rm)
make_instr_helper(rm2r)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
