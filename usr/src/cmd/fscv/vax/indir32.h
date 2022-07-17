/* @(#)indir32.h	2.1		*/
#include <sys/param.h>
/* This structure defines an indirect block on a 32 bit VAX */
	struct ind32 {
#if m68k || MC68
	long b32_blk[BSIZE/4];
#else
 	short b32_num;
 	short fill1;
 	short b32_blk[254];
#endif
	};
