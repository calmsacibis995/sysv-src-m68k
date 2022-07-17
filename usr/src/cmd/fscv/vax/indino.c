/* @(#)indino.c	2.1		*/
#if m68k || MC68
indino(tovax,fdo,fsize,size,num,blk,dirent)
#else
indino(tovax,fdo,fsize,size,num,blk)
#endif

int tovax,fdo,num;
long fsize,blk;
long *size;
#if m68k || MC68
int dirent;
#endif

{
	/*
		 * This recursive subroutine converts the inode indirect blocks
		 * by swapping the two words on longs.
		 */


	/*
		 * tovax = 1 if converting to a vax format 
		 * fdo = output file descriptor
		 * num = current number being converted
		 *	if num = 0 single indirect block
		 *	if num = 1 double indirect block
		 *	if num = 2 tripple indirec block
		 * blk = block address of indirect block
		 * dirent = nonzero if the current file
		 *			is a directory (m68k)
		 */

#include <stdio.h>
#if m68k || MC68
#include <sys/param.h>
#endif

	/* local data */
#if m68k || MC68
	long buff[BSIZE/4];
#else
 	long buff[256];
#endif
	int in,out,i;

	/* Define a variable for running on VAX (simplifies code generation */
#ifdef vax
#define ONVAX 1
#else
#define ONVAX 0
#endif

	/* program */

	/* See if any more indirect blocks */
	/* num=0 corresponds to inode address 10 */
	if(num < 0 | *size > fsize) return(0);

	/* Add contribution of this indirect block if at level 0 */
#if m68k || MC68
	if(num == 0) *size += (long)BSIZE*BSIZE/4;
#else
 	if(num == 0) *size += (long)512*128;
#endif

	/* Just return if this is an empty indirect pointer */
	if(blk == 0) return(0);

	/* read in next indirect block */
#if m68k || MC68
	lseek(fdo,blk*BSIZE,0);
	in = read(fdo,buff,BSIZE);
#else
 	lseek(fdo,blk*512,0);
 	in = read(fdo,buff,512);
#endif
	if(in <= 0) {
		perror("Error reading inode indirect block");
		return(-1);
	}

	/* convert the inode */
#if m68k || MC68
	for (i = 0; i < BSIZE/4; i++) {
#else
 	for (i = 0; i < 256; i++) {
#endif

		/*
				 * The following code decides whether the conversion should be
				 * done before or after calling indino. The conversion will
				 * be done before calling indino if conversion is to the machine
				 * on which this program is running.
				 */
#if m68k || MC68
		if(ONVAX == tovax) swapl(&buff[i]);
		if((num == 0) && dirent)
			dodir(fdo,buff[i]);
		if((in = indino(tovax,fdo,fsize,size,num-1,buff[i],dirent)) < 0)
			return(in);
		if(ONVAX != tovax) swapl(&buff[i]);
#else
 		if(ONVAX == tovax) swap(&buff[i]);
 		if((in = indino(tovax,fdo,fsize,size,num-1,buff[i])) < 0) return(in);
 		if(ONVAX != tovax) swap(&buff[i]);
#endif
	}

	/* write out next indirect block */
#if m68k || MC68
	lseek(fdo,blk*BSIZE,0);
	out = write(fdo,buff,BSIZE);
	if(out != BSIZE) {
#else
 	lseek(fdo,blk*512,0);
 	out = write(fdo,buff,512);
 	if(out != 512) {
#endif
		perror("Error writing inode indirect block");
		return(-1);
	}

	return(0);
}
