#include "trap.h"
#include "FLOAT.h"

int main() {
	FLOAT a = f2F(1.2);
	nemu_assert(a == 0x13333);
	FLOAT b = f2F(5.6);
	nemu_assert(b == 0x59999);
	FLOAT c = f2F(-1.2);
	nemu_assert(c == 0xfffecccd);
	FLOAT d = f2F(-1.0);
	nemu_assert(d == -65536);
	FLOAT e = f2F(1.0);
	nemu_assert(e == 65536);
	FLOAT f = f2F(0.551222);
	nemu_assert(f == 36124);
	HIT_GOOD_TRAP;
	return 0;
}
