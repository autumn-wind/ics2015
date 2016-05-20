#include "cpu/exec/helper.h"

make_helper(leave) {
	cpu.esp = cpu.ebp;
	cpu.ebp = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	return 1;
}
