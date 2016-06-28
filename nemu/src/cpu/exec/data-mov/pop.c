#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "pop-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "pop-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "pop-template.h"
#undef DATA_BYTE

make_helper_v(pop_r)
make_helper_v(pop_rm)

static inline uint32_t pop_gen_regs() {
	uint32_t temp = swaddr_read(cpu.esp, 4, SS);
	cpu.esp += 4;
	return temp;
}

make_helper(popa) {
	cpu.edi = pop_gen_regs();
	cpu.esi = pop_gen_regs();
	cpu.ebp = pop_gen_regs();
	pop_gen_regs();
	cpu.ebx = pop_gen_regs();
	cpu.edx = pop_gen_regs();
	cpu.ecx = pop_gen_regs();
	cpu.eax = pop_gen_regs();
	return 1;
}
