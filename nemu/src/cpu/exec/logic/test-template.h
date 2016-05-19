#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	unsigned int result = (&ops_decoded.dest)->val & (&ops_decoded.src)->val;
	set_ZF(result);
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
