/*	@(#)h_abs.c	2.1 	*/
short h_abs(x)
short *x;
{
if(*x >= 0)
	return(*x);
return(- *x);
}
