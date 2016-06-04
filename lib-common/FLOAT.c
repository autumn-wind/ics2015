#include "FLOAT.h"

#define GET_SIGN(x) ((x & 0x80000000) >> 31)
#define GET_E(x) ((x & 0x7F800000) >> 23)
#define SET_E(x) ((x & 0x007FFFFF) | 0x00800000)

FLOAT f2F(float a) {
	int i = *(int *)&a;
	int sign = GET_SIGN(i);
	int e = GET_E(i);
	if(e == 0) {
		return 0;
	}
	else if(e == 255) {
		nemu_assert(0);
	} 
	else {
		int x = e - 127 + 16;
		if(x < 0) 
			i = 0;
		else if(x == 0)
			i = 1;
		else {
			i = SET_E(i);
			if(x <= 23)
				i = i >> (23 - x);
			else if(x  <= 30)
				i = i << (x - 23);
			else
				nemu_assert(0);
		}
		return sign ? -i : i;
	}
}

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	long long r = (long long)a * b;
	return r >> SCALE;
	/*return a * (b >> SCALE);*/
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	asm volatile(
			"shl $0x10, %1\n\t"
			"sar $0x10, %2\n\t"
			"idivl %3"
			:"=a"(a)
			:"0"(a), "d"(a), "g"(b)
			:"memory");
	return a;

	/*FLOAT d = a >> 16;*/
	/*a = a << 16;*/
	/*asm volatile(*/
			/*"idivl %3"*/
			/*:"=a"(a)*/
			/*:"0"(a), "d"(d), "g"(b)*/
			/*:"memory");*/
	/*return a;*/
}

FLOAT Fabs(FLOAT a) {
	/*set_bp();*/
	return a >= 0 ? a : -a;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

int F2int(FLOAT a) {
	return a >> SCALE;
}

FLOAT int2F(int a) {
	return a << SCALE;
}

FLOAT F_mul_int(FLOAT a, int b) {
	return a * b;
}

FLOAT F_div_int(FLOAT a, int b) {
	return a / b;
}

