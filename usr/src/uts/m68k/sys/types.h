/* @(#)types.h	2.2	 */
typedef	struct { int r[1]; } *	physadr;
typedef	long		daddr_t;
typedef	char *		caddr_t;
typedef	unsigned int	uint;
typedef	unsigned short	ushort;
typedef	ushort		ino_t;
typedef short		cnt_t;
typedef	long		time_t;
#ifdef m68k
typedef	int		label_t[13];
#else
typedef	int		label_t[10];
#endif
typedef	short		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
typedef	long		key_t;
