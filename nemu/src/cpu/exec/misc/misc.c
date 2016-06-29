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
	unsigned int lgdt_addr_operand;
	if(cpu.PE == 0) {
		/* segment selector no use here because the system is still in real mode */
		lgdt_addr_operand = swaddr_read(eip + 2, 4, CS);
		cpu.gdtr.limit = swaddr_read(lgdt_addr_operand, 2, DS);
		cpu.gdtr.base = swaddr_read(lgdt_addr_operand + 2, 4, DS);
		return 1 + 1 + 4;
	}
	else {
		ModR_M modrm_byte;
		modrm_byte.val = swaddr_read(eip + 1, 1, CS);
		Operand rm;
		int len = load_addr(eip + 1, &modrm_byte, &rm);
		lgdt_addr_operand = rm.addr;
		cpu.gdtr.limit = swaddr_read(lgdt_addr_operand, 2, rm.sreg);
		cpu.gdtr.base = swaddr_read(lgdt_addr_operand + 2, 4, rm.sreg);
		return 1 + len;
	}
}

make_helper(lidt) {
	unsigned int lidt_addr_operand;
	if(cpu.PE == 0) {
		/* segment selector no use here because the system is still in real mode */
		lidt_addr_operand = swaddr_read(eip + 2, 4, CS);
		cpu.idtr.limit = swaddr_read(lidt_addr_operand, 2, DS);
		cpu.idtr.base = swaddr_read(lidt_addr_operand + 2, 4, DS);
		return 1 + 1 + 4;
	}
	else {
		ModR_M modrm_byte;
		modrm_byte.val = swaddr_read(eip + 1, 1, CS);
		Operand rm;
		int len = load_addr(eip + 1, &modrm_byte, &rm);
		lidt_addr_operand = rm.addr;
		cpu.idtr.limit = swaddr_read(lidt_addr_operand, 2, rm.sreg);
		cpu.idtr.base = swaddr_read(lidt_addr_operand + 2, 4, rm.sreg);
		return 1 + len;
	}
}

make_helper(mov_from_cr0) {
	char modrm_byte = swaddr_read(eip + 1, 1, CS);
	char reg_index = modrm_byte & 0x07;
	char special_reg_index = (modrm_byte & 0x38) >> 3;
	switch(special_reg_index) {
		case 0:	reg_l(reg_index) = cpu.cr0;break;
		case 3: reg_l(reg_index) = cpu.cr3.val;break;
		default:assert(0);
	}
	return 1 + 1;
}

make_helper(mov_to_cr0) {
	char modrm_byte = swaddr_read(eip + 1, 1, CS);
	char reg_index = modrm_byte & 0x07;
	char special_reg_index = (modrm_byte & 0x38) >> 3;
	switch(special_reg_index) {
		case 0:	cpu.cr0 = reg_l(reg_index);break;
		case 3: cpu.cr3.val = reg_l(reg_index);
#ifdef USE_TLB
				tlb_init();
#endif
				break;
		default:assert(0);
	}
	return 1 + 1;
}

make_helper(ljmp) {
	cpu.eip = swaddr_read(eip + 1, 4, CS);
	cpu.cs.selector = swaddr_read(eip + 1 + 4, 2, CS);
	return 0;
}

make_helper(std) {
	cpu.DF = 1;
	return 1;
}

void raise_intr(uint8_t);

make_helper(int_i) {
	if(cpu.PE == 0)
		assert(0);
	uint8_t intr_nr = swaddr_read(eip + 1, 1, CS);
	raise_intr(intr_nr);
	/* should never return from raise_intr() */
	assert(0);
	return 2;
}

make_helper(iret) {
	if(cpu.PE == 0)
		assert(0);
	uint32_t instr_pointer = swaddr_read(cpu.esp, 4, SS);
	cpu.esp += 4;
	uint16_t cs = swaddr_read(cpu.esp, 2, SS);
	cpu.esp += 4;
	uint32_t eflags = swaddr_read(cpu.esp, 4, SS);
	cpu.esp += 4;
	if(cs == 0)
		assert(0);
	uint32_t gdt_desc[2];
	gdt_desc[0] = lnaddr_read(cpu.gdtr.base + (cs & 0xFFF8), 4);
	gdt_desc[1] = lnaddr_read(cpu.gdtr.base + (cs & 0xFFF8) + 4, 4);
	SegDesc *p_gdt_desc = (SegDesc *)gdt_desc;
	if(p_gdt_desc->present == 0)
		assert(0);

	cpu.eip = instr_pointer;

	cpu.cs.selector = cs;
	cpu.cs.base = (p_gdt_desc->base_31_24 << 24) | (p_gdt_desc->base_23_16 << 16) | p_gdt_desc->base_15_0;
	uint32_t gdt_desc_limit = (p_gdt_desc->limit_19_16 << 16) | p_gdt_desc->limit_15_0;
	cpu.cs.limit = p_gdt_desc->granularity ? (gdt_desc_limit << 12) | 0xFFF : gdt_desc_limit;

	cpu.EFLAGS = eflags;

	return 0;
}

/* float instructions, do nothing */
make_helper(float_instr) {
	/* do nothing */
	Operand reg, rm;
	rm.size = 4;
	int len = read_ModR_M(eip + 1, &rm, &reg);
	return 1 + len;
}

make_helper(float_instr2) {
	/* do nothing */
	Operand reg, rm;
	rm.size = 4;
	int len = read_ModR_M(eip + 1, &rm, &reg);
	return 1 + len;
}

make_helper(hlt) {
	if(cpu.IF == 0)
		assert(0);
	for(; ;) {
		if(cpu.INTR)
			break;
	}
	return 1;
}

make_helper(cli) {
	cpu.IF = 0;
	return 1;
}

make_helper(sti) {
	cpu.IF = 1;
	return 1;
}
