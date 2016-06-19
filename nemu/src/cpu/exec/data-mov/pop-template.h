#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	OPERAND_W(op_src, swaddr_read(cpu.esp, 4, SS));
	print_asm_template2();
	cpu.esp += 4;
}


make_instr_helper(r)
make_instr_helper(rm)


#include "cpu/exec/template-end.h"
