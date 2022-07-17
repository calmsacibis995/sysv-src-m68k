/*	@(#)gendefs.h	2.1		*/
/*	static	char	sccsid[] = "@(#) gendefs.h: 2.4 6/23/83";			*/


#define NO		0
#define YES		1

#define TESTVAL		-2

#define NFILES		15

#define NCPS		8	/* Non-flexname character limit.	*/
#define BITSPBY		8
#define BITSPOW		16
#define OUTWTYPE	unsigned short
#define OUT(a,b)	fwrite((char *)(&a),sizeof(OUTWTYPE),1,b)

#define SCTALIGN	2L	/* Byte alignment for sections.		*/
#define TXTFILL		0x4e71	/* Text region "fill" nop instruction.	*/
#define FILL		0L
#define NULLSYM		((symbol *) NULL)


/* Index of action routines in modes array.				 */

#define	SETFILE		1
#define	LINENBR		2
#define	LINENUM		3
#define	LINEVAL		4
#define	DEFINE		5
#define	ENDEF		6
#define	SETVAL		7
#define	SETTYP		8
#define	SETLNO		9
#define	SETSCL		10
#define	SETTAG		11
#define	SETSIZ		12
#define	SETDIM1		13
#define	SETDIM2		14
#define	NEWSTMT		15
#define GENRELOC	16
#define BRLOC		17
#define BRAOPT		18
#define BCCOPT		19
#define BSROPT		20
#define ABSOPT		21
#define SWBEG		22
#define MOVE		23
#define IOPT		24
#define INSTI		25
#define ABSBR		26
#define NDXRELOC	27

#define NACTION		27
