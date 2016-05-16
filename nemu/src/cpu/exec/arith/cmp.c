#include "cpu/exec/helper.h"

static void do_cmp_l() {
	unsigned int result = (&ops_decoded.dest)->val - (&ops_decoded.src)->val;
	set_ZF(result);
}

make_helper(cmp_si2rm_w){
	assert(0);
	return 0;
}

make_helper(cmp_si2rm_l){
	return idex(eip, decode_si2rm_l, do_cmp_l);
}

make_helper_v(cmp_si2rm)
