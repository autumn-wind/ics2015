#include <setjmp.h>
#include "nemu.h"

#define INTERRUPT_GATE_32   0xE
#define TRAP_GATE_32        0xF

extern jmp_buf jbuf;

void raise_intr(uint8_t intr_nr) {
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */
	
	if((intr_nr << 3) + 7 > cpu.idtr.limit)
		assert(0);
	uint32_t idt_desc[2];
	idt_desc[0] = lnaddr_read(cpu.idtr.base + (intr_nr << 3), 4);
	idt_desc[1] = lnaddr_read(cpu.idtr.base + (intr_nr << 3) + 4, 4);
	GateDesc *p_idt_desc = (GateDesc *)idt_desc;
	if(p_idt_desc->present == 0)
		assert(0);
	uint16_t seg_seclector = p_idt_desc->segment;
	if(seg_seclector == 0)
		assert(0);
	if((seg_seclector & 0xFFF8) + 7 > cpu.gdtr.limit)
		assert(0);
	uint32_t gdt_desc[2];
	gdt_desc[0] = lnaddr_read(cpu.gdtr.base + (seg_seclector & 0xFFF8), 4);
	gdt_desc[1] = lnaddr_read(cpu.gdtr.base + (seg_seclector & 0xFFF8) + 4, 4);
	SegDesc *p_gdt_desc = (SegDesc *)gdt_desc;
	if(p_gdt_desc->present == 0)
		assert(0);
	if(cpu.eip > cpu.cs.limit)
		assert(0);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.EFLAGS, SS);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 2, cpu.cs.selector, SS);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.eip + 2, SS);
	cpu.cs.selector = seg_seclector;
	cpu.eip = (p_idt_desc->offset_31_16 << 16) | p_idt_desc->offset_15_0;
	cpu.cs.base = (p_gdt_desc->base_31_24 << 24) | (p_gdt_desc->base_23_16 << 16) | p_gdt_desc->base_15_0;
	uint32_t gdt_desc_limit = (p_gdt_desc->limit_19_16 << 16) | p_gdt_desc->limit_15_0;
	cpu.cs.limit = p_gdt_desc->granularity ? (gdt_desc_limit << 12) | 0xFFF : gdt_desc_limit;
	if(p_idt_desc->type == INTERRUPT_GATE_32)
		cpu.IF = 0;
	
	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}
