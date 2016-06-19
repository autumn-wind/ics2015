#include "cpu/exec/template-start.h"

#define instr stos

static void do_execute() {
	uint32_t temp = 0;
	switch(DATA_BYTE) {
		case 1: temp = cpu.eax & 0x000000FF;break;
		case 2: temp = cpu.eax & 0x0000FFFF;break;
		case 4: temp = cpu.eax & 0xFFFFFFFF;break;
		default: assert(0);
	}
	swaddr_write(cpu.edi, DATA_BYTE, temp, ES);
	int8_t sign = 1;
	if(cpu.DF != 0)
		sign = -1;
	cpu.edi += DATA_BYTE * sign;

	print_asm_template2();
}

make_instr_helper(none)

#include "cpu/exec/template-end.h"
