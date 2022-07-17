#if m68k || MC68
/* @(#)indfm.c	2.1		*/
indfm(fdo,fblock)
long *fblock;
int fdo;
/* Fblock is first block to be read */
{
	/*
		 * This  subroutine reads a free list indirect block number
		 * "block" in VAX format and converts it to MC68000 format.
		 */

#include <stdio.h>
#include <sys/param.h>

	int i,in,out;
	long block;

#include "indir68.h"
#include "indir32.h"

	static struct ind68 i68;
	static struct ind32 i32;

	/* Get first block */
	block = *fblock;

	while (block != 0) {

		/* Position file to desired block */
		lseek(fdo,block*BSIZE,0);

		/* Read in block and convert */
		in = read(fdo,&i32,BSIZE);
		if (in <= 0) {
			perror("Error reading free list");
			return(1);
		};

		/*
		 *	The extra one in the for statement below
		 *	reflects the need to assign over the count
		 *	in the first long of the indirect block.
		 *	Remember, int's on the MC68000 and the VAX
		 *	are the same size ... but need to be converted.
		 */

		for ( i = 0; i < 50+1; i++) {
			i68.b68_blk[i] = i32.b32_blk[i];
			swapl(&i68.b68_blk[i]);
		};

		/* Write out indirect block */
		lseek(fdo,block*BSIZE,0);
		out = write(fdo,&i68,in);
		if (out != in ) {
			perror("Error writing free list");
			return(1);
		};

		/*
		 * This requires knowledge of the machine on which we are running
		 * since both formats are available.
		 * Step down the free list.
		 * Block one is needed since block zero is the free count.
		 */
#ifdef vax
		block = i32.b32_blk[1] ;
#else
		block = i68.b68_blk[1] ;
#endif

	};
	return(i);
}
#endif
