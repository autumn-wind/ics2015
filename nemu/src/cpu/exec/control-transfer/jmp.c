#include "cpu/exec/helper.h"

static void do_jmp_b() {
	cpu.eip += (&ops_decoded.src)->val; 
}

make_helper(jmp_rel_b) {
	return idex(eip, decode_rel_b, do_jmp_b);
}

