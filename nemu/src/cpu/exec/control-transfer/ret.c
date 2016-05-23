#include "cpu/exec/helper.h"

make_helper(ret) {
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	return 0;
}

make_helper(ret_i) {
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	int imm16 = instr_fetch(eip + 1, 2);
	cpu.esp += imm16;
	return 0;
}

