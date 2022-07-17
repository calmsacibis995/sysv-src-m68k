/*	@(#)ops.h	2.1		*/
/* ops.h
/*	@(#) ops.h:	2.1 83/07/08	*/

/* MC68000 Optimizer */

/* Definitions of opcode related things. */


#define	MAXOPS	2		/* Two operands per instruction, max. */
typedef	unsigned char OPCODE;	/* Object holding opcode number */

typedef unsigned short FLAGS;	/* holds operation flag bits */

/* This structure contains the invariant information for each instruction.
** For transfers of control (jumps, branches, subroutine calls, etc.), the
** highest numbered operand is assumed to contain the control target.
*/

struct op
{
    char * op_name;		/* pointer to instruction print-name */
    OPCODE op_code;		/* operation code number for instruction */
    unsigned char op_nsrc;	/* number of source operands.  Source operands
				** are those for which the processor does a
				** read cycle.  For example, add.w has two
				** source operands, while mov.w has just one.
				** Source operands are assumed to be consec-
				** utive, starting with the first operand.
				*/
    unsigned char op_dnum;	/* Destination operand number.  If the op has
				** no destination, value is NODEST.  A desti-
				** nation operand is one whose value is changed.
				** In add.l, the second operand is both a source
				** AND a destination operand, whereas the
				** second operand of mov.l is just a dest.
				*/
    unsigned char op_ssize[MAXOPS];
				/* size of each source operand, addressed as
				** 0 - MAXOPS-1, in bytes.
				*/
    unsigned char op_dsize;	/* size of destination operand, in bytes */
    OPCODE op_opposite;		/* op code of "opposite" instruction, in
				** some sense.  For example, for conditional
				** jumps, this is the "reversed" branch.
				*/
    FLAGS op_flags;		/* various flag bits for inst. -- see below */
};

#define	NODEST	0xFF		/* value meaning no destination */
#define	NOOPP	'\0'		/* value meaning no opposite instruction */

typedef struct op OP;		/* Declare simple form */

/* Define flags for op_flags field.  These are bit flags. */

#define	OF_LAB		0x001	/* op is normal label */
#define	OF_UBRA		0x002	/* op is unconditional branch */
#define	OF_HLAB		0x004	/* op is hard label */
#define	OF_HBRA		0x008	/* op is hard branch */
#define	OF_CBRA		0x010	/* op is conditional branch */
#define	OF_NCBRA	0x020	/* op is conditional branch with null
				** destination (e.g., trapv)
				*/
#define	OF_RCBRA	0x040	/* op is reversible conditional branch */
#define	OF_CC		0x080	/* op affects condition codes */
#define	OF_CCA		0x100	/* op affects condition codes, except if
				** destination is address register
				*/
#define	OF_MODSP	0x200	/* op modifies stack pointer implicitly */
#define	OF_LD		0x400	/* op has funny live/dead characteristics */
#define	OF_VTRACE	0x800	/* op has funny value trace behavior */
#define	OF_NOTAB	0x1000	/* op is not in opcode table */
#define	OF_PSEUDO	0x2000	/* op is a pseudo-op */
/* Define op code numbers.  These numbers form a 1 to 1
** correspondence with the op code names in optab.c .  The order must
** be identical: the ASCII collating sequence for the op names.  The
** numbers must be sequential.
*/

/* Minimum and maximum "real" op codes */

#define	OP_MIN	10	/* minimum */
#define	OP_MAX	202	/* maximum */

#define	ABCD_B	10		/* abcd.b */
#define	ADD_B	11		/* add.b */
#define	ADD_L	12		/* add.l */
#define	ADD_W	13		/* add.w */
#define	ADDX_B	14		/* addx.b */
#define	ADDX_L	15		/* addx.l */
#define	ADDX_W	16		/* addx.w */
#define	AND_B	17		/* and.b */
#define	AND_L	18		/* and.l */
#define	AND_W	19		/* and.w */
#define	ASL_B	20		/* asl.b */
#define	ASL_L	21		/* asl.l */
#define	ASL_W	22		/* asl.w */
#define	ASR_B	23		/* asr.b */
#define	ASR_L	24		/* asr.l */
#define	ASR_W	25		/* asr.w */
#define	BCC	26		/* bcc */
#define	BCC_B	27		/* bcc.b */
#define	BCHG	28		/* bchg */
#define	BCLR	29		/* bclr */
#define	BCS	30		/* bcs */
#define	BCS_B	31		/* bcs.b */
#define	BEQ	32		/* beq */
#define	BEQ_B	33		/* beq.b */
#define	BGE	34		/* bge */
#define	BGE_B	35		/* bge.b */
#define	BGT	36		/* bgt */
#define	BGT_B	37		/* bgt.b */
#define	BHI	38		/* bhi */
#define	BHI_B	39		/* bhi.b */
#define	BHS	40		/* bhs */
#define	BHS_B	41		/* bhs.b */
#define	BLE	42		/* ble */
#define	BLE_B	43		/* ble.b */
#define	BLO	44		/* blo */
#define	BLO_B	45		/* blo.b */
#define	BLS	46		/* bls */
#define	BLS_B	47		/* bls.b */
#define	BLT	48		/* blt */
#define	BLT_B	49		/* blt.b */
#define	BMI	50		/* bmi */
#define	BMI_B	51		/* bmi.b */
#define	BNE	52		/* bne */
#define	BNE_B	53		/* bne.b */
#define	BPL	54		/* bpl */
#define	BPL_B	55		/* bpl.b */
#define	BR	56		/* br */
#define	BR_B	57		/* br.b */
#define	BRA	58		/* bra */
#define	BRA_B	59		/* bra.b */
#define	BSET	60		/* bset */
#define	BSR	61		/* bsr */
#define	BSR_B	62		/* bsr.b */
#define	BTST	63		/* btst */
#define	BVC	64		/* bvc */
#define	BVC_B	65		/* bvc.b */
#define	BVS	66		/* bvs */
#define	BVS_B	67		/* bvs.b */
#define	BYTE	68		/* byte */
#define	CHK_W	69		/* chk.w */
#define	CLR_B	70		/* clr.b */
#define	CLR_L	71		/* clr.l */
#define	CLR_W	72		/* clr.w */
#define	CMP_B	73		/* cmp.b */
#define	CMP_L	74		/* cmp.l */
#define	CMP_W	75		/* cmp.w */
#define	COMM	76		/* comm */
#define	DATA	77		/* data */
#define	DBCC	78		/* dbcc */
#define	DBCS	79		/* dbcs */
#define	DBEQ	80		/* dbeq */
#define	DBF	81		/* dbf */
#define	DBGE	82		/* dbge */
#define	DBGT	83		/* dbgt */
#define	DBHI	84		/* dbhi */
#define	DBHS	85		/* dbhs */
#define	DBLE	86		/* dble */
#define	DBLO	87		/* dblo */
#define	DBLS	88		/* dbls */
#define	DBLT	89		/* dblt */
#define	DBMI	90		/* dbmi */
#define	DBNE	91		/* dbne */
#define	DBPL	92		/* dbpl */
#define	DBR	93		/* dbr */
#define	DBRA	94		/* dbra */
#define	DBT	95		/* dbt */
#define	DBVC	96		/* dbvc */
#define	DBVS	97		/* dbvs */
#define	DEF	98		/* def */
#define	DIVS_W	99		/* divs.w */
#define	DIVU_W	100		/* divu.w */
#define	EOR_B	101		/* eor.b */
#define	EOR_L	102		/* eor.l */
#define	EOR_W	103		/* eor.w */
#define	EVEN	104		/* even */
#define	EXG	105		/* exg */
#define	EXT_L	106		/* ext.l */
#define	EXT_W	107		/* ext.w */
#define	FILE_	108		/* file */
#define	GLOBAL	109		/* global */
#define	JMP	110		/* jmp */
#define	JSR	111		/* jsr */
#define	LCOMM	112		/* lcomm */
#define	LEA_L	113		/* lea.l */
#define	LINK	114		/* link */
#define	LN	115		/* ln */
#define	LONG	116		/* long */
#define	LSL_B	117		/* lsl.b */
#define	LSL_L	118		/* lsl.l */
#define	LSL_W	119		/* lsl.w */
#define	LSR_B	120		/* lsr.b */
#define	LSR_L	121		/* lsr.l */
#define	LSR_W	122		/* lsr.w */
#define	MOV_B	123		/* mov.b */
#define	MOV_L	124		/* mov.l */
#define	MOV_W	125		/* mov.w */
#define	MOVM_L	126		/* movm.l */
#define	MOVM_W	127		/* movm.w */
#define	MOVP_W	128		/* movp.w */
#define	MOVP_L	129		/* movp.l */
#define	MULS_W	130		/* muls.w */
#define	MULU_W	131		/* mulu.w */
#define	NBCD_B	132		/* nbcd.b */
#define	NEG_B	133		/* neg.b */
#define	NEG_L	134		/* neg.l */
#define	NEG_W	135		/* neg.w */
#define	NEGX_B	136		/* negx.b */
#define	NEGX_L	137		/* negx.l */
#define	NEGX_W	138		/* negx.w */
#define	NOP	139		/* nop */
#define	NOT_B	140		/* not.b */
#define	NOT_L	141		/* not.l */
#define	NOT_W	142		/* not.w */
#define	OR_B	143		/* or.b */
#define	OR_L	144		/* or.l */
#define	OR_W	145		/* or.w */
#define	ORG	146		/* org */
#define	PEA_L	147		/* pea.l */
#define	RESET	148		/* reset */
#define	ROL_B	149		/* rol.b */
#define	ROL_L	150		/* rol.l */
#define	ROL_W	151		/* rol.w */
#define	ROR_B	152		/* ror.b */
#define	ROR_L	153		/* ror.l */
#define	ROR_W	154		/* ror.w */
#define	ROXL_B	155		/* roxl.b */
#define	ROXL_L	156		/* roxl.l */
#define	ROXL_W	157		/* roxl.w */
#define	ROXR_B	158		/* roxr.b */
#define	ROXR_L	159		/* roxr.l */
#define	ROXR_W	160		/* roxr.w */
#define	RTE	161		/* rte */
#define	RTR	162		/* rtr */
#define	RTS	163		/* rts */
#define	SBCD_B	164		/* sbcd.b */
#define	SCC_B	165		/* scc.b */
#define	SCS_B	166		/* scs.b */
#define	SEQ_B	167		/* seq.b */
#define	SET	168		/* set */
#define	SF_B	169		/* sf.b */
#define	SGE_B	170		/* sge.b */
#define	SGT_B	171		/* sgt.b */
#define	SHI_B	172		/* shi.b */
#define	SHORT	173		/* short */
#define	SHS_B	174		/* shs.b */
#define	SLE_B	175		/* sle.b */
#define	SLO_B	176		/* slo.b */
#define	SLS_B	177		/* sls.b */
#define	SLT_B	178		/* slt.b */
#define	SMI_B	179		/* smi.b */
#define	SNE_B	180		/* sne.b */
#define	SPACE	181		/* space */
#define	SPL_B	182		/* spl.b */
#define	ST_B	183		/* st.b */
#define	STOP	184		/* stop */
#define	SUB_B	185		/* sub.b */
#define	SUB_L	186		/* sub.l */
#define	SUB_W	187		/* sub.w */
#define	SUBX_B	188		/* subx.b */
#define	SUBX_L	189		/* subx.l */
#define	SUBX_W	190		/* subx.w */
#define	SVC_B	191		/* svc.b */
#define	SVS_B	192		/* svs.b */
#define	SWAP_W	193		/* swap.w */
#define	SWBEG	194		/* swbeg */
#define	TAS_B	195		/* tas.b */
#define	TEXT	196		/* text */
#define	TRAP	197		/* trap */
#define	TRAPV	198		/* trapv */
#define	TST_B	199		/* tst.b */
#define	TST_L	200		/* tst.l */
#define	TST_W	201		/* tst.w */
#define	UNLK	202		/* unlk */


/* Define other "op codes" */

#define	LABEL	220	/* for code labels.  The first operand is the
			** label itself.
			*/
#define	SWITCH	221	/* for switch constructs */
