/*	@(#)crash.h	2.1		*/
#include  "a.out.h"
#include  "signal.h"
#include  "sys/param.h"
#include  "sys/sysmacros.h"
#include  "sys/types.h"
#include  "sys/dir.h"
#include  "sys/user.h"
#include  "sys/var.h"
#include  "stdio.h"
#ifdef m68k
#include  "sgs.h"
#endif

#if (defined(vax) || defined(m68k))
#define N_TEXT	1
#define N_DATA	2
#define N_BSS	3
#endif

#ifdef m68k
#define	VIRT_MEM	0xffffff
#else
#define	VIRT_MEM	0x3fffffff
#endif

#define	SYM_VALUE(ptr)	(ptr->n_value & VIRT_MEM)
#ifdef	pdp11
#define	FMT	"%6.6o"
#define	HDR	sizeof(struct exec)
#define TXT	(HDR + abuf.a_data)
#endif
#if (defined(vax) || defined(m68k))
#define	FMT	"%8.8x"
#endif
#define	SWAPPED	1	/* Returned by getuarea if process is swapped */


struct	frame	{
	unsigned  f_r5;
	unsigned  f_ret;
} ;

#ifdef m68k
struct	dstk	{
	int	d0;
	int	d1;
	int	d2;
	int	d3;
	int	d4;
	int	d5;
	int	d6;
	int	d7;
	int	a0;
	int	a1;
	int	a2;
	int	a3;
	int	a4;
	int	a5;
	int	a6;
	int	ksp;
	int	sr;
	int	usp;
#ifdef MC68010
	int	vbr;
#endif
} ;

#else
struct	dstk	{
	int	r0;
	int	r1;
	int	r2;
	int	r3;
	int	r4;
	int	r5;
	int	r6;
	int	r7;
	int	r8;
	int	r9;
	int	r10;
	int	r11;
	int	r12;
	int	r13;
	int	ipl;
	int	mapen;
	int	pcbb;
	int	stkptr;
} ;
#endif

struct	glop	{
	int	g_x0;
	int	g_x1;
	int	g_r0;
	int	g_r1;
	int	g_r2;
	int	g_r3;
	int	g_r4;
	int	g_r5;
	int	g_sp;
	unsigned  int  g_ka6;
} ;

struct	tsw	{
	char	*t_nm;
	int	t_sw;
	char	*t_dsc;
} ;

struct	prmode	{
	char	*pr_name;
	int	pr_sw;
} ;

#define	USIZ	(USIZE * NBPC)		/* (pdp=16*64, vax=4*512) m68k=4*512 */
#define MAXI	30

extern	struct	var	v;
extern	int	mem;
extern	int	kmem;

struct	uarea {
	struct user u;
	char	stk[USIZ - sizeof(struct user)];
};

#ifdef	pdp11
struct	xinterface {	/* jsr r5, call; jmp function */
	int	i_jsr;
	int	i_call;
	int	i_jmp;
	int	i_func;
};
#endif

#ifdef m68k
extern	struct	nlist	*File, *Inode, *Mount, *Swap, *Core, *Proc, *Sbuf,
			*Sys, *Time, *Panic, *Etext, *Text, *V, *Mmu,
			*Buf, *End, *Callout, *Lbolt, *Dmpstk, *Curproc, *U;
#else
extern	struct	nlist	*File, *Inode, *Mount, *Swap, *Core, *Proc, *Sbuf,
			*Sys, *Time, *Panic, *Etext, *Text, *V, *Sbrpte,
			*Buf, *End, *Callout, *Lbolt, *Dmpstk, *Curproc, *U;
#endif

#define	STACK	1
#define	UAREA	2
#define	FILES	3
#define	TRACE	4
#define	QUIT	5
#define	PCBLK	6
#define	INODE	7
#define	MOUNT	8
#define	TTY	9
#define	Q	10
#define	TEXT	11
#define	TS	13
#define	DS	14
#define	PROC	15
#define	STAT	16
#define	KFP	17
#define	BUFHDR	20
#define	BUFFER	21
#define	TOUT	22
#define	NM	23
#define	OD	24
#define	MAP	25
#define	VAR	28

#define	DIRECT	2
#define	OCTAL	3
#define	DECIMAL	4
#define	CHAR	5
#define	WRITE	6
#define	INODE	7
#define	BYTE	8
#define	LDEC	9
#define	LOCT	10
#define	HEX	11

