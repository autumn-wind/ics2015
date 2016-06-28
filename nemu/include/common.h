#ifndef __COMMON_H__
#define __COMMON_H__

#define USE_RAMDISK
//#define USE_L1_CACHE
//#define USE_L2_CACHE //when defined, macro USE_L1_CACHE must be undefined

/* You will define this macro in PA4 */
//#define HAS_DEVICE

#define DEBUG
#define LOG_FILE

#include "debug.h"
#include "macro.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint8_t bool;
typedef uint32_t hwaddr_t;
typedef uint32_t lnaddr_t;
typedef uint32_t swaddr_t;

typedef uint16_t ioaddr_t;

#pragma pack (1)
typedef union {
	uint32_t _4;
	uint32_t _3	: 24;
	uint16_t _2;
	uint8_t _1;
} unalign;
#pragma pack ()

#define false 0
#define true 1
#define EXPR_LEN 64

/* the 64bit segment descriptor */
typedef struct SegmentDescriptor {
	uint32_t limit_15_0          : 16;
	uint32_t base_15_0           : 16;
	uint32_t base_23_16          : 8;
	uint32_t type                : 4;
	uint32_t segment_type        : 1;
	uint32_t privilege_level     : 2;
	uint32_t present             : 1;
	uint32_t limit_19_16         : 4;
	uint32_t soft_use            : 1;
	uint32_t operation_size      : 1;
	uint32_t pad0                : 1;
	uint32_t granularity         : 1;
	uint32_t base_31_24          : 8;
} SegDesc;

typedef struct GateDescriptor {
	uint32_t offset_15_0      : 16;
	uint32_t segment          : 16;
	uint32_t pad0             : 8;
	uint32_t type             : 4;
	uint32_t system           : 1;
	uint32_t privilege_level  : 2;
	uint32_t present          : 1;
	uint32_t offset_31_16     : 16;
} GateDesc;

#endif
