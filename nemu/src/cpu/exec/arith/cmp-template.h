#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	unsigned int left = (&ops_decoded.dest)->val;
	unsigned int right = (&ops_decoded.src)->val;
	unsigned int result = left - right;
	set_CF(left, right);
	set_ZF(result);
}

make_instr_helper(si2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
