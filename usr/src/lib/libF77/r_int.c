/*	@(#)r_int.c	2.1 	*/
double r_int(x)
float *x;
{
double floor();

return( (*x>0) ? floor(*x) : -floor(- *x) );
}
