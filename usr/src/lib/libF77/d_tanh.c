/*	@(#)d_tanh.c	2.1 	*/
double d_tanh(x)
double *x;
{
double tanh();
return( tanh(*x) );
}
