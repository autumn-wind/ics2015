#ifndef __CACHE_H__
#define __CACHE_H__

uint32_t level_1_cache_read(hwaddr_t, size_t);
uint32_t level_2_cache_read(hwaddr_t, size_t);

void level_1_cache_write(hwaddr_t, size_t, uint32_t);
void level_2_cache_write(hwaddr_t, size_t, uint32_t);

void level_1_cache_init(void);
void level_2_cache_init(void);
#endif
