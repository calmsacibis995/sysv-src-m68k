/*	@(#)math.h	2.1.1.1		*/
/*	@(#)math.h	1.3	UNIX System V/68	*/
/*	@(#) math.h:	2.3 83/07/08	*/
#ifndef _POLY9
extern int errno, signgam;

extern double atof(), frexp(), ldexp(), modf();
extern double j0(), j1(), jn(), y0(), y1(), yn();
extern double erf(), erfc();
extern double exp(), log(), log10(), pow(), sqrt();
extern double floor(), ceil(), fmod(), fabs();
extern double gamma();
extern double hypot();
extern int matherr();
extern double sinh(), cosh(), tanh();
extern double sin(), cos(), tan(), asin(), acos(), atan(), atan2();

/* some useful constants */
#define M_E	2.7182818284590452354
#define M_LOG2E	1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2	0.69314718055994530942
#define M_LN10	2.30258509299404568402
#define M_PI	3.14159265358979323846
#define M_PI_2	1.57079632679489661923
#define M_PI_4	0.78539816339744830962
#define M_1_PI	0.31830988618379067154
#define M_2_PI	0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2	1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#if u3b || u3b5
#define MAXFLOAT	((float)3.40282346638528860e+38)
#endif
#if m68k
/* This code is an ugly *** C R O C K *** to get around the problems
** of cross-compiling float/double constants.  The constant in question
** is the maximum float/double value.  Since the VAX has a smaller range
** than the 68000, it cannot convert MAXFLOAT using the native atof().
** What we do here is define a local constant value whose bits are the
** right ones for the 68000.
*/

#ifndef	__VALUE_H__
    extern double _MaXdOuB, _MiNdOuB;
    extern float _MaXfLoA, _MiNfLoA;
    asm("	data 1");
    asm("	even");
    asm("_MaXdOuB:");
    asm("	long 0x7fefffff");
    asm("	long 0xffffffff");
    asm("_MaXfLoA:");
    asm("	long 0x7f7fffff");
    asm("_MiNdOuB:");
    asm("	long 0x00100000");
    asm("	long 0x0");
    asm("_MiNfLoA:");
    asm("	long 0x00800000");
#define __VALUE_H__		/* just do it once */
#endif
#define	MAXFLOAT	_MaXfLoA
#endif
#if pdp11 || vax
#define MAXFLOAT	((float)1.701411733192644299e+38)
#endif
#if gcos
#define MAXFLOAT	((float)1.7014118219281863150e+38)
#endif
#define HUGE	MAXFLOAT

#define _ABS(x)	((x) < 0 ? -(x) : (x))
#define _REDUCE(TYPE, X, XN, C1, C2)	{ \
	double x1 = (double)(TYPE)X, x2 = X - x1; \
	X = x1 - (XN) * (C1); X += x2; X -= (XN) * (C2); }
#define _POLY1(x, c)	((c)[0] * (x) + (c)[1])
#define _POLY2(x, c)	(_POLY1((x), (c)) * (x) + (c)[2])
#define _POLY3(x, c)	(_POLY2((x), (c)) * (x) + (c)[3])
#define _POLY4(x, c)	(_POLY3((x), (c)) * (x) + (c)[4])
#define _POLY5(x, c)	(_POLY4((x), (c)) * (x) + (c)[5])
#define _POLY6(x, c)	(_POLY5((x), (c)) * (x) + (c)[6])
#define _POLY7(x, c)	(_POLY6((x), (c)) * (x) + (c)[7])
#define _POLY8(x, c)	(_POLY7((x), (c)) * (x) + (c)[8])
#define _POLY9(x, c)	(_POLY8((x), (c)) * (x) + (c)[9])

struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

#define DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6
#endif
