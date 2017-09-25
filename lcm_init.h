/*  routines used LCM algorithms */
/* 2005/8/18 Takeaki Uno */
/* This program is available for only academic use.
   Neither commercial use, modification, nor re-distribution is allowed */

#ifndef _lcm_init_h_
#define _lcm_init_h_

int LCM_BM_space;
int *LCM_BM_weight;
char *LCM_BM_highbit;
int LCM_BM_MAXITEM, LCM_BM_prefix_max;
int *LCM_BM_prefix;
int LCM_BITMAP;
int LCM_SIG_ON_TRSACT;
int LCM_iters2, LCM_iters3;

int BITMASK_UPPER1[32] = { 0xffffffff, 0xfffffffe, 0xfffffffc, 0xfffffff8,
                           0xfffffff0, 0xffffffe0, 0xffffffc0, 0xffffff80,
                           0xffffff00, 0xfffffe00, 0xfffffc00, 0xfffff800,
                           0xfffff000, 0xffffe000, 0xffffc000, 0xffff8000,
                           0xffff0000, 0xfffe0000, 0xfffc0000, 0xfff80000,
                           0xfff00000, 0xffe00000, 0xffc00000, 0xff800000,
                           0xff000000, 0xfe000000, 0xfc000000, 0xf8000000,
                           0xf0000000, 0xe0000000, 0xc0000000, 0x80000000 };
int BITMASK_UPPER1_[32] = { 0xfffffffe, 0xfffffffc, 0xfffffff8, 0xfffffff0,
                            0xffffffe0, 0xffffffc0, 0xffffff80, 0xffffff00,
                            0xfffffe00, 0xfffffc00, 0xfffff800, 0xfffff000,
                            0xffffe000, 0xffffc000, 0xffff8000, 0xffff0000,
                            0xfffe0000, 0xfffc0000, 0xfff80000, 0xfff00000,
                            0xffe00000, 0xffc00000, 0xff800000, 0xff000000,
                            0xfe000000, 0xfc000000, 0xf8000000, 0xf0000000,
                            0xe0000000, 0xc0000000, 0x80000000, 0x00000000 };

int BITMASK_LOWER1[32] = { 0x00000000, 0x00000001, 0x00000003, 0x00000007,
                           0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
                           0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
                           0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
                           0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
                           0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
                           0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
                           0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff };
int BITMASK_LOWER1_[32] = { 0x00000001, 0x00000003, 0x00000007, 0x0000000f,
                            0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
                            0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
                            0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
                            0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
                            0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
                            0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
                            0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff };

int BITMASK_1[32] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008,
                      0x00000010, 0x00000020, 0x00000040, 0x00000080,
                      0x00000100, 0x00000200, 0x00000400, 0x00000800,
                      0x00001000, 0x00002000, 0x00004000, 0x00008000,
                      0x00010000, 0x00020000, 0x00040000, 0x00080000,
                      0x00100000, 0x00200000, 0x00400000, 0x00800000,
                      0x01000000, 0x02000000, 0x04000000, 0x08000000,
                      0x10000000, 0x20000000, 0x40000000, 0x80000000 };
int BITMASK_31[32] = { 0xfffffffe, 0xfffffffd, 0xfffffffb, 0xfffffff7,
                       0xffffffef, 0xffffffdf, 0xffffffbf, 0xffffff7f,
                       0xfffffeff, 0xfffffdff, 0xfffffbff, 0xfffff7ff,
                       0xffffefff, 0xffffdfff, 0xffffbfff, 0xffff7fff,
                       0xfffeffff, 0xfffdffff, 0xfffbffff, 0xfff7ffff,
                       0xffefffff, 0xffdfffff, 0xffbfffff, 0xff7fffff,
                       0xfeffffff, 0xfdffffff, 0xfbffffff, 0xf7ffffff,
                       0xefffffff, 0xdfffffff, 0xbfffffff, 0x7fffffff };


time_t LCM_start_time;
int LCM_maximum_time ;
int LCM_iters, LCM_pruned_iters, LCM_prepruned_iters;
char *LCM_input_file, *LCM_output_file, *LCM_weight_file;
FILE *LCM_fp;

/**  for shrink **/
int *LCM_shrink_p;   /* working space for shrinking */
QUEUE LCM_shrink_jump;  /* working space for shrinking */
QUEUE LCM_shrink_jump1, LCM_shrink_jump2;  /* working space for shrinking */
int *LCM_shrink_result, **LCM_shrink_shift;
int LCM_sig_siz;

int *LCM_perm, LCM_perm2[32], *LCM_trsact_perm;
int LCM_Eend, LCM_frq, *LCM_maxfreq;
int LCM_th, LCM_upth, LCM_itemset_ub, LCM_itemset_lb;
QUEUE LCM_jump, LCM_add, LCM_itemset, *LCM_added;
QUEUE_INT *LCM_buf, *LCM_buf2;
ARY LCM_Trsact, LCM_Occ; /* transactions, and denotations */

int LCM_Trsact_itemnum; /* #items in the given not shrinked database */
int LCM_Trsact_num, LCM_Trsact_weight;  /* #transactions in the given not shrinked database */
int LCM_database_siz;  /* total #elements in the given not shrinked database */
int LCM_maxTrsact_siz, LCM_maxTrsact_siz_org;  /* max. size of transactions */
int LCM_shrink_maxocc, LCM_shrink_maxbuf;
int LCM_buf_siz;

QUEUE *LCM_occ; /* LCM_occ := LCM_Occ.h */
QUEUE *LCM_trsact; /* LCM_trsact := LCM_Trsact.h */
int *LCM_sc;    /* # of itemsets of size i, and max.size of itemsets */
  /* print_flag&1:output to file &2:print messages &4 print solutions */
int LCM_problem, LCM_full_flag;

int *LCMV_trsact;

#ifdef LCM_MESSAGES
int LCM_print_flag=2;
#else 
int LCM_print_flag=0;
#endif

int FILE_err;

#endif
