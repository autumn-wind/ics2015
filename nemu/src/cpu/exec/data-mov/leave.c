#include "cpu/exec/helper.h"

make_helper(leave) {
	if(ops_decoded.is_data_size_16)
		assert(0);
	cpu.esp = cpu.ebp;
	cpu.ebp = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	return 1;
}

