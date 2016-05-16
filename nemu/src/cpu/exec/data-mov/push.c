#include "cpu/exec/helper.h"

static void do_push_l() {
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, (&ops_decoded.src)->val);
}

make_helper(push_r_w) {
	assert(0);
}

make_helper(push_r_l) {
	return idex(eip, decode_r_l, do_push_l);
}

make_helper_v(push_r)
