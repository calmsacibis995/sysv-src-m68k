/*	@(#)fmod.c	2.1.1.1		*/
/*	@(#)fmod.c	1.8	*/
/*LINTLIBRARY*/
/*
 *	fmod(x, y) returns the remainder of x on division by y,
 *	with the same sign as x,
 *	except that if |y| << |x|, it returns 0.
 */

#include <math.h>

double
fmod(x, y)
register double x, y;
{
	double d; /* can't be in register because of modf() below */

	/*
	 * The next lines determine if |y| is negligible compared to |x|,
	 * without dividing, and without adding values of the same sign.
	 */
	if( y==0.0 ) return( x );	/* fmod(x,0.0)==x */
	d = _ABS(x);
	if (d - _ABS(y) == d)
		return (0.0);
#if !defined(pdp11) && !defined(u3b5) /* pdp11 and u3b5 "cc" cant handle
					cast of a double to a void	*/
	(void)
#endif
	modf(x/y, &d); /* now it's safe to divide without overflow */
	return (x - d * y);
}
