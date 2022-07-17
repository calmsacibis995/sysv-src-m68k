/*	@(#)sysdef.c	2.1		*/
#include	"stdio.h"
#include	"sys/var.h"
#include	"a.out.h"
#include	"sys/param.h"
#include	"sys/sysmacros.h"	/* for ctob() */

/* #define	DEBUG			/*------------------ temp ----------*/
#if	vax && m68k
#undef	vax
#define	BSWAP			/* vax host; m68k target */
#endif
#define	PAGESZ	512
#define	D7	0x38000000	/* From Unibus vector table in univec.c */
#define	USRMASK	0x3fffffff
#define	MAPSZ	4
#define	MAXI	200
#define	BUS	0340
#define	CC	0037
#define	BLOCK	010
#define	INTR	0	/* l_nptr[0] for interrupt routines */
#define	ADDR	1	/* l_nptr[1] for device address     */
#define	CNT	2	/* l_nptr[2] for device count       */
#define	STRAT	3	/* l_nptr[3] for strategy if blkdev */

#define	INTSZ		sizeof (int)
#define	SHORTSZ		sizeof (short)
#ifndef	m68k
#define	PRE		1		/* pre-pended underscore */
#define	SYM_VALUE(ptr)	(ptr->n_value & USRMASK)
#else
#define	PRE		0		/* no pre-pended underscore */
#define	OFFSET(n)	(n + (n < sgs.scnhdr[1].s_paddr ? toffset : doffset))
#define	SYM_VALUE(ptr)	OFFSET(ptr->n_value)
#define	SYM_FOUND(ptr)	(ptr->n_scnum)	/* true if symbol was found */
#endif

#define	N_IFC		((start->n_value - IFC) / IFC_SZ)
#define	TXT		(HDR + aout.a_data)

struct	var	v;
#if	vax || m68k
struct sgs {
	struct filehdr filehdr;
	struct aouthdr aout;
	struct scnhdr scnhdr[3];
} sgs;
#define	HDR		sizeof (struct sgs)
#else
struct	exec	aout;
#define	HDR		sizeof (struct exec)
#endif
char	*os = "/unix";
char	*mr = "/etc/master";
char	line[256], nm[20], pre[20], intrnm[20],
	devaddr[20], devcnt[20], strat[20];
int	x, address, vsz, unit, bus, deft,
	count, typ, bcnt, maus, mcnt, smem, mesg, sem,
	n_swap, power;
short	root, swap, dump, pipe;
long	swap_low;
int	IFC = 01000;	/* Start of Interface routines */
int	*vec, nexus;
int	bndry();
#ifndef	m68k
int	offset;
#else
#define	offset	0	/* m68k symbols can be in text space as well as data */
int	toffset, doffset;
#endif
FILE	*system, *mast, *sysseek;

struct nlist	nl[MAXI], *nlptr, *setup(), *endnm, *start,
	*Messag, *Sema, *Maus, *bdevcnt, *bdevsw, *X25, *mbacnt,
	*rootdev, *swapdev, *dumpdev, *swplo, *nswap, *Shmem,
	*pipedev, *Power, *UNIvec, *umemvad, *Mbacf,
	*vs, *smap, *cmap, *callo, *restart;

#ifdef	m68k
struct nlist	*sysdef1, *sysdef2, *sysdef3, *sysdefr, *sysdefp;
struct nlist	*sysdevs, *ram_nbr, *ram_tbl, *probe_tbl;
char	sysdef[800];		/* static buffer for sysdef? data */
#endif

struct	link {
	char	l_cfnm[8];	/* config name from master table */
	struct	nlist	*l_nptr;/* ptr to name list structure */
	int	l_vsz;		/* vector size */
	int	l_def;		/* default unit count */
	int	l_dtype;	/* device type - BLOCK */
	int	l_bus;		/* bus request from master table */
	char	l_used;		/* set when device entry is printed */
} ln[MAXI/3], *lnptr, *setln(), *endln, *search(), *majsrch();

struct	vector {
#ifdef	vax
	int	vaxvec;
#endif
#ifdef	m68k
	long   m68kvec;
#endif
#ifdef	pdp11
	int	v_pc;
	int	v_psw;
#endif
} vector, *addr;

struct mbacf {
	int	nexus;
	int	bus_req;
	int	dev_addr;
} mbadev;

struct	bdev {
	int	b_open;
	int	b_close;
	int	b_strategy;
	int	b_tab;
} bent;

#ifdef	pdp11
struct	interface {	/* jsr r5, call; function */
	int	i_jsr;
	int	i_call;
	int	i_func;
} ifc[MAXI/3];

struct	xinterface {	/* jsr r5, call; jmp function */
	int	i_jsr;
	int	i_call;
	int	i_jmp;
	int	i_func;
} xifc[MAXI/3];

int	ovly = 0;	/* set to 1 if overlay system */
int	IFC_SZ = sizeof(ifc[0]);
#endif

struct x25info{
	int	x25_0_0;
	int	x25_0_1;
	int	x25links;
	int	x25bufs;
	int	x25bytes;
} x25info;


main(argc, argv)
	int	argc;
	char	**argv;
{

	register int i;

	switch(argc) {
	case 3:	mr = argv[2];
	case 2: os = argv[1];
	case 1: break;
	default:
		fprintf(stderr, "usage: sysdef  [ /unix [ /etc/master ] ]");
		exit(1);
	}

#ifdef	pdp11
	offset = (-HDR);
#endif

	if((system = fopen(os,"r")) == NULL) {
		fprintf(stderr,"cannot read %s\n",os);
		exit(1);
	}

/* read in a.out header */
#if	vax || m68k
	if(fread(&sgs, sizeof sgs, 1, system) != 1) {
#else
	if(fread(&aout, sizeof aout, 1, system) != 1) {
#endif
		printf("cannot read a.out header\n");
		exit(1);
	}
#if	vax || m68k
	if(sgs.aout.magic != 0410) {
#else
	if(BADMAG(aout)) {
#endif
		printf("invalid a.out format\n");
		exit(1);
	}
#if	vax
	offset = PAGESZ - (sgs.aout.tsize % PAGESZ) - HDR;
#endif
#if	m68k
	toffset = sgs.scnhdr[0].s_scnptr - sgs.scnhdr[0].s_paddr;
	doffset = sgs.scnhdr[1].s_scnptr - sgs.scnhdr[1].s_paddr;
#endif
#ifdef	pdp11
	ovly = (aout.a_magic == A_MAGIC5);
	if (ovly) printf ("* PDP11 overlay system\n");
#endif
	if((sysseek = fopen(os,"r")) == NULL) {
		fprintf(stderr,"cannot read %s\n",os);
		exit(1);
	}
	if((mast = fopen(mr, "r")) == NULL) {
		fprintf(stderr, "cannot read %s\n", mr);
		exit(1);
	}

	nlptr = nl;
	lnptr = ln;
#ifdef	m68k
	bdevcnt = setup("bdevcnt");
	bdevsw = setup("bdevsw");
	rootdev = setup("rootdev");
	pipedev = setup("pipedev");
	swapdev = setup("swapdev");
	swplo = setup("swplo");
	nswap = setup("nswap");
	dumpdev = setup("dumpdev");
	Power = setup("pwr_clr");
	Sema = setup("seminfo");
	Messag = setup("msginfo");
	Shmem = setup("shminfo");
	X25 = setup("x25info");
	vs = setup("v");
	sysdef1 = setup("sysdef1");
	sysdef2 = setup("sysdef2");
	sysdef3 = setup("sysdef3");
	sysdefr = setup("sysdefr");
	sysdefp = setup("sysdefp");
	sysdevs = setup("sysdevs");
	ram_nbr = setup("ram_nbr");
	ram_tbl = setup("ram_tbl");
	probe_tbl = setup("probe_tbl");
#else
	start = setup("start");
	restart = setup("restart");
	UNIvec = setup("_UNIvec");
	Mbacf = setup("_mbacf");
	umemvad = setup("_umemvad");
	bdevcnt = setup("_bdevcnt");
	mbacnt = setup("_mba_cnt");
	bdevsw = setup("_bdevsw");
	rootdev = setup("_rootdev");
	pipedev = setup("_pipedev");
	swapdev = setup("_swapdev");
	swplo = setup("_swplo");
	nswap = setup("_nswap");
	dumpdev = setup("_dumpdev");
	Power = setup("_pwr_clr");
	Sema = setup("_seminfo");
	Messag = setup("_msginfo");
	X25 = setup("_x25info");
#endif
#if	vax
	Shmem = setup("_shminfo");
#endif
#ifdef	pdp11
	Maus = setup("_mausmap");
#endif
#if	PRE
	pre[0] = '_';
#endif

	while(fgets(line, 256, mast) != NULL) {
		if(line[0] == '*')
			continue;
		if(line[0] == '$')
			break;
		if (sscanf(line, "%s %d %d %o %s %d %o %d %d %d ",
			nm,&vsz,&x,&typ,&pre[PRE],&x,&x,&x,&deft,&bus,&x) != 10)
			continue;
#ifdef	m68k
		if ((typ & BLOCK) == 0 || vsz != 4)
			continue;
#endif
		if(!strcmp(nm, "memory") || !strcmp(nm, "tty"))
			continue;
		strcpy(intrnm, pre);
		strcpy(devaddr, pre);
		strcpy(devcnt, pre);
		strcat(devaddr, "_addr");	/* _pre_addr */
		strcat(devcnt, "_cnt");	/* _pre_cnt */
#ifdef	m68k
		strcat(intrnm, "int");
#else
		switch(vsz) {
			case 8: strcat(intrnm, "rint"); break;
			case 1: /* massbus devices */
			case 4: strcat(intrnm, "intr"); break;
			case 0: break; /* pseudo devices */
			default: continue;
		}
#endif
		setln(nm, setup(intrnm), vsz, deft, typ & BLOCK, bus);
		setup(devaddr);
		setup(devcnt);
		if(typ & BLOCK) {
			strcpy(strat, pre);
			strcat(strat, "strategy");
			setup(strat);
		}
	}
	endnm = setup("");
	endln = lnptr;
	nlist(os, nl);

#ifdef	DEBUG
/*
 *	Print out the namelist
 */
	for(lnptr=ln; lnptr != endln; ++lnptr) {
	    register k;
	    k =  lnptr->l_dtype & BLOCK ? STRAT : CNT;
	    for ( ; k >= INTR ; k-- )
		if ((lnptr->l_nptr[k]).n_type || (lnptr->l_nptr[k]).n_value)
			printf(NBPW==4
				? "%-8.8s\t%o\t%x\t%d\n" : "%-8.8s\t%o\t%o%d\n",
				(lnptr->l_nptr[k]).n_name,
				(lnptr->l_nptr[k]).n_type,
				(lnptr->l_nptr[k]).n_value, k);
	}
#endif

	fseek(system, (long) SYM_VALUE(bdevcnt) - offset, 0);
	fread(&bcnt, INTSZ, 1, system);
#ifndef	m68k
	fseek(system, (long) SYM_VALUE(mbacnt) - offset, 0);
	fread(&mcnt, INTSZ, 1, system);
#endif
	fseek(system, (long) SYM_VALUE(rootdev) - offset, 0);
	fread(&root, SHORTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(pipedev) - offset, 0);
	fread(&pipe, SHORTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(swapdev) - offset, 0);
	fread(&swap, SHORTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(dumpdev) - offset, 0);
	fread(&dump, SHORTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(Power) - offset, 0);
	fread(&power, INTSZ, 1, system);
#ifdef	pdp11
	fseek(system, (long) SYM_VALUE(Maus) - offset, 0);
	fread(&maus, INTSZ, 1, system);
#endif
#ifdef	BSWAP
	lswap(&bcnt);
#ifndef	m68k
	lswap(&mcnt);
#endif
	lswap(&power);
#ifdef	pdp11
	lswap(&maus);
#endif
#endif

#ifdef	vax
	vaxdevs();
#endif
#ifdef	pdp11
	pdpdevs();
#endif
#ifdef	m68k
	m68kdevs();
#else

	printf("*\n* Pseudo Devices\n*\n");
	pseudo_devs();
#endif

	/* rootdev, swapdev, pipedev, dumpdev, power */

	printf("*\n* System Devices\n*\n");

#ifdef	m68k
	if (m68ksysdevs()) {
#endif
	if((lnptr = majsrch((root >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown root device\n");
	else
		printf("root\t%s\t%o\n", lnptr->l_cfnm, root & 0377);

	if((lnptr = majsrch((swap >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown swap device\n");
	else {
		printf("swap\t%s\t%o\t", lnptr->l_cfnm, swap & 0377);
		fseek(system, (long) SYM_VALUE(swplo) - offset, 0);
		fread(&swap_low, INTSZ, 1, system);
		fseek(system, (long) SYM_VALUE(nswap) - offset, 0);
		fread(&n_swap, INTSZ, 1, system);
#ifdef	BSWAP
		lswap(&swap_low);
		lswap(&n_swap);
#endif
		printf("%ld\t%d\n", swap_low, n_swap);
	}

	if((lnptr = majsrch((pipe >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown pipe device\n");
	else
		printf("pipe\t%s\t%o\n", lnptr->l_cfnm, pipe & 0377);

	if((lnptr = majsrch((dump >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown dump device\n");
	else
		printf("dump\t%s\t%o\n", lnptr->l_cfnm, dump & 0377);
#ifdef	m68k
	}
#endif

	/* easy stuff */

#ifndef	m68k
	nlptr = nl;
	vs = setup("_v");
	smap = setup("_swapmap");
	callo = setup("_callout");
	endnm = setup("");
	nlist(os, nl);
	for(nlptr = vs; nlptr != endnm; nlptr++) {
#ifdef	vax
		if(nlptr->n_value == 0) {
#else
		if(nlptr->n_type == 0) {
#endif
			fprintf(stderr, "namelist error\n");
			exit(1);
		}
	}
#else	/* m68k: only need to verify v was found */
	if (!SYM_FOUND(vs)) {
		fprintf(stderr, "\"v\" structure not found\n");
		exit(1);
	}
#endif
	fseek(system, (long) SYM_VALUE(vs) - offset, 0);
	fread(&v, sizeof v, 1, system);
#ifdef	BSWAP
	lswap(&v.v_buf);
	lswap(&v.v_call);
	lswap(&v.v_inode);
	lswap(&v.v_file);
	lswap(&v.v_mount);
	lswap(&v.v_proc);
	lswap(&v.v_text);
	lswap(&v.v_clist);
	lswap(&v.v_sabuf);
	lswap(&v.v_maxup);
#if	pdp11 || m68k
	lswap(&v.v_cmap);
#endif
	lswap(&v.v_smap);
	lswap(&v.v_hbuf);
	lswap(&v.v_pbuf);
#ifdef	pdp11
	lswap(&v.v_iblk);
#endif
	lswap(&X25->n_value);
#endif
	printf("*\n* Tunable Parameters\n*\n");
	printf("buffers\t%11d\n", v.v_buf);
	printf("calls\t%11d\n", v.v_call);
	printf("inodes\t%11d\n", v.v_inode);
	printf("files\t%11d\n", v.v_file);
	printf("mounts\t%11d\n", v.v_mount);
	printf("procs\t%11d\n", v.v_proc);
	printf("texts\t%11d\n", v.v_text);
	printf("clists\t%11d\n", v.v_clist);
	printf("sabufs\t%11d\n", v.v_sabuf);
	printf("maxproc\t%11d\n", v.v_maxup);
#if	pdp11 || m68k
	printf("coremap\t%11d\n", v.v_cmap);
#endif
	printf("swapmap\t%11d\n", v.v_smap);
	printf("hashbuf\t%11d\n", v.v_hbuf);
	printf("physbuf\t%11d\n", v.v_pbuf);
#ifdef	pdp11
	printf("iblocks\t%11d\n", v.v_iblk);
#endif
	if(X25->n_value != 0) {
		fseek(system, (long) SYM_VALUE(X25) - offset, 0);
		fread(&x25info, sizeof(x25info), 1, system);
		printf("x25links%11d\n", x25info.x25links);
		printf("x25bufs\t%11d\n", x25info.x25bufs);
		printf("x25bytes  (%d*1024)\n",(x25info.x25bytes/1024));
	}
	printf("power\t%11d\n", power ? 1 : 0);
	printf("mesg\t%11d\n", Messag->n_value ? 1 : 0);
	printf("sema\t%11d\n", Sema->n_value ? 1 : 0);
#if	vax || m68k
	printf("shmem\t%11d\n", Shmem->n_value ? 1 : 0);
#endif
#ifdef	pdp11
	printf("maus\t%11d\n", (maus != -1) ? 1 : 0);
#endif
#ifdef	m68k
	m68kextras();
#endif
	exit(0);
}

/*
 * setup - add an entry to a namelist structure array
 */
struct	nlist	*
setup(nam)
	char	*nam;
{
	if(nlptr >= &nl[MAXI]) {
		fprintf(stderr, "internal name list overflow\n");
		exit(1);
	}
	strncpy(nlptr->n_name, nam, 8);
	nlptr->n_type = 0;
	nlptr->n_value = 0;
	return(nlptr++);
}

/*
 * setln - set up internal link structure for later
 * function look-up.  Records useful information from the
 * /etc/master table description.
 */
struct	link	*
setln(cf, np, vz, dft, dt, br)
	char	*cf;
	struct	nlist	*np;
	int	vz;
	int	dft;
	int	dt;
	int	br;
{
	if(lnptr >= &ln[MAXI/3]) {
		fprintf(stderr, "internal link array overflow\n");
		exit(1);
	}
	strcpy(lnptr->l_cfnm, cf);
	lnptr->l_nptr = np;
	lnptr->l_vsz = vz;
	lnptr->l_def = dft;
	lnptr->l_dtype = dt;
	lnptr->l_bus = br;
	return(lnptr++);
}

/*
 * search - search for a function indirectly through
 * the link structure.
 */
#ifndef	m68k
struct	link	*
search(vctr,sub)
#ifdef	vax
	int	vctr;
#else
	struct	interface	*vctr;
#endif
	int	sub;
{
	register struct	link	*lnkptr;
#ifdef pdp11
	struct xinterface xi;
	long	off;
#endif

	for(lnkptr = ln; lnkptr != endln; lnkptr++) {
#ifdef	vax
#ifdef	DEBUG
printf("link val=%x vctr=%x\n",(lnkptr->l_nptr[sub]).n_value, (vctr & ~D7));
#endif
		if((lnkptr->l_nptr[sub]).n_value == (vctr & ~D7)) {
#else
#ifdef	DEBUG
printf("link val=%o vctr=%o\n",(lnkptr->l_nptr[sub]).n_value, vctr->i_func);
#endif
		if((lnkptr->l_nptr[sub]).n_value == (unsigned)vctr->i_func) {
#endif
			lnkptr->l_used = (char)1;
			return(lnkptr);
		}
	}
#ifdef pdp11
       /*
	* if overlay-loaded system, use interface table address to follow
	* chain into overlay segments and give search a second chance.
	*/
	if (ovly) {
#ifdef DEBUG
		printf("in overlay search\n");
#endif
		off = (unsigned) vctr->i_func;
		fseek(system, off + TXT, 0);
		fread(&xi, sizeof(xi), 1, system);
		for(lnkptr = ln; lnkptr != endln; lnkptr++) {
#ifdef	DEBUG
printf("link val=%o vctr=%o\n",(lnkptr->l_nptr[sub]).n_value, xi.i_func);
#endif
			if((lnkptr->l_nptr[sub]).n_value == (unsigned)xi.i_func-4) {
				lnkptr->l_used = (char)1;
				return(lnkptr);
			}
		}
	}
#endif
	return(NULL);
}
#else	/* m68k */
struct	link	*
search(vctr,sub)
	long	vctr;
	int	sub;
{
	register struct	link	*lnkptr;
	register long value;

	for(lnkptr = ln; lnkptr != endln; lnkptr++) {
		value = (lnkptr->l_nptr[sub]).n_value;
		if(value == vctr) {
			lnkptr->l_used = (char)1;
			return(lnkptr);
		}
	}
	return(NULL);
}
#endif

/*
 * majsrch - search for a link structure given the major
 * device number of the device in question.
 */
struct	link *
majsrch(maj)
	int	maj;
{
	register struct	link	*lnkptr;
#ifdef pdp11
	struct xinterface	xi;
	long	off;
#endif

	if(maj > bcnt - 1)
		return(NULL);

	fseek(system,(long)SYM_VALUE(bdevsw)+(maj*sizeof(bent))-offset,0);
	fread(&bent, sizeof bent, 1, system);
#ifdef	BSWAP
	lswap(&bent.b_open);
	lswap(&bent.b_close);
	lswap(&bent.b_strategy);
	lswap(&bent.b_tab);
#endif

	for(lnkptr = ln; lnkptr != endln; lnkptr++)
		if(lnkptr->l_dtype) {
#ifdef	DEBUG
#ifdef	pdp11
printf("link val=%o bent=%o\n",(lnkptr->l_nptr[STRAT]).n_value, bent.b_strategy);
#else
printf("link val=%x vctr=%x\n",(lnkptr->l_nptr[STRAT]).n_value, bent.b_strategy);
#endif
#endif
			if((lnkptr->l_nptr[STRAT]).n_value == bent.b_strategy)
				return(lnkptr);
		}
#ifdef pdp11
       /*
	* if overlay-loaded system, use interface table address to follow
	* chain into overlay segments and give search a second chance.
	*/
	if (ovly) {
#ifdef DEBUG
		printf("in overlay search\n");
#endif
		off = (unsigned) bent.b_strategy;
		fseek(system, off + TXT, 0);
		fread(&xi, sizeof(xi), 1, system);
		for(lnkptr = ln; lnkptr != endln; lnkptr++) {
#ifdef	DEBUG
printf("link val=%o vctr=%o\n",(lnkptr->l_nptr[STRAT]).n_value, xi.i_func);
#endif
			if((lnkptr->l_nptr[STRAT]).n_value == (unsigned)xi.i_func-4) {
				lnkptr->l_used = (char)1;
				return(lnkptr);
			}
		}
	}
#endif
	return(NULL);
}

#ifdef	vax
vaxdevs()
{
	register int i;
	if(UNIvec->n_value == 0) {
		fprintf(stderr, "%s %s\n", "symbol \"UNIvec\" undefined; ",
			"invalid /unix file");
		exit(1);
	}
	if(Mbacf->n_scnum == 0) {
		fprintf(stderr, "%s %s\n", "symbol \"_mbacf\" undefined; ",
			"invalid /unix file");
		exit(1);
	}

	printf("*\n* MASSBUS\n* dev\tnexus\taddr\tbus\n*\n");

/* MASSBUS devices from _mbacf */
	if(fseek(system, (long)SYM_VALUE(Mbacf) - offset, 0) != NULL) {
		fprintf(stderr, "sysdef: seek failed on %s\n",system);
		exit(1);
	}

	for(i=0; i < mcnt; ++i) {
		fread(&mbadev, sizeof mbadev, 1, system);
		if(mbadev.nexus == NULL) continue;
		if((lnptr = search(mbadev.dev_addr,ADDR)) == NULL) {
			fprintf(stderr,
				"unknown device at NEXUS %3d\n", mbadev.nexus);
			continue;
		}

		printf("%s\t%3d\t%d\t%d",
			lnptr->l_cfnm,mbadev.nexus,0,mbadev.bus_req);
		printf("\n");
	}

	if(fseek(system, (long)SYM_VALUE(UNIvec) - offset, 0) != NULL) {
		fprintf(stderr, "sysdef: seek failed on %s\n",system);
		exit(1);
	}

/* on a VAX the following works only for UNIBUS devices */

	printf("*\n* UNIBUS\n* dev\tvector\taddr\tbus\tcount\n*\n");

	for(vec=0; vec < (int *)(INTSZ * 128); ++vec) {
		fread(&vector, sizeof vector, 1, system);
		if(vector.vaxvec == NULL) continue;
		if((lnptr = search(vector.vaxvec,INTR)) == NULL) {
			fprintf(stderr,"unknown device at vector %3o\n", vec);
			continue;
		}
		printf("%s\t%3o", lnptr->l_cfnm, vec);
		unit = (vector.vaxvec & D7) >> 27;
		if(fseek(sysseek,
			(long)(((lnptr->l_nptr[ADDR]).n_value & USRMASK)
			- ftell(sysseek) + (INTSZ * unit) - offset), 1) != NULL) {
			fprintf(stderr,"bad seek for device address\n");
		}
		if(fread(&address, sizeof(address), 1, sysseek) != 1) {
			fprintf(stderr,"cannot read dev_addr\n");
		}

		printf("\t%lo\t%1o",(address), lnptr->l_bus);

		fseek(sysseek, (long)(lnptr->l_nptr[CNT].n_value & USRMASK)
			- ftell(sysseek) - offset, 1);
		fread(&count, INTSZ, 1, sysseek);
		if((unit + 1) * lnptr->l_def > count)
			printf("\t%d", count % lnptr->l_def);
		else
			printf("\t%d", lnptr->l_def);
		printf("\n");

		if(lnptr->l_vsz == 8) {
			vec++;
			fread(&vector, sizeof vector, 1, system);
		}
	}
}
#endif

#ifdef	pdp11
pdpdevs()
{
	register int i;
	if(start->n_type == 0) {
		fprintf(stderr, "%s %s\n",
			"symbol \"start\" undefined; ",
			"invalid /unix file");
		exit(1);
	}
	if(restart->n_value) {
		IFC = 01020;
		IFC_SZ = sizeof(xifc[0]);
		if((start->n_value - IFC) > (MAXI / 3 * (sizeof ifc[0]))) {
			printf("internal interface array overflow\n");
			exit(1);
		}
		fseek(system, (long) IFC + TXT, 0);
		fread(xifc, sizeof xifc[0], N_IFC, system);
		for(i=0; i < N_IFC; ++i) {
			ifc[i].i_jsr = xifc[i].i_jsr;
			ifc[i].i_call = xifc[i].i_call;
			ifc[i].i_func = (xifc[i].i_func + IFC + 
				( i + 1) * 8);
		}
	}
	else {
		if((start->n_value - IFC) > (MAXI / 3 * (sizeof ifc[0]))) {
			fprintf(stderr, "internal interface array overflow\n");
			exit(1);
		}
		fseek(system, (long) IFC - offset, 0);
		fread(ifc, sizeof ifc[0], (start->n_value - IFC) /
			sizeof ifc[0], system);
	}

	printf("*\n* dev\tvector\taddr\tbus\tcount\n*\n");
	for(addr = (struct vector *)0; addr != (struct vector *)01000; addr++) {
		if(addr == (struct vector *)060)
			addr = (struct vector *)070;	/* skips console */
		fseek(system, (long)addr - offset, 0);
		fread(&vector, sizeof vector, 1, system);
		if((vector.v_pc <= IFC) || (vector.v_pc >= start->n_value))
			continue;	/* skips clio, traps, jmp, etc */
		if((lnptr = search(&ifc[(vector.v_pc - IFC) / IFC_SZ],INTR))
			== NULL) {
			fprintf(stderr,
				"unknown device interrupts at vector %3o\n",
				addr);
			continue;
		}
		printf("%s\t%3o", lnptr->l_cfnm, addr);
		unit = vector.v_psw & CC;
		bus = (vector.v_psw & BUS) >> 5;
		fseek(system,(long)(lnptr->l_nptr[ADDR].n_value+2*unit-offset),0);
		fread(&address, 2, 1, system);
		printf("\t%6o\t%1o", address, bus);
		fseek(system, (long)(lnptr->l_nptr[CNT].n_value - offset), 0);
		fread(&count, NBPW, 1, system);
		if((unit + 1) * lnptr->l_def > count)
			printf("\t%d", count % lnptr->l_def);
		else
			printf("\t%d", lnptr->l_def);
		printf("\n");
		if(lnptr->l_vsz == 8)
			addr++;
	}
}
#endif

#ifndef	m68k
pseudo_devs()
{
	register struct link	*lnkptr;
	int			count;

	for(lnkptr = ln; lnkptr != endln; lnkptr++) {
		if( ! (lnkptr->l_nptr[CNT]).n_value) {  /* dev_cnt undefined*/
			continue;
		}
		if(lnkptr->l_used) {	/*has already been printed*/
			continue;
		}
		if(strcmp(lnkptr->l_cfnm,"dl11") == 0) { /* /dev/console */
			continue;
		}
		fseek(system, (long)
			((lnkptr->l_nptr[CNT]).n_value & USRMASK) - offset, 0);
		fread(&count, INTSZ, 1, system);
		printf("%s\t%d\t%d\t%d\t%d\n",lnkptr->l_cfnm,0,0,0,count);
	}
}
#endif

#ifdef	BSWAP
/*
 * m68k/vax swap the bytes of a long
 */
lswap(ap)
long *ap;
{
	short b[2];
	register short *bp = b;
	register short *cp = (short *) ap;

	bp[0] = cp[1];
	bp[1] = cp[0];
	*ap = *(long*)b;
}
#endif

#ifdef	m68k
/*
 * seek to a symbol on the system file
 */
sym_seek(sym)
struct nlist *sym;
{
	if (fseek(system, (long) SYM_VALUE(sym), 0) != NULL) {
		fprintf(stderr,"sysdef: sym_seek failed\n");
		exit(1);
	}
}

/*
 * read an int from the system file
 */
int_read(addr)
int *addr;
{
	if (fread(addr, sizeof *addr, 1, system) != 1) {
		fprintf(stderr,"sysdef: int_read failed\n");
		exit(1);
	}
#ifdef	BSWAP
	lswap(addr);
#endif
}

/*
 * read a short from the system file
 */
short_read(addr)
short *addr;
{
	if (fread(addr, sizeof *addr, 1, system) != 1) {
		fprintf(stderr,"sysdef: short_read failed\n");
		exit(1);
	}
}

/*
 * load sysdef1,2,3 data into the char array sysdef[]
 */
getdef(sym)
register struct nlist *sym;
{
	register char *p = sysdef;

	if (!SYM_FOUND(sym)) {
		*p = 0;
		return(1);
	}
#ifdef	BSWAP
	{	register i = (SYM_VALUE(sym)&1) == 0;
		if (fseek(system, (long) SYM_VALUE(sym)&~1, 0) != NULL) {
			fprintf(stderr,"sysdef: system seek failed\n");
			exit(1);
		}
		for (;; i=1) {
			char bytes[2];

			short_read(bytes);
			for ( ; i >= 0; i--) {
				if (p == &sysdef[sizeof sysdef]) {
					fprintf(stderr, "sysdef: sysdef[] overflow\n");
					exit(1);
				}
				if ((*p++ = bytes[i]) == 0)
					return(0);
			}
		}
	}
#else
	sym_seek(sym);
	for (;;) {
		if (p == &sysdef[sizeof sysdef]) {
			fprintf(stderr, "sysdef: sysdef buffer overflow\n");
			exit(1);
		}
		if ((*p++ = fgetc(system)) == 0)
			return(0);
	}
#endif
}

/*
 * print the contents of sysdef[]
 */
prdef()
{
	register char *p = sysdef;
	register char c;

	while (c = *p++)
		putchar(c);
}

/*
 * Print the device data from sysdef1
 */
m68kdevs()
{
	printf("* m68kvec\n*\n* dev\tvector\taddr\tbus\tcount\n*\n");

	if (getdef(sysdef1)) {
		fprintf(stderr,
			"symbol \"sysdef1\" missing; invalid /unix file");
		exit(1);
	}
	prdef();
}

/*
 * Print the system devices from the "sysdevs" table if possible.
 * If not, return true so that the normal means is tried.
 */
m68ksysdevs()
{
	register struct link *lptr;
	register short i;
	short ndevs;
	struct d {
		unsigned short root;
		unsigned short pipe;
		unsigned short swap;
		unsigned short dump;
		int swplo, nswap;
	} d[16];
	register struct d *dp;

	/* check if it's reasonible to do it this way */
	if (!SYM_FOUND(sysdevs))
		return(1);
	sym_seek(sysdevs);
	short_read(&ndevs);
	if (ndevs < 2)
		return(1);
	if (ndevs > 16) {
		fprintf(stderr,"sysdef: too many system devices: %u\n", ndevs);
		ndevs = 16;
	}

	/* read the sysdevs table */
	if (fread(d, sizeof (struct d), ndevs, system) != ndevs)
		return(1);
	/* root */
	if ((lptr = majsrch((root >> 8) & 0377)) == NULL) {
		fprintf(stderr, "unknown root device\n");
		printf("root\t?\t");
	} else
		printf("root\t%s\t", lptr->l_cfnm);
	for (i=0, dp=d ; ; ) {
		printf("%02o", dp++->root & 0377);
		if (++i == ndevs) {
			putchar('\n');
			break;
		}
		printf(", ");
	}

	/* swap */
	if ((lptr = majsrch((swap >> 8) & 0377)) == NULL) {
		fprintf(stderr, "unknown swap device\n");
		printf("swap\t?\t");
	} else
		printf("swap\t%s\t", lptr->l_cfnm);
	for (i=0, dp=d ; ; dp++) {
#ifdef	BSWAP
		lswap(&dp->swplo);
		lswap(&dp->nswap);
#endif
		printf("%02o %d %d", dp->swap & 0377, dp->swplo, dp->nswap);
		if (++i == ndevs) {
			putchar('\n');
			break;
		}
		printf(", ");
	}

	/* pipe */
	if ((lptr = majsrch((pipe >> 8) & 0377)) == NULL) {
		fprintf(stderr, "unknown pipe device\n");
		printf("pipe\t?\t");
	} else
		printf("pipe\t%s\t", lptr->l_cfnm);
	for (i=0, dp=d ; ; ) {
		printf("%02o", dp++->pipe & 0377);
		if (++i == ndevs) {
			putchar('\n');
			break;
		}
		printf(", ");
	}

	/* dump */
	if ((lptr = majsrch((dump >> 8) & 0377)) == NULL) {
		fprintf(stderr, "unknown dump device\n");
		printf("dump\t?\t");
	} else
		printf("dump\t%s\t", lptr->l_cfnm);
	for (i=0, dp=d ; ; ) {
		printf("%02o", dp++->dump & 0377);
		if (++i == ndevs) {
			putchar('\n');
			break;
		}
		printf(", ");
	}
	return(0);
}

/*
 * print the extra stuff that supports m68k unix
 */
m68kextras()
{
	/* check if any m68k specific parameters */

	if (!SYM_FOUND(sysdef2) && !SYM_FOUND(sysdef3)
	&&  !SYM_FOUND(sysdefr) && !SYM_FOUND(sysdefp))
		return;

	printf("*\n* m68k Specific Parameters\n*\n");

	/* print any non-var "parameters" */

	if (getdef(sysdef2) == 0)
		prdef();

	/* print any stuff from sysdef3 */

	if (getdef(sysdef3) == 0)
		prdef();

	/*
	 * print the ram table.  The presence of the symbol "sysdefr"
	 * means that the ram table was specified, rather than default.
	 * ram tables can only be specified in the dfile.
	 */
	if (SYM_FOUND(sysdefr)) {
		unsigned nram;
		register unsigned i, j;

		sym_seek(ram_nbr);
		int_read(&nram);
		sym_seek(ram_tbl);
		for (i = nram ; i ; i--) {
			unsigned short ram[4];
#define	R_LOW	ram[0]
#define	R_HIGH	ram[1]
#define	R_SIZE	ram[2]
#define	R_FLAG	ram[3]

			if (fread(ram, sizeof ram, 1, system) != 1) {
				fprintf(stderr,"sysdef: system read failed\n");
				exit(1);
			}
			/* reconstruct the dfile's ram flag */
			j = (R_FLAG&1) == 0;	/* init bit */
			if (R_FLAG&2) {
				j |= 4;		/* multiple */
				if (R_SIZE == (R_HIGH - R_LOW)) {
					R_SIZE = 0;
					j |= 2;	/* single */
				}
			}
			printf("ram\t%o\t%x\t%x\t%x\n",
				j, ctob(R_LOW), ctob(R_HIGH), ctob(R_SIZE));
		}
	}

	/*
	 * print the probe table.  The presence of the symbol "sysdefp" means
	 * that at least part of the probe table was specified in the dfile.
	 * sysdefp contains the index number of the last probe table entry
	 * from the master file.
	 */
	if (SYM_FOUND(sysdefp)) {
		short nprobe;

		sym_seek(sysdefp);
		short_read(&nprobe);
		if (!SYM_FOUND(probe_tbl)) {
			fprintf(stderr,"sysdef: \"probe_tbl\" missing\n");
			exit(1);
		}
		/* go to first probe_tbl entry from the dfile */
		if (fseek(system,
		(long) SYM_VALUE(probe_tbl) + nprobe * (INTSZ + SHORTSZ), 
		0) != NULL) {
			fprintf(stderr,"sysdef: system probe_tbl seek failed\n");
			exit(1);
		}
		for (;;) {
			long p_addr;
			short p_byte;

			int_read(&p_addr);
			if (p_addr == 0)
				break;
			short_read(&p_byte);
			if (p_byte < 0)
				printf("probe\t%x\t-1\n", p_addr);
			else
				printf("probe\t%x\t%x\n", p_addr, p_byte);
		}
	}
}
#endif	/* m68k */
