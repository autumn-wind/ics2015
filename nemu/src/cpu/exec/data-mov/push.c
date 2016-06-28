#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "push-template.h"
#undef DATA_BYTE

make_helper_v(push_r)
make_helper_v(push_rm)
make_helper_v(push_i)

static inline void push_gen_reg(uint32_t reg) {
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, reg, SS);
}

make_helper(pusha) {
	uint32_t temp = cpu.esp;
	push_gen_reg(cpu.eax);
	push_gen_reg(cpu.ecx);
	push_gen_reg(cpu.edx);
	push_gen_reg(cpu.ebx);
	push_gen_reg(temp);
	push_gen_reg(cpu.ebp);
	push_gen_reg(cpu.esi);
	push_gen_reg(cpu.edi);
	return 1;
}
