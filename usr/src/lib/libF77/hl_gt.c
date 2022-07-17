/*	@(#)hl_gt.c	2.1 	*/
short l_gt(a,b,la,lb)
char *a, *b;
long int la, lb;
{
return(s_cmp(a,b,la,lb) > 0);
}
