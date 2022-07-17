/*	@(#)defs.h	2.1		*/
#define	ON	1
#define	OFF	0

#define FATAL	1
#define ERROR	-999L

#define	BSIZE	256
#ifdef m68k
#define MAXSECTIONS	256
#define MAXTEMPENTS	1024
#endif

#define	TEMPENT		struct tempent
struct	tempent
{
	long	new;
	long	old;
#if m68k || !AR16WR
	TEMPENT	*link;
#endif
};

#define	TEMPSZ		sizeof(TEMPENT)

/*	@(#) defs.h: 1.4 1/19/83	*/
