/* @(#)indfv.c	2.1		*/
indfv(fdo,fblock)
long *fblock;
int fdo;
/* Fblock is first block to be read */
{
	/*
		 * This  subroutine reads a free list indirect block number
		 * "block" in MC68000 format and converts it to VAX format.
		 */

#include <stdio.h>
#if m68k || MC68
#include <sys/param.h>
#endif

	int i,in,out;
	long block;

#if m68k || MC68
#include "indir68.h"
#else
#include "indir16.h"
#endif
#include "indir32.h"

#if m68k || MC68
	static struct ind68 i68;
#else
 	static struct ind16 i16;
#endif
	static struct ind32 i32;

	/* Get first block */
	block = *fblock;


	while (block != 0) {

		/* Position file to desired block */
#if m68k || MC68
		lseek(fdo,block*BSIZE,0);
#else
 		lseek(fdo,block*512,0);
#endif

		/* Read in block and convert */
#if m68k || MC68
		in = read(fdo,&i68,BSIZE);
#else
 		in = read(fdo,&i16,512);
#endif
		if (in <= 0) {
			perror("Error reading free list");
			return(1);
		};

#if m68k || MC68
		/*
		 *	The extra one in the for statement below
		 *	reflects the need to assign over the count
		 *	in the first long of the indirect block.
		 *	Remember, int's on the MC68000 and the VAX
		 *	are the same size ... but need to be converted.
		 */

		for ( i = 0; i < 50+1; i++) {
			i32.b32_blk[i] = i68.b68_blk[i];
			swapl(&i32.b32_blk[i]);
		};
#else
 		i32.b32_num = i16.b16_num;

 		for ( i = 0; i < 50; i++) {
 			i32.b32_blk[2*i] = i16.b16_blk[2*i+1];
 			i32.b32_blk[2*i+1] = i16.b16_blk[2*i];
		};
#endif

		/* Write out indirect block */
#if m68k || MC68
		lseek(fdo,block*BSIZE,0);
#else
 		lseek(fdo,block*512,0);
#endif
		out = write(fdo,&i32,in);
		if (out != in ) {
			perror("Error writing free list");
			return(1);
		};

		/*
		 * Set "block" to long integer for next time through loop.
		 * This requires knowledge on the machine on which we are running
		 * since both formats are available.
		 */
#ifdef m68k
		block = i68.b68_blk[1] ;
#else
		block = *(long *)&i32.b32_blk[0] ;
#endif

	};
	return(i);
}
