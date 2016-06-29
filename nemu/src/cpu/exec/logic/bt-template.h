#include "cpu/exec/template-start.h"

#define instr bt

static void do_execute() {
	unsigned int left = (&ops_decoded.dest)->val;
	unsigned int right = (&ops_decoded.src)->val;
	cpu.CF = (left >> right) & 0x1;
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
