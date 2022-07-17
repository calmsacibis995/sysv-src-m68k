/* @(#)param.h	2.16.1.1	 */
/*
 * fundamental variables
 * don't change too often
 */

#define	NOFILE	20		/* max open files per process */
#define	MAXPID	30000		/* max process id */
#define	MAXUID	60000		/* max user id */
#define	MAXLINK	1000		/* max links */

#define	MAXMEM	(16*128)	/* max core in 512-byte clicks */
#define	MAXBLK	125		/* max blocks possible for phys IO */
#define	SWAPSIZE	64	/* granularity of partial swaps (in clicks) */
#define	SSIZE	4		/* initial stack size (*512 bytes) */
#define	SINCR	2		/* increment of stack (*512 bytes) */
#define	USIZE	4		/* size of user block (*512 bytes) */
#ifdef m68k
#ifdef M68020
#define	USRSTACK (0)				/* Start of user stack */
#else
#define	USRSTACK (0x1000000)			/* Start of user stack */
#endif
#else
#define	USRSTACK (0x80000000-ctob(USIZE))	/* Start of user stack */
#endif

#define	CANBSIZ	256		/* max size of typewriter line	*/
#define	HZ	60		/* Ticks/second of the clock */
#define	NCARGS	5120		/* # characters in exec arglist */

/*
 * priorities
 * should not be altered too much
 */

#define	PMASK	0177
#define	PCATCH	0400
#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PZERO	25
#define	NZERO	20
#define	PPIPE	26
#define	PWAIT	30
#define	PSLEP	39
#define	PUSER	60
#define	PIDLE	127

/*
 * fundamental constants of the implementation--
 * cannot be changed easily
 */

#define	NBPW	sizeof(int)	/* number of bytes in an integer */

#ifndef	FsTYPE
#define	FsTYPE	3
#endif

#if FsTYPE==1
	/* Original 512 byte file system */
#define	BSIZE	512		/* size of file system block (bytes) */
#define	SBUFSIZE	BSIZE	/* system buffer size */
#define	BSHIFT	9		/* LOG2(BSIZE) */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	0777		/* BSIZE-1 */
#define	INOPB	8		/* inodes per block */
#define	INOSHIFT	3	/* LOG2(INOPB) if exact */
#define	NMASK	0177		/* NINDIR-1 */
#define	NSHIFT	7		/* LOG2(NINDIR) */
#define	FsBSIZE(dev)	BSIZE
#define	FsBSHIFT(dev)	BSHIFT
#define	FsNINDIR(dev)	NINDIR
#define	FsBMASK(dev)	BMASK
#define	FsINOPB(dev)	INOPB
#define	FsLTOP(dev, b)	b
#define	FsPTOL(dev, b)	b
#define	FsNMASK(dev)	NMASK
#define	FsNSHIFT(dev)	NSHIFT
#define	FsITOD(dev, x)	itod(x)
#define	FsITOO(dev, x)	itoo(x)
#endif

#if FsTYPE==2
	/* New 1024 byte file system */
#define	BSIZE	1024		/* size of file system block (bytes) */
#define	SBUFSIZE	BSIZE	/* system buffer size */
#define	BSHIFT	10		/* LOG2(BSIZE) */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	01777		/* BSIZE-1 */
#define	INOPB	16		/* inodes per block */
#define	INOSHIFT	4	/* LOG2(INOPB) if exact */
#define	NMASK	0377		/* NINDIR-1 */
#define	NSHIFT	8		/* LOG2(NINDIR) */
#define	FsBSIZE(dev)	BSIZE
#define	FsBSHIFT(dev)	BSHIFT
#define	FsNINDIR(dev)	NINDIR
#define	FsBMASK(dev)	BMASK
#define	FsINOPB(dev)	INOPB
#define	FsLTOP(dev, b)	(b<<1)
#define	FsPTOL(dev, b)	(b>>1)
#define	FsNMASK(dev)	NMASK
#define	FsNSHIFT(dev)	NSHIFT
#define	FsITOD(dev, x)	itod(x)
#define	FsITOO(dev, x)	itoo(x)
#endif

#if FsTYPE==3
	/* Dual system */
#define	BSIZE	512		/* size of file system block (bytes) */
#define	SBUFSIZE	(BSIZE*2)	/* system buffer size */
#define	BSHIFT	9		/* LOG2(BSIZE) */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	0777		/* BSIZE-1 */
#define	INOPB	8		/* inodes per block */
#define	INOSHIFT	3	/* LOG2(INOPB) if exact */
#define	NMASK	0177		/* NINDIR-1 */
#define	NSHIFT	7		/* LOG2(NINDIR) */
#define	Fs2BLK	0x2000
#define	FsLRG(dev)	(dev&Fs2BLK)
#define	FsBSIZE(dev)	(FsLRG(dev) ? (BSIZE*2) : BSIZE)
#define	FsBSHIFT(dev)	(FsLRG(dev) ? 10 : 9)
#define	FsNINDIR(dev)	(FsLRG(dev) ? 256 : 128)
#define	FsBMASK(dev)	(FsLRG(dev) ? 01777 : 0777)
#define	FsBOFF(dev, x)	(FsLRG(dev) ? ((x)&01777) : ((x)&0777))
#define	FsBNO(dev, x)	(FsLRG(dev) ? ((x)>>10) : ((x)>>9))
#define	FsINOPB(dev)	(FsLRG(dev) ? 16 : 8)
#define	FsLTOP(dev, b)	(FsLRG(dev) ? b<<1 : b)
#define	FsPTOL(dev, b)	(FsLRG(dev) ? b>>1 : b)
#define	FsNMASK(dev)	(FsLRG(dev) ? 0377 : 0177)
#define	FsNSHIFT(dev)	(FsLRG(dev) ? 8 : 7)
#define	FsITOD(dev, x)	(daddr_t)(FsLRG(dev) ? \
	((unsigned)x+(2*16-1))>>4 : ((unsigned)x+(2*8-1))>>3)
#define	FsITOO(dev, x)	(daddr_t)(FsLRG(dev) ? \
	((unsigned)x+(2*16-1))&017 : ((unsigned)x+(2*8-1))&07)
#define	FsINOS(dev, x)	(FsLRG(dev) ? \
	((x&~017)+1) : ((x&~07)+1))
#endif

#define	NICFREE	50		/* number of superblock free blocks */
#define	NCPS	1		/* Number of clicks per segment */
#define	NBPC	512		/* Number of bytes per click */
#define	NCPD	1		/* Number of clicks per disk block */
#define	BPCSHIFT	9	/* LOG2(NBPC) if exact */
#define	NULL	0
#define	CMASK	0		/* default mask for file creation */
#define	CDLIMIT	(1L<<11)	/* default max write address */
#define	NODEV	(dev_t)(-1)
#define	ROOTINO	((ino_t)2)	/* i number of all roots */
#define	SUPERB	((daddr_t)1)	/* physical block number of the super block */
#define	SUPERBOFF	512	/* byte offset of the super block */
#define	DIRSIZ	14		/* max characters per directory */
#define	NICINOD	100		/* number of superblock inodes */
#define	CLKTICK	16667		/* microseconds in a clock tick */

#define	UMODE	PS_CUR		/* usermode bits */
#ifdef m68k
#define	USERMODE(ps)	((ps & UMODE) == 0)
#else
#define	USERMODE(ps)	((ps & UMODE) == UMODE)
#endif

#define	BASEPRI(ps)	((ps & PS_IPL) != 0)

#ifdef m68k
#define	SEG_SYS		(-1)	/* System space */
#define	SEG_TEXT	0	/* Text segment. Shared by default */
#define	SEG_DATA	1	/* Data+Bss. Includes Text if not shared */
#define	SEG_STACK	2	/* Stack segment */
#define	SEG_SHM		3	/* Shared memory segment. */
#else
#define	SEG_SYS		1	/* System space */
#define	SEG_TEXT	2	/* Text segment. Shared by default */
#define	SEG_DATA	0	/* Data+Bss. Includes Text if not shared */
#endif

#if defined(vax) || defined(pdp11)
#define	lobyte(X)	(((unsigned char *)&X)[0])
#define	hibyte(X)	(((unsigned char *)&X)[1])
#define	loword(X)	(((ushort *)&X)[0])
#define	hiword(X)	(((ushort *)&X)[1])
#else
#define	lobyte(X)	(((unsigned char *)&X)[1])
#define	hibyte(X)	(((unsigned char *)&X)[0])
#define	loword(X)	(((ushort *)&X)[1])
#define	hiword(X)	(((ushort *)&X)[0])
#endif
