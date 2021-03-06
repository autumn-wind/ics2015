#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "trap.h"

#define SCALE 16

typedef int FLOAT;

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT sqrt(FLOAT);
FLOAT pow(FLOAT, FLOAT);

int F2int(FLOAT);
FLOAT int2F(int);
FLOAT F_mul_int(FLOAT, int);
FLOAT F_div_int(FLOAT, int);

//static inline int F2int(FLOAT a) {
	//return a >> SCALE;
//}

//static inline FLOAT int2F(int a) {
	//return a << SCALE;
//}

//static inline FLOAT F_mul_int(FLOAT a, int b) {
	//return a * b;
//}

//static inline FLOAT F_div_int(FLOAT a, int b) {
	//return a / b;
//}

#endif
