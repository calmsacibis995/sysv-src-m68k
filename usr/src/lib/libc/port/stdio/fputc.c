/*	@(#)fputc.c	2.1 	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <stdio.h>

int
fputc(c, fp)
int	c;
register FILE *fp;
{
	return(putc(c, fp));
}
