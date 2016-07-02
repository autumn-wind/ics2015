#include "cpu/exec/helper.h"

make_helper(cwde_w) {
	assert(0);
	return 1;
}

make_helper(cwde_l) {
	int16_t ax = reg_w(0);
	if(ax < 0) {
		cpu.eax |= 0xFFFF0000;
	}
	else {
		cpu.eax &= 0x0000FFFF;
	}
	return 1;
}

make_helper_v(cwde)
