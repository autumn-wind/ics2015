#include "FLOAT.h"

#define GET_SIGN(x) ((x & 0x80000000) >> 31)
#define GET_E(x) ((x & 0x7F800000) >> 23)
#define SET_E(x) ((x & 0x007FFFFF) | 0x00800000)

__attribute__((regparm(0))) FLOAT f2F(float a) {
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
	long long r = a * b;
	return r >> SCALE;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	FLOAT r = a / b;
	return r << SCALE;
}

FLOAT Fabs(FLOAT a) {
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

