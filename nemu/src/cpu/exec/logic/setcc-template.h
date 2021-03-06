#include "cpu/exec/template-start.h"

#define instr setne
static void do_execute() {
	if(cpu.ZF == 0) {
		OPERAND_W(op_src, 1);
	}
	else {
		OPERAND_W(op_src, 0);
	}
	print_asm_template2();
}
make_instr_helper(rm)
#undef instr

#define instr setge
static void do_execute() {
	if(cpu.SF == cpu.OF) {
		OPERAND_W(op_src, 1);
	}
	else {
		OPERAND_W(op_src, 0);
	}
	print_asm_template2();
}
make_instr_helper(rm)
#undef instr

#define instr setl
static void do_execute() {
	if(cpu.SF != cpu.OF) {
		OPERAND_W(op_src, 1);
	}
	else {
		OPERAND_W(op_src, 0);
	}
	print_asm_template2();
}
make_instr_helper(rm)
#undef instr

#define instr setle
static void do_execute() {
	if(cpu.ZF == 1 || cpu.SF != cpu.OF) {
		OPERAND_W(op_src, 1);
	}
	else {
		OPERAND_W(op_src, 0);
	}
	print_asm_template2();
}
make_instr_helper(rm)
#undef instr

#define instr setg
static void do_execute() {
	if(cpu.ZF == 0 && cpu.SF == cpu.OF) {
		OPERAND_W(op_src, 1);
	}
	else {
		OPERAND_W(op_src, 0);
	}
	print_asm_template2();
}
make_instr_helper(rm)
#undef instr

#define instr sete
static void do_execute() {
	if(cpu.ZF == 1) {
		OPERAND_W(op_src, 1);
	}
	else {
		OPERAND_W(op_src, 0);
	}
	print_asm_template2();
}
make_instr_helper(rm)
#undef instr

#include "cpu/exec/template-end.h"
