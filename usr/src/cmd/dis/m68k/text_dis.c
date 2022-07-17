/*	@(#)text_dis.c	2.1		*/
/* MC68000 Disassembler */

	static char SCCSID[] = "@(#) text_dis.c:	2.9 83/07/08";

#include	<stdio.h>
#include	<setjmp.h>
#include	<filehdr.h>
#include	<ldfcn.h>
#include	<scnhdr.h>
#include	"dis.h"
#include	<sgs.h>		/* for definition of SGS */

#define		BYTE		1 /* values for size parm to eff_add */
#define		WORD		2
#define		LONG		3

#define		SIGNED		1 /* immediate value signed or unsigned */
#define		UNSIGNED	0

#define		MAXERRS	10	/* Maximum number of errors allowed	*/
				/* before abandoning this disassembly	*/
				/* as a hopeless case.			*/

static int	errlev = 0;	/* Errors encountered during the	*/
				/* disassembly, probably due to being	*/
				/* out of sync.				*/

unsigned short curinstr;	/* for saving first part of instruction
				   when cur2bytes is used to read displ */
unsigned short oldinstr = 0;	/* to save previous instruction for
				   testing that swbeg follows jump	*/

jmp_buf env;			/* used by set/longjmp when an error is
				   located down in a subroutine these
				   routines allow continuing after resync */

/*	From dis_extn.c.	*/
extern SCNHDR	scnhdr;
extern LDFILE	*t_ptr;
extern int	Lflag;
extern long	loc;
extern char	mneu[];
extern char	*sname;
extern char	*fname;
extern unsigned short	cur2bytes;

/*	From dis_utls.c.	*/
extern		convert();
extern int	get2bytes();
extern		line_nums();
extern		looklabel();
extern		printline();
extern		prt_offset();
extern		resync();
extern short	sect_name();

extern void	bit_movep_imm();
extern void	move_byte();
extern void	move_long();
extern void	move_word();
extern void	miscell();
extern void	addq_subq_scc_dbcc();
extern void	bcc_bsr_bra();
extern void	moveq();
extern void	or_div_sbcd();
extern void	sub_subx();
extern void	unassigned();
extern void	cmp_eor();
extern void	and_mul_abcd_exg();
extern void	add_addx();
extern void	shft_rot();

extern void	move_address();
extern void	recover();
extern char	*eff_add();


char	conv_temp[NHEX];	/* Temporary location for ascii	*/
				/* representation of operands.	*/

char	comp_temp[NHEX];	/* temp for calling compoff	*/

char	size[] = {'b','w','l'};

char	*cond_codes[] =
	{
		"t      ",	"f      ",	"hi     ",	"ls     ",
		"cc     ",	"cs     ",	"ne     ",	"eq     ",
		"vc     ",	"vs     ",	"pl     ",	"mi     ",
		"ge     ",	"lt     ",	"gt     ",	"le     "
	};

char 	*addregs[] = 
	{ "%a0","%a1","%a2","%a3","%a4","%a5","%fp","%sp" };

dis_text()
{
	short sect_num;
	static void (*code_map[16])() =
	{
		bit_movep_imm,
		move_byte,
		move_long,
		move_word,
		miscell,
		addq_subq_scc_dbcc,
		bcc_bsr_bra,
		moveq,
		or_div_sbcd,
		sub_subx,
		unassigned,
		cmp_eor,
		and_mul_abcd_exg,
		add_addx,
		shft_rot,
		unassigned
	};
	if (Lflag > 0)
		sect_num = sect_name();

	for (loc = scnhdr.s_paddr; setjmp(env),
		((loc-scnhdr.s_paddr) < scnhdr.s_size) && (errlev < MAXERRS); 
		printline())
	{
		if (Lflag > 0)
			looklabel(loc,sect_num);
		line_nums(scnhdr.s_nlnno);
		prt_offset();
		mneu[0] = '\0';
		oldinstr = curinstr;
		get2bytes(t_ptr);
		curinstr = cur2bytes;	/* save bytes in case eff_add changes it									*/
		comp_temp[0] = '\0';
		(*code_map[HI4OF16(cur2bytes)])();
		strcat(mneu,comp_temp);	/* if there was any pc rel computation 
					   put it at the end of assembly line */
	}
}	/* dis_text() */


void
move_address()
{
	strcat(mneu,"mova._  ");
	mneu[5] = BIT12(cur2bytes) ? 'w' : 'l';
	strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
		BIT12(cur2bytes) ? WORD : LONG,SIGNED));
	sprintf(mneu,"%s,%s",mneu,addregs[BITS11_9(curinstr)]);
}

void
bit_movep_imm()
{ 
	static char *misc_ops[4] = {
			"btst    ",
			"bchg    ",
			"bclr    ",
			"bset    "
		};
	if (BIT8(cur2bytes)) {
		if (BITS5_3(cur2bytes) == 1) {	/* movep */
			strcat(mneu,"movp._  ");
			mneu[5] = BIT6(cur2bytes) ? 'l' : 'w';
			if (BIT7(cur2bytes))
				sprintf(mneu,"%s%%d%o,%s",mneu,
					BITS11_9(curinstr),
					eff_add(5,BITS2_0(cur2bytes),
						NULL,NULL));
			else
				sprintf(mneu,"%s%s,%%d%o",mneu,
					eff_add(5,BITS2_0(cur2bytes),NULL,
						NULL), BITS11_9(curinstr));
			}
		else {	/* dynamic bit */
			strcat(mneu,misc_ops[BITS7_6(cur2bytes)]);
			sprintf(mneu,"%s%%d%o,%s",mneu,BITS11_9(curinstr),
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					NULL,NULL));
			}
		return;
		} /* end if (BIT8(cur2bytes)) */
	switch(BITS11_9(cur2bytes)) {
	char add_temp[16];
	case 0:
		if (BITS7_6(cur2bytes) == 3) {
			fprintf(stderr,"\n%s%s ERROR : illegal bit setting for or immediate instruction %x\n",
				SGS,TOOL,cur2bytes);
			recover();
			}
		strcat(mneu,"ori._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(7,4,BITS7_6(cur2bytes) + 1,UNSIGNED));
		strcat(mneu,",");
		if (BITS5_0(curinstr) == 074) {
			if (BITS7_6(curinstr) == 0)
				strcat(mneu,"%cc");
			else if BITS7_6(curinstr == 1)
				strcat(mneu,"%sr");
			else {
				fprintf(stderr,"\n%s%s ERROR : illegal bit setting for or immediate instruction %x\n",
					SGS,TOOL,curinstr);
				recover();
				}
			}
		else
			strcat(mneu,eff_add(BITS5_3(curinstr),
				BITS2_0(curinstr),NULL,NULL));
		return;
	case 1:
		if (BITS7_6(cur2bytes) == 3) {
			fprintf(stderr,"\n%s%s ERROR : illegal bit setting for and immediate instruction %x\n",
				SGS,TOOL,cur2bytes);
			recover();
			}
		strcat(mneu,"andi._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(7,4,BITS7_6(cur2bytes) + 1,UNSIGNED));
		strcat(mneu,",");
		if (BITS5_0(curinstr) == 074) {
			if (BITS7_6(curinstr) == 0)
				strcat(mneu,"%cc");
			else if (BITS7_6(curinstr) == 1)
				strcat(mneu,"%sr");
			else {
				fprintf(stderr,"\n%s%s ERROR : illegal bit setting for and immediate instruction %x\n",
					SGS,TOOL,curinstr);
				recover();
				}
			}
		else
			strcat(mneu,eff_add(BITS5_3(curinstr),
				BITS2_0(curinstr),NULL,NULL));
		return;
	case 2:
		if ((BITS7_6(cur2bytes) == 3) || (BITS5_0(cur2bytes) == 074)) {
			fprintf(stderr,"\n%s%s ERROR : illegal bit settings for subi instruction %x\n",
				SGS,TOOL,cur2bytes);
			recover();
			}
		strcat(mneu,"subi._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(7,4,BITS7_6(cur2bytes) + 1,UNSIGNED));
		strcat(mneu,",");
		strcat(mneu,eff_add(BITS5_3(curinstr),BITS2_0(curinstr),
					NULL,NULL));
		return;
	case 3:
		if ((BITS7_6(cur2bytes) == 3) || (BITS5_0(cur2bytes) == 074)) {
			fprintf(stderr,"\n%s%s ERROR : illegal bit settings for addi instruction %x\n",
				SGS,TOOL,cur2bytes);
			recover();
			}
		strcat(mneu,"addi._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(7,4,BITS7_6(cur2bytes) + 1,UNSIGNED));
		strcat(mneu,",");
		strcat(mneu,eff_add(BITS5_3(curinstr),BITS2_0(curinstr),
					NULL,NULL));
		return;
	case 4:
			strcat(mneu,misc_ops[BITS7_6(cur2bytes)]);
			strcat(mneu,eff_add(7,4,(BITS7_6(cur2bytes) ==1) ?
						BYTE : WORD,UNSIGNED));
			strcat(mneu,",");
			strcat(mneu,eff_add(BITS5_3(curinstr),
					BITS2_0(curinstr),NULL,NULL));
			return;
	case 5:
		if (BITS7_6(cur2bytes) == 3) {
			fprintf(stderr,"\n%s%s ERROR : illegal bit setting for exclusive or immediate instruction %x\n",
				SGS,TOOL,cur2bytes);
			recover();
			}
		strcat(mneu,"eori._   ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(7,4,BITS7_6(cur2bytes) + 1,UNSIGNED));
		strcat(mneu,",");
		if (BITS5_0(curinstr) == 074) {
			if (BITS7_6(curinstr) == 0)
				strcat(mneu,"%cc");
			else if BITS7_6(curinstr == 1)
				strcat(mneu,"%sr");
			else {
				fprintf(stderr,"\n%s%s ERROR : illegal bit setting for exclusive or immediate instruction %x\n",
					SGS,TOOL,curinstr);
				recover();
				}
			}
		else
			strcat(mneu,eff_add(BITS5_3(curinstr),
				BITS2_0(curinstr),NULL,NULL));
		return;
	case 6:
		if ((BITS7_6(cur2bytes) == 3) || (BITS5_0(cur2bytes) == 074)) {
			fprintf(stderr,"\n%s%s ERROR : illegal bit settings for cmpi instruction %x\n",
				SGS,TOOL,cur2bytes);
			recover();
			}
		strcat(mneu,"cmpi._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		strcpy(add_temp,eff_add(7,4,BITS7_6(cur2bytes)+1,SIGNED));
		strcat(mneu,eff_add(BITS5_3(curinstr),BITS2_0(curinstr),
					NULL,NULL));
		strcat(mneu,",");
		strcat(mneu,add_temp);
		return;
	case 7:
		fprintf(stderr,"\n%s%s ERROR : impossible bit settings for an instruction %x\n",
			SGS,TOOL,cur2bytes);
		recover();
	}
}


void
move_byte()
{
	if (BITS8_6(cur2bytes) == 0x1L ) {
		fprintf(stderr,"\n%s%s ERROR : impossible bit settings on move instruction %x\n",
			SGS, TOOL, cur2bytes);
		recover();
		}
	else {
		strcat(mneu,"mov.b   ");
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),BYTE,
			SIGNED));
		strcat(mneu,",");
		strcat(mneu,eff_add(BITS8_6(curinstr),BITS11_9(curinstr),NULL,
			NULL));
		}
}	/* move_byte()	*/



void
move_long()
{
	if (BITS8_6(cur2bytes) == 0x1L )
		move_address();
	else {
		strcat(mneu,"mov.l   ");
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),LONG,
			SIGNED));
		strcat(mneu,",");
		strcat(mneu,eff_add(BITS8_6(curinstr),BITS11_9(curinstr),NULL,
			NULL));
	}
}	/* move_long()	*/

void
move_word()
{
	if (BITS8_6(cur2bytes) == 0x1L )
		move_address();
	else {
		strcat(mneu,"mov.w   ");
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),WORD,
			SIGNED));
		strcat(mneu,",");
		strcat(mneu,eff_add(BITS8_6(curinstr),BITS11_9(curinstr),NULL,
			NULL));
	}
}	/* move_word()	*/



void
miscell()
{
	if (BIT8(cur2bytes))
	{
		if (!BIT7(cur2bytes))
		{
			fprintf(stderr, "\n%s%s ERROR: impossible bit setting for instruction - %x\n",
				SGS, TOOL, cur2bytes);
			recover();
		}
		if (BIT6(cur2bytes))
			sprintf(mneu,"lea.l   %s,%s",eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),LONG,UNSIGNED),
				addregs[BITS11_9(curinstr)]);
		else
			sprintf(mneu,"chk.w   %s,%%d%o",
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					WORD,UNSIGNED),BITS11_9(curinstr));
		return;
	}

	switch (BITS11_9(cur2bytes))
	{
	case 0:
		if (BITS7_6(cur2bytes) == 3) {
			strcat(mneu,"mov.w   %sr,");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
			}
		else {
			strcat(mneu,"negx._  ");
			mneu[5] = size[BITS7_6(cur2bytes)];
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
			}
		return;
	case 1:
		if (BITS7_6(cur2bytes) == 3) {
			fprintf(stderr,"\n%s%s ERROR : impossible bit settings for clr instruction %x \n",
				SGS, TOOL, cur2bytes);
			recover();
			}
		strcat(mneu,"clr._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
			NULL,NULL));
		return;
	case 2:
		if (BITS7_6(cur2bytes) == 3)
		{
			strcat(mneu,"mov.w   ");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),WORD,UNSIGNED));
			strcat(mneu,",%%cc");
			return;
		}
		strcat(mneu,"neg._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
			NULL,NULL));
		return;
	case 3:
		if (BITS7_6(cur2bytes) == 3)
		{
			strcat(mneu,"mov.w   ");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),WORD,UNSIGNED));
			strcat(mneu,",%sr");
			return;
		}
		strcat(mneu,"not._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
			NULL,NULL));
		return;
	case 5:
		if (BITS7_6(cur2bytes) == 3)
		{
			if (BITS5_0(cur2bytes) == 074) {
				int i;
				if (BITS15_7(oldinstr) == 0235){
					get2bytes(t_ptr);
					curinstr = cur2bytes;
					convert(cur2bytes,conv_temp,LEAD);
					sprintf(mneu,"swbeg   &%s",conv_temp);
					for(i=1;i<=curinstr;i++) {
						printline();
						printf("\t");
							/* no need to call line_							nums - just need tab */
						mneu[0] = '\0';
						prt_offset();
						strcat(mneu,eff_add(7,4,WORD,
							SIGNED));
						}
					return;
					}
				else {
					sprintf(mneu,"illegal ");
					return;
					}
				}
			strcat(mneu,"tas.b   ");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
			return;
		}
		strcat(mneu,"tst._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
			NULL,NULL));
		return;
	case 4:
		switch(BITS7_6(cur2bytes)) {
		case 0:
			strcat(mneu,"nbcd.b  ");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
			return;
		case 1:
			if (BITS5_3(cur2bytes)) {
				strcat(mneu,"pea.l   ");
				strcat(mneu,eff_add(BITS5_3(cur2bytes),
					BITS2_0(cur2bytes),NULL,NULL));
				return;
				}
			else {
				sprintf(mneu,"swap.w  %%d%o",
					BITS2_0(cur2bytes));
				return;
				}
		case 2:
			if (!BITS5_3(cur2bytes)) {
				sprintf(mneu,"ext.w   %%d%o",
					BITS2_0(cur2bytes));
				return;
				}
			/* movem falls thru here */
		case 3:
			if (!BITS5_3(cur2bytes)) {
				sprintf(mneu,"ext.l   %%d%o",
					BITS2_0(cur2bytes));
				return;
				}
			/* movem falls thru here */
		} /* end switch BITS7_6 */
	case 6:
		{
		unsigned short mask;		/* gets movm.[wl] mask */
		char add_temp[25];
		add_temp[0] = '\0';
		if (!BIT7(cur2bytes)) {
			fprintf(stderr,"\n%s%s ERROR : impossible bit settings for movm.S instruction %x \n",
				SGS, TOOL, cur2bytes);
			recover();
			}
		get2bytes(t_ptr);		/* read mask and save */
		mask = cur2bytes;
		strcpy(mneu,"movm._  ");
		mneu[5] = BIT6(curinstr) ? 'l' : 'w';
		strcat(add_temp,eff_add(BITS5_3(curinstr),BITS2_0(curinstr),
			NULL,NULL));
		if(BIT10(curinstr))
			sprintf(mneu,"%s%s,",mneu,add_temp);
		strcat(mneu,"&");
		convert(mask, conv_temp,LEAD);
		strcat(mneu,conv_temp);
		if (!BIT10(curinstr))
			sprintf(mneu,"%s,%s",mneu,add_temp);
		return;
		}
	case 7:
		switch (BITS7_6(cur2bytes))
		{
		case 0:
			fprintf(stderr,"\n%s%s ERROR: impossible bit setting for instruction - %x\n",
				SGS, TOOL, cur2bytes);
			recover();
		case 1:
			switch (BITS5_4(cur2bytes))
			{
			case 0:
				strcat(mneu,"trap    &");
				convert(BITS3_0(cur2bytes),conv_temp,LEAD);
				strcat(mneu,conv_temp);
				return;
			case 1:
				if (BIT3(cur2bytes))
					sprintf(mneu,"unlk    %s",
						addregs[BITS2_0(cur2bytes)]);
				else
				{
					sprintf(mneu,"link    %s,&",
						addregs[BITS2_0(cur2bytes)]);
					get2bytes(t_ptr);
					if (BIT15(cur2bytes))
						strcat(mneu,"-");
					convert(BIT15(cur2bytes) ?
						-(short)cur2bytes : cur2bytes,
						conv_temp,LEAD);
					strcat(mneu,conv_temp);
				}
				return;
			case 2:
				if (BIT3(cur2bytes))
					sprintf(mneu,"mov.l   %%usp,%s",
						addregs[BITS2_0(cur2bytes)]);
				else
					sprintf(mneu,"mov.l   %s,%%usp",
						addregs[BITS2_0(cur2bytes)]);
				return;
			case 3:
			{
				static char	*misc_ops[8] =
					{
						"reset",
						"nop",
						"stop    &",
						"rte",
						"",
						"rts",
						"trapv",
						"rtr"
					};

				if (BIT3(cur2bytes) || (BITS2_0(cur2bytes) ==4))
				{
					fprintf(stderr, "\n%s%s ERROR : impossible bit setting for instruction - %x\n",
						SGS, TOOL, cur2bytes);
					recover();
				}
				strcat(mneu,misc_ops[BITS2_0(cur2bytes)]);
				if (BITS2_0(cur2bytes) == 2)	/* stop	*/
				{
					get2bytes(t_ptr);
					convert(cur2bytes,conv_temp,LEAD);
					strcat(mneu,conv_temp);
				}
				return;
			}
			}	/* switch (BITS5_4(cur2bytes))	*/
		case 2:
			strcat(mneu,"jsr     ");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
			return;
		case 3:
			strcat(mneu,"jmp     ");
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
			return;
		}	/* switch (BITS7_6(cur2bytes))	*/
	}	/* switch (BITS11_9(cur2bytes))	*/
}	/* miscell() */



void
addq_subq_scc_dbcc()
{
	if (BITS7_6(cur2bytes) == 3)
		if (BITS5_3(cur2bytes) == 1)
		{
			strcat(mneu,"db");
			strcat(mneu,cond_codes[BITS11_8(cur2bytes)]);
			/* dbCC is one character longer than other CCs,	*/
			/* so null out the ninth byte to keep aligned.	*/
			mneu[8] = '\0';
			sprintf(conv_temp,"%%d%o,",BITS2_0(cur2bytes));
			strcat(mneu,conv_temp);
			get2bytes(t_ptr);
			if (BIT15(cur2bytes))
				strcat(mneu,"-");
			convert(BIT15(cur2bytes) ? -(short) cur2bytes : 
				cur2bytes,conv_temp,LEAD);
			strcat(mneu,conv_temp);
			compoff((BIT15(cur2bytes) ? ((long) (short) cur2bytes) :
				(long) cur2bytes) -2, comp_temp);
				/* the -2 above is needed because loc has been 
				   updated when getting the displacement, but 
				   for Motorola the pc is the address of the 
				   extension word */
		}
		else
		{
			strcat(mneu,"s");
			strcat(mneu,cond_codes[BITS11_8(cur2bytes)]);
			if (BITS11_8(cur2bytes) < 2)
			{
				/* cc is only one character */
				mneu[2] = '.';
				mneu[3] = 'b';
			}
			else
			{
				/* cc is two characters */
				mneu[3] = '.';
				mneu[4] = 'b';
			}
			strcat(mneu,eff_add(BITS5_3(cur2bytes),
				BITS2_0(cur2bytes),NULL,NULL));
		}
	else
	{
		strcpy(mneu,BIT8(cur2bytes) ? "subq._  &" : "addq._  &");
		mneu[5] = size[BITS7_6(cur2bytes)];
		convert(BITS11_9(cur2bytes) ? BITS11_9(cur2bytes) : 8,
			conv_temp,LEAD);
		strcat(mneu,conv_temp);
		strcat(mneu,",");
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
			NULL,NULL));
	}
}	/* addq_subq_scc_dbcc()	*/



void
bcc_bsr_bra()
{
	strcpy(mneu,"b");
	if (BITS11_8(cur2bytes) == 1)
		strcat(mneu,"sr     ");
	else if (BITS11_8(cur2bytes) == 0)
		strcat(mneu,"ra     ");
	else
		strcat(mneu,cond_codes[BITS11_8(cur2bytes)]);
	if (LOW8(cur2bytes))
	{
		mneu[3] = '.';
		mneu[4] = 'b';
		convert(BIT7(cur2bytes) ?
			-((short) ( LOW8(cur2bytes) | 0xff00)) :
			LOW8(cur2bytes),conv_temp,LEAD);
		compoff(BIT7(cur2bytes) ? 
			((long)(short) (LOW8(cur2bytes) | 0xff00)) : 
			(long) LOW8(cur2bytes), comp_temp);
		if (BIT7(cur2bytes))
			strcat(mneu,"-");
	}
	else
	{
		get2bytes(t_ptr);
		convert(BIT15(cur2bytes) ? -((short) cur2bytes) :
			cur2bytes,conv_temp,LEAD);
		compoff((BIT15(cur2bytes) ? ((long) (short) cur2bytes) :
			(long) cur2bytes) -2, comp_temp);
				/* the -2 above is needed because loc has been 
				   updated when getting the displacement, but 
				   for Motorola the pc is the address of the 
				   extension word */
		if (BIT15(cur2bytes))
			strcat(mneu,"-");
	}
	strcat(mneu,conv_temp);
}	/* bcc_bsr_bra() */



void
moveq()
{
/*
	if (BIT8(cur2bytes)) {
		fprintf(stderr,"\n%s%s ERROR : Bit 8 is on for move quick instruction %x\n",
			SGS,TOOL,cur2bytes);
		recover();
		}
*/
	strcpy(mneu,"movq.l  &");
	if (BIT7(cur2bytes))
		strcat(mneu,"-");
	convert(BIT7(cur2bytes) ? 
		-(short) (LOW8(cur2bytes) | 0xff00) : 
		LOW8(cur2bytes), conv_temp,LEAD);
	strcat(mneu,conv_temp);
	sprintf(conv_temp,",%%d%o",BITS11_9(cur2bytes));
	strcat(mneu,conv_temp);
}	/* moveq() */

void
or_div_sbcd()
{
	if (BITS7_6(cur2bytes) == 3) {	/* divide */
		strcat(mneu,"div_.w  ");
		if (BIT8(cur2bytes))
			mneu[3] = 's';
		else
			mneu[3] = 'u';
		sprintf(mneu,"%s%s,%%d%o",mneu,
			eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),WORD,
				BIT8(cur2bytes) ? SIGNED : UNSIGNED),
			BITS11_9(curinstr));
		}
	else if (BITS8_6(cur2bytes) == 4 && BITS5_4(cur2bytes) == 0) {
					/* sbcd */
		if (BIT3(cur2bytes))
			sprintf(mneu,"sbcd.b  -(%s),-(%s)",
				addregs[BITS2_0(cur2bytes)],
				addregs[BITS11_9(cur2bytes)]);
		else
			sprintf(mneu,"sbcd.b  %%d%o,%%d%o",
				BITS2_0(cur2bytes),BITS11_9(cur2bytes));
		}
	else {				/* or */
		unsigned short inst = cur2bytes;	/* remember inst. */
		char operand[32];

		strcat(mneu,"or._    ");
		mneu[3] = size[BITS7_6(inst)];
		operand[0] = '\0';
		strcat(operand, eff_add(BITS5_3(inst),BITS2_0(inst),
				BITS7_6(inst)+1,UNSIGNED));
		if (!BIT8(inst))
			sprintf(mneu, "%s%s,",mneu,operand);
		sprintf(mneu,"%s%%d%o",mneu,BITS11_9(inst));
		if (BIT8(inst))
			sprintf(mneu, "%s,%s",mneu,operand);
		}
}

void
sub_subx()
{
	if (BITS7_6(cur2bytes) == 3) {	/* suba */
		strcat(mneu,"suba._  ");
		mneu[5] = BIT8(cur2bytes) ? 'l' : 'w';
		sprintf(mneu,"%s%s,%s",mneu,
			eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
				BIT8(cur2bytes) ? LONG : WORD,SIGNED),
			addregs[BITS11_9(curinstr)]);
		}
	else if (BIT8(cur2bytes) && !BITS5_4(cur2bytes)) { /* subx */
		strcat(mneu,"subx._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		if (BIT3(cur2bytes))
			sprintf (mneu,"%s-(%s),-(%s)",mneu,
				addregs[BITS2_0(cur2bytes)],
				addregs[BITS11_9(cur2bytes)]);
		else 
			sprintf(mneu,"%s%%d%o,%%d%o",mneu,BITS2_0(cur2bytes),
				BITS11_9(cur2bytes));
		}
	else {				/* sub */
		strcat(mneu,"sub._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		if (BIT8(cur2bytes))
			sprintf(mneu,"%s%%d%o,%s",mneu,BITS11_9(curinstr),
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					NULL,NULL));
		else
			sprintf(mneu,"%s%s,%%d%o",mneu,
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					BITS7_6(cur2bytes) + 1,UNSIGNED),
				BITS11_9(curinstr));
		}
}

void
cmp_eor()
{
	if (BITS7_6(cur2bytes) == 3) {	/* cmpa */
		strcat(mneu,"cmpa._  ");
		mneu[5] = BIT8(cur2bytes) ? 'l' : 'w';
		sprintf(mneu,"%s%s,%s",mneu,addregs[BITS11_9(curinstr)],
			eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
				BIT8(cur2bytes) ? LONG : WORD,SIGNED));
		}
	else if (BIT8(cur2bytes) && (BITS5_3(cur2bytes) == 1)) { /* cmpm */
		strcat(mneu,"cmpm._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		sprintf(mneu,"%s(%s)+,(%s)+",mneu,
			addregs[BITS11_9(cur2bytes)],
			addregs[BITS2_0(cur2bytes)]);
		}
	else {				/* cmp or eor */
		if (BIT8(cur2bytes))
			strcat(mneu,"eor._   ");
		else
			strcat(mneu,"cmp._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		sprintf(mneu,"%s%%d%o,%s",mneu,BITS11_9(curinstr),
			eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
				BITS7_6(cur2bytes) +1,BIT8(cur2bytes) ?
				UNSIGNED : SIGNED));
		}
}

void
and_mul_abcd_exg()
{
	if(BITS7_6(cur2bytes) == 3) {	/* mul */
		if (BIT8(cur2bytes))
			strcat(mneu,"muls.w  ");
		else
			strcat(mneu,"mulu.w  ");
		sprintf(mneu,"%s%s,%%d%o",mneu,eff_add(BITS5_3(cur2bytes),
			BITS2_0(cur2bytes),WORD,
			BIT8(cur2bytes) ? SIGNED : UNSIGNED),
			BITS11_9(curinstr));
		}
	else if(BIT8(cur2bytes) && !BITS5_4(cur2bytes)) { /* abcd or exg */
		if (BITS8_6(cur2bytes) == 4) {	/* abcd */
			strcat(mneu,"abcd.b  ");
			if (BIT3(cur2bytes))
				sprintf(mneu,"%s-(%s),-(%s)",mneu,
					addregs[BITS2_0(cur2bytes)],
					addregs[BITS11_9(cur2bytes)]);
			else
				sprintf(mneu,"%s%%d%o,%%d%o",mneu,
					BITS2_0(cur2bytes),BITS11_9(cur2bytes));
			}
		else if (BITS8_3(cur2bytes) == 050)
			sprintf(mneu,"exgd    %%d%o,%%d%o",BITS11_9(cur2bytes),
				BITS2_0(cur2bytes));
		else if (BITS8_3(cur2bytes) == 051)
			sprintf(mneu,"exga    %s,%s",
				addregs[BITS11_9(cur2bytes)],
				addregs[BITS2_0(cur2bytes)]);
		else if (BITS8_3(cur2bytes) == 061)
			sprintf(mneu,"exgm    %%d%o,%s",BITS11_9(cur2bytes),
				addregs[BITS2_0(cur2bytes)]);
		else {
			fprintf(stderr,"\n%s%s ERROR : impossible bit settings for instruction %x\n",
				SGS, TOOL, cur2bytes);
			recover();
			}
		}
	else {	/* and */
		strcat(mneu,"and._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		if (BIT8(cur2bytes))
			sprintf(mneu,"%s%%d%o,%s",mneu,BITS11_9(curinstr),
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					NULL,NULL));
		else
			sprintf(mneu,"%s%s,%%d%o",mneu,
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					BITS7_6(cur2bytes) + 1,UNSIGNED),
				BITS11_9(curinstr));
		}
}

void
add_addx()
{
	if (BITS7_6(cur2bytes) == 3) {	/* adda */
		strcat(mneu,"adda._  ");
		mneu[5] = BIT8(cur2bytes) ? 'l' : 'w';
		sprintf(mneu,"%s%s,%s",mneu,
			eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
				BIT8(cur2bytes) ? LONG : WORD,UNSIGNED),
			addregs[BITS11_9(curinstr)]);
		}
	else if (BIT8(cur2bytes) && !BITS5_4(cur2bytes)) { /* addx */
		strcat(mneu,"addx._  ");
		mneu[5] = size[BITS7_6(cur2bytes)];
		if (BIT3(cur2bytes))
			sprintf (mneu,"%s-(%s),-(%s)",mneu,
				addregs[BITS2_0(cur2bytes)],
				addregs[BITS11_9(cur2bytes)]);
		else 
			sprintf(mneu,"%s%%d%o,%%d%o",mneu,BITS2_0(cur2bytes),
				BITS11_9(cur2bytes));
		}
	else {				/* add */
		strcat(mneu,"add._   ");
		mneu[4] = size[BITS7_6(cur2bytes)];
		if (BIT8(cur2bytes))
			sprintf(mneu,"%s%%d%o,%s",mneu,BITS11_9(curinstr),
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					NULL,NULL));
		else
			sprintf(mneu,"%s%s,%%d%o",mneu,
				eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
					BITS7_6(cur2bytes) + 1,UNSIGNED),
				BITS11_9(curinstr));
		}
}
	
void
shft_rot()
{
	char	direction;		/* l -- left, r -- right	*/

	direction = BIT8(cur2bytes) ? 'l' : 'r';

	if (BITS7_6(cur2bytes) == 3)	/* Memory rotate.		*/
	{
		if (BIT11(cur2bytes) != 0)
		{
			fprintf(stderr, "\n%s%s ERROR: bit 11 of memory shift/rotate instruction must be zero %x\n", 
				SGS, TOOL, cur2bytes);
			recover();
		}

		switch (BITS10_9(cur2bytes))
		{
		case 0:	strcat(mneu,"as_.w   &1,");
			mneu[2] = direction;
			break;
		case 1: strcat(mneu,"ls_.w   &1,");
			mneu[2] = direction;
			break;
		case 2: strcat(mneu,"rox_.w  &1,");
			mneu[3] = direction;
			break;
		case 3: strcat(mneu,"ro_.w   &1,");
			mneu[2] = direction;
			break;
		}
		strcat(mneu,eff_add(BITS5_3(cur2bytes),BITS2_0(cur2bytes),
			NULL,NULL));
	}
	else				/* Register rotate.		*/
	{

		switch (BITS4_3(cur2bytes))
		{
		case 0:	strcat(mneu,"as_._   ");
			mneu[2] = direction;
			mneu[4] = size[BITS7_6(cur2bytes)];
			break;
		case 1: strcat(mneu,"ls_._   ");
			mneu[2] = direction;
			mneu[4] = size[BITS7_6(cur2bytes)];
			break;
		case 2: strcat(mneu,"rox_._  ");
			mneu[3] = direction;
			mneu[5] = size[BITS7_6(cur2bytes)];
			break;
		case 3: strcat(mneu,"ro_._   ");
			mneu[2] = direction;
			mneu[4] = size[BITS7_6(cur2bytes)];
			break;
		}

		if (BIT5(cur2bytes))
		{
			sprintf(conv_temp,"%%d%o,%%d%o",
				BITS11_9(cur2bytes),BITS2_0(cur2bytes));
			strcat(mneu,conv_temp);
		}
		else
		{
			strcat(mneu,"&");
			convert(BITS11_9(cur2bytes) ? BITS11_9(cur2bytes) : 8, 
				conv_temp, LEAD);
			sprintf(conv_temp,"%s,%%d%o", conv_temp,
				BITS2_0(cur2bytes));
			strcat(mneu,conv_temp);
		}
	}
}	/* shft_rot()	*/



void
unassigned()
{
	strcpy(mneu,"***ERROR--unassigned op code***");
	recover();
}	/* unassigned() */

char *
eff_add(mode,reg,size,sign)
int mode,reg,size;
	/* size will be NULL, BYTE, WORD, or LONG */
short sign;
{
unsigned long fourbytes;
static char address_fld[32];
	address_fld[0] = '\0';
	switch(mode)
	{
	case 0:			/* data register direct */
		sprintf(address_fld, "%%d%o", reg);
		break;
	case 1:			/* address register direct */
		sprintf(address_fld, "%s", addregs[reg]);
		break;
	case 2:			/* address register indirect */
		sprintf(address_fld, "(%s)", addregs[reg]);
		break;
	case 3:			/* address register indirect with post incr */
		sprintf(address_fld, "(%s)+", addregs[reg]);
		break;
	case 4:			/* address register indirect with pre decr */
		sprintf(address_fld, "-(%s)", addregs[reg]);
		break;
	case 5:			/* address register indirect with displ */
		get2bytes(t_ptr);
		if (BIT15(cur2bytes))
			strcat(address_fld, "-");
		convert(BIT15(cur2bytes) ? -(short) cur2bytes : cur2bytes,
			conv_temp,LEAD);
		sprintf(address_fld,"%s%s(%s)", address_fld, conv_temp, 
			addregs[reg]);
		break;
	case 6:			/* address register indirect with index */
		get2bytes(t_ptr);
		if (BITS10_8(cur2bytes)) {
			/* these bits must be zero for this address mode */
			fprintf(stderr, "\n%s%s ERROR: bits 10-8 of extension word are non-zero %x\n", 
				SGS, TOOL, cur2bytes);
			recover();
			}
		if (BIT7(cur2bytes))
			strcat(address_fld, "-");
		convert(BIT7(cur2bytes) ?
			-(short) (LOW8(cur2bytes) | 0xff00) : LOW8(cur2bytes),
			conv_temp,LEAD);
		sprintf(address_fld,"%s%s(%s,%%%c%o.%c)", 
			address_fld, conv_temp, addregs[reg],
			BIT15(cur2bytes) ? 'a' : 'd' , BITS14_12(cur2bytes),
			BIT11(cur2bytes) ? 'l' : 'w' );
		break;
	case 7:			/* absolute, pc, and immediate */
		switch(reg)		/* reg is not really a register here */
		{
		case 0:		/* absolute short */
			get2bytes(t_ptr);
			convert((short) cur2bytes,address_fld,LEAD);
			strcat(address_fld,".W");
			break;	
		case 1:		/* absolute long */
			get2bytes(t_ptr);
			fourbytes = cur2bytes << 16;
			get2bytes(t_ptr);
			fourbytes |= cur2bytes;
			convert(fourbytes,address_fld,LEAD);
			strcat(address_fld,".L");
			break;
		case 2:		/* pc with displ */
			get2bytes(t_ptr);
			if (BIT15(cur2bytes))
				strcat(address_fld, "-");
			convert(BIT15(cur2bytes) ?
				-(short) cur2bytes : cur2bytes,
				conv_temp, LEAD);
			sprintf(address_fld, "%s%s(%%pc)",
				address_fld, conv_temp);
			compoff((BIT15(cur2bytes) ? ((long) (short) cur2bytes) :
				(long) cur2bytes) -2, comp_temp);
				/* the -2 above is needed because loc has been 
				   updated when getting the displacement, but 
				   for Motorola the pc is the address of the 
				   extension word */
			break;
		case 3:		/* pc with index */
			get2bytes(t_ptr);
			if (BITS10_8(cur2bytes)) {
				/* these bits must be zero for this address mode */
				fprintf(stderr, "\n%s%s ERROR: bits 10-8 of extension word are non-zero %x\n", 
					SGS, TOOL, cur2bytes);
				recover();
			}
			if (BIT7(cur2bytes))
				strcat(address_fld, "-");
			convert(BIT7(cur2bytes) ?
				-(short) (LOW8(cur2bytes) | 0xff00) :
				LOW8(cur2bytes),
				conv_temp,LEAD);
			sprintf(address_fld,"%s%s(%%pc,%%%c%o.%c)", 
				address_fld, conv_temp, 
				BIT15(cur2bytes) ? 'a' : 'd' ,
				BITS14_12(cur2bytes),
				BIT11(cur2bytes) ? 'l' : 'w' );
			compoff((BIT7(cur2bytes) ? 
				((long)(short) (LOW8(cur2bytes) | 0xff00)) : 
				(long) LOW8(cur2bytes)) - 2, comp_temp);
				/* the -2 above is needed because loc has been 
				   updated when getting the index word, but 
				   for Motorola the pc is the address of
				   this word */
			sprintf(comp_temp,"%s+%%%c%o",comp_temp,
				BIT15(cur2bytes) ? 'a' : 'd' ,
				BITS14_12(cur2bytes));
			break;
		case 4:		/* immediate */
			switch(size)
			{
			case NULL:
				fprintf(stderr,"\n%s%s ERROR: Null size for immediate data %x\n",
					SGS,TOOL, cur2bytes);
				recover();
				break;
			case BYTE:
				get2bytes(t_ptr);
				if (BITS15_8(cur2bytes))
					fprintf(stderr,"\n%s%s ERROR: first byte of extension word not zero for byte immediate data address mode %x\n",
						SGS,TOOL, cur2bytes);
				strcat(address_fld, "&");
				if ((sign == SIGNED) && BIT7(cur2bytes))
					strcat(address_fld, "-");
				convert(((sign == SIGNED) && BIT7(cur2bytes)) ?
					-(short) (LOW8(cur2bytes) | 0xff00) :
					LOW8(cur2bytes),
					conv_temp,LEAD);
				strcat(address_fld, conv_temp);
				break;
			case WORD:
				get2bytes(t_ptr);
				strcat(address_fld, "&");
				if ((sign == SIGNED) && BIT15(cur2bytes))
					strcat(address_fld, "-");
				convert(((sign == SIGNED) && BIT15(cur2bytes)) ?
					-(short) cur2bytes : cur2bytes ,
					conv_temp,LEAD);
				strcat(address_fld, conv_temp);
				break;
			case LONG:
				get2bytes(t_ptr);
				strcat(address_fld, "&");
				fourbytes = cur2bytes << 16;
				get2bytes(t_ptr);
				fourbytes |= cur2bytes;
				if ((sign == SIGNED) && HIOF32(fourbytes))
					strcat(address_fld, "-");
				convert(((sign == SIGNED) && HIOF32(fourbytes))?
					-(long) fourbytes : fourbytes,
					conv_temp, LEAD);
				strcat(address_fld, conv_temp);
				break;
			} /* end of size switch */
			break;
		default:
			fprintf(stderr,"\n%s%s  ERROR: invalid register number for mode 7\n",SGS,TOOL);
			recover();
		} /* end reg switch */
		break;
	} /* end mode switch */
return(address_fld);
}

void
recover()
{
	printline();
	if (resync() == FAILURE)
		if (++errlev > MAXERRS)
		{
			fprintf(stderr,"\n%s%%s: %s: %s:",SGS,TOOL,fname,sname);
			fprintf(stderr," too many errors\n");
			fprintf(stderr,"\tdisassembly terminated\n");
			exit(127);
		}
	longjmp(env, 1); /* the value 1 is not actually used in this program */
}
