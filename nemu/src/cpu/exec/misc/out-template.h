#include "cpu/exec/template-start.h"

make_helper(concat(out_i_, SUFFIX)) {
	uint16_t imm = swaddr_read(eip + 1, 1, CS) & 0x00FF;
	pio_write(imm, DATA_BYTE, REG(0));
	return 1 + 1;
}

make_helper(concat(out_, SUFFIX)) {
	pio_write(reg_w(2), DATA_BYTE, REG(0));
	return 1;
}

#include "cpu/exec/template-end.h"
