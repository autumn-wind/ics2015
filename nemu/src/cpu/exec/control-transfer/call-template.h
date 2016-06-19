#include "cpu/exec/template-start.h"

#define instr call
static void do_execute() {
	cpu.esp -= 4;
	int instr_len = concat(decode_rel_, SUFFIX)(cpu.eip + 1) + 1;
	swaddr_write(cpu.esp, 4, cpu.eip + instr_len, SS);
	cpu.eip += (&ops_decoded.src)->val;
}

make_instr_helper(rel)
#undef instr

#define instr callrm
static void do_execute() {
	cpu.esp -= 4;
	int instr_len = concat(decode_rm_, SUFFIX)(cpu.eip + 1) + 1;
	swaddr_write(cpu.esp, 4, cpu.eip + instr_len, SS);
	cpu.eip = op_src->val - instr_len;
}

make_instr_helper(rm)
#undef instr

#include "cpu/exec/template-end.h"
