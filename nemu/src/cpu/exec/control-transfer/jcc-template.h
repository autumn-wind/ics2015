#include "cpu/exec/template-start.h"

#define instr ja
static void do_execute() {
	if(cpu.ZF == 0 && cpu.CF == 0) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr je
static void do_execute() {
	if(cpu.ZF == 1) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jae
static void do_execute() {
	if(cpu.CF == 0) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jne
static void do_execute() {
	if(cpu.ZF == 0) {
		//printf("%x\n", (&ops_decoded.src)->val);
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jb
static void do_execute() {
	if(cpu.CF == 1) {
		//printf("%x\n", (&ops_decoded.src)->val);
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jbe
static void do_execute() {
	if(cpu.ZF == 1 || cpu.CF == 1) {
		//printf("%x\n", (&ops_decoded.src)->val);
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jl
static void do_execute() {
	if(cpu.SF != cpu.OF) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jle
static void do_execute() {
	if(cpu.ZF == 1 || cpu.SF != cpu.OF) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jg
static void do_execute() {
	if(cpu.ZF == 0 && cpu.SF == cpu.OF) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#define instr jge
static void do_execute() {
	if(cpu.SF == cpu.OF) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_instr_helper(rel)
#undef instr

#include "cpu/exec/template-end.h"
