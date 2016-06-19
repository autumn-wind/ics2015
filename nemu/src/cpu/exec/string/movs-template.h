#include "cpu/exec/template-start.h"

#define instr movs

static void do_execute() {
	uint32_t temp = swaddr_read(cpu.esi, DATA_BYTE, DS);
	swaddr_write(cpu.edi, DATA_BYTE, temp, ES);
	int8_t sign = 1;
	if(cpu.DF != 0)
		sign = -1;
	cpu.esi += DATA_BYTE * sign;
	cpu.edi += DATA_BYTE * sign;

	print_asm_template2();
}

make_instr_helper(none)

#include "cpu/exec/template-end.h"
