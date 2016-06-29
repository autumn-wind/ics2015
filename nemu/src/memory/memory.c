#include "common.h"
#include "cache.h"
#include "segtranslate.h"
#include "pagetranslate.h"
#include "device/mmio.h"

uint32_t dram_read(hwaddr_t, size_t); //read 4 bytes each time
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint64_t hwaddr_access_count = 0;

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	hwaddr_access_count += 1;

	int io_map_nr = is_mmio(addr);
	if(io_map_nr != -1) {
		/*map to I/O space*/
		return mmio_read(addr, len, io_map_nr);
	}
	else {
		/*map to dram*/
#if defined(USE_L1_CACHE) || defined(USE_L2_CACHE)
		return level_1_cache_read(addr, len);
#else
		return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
#endif
	}
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	hwaddr_access_count += 1;

	int io_map_nr = is_mmio(addr);
	if(io_map_nr != -1) {
		/*map to I/O space*/
		mmio_write(addr, len, data, io_map_nr);
	}
	else {
		/*map to dram*/
#if defined(USE_L1_CACHE) || defined(USE_L2_CACHE)
		level_1_cache_write(addr, len, data);
#else
		dram_write(addr, len, data);
#endif
	}
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	if ((addr & 0x00000FFF) + len > 4096) {
		/* this is a special case, you can handle it later. */
		size_t second_part_len = (addr & 0x00000FFF) + len - 4096;
		size_t first_part_len = len - second_part_len;
		if(first_part_len != 3 && second_part_len != 3) {
			uint32_t first_part = lnaddr_read(addr, first_part_len);
			uint32_t second_part = lnaddr_read(addr + first_part_len, second_part_len);
			return first_part | (second_part << (first_part_len << 3));
		}
		else {
			/* first_part_len == 3 or second_part_len == 3 */
			/*assert(0);*/
			uint32_t first_part = lnaddr_read(addr, 1);
			uint32_t middle_part = lnaddr_read(addr + 1, 2);
			uint32_t last_part = lnaddr_read(addr + 3, 1);
			return first_part | (middle_part << 8) | last_part << 24;
		}
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
	if ((addr & 0x00000FFF) + len > 4096) {
		/* this is a special case, you can handle it later. */
		size_t second_part_len = (addr & 0x00000FFF) + len - 4096;
		size_t first_part_len = len - second_part_len;
		if(first_part_len != 3 && second_part_len != 3) {
			uint32_t first_part = data & (~0u >> (second_part_len << 3));
			uint32_t second_part = data >> (first_part_len << 3);
			lnaddr_write(addr, first_part_len, first_part);
			lnaddr_write(addr + first_part_len, second_part_len, second_part);
		}
		else {
			/* first_part_len or second_part_len equals to 3 */
			assert(0);
			uint32_t data1 = data & 0x000000FF;
			uint32_t data2 = ( data & 0x00FFFF00 ) >> 8;
			uint32_t data3 = ( data &0xFF000000) >> 24;
			lnaddr_write(addr, 1, data1);
			lnaddr_write(addr + 1, 2, data2);
			lnaddr_write(addr + 3, 1, data3);
		}
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

