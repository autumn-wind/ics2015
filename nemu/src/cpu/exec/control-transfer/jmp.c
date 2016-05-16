#include "cpu/exec/helper.h"

static void do_je_b() {
	if(cpu.ZF == 1) {
		cpu.eip += (&ops_decoded.src)->val; 
	}
}

make_helper(je_rel_b) {
	return idex(eip, decode_rel_b, do_je_b);
}

