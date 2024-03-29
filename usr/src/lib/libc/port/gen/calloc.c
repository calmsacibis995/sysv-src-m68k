/*	@(#)calloc.c	2.1 	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*	calloc - allocate and clear memory block
*/
#define CHARPERINT (sizeof(int)/sizeof(char))
#define NULL 0

extern char *malloc();
extern void free();

char *
calloc(num, size)
unsigned num, size;
{
	register char *mp;
	register int *q;
	register m;

	num *= size;
	mp = malloc(num);
	if(mp == NULL)
		return(NULL);
	q = (int*)mp;
	m = (num+CHARPERINT-1)/CHARPERINT;
	while(--m >= 0)
		*q++ = 0;
	return(mp);
}

/*ARGSUSED*/
void
cfree(p, num, size)
char *p;
unsigned num, size;
{
	free(p);
}
