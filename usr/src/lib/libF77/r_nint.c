/*	@(#)r_nint.c	2.1 	*/
double r_nint(x)
float *x;
{
double floor();

return( (*x)>=0 ?
	floor(*x + .5) : -floor(.5 - *x) );
}
