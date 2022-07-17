/*	@(#)setjmp.h	2.1	 */
#ifndef _JBLEN

#ifdef vax
#define _JBLEN	10
#endif

#ifdef pdp11
#define _JBLEN	3
#endif

#ifdef u370
#define _JBLEN	4
#endif

#ifdef u3b
#define _JBLEN	11
#endif

#ifdef m68k
#define _JBLEN	13
#endif

typedef int jmp_buf[_JBLEN];

#endif
