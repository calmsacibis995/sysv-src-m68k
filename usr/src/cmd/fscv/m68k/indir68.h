/* @(#)indir68.h	2.1		*/
#include <sys/param.h>
/* This structure defines an indirect block on a 32 bit MC68000 */
	struct ind68 {
	long b68_blk[BSIZE/4];
	};
