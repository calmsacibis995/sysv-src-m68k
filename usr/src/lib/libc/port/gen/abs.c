/*	@(#)abs.c	2.1 	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/

int
abs(arg)
register int arg;
{
	return (arg >= 0 ? arg : -arg);
}
