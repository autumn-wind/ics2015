#include "cpu/exec/helper.h"

static void do_test_l() {
	unsigned int result = (&ops_decoded.dest)->val & (&ops_decoded.src)->val;
	set_ZF(result);
}

make_helper(test_r2rm_w) {
	assert(0);
	return 0;
}

make_helper(test_r2rm_l) {
	return idex(eip, decode_r2rm_l, do_test_l);
}

make_helper_v(test_r2rm)
