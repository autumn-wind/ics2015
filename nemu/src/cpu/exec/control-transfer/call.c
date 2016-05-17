#include "cpu/exec/helper.h"

static void do_call_l() {
	cpu.esp -= 4;
	int len = decode_rel_l(cpu.eip + 1) + 1;
	swaddr_write(cpu.esp, 4, cpu.eip + len);
	cpu.eip += (&ops_decoded.src)->val;
}

make_helper(call_rel_w) {
	assert(0);
	return 0;
}

make_helper(call_rel_l) {
	return idex(eip, decode_rel_l, do_call_l);
}

make_helper_v(call_rel)
