/*	@(#)tell.c	2.1 	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * return offset in file.
 */

extern long lseek();

long
tell(f)
int	f;
{
	return(lseek(f, 0L, 1));
}
