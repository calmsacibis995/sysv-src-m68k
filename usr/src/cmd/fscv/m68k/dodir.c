#if m68k || MC68
/*	@(#)dodir.c	2.1		*/
dodir( fdo, blk )
int		fdo;
long	blk;
/* blk is the block number of a directory entry block */
{
	/*
	 *	This subroutine reads a directory entry block and
	 *	makes the MC68000 <-> VAX conversions.
	 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>

	struct	direct	dbuf[BSIZE/sizeof(struct direct)];
	int		k;

	if( blk == 0 )		/* empty block */
		return;

	/* position file to desired block */
	lseek(fdo,blk*BSIZE,0);

	/* read data buffer */
	if( read(fdo,dbuf,BSIZE) != BSIZE ) {
		perror("Error reading directory block");
		return;
	}

	for( k = 0; k < BSIZE/sizeof(dbuf[0]); k++ )
		swaps(&dbuf[k].d_ino);

	/* position file to desired block */
	lseek(fdo,blk*BSIZE,0);

	/* write data buffer */
	if( write(fdo,dbuf,BSIZE) != BSIZE )
		perror("Error in directory block write");
}
#endif
