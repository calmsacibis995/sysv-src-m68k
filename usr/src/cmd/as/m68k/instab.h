/*	@(#)instab.h	2.1		*/

/*		Header file for mc68000 assembler.			*/

typedef	enum
	{
		FALSE,
		TRUE
	}	BOOL;


#define	B	0		/* Size attribute .b == BYTE.		*/
#define W	1		/* Size attribute .w == WORD.		*/
#define L	2		/* Size attribute .l == LONG.		*/
#define UNSPEC	3		/* No size attribute specified.		*/

/* These must fit within a byte */

#define PSEUDO	0x40
#define DADD	0x41
#define ADD	0x42
#define XADD	0x43
#define LOGAND	0x44
#define ASHFT	0x45
#define BRA	0x46
#define	BR	0x47
#define BCHG	0x48
#define BTST	0x49
#define BSR	0x4a
#define MULT	0x4b
#define OPTSIZE	0x4c
#define CMP	0x4d
#define DBCC	0x4e
#define LOGEOR	0x4f
#define EXG	0x50
#define EXT	0x51
#define LEA	0x52
#define LINK	0x53
#define LSHFT	0x54
#define MOV	0x55
#define MOVM	0x56
#define MOVP	0x57
#define LOGOR	0x58
#define ROT	0x59
#define ROTX	0x5a
#define STOP	0x5b
#define SWAP	0x5c
#define TRAP	0x5d
#define UNLK	0x5e
#define LOGSRC	0x5f
#define LOGDST	0x60
#define ADDQ	0x61
#define MOVMMR	0x62
#define MOVMRM	0x63
#define ADDSRC	0x64
#define ADDDST	0x65
#define RTD	0x66
#define MOVS	0x67
#define	DEFAULT	0x70			/* default; size (B,W,L) added in */
/* Allow for DEFAULT+[B|W|L|UNSPEC] */

#define ADREG	000
#define AAREG	010
#ifdef M68020
#	define NULAREG	001
#	define NULDREG	002
#	define NULPCREG 003
#endif
#define AIREG   020
#define AINC    030
#define ADEC    040
#define AOFF	050
#define ANDX	060
#define ABSW	070
#define ABSL	071
#define APCOFF	072
#define APCNDX	073
#define AIMM	074


#define PCREG   0x70L
#define CCREG   0x72L
#define SRREG   0x73L
#define USPREG	0x800L	/* this is an opcode, not a type! (i.e. > byte) */
#define CTLREG	0x74L


#define CONTROL	0x27e
#define CONPRE	0x2f8
#define CONPOST	0x37e
#define ALTMEM	0x3f8
#define DATALT	0xbf8
#define DATNIM	0xbfe
#define DATAA	0xbff
#define ALT	0xff8
#define ALL	0xfff

#ifdef	M68020		
#	define X1	0	/* define scales for indexed addressing */
#	define X2	1
#	define X4	2
#	define X8	3

#	define INDIRECT 00	/* no memory indirection */
#	define POSTNDXMI 04	/* post indexed memory indirect */
#	define PRENDXMI 014	/* pre indexed memory indirect */

#	define SET	1	/* as in "ss" in sized registers */
#	define NOTSET	0
#endif

struct	arg
	{
 		BYTE		atype;
		instr		*areg1;
		instr		*areg2;
		BYTE		asize;		/* associated with index reg */
#ifdef	M68020
		BYTE		ascale;		/* associated with index reg */
		short		aflags;
#endif
#ifndef	M68020
		struct exp	*xp;
#else
		struct exp	*xp1;
		struct exp	*xp2;
#define xp	xp1
#endif
	};

struct	exp
	{
		short	xtype;
		symbol	*xsymptr;
		long	xvalue;
#ifdef	M68020
		BYTE	xsize;			/* sized displacements	*/
#endif
	};

typedef union
	{
		long		ylong;
 		int		yint;
		symbol		*yname;
		instr		*yinst;
		struct exp	*yexp;
		struct arg	*yarg;
		char		*ystr;
	} YYSTYPE;

extern  YYSTYPE		yylval;
extern struct arg	*ap;
extern struct exp	*xp;
