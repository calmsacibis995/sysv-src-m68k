/*	@(#)h_dim.c	2.1 	*/
short h_dim(a,b)
short *a, *b;
{
return( *a > *b ? *a - *b : 0);
}
