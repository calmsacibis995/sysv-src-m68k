/*	"@(#)move.c	2.1	"	*/

move(from, to, count)
register char *from, *to;
register int count;
{
	while ((count--) > 0)
		*to++ = *from++;
}
