#include "cpu/exec/template-start.h"

#define instr cmps

static void do_execute() {
	unsigned long long left = swaddr_read(cpu.esi, DATA_BYTE);
	unsigned long long right = swaddr_read(cpu.edi, DATA_BYTE);
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
