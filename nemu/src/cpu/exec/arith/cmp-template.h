#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	//uint32_t left = (&ops_decoded.dest)->val;
	//uint32_t right = (&ops_decoded.src)->val;
	//uint32_t result = left - right;
	//set_CF(left, right, 0);
	unsigned long long left = (&ops_decoded.dest)->val;
	unsigned long long right = (&ops_decoded.src)->val;
	unsigned long long result = left - right;
	set_CF(result, DATA_BYTE);
	set_ZF(result);
}

make_instr_helper(si2rm)
make_instr_helper(r2rm)
make_instr_helper(i2a)
make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
