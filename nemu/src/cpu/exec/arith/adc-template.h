#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	OPERAND_W(op_dest, op_dest->val + op_src->val + cpu.CF);
	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(si2rm)

#include "cpu/exec/template-end.h"
