/*	@(#)strrchr.c	2.1 	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
*/

#define NULL 0

char *
strrchr(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;
	do {
		if(*sp == c)
			r = sp;
	} while(*sp++);
	return(r);
}
