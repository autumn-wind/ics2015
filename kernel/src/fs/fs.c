#include "common.h"

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
const file_info file_table[] = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
	{"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
	{"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
	{"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
	{"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
	{"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
	{"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
	{"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
	{"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
	{"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
	{"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
	{"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
	{"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);
void serial_printc(char);

/* TODO: implement a simplified file system here. */

typedef struct {
	bool opened;
	uint32_t offset;
} Fstate;

Fstate file_state[NR_FILES + 3];

bool strcmp(const char *dst, const char *src) {
	if(dst == NULL || src == NULL) {
		return false;
	}
	while(*dst != '\0') {
		if(*dst++ == *src++)
			continue;
		else
			return false;
	}
	return *src == '\0';
}

int fs_open(const char *pathname, int flags) {
	assert(pathname);
	int i;
	for(i = 0; i < NR_FILES; ++i) {
		if(strcmp(pathname, file_table[i].name)) {
			file_state[i + 3].opened = true;
			file_state[i + 3].offset = 0; //also equals to number of bytes have been read or written
			return i + 3;
		}
	}
	nemu_assert(0);
	return -1;
}

int fs_read(int fd, void *buf, int len) {
	nemu_assert(fd > 2);
	Fstate *fstat = &file_state[fd];
	nemu_assert(fstat->opened);
	const file_info *file = &file_table[fd - 3];
	int remain_byte = file->size - fstat->offset;
	int real_count = len < remain_byte ? len : remain_byte;
	if(real_count > 0) {
		ide_read((uint8_t *)buf, file->disk_offset + fstat->offset, real_count);
		fstat->offset += real_count;
	}
	return real_count > 0 ? real_count : 0;
}

int fs_write(int fd, void *buf, int len) {
	assert(fd != 0);
	if (fd == 1 || fd == 2) {
		/*write to serial*/
		int i;
		for(i = 0; i < len; ++i) {
			serial_printc(*((char *)buf + i));
		}
		return len;
	}
	else {
		/*write to disk*/
		assert(fd > 2);
		Fstate *fstat = &file_state[fd];
		nemu_assert(fstat->opened);
		const file_info *file = &file_table[fd - 3];
		int remain_byte = file->size - fstat->offset;
		int real_count = len < remain_byte ? len : remain_byte;
		if(real_count > 0) {
			ide_write((uint8_t *)buf, file->disk_offset + fstat->offset, real_count);
			fstat->offset += real_count;
		}
		return real_count > 0 ? real_count : 0;
	}
}

int fs_lseek(int fd, int offset, int whence) {
	nemu_assert(fd > 2);
	Fstate *fstat = &file_state[fd];
	nemu_assert(fstat->opened);
	const file_info *file = &file_table[fd - 3];
	switch(whence) {
		case SEEK_SET:
			fstat->offset = offset; break;
		case SEEK_CUR:
			fstat->offset += offset; break;
		case SEEK_END:
			fstat->offset = file->size - 1 + offset; break;
		default:
			break;
	}
	/*Log("file offset: %d, file size: %d\n", fstat->offset, file->size);*/
	assert(fstat->offset < file->size);
	return fstat->offset;
}

int fs_close(int fd) {
	assert(fd > 2);
	Fstate *fstat = &file_state[fd];
	fstat->opened = false;
	fstat->offset = 0;
	return 0;
}


