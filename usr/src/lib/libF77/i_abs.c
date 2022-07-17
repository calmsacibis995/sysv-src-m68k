/*	@(#)i_abs.c	2.1 	*/
long int i_abs(x)
long int *x;
{
if(*x >= 0)
	return(*x);
return(- *x);
}
