/*	@(#)derfc_.c	2.1 	*/
double derfc_(x)
double *x;
{
double erfc();

return( erfc(*x) );
}
