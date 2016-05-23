#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	unsigned long long result = (&ops_decoded.dest)->val & (&ops_decoded.src)->val;
	cpu.CF = cpu.OF = 0;
	set_ZF(result);
	set_SF(result, DATA_BYTE);
}

make_instr_helper(i2r)
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
