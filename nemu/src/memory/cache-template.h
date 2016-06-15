#include "cache-template-start.h"
//#if CACHE_SIZE == CACHE_GROUP_NUM * CACHE_SET_NUM * CACHE_BLOCK_SIZE

/*cache line*/
typedef struct concat3(LEVEL_, CACHE_LEVEL, _CACHE_SET) {
	uint8_t block[CACHE_BLOCK_SIZE];
	struct {
		uint32_t tag	:ADDR_TAG_LEN;
		uint32_t valid	:1;
#ifdef WRITE_BACK
		uint32_t dirty	:1;
#endif
	};
}concat3(LEVEL_, CACHE_LEVEL, _CACHE_SET);

/*cache group*/
typedef struct concat3(LEVEL_, CACHE_LEVEL, _CACHE_GROUP) {
	concat3(LEVEL_, CACHE_LEVEL, _CACHE_SET) line[CACHE_SET_NUM];
	uint8_t free_set_num;
}concat3(LEVEL_, CACHE_LEVEL, _CACHE_GROUP);

concat3(LEVEL_, CACHE_LEVEL, _CACHE_GROUP) concat3(level_, CACHE_LEVEL, _cache)[CACHE_GROUP_NUM];

/*access cache, return cache line pointer if hit or NULL if miss*/
concat3(LEVEL_, CACHE_LEVEL, _CACHE_SET)* concat3(level_, CACHE_LEVEL, _hit)(concat3(LEVEL_, CACHE_LEVEL, _CACHE_GROUP) *pGroup, hwaddr_t addr) {
	assert(pGroup != NULL);
	int i;
	for(i = 0; i < CACHE_SET_NUM; ++i) {
		if((pGroup->line)[i].valid) {
			if((pGroup->line)[i].tag == get_tag(addr)) {
				return &(pGroup->line)[i];
			}
		}
	}
	return NULL;
}

/*read from next level cache*/
void concat3(level_, CACHE_LEVEL, _read_next_level_cache)(hwaddr_t addr, size_t len,concat3(LEVEL_, CACHE_LEVEL, _CACHE_GROUP) *pGroup ,uint32_t (*p_next_level_cache_read)(hwaddr_t, size_t), void (*p_next_level_cache_write)(hwaddr_t, size_t, uint32_t)) {
	uint32_t block_start_addr = addr & (~(uint32_t)(CACHE_BLOCK_SIZE - 1));
	uint32_t new_block[CACHE_BLOCK_SIZE / 4];
	int i;
	/*read from next level*/
	for(i = 0; i < CACHE_BLOCK_SIZE / 4; ++i)
		new_block[i] = p_next_level_cache_read(block_start_addr + i * 4, 4);
	/*find empty line to load newly read block*/
	for(i = 0; i < CACHE_SET_NUM; ++i) {
		if((pGroup->line)[i].valid == 0)
			break;
	}
	/*empty line found, load directly*/
	if(i < CACHE_SET_NUM) {
		memcpy((pGroup->line)[i].block, (uint8_t *)new_block, CACHE_BLOCK_SIZE);
		(pGroup->line)[i].valid = 1;
		(pGroup->line)[i].tag = get_tag(addr);
	}
	/*no empty line, must replace one*/
	else {
		srand((uint32_t)time(NULL));
		uint32_t random_line = rand() & (CACHE_SET_NUM - 1);
#ifdef WRITE_THROUGH
		memcpy((pGroup->line)[random_line].block, (uint8_t *)new_block, CACHE_BLOCK_SIZE);
		(pGroup->line)[random_line].valid = 1;
		(pGroup->line)[random_line].tag = get_tag(addr);
#else

#ifdef WRITE_BACK
		/*cache line is not dirty, drop dirty*/
		if((pGroup->line)[random_line].dirty == 0) {
			memcpy((pGroup->line)[random_line].block, (uint8_t *)new_block, CACHE_BLOCK_SIZE);
		}
		/*cache line is dirty, write to low level cache first*/
		else {
			uint32_t dirty_block_start_addr = ((pGroup->line)[random_line].tag << (ADDR_BLOCK_LEN + ADDR_GROUP_LEN)) | (get_group_idx(addr) << ADDR_BLOCK_LEN);
			uint32_t *temp = (uint32_t *)((pGroup->line)[random_line].block);
			for(i = 0; i < CACHE_BLOCK_SIZE / 4; ++i) 
				p_next_level_cache_write(dirty_block_start_addr + i * 4, 4, temp[i]);
			memcpy((pGroup->line)[random_line].block, (uint8_t *)new_block, CACHE_BLOCK_SIZE);
		}
		(pGroup->line)[random_line].valid = 1;
		(pGroup->line)[random_line].tag = get_tag(addr);
		(pGroup->line)[random_line].dirty = 0;
#endif

#endif
	}

}

/*read from cache*/
uint32_t concat3(level_, CACHE_LEVEL, _cache_read)(hwaddr_t addr, size_t len) {
	/*get cache group*/
	uint32_t group_idx = get_group_idx(addr);
	assert(group_idx < CACHE_GROUP_NUM);
	concat3(LEVEL_, CACHE_LEVEL, _CACHE_GROUP) *pGroup = &concat3(level_, CACHE_LEVEL, _cache)[group_idx];


	concat3(LEVEL_, CACHE_LEVEL, _CACHE_SET) *pSet = concat3(level_, CACHE_LEVEL, _hit)(pGroup, addr);
	/*cache hit*/
	if(pSet != NULL) {
		uint32_t offset_in_block = get_block(addr);
		assert(offset_in_block < CACHE_BLOCK_SIZE);
		uint32_t remain_block_len = CACHE_BLOCK_SIZE - offset_in_block;
		if(len <= remain_block_len) {
			/*data in a block*/
			return unalign_rw(pSet->block + offset_in_block, 4) & (~0u >> ((4 - len) << 3));
		}
		else {
			/*data cross cache boundary*/
			uint32_t low_part = unalign_rw(pSet->block + offset_in_block, 4) & (~0u >> ((4 - remain_block_len) << 3));
			uint32_t high_part_len = len - remain_block_len;
			uint32_t high_part = hwaddr_read(addr + remain_block_len, high_part_len);
			return low_part | (high_part << (remain_block_len << 3));
		}
	}
	/*cache miss*/
	else {
		uint32_t (*p_next_level_cache_read)(hwaddr_t, size_t) = NULL;
		void (*p_next_level_cache_write)(hwaddr_t, size_t, uint32_t) = NULL;
		switch(CACHE_LEVEL) {
			case 1: p_next_level_cache_read = level_2_cache_read;
					p_next_level_cache_write = level_2_cache_write;
					break;
			case 2: p_next_level_cache_read = dram_read;
					p_next_level_cache_write = dram_write;
					break;
			default: assert(0);break;
		}
		concat3(level_, CACHE_LEVEL, _read_next_level_cache)(addr, len, pGroup, p_next_level_cache_read, p_next_level_cache_write);
		return concat3(level_, CACHE_LEVEL, _cache_read)(addr, len);
		/*return p_next_level_cache_read(addr, len);*/
	}
}

//#else

//#error wrong CACHE_LEVEL config

//#endif

#include "cache-template-end.h"

