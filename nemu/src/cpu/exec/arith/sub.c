#include "cpu/exec/helper.h"

static void do_sub_l() {
	unsigned int result = (&ops_decoded.dest)->val - (&ops_decoded.src)->val;
	set_ZF(result);
	write_operand_l((&ops_decoded.dest), result);
}

make_helper(sub_si2rm_w){
	assert(0);
	return 0;
}

make_helper(sub_si2rm_l){
	return idex(eip, decode_si2rm_l, do_sub_l);
}

make_helper_v(sub_si2rm)
