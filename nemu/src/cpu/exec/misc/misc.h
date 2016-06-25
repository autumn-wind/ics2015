#ifndef __MISC_H__
#define __MISC_H__

make_helper(nop);
make_helper(int3);
make_helper(lea);
make_helper(cld);
make_helper(lgdt);
make_helper(mov_from_cr0);
make_helper(mov_to_cr0);
make_helper(ljmp);
make_helper(std);

#endif
