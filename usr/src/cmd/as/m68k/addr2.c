/*	@(#)addr2.c	2.1		*/
static	char	sccsid[] = "@(#) addr2.c: 2.1 11/28/83";
/* static	char	sccsid[] = "@(#) addr2.c: 2.8 6/28/83"; */


#include	<stdio.h>
#include	"symbols.h"
#include	"instab.h"
#include	"gendefs.h"
#include	"codeout.h"
#include	"reloc.h"


extern unsigned short
		relent;	/* Number of reloc entries written to fdrel.	*/

extern long	newdot;

extern symbol	symtab[];

extern FILE	*fdrel,	/* File of relocation table information.	*/
		*fdcode;/* File of intermediate object code.		*/

extern BYTE	*longsdi;


extern	setfile(),
	lineno(),
	linenum(),
	lineval(),
	define(),
	endef(),
	setval(),
	settyp(),
	setlno(),
	setscl(),
	settag(),
	setsiz(),
	setdim1(),
	setdim2(),
	newstmt(),
	genreloc(),
	brloc(),
	braopt(),
	bccopt(),
	bsropt(),
	absopt(),
	swbeg(),
	move(),
	iopt(),
	insti(),
	absbr(),
	ndxreloc();


int	(*(modes[NACTION+2]))() =
	{
	0,
	setfile,
	lineno,
	linenum,
	lineval,
	define,
	endef,
	setval,
	settyp,
	setlno,
	setscl,
	settag,
	setsiz,
	setdim1,
	setdim2,
	newstmt,
	genreloc,
	brloc,
	braopt,
	bccopt,
	bsropt,
	absopt,
	swbeg,
	move,
	iopt,
	insti,
	absbr,
	ndxreloc,
	0
	};


genreloc(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	char		*rsym;
	prelent		trelent;
	short		stype;
	long		testval;

	if (sym != NULLSYM)
	{
		code->cvalue += sym->value;
		stype = sym->styp & TYPE;
		testval = code->cvalue;
		if ((code->cnbits == BITSPBY && (testval < -128 || testval > 127)) ||
		    (code->cnbits == BITSPOW && (testval < -32768 || testval > 32767)))
			aerror("size of code generated cannot hold operand");
	}
	else
		return;

	switch (stype)
	{
	case ABS:
		return;
	case TXT:
		rsym = ".text";
		break;
	case DAT:
		rsym = ".data";
		break;
	case BSS:
		rsym = ".bss";
		break;
	case UNDEF:
		sym->styp |= EXTERN;	/* Make sure it gets into the	*/
		rsym = sym->_name.name;	/*	symbol table.		*/
		break;
	default:
		aerror("attempt to relocate weird type");
	}

	trelent.relval = newdot;
	trelent.relname = rsym;
	if (code->cnbits == 8)
		trelent.reltype = R_RELBYTE;
	else if (code->cnbits == 16)
		trelent.reltype = R_RELWORD;
	else
		trelent.reltype = R_RELLONG;
	fwrite((char *)(&trelent),sizeof(prelent),1,fdrel);
	++relent;
}	/* genreloc()	*/



brloc(sym,code)
	register symbol	*sym;
	register codebuf	*code;
{
	prelent		trelent;
	register short	stype;
	register long	offset;

	offset = sym->value - newdot;
	if (code->cnbits == BITSPBY)
		offset -= 1;
	code->cvalue += offset;
	stype = sym->styp & TYPE;

	if (stype == UNDEF)
	{
		sym->styp |= EXTERN;	/* Make sure it gets into the	*/
		trelent.relval = newdot;/*	symbol table.		*/
		trelent.relname = sym->_name.name;
		if (code->cnbits == 8)
			trelent.reltype = R_PCRBYTE;
		else
			trelent.reltype = R_PCRWORD;
		fwrite((char *)(&trelent),sizeof(prelent),1,fdrel);
		++relent;
	}
	else				/* check for valid offset */
	{
	    if (offset < 0)
		offset = ~offset;	/* force sign bits to 0 */
	    if (   (code->cnbits == 8  && (offset & ~0x7fL)   != 0)
		|| (code->cnbits == 16 && (offset & ~0x7fffL) != 0)
		)
		aerror("branch offset is too remote");
	}
}	/* brloc()	*/



braopt(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	if (*++longsdi)
	{	/* Original bra.w is no good, and the second word	*/
		/* is only good for the symbol it contains.  We need	*/
		/* a jmp abs.l to the symbol in the second code chunk.	*/
		codgen(BITSPOW,0x4ef9);			/*	jmp	*/
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		code->cnbits = 2*BITSPOW;
		code->cvalue = 0;
		genreloc(sym,code);
		return;
	}
	/* Otherwise, original bra.w is ok, so codgen gets code as-is.	*/
}	/* braopt()	*/



bccopt(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	if (*++longsdi)
	{	/* Original bcc.w is no good, and the second word	*/
		/* is only good for the symbol it contains.  Negate	*/
		/* the direction of the branch, then use this to branch	*/
		/* around a jmp.  Direction is reversed by toggling	*/
		/* eighth bit.						*/
		code->cvalue ^= 0x100;

		/* Add a displacement which will get past the jmp.	*/
		code->cvalue |= 6;
		codgen(code->cnbits,code->cvalue);
		codgen(BITSPOW,0x4ef9);			/*	jmp	*/
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		code->cnbits = 2*BITSPOW;
		code->cvalue = 0;
		genreloc(sym,code);	
		return;
	}
	/* Otherwise, original bcc.w is ok, so codgen gets code as-is.	*/
}	/* bccopt()	*/



bsropt(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	if (*++longsdi)
	{	/* Original bsr.w is no good, and the second word	*/
		/* is only good for the symbol it contains.  We need	*/
		/* a jsr abs.l to the symbol in the second code chunk.	*/
		codgen(BITSPOW,0x4eb9);			/*	jsr	*/
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		code->cnbits = 2*BITSPOW;
		code->cvalue = 0;
		genreloc(sym,code);
		return;
	}
	/* Otherwise, original bsr.w is ok, so codgen gets code as-is.	*/
}	/* bsropt()	*/



absopt(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	if (*++longsdi)
	{	/* original abs.w is no good.  The mode field needs to	*/
		/* be changed to abs.l, and the next word has to be	*/
		/* expanded to two words.				*/
		code->cvalue |= 0x1;
	}
	codgen(code->cnbits,code->cvalue);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	if (*longsdi)
		code->cnbits = 2*BITSPOW;
	sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
	genreloc(sym,code);
}	/* absopt()	*/



swbeg(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	int		swbegct;
	int		i;
	long		swtabbase;

	codgen(code->cnbits,code->cvalue);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	swbegct = code->cvalue;
	codgen(code->cnbits,code->cvalue);
	/* Swbeg line ends in newline, followed by a line with just	*/
	/* the switch table beginning, then a newline, then "arg"	*/
	/* switch table elements, each accompanied by a newline.	*/
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
	newstmt(sym,code);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
	newstmt(sym,code);
	swtabbase = newdot;	/* The value of the location counter	*/
				/*	for the right hand side of the	*/
				/*	label subtraction expression.	*/

	for (i = 1; i < swbegct; i++)
	{
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		sym = symtab + code->cindex - 1;
		codgen(code->cnbits,sym->value - swtabbase);
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		newstmt(sym,code);
	}

	/* We've completed all but one of the elements of the switch	*/
	/* table.  Codout (the routine which invoked this routine)	*/
	/* will call codgen with what we leave in code.  Therefore, the	*/
	/* last element of the switch table is left in code for codout.	*/

	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	sym = symtab + code->cindex - 1;
	code->cvalue = sym->value - swtabbase;
}	/* swbeg()	*/



move(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	int		mode1;
	int		mode2;

	codgen(code->cnbits,code->cvalue);
	mode1 = code->cvalue & 070;
	mode2 = code->cvalue & 0700;
	if (mode1 > 040)	/* Source has more operand data.	*/
	{
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		if (code->caction == GENRELOC)
		{
			sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
			genreloc(sym,code);
		}
		codgen(code->cnbits,code->cvalue);
	}
	if (mode2 > 0400)	/* Destination has more operand data.	*/
	{
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		if (code->caction == GENRELOC)
		{
			sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
			genreloc(sym,code);
		}
		codgen(code->cnbits,code->cvalue);
	}
	code->cnbits = 0;
}	/* move()	*/



iopt(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	if (*++longsdi)
		code->cvalue |= 0x1;
	codgen(code->cnbits,code->cvalue);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	if (code->caction == GENRELOC)
	{
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		genreloc(sym,code);
	}
	codgen(code->cnbits,code->cvalue);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	if (*longsdi)
		code->cnbits = 2*BITSPOW;
	sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
	genreloc(sym,code);
}	/* iopt()	*/



insti(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	int		mode;

	mode = code->cvalue & 070;
	codgen(code->cnbits,code->cvalue);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	if (code->caction == GENRELOC)
	{
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		genreloc(sym,code);
	}
	if (mode > 040)
	{
		codgen(code->cnbits,code->cvalue);
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		if (code->caction == GENRELOC)
		{
			sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
			genreloc(sym,code);
		}
	}
}	/* insti()	*/



absbr(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	if (*++longsdi)
	{	/* Original pc-relative instruction is no good, so we	*/
		/* convert it to the same instruction using the abs.l	*/
		/* mode.						*/
		code->cvalue ^= 03;
		codgen(code->cnbits,code->cvalue);
		if (fread(code,sizeof(*code),1,fdcode) != 1)
			aerror("unexpected EOF on temporary (code) file");
		code->cnbits = 2*BITSPOW;
		sym = code->cindex ? (symtab + code->cindex - 1) : NULLSYM;
		genreloc(sym,code);
		return;
	}
	codgen(code->cnbits,code->cvalue);
	if (fread(code,sizeof(*code),1,fdcode) != 1)
		aerror("unexpected EOF on temporary (code) file");
	if (code->cindex)
	{
		sym = symtab + code->cindex - 1;
		brloc(sym,code);
	}
}	/* absbr()	*/



ndxreloc(sym,code)
	register symbol	*sym;
	codebuf		*code;
{
	prelent		trelent;

	if (sym == NULLSYM)
		return;
	codgen(BITSPBY,code->cvalue >> BITSPBY);
	code->cvalue &= 0xff;
	code->cnbits = BITSPBY;
	genreloc(sym,code);
}	/* ndxreloc()	*/
