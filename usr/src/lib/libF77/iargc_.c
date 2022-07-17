/*	@(#)iargc_.c	2.1 	*/
long int iargc_()
{
extern int xargc;
return ( xargc - 1 );
}
