/*	@(#)l_lt.c	2.1 	*/
long int l_lt(a,b,la,lb)
char *a, *b;
long int la, lb;
{
return(s_cmp(a,b,la,lb) < 0);
}
