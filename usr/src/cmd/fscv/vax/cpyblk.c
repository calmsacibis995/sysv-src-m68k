/* @(#)cpyblk.c	2.1		*/
cpyblk(num,fdi,fdo)
int num,fdi,fdo;
{
	/*
	 * This subroutine copies "num" blocks from input file descriptor fdi
	 * to output file descriptor fdo.
	 */

#include <stdio.h>
#if m68k || MC68
#include <sys/param.h>
#endif
#include "params.h"

	int in,out;
#if m68k || MC68
	char buff[BUFNOS*BSIZE];
#else
 	char buff[BUFNOS*512];
#endif

	/* Make sure too many blocks not read */
	if (BUFNOS < num) {
		printf("cpyblk: Illegal number of blocks\n");
		return(-1);
	};

#if m68k || MC68
	in = read(fdi,buff,num*BSIZE);
#else
 	in = read(fdi,buff,num*512);
#endif
	if (in < 0) {
		perror("Error copying block from input file");
		return(in);
	};

	if (in > 0) {
		out = write(fdo,buff,in);
		if (in != out) {
			perror("Error copying block to output file");
			return(-1);
		};
	};
	return(in);
}
