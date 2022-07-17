/*	@(#)parse20.y	2.1		*/
%{
static	char	sccsid[] = "@(#) parse20.y: 2.1 11/28/83";
/* static	char	sccsid[] = "@(#) parse.y: 2.6 6/24/83"; */


/*			Motorola 68000 assembler			*/

#include	<stdio.h>
#include	<string.h>
#include	"symbols.h"
#include	"instab.h"
#include	"gendefs.h"

extern unsigned short
		cline,		/* Line number from the .ln pseudo op.	*/
		line;		/* Current input file line number.	*/

extern FILE	*fdin;		/* Input source.			*/
extern short	opt;		/* Sdi optimization flag.		*/
extern char	file[],		/* Assembly input file name.		*/
		cfile[];	/* Original C file name.		*/

extern symbol	*dot;		/* Current pc pointer.			*/
extern long	newdot,		/* Up-to-date value of pc pointer.	*/
		dotbss;

struct arg	arglist[3];
struct arg	*ap = arglist;
struct	exp	explist[100];
struct	exp	*xp = explist;

long		outword;	/* Bit field accumulator.		*/
int		bitsout;	/* Number of bits accumulated.		*/
BYTE		bytesout;	/* Output displacement.			*/
int		swbegct;	/* Number of switch locations.		*/

#ifdef	M68020
int		scaleseen; 	/* only one reg can be scaled		*/
int		sizeseen;	/* only one reg can be sized		*/
int		regseen;	/* count registers as we see them	*/
int		indexreg;	/* remember position of index register	*/
int		ssflag;		/* have we seen a size-scale		*/
#endif 

struct exp	*combine();	/* Handles expression arithmetic.	*/
extern upsymins	*lookup();	/* Symbol table lookup routine.		*/
extern		deflab();	/* Define label for sdi handler.	*/
extern		gen1op();	/* Handles one-operand instructions.	*/
extern		gen2op();	/* Handles two-operand instructions.	*/
extern		resolve();	/* resolves all SDI's to known length   */

char		err_buf[BUFSIZ];
char		yytext[BUFSIZ];
%}

/* Symbolic Debugger Support Pseudo Operations				*/
%token	IFILE	ILN	IDEF	IENDEF	IVAL
%token	ITYPE	ILINE	ISCL	ITAG	ISIZE	IDIM


%token	ISET	IGLOBAL	IDATA	ITEXT	IBYTE	ISHORT	ILONG
%token	ISPACE	IORG	ICOMM	ILCOMM	IEVEN	ISWBEG

%token	COLON	AMP	NOCHAR	SPC	ALPH	DIGIT	SQ
%token	SH	CM	NL	LP	RP	SEMI	DQ	TWID

%token	LBRKT	RBRKT	/*	68020 support	*/

%left	PLUS	MINUS
%left	MUL	DIV
%left	UMINUS		/* Only used for precedence--no such token.	*/

%token	<yname>	NAME
%token	<yinst>	INST0	INST1	INST2	REG
%token	<yint>	SIZE	PLUS	MINUS	MUL	DIV	LP	RP
%token	<ylong>	INT
%token	<ystr>	STRING

%type	<yinst>	reg.ss					/* 68020 */
%type	<yint>	optsize	optscl	ss	lp	rp	/* 68020 */
%type	<yarg>	arg
%type	<yexp>	expr	disp

%%

file	:	/* empty */
	|	file linstruction NL
	{
		line++;
		dot->value = newdot;
		generate(0,NEWSTMT,(long) line,NULLSYM);
		ap = arglist;
		xp = explist;
	}
	|	file linstruction SEMI
	{
		dot->value = newdot;
		generate(0,NEWSTMT,(long) line,NULLSYM);
		ap = arglist;
		xp = explist;
	}
	|	file error NL
	{
		line++;
		yyerrok;
		dot->value = newdot;
		ap = arglist;
		xp = explist;
	}
	;


linstruction:	labels instruction
	;

labels	:	/* empty */
	|	labels NAME COLON
	{
		if (($2->styp & TYPE) != UNDEF)
		{
			strcpy(err_buf,"multiply defined label--");
			strcat(err_buf,yytext);
			yyerror(err_buf);
		}
		$2->value = newdot;
		$2->styp |= dot->styp;
		$2->sectnum = dot->sectnum;
		if (opt && ((dot->styp & TYPE) == TXT) && dot->sectnum == 0)
			deflab($2);
	}
	;

instruction:
		IFILE STRING
	{
		if (cfile[0] != '\0')
			yyerror("only one .file allowed");
		strcpy(cfile,$2);
		generate(0,SETFILE,0,NULLSYM);
	}
	|	ILN expr
	{
		cline = (short) $2->xvalue;
		generate(0,LINENBR,$2->xvalue,$2->xsymptr);
	}
	|	ILN expr CM expr
	{
		cline = (short) $2->xvalue;
		generate(0,LINENUM,$2->xvalue,$2->xsymptr);
		generate(0,LINEVAL,$4->xvalue,$4->xsymptr);
	}
	|	IDEF NAME
	{
		generate(0,DEFINE,0L,$2);
	}
	|	IENDEF
	{
		generate(0,ENDEF,0,NULLSYM);
	}
	|	IVAL expr
	{
		generate(0,SETVAL,$2->xvalue,$2->xsymptr);
	}
	|	ITYPE expr
	{
		generate(0,SETTYP,$2->xvalue,$2->xsymptr);
	}
	|	ILINE expr
	{
		generate(0,SETLNO,$2->xvalue,$2->xsymptr);
	}
	|	ISCL expr
	{
		generate(0,SETSCL,$2->xvalue,$2->xsymptr);
	}
	|	ITAG NAME
	{
		generate(0,SETTAG,0,$2);
	}
	|	ISIZE expr
	{
		generate(0,SETSIZ,$2->xvalue,$2->xsymptr);
	}
	|	dotdim
	|	ISET NAME CM expr
	{
		if (($4->xtype & TYPE) == UNDEF)
			yyerror("illegal expression in .set");
		if ($2 == NULL)
			yyerror("unable to define symbol in .set");
		else
		{
			$2->value = $4->xvalue;
			$2->styp = $4->xtype;
		}
	}
	|	IGLOBAL names
	|	IDATA
	{
		cgsect(DAT,0);
	}
	|	IDATA expr
	{
		cgsect(DAT,$2->xvalue);
	}
	|	ITEXT
	{
		cgsect(TXT,0);
	}
	|	ITEXT expr
	{
		cgsect(TXT,$2->xvalue);
	}
	|	IBYTE {bytesout = 1;} explist
	{
		if (bitsout)
		{
			generate(BITSPBY*bytesout,0,outword,NULLSYM);
			outword = 0;
			bitsout = 0;
		}
	}
	|	ISHORT {bytesout = 2;} explist
	{
		if (bitsout)
		{
			generate(BITSPBY*bytesout,0,outword,NULLSYM);
			outword = 0;
			bitsout = 0;
		}
	}
	|	ILONG {bytesout = 4;} explist
	{
		if (bitsout)
		{
			generate(BITSPBY*bytesout,0,outword,NULLSYM);
			outword = 0;
			bitsout = 0;
		}
	}
	|	ISPACE expr
	{
		if ($2->xtype != ABS)
			yyerror("space size not absolute");
		fill($2->xvalue);
	}
	|	IORG expr
	{
		if ($2->xtype != ABS)
			yyerror("org requires absolute operand");
		if (($2->xvalue - dot->value) < 0)
			yyerror("backwards org");
		else
		{
			if (opt && ((dot->styp & TYPE) == TXT) 
			    && dot->sectnum == 0)
			   resolve();	/* force PC to known value */
			fill($2->xvalue - dot->value);
		}
	}
	|	ICOMM NAME CM expr
	{
		if ($4->xtype != ABS)
			yyerror("comm size not absolute");
		if (($2->styp & TYPE) != UNDEF)
			yyerror("illegal attempt to redefine symbol");
		else
		{
			$2->styp = (EXTERN | UNDEF);
			$2->value = $4->xvalue;
		}
	}
	|	ILCOMM NAME CM expr
	{
		if ($4->xtype != ABS)
			yyerror("lcomm size not absolute");
		if (($2->styp & TYPE) != UNDEF)
			yyerror("illegal attempt to redefine symbol");
		else
		{
		/* assume that odd length items may be on arbitrary
		** boundaries, but even length items must be on even
		** boundaries
		*/
			if (($2->value & 01) == 0 && (dotbss & 01) != 0)
			    dotbss++;
			$2->value = dotbss;
			dotbss += $4->xvalue;
			$2->styp |= BSS;
		}
	}
	|	IEVEN
	{
		ckalign(2);
	}
	|	ISWBEG arg
	{
		generate(BITSPOW,SWBEG,0x4afc,NULLSYM);
		generate(BITSPOW,SWBEG,$2->xp->xvalue,NULLSYM);
		swbegct = $2->xp->xvalue;
	}
	|	INST0
	{
		/* No operands, and no size attribute.			*/
		generate(BITSPOW,0,$1->opcode,NULLSYM);
	}
	|	INST1 optsize arg
	{
		gen1op($1,$2,$3);
	}
	|	INST2 optsize arg CM arg
	{
		gen2op($1,$2,$3,$5);
	}
	|	/* empty */
	;

dotdim	:	IDIM expr
	{
		generate(0,SETDIM1,$2->xvalue,$2->xsymptr);
	}
	|	dotdim CM expr
	{
		generate(0,SETDIM2,$3->xvalue,$3->xsymptr);
	}
	;
names	:	NAME			/* Part of .globl handling.	*/
	{
		$1->styp |= EXTERN;
	}
	|	names CM NAME
	{
		$3->styp |= EXTERN;
	}
	;

optsize	:	SIZE
	{
		$$ = $1;
	}
	|	/* empty */
	{
		$$ = UNSPEC;
	}
	;

explist	:	outexpr
	|	explist CM outexpr
	;

outexpr	:	expr
	{
		if (bitsout)
		{
			generate(BITSPBY*bytesout,0,outword,NULLSYM);
			outword = 0;
			bitsout = 0;
		}
		if (swbegct-- > 0)
			generate(BITSPBY*bytesout,SWBEG,0,$1->xsymptr);
		else
			generate(BITSPBY*bytesout,GENRELOC,$1->xvalue,$1->xsymptr);
	}
	|	expr COLON expr
	{
		if ($1->xtype != ABS)
			yyerror("width expression not absolute");
		if ($1->xvalue > (BITSPBY * bytesout))
			yyerror("expression crosses field boundary");
		if (($1->xvalue + bitsout) > (BITSPBY * bytesout))
		{
			generate(BITSPBY*bytesout,0,outword,NULLSYM);
			bitsout = 0;
			outword = 0;
		}
		$3->xvalue <<= bitsout;
		outword |= $3->xvalue;
		bitsout += $1->xvalue;
	}
	;

arg	:	expr
	{
		ap->atype = ABSL;
		ap->xp = $1;
		ap->areg1 = 0;
		ap->areg2 = 0;
		$$ = ap++;
	}
	|	REG
	{
		
		ap->atype = $1->tag;
		ap->areg1 = $1;
		ap->areg2 = 0;
		$$ = ap++;
	}
	|	MINUS LP REG RP
	{
		ap->atype = ADEC;
		if ($3->tag != AAREG)
			yyerror("predecrement requires address register");
		ap->areg1 = $3;
		ap->areg2 = 0;
		$$ = ap++;
	}
	|	LP REG RP PLUS
	{
		ap->atype = AINC;
		if ($2->tag != AAREG)
			yyerror("postincrement requires address register");
		ap->areg1 = $2;
		ap->areg2 = 0;
		$$ = ap++;
	} 
	|	AMP expr
	{
		ap->atype = AIMM;
		ap->xp = $2;
		ap->areg1 = 0;
		ap->areg2 = 0;
		$$ = ap++;
	}
	|	ind
	{
		$$ = ap++;
	}
	;

/*	
 *	68020 - register indirect w/ index and displacement
 *
 *	three forms:
 *
 *	1. indexed indirect w/ displacement
 *
 *		( bd[.s], An, Rn[.t][*scale] )
 *
 *	2. memory indirect w/ pre-index and displacement
 *
 *		( od[.t], [bd[.t], An, Rn[.t][*scale]] )
 *
 *	3. memory indirect w/ post-index and displacement
 *
 *		( od[.t], [bd[.t], An], Rn[.t][*scale] )
 *
 */

ind	:	nmi
	|	memind
	|	expr lp REG rp			/* 68000 old forms */
	{
		mkindex_indirect(INDIRECT,$3,0,$1);
	}
	|	expr lp REG CM reg.ss rp	/* 68000 old forms */
	{	
		mkindex_indirect(INDIRECT,$3,$5,$1);
	}
	;


	/* nmi - No Memory Indirect */
nmi	:	lp disp CM reg.ss CM reg.ss rp
	{	
		mkindex_indirect(INDIRECT,$4,$6,$2);
	}
	|	lp reg.ss CM disp CM reg.ss rp
	{
		mkindex_indirect(INDIRECT,$2,$6,$4);
	}
	|	lp reg.ss CM reg.ss CM disp rp
	{
		mkindex_indirect(INDIRECT,$2,$4,$6);
	}
	|	lp disp CM reg.ss rp
	{
		mkindex_indirect(INDIRECT,$4,0,$2);
	}
	|	lp reg.ss CM disp rp
	{
		mkindex_indirect(INDIRECT,$2,0,$4);
	}
	|	lp reg.ss CM reg.ss rp
	{
		mkindex_indirect(INDIRECT,$2,$4,0);
	}
	|	lp reg.ss rp
	{
		mkindex_indirect(INDIRECT,$2,0,0);
	}
	|	lp disp rp
	{
		mkindex_indirect(INDIRECT,0,0,$2);
	}
	;


memind	:	postind
	{
		;
	}
	|	preind
	{
		;
	}
	;


postind	:	lp mbd CM reg.ss CM disp rp
	{
		mk_post_ind($4,$6);
	}
	|	lp reg.ss CM mbd CM disp rp
	{
		mk_post_ind($2,$6);
	}
	|	lp reg.ss CM disp CM mbd rp
	{
		mk_post_ind($2,$4);
	}
	|	lp disp CM reg.ss CM mbd rp
	{
		mk_post_ind($4,$2);
	}
	|	lp disp CM mbd CM reg.ss rp
	{
		mk_post_ind($6,$2);
	}
	|	lp mbd CM disp CM reg.ss rp
	{
		mk_post_ind($6,$4);
	}
	|	lp mbd CM disp rp
	{
		mk_post_ind(0,$4);
	}
	|	lp disp CM mbd rp
	{
		mk_post_ind(0,$2);
	}
	|	lp reg.ss CM mbd rp
	{
		mk_post_ind($2,0);
	}
	|	lp mbd CM reg.ss rp
	{
		mk_post_ind($4,0);
	}
	|	lp mbd rp
	{
		mk_post_ind(0,0);
	}
	;


preind	:	lp mbid CM disp rp
	{
		ap->xp2 = $4;
	}
	|	lp disp CM mbid rp
	{	
		ap->xp2 = $2;
	}
	|	lp mbid rp
	{
		ap->xp2 = 0;
	}
	;


mbd	:	LBRKT reg.ss CM disp RBRKT
	{
		mkindex_indirect(POSTNDXMI,0,$2,$4);
	}
	|	LBRKT disp CM reg.ss RBRKT
	{
		mkindex_indirect(POSTNDXMI,0,$4,$2);
	}
	|	LBRKT disp RBRKT
	{
		mkindex_indirect(POSTNDXMI,0,0,$2);
	}
	|	LBRKT reg.ss RBRKT
	{
		mkindex_indirect(POSTNDXMI,0,$2,0);
	}
	;


mbid	:	LBRKT disp CM reg.ss CM reg.ss RBRKT
	{	
		mkindex_indirect(PRENDXMI,$4,$6,$2);
	}
	|	LBRKT reg.ss CM disp CM reg.ss RBRKT
	{ 
		mkindex_indirect(PRENDXMI,$2,$6,$4);
	}
	|	LBRKT reg.ss CM reg.ss CM disp RBRKT
	{
		memindex_indirect(PRENDXMI,$2,$4,$6);
	}
	;


disp	:	expr optsize
	{
		xp->xsize = $2;
		$$ = $1;
	}
	;


reg.ss	:	REG ss
	{
		regseen++;
		if( $2 == SET )		/* mark index register if seen */
			indexreg = regseen;
		$$ = $1;
	}
	;


ss	:	optsize optscl
	{ 
		if( ssflag && ($1 != UNSPEC || $2 == SET) )
		{
			yyerror("only one index register allowed");
			$$ = NOTSET;
		}
		if( $1 == SET || $2 == SET )
			ssflag++;
		$$ = SET;
	}
	;

optscl	:	MUL INT
	{
		switch($2)
		{
			case 1:
				ap->ascale = X1;
				break;
			case 2:
				ap->ascale = X2;
				break;
			case 4:
				ap->ascale = X4;
				break;
			case 8:
				ap->ascale = X8;
				break;
			default:
				yyerror("illegal scale for index register");
				ap->ascale = X1;
		}
		$$ = SET;
	}
	|	/* empty */
	{
		ap->ascale = X1;
		$$ = NOTSET;
	}
	;



lp	:	LP
	{
		ssflag = 0;
		regseen = 0;
		indexreg = 0;
		$$ = $1;
	}
	;

rp	:	RP
	{
		$$ = $1;
	}
	;

expr	:	NAME
	{
		if ((xp->xtype = $1->styp & TYPE) == ABS)
		{
			xp->xvalue = $1->value;
			xp->xsymptr = NULL;
		}
		else
		{
			xp->xvalue = 0;
			xp->xsymptr = $1;
		}
		$$ = xp++;
	}
	|	INT
	{
		xp->xtype = ABS;
		xp->xvalue = $1;
		xp->xsymptr = NULL;
		$$ = xp++;
	}
	|	LP expr RP
	{
		$$ = $2;
	}
	|	expr PLUS expr
	{
		$$ = combine(PLUS,$1,$3);
	}
	|	expr MINUS expr
	{
		$$ = combine(MINUS,$1,$3);
	}
	|	expr MUL expr
	{
		$$ = combine(MUL,$1,$3);
	}
	|	expr DIV expr
	{
		$$ = combine(DIV,$1,$3);
	}
	|	MINUS expr				%prec UMINUS
	{
		xp->xtype = ABS;
		xp->xsymptr = NULL;
		xp->xvalue = 0;
		$$ = combine(MINUS,xp++,$2);
	}
	;

%%

int	type[] = {
	EOF,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	SPC,	NL,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	SPC,	0,	DQ,	SH,	0,	REG,	AMP,	SQ,
	LP,	RP,	MUL,	PLUS,	CM,	MINUS,	SIZE,	DIV,
	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,
	DIGIT,	DIGIT,	COLON,	SEMI,	0,	0,	0,	0,
	0,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,
	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,
	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,
	ALPH,	ALPH,	ALPH,	LBRKT,	0,	RBRKT,	0,	ALPH,
	0,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,
	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,
	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,	ALPH,
	ALPH,	ALPH,	ALPH,	0,	0,	0,	TWID,	0,
};

yylex()
{
	upsymins	istp;
	static BOOL	mnemseen = FALSE; /* Mnemonic seen on line yet?	*/
	register char	*yycharptr;
	register int	val;
	register int	ctype;
	register long	intval;

	while (type[(val = getc(fdin)) + 1] == SPC)
		;

	switch (ctype = type[val + 1])
	{

	case SEMI:	/* Reinitialize for next statement.		*/
		mnemseen = FALSE;
		return(SEMI);

	case SH:	/* Comment until the end of the line.		*/
		while ((val = getc(fdin)) != '\n' && val > 0)
			;
		/* Now same as in case NL.				*/

	case NL:	/* Reinitialize for next statement.		*/
		mnemseen = FALSE;
		return(NL);


	case TWID:
		val = '.';
	case ALPH:
		yycharptr = yytext;
		do
		{
#if FLEXNAMES
			if (yycharptr <= &yytext[BUFSIZ - 1])
#else
			if (yycharptr <= &yytext[NCPS])
#endif
				*yycharptr++ = val;
		} while ((ctype = type[(val = getc(fdin)) + 1]) == ALPH || ctype == DIGIT || ctype == REG);
		*yycharptr = '\0';
		while (val=='\t' || val==' ')
			val = getc(fdin);
		ungetc(val,fdin);
		if (val == ':')
			istp = *lookup(yytext,INSTALL,USRNAME);
		else
			if (mnemseen)
				istp = *lookup(yytext,INSTALL,USRNAME);
			else
			{
				istp = *lookup(yytext,N_INSTALL,MNEMON);
				if (istp.stp == NULL)
					yyerror("invalid instruction name");
				yylval.yinst = istp.itp;
				mnemseen = TRUE;
				return(istp.itp->val + 256);
			}
		yylval.yname = istp.stp;
		return(NAME);

	case REG:
		yycharptr = yytext;
		do
		{
#if FLEXNAMES
			if (yycharptr <= &yytext[BUFSIZ - 1])
#else
			if (yycharptr <= &yytext[NCPS])
#endif
				*yycharptr++ = val;
		} while ((ctype = type[(val = getc(fdin)) + 1]) == ALPH || ctype == DIGIT || ctype == REG);
		ungetc(val,fdin);
		*yycharptr = '\0';
		istp = *lookup(yytext,N_INSTALL,MNEMON);
		if (istp.stp == NULL || istp.itp->val != REG-256)
			yyerror("invalid register name");
		yylval.yinst = istp.itp;
		return(REG);

	case DIGIT:
		intval = val - '0';
		if (val == '0')
		{
			val = getc(fdin);
			if (val == 'x' || val == 'X')
			{	/* hex base	*/
				while (type[(val = getc(fdin)) + 1] == DIGIT ||
					('a' <= val && val <= 'f' || 'A' <= val && val <= 'F'))
				{
					intval <<= 4;
					if ('a' <= val && val <= 'f')
						intval += val - 'a' + 10;
					else if ('A' <= val && val <= 'F')
						intval += val - 'A' + 10;
					else intval += val - '0';
				}
			}	/* hex base	*/
			else
				if (type[val + 1] == DIGIT)
					do
					{	/* octal base	*/
						intval <<= 3;
						intval += val - '0';
					} while (type[(val = getc(fdin)) + 1] == DIGIT);
		}	/* hex or octal base	*/
		else
		{	/* decimal base	*/
			while (type[(val = getc(fdin)) + 1] == DIGIT)
			{
				intval *= 10;
				intval += val - '0';
			}
		}	/* decimal base	*/
		ungetc(val,fdin);
		yylval.yint = intval;
		return(INT);

	case SIZE:
		val = getc(fdin);
		yylval.yint = UNSPEC;
		if (val == 'b' || val == 'B')
			yylval.yint = B;
		else if (val == 'w' || val == 'W')
			yylval.yint = W;
		else if (val == 'l' || val == 'L')
			yylval.yint = L;
		else
			yyerror("unknown size specification");
		return(SIZE);

	case SQ:
		if ((val = getc(fdin)) == '\n')
			line++;
		if (val == '\\')
			switch (val = getc(fdin))
			{
			case 'n':
				val = '\n';
				break;
			case 'r':
				val = '\r';
				break;
			case '\\':
			case '\'':
				break;
			case 'b':
				val = '\b';
				break;
			case 't':
				val = '\t';
				break;
			case 'v':
				val = '\013';
				break;
			case 'f':
				val = '\f';
				break;
			default:
				yyerror("illegal escaped character");
			}
		yylval.yint = val;
		return(INT);

	case DQ:
		yylval.ystr = yycharptr = yytext;
		while ((val = getc(fdin)) != '"' && val != '\n' && val != EOF)
		{
			if (yycharptr < &yytext[BUFSIZ-1])
				*yycharptr++ = val;
		}
		*yycharptr = '\0';
		if (val != '"')
			yyerror("nonterminated string");
		return(STRING);

	case 0:
		yyerror("illegal character");
		return(NOCHAR);

	default:
		return(ctype);
	}
}	/* yylex()	*/



struct exp *
combine(operator,left,right)
int		operator;
register struct exp	*left,
		*right;
{
	switch (operator)
	{
	case PLUS:
		if (left->xsymptr == NULL)
		{
			left->xsymptr = right->xsymptr;
			left->xtype = right->xtype;
		}
		else if (right->xsymptr != NULL)
			yyerror("illegal addition");
		left->xvalue += right->xvalue;
		return(left);
	case MINUS:
		if (swbegct > 0)
			return(left);
		if (left->xsymptr == NULL)
		{
			if (right->xsymptr != NULL)
				yyerror("illegal subtraction");
			left->xvalue -= right->xvalue;
		}
		else if (right->xsymptr == NULL)
			left->xvalue -= right->xvalue;
		else if (left->xtype == right->xtype && left->xtype != UNDEF)
		{
			left->xtype = ABS;
			left->xvalue = left->xvalue - right->xvalue +
				       left->xsymptr->value - right->xsymptr->value;
			left->xsymptr = NULL;		/* no symbol now */
		}
		else
		{
			yyerror("illegal subtraction");
			left->xsymptr = NULL;
			left->xtype = ABS;
			left->xvalue = 0;
		}
		return(left);
	case MUL:
		if (left->xsymptr != NULL || right->xsymptr != NULL)
			yyerror("illegal multiplication");
		left->xsymptr = NULL;
		left->xtype = ABS;
		left->xvalue *= right->xvalue;
		return(left);
	case DIV:
		if (left->xsymptr != NULL || right->xsymptr != NULL)
			yyerror("illegal division");
		left->xsymptr = NULL;
		left->xtype = ABS;
		left->xvalue /= right->xvalue;
		return(left);
	}
}	/* combine()	*/



fill(nbytes)
register long	nbytes;
{
	register long	fillval;

	if (nbytes <= 0)
	    return;
	
	if (newdot & 1)			/* always use FILL to get to even
					** boundary
					*/
	{
	    nbytes--;
	    generate(BITSPBY,0,FILL,NULLSYM);
	}

	fillval = ((dot->styp & TYPE) == TXT) ? TXTFILL : FILL;

	while (nbytes >= 2)
	{
		generate(BITSPOW,0,fillval,NULLSYM);
		nbytes -= 2;
	}
	if (nbytes)			/* fill out odd byte with FILL */
		generate(BITSPBY,0,FILL,NULLSYM);
}	/* fill()	*/



ckalign(size)
register int	size;
{
	register int	mod;

	if ((mod = newdot % size) != 0)
		fill(size - mod);
}	/* ckalign()	*/

/*
 *		68020 SUPPORT FUNCTIONS
 *
 *	mkindex_indirect()	- enters with four arguments:
 *
 *		type (short)
 *		disp (struct *exp)
 *		reg1 (instr *)
 *		reg2 (instr *)
 *
 *	we're not absolutely sure which register is the
 *	base and which is the index when no size or scale
 *	has been specified up to this point, so we need
 *	to determine which is which.  the possible base
 *	registers are:
 *
 *		%an, %pc, %zan (the zero reg placeholder)
 *
 *	and the base register may be missing altogether.
 *
 *	possible index registers are:
 *
 *		%dn, %an, %zan, %zdn
 *
 */

#define NULLREG	(instr *)0


mkindex_indirect(type,reg1,reg2,disp)
short type;
instr	*reg1;
instr	*reg2;
struct	exp	*disp;
{
	instr	*tmpreg;

	/*
	 *	determine index register (if any and not specified)
	 *	and flip registers if necessary so base is reg1 and
	 *	index is reg2.
	 */
	
	if((indexreg == 0 && reg1 && reg1->tag == NULDREG || reg1->tag == ADREG)
		|| indexreg == 1 )
	{
		tmpreg = reg1;
		reg1 = reg2;
		reg2 = tmpreg;
	}

	mk_full_ind(type,reg1,reg2,disp,0);
}


/*
 *	common routine to construct indexed indirect
 *	addressing argument.
 *
 *	we enter with 5 arguments:
 *
 *		type		- type of indirect addressing, 3 kinds
 *		base		- the base register
 *		index		- the index register
 *		base_disp	- the displacement (before indirection)
 *
 *	the regs are preordered before entering this routine so we
 *	need only to check that the regs are the proper types for
 *	the given argument.
 */


mk_full_ind(type,base,index,base_disp)
short	type;
instr	*base;
instr	*index;
struct exp	*base_disp;
{
	/*
	 *	check regs for proper types
	 */

	if( base && base->tag != AAREG || base->tag != PCREG 
		|| base->tag != NULAREG || base->tag != NULPCREG )
	{
		yyerror("base register must be %pc, %an, %zpc or %zan");
		base = NULLREG;		/* try and keep it sane! */
	}

	if( index && index->tag != ADREG || index->tag != AAREG 
		|| index->tag != NULDREG || index->tag != NULAREG )
	{
		yyerror("index register must be %dn, %an, %zdn or %zan");
		index = NULLREG;		/* try and keep it sane! */
	}

	/*
	 *	null out regs tagged NULxxx 
	 */

	if( base && base->tag == NULAREG || base->tag == NULPCREG )
		base = NULLREG;

	if( index && index->tag == NULDREG || index->tag == NULAREG )
		index = NULLREG;

	/*
	 *	set addressing mode, assign regs to argument and return
	 */

	if( base && base->tag == PCREG )
		ap->atype = APCNDX;
	else
		ap->atype = ANDX;

	ap->aflags = type;
	ap->xp1 = base_disp;
	ap->areg1 = base;
	ap->areg2 = index;
}


/*
 *	mk_post_ind():	add in index and displacement for 
 *			post index memory indirect.
 *
 *	obviously if there was an error in the previous
 *	mkindex_indirect() we can overlay an already incorrect
 *	argument descriptor part, not to worry - we're doomed.
 *	let's just continue for syntax checking as long as possible.
 */

mk_post_ind(index,disp)
instr	*index;
struct exp	*disp;
{
	if( index && index->tag != NULDREG || index->tag != ADREG
		|| index->tag != NULAREG || index->tag != AAREG )
	{
		yyerror("only %an,%zan,%dn,%zdn index in memory indirect");
		index = NULLREG;
	}

	if( index && index->tag == NULDREG || index->tag == NULAREG )
		index = NULLREG;

	ap->areg2 = index;
	ap->xp2 = disp;		/* displacement after memory indirection */
}
