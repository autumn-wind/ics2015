#include "cpu/exec/template-start.h"

#define instr movzx
static void do_execute() {
	OPERAND_W(op_dest, (op_src->val & 0x000000FF));
	print_asm_template2();
}

make_instr_helper(rm2r)
#undef instr

#define instr movzxw
static void do_execute() {
	OPERAND_W(op_dest, (op_src->val & 0x0000FFFF));
	print_asm_template2();
}

make_instr_helper(rm2r)
#undef instr

#include "cpu/exec/template-end.h"
