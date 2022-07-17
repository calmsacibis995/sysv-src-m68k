/*	@(#)signal_.c	2.1 	*/
/*	3.0 SID #	1.2	*/
signal_(sigp, procp)
int *sigp, (**procp)();
{
return( signal(*sigp, *procp) );
}
