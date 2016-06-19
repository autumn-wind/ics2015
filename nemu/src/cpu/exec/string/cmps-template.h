#include "cpu/exec/template-start.h"

#define instr cmps

static void do_execute() {
	unsigned int mask = 0;
	switch(DATA_BYTE) {
		case 1:	mask = 0x000000FF;break;
		case 2: mask = 0x0000FFFF;break;
		case 4: mask = 0xFFFFFFFF;break;
		default:assert(0);
	}
	unsigned long long left = swaddr_read(cpu.esi, DATA_BYTE, DS) & mask;
	unsigned long long right = swaddr_read(cpu.edi, DATA_BYTE, ES) & mask;
	unsigned long long result = left - right;
	set_CF(result, DATA_BYTE);
	set_ZF(result, DATA_BYTE);
	set_OF(left, right, result, DATA_BYTE, 1);
	set_SF(result, DATA_BYTE);

	int8_t sign = 1;
	if(cpu.DF != 0)
		sign = -1;
	cpu.esi += DATA_BYTE * sign;
	cpu.edi += DATA_BYTE * sign;

	print_asm_template2();
}

make_instr_helper(none)

#include "cpu/exec/template-end.h"
