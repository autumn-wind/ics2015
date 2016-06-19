#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(cld) {
	cpu.DF = 0;
	return 1;
}

make_helper(lgdt) {
	/* segment selector no use here because the system is still in real mode */
	unsigned int lgdt_addr_operand = swaddr_read(eip + 2, 4, CS);
	cpu.gdtr.limit = swaddr_read(lgdt_addr_operand, 2, CS);
	cpu.gdtr.base = swaddr_read(lgdt_addr_operand + 2, 4, CS);
	return 1 + 1 + 4;
}

make_helper(mov_from_cr0) {
	char reg_index = (swaddr_read(eip + 1, 1, CS) & 0x38) >> 3;
	reg_l(reg_index) = cpu.cr0;
	return 1 + 1;
}

make_helper(mov_to_cr0) {
	char reg_index = (swaddr_read(eip + 1, 1, CS) & 0x38) >> 3;
	cpu.cr0 = reg_l(reg_index);
	return 1 + 1;
}

make_helper(ljmp) {
	cpu.eip = swaddr_read(eip + 1, 4, CS);
	cpu.cs.selector = swaddr_read(eip + 1 + 4, 2, CS);
	return 0;
}
