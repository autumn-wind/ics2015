#include "cpu/exec/template-start.h"

#define instr mov
static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX), DS);

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr, DS);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}
#undef instr

#if DATA_BYTE == 2 || DATA_BYTE == 4
#define instr cmova
static void do_execute() {
	if(cpu.CF == 0 && cpu.ZF == 0){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovae
static void do_execute() {
	if(cpu.CF == 0){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmove
static void do_execute() {
	if(cpu.ZF == 1){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovs
static void do_execute() {
	if(cpu.SF == 1){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovg
static void do_execute() {
	if(cpu.ZF == 0 && cpu.SF == cpu.OF){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovns
static void do_execute() {
	if(cpu.SF == 0){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovge
static void do_execute() {
	if(cpu.SF == cpu.OF){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovbe
static void do_execute() {
	if(cpu.CF == 1 || cpu.ZF == 1){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovl
static void do_execute() {
	if(cpu.SF != cpu.OF){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovle
static void do_execute() {
	if(cpu.ZF == 1 || cpu.SF != cpu.OF){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#define instr cmovne
static void do_execute() {
	if(cpu.ZF == 0){
		OPERAND_W(op_dest, op_src->val);
		print_asm_template2();
	}
}
make_instr_helper(rm2r)
#undef instr

#endif

#include "cpu/exec/template-end.h"
