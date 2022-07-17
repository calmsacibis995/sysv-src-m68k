/*	@(#)r_dim.c	2.1 	*/
double r_dim(a,b)
float *a, *b;
{
return( *a > *b ? *a - *b : 0);
}
