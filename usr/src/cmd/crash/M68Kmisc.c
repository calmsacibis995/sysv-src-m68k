/*	@(#)M68Kmisc.c	2.1		*/
#include	"crash.h"
#include	"sys/inode.h"
#include	"sys/proc.h"
#include	"sys/file.h"
#include	"sys/mount.h"
#include	"sys/text.h"
#define	SEGBYTES	0x40000000L	/* power of 2 to the 30th */
#define	CORECLKS	NBPC		/* Core Clicks NBPC from sys/param.h  */
#define	BITMASK		(NBPC-1) 	/* Remove low order bits */

extern	struct	dstk	dmpstack;
extern	struct	glop	dumpglop;
extern	struct	uarea	x;
int	nmfd;

fatal(str)
	char	*str;
{
	printf("error: %s\n", str);
	exit(1);
}

error(str)
	char	*str;
{
	printf("error: %s\n", str);
}

atoi(s)
	register  char  *s;
{
	register  base, numb;

	numb = 0;
	if(*s == '0')
		base = 8;
	else
		base = 10;
	while(*s) {
		if(*s < '0' || *s > '9') {
			error("number expected");
			return(-1);
		}
		numb = numb * base + *s++ - '0';
	}
	return(numb);
}

init()
{
	extern	char	*dumpfile;
	extern	struct	dstk	dmpstack;
	extern	int	Kfp;
	extern	char	*namelist;
	extern	struct	nlist	*stbl;
	extern  unsigned  symcnt;
	extern	struct	var	v;

	long	symloc;
	char	*sbrk();
	int	sigint();
	struct	nlist	*symsrch();
	int	bufaddr;

	if((mem = open(dumpfile, 0)) < 0)
		fatal("cannot open dump file");

	nmfd = open(namelist, 0);

	rdsymtab();

	Dmpstk = symsrch("dumpstk");
	Curproc = symsrch("curproc");
	U = symsrch("u");
	File = symsrch("file");
	Inode = symsrch("inode");
	Mount = symsrch("mount");
	Proc = symsrch ("proc");
	Text = symsrch("text");
	Swap = symsrch("swapmap");
	Sbuf = symsrch("sbuf");
	Core = symsrch("coremap");
	V = symsrch("v");
	Mmu = symsrch("mmu_table");
	Sys = symsrch("utsname");
	Time = symsrch("time");
	Lbolt = symsrch("lbolt");
	Panic = symsrch("panicstr");
	Etext = symsrch("etext");
	End = symsrch("end");
	Callout = symsrch("callout");

	if(readmem(&bufaddr, SYM_VALUE(Sbuf),
		sizeof bufaddr) != sizeof bufaddr) {
		Buf = symsrch("buf");
		error("read error on buf");
	} else {
		Sbuf->n_value = bufaddr;
#ifdef DEBUG
		printf("bufaddr = %x\n", bufaddr);
#endif
		Buf = Sbuf;
	}

	if(readmem(&v,(long)SYM_VALUE(V), sizeof v) != sizeof v)
		error("read error on v structure");
	v.ve_inode = (char *)(((unsigned)v.ve_inode - Inode->n_value) /
		sizeof (struct inode));
	v.ve_file = (char *)(((unsigned)v.ve_file - File->n_value) /
		sizeof (struct file));
	v.ve_mount = (char *)(((unsigned)v.ve_mount - Mount->n_value) /
		sizeof (struct mount));
	v.ve_proc = (char *)(((unsigned)v.ve_proc - Proc->n_value) /
		sizeof (struct proc));
	v.ve_text = (char *)(((unsigned)v.ve_text - Text->n_value) /
		sizeof (struct text));

	if(!symsrch("con_tty")) { 
		if(!settty("acia"))  /* settty() appends a "_tty" */
			printf("invalid tty structure\n");
	}
	
	signal(SIGINT, sigint);
}

/*
 *	Gather the uarea together and put it into the structure x.u
 *
 *	The particular uarea is selectable by the process table slot.
*/
getuarea(slot)
{
	struct	proc pbuf;
	int	i;
	int	curproc;
	long	addr;

	if(slot == -1) {
		if(readmem((char *)&curproc, (long)SYM_VALUE(Curproc),
			sizeof curproc) == -1) {
				error("read error on curproc");
				return(-1);
		}
		slot = (curproc - Proc->n_value) / sizeof(struct proc);
	}
	if(readmem((char *)&pbuf, SYM_VALUE(Proc)+slot*sizeof(struct proc),
		sizeof(pbuf)) != sizeof(pbuf)) {
			error("read error on proc table");
			return(-1);
	}


	if ((pbuf.p_flag& (SLOAD | SSPART)) == 0) {
		return(SWAPPED);
	} else {
		lseek(mem,(long)pbuf.p_addr,0);
		if(read(mem,(char *)&x.u,USIZE*CORECLKS) != USIZE*CORECLKS)
			return(-1);
	}

	return(0);
}
