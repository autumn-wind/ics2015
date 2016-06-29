#include "cpu/exec/template-start.h"

make_helper(concat(in_i_, SUFFIX)) {
	uint16_t imm = swaddr_read(eip + 1, 1, CS) & 0x00FF;
	REG(0) = pio_read(imm, DATA_BYTE);
	return 1 + 1;
}

make_helper(concat(in_, SUFFIX)) {
	REG(0) = pio_read(reg_w(2), DATA_BYTE);
	return 1;
}

#include "cpu/exec/template-end.h"
