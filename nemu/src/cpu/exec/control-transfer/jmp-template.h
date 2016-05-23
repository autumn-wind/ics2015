#include "cpu/exec/template-start.h"

#define instr jmp
static void do_execute() {
	cpu.eip += (&ops_decoded.src)->val; 
}

make_instr_helper(rel)
#undef instr

#define instr jmpni
static void do_execute() {
	int len = concat(decode_rm_, SUFFIX)(cpu.eip + 1);
	cpu.eip = (&ops_decoded.src)->val - (1 + len); 
}

make_instr_helper(rm)
#undef instr


#include "cpu/exec/template-end.h"
