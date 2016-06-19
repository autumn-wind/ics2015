#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, (&ops_decoded.src)->val, SS);
}


make_instr_helper(r)
make_instr_helper(rm)
make_instr_helper(i)


#include "cpu/exec/template-end.h"
