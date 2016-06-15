#include "common.h"
#include <stdlib.h>
#include <time.h>

uint32_t level_1_cache_read(hwaddr_t, size_t);
uint32_t level_2_cache_read(hwaddr_t, size_t);
uint32_t dram_read(hwaddr_t, size_t);
uint32_t hwaddr_read(hwaddr_t, size_t);

void level_1_cache_write(hwaddr_t, size_t, uint32_t);
void level_2_cache_write(hwaddr_t, size_t, uint32_t);
void dram_write(hwaddr_t, size_t, uint32_t);
void hwaddr_write(hwaddr_t, size_t, uint32_t);

uint64_t time_consuming = 0;

#define CACHE_LEVEL		 1
#define CACHE_SIZE		 (64 * 1024)
#define CACHE_BLOCK_SIZE 64
#define CACHE_SET_NUM	 8
#define CACHE_GROUP_NUM	 256
#define WRITE_THROUGH
#define WRITE_NOT_ALLOC

#define ADDR_BLOCK_PART 0x0000003F
#define ADDR_GROUP_PART	0x00003FC0
#define ADDR_TAG_PART	0xFFFFC000

#define ADDR_BLOCK_LEN	6
#define ADDR_GROUP_LEN	8	
#define ADDR_TAG_LEN	18

#include "cache-template.h"

#undef CACHE_LEVEL
#undef CACHE_SIZE
#undef CACHE_BLOCK_SIZE
#undef CACHE_SET_NUM
#undef CACHE_GROUP_NUM
#undef WRITE_THROUGH
#undef WRITE_NOT_ALLOC

#undef ADDR_BLOCK_PART
#undef ADDR_GROUP_PART
#undef ADDR_TAG_PART

#undef ADDR_BLOCK_LEN
#undef ADDR_GROUP_LEN
#undef ADDR_TAG_LEN

#define CACHE_LEVEL		 2
#define CACHE_SIZE		 (4 * 1024 *1024)
#define CACHE_BLOCK_SIZE 64
#define CACHE_SET_NUM	 16
#define CACHE_GROUP_NUM	 (4 * 1024)
#define WRITE_BACK
#define WRITE_ALLOC

#define ADDR_BLOCK_PART 0x0000003F
#define ADDR_GROUP_PART	0x0003FFC0
#define ADDR_TAG_PART	0xFFFC0000

#define ADDR_BLOCK_LEN	6
#define ADDR_GROUP_LEN	12	
#define ADDR_TAG_LEN	14

#include "cache-template.h"

#undef CACHE_LEVEL
#undef CACHE_SIZE
#undef CACHE_BLOCK_SIZE
#undef CACHE_SET_NUM
#undef CACHE_GROUP_NUM
#undef WRITE_BACK
#undef WRITE_ALLOC

#undef ADDR_BLOCK_PART
#undef ADDR_GROUP_PART
#undef ADDR_TAG_PART

#undef ADDR_BLOCK_LEN
#undef ADDR_GROUP_LEN
#undef ADDR_TAG_LEN
