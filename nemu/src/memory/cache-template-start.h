#define get_block(addr) (addr & ADDR_BLOCK_PART)
#define get_group_idx(addr) ((addr & ADDR_GROUP_PART) >> ADDR_BLOCK_LEN)
#define get_tag(addr) ((addr & ADDR_TAG_PART) >> (ADDR_GROUP_LEN + ADDR_BLOCK_LEN))

