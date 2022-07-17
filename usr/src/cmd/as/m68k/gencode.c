/*	@(#)gencode.c	2.1.1.1		*/
static	char	sccsid[] = "@(#) gencode.c: 2.1.1.1 1/13/84";
/* static	char	sccsid[] = "@(#) gencode.c: 2.11 6/30/83"; */


#include	<stdio.h>
#include	"symbols.h"
#include	"instab.h"
#include	"gendefs.h"
#include	"expand.h"
#include	"expand2.h"


extern long	newdot;
extern short	opt;		/* No span-dependent optimization flag.	*/
extern upsymins	*lookup();

int		as10 = {0};

long 		object;
int		numwords;
long		word;
symbol		*symptr;
unsigned short	action;


eff_add(arg,tag,size)
register struct arg	*arg;
register int		tag;
register int		size;
{
	register int	sdiflag;

	numwords = 0;
	action = 0;
	switch (arg->atype)
	{
	case ADREG:
		if ((tag & 0x800) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (%dn)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case AAREG:
		if ((tag & 0x400) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (%an)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case AIREG:
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case AINC:
		if ((tag & 0x100) == 0)
		{
			yyerror("attempt to use incorrect addressing mode ((%an)+)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case ADEC:
		if ((tag & 0x80) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (-(%an))");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case APCOFF:
		if ((tag & 0x4) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (d(%pc))");
			return;
		}
	case AOFF:
		numwords = 1;
		action = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
		object |= arg->atype;
		if (arg->atype == AOFF)
			object |= arg->areg1->opcode;
		word = arg->xp->xvalue;
		if (word > 32767L || word < -32768L)
			yyerror("displacement exceeds 16 bits");
		symptr = arg->xp->xsymptr;
		return;

	case APCNDX:
		if ((tag & 0x2) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (d(%pc,%rn.A))");
			return;
		}
	case ANDX:
		numwords = 1;
		action = (arg->xp->xsymptr == NULL) ? 0 : NDXRELOC;
		object |= arg->atype;
		if (arg->atype == ANDX)
			object |= arg->areg1->opcode;
		word = (arg->areg2->tag == AAREG) ? (1 << 15) : 0;
		word |= (arg->areg2->opcode << 12);
		word |= (arg->asize == L) ? (1 << 11) : 0;
		word |= (arg->xp->xvalue & 0xff);
		if (arg->xp->xvalue > 127 || arg->xp->xvalue < -128)
			yyerror("index displacement exceeds 8 bits");
		symptr = arg->xp->xsymptr;
		return;

	case ABSW:
	case ABSL:
		if (tag & 0x1000)
		{
			numwords = 2;
			action = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
			object |= ABSL;
			word = arg->xp->xvalue;
			symptr = arg->xp->xsymptr;
			return;
		}
		if (tag & 0x4000)
			sdiflag = SDI6;
		else if (tag & 0x2000)
			sdiflag = SDI5;
		else
			sdiflag = SDI4;
		if (opt)
			switch (shortsdi(arg->xp->xsymptr,arg->xp->xvalue,sdiflag))
		{
		case S_SDI:	/* abs.w guaranteed to work.		*/
			/* If the tag is CONTROL, then the instructions	*/
			/* is a jmp, jsr, pea or lea.  These will use	*/
			/* the pc-relative capability in the case that	*/
			/* the short form works.			*/
			numwords = 1;
			action = (arg->xp->xsymptr == NULL) ? 0 :
				((tag == CONTROL) ? BRLOC : GENRELOC);
			if (tag == CONTROL)
				object |= APCOFF;
			else
				object |= ABSW;
			word = arg->xp->xvalue;
			symptr = arg->xp->xsymptr;
			return;
		case U_SDI:	/* Don't know, so generate a short	*/
				/*	for now.			*/
			/* Again, if the tag is CONTROL, then we use	*/
			/* the pc-relative mode for now, and have a	*/
			/* special action routine handle things in the	*/
			/* second pass.					*/
			numwords = 1;
			if (tag == CONTROL)
			{
				action = ABSBR;
				object |= APCOFF;
			}
			else
			{
				action = ABSOPT;
				object |= ABSW;
			}
			word = arg->xp->xvalue;
			symptr = arg->xp->xsymptr;
			return;
		case L_SDI:
			break;
			/* Fall through to no-optimize code.		*/
		}
		numwords = 2;
		action = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
		object |= ABSL;
		word = arg->xp->xvalue;
		symptr = arg->xp->xsymptr;
		return;

	case AIMM:
		if (tag & 0x1 == 0)
		{
			yyerror("attempt to use incorrect addressing mode (immediate)");
			return;
		}
		object |= AIMM;
		symptr = arg->xp->xsymptr;
		action = (symptr == NULL) ? 0 : GENRELOC;
		switch (size)
		{
		case B:
			numwords = 1;
			word = arg->xp->xvalue & 0xff;
			return;
		case W:
		case UNSPEC:
			numwords = 1;
			word = arg->xp->xvalue & 0xffff;
			return;
		case L:
			numwords = 2;
			word = arg->xp->xvalue;
			return;
		}
	default:
		yyerror("invalid operand type for instruction");
	}
}	/* eff_add()	*/



gen1op(inst,size,arg1)
register instr		*inst;
register int		size;
register struct arg	*arg1;
{
	switch (inst->tag)
	{
	case TRAP:
		if (size != UNSPEC)
			yyerror("trap instruction should have no size attribute");
		if (arg1->atype != AIMM || arg1->xp->xtype != ABS)
			yyerror("trap vector must be absolute, immediate operand");
		if (arg1->xp->xvalue < 0 || arg1->xp->xvalue > 15)
		{
			yyerror("trap vector must be between 0 and 15");
			arg1->xp->xvalue &= 0xf;
		}
		object = inst->opcode | arg1->xp->xvalue;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case STOP:
		if (size != UNSPEC)
			yyerror("stop instruction should have no size attribute");
		if (arg1->atype != AIMM || arg1->xp->xtype != ABS)
			yyerror("stop operand must be absolute, immediate operand");
		if (arg1->xp->xvalue < -(1<<15) || arg1->xp->xvalue >= (1<<15))
		{
			werror("stop operand must fit into a word (masked)");
			arg1->xp->xvalue &= 0xffff;
		}
		generate(BITSPOW,0,inst->opcode,NULLSYM);
		generate(BITSPOW,0,arg1->xp->xvalue,NULLSYM);
		return;

	case SWAP:
		if (size == B || size == L)
			yyerror("only word size attribute allowed for swap instruction");
		if (arg1->atype != ADREG)
			yyerror("swap instruction operand must be a data register");
		object = inst->opcode | arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case EXT:
		if (size == B)
			yyerror("ext instruction only allows word or long size attribute");
		if (arg1->atype != ADREG)
			yyerror("ext instruction operand must be a data register");
		object = inst->opcode | arg1->areg1->opcode;
		object |= (size == L) ? 0xc0 : 0x80;
		generate(BITSPOW,0,object,NULLSYM);
		return;
	
	case UNLK:
		if (size != UNSPEC)
			yyerror("unlk instruction should have no size attribute");
		if (arg1->atype != AAREG)
			yyerror("swap instruction operand must be an address register");
		object = inst->opcode | arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case BRA:
		if (size == B)
		{
			generate(BITSPBY,0,inst->opcode >> BITSPBY,NULLSYM);
			generate(BITSPBY,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		{
			if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI1))
			{
			case S_SDI:	/* bra.w guaranteed to work.	*/
				generate(BITSPOW,0,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case U_SDI:	/* Don't know, so generate a	*/
					/*	short for now.		*/
				generate(BITSPOW,BRAOPT,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case L_SDI:
				break;
				/* Fall through to no-optimize code.	*/
			}
			/* Generate long jump (last resort).	*/
			generate(BITSPOW,0,0x4ef9,NULLSYM);
			generate(2*BITSPOW,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}

	case BR:
		if (size == B)
		{
			generate(BITSPBY,0,inst->opcode >> BITSPBY,NULLSYM);
			generate(BITSPBY,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		{
			if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI2))
			{
			case S_SDI:	/* br.w guaranteed to work.	*/
				generate(BITSPOW,0,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case U_SDI:	/* Don't know, so generate a	*/
					/*	short for now.		*/
				generate(BITSPOW,BCCOPT,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case L_SDI:
				break;
				/* Fall through to no-optimize code.	*/
			}
			/* Negate the direction of the branch,	*/
			/* then use this to branch around a	*/
			/* jmp.  Direction is reversed by	*/
			/* toggling eighth bit.			*/
			object = inst->opcode ^ 0x100;

			/* Add a displacement which will get	*/
			/* past the jmp.			*/
			object |= 6;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,0,0x4ef9,NULLSYM);
			generate(2*BITSPOW,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}

	case BSR:
		if (size == B)
		{
			generate(BITSPBY,0,inst->opcode >> BITSPBY,NULLSYM);
			generate(BITSPBY,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		{
			if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI3))
			{
			case S_SDI:	/* bsr.w guaranteed to work.	*/
				generate(BITSPOW,0,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case U_SDI:	/* Don't know, so generate a	*/
					/*	short for now.		*/
				generate(BITSPOW,BSROPT,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case L_SDI:
				break;
				/* Fall through to no-optimize code.	*/
			}
			/* Generate long jsr (last resort).	*/
			generate(BITSPOW,0,0x4eb9,NULLSYM);
			generate(2*BITSPOW,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
	case RTD:
		if ( !as10 )
			werror("a 68010 instruction");
		if (size != UNSPEC)
			yyerror("rtd is unsized");
		if (arg1->xp->xvalue < -(1<<15) || arg1->xp->xvalue >= (1<<15))
		{
			werror("rtd displacement is 16 bit signed (masked)");
			arg1->xp->xvalue &= 0xffff;
		}
		generate(BITSPOW,0,inst->opcode,NULLSYM);
		if (arg1->atype != AIMM)
			yyerror("rtd instruction requires immediate operand");
		else
			generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg1->xp->xvalue,arg1->xp->xsymptr);
		return;

	case OPTSIZE:		/*	clr, neg, negx, not, tst.	*/
		if (size == UNSPEC)
			size = W;
		object = inst->opcode | (size << 6);
		eff_add(arg1,DATALT,size);
		generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
		return;

	default:	/* jmp, jsr, nbcd, pea, scc, tas		*/
		if (   size != UNSPEC		/* check for correct size */
		    && size != ((int)(unsigned char)inst->tag - DEFAULT))
			yyerror("incorrect size specification for instruction");
		object = inst->opcode;
		if (inst->tag == DEFAULT+B)
			eff_add(arg1,DATALT,size);
		else
			eff_add(arg1,CONTROL,size);
		if (action == BRLOC)
			generate(BITSPOW,0,object,NULLSYM);
		else
			generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
		return;
	}
}	/* gen1op()	*/



gen2op(inst,size,arg1,arg2)
register instr		*inst;
register int		size;
register struct arg	*arg1,
			*arg2;
{
	char	instr_buf[NCPS];

	object = inst->opcode;

	switch (inst->tag)
	{
	case EXG:
	    {
		register unsigned short regs = (arg1->areg1->opcode << 9) | arg2->areg1->opcode;
					/* assume normal register placement */

		if (size != L && size != UNSPEC)
			yyerror("exg requires long attribute, if any");
		if (arg1->atype == ADREG && arg2->atype == ADREG)
			object |= 0x40;		/* Data <==> data.	*/
		else if (arg1->atype == AAREG && arg2->atype == AAREG)
			object |= 0x48;		/* Address <==> address.*/
		else				/* Address <==> data */
		{
			object |= 0x88;
			if (arg1->atype == AAREG) /* must reverse regs. */
			    regs = (arg2->areg1->opcode << 9) | arg1->areg1->opcode;
		}
		object |= regs;
		generate(BITSPOW,0,object,NULLSYM);
		return;
	    }

	case LINK:
		if (size != UNSPEC)
			yyerror("link instruction is unsized");
		if (arg1->atype != AAREG)
			yyerror("address register required as first operand of link instruction");
		else
			object |= arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		if (arg2->atype != AIMM)
			yyerror("immediate operand required as second operand of link instruction");
		else if (arg2->xp->xvalue < -(1<<15) || arg2->xp->xvalue >= (1<<15))
		{
			werror("link displacement must fit into a word (masked)");
			arg2->xp->xvalue &= 0xffff;
		}
			generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg2->xp->xvalue,arg2->xp->xsymptr);
		return;

	case DADD:
		if (size != UNSPEC && size != B)
			yyerror("BCD instructions are byte instructions");
		size = B;
	case XADD:
		if (arg1->atype == ADEC && arg2->atype == ADEC)
			object |= 0x8;
		else if (arg1->atype != ADREG || arg2->atype != ADREG)
		{
			yyerror("only data register or predecrement address register pairs allowed as operands");
			return;
		}
		object |= (arg2->areg1->opcode << 9);
		object |= (size << 6);
		object |= arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case LOGAND:
	case LOGOR:
	case LOGEOR:
		if (arg1->atype == AIMM)
		{
			strcpy(instr_buf,inst->name);
			strcat(instr_buf,"i");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			if (arg2->atype == CCREG)
			{
				object |= 0x3c;
				generate(BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			}
			else if (arg2->atype == SRREG)
			{
				object |= 0x7c;
				generate(BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			}
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			eff_add(arg2,DATALT,size);
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
			if (size == B)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xff,arg1->xp->xsymptr);
			else if (size == W)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xffff,arg1->xp->xsymptr);
			else
				generate(2*BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue,arg1->xp->xsymptr);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		if (size == UNSPEC)
			size = W;
		if (inst->tag == LOGEOR)
		{
			if (arg1->atype != ADREG)
			{
				yyerror("first operand for eor must be a data register");
				return;
			}
			object |= (arg1->areg1->opcode << 9);
			object |= (1 << 8);
			object |= (size << 6);
			eff_add(arg2,DATALT,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg2->atype == ADREG)
		{
			object |= (arg2->areg1->opcode << 9);
			object |= (size << 6);
			eff_add(arg1,DATAA,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else
		{
			object |= (arg1->areg1->opcode << 9);
			object |= (1 << 8);
			object |= (size << 6);
			eff_add(arg2,ALTMEM,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}

	case ASHFT:
	case LSHFT:
	case ROT:
	case ROTX:
		if (arg1->atype == AIMM && 
		    arg2->atype != ADREG && arg2->atype != AAREG &&
		    arg2->atype != APCOFF && arg2->atype != APCNDX &&
		    arg2->atype != AIMM)
		{
			if(arg1->xp->xvalue != 1)
				yyerror("rotate/shift of memory only by 1");
			if (size != W && size != UNSPEC)
				yyerror("rotate/shift of memory requires word attribute, if any");
			eff_add(arg2,ALTMEM,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else
		{
			/* Turn off the immedate, size and i/r	*/
			/*	fields so they can be set.	*/
			if(arg2->atype != ADREG)
				yyerror("rotate/shift greater than 1 req's (%d) register");
			object &= ~0xeff;
			if (inst->tag == LSHFT)
				object |= 0x8;
			else if (inst->tag == ROT)
				object |= 0x18;
			else if (inst->tag == ROTX)
				object |= 0x10;
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			if (arg1->atype == ADREG)
			{
				object |= 0x20;
				object |= (arg1->areg1->opcode << 9);
				object |= arg2->areg1->opcode;
				generate(BITSPOW,0,object,NULLSYM);
				return;
			}
			else if (arg1->atype == AIMM)
			{
				object |= arg2->areg1->opcode;

				/* Shift amount has upper limit of 8.	*/
				/* Eight is encoded as 0 (since a shift	*/
				/* by zero makes little sense).  Shifts	*/
				/* are broken up into groups of shift-	*/
				/* by-eight plus any remaining shift.	*/
				/* Note that the shift amount must be	*/
				/* known in the first pass.		*/

				while (arg1->xp->xvalue >= 8)
				{
					generate(BITSPOW,0,object,NULLSYM);
					arg1->xp->xvalue -= 8;
				}
				if (arg1->xp->xvalue > 0)
				{
					object |= (arg1->xp->xvalue << 9);
					generate(BITSPOW,0,object,NULLSYM);
				}
				return;
			}
			else
				yyerror("invalid operands for shift/rotate instruction");
			return;
		}

	case LEA:
		if (size != UNSPEC && size != L)
			yyerror("lea instruction has incorrect size attribute");
		if (arg2->atype != AAREG)
			yyerror("lea instruction requires an address register for second operand");
		else
			object |= (arg2->areg1->opcode << 9);
		eff_add(arg1,CONTROL,size);
		if (action == BRLOC)
			generate(BITSPOW,0,object,NULLSYM);
		else
			generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
		return;

	case MULT:
		if (size != UNSPEC && size != W)
			yyerror("instruction has incorrect size attribute");
		if (arg2->atype != ADREG)
			yyerror("instruction requires data register for second operand");
		else
			object |= (arg2->areg1->opcode << 9);
		eff_add(arg1,DATAA,size);
		generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
		return;

	case DBCC:
		if (size != UNSPEC && size != W)
			yyerror("dbCC instruction requires word attribute, if any");
		if (arg1->atype != ADREG)
			yyerror("dbCC instruction requires data register for first operand");
		else
			object |= arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		generate(BITSPOW,BRLOC,arg2->xp->xvalue,arg2->xp->xsymptr);
		return;

	case CMP:
		/* Keep in mind the syntactic change in the operand	*/
		/*	ordering.					*/
		if (size == UNSPEC)
		{
			yyerror("size needed on compare instruction");
			size = W;
		}
		if (arg1->atype == AINC && arg2->atype == AINC)
		{
			/*		cmpm				*/
			object |= (arg1->areg1->opcode << 9);
			object |= 0x108;
			object |= (size << 6);
			object |= arg2->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg1->atype == AAREG)
		{
			/*		cmpa				*/
			if (size == B)
			{
				yyerror("byte size attribute illegal for compare address instruction");
				return;
			}
			object |= (arg1->areg1->opcode << 9);
			object |= (size == W) ? 0xc0 : 0x1c0;
			eff_add(arg2,ALL,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg2->atype == AIMM)
		{
			/*		cmpi				*/
			strcpy(instr_buf,inst->name);
			strcat(instr_buf,"i");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			object |= (size << 6);
			if (size == L)
				eff_add(arg1,DATALT | 0x4000,size);
			else
				eff_add(arg1,DATALT | 0x2000,size);
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
			if (size == B)
				generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg2->xp->xvalue & 0xff,arg2->xp->xsymptr);
			else if (size == W)
				generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg2->xp->xvalue & 0xffff,arg2->xp->xsymptr);
			else
				generate(2*BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg2->xp->xvalue,arg2->xp->xsymptr);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == ADREG)
		{
			object |= (arg1->areg1->opcode << 9);
			object |= (size << 6);
			eff_add(arg2,ALL,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else
		{
			yyerror("invalid operands for compare instruction");
			return;
		}

	case BCHG:
	case BTST:
		if (size != UNSPEC)
			yyerror("bit instructions should not have a size attribute");
		if (arg1->atype == ADREG)
		{
			object |= (arg1->areg1->opcode << 9);
			if (inst->tag == BTST)
				eff_add(arg2,DATNIM,size);
			else
				eff_add(arg2,DATALT,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
		}
		else if (arg1->atype == AIMM)
		{
			/* Object code for bit instructions is 0x01?0	*/
			/*	when the bit number is dynamic (in a	*/
			/*	register), but 0x08?0 for a static bit	*/
			/*	number (bit number is immediate).	*/
			/*	Object code is converted by turning off	*/
			/*	the 1 bit and turning on the 8 bit.	*/
			object ^= 0x0100;
			object |= 0x0800;
			if (inst->tag == BTST)
				eff_add(arg2,DATNIM,size);
			else
				eff_add(arg2,DATALT,size);
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
			generate(BITSPOW,
				(arg1->xp->xsymptr == NULLSYM) ? 0 : GENRELOC,
				arg1->xp->xvalue,arg1->xp->xsymptr);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
		}
		else
			yyerror("incorrect first operand to bit instruction");
		return;

	case ADD:
		if (arg1->atype == AIMM &&
			    arg1->xp->xsymptr == NULLSYM &&
			    arg1->xp->xvalue > 0 &&
			    arg1->xp->xvalue <= 8)
		{
			/* addq/subq */
			strcpy(instr_buf,inst->name);
			strcat(instr_buf,"q");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			if (arg1->xp->xvalue != 8)
				object |= (arg1->xp->xvalue << 9);
			if (size == UNSPEC)
				size = W;
			if (size == B && arg2->atype == AAREG)
			{
				yyerror("additive instruction with address destination cannot have byte attribute");
				size = W;
			}
			object |= (size << 6);
			eff_add(arg2,ALT,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg2->atype == AAREG)
		{
			/*		adda/suba		*/
			if (size == B)
				yyerror("additive instruction with address destination cannot have byte attribute");
			object |= (arg2->areg1->opcode << 9);
			object |= (size == L) ? 0x1c0 : 0xc0;
			eff_add(arg1,ALL,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == AIMM)
		{
			/* addi, where destination is not %an, and
			** literal is not between 1 and 8
			*/
			strcpy(instr_buf,inst->name);
			strcat(instr_buf,"i");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			if (size == L)
				eff_add(arg2,DATALT | 0x4000,size);
			else
				eff_add(arg2,DATALT | 0x2000,size);
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
			if (size == B)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xff,arg1->xp->xsymptr);
			else if (size == W)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xffff,arg1->xp->xsymptr);
			else
				generate(2*BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue,arg1->xp->xsymptr);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg2->atype == ADREG)
		{
			object |= (arg2->areg1->opcode << 9);
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			eff_add(arg1,ALL,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == ADREG)
		{
			object |= (arg1->areg1->opcode << 9);
			object |= 0x100;
			object |= (size << 6);
			eff_add(arg2,ALTMEM,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else
			yyerror("illegal operand combination for additive instruction");
		return;

	case MOVM:
		if (size == B)
			yyerror("movm instruction allows only word and long attribute");
		if (size == L)
			object |= 0x40;
		if (arg2->atype == AIMM)
		{
			object |= 0x400;
			eff_add(arg1,CONPOST | 0x2000,size);
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
			generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg2->xp->xvalue,arg2->xp->xsymptr);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
		}
		else if (arg1->atype != AIMM)
			yyerror("incorrect operands for movm instruction");
		else
		{
			eff_add(arg2,CONPRE | 0x2000,size);
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
			generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg1->xp->xvalue,arg1->xp->xsymptr);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
		}
		return;

	case MOVS:
		{
			register int inst2wd;

			if( !as10 )
				werror("a 68010 instruction");
			if ((inst2wd=(arg1->atype == AAREG)) || arg1->atype == ADREG)
			{
				eff_add(arg2,ALTMEM,size);
				inst2wd = (inst2wd << 15) | (arg1->areg1->opcode<< 12) | (1 << 11);
			}
			else if ((inst2wd=(arg2->atype == AAREG)) || arg2->atype == ADREG)
			{
				eff_add(arg1,ALTMEM,size);
				inst2wd = (inst2wd << 15) | (arg2->areg1->opcode<< 12);
			}
			else
				yyerror("one register operand req'd for movs");
			object |= (size == UNSPEC ? W : size) << 6;
			generate(BITSPOW,action,object,NULLSYM);
			generate(BITSPOW,0,inst2wd,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}

	case MOVP:
		if (size == B)
			yyerror("movp instruction allows only word and long attribute");
		if (arg1->atype == ADREG && arg2->atype == AOFF)
		{
			object |= (arg1->areg1->opcode << 9);
			object |= (((size == L) ? 7 : 6) << 6);
			object |= arg2->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg2->xp->xvalue,arg2->xp->xsymptr);
		}
		else if (arg1->atype == AOFF && arg2->atype == ADREG)
		{
			object |= (arg2->areg1->opcode << 9);
			object |= (((size == L) ? 5 : 4) << 6);
			object |= arg1->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg1->xp->xvalue,arg1->xp->xsymptr);
		}
		else
			yyerror("incorrect operands for movp instruction");
		return;

	case MOV:
		if (arg2->atype == CCREG)
		{
			object = 0x44c0;
			eff_add(arg1,DATAA,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == CCREG)
		{
			if( !as10 )
				werror("a 68010 instruction");
			object = 0x42c0;
			eff_add(arg2,DATALT,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == CTLREG )
		{
			if (size == UNSPEC )
				size = L;
			else if(size != L)
				yyerror("only long size attribute allowed");
			if (arg2->atype == AAREG)
			{
				if (arg1->areg1->opcode == USPREG)
				{	/* old form is one word */
					object = 0x4e68;
					object |= arg2->areg1->opcode;
					generate(BITSPOW,0,object,NULLSYM);
					return;
				}
				object = (0x8 | arg2->areg1->opcode) << 12;
			}
			else if (arg2->atype == ADREG)
				object = (arg2->areg1->opcode) << 12;
			else
				yyerror("destination operand must be a register");
			if( !as10 )
				werror("a 68010 instruction");
			object |= arg1->areg1->opcode;
			generate(BITSPOW,0,0x4e7a,NULLSYM);
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg2->atype == CTLREG )
		{
			if (size == UNSPEC )
				size = L;
			else if(size != L)
				yyerror("only long size attribute allowed");
			if (arg1->atype == AAREG)
			{
				if (arg2->areg1->opcode == USPREG)
				{
					object = 0x4e60;
					object |= arg1->areg1->opcode;
					generate(BITSPOW,0,object,NULLSYM);
					return;
				}
				object = (0x8 | arg1->areg1->opcode) << 12;
			}
			else if (arg1->atype == ADREG)
				object = (arg1->areg1->opcode) << 12;
			else
				yyerror("source operand must be a register");
			if( !as10 )
				werror("a 68010 instruction");
			object |= arg2->areg1->opcode;
			generate(BITSPOW,0,(0x4e7a | 0x1),NULLSYM);
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg2->atype == SRREG)
		{
			object = 0x46c0;
			eff_add(arg1,DATAA,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == SRREG)
		{
			object = 0x40c0;
			eff_add(arg2,DATALT,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else if (arg1->atype == AIMM && arg2->atype == ADREG &&
			 (size == L || size == UNSPEC) &&
			 arg1->xp->xsymptr == NULLSYM &&
			 arg1->xp->xvalue >= -128 && arg1->xp->xvalue <= 127)
		{
			object = 0x7000;
			object |= (arg2->areg1->opcode << 9);
			object |= (arg1->xp->xvalue & 0xff);
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg2->atype == AAREG)
		{
			object = 0x0040;
			if (size == B)
				yyerror("mov to address register cannot have byte attribute");
			object |= (((size == L) ? 0x2 : 0x3) << 12);
			object |= (arg2->areg1->opcode << 9);
			eff_add(arg1,ALL,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			return;
		}
		else
		{
			register long		savword;
			register int		savnum;
			register unsigned short	savaction;
			register symbol		*savsym;

			if (size == UNSPEC)
				size = W;
			savword = inst->opcode;
			eff_add(arg2,DATALT | 0x1000,size);
			/* Pick off mode and register for destination,	*/
			/*	and move them up to where they belong.	*/
			savword |= ((object & 07) << 9);
			savword |= ((object & 070) << 3);
			object = savword;
			savnum = numwords;
			savaction = action;
			savsym = symptr;
			savword = word;
			if (size == B)
				object |= 0x1000;
			else if (size == W)
				object |= 0x3000;
			else
				object |= 0x2000;
			eff_add(arg1,ALL | 0x1000,size);
			if (action == 0 && savaction == 0)
				generate(BITSPOW,0,object,NULLSYM);
			else
				generate(BITSPOW,MOVE,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			if (savnum == 1)
				generate(BITSPOW,savaction,savword,savsym);
			else if (savnum == 2)
				generate(2*BITSPOW,savaction,savword,savsym);
			return;
		}
	default:
		yyerror("unknown two-operand instruction usage");
	}
}	/* gen2op()	*/




flags(flag)
char	flag;
{
	char	errmsg[28];

	sprintf(errmsg,"illegal flag (%c) - ignored",flag);
	werror(errmsg);
}	/* flag()	*/
