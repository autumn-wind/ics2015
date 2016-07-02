#ifndef __MOV_H__
#define __MOV_H__

make_helper(mov_i2r_b);
make_helper(mov_i2rm_b);
make_helper(mov_r2rm_b);
make_helper(mov_rm2r_b);
make_helper(mov_a2moffs_b);
make_helper(mov_moffs2a_b);

make_helper(mov_i2r_v);
make_helper(mov_i2rm_v);
make_helper(mov_r2rm_v);
make_helper(mov_rm2r_v);
make_helper(mov_a2moffs_v);
make_helper(mov_moffs2a_v);

make_helper(mov_rm2sreg_w);

make_helper(cmova_rm2r_v);
make_helper(cmove_rm2r_v);
make_helper(cmovs_rm2r_v);
make_helper(cmovg_rm2r_v);
make_helper(cmovge_rm2r_v);
make_helper(cmovbe_rm2r_v);
make_helper(cmovns_rm2r_v);
make_helper(cmovne_rm2r_v);

#endif
