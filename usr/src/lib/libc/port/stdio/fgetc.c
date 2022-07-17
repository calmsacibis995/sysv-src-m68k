/*	@(#)fgetc.c	2.1 	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <stdio.h>

int
fgetc(fp)
register FILE *fp;
{
	return(getc(fp));
}
