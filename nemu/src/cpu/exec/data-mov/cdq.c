#include "cpu/exec/helper.h"

make_helper(cdq_w) {
	assert(0);
	return 1;
}

make_helper(cdq_l) {
	int eax = cpu.eax;
	if(eax < 0) {
		cpu.edx = 0xFFFFFFFF;
	}
	else {
		cpu.edx = 0;
	}
	return 1;
}

make_helper_v(cdq)
