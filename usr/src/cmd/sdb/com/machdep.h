	/*	@(#)machdep.h	2.1		*/

#include <sys/param.h>
#include <sys/types.h>
#include <signal.h>

#ifdef u3b
#include <sys/macro.h>	/* has stob, for sys/seg.h below (also has ptob) */
#else
#include <sys/sysmacros.h>
#endif

#include <sys/dir.h>

#ifdef u3b
#include <sys/seg.h>	/* needed for USRSTACK */
#include <sys/istk.h>	/* needed for istk structure in struct user */
#endif

#include <sys/user.h>
#include <sys/errno.h>
#include <a.out.h>
#include <sys/reg.h>

#ifdef u3b
#define XTOB(x) (ptob(x))
#define SUBCALL		(isubcall(dot,ISP))	/* subroutine call instruction*/
#define SUBRET		get(SDBREG(SP)-8, DSP)	/* subroutine ret instruction */
#define RETREG		0			/* register with ret value    */

/* Given the current fp (frame), these are the next frame pc, ap, fp */
#define NEXTCALLPC (frame - (13*WORDSIZE))
#define NEXTARGP   (frame - (12*WORDSIZE))
#define NEXTFRAME  (frame - (11*WORDSIZE))

#define TXTRNDSIZ	0x20000L	/* = 128K = (1L<<17)	*/

/* Address of UBLOCK (absolute) is beginning of segment 5 */
#define ADDR_U ((unsigned) (0x5 * TXTRNDSIZ))

#define	PROCOFFSET	0	/* offset from label to first instruction */
#define BKOFFSET   10	/* offset from beginning of proc to bkpt (no -O) */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define APNO	9	/* argument pointer register number */
#define FPNO	10	/* frame pointer register number */
#define NUMREGLS 16	/* number of "registers" in reglist[] */
#define NUMREGS	16	/* number of general purpose registers */

#define ISREGVAR(regno)	(3<=(regno) && (regno) <= 8)

#endif

#ifdef vax

#define XTOB(x) (ctob(x))
#define SUBCALL		((get(dot,ISP) & 0xff) == 0xfb) /* subroutine call */
#define SUBRET		get(SDBREG(SP)+16, DSP)	/* subroutine ret instruction */
#define RETREG		0			/* register with ret value */

/* Given the current fp (frame), these are the next frame pc, ap, fp */
#define NEXTCALLPC (frame + (4*WORDSIZE))
#define NEXTARGP   (frame + (2*WORDSIZE))
#define NEXTFRAME  (frame + (3*WORDSIZE))

#define TXTRNDSIZ 512L

#define ADDR_U ((unsigned) 0x7ffff800)	/* absolute address of UBLOCK */

#define	PROCOFFSET	2	/* offset from label to first instruction */
#define BKOFFSET    0	/* offset from beginning of proc to bkpt */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define APNO	12	/* argument pointer register number */
#define FPNO	13	/* frame pointer register number */
#define NUMREGLS 17	/* number of "registers" in reglist[] */
#define NUMREGS	16	/* number of general purpose registers */

#define ISREGVAR(regno) (6 <= (regno) && (regno) < 12)
#endif
#ifdef m68k

#define NARGDEFAULT 3
#define XTOB(x) (ctob(x))
#define ISJSR		((get(dot,ISP) & 0xffc00000) == 0x4e800000)  /* jsr call */
#define ISBSR		((get(dot,ISP) & 0xff000000) == 0x61000000)  /* bsr call */
#define SUBCALL		(ISJSR || ISBSR)
#define SUBRET		get(SDBREG(SP), DSP)	/* subroutine ret instruction */
#define RETREG		0			/* register with ret value (d0)  */

/* Given the current fp (frame), these are the next frame pc, ap, fp */
#define NEXTCALLPC (frame + WORDSIZE)
#define NEXTFRAME  (frame)
#define NEXTARGP   NEXTFRAME   /* no arg ptr for 68k */

#define TXTRNDSIZ 512L

#ifdef VARU		/* MC68000 and MC68010 systems with no memory mgt */
#define ADDR_U	ubase	/* ubase computed in setuaddr() <setup.c> */
#else
#ifdef MC68020
#define ADDR_U	((unsigned)(0x100000000 - XTOB(USIZE)))
#else			/* 68000 and 68010 systems with memory management */
#define ADDR_U	((unsigned)(0x1000000 - XTOB(USIZE)))
#endif
#endif

#define	PROCOFFSET	0	/* offset from label to first instruction */
#define BKOFFSET    0	/* offset from beginning of proc to bkpt */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define FPNO	14	/* frame pointer register number */
#define APNO	FPNO	/* 68000 does not have arg ptr, define in terms of fp */
#define AP	FP	/* FP defined in /usr/include/sys/reg.h */
#define NUMREGLS 18	/* number of "registers" in reglist[] */
#define NUMREGS	18	/* number of registers */

#define ISDATAREG(regno) ((2 <= (regno) && (regno) < 8)) 
#define ISADDRREG(regno) ((10 <= (regno) && (regno) < 14)) 
#define ISREGVAR(regno) (ISDATAREG(regno) || ISADDRREG(regno))
#endif
extern char uu[XTOB(USIZE)];

extern ADDR	callpc, frame, argp;	/* current stack frame */

#define WORDSIZE (sizeof(int))	/* wordsize in bytes on this machine */
#define REGSIZE WORDSIZE	/* register size in bytes on this machine */

#define NOBACKUP 0		/* set to 1 if machine does not back up */
			        /* to previous instruction at exception */
#define ADDRTYPE	"d"	/* type of address for getval */

#ifdef MC68020
#define MAXPOS 	0x7fffffff 	/* max int instead of max addr for 68020 */
#else
#define MAXPOS	0x7fffff	/* maximum address */
#endif

/*  two extra numbers to be printed with regs; in optab.c */
/*  removed because these are not offsets from R0; can't use SDBREG */
/*
#define VAL1	((unsigned)&(((struct user *) 0)->u_rval1)
#define VAL2	((unsigned)&(((struct user *) 0)->u_rval2)
*/

/* ptracew modes */
#define	SETTRC	0
#define	RDUSER	2
#define	RIUSER	1
#define	WDUSER	5
#define WIUSER	4
#define	RUREGS	3
#define	WUREGS	6
#define	CONTIN	7
#define	EXIT	8
#define SINGLE	9

extern REGLIST reglist [];

#define SDBREG(A) (((struct user *)uu)->u_ar0[A])
#define SYSREG(A) ((int) (((char *) (&SDBREG(A)) - ((int) uu))))
#define ISREGN(reg)	(0<= (reg) && (reg) < NUMREGS)
#define USERR0  SDBREG(R0)
#define USERPC  SDBREG(PC)

union word {
	char c[WORDSIZE]; 
	unsigned char cu[WORDSIZE];
	short s[WORDSIZE/2];
	unsigned short su[WORDSIZE/2];
	int w;
	unsigned int iu;
	long l;
	float f;
};
union dbl {
	struct {
		int w1, w2;
	} ww;
	double dd;
	int i;
	float f;
	char cc[WORDSIZE*2];
};
