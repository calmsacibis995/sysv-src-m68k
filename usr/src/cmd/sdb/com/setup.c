	/*	@(#)setup.c	2.1		*/

/*
 *	UNIX debugger
 *
 * 		Everything here is MACHINE DEPENDENT
 *			and operating system dependent
 */

#include "head.h"
#include "coff.h"

extern MSG		BADMAGN;

INT	wtflag = 0;	/* set to allow writing to a.out, core, ISP */

static L_INT		txtsiz;
static L_INT		datsiz;
static L_INT		datbas;
static L_INT		stksiz;
static L_INT		symbas;		/*  not using ?? */
static L_INT		entrypt;

extern INT		argcount;
extern INT		signo;
extern INT		magic;
extern int		errno;

extern STRING		symfil;
extern STRING		corfil;

#if (m68k && VARU)
extern ADDR		ubase;
#endif

#define AOUTHDRSIZ	(filhdr.f_opthdr)

#define	maxfile		(1L<<24)

FILHDR	filhdr;		/* a.out file header */
AOUTHDR	aouthdr;	/* a.out Unix (optional) header */
SCNHDR	*scnhdrp;	/* pointer to first section header (space by sbrk) */

setsym()
{
	fsym = getfile(symfil,1);
	txtmap.ufd = fsym;
	errno = 0;
	if (read(fsym,&filhdr,FILHSZ) == FILHSZ	&&
	   ISMAGIC(filhdr.f_magic)		&&
	   AOUTHDRSIZ == sizeof aouthdr		&&
	   read(fsym, &aouthdr, AOUTHDRSIZ)==AOUTHDRSIZ)
	{
		magic=aouthdr.magic;
		if (magic==OMAGIC   || 	/* Writable text: private */
		   magic==NMAGIC)	/* Readonly text: private */
		{
			txtsiz=aouthdr.tsize;
			datsiz=aouthdr.dsize;
			symbas=txtsiz+datsiz;
			rdschdrs();

			switch (magic) {
			/*  use to have many more "magic" cases here */
			/*	assuming text is first section */

			case OMAGIC:	/* 0407 */
				txtmap.b1=0;
				txtmap.e1=symbas;
				txtmap.f1=scnhdrp[0].s_scnptr;
				txtmap.b2=datbas=0;
				txtmap.e2=symbas;
				txtmap.f2=txtmap.f1;
				break;

			case NMAGIC:	/* 0410 */
				txtmap.b1=0;
				txtmap.e1=txtsiz;
				txtmap.f1=scnhdrp[0].s_scnptr;
				txtmap.b2=datbas=round(txtsiz,TXTRNDSIZ);
				txtmap.e2=datbas+datsiz;
				txtmap.f2=txtmap.f1+txtmap.e1;
				break;

			}
			entrypt = aouthdr.entry;
			ststart = filhdr.f_symptr;
#ifdef  FLEXNAMES
			/* need end of symbol table if flexnames are present */
			stend = ststart+filhdr.f_nsyms*SYMESZ;
#endif
		}
		else {
			magic = 0;
			fprintf(FPRT1, "Warn: No magic for %s;\n", symfil);
		}
	}
	else {		/*  may be a ".o" file */
		if (ISMAGIC(filhdr.f_magic)) {
			magic = filhdr.f_magic;
			rdschdrs();
			/* assuming 3 sections; text, data, and bss */
			txtsiz = scnhdrp[0].s_size;
			datsiz = scnhdrp[1].s_size;
			symbas = txtsiz+datsiz;
			txtmap.b1 = 0;
			txtmap.e1 = txtsiz;
			txtmap.f1 = scnhdrp[0].s_scnptr;
			txtmap.b2 = datbas = txtsiz;
			txtmap.e2 = txtsiz+datsiz;
			txtmap.f2 = scnhdrp[1].s_scnptr;
			entrypt = 0;
			ststart = filhdr.f_symptr;
#ifdef  FLEXNAMES
			/* need end of symbol table if flexnames are present */
			stend = ststart+filhdr.f_nsyms*SYMESZ;
#endif
		}
	}
#if DEBUG > 1
	if(mjmdflag)
		fprintf(FPRT2, "magic=%#o;\n", magic);
#endif
	if (magic == 0) txtmap.e1 = maxfile;
}

setcor()
{
	datmap.ufd = fcor = getfile(corfil,2);
	if(fcor < 0)
		return;
	if (read(fcor, uu, XTOB(USIZE))==XTOB(USIZE)
/*	   ANDF (((struct user *)uu)->u_pcb.pcb_ksp & 0xF0000000L)==0x80000000L	 removed */
/*	   ANDF (((struct user *)uu)->u_pcb.pcb_usp & 0xF0000000L)==0x70000000L	 removed */
	   /*  sure a core file */
	   && magic && magic == ((struct user *)uu)->u_exdata.ux_mag)
	{
	/* ((struct user *)uu)->u_ar0 is an absolute address, currently
	   0x7fff ffb8 in the VAX, and 0x00a0 07b8 in the 3B-20.
	   It must be converted to an addess relative to the beginning
	   of UBLOCK, which is done by subtracting the absolute address
	   of UBLOCK (ADDR_U, #defined in machdep.h) to get the offset
	   from the beginning of the user area, and then adding the
	   sdb internal addess of the user area (uu).		*/

	((struct user *)uu)->u_ar0 =
		(int *) ((char *)(((struct user *)uu)->u_ar0) -
			ADDR_U + (int)uu);

		signo = ((struct user *)uu)->u_arg[0]&037;
		txtsiz = XTOB(((struct user *)uu)->u_tsize);
		datsiz = XTOB(((struct user *)uu)->u_dsize);
		stksiz = XTOB(((struct user *)uu)->u_ssize);
#if DEBUG > 1
		if(mjmdflag > 1) {
			fprintf(FPRT2,
			    "((struct user *)uu)->u_tsize=%#x; ((struct user *)uu)->u_dsize=%#x; ((struct user *)uu)->u_ssize=%#x;\n",
				((struct user *)uu)->u_tsize, ((struct user *)uu)->u_dsize, ((struct user *)uu)->u_ssize);
			fprintf(FPRT2, "txtsiz=%#x; datsiz=%#x; stksiz=%#x;\n",
				txtsiz, datsiz, stksiz);
		}
#endif
		if (magic == NMAGIC)
			datmap.b1=datbas=round(txtsiz,TXTRNDSIZ);
		else
			datmap.b1=datbas=0;
		datmap.e1=(magic==OMAGIC?txtsiz:datmap.b1)+datsiz;
		datmap.f1 = XTOB(USIZE);
#ifdef u3b
		datmap.b2 = USRSTACK;	/* loc of usr stack, in <sys/seg.h> */
		datmap.e2 = USRSTACK + stksiz;
#else
		/* VAX stack grows down from USRSTACK, in <sys/param.h> */
		/*  stksiz includes sizeof user area - must add back */
		datmap.b2 = USRSTACK + XTOB(USIZE) - stksiz;
		datmap.e2 = USRSTACK;
#endif
		datmap.f2 = XTOB(USIZE)+(magic==NMAGIC?datsiz:datmap.e1);
/* ??	signo = *(ADDR *)(((ADDR)uu)+XTOB(USIZE)-4*sizeof(int)); */
#if DEBUG > 1
		if(mjmdflag > 0)
			fprintf(FPRT2, "((struct user *)uu)->u_arg[0]=%#x; signo=%#x;\n",
					((struct user *)uu)->u_arg[0], signo);
#endif
	      /*  put test up front, so don't need here
		if (magic && magic != ((struct user *)uu)->u_exdata.ux_mag) {
			fprintf(FPRT1, "%s: 0%o, 0%o\n",
				BADMAGN, magic, 
				((struct user *)uu)->u_exdata.ux_mag);
		}
	      */
	}
	else {		/*  e.g. /dev/kmem or any ordinary file */
		datmap.e1 = datmap.e2 = maxfile;
		datmap.b1 = datmap.f1 = 0;
		datmap.b2 = datmap.f2 = 0;
		fprintf(FPRT1, "Warning: `%s' not a core file.\n", corfil);
		fakecor = 1;
	}
}
#if (m68k && VARU)		/* required if no memory mgt */
/* This procedure determines the address of the UBLOCK.
   This address is used in computing the location of the register
   variables (u_ar0) from a core file or a running program    */

ADDR
setuaddr() {	/* this code is a modified version of nlist(3c) */
	struct	filehdr	buf;
	struct	syment	sym, usym;
	struct	auxent	aux;
	long n;
	int	nlen=SYMNMLEN, bufsiz=FILHSZ;
	register struct syment  *q;
	long	sa;
	int	fd;

	ubase = 0L;			/* returns 0L to indicate not found */
	strncpy(usym.n_name,"u",8);	/* initialize the syment name field */

	if((fd = open("/unix", 0)) < 0)
		return(0L);
	(void) read(fd, (char *)&buf, bufsiz);
	if(!ISMAGIC(buf.f_magic)) {
		(void) close(fd);
		return(0L);
	}

	sa = buf.f_symptr;	/* direct pointer to sym tab */
	lseek(fd, sa, 0);
	q = &sym;
	n = buf.f_nsyms;	/* num. of sym tab entries */

	while(n) {
		read(fd, (char *)&sym, SYMESZ);
		n -= (q->n_numaux + 1L);
		if (q->n_numaux != 0)	/* read past aux ent, if there is one */
			read(fd, (char *) &aux, AUXESZ);
		if(strncmp(usym.n_name, q->n_name, nlen) == 0) {
			ubase = q->n_value;	/* load address of u */
			break;
		}
	}
	(void) close(fd);
	return(ubase);
}
#endif

create(f)
STRING	f;
{	int fd;
	if ((fd=creat(f,0644)) >= 0) {
		close(fd);
		return(open(f,wtflag));
	}
	else return(-1);
}

getfile(filnam,cnt)
STRING	filnam;
{
	REG INT		fd;

	if (!eqstr("-",filnam)) {
		fd=open(filnam,wtflag);
		if (fd<0 && argcount>cnt) {
			if (fd < 0)
				fprintf(FPRT1, "cannot open `%s'\n", filnam);
		}
	}
	else	fd = -1;
	return(fd);
}

/* rdschdrs(): read section headers */
rdschdrs()
{
	register unsigned nb;
	extern FILHDR filhdr;		/* a.out file header */

	nb = filhdr.f_nscns * SCNHSZ;
	scnhdrp = (SCNHDR *) sbrk(nb);
	if (read(fsym, scnhdrp, nb) != nb)
		fprintf(FPRT1, "Warn: section header read error\n");
	/* chkerr();	 does longjmp and haven't done setjmp yet */
	return(filhdr.f_nscns);
}
