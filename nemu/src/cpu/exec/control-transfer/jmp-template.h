#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	cpu.eip += (&ops_decoded.src)->val; 
}

make_instr_helper(rel)

#include "cpu/exec/template-end.h"
