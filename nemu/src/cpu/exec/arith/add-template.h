#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
	//uint32_t left = (&ops_decoded.dest)->val;
	//uint32_t right = (&ops_decoded.src)->val;
	//set_CF(left, right, 1);
	unsigned long long left = (&ops_decoded.dest)->val;
	unsigned long long right = (&ops_decoded.src)->val;
	unsigned long long result = left + right;
	set_CF(result, DATA_BYTE);
	OPERAND_W(op_dest, op_dest->val + op_src->val);
	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(si2rm)
make_instr_helper(i2a)
make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
