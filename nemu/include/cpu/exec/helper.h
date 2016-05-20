#ifndef __EXEC_HELPER_H__
#define __EXEC_HELPER_H__

#include "cpu/helper.h"
#include "cpu/decode/decode.h"

#define make_helper_v(name) \
	make_helper(concat(name, _v)) { \
		return (ops_decoded.is_data_size_16 ? concat(name, _w) : concat(name, _l)) (eip); \
	}

#define do_execute concat4(do_, instr, _, SUFFIX)

#define make_instr_helper(type) \
	make_helper(concat5(instr, _, type, _, SUFFIX)) { \
		return idex(eip, concat4(decode_, type, _, SUFFIX), do_execute); \
	}

extern char assembly[];
#ifdef DEBUG
#define print_asm(...) Assert(snprintf(assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif

#define print_asm_template1() \
	print_asm(str(instr) str(SUFFIX) " %s", op_src->str)

#define print_asm_template2() \
	print_asm(str(instr) str(SUFFIX) " %s,%s", op_src->str, op_dest->str)

#define print_asm_template3() \
	print_asm(str(instr) str(SUFFIX) " %s,%s,%s", op_src->str, op_src2->str, op_dest->str)

static inline void set_CF(unsigned long long result, uint8_t data_size) {
	//unsigned long long cb = result & (0x1LL << (data_size * 8));
	//printf("%lld\n", cb);
	if(result & (0x1LL << (data_size * 8))) {
		cpu.CF = 1;
	}
	else {
		cpu.CF = 0;
	}
}

static inline void set_ZF(unsigned long long result) {
	if(result == 0LL) {
		cpu.ZF = 1;
	}
	else {
		cpu.ZF = 0;
	}
}

static inline void set_OF(unsigned long long left, unsigned long long right, unsigned long long result, uint8_t data_size, uint8_t add_or_sub) {
	uint8_t shift_num = data_size * 8 - 1;
	uint8_t left_sign = (left & (1LL << shift_num)) >> shift_num;
	uint8_t right_sign = (right & (1LL << shift_num)) >> shift_num;
	uint8_t result_sign = (result & (1LL << shift_num)) >> shift_num;
	//sub
	if(add_or_sub == 1) {
		if((left_sign == 1 && right_sign == 0 && result_sign == 0) || (left_sign == 0 && right_sign == 1 && result_sign == 1)) {
			cpu.OF = 1;
		}
		else {
			cpu.OF = 0;
		}
	}
	//add
	else {
		if((left_sign == 0 && right_sign == 0 && result_sign == 1) || (left_sign == 1 && right_sign == 1 && result_sign == 0)) {
			cpu.OF = 1;
		}
		else {
			cpu.OF = 0;
		}
	}
}

static inline void set_SF(unsigned long long result, uint8_t data_size) {
	uint8_t shift_num = data_size * 8 - 1;
	cpu.SF = (result & (1LL << shift_num)) >> shift_num;
}

//#define HIGH_ORDER_BIT 0x80000000
//static inline void set_CF(uint32_t left, uint32_t right, uint8_t add_or_sub) {
	////[> sub <]
	//if(add_or_sub == 0) {
		//if(left < right)
			//cpu.CF = 1;
		//else
			//cpu.CF = 0;
	//}
	////[> add <]
	//else if(add_or_sub == 1) {
		//uint32_t result = left + right;
		//union {
			//struct {
				//uint8_t left	:1;
				//uint8_t right	:1;
				//uint8_t result	:1;
			//};
			//uint8_t val;
		//} high_bit;
		//high_bit.val = 0;
		//high_bit.left = (left & HIGH_ORDER_BIT) >> 31;
		//high_bit.right = (right & HIGH_ORDER_BIT) >> 31;
		//high_bit.result = (result & HIGH_ORDER_BIT) >> 31;
		//if((high_bit.left == 1 && high_bit.right == 1) || (((high_bit.left ^ high_bit.right) == 1) && high_bit.result == 1)) {
			//cpu.CF = 1;
		//}
		//else {
			//cpu.CF = 0;
		//}
	//}
	//else {
		//assert(0);
	//}
//}
//#undef HIGH_ORDER_BIT


#endif

