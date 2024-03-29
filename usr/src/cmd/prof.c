/*	@(#)prof.c	2.1		*/
/*
 *	Program profiling report generator.
 *
 *	Usage:
 *
 *	prof [ -[ntca] ] [ -[ox] ] [ -g ] [ -z ] [ -s ] [ -m mdata ] [ prog ]
 *
 *	Where "prog" is the program that was profiled; "a.out" by default.
 *	Options are:
 *
 *	-n	Sort by symbol name.
 *	-t	Sort by decreasing time.
 *	-c	Sort by decreasing number of calls.
 *	-a	Sort by increasing symbol address.
 *
 *	The options that determine the type of sorting are mutually exclusive.
 *	Additional options are:
 *
 *	-o	Include symbol addresses in output (in octal).
 *	-x	Include symbol addresses in output (in hexadecimal).
 *	-g	Include non-global T-type symbols in output.
 *	-z	Include all symbols in profiling range, even if zero
 *			number of calls or time.
 *	-h	Suppress table header.
 *	-s	Follow report with additional statistical information.
 *	-m mdata Use file "mdata" instead of MON_OUT for profiling data.
 */

#include <stdio.h>
#include <a.out.h>
#include <sys/types.h>			/* Needed for "stat.h". */
#include <sys/stat.h>
#include <sys/param.h>			/* for HZ */
#include <mon.h>

/* The ISMAGIC macro should be defined in some system (i.e. global)
 * header.  Until it is, it is defined here.  It can also be found
 * in various copies of sgs.h.  This macro requires a.out.h for
 * the magic numbers.
 */

#if m68k
#define ISMAGIC(x)      (((unsigned short)x)==(unsigned short)MC68MAGIC)
#endif
#if vax
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)VAXROMAGIC) || \
			  (((unsigned short)x)==(unsigned short)VAXWRMAGIC))
#endif
#if u3b
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)N3BMAGIC) || \
			(((unsigned short)x)==(unsigned short)NTVMAGIC))
#endif

#define PROC				/* Mark procedure names. */
#define Print	(void)printf
#define Fprint	(void)fprintf

#if m68k
	/* Max positive difference between a fnpc and sl_addr for match */
#define CCADIFF 22
#endif

#if vax
	/* Max positive difference between a fnpc and sl_addr for match */
#define CCADIFF	22
	/* Type if n_type field in file symbol table entry. */
#endif

#if pdp11
	/* Max positive difference between a fnpc and sl_addr for match */
#define CCADIFF 16
	/* Type of n_type field in file symbol table. */
typedef int type_t;
#endif

#if (u3b)
	/* Max positive difference between a fnpc and sl_addr for match */
#define CCADIFF	20	/*   ?? (16 would probably do) */
	/* For u3b, the "type" is storage class + section number (no type_t) */
#endif

#define SEC(ticks) ((double)(ticks)/HZ)		/* Convert clock ticks to seconds. */

#if pdp11
	/* Title fragment used if symbol addresses included in output */
char atitle[] = " Addr. ";
	/* Format for addresses in output. */
char aformat[] = "%6o ";
#endif
#if ( m68k || u3b || vax )
	/* Title fragment used if symbol addresses in output ("-o" or "-x"). */
char atitle[] = " Address ";
	/* Format for addresses in output */
char aformat[] = "%8o ";
#endif

#if !( m68k || vax || pdp11 || u3b)	  
	/* Make sure something we are set up for.  Else lay egg. */
#include "### No code for processor type ###"
#endif

   /* Used for "unsigned fixed-point fraction with binary scale at the left". */
#define BIAS	((long)0200000L)

#if pdp11
	/* Test proper symbol type. */
#define TXTSYM(t) (((t) & gmask) == gmatch)
	/* Default symbol type mask. */
# define STABTYPES	0	/* sdb not implemented */
type_t	gmask = STABTYPES | N_TYPE | N_EXT;
	/* Default symbol type when masked with above. */
type_t	gmatch = N_TEXT | N_EXT;
#endif

#if ( m68k || u3b || vax)	  
	/* Test proper symbol type. */
#define TS1(s)	((s>0) && strcmp(scnhdrp[s-1].s_name, _TEXT) == 0)  
#define TS2(c)	(c == C_EXT || (c == C_STAT && gflag))
#define TS3(n)	(strcmp(n, _TEXT) != 0)		/* do not want ".text" */
#define TXTSYM(s,c,n)	(TS1(s) && TS2(c) && TS3(n))		 
int gflag = 0;			/*  replaces gmatch and gmask */
#endif

#ifdef ddt
int	debug;
#define DEBUG(exp)	exp
#else
#define DEBUG(exp)
#endif

FILE	*sym_iop;		/* For program ("a.out") file. */
#if	FLEXNAMES
	FILE	*sym_strs_iop;	/* pointer to extract strings */
	long	sym_strs_start;	/* start of strings in object file */
#endif
FILE	*mon_iop;		/* For profile (MON_OUT) file. */
char	*sym_fn = "a.out";	/* Default program file name. */
char	*mon_fn = MON_OUT;	/* Default profile file name.
					May be changed by "-m file". */

extern char *realloc(), *strncpy(), *optarg;
extern int errno, optind;
extern long strtol();
extern void qsort(), exit(), perror();

	/* For symbol table entries read from program file. */
#if ( m68k || u3b || vax)
#define nlist syment
#endif
struct nlist nl;

	/* Number of chars in a symbol. For subsequent sizes & counts. */
#define N_NAME	sizeof(nl.n_name)

/* Compare routines called from qsort(). */

int	c_ccaddr();	/* Compare fnpc fields of cnt structures. */

int	c_sladdr();	/* Compare   sl_addr fields of slist structures. */

int	c_time();	/*	"    sl_time    "    "   "	"      */

int	c_name();	/*	"    sl_name    "    "   "	"      */

int	c_ncalls();	/*	"    sl_count   "    "   "	"      */

	/* Other stuff. */

	/* Return size of open file (arg is file descriptor) */
off_t	fsize();

	/* Memory allocation. Like malloc(), but no return if error. */
char	*Malloc();

	/* Scan past path part (if any) in the ... */
char	*basename();

	/* command name, for error messages. */
char	*cmdname;

/* Structure of subroutine call counters (cnt) is defined in mon.h. */

/* Structure for header of mon.out (hdr) is defined in mon.h. */

	/* Local representation of symbols and call/time information. */
struct slist {
#ifndef FLEXNAMES
	char sl_name[N_NAME];	/* Symbol name. */
#else
	union					/* NOTE:	 */
	{					/* mirrors	 */
		char	_sl_name[N_NAME];	/* syment struct */
		struct				/* from a.out.h  */
		{
			long _sl_zeroes;
			long _sl_offset;
		} _sl_sl;
		char	*_sl_nptr[2];
	} _sl;
#define sl_name		_sl._sl_name
#define sl_nptr		_sl._sl_nptr[1]
#define sl_zeroes	_sl._sl_sl._sl_zeroes
#define sl_offset	_sl._sl_sl._sl_offset
#endif
	char *sl_addr;		/* Address. */
	long sl_count;		/* Count of subroutine calls */
	float sl_time;		/* Count of clock ticks in this routine,
						converted to secs. */
};

#if ( m68k || u3b || vax)
#define AOUTHSZ		(filhdr.f_opthdr)
FILHDR	filhdr;		/* a.out file header */
AOUTHDR	aouthdr;	/* a.out Unix (optional) header */
SCNHDR	*scnhdrp;	/* pointer to first section header (space by Malloc) */
#endif
#if (pdp11)
struct exec fhead;	/* Program file ("a.out") header. */
#endif

struct hdr head;	/* Profile file (MON_OUT) header. */

int	(*sort)() = NULL;	/* Compare routine for sorting output symbols.
						Set by "-[acnt]". */

int	flags;		/* Various flag bits. */

char	*pc_l;		/* From head.lpc. */

char	*pc_h;		/*   "  head.hpc. */

/* Bit macro and flag bit definitions. */

#define FBIT(pos)	(01 << (pos))	/* Returns value with bit pos set. */
#define F_SORT		FBIT(0)		/* Set if "-[acnt]" seen. */
#define F_VERBOSE	FBIT(1)		/* Set if "-s" seen. */
#define F_ZSYMS		FBIT(2)		/* Set if "-z" seen. */
#define F_PADDR		FBIT(3)		/* Set if "-o" or "-x" seen. */
#define F_NHEAD		FBIT(4)		/* Set if "-h" seen. */

PROC
main(argc, argv)
int argc;
char **argv;
{
	char buffer[BUFSIZ];	/* buffer for printf */

	WORD *pcounts;	/* Pointer to allocated area for
						pcounts: PC clock hit counts */

	register WORD *pcp;	/* For scanning pcounts. */

	struct cnt *ccounts;	/* Pointer to allocated area for cnt
					structures: subr PC-call counts. */

	register struct cnt *ccp;	/* For scanning ccounts. */

	struct slist *slist;	/* Pointer to allocated slist structures: symbol
						name/address/time/call counts */

	register struct slist *slp;	/* For scanning slist */

	int vn_cc, n_cc;	/* Number of cnt structures in profile data
						file (later # ones used). */

	int n_pc;	/* Number of pcounts in profile data file. */

	int n_syms;	/* Number of text symbols (of proper type)
					that fill in range of profiling. */

	int n_nonzero;	/* Number of (above symbols) actually printed
					because nonzero time or # calls. */

	int symttl;	/* Total # symbols in program file sym-table */

	int i;

	register int n;

	off_t symaddr;	/* Address of symbol table in program file. */

	long sf;	/* Scale for index into pcounts:
						i(pc) = ((pc - pc_l) * sf)/sf. */

	long s_inv;	/* Inverse: i_inv(i) =
					{pc00, pc00+1, ... pc00+s_inv-1}. */

	unsigned pc_m;	/* Range of PCs profiled: pc_m = pc_h - pc_l */

	float t, t0;
	float t_tot;	/* Total time: SEC(sum of all pcounts[i]) */

	setbuf(stdout, buffer);
	cmdname = basename(*argv);	/* command name. */

	while ((n = getopt(argc, argv, "canthsgzoxT:m:")) != EOF) {

		switch (n) {
		int (*fcn)();	/* For function to sort results. */

		case 'm':	/* Specify data file:	-m file */
			mon_fn = optarg;
			break;

#ifdef ddt
		case 'T':	/* Set trace flags: -T(octnum) */
			debug = (int)strtol(optarg, 0, 8);
			break;
#endif

		case 'n':	/* Sort by symbol name. */
			fcn = c_name;
			goto check;

		case 't':	/* Sort by decreasing time. */
			fcn = c_time;
			goto check;

		case 'c':	/* Sort by decreasing # calls. */
			fcn = c_ncalls;
			goto check;

		case 'a':	/* Sort by increasing symbol address
						(don't have to -- it will be) */
			fcn = NULL;
		check:		/* Here to check sort option conflicts. */
			if (sort != NULL && sort != fcn) {
				Fprint(stderr,
			   "%s: Warning: %c overrides previous specification\n",
				    cmdname, n);
			}
			sort = fcn;	/* Store sort routine */
			flags |= F_SORT; /* Note have done so */
			break;

		case 'o':	/* Include symbol addresses in output. */
		case 'x':	/* Include symbol addresses in output. */
			aformat[2] = n;	/* 'o' or 'x' in format */
			flags |= F_PADDR;	/* Set flag. */
			break;

		case 'g':	/* Include local T symbols as well as global*/
#if (pdp11)
			gmatch = N_TEXT; /* Value to match ...*/
			gmask &= ~N_EXT; /* using this mask. */
#endif
#if ( m68k || u3b || vax)
			gflag++;	  
#endif
			break;

		case 'z':	/* Print all symbols in profiling range,
				 		 even if no time or # calls. */
			flags |= F_ZSYMS;	/* Set flag. */
			break;

		case 'h':	/* Suppress table header. */
			flags |= F_NHEAD;
			break;

		case 's':	/* Follow normal output with extra summary. */
			flags |= F_VERBOSE;	/* Set flag (...) */
			break;

		case '?':	/* But no good. */
			Fprint(stderr,
			    "%s: Unrecognized option: %c\n", cmdname, n);
			exit(1);

		}	/* End switch (n) */
	}	/* End while (getopt) */

	if (optind < argc)
		sym_fn = argv[optind];	/* name other than `a.out' */

	if (sort == NULL && !(flags & F_SORT))
				/* If have not specified sort mode ... */
		sort = c_time;		/* then sort by decreasing time. */

		/* Open program file (has symbols). */
	if ((sym_iop = fopen(sym_fn, "r")) == NULL)
		Perror(sym_fn);

#if	FLEXNAMES
		/* Open program file for symbol table strings */
	if ((sym_strs_iop = fopen(sym_fn, "r")) == NULL)
		Perror(sym_fn);
#endif

		/* Open monitor data file (has counts). */
	if ((mon_iop = fopen(mon_fn, "r")) == NULL)
		Perror(mon_fn);

		/* Read header of executable file. */
#if ( m68k || u3b || vax)	/*  read filhdr and aouthdr instead of fhead */
	(void)fread((char *)&filhdr, FILHSZ, 1, sym_iop);
	(void)fread((char *)&aouthdr, AOUTHSZ, 1, sym_iop);
	/*  read section headers to get true test for is a symbol .text */
	scnhdrp = (SCNHDR *) Malloc(filhdr.f_nscns, SCNHSZ);
	(void)fread((char *)scnhdrp, SCNHSZ, filhdr.f_nscns, sym_iop);
	if (!(ISMAGIC(filhdr.f_magic) && AOUTHSZ == sizeof aouthdr)) {
#endif
#if (pdp11)
	(void)fread((char *)&fhead, sizeof(struct exec), 1, sym_iop);
	if (BADMAG(fhead)) {		/* Verify proper "magic" number. */
#endif

		Fprint(stderr, "%s: %s: improper format\n", cmdname, sym_fn);
		exit(1);
	}

	/* Compute the file address of symbol table. Machine-dependent. */
#if pdp11
		/* Size of text & data. */
	symaddr = (long)fhead.a_text + (long)fhead.a_data;
	if (!fhead.a_flag)		/* If relocation bits there, */
		symaddr += symaddr;	/* then double the offset. */
	symaddr += sizeof(struct exec);	/* Now add in fixed header size. */
#endif
#if ( m68k || u3b || vax)
	symaddr = filhdr.f_symptr;	  
#endif

		/* Number of symbols in file symbol table. */
#if ( m68k || u3b || vax)
	symttl = filhdr.f_nsyms;	  
#endif
#if (pdp11)
	symttl = fhead.a_syms/sizeof(struct nlist);
#endif
	if (symttl == 0) {		/* This is possible. */
		Fprint(stderr, "%s: %s: no symbols\n", cmdname, sym_fn);
		exit(0);		/* Note zero exit code. */
	}

	/* Get size of file containing profiling data. Read header part. */
	n = fsize(fileno(mon_iop));
	if (fread((char *)&head, sizeof(struct hdr), 1, mon_iop) != 1)
		eofon(mon_iop, mon_fn);		/* Probably junk file. */

	/* Get # cnt structures (they follow header),
						and allocate space for them. */

	n_cc = head.nfns;
	ccounts = (struct cnt *)Malloc(n_cc, sizeof(struct cnt));

		/* Read the call addr-count pairs. */
	if (fread((char *)ccounts, sizeof(struct cnt), n_cc, mon_iop) != n_cc)
		eofon(mon_iop, mon_fn);

	/* Compute # PC counters (pcounts), which occupy whatever is left
				of the file after the header and call counts. */

	n_pc = (n - sizeof(head) - n_cc * sizeof(struct cnt))/sizeof(WORD);
	ccp = &ccounts[n_cc];	/* Point to last (+1) of call counters ... */
	do {		/* and scan backward until find highest one used. */
		if ((--ccp)->mcnt)
			break;		/* Stop when find nonzero count. */
	} while (--n_cc > 0);		/* Or all are zero. */

	/* If less than all cnt entries are used, return unused space. */
	if (n_cc < head.nfns) {
		if ((ccp = (struct cnt *)realloc((char *)ccounts,
		    (unsigned)n_cc * sizeof(struct cnt))) == NULL)
			snh();	/* Should not fail when reducing size. */
	}

	/* If more than 250 cnt entries used set verbose for warning */
	if (n_cc > (MPROGS0 * 5)/6)
		flags |= F_VERBOSE;

		/* Space for PC counts. */
	pcounts = (WORD *)Malloc(n_pc, sizeof(WORD));
		/* Read the PC counts from rest of MON_OUT file. */
	if (fread((char *)pcounts, sizeof(WORD), n_pc, mon_iop) != n_pc)
		eofon(mon_iop, mon_fn);

	/*
	Having gotten preliminaries out of the way, get down to business.
	The range pc_m of addresses over which profiling was done is
	computed from the low (pc_l) and high (pc_h) addresses, gotten
	from the MON_OUT header.  From this and the number of clock
	tick counters, n_pc, is computed the so-called "scale", sf, used
	in the mapping of addresses to indices, as follows:

			(pc - pc_l) * sf
		i(pc) = ----------------
			 0200000

	Also, the N-to-one value, s_inv, such that

		i(pc_l + K * s_inv + d) = K, for 0 <= d < s_inv

	Following this, the symbol table is scanned, and those symbols
	that qualify are counted.  These  are T-type symbols, excluding
	local (nonglobal) unless the "-g" option was given. Having thus
	determined the space requirements, space for symbols/times etc.
	is allocated, and the symbol table re-read, this time keeping
	qualified symbols.
	*/


	pc_l = head.lpc;	/* Low PC of range that was profiled. */
	pc_h = head.hpc;	/* First address past range of profiling. */
	pc_m = pc_h - pc_l;	/* Range of profiled addresses. */

DEBUG(if (debug) Fprint(stderr,
"low pc = %#o, high pc = %#o, range = %#o = %u\n\
call counts: %u, %u used; pc counters: %u\n",
pc_l, pc_h, pc_m, pc_m, head.nfns, n_cc, n_pc);)

	sf = (BIAS * n_pc)/pc_m;  /* The "scale" used to map PCs to indices. */
	s_inv = pc_m/n_pc;	/* Range of PCs mapped into one index. */

DEBUG(if (debug) Fprint(stderr, "sf = %#lo, s_inv = %ld\n", (long)sf, s_inv);)

		/* Prepare to read symbols from "a.out" (or whatever). */
	(void)fseek(sym_iop, symaddr, 0);
#if	FLEXNAMES
		/* compute start of string table in "a.out" (or whatever) */
	sym_strs_start = symaddr + (symttl * sizeof(struct syment));
		/* position string pointer to start of flexnames */
	(void)fseek(sym_strs_iop, sym_strs_start, 0);
#endif
	n_syms = 0;			/* Init count of qualified symbols. */
	n = symttl;			/* Total symbols. */
	while (--n >= 0)			/* Scan symbol table. */
		if (readnl())	/* Read and examine symbol, count qualifiers */
			n_syms++;

DEBUG(if (debug) Fprint(stderr, "%u symbols, %u qualify\n", symttl, n_syms);)

		/* Allocate space for qualified symbols. */

	slist = slp = (struct slist *)Malloc(n_syms, sizeof(struct slist));
DEBUG(debug &= ~020;)

		/* Re-position to beginning of symbol table. */
	(void)fseek(sym_iop, symaddr, 0);
#if	FLEXNAMES
		/* Re-position string pointer to start of flexnames */
	(void)fseek(sym_strs_iop, sym_strs_start, 0);
#endif
	for (n = n_syms; n > 0; ) {	/* Loop on number of qualified symbols. */
		if (readnl()) {	/* Get one. Check again. */
				/* Is qualified. Move name ... */
#if	FLEXNAMES
			(void)namecopy(slp, &nl);
#else
			(void)strncpy(slp->sl_name, nl.n_name, N_NAME);
#endif
				/* and address into slist structure. */
			slp->sl_addr = (char *)nl.n_value;

				/* set other slist fields to zero. */
			slp->sl_time = 0.0;
			slp->sl_count = 0;
DEBUG(if (debug & 02) Fprint(stderr,
"%-8.8s: %#8o\n", slp->sl_name, slp->sl_addr);)

			slp++;
			--n;
		}
	}

	/*
	Now attempt to match call counts with symbols.  To do this, it
	helps to first sort both the symbols and the call address/count
	pairs by ascending address, since they are generally not, to
	begin with.  The addresses associated with the counts are not,
	of course, the subroutine addresses associated with the symbols,
	but some address slightly past these. Therefore a given count
	address (in the fnpc field) is matched with the closest symbol
	address (sl_addr) that is:
		(1) less than the fnpc value but,
		(2) not more than CCADIFF bytes less than it.
	The value of CCADIFF is roughly the size of the code between
	the subroutine entry and that following the call to the mcount
	routine.  In other words, unreasonable matchups are avoided.
	Situations such as this could arise when static procedures are
	counted but the "-g" option was not given to this program,
	causing the symbol to fail to qualify.  Without this limitation,
	unmatched counts could be erroneously charged.
	*/


	ccp = ccounts;			/* Point to first call counter. */
	slp = slist;			/*   "		"   "   symbol. */
		/* Sort call counters and ... */
	qsort((char *)ccp, (unsigned)n_cc, sizeof(struct cnt), c_ccaddr);
		/* symbols by increasing address. */
	qsort((char *)slp, (unsigned)n_syms, sizeof(struct slist), c_sladdr);
	vn_cc = n_cc;			/* save this for verbose option */
		/* Loop to match up call counts & symbols. */
	for (n = n_syms; n > 0 && vn_cc > 0; ) {
		if (slp->sl_addr < ccp->fnpc &&
		    ccp->fnpc <= slp->sl_addr+CCADIFF) {

DEBUG(if (debug & 04) Fprint(stderr,
"Routine %-8.8s @ %#8x+%-2d matches count address %#8x\n",
slp->sl_name, slp->sl_addr, ccp->fnpc-slp->sl_addr, ccp->fnpc);)

			slp->sl_count = ccp->mcnt;	/* Copy count. */
			++ccp;
			++slp;
			--vn_cc;
			--n;
		} else if (ccp->fnpc < slp->sl_addr) {
			++ccp;
			--vn_cc;
		} else {
			++slp;
			--n;
		}
	}

	/*
	The distribution of times to addresses is done on a proportional
	basis as follows: The t counts in pcounts[i] correspond to clock
	ticks for values of pc in the range pc, pc+1, ..., pc+s_inv-1
	(odd addresses excluded for PDP11s).  Without more detailed
	information, it must be assumed that there is no greater probability
	of the clock ticking for any particular pc in this range than for
	any other.  Thus the t counts are considered to be equally distributed
	over the addresses in the range, and that the time for any given
	address in the range is pcounts[i]/s_inv.

	The values of the symbols that qualify, bounded below and above
	by pc_l and pc_h, respectively, partition the profiling range into
	regions to which are assigned the total times associated with the
	addresses they contain in the following way:

	The sum of all pcounts[i] for which the corresponding addresses are
	wholly within the partition are charged to the partition (the
	subroutine whose address is the lower bound of the partition).

	If the range of addresses corresponding to a given t = pcounts[i]
	lies astraddle the boundary of a partition, e.g., for some k such
	that 0 < k < s_inv-1, the addresses pc, pc+1, ..., pc+k-1 are in
	the lower partition, and the addresses pc+k, pc+k+1, ..., pc+s_inv-1
	are in the next partition, then k*pcounts[i]/s_inv time is charged
	to the lower partition, and (s_inv-k) * pcounts[i]/s_inv time to the
	upper.  It is conceivable, in cases of large granularity or small
	subroutines, for a range corresponding to a given pcounts[i] to
	overlap three regions, completely containing the (small) middle one.
	The algorithm is adjusted appropriately in this case.
	*/


	pcp = pcounts;				/* Reset to base. */
	slp = slist;				/* Ditto. */
	t0 = 0.0;				/* Time accumulator. */
	for (n = 0; n < n_syms; n++) {		/* Loop on symbols. */
			/* Start addr of region, low addr of overlap. */
		char *pc0, *pc00;
			/* Start addr of next region, low addr of overlap. */
		char *pc1, *pc10;
		 /* First index into pcounts for this region and next region. */
		register int i0, i1;
		long ticks;

			/* Address of symbol (subroutine). */
		pc0 = slp[n].sl_addr;

			/* Address of next symbol, if any or top of profile
								range, if not */
		pc1 = (n < n_syms - 1) ? slp[n+1].sl_addr : pc_h;

			/* Lower bound of indices into pcounts for this range */

		i0 = ((pc0 - pc_l) * sf)/BIAS;

			/* Upper bound (least or least + 1) of indices. */
		i1 = ((pc1 - pc_l) * sf)/BIAS;

		if (i1 >= n_pc)				/* If past top, */
			i1 = n_pc - 1;				/* adjust. */

			/* Lowest addr for which count maps to pcounts[i0]; */
		pc00 = pc_l + (long)((BIAS * i0)/sf);

			/* Lowest addr for which count maps to pcounts[i1]. */
		pc10 = pc_l + (long)((BIAS * i1)/sf);

DEBUG(if (debug & 010) Fprint(stderr,
"%-8.8s\ti0 = %4d, pc00 = %#6o, pc0 = %#6o\n\
\t\ti1 = %4d, pc10 = %#6o, pc1 = %#6o\n\t\t",
slp[n].sl_name, i0, pc00, pc0, i1, pc10, pc1);)

		t = 0;			/* Init time for this symbol. */
		if (i0 == i1) {
			/* Counter overlaps two areas? (unlikely unless large
								granularity). */
			ticks = pcp[i0];	/* # Times (clock ticks). */

			    /* Time less that which overlaps adjacent areas */
			t += (pc1 - pc0) * SEC(ticks)/s_inv;

DEBUG(if (debug & 010) Fprint(stderr, "%ld/%ld", (pc1 - pc0) * ticks, s_inv);)
		} else {
				/* Overlap with previous region? */
			if (pc00 < pc0) {
				ticks = pcp[i0];

				/* Get time of overlapping area and subtract
						proportion for lower region. */
				t += SEC(pcp[i0]) - (pc0 - pc00) * SEC(ticks)/s_inv;

				/* Do not count this time when summing times
						wholly within the region. */
				i0++;
DEBUG(if (debug & 010) Fprint(stderr, "%ld/%ld + ", (pc0 - pc00) * ticks, s_inv);)
			}

			/* Init sum of counts for PCs not shared w/other
								routines. */
			ticks = 0;

			/* Stop at first count that overlaps following
								routine. */
			for (i = i0; i < i1; i++)
				ticks += pcp[i];

			t += SEC(ticks);  /* Convert to secs & add to total. */
DEBUG(if (debug & 010) Fprint(stderr, "%ld", ticks);)
			/* Some overlap with low addresses of next routine? */
			if (pc10 < pc1) {
					/* Yes. Get total count ... */
				ticks = pcp[i1];

				/* and accumulate proportion for addresses in
							range of this routine */
				t += (pc1 - pc10) * SEC(ticks)/s_inv;
DEBUG(if (debug & 010) Fprint(stderr, " + %ld/%ld", (pc1 - pc10) * ticks, s_inv);)
			}
		}		/* End if (i0 == i1) ... else ... */

		slp[n].sl_time = t;	/* Store time for this routine. */
		t0 += t;		/* Accumulate total time. */
DEBUG(if (debug & 010) Fprint(stderr, " ticks = %.2f msec\n", t);)
	}	/* End for (n = 0; n < n_syms; n++) */

	/* Final pass to total up time. */

	for (n = n_pc, t_tot = 0.0; --n >= 0; t_tot += SEC(*pcp++));

	/*
	Most of the heavy work is done now.  Only minor stuff remains.
	The symbols are currently in address order and must be re-sorted
	if desired in a different order.  Report generating options
	include "-o" or "-x": Include symbol address, which causes another column
	in the output; and "-z": Include symbols in report even if zero
	time and call count.  Symbols not in profiling range are excluded
	in any case.  Following the main body of the report, the "-s"
	option causes certain additional information to be printed.
	*/

DEBUG(if (debug) Fprint(stderr,
"Time unaccounted for: %.7G\n", t_tot - t0);)

	if (sort)	/* If comparison routine given then use it. */
		qsort((char *)slp, (unsigned)n_syms, sizeof(struct slist), sort);

	if (!(flags & F_NHEAD)) {
		if (flags & F_PADDR)
			Print(atitle);	/* Title for addresses. */
#if	FLEXNAMES
		(void)puts("Name                 %Time Seconds Cumsecs  #Calls   msec/call");
#else
		(void)puts("Name     %Time Seconds Cumsecs  #Calls   msec/call");
#endif FLEXNAMES
	}
	t = 0.0;			/* Init cumulative time. */
	if (t_tot != t)			/* Convert to percent. */
		t_tot = 100.0/t_tot;	/* Prevent divide-by-zero fault */
	n_nonzero = 0;	/* Number of symbols with nonzero time or # calls. */
	for (n = n_syms, slp = slist; --n >= 0; slp++) {
		long count = slp->sl_count;	/* # Calls. */

		t0 = slp->sl_time;	/* Time (sec). */
		if (t0 == 0.0 && count == 0 && !(flags & F_ZSYMS))
			continue; /* Don't do entries with no action. */
		n_nonzero++;		/* Count # entries printed. */
		if (flags & F_PADDR)	/* Printing address of symbol? */
			Print(aformat, slp->sl_addr);
		t += t0;	/*  move here; compiler bug  !! */
#ifdef FLEXNAMES
		Print("%-20s%6.1f%8.2f%8.2f",	/* Accumulate time. */
		    (slp->sl_zeroes ? slp->sl_name : slp->sl_nptr),
		    t0 * t_tot, t0, t);
#else
		Print("%-8.8s%6.1f%8.2f%8.2f",	/* Accumulate time. */
		    slp->sl_name, t0 * t_tot, t0, t);
#endif FLEXNAMES
		if (count) {		/* Any calls recorded? */
		/* Get reasonable number of fractional digits to print. */
			int fdigits = fprecision(count);
			Print("%8ld%#*.*f", count, 8 + fdigits, fdigits,
			    1000.0*t0/count);
		}
		(void)putchar('\n');		/* Wrap up line. */
	}

	if (flags & F_VERBOSE) {		/* Extra info? */
		Fprint(stderr, "%5d/%d call counts used\n", n_cc, head.nfns);
		Fprint(stderr, "%5d/%d symbols qualified", n_syms, symttl);
		if (n_nonzero < n_syms)
			Fprint(stderr,
			    ", %d had zero time and zero call-counts\n",
			    n_syms - n_nonzero);
		else
			(void)putc('\n', stderr);
		Fprint(stderr, "%#x scale factor\n", (long)sf);
	}
	exit(0);
}

/* Return size of file associated with file descriptor fd. */

PROC off_t
fsize(fd)
{
	struct stat sbuf;

	if (fstat(fd, &sbuf) < 0)		/* Status of open file. */
		Perror("stat");
	return (sbuf.st_size);			/* This is a long. */
}

/* Read symbol entry. Return TRUE if satisfies conditions. */
/*  readnl() entirely new for COFF */

PROC
readnl()
{

#if ( m68k || u3b || vax)   /*  for COFF, special care for auxiliary entries */
	static char numaux = 0;	  
	if (numaux <= 0) {	/* a new symbol table entry */
		if (fread((char *)&nl, SYMESZ, 1, sym_iop) != 1)
			eofon(sym_iop, sym_fn);
		numaux = nl.n_numaux;
	}
	else {
		if (fread((char *)&nl, AUXESZ, 1, sym_iop) != 1)
			eofon(sym_iop, sym_fn);
		numaux--;
		return(0);	/*  skip over auxiliary entries */
	}
	
#if	FLEXNAMES
	if (!nl.n_zeroes)
	{
		register char *pnl_name;
		static char nl_namebuf[BUFSIZ];

		pnl_name = nl_namebuf;
		fseek(sym_strs_iop,sym_strs_start+nl.n_offset,0);
		do
			fread(pnl_name,1,1,sym_strs_iop);
		while(*pnl_name++);
		nl.n_nptr = nl_namebuf;
	}
#endif FLEXNAMES

DEBUG(if (debug & 020)
Fprint(stderr, "`%-8.8s'\tclass=%#4o, value=%#8.6o\n",
nl.n_name, (unsigned char)nl.n_sclass, nl.n_value);)
	/*
	TXTSYM accepts global (and local, if "-g" given) T-type symbols.
	Only those in the profiling range are useful.
	*/
	
#if	FLEXNAMES
		return (TXTSYM(nl.n_scnum, nl.n_sclass, 
		(nl.n_zeroes ? nl.n_name : nl.n_nptr)) &&
		pc_l <= (char *)nl.n_value && (char *)nl.n_value < pc_h &&
		(nl.n_sclass == C_EXT || 
		(nl.n_zeroes ? strcmp(nl.n_name,"_eprol") : 1 )));
#else
		return (TXTSYM(nl.n_scnum, nl.n_sclass, nl.n_name) &&
		pc_l <= (char *)nl.n_value && (char *)nl.n_value < pc_h &&
		(nl.n_sclass == C_EXT || 
		strcmp(nl.n_name, "_eprol")));
#endif FLEXNAMES

#endif

#if pdp11
	if (fread((char *)&nl, sizeof(struct nlist), 1, sym_iop) != 1)
		eofon(sym_iop, sym_fn);
DEBUG(if (debug & 020)
Fprint(stderr, "`%-8.8s'\ttype=%#4o, value=%#8.6o\n",
nl.n_name, (unsigned char)nl.n_type, nl.n_value);)
	/*
	TXTSYM accepts global (and local, if "-g" given) T-type symbols.
	Only those in the profiling range are useful.
	*/
	return (TXTSYM(nl.n_type) &&
		pc_l <= (char *)nl.n_value && (char *)nl.n_value < pc_h &&
		(nl.n_type == N_EXT || strcmp(nl.n_name, "eprol")));
#endif

}


#if	FLEXNAMES


namecopy(sp, np)
struct slist *sp;
struct nlist *np;
{
	if( np->n_zeroes )
		strncpy(sp->sl_name,np->n_name,SYMNMLEN);
	else
	{
		sp->sl_nptr = (char *)Malloc((strlen(np->n_nptr)+1),sizeof(char));
		strcpy(sp->sl_nptr,np->n_nptr);
	}
}
#endif

/* Error-checking memory allocator. Guarentees good return (else none at all).*/

PROC char *
Malloc(item_count, item_size)
{
	char *malloc();
	register char *p;

	if ((p = malloc((unsigned)item_count * (unsigned)item_size)) == NULL)
		Perror("Out of space");
	return (p);
}

/*
	Given the quotiant Q = N/D, where entier(N) == N and D > 0, an
	approximation of the "best" number of fractional digits to use
	in printing Q is f = entier(log10(D)), which is crudely produced
	by the following routine.
*/

PROC int
fprecision(count)
long count;
{
	return (count < 10 ? 0 : count < 100 ? 1 : count < 1000 ? 2 :
	    count < 10000 ? 3 : 4);
}

/*
	Return pointer to base name (name less path) of string s.
	Handles case of superfluous trailing '/'s, and unlikely
	case of s == "/".
*/

PROC char *
basename(s)
register char *s;
{
	register char *p;

	p = &s[strlen(s)];			/* End (+1) of string. */
	while (p > s && *--p == '/')		/* Trim trailing '/'s. */
		*p = '\0';
	p++;					/* New end (+1) of string. */
	while (p > s && *--p != '/');		/* Break backward on '/'. */
	if (*p == '/')		/* If found '/', point to 1st following. */
		p++;
	if (*p == '\0')
		p = "/";			/* If NULL, must be "/". (?) */
	return (p);
}

/* Here if unexpected read problem. */

PROC
eofon(iop, fn)
register FILE *iop;
register char *fn;
{
	if (ferror(iop))		/* Real error? */
		Perror(fn);		/* Yes. */
	Fprint(stderr, "%s: %s: Premature EOF\n", cmdname, fn);
	exit(1);
}

/* Version of perror() that prints cmdname first. */

PROC
Perror(s)
char *s;
{				/* Print system error message & exit. */
	register int err = errno;	/* Save current errno in case */

	Fprint(stderr, "%s: ", cmdname);
	errno = err;			/* Put real error back. */
	perror(s);			/* Print message. */
	exit(1);			/* Exit w/nonzero status. */
}

/* Here for things that "Should Never Happen". */

PROC
snh()
{
	Fprint(stderr, "%s: Internal error\n", cmdname);
	(void)abort();
}

/*
	Various comparison routines for qsort. Uses:

	c_ccaddr	- Compare fnpc fields of cnt structs to put
				call counters in increasing address order.
	c_sladdr	- Sort slist structures on increasing address.
	c_time		-  "	 "	  "      " decreasing time.
	c_ncalls	-  "	 "	  "      " decreasing # calls.
	c_name		-  "	 "	  "      " increasing symbol name
*/

#define CMP2(v1,v2)	((v1) < (v2) ? -1 : (v1) == (v2) ? 0 : 1)
#define CMP1(v)		CMP2(v, 0)

PROC
c_ccaddr(p1, p2)
register struct cnt *p1, *p2;
{
	return (CMP2(p1->fnpc, p2->fnpc));
}

PROC
c_sladdr(p1, p2)
register struct slist *p1, *p2;
{
	return (CMP2(p1->sl_addr, p2->sl_addr));
}

PROC
c_time(p1, p2)
register struct slist *p1, *p2;
{
	register float dtime = p2->sl_time - p1->sl_time; /* Decreasing time. */

	return (CMP1(dtime));
}

PROC
c_ncalls(p1, p2)
register struct slist *p1, *p2;
{
	register int diff = p2->sl_count - p1->sl_count; /* Decreasing # calls. */

	return (CMP1(diff));
}

PROC
c_name(p1, p2)
register struct slist *p1, *p2;
{
#if	FLEXNAMES
	register int diff;

	if( p1->sl_zeroes && p2->sl_zeroes )
		diff = strncmp(p1->sl_name,p2->sl_name,N_NAME);
	else if( !p1->sl_zeroes && !p2->sl_zeroes )
		diff = strcmp(p1->sl_nptr,p2->sl_nptr);
	else if( p1->sl_zeroes )
	{
		if( !(diff = strncmp(p1->sl_name,p2->sl_nptr,N_NAME)) )
			diff = -1;
	}
	else	/* only one possibility left */
	{
		if( !(diff = strncmp(p1->sl_nptr,p2->sl_name,N_NAME)) )
			diff = 1;
	}
#else
	register int diff = strncmp(p1->sl_name,p2->sl_name,N_NAME);
#endif FLEXNAMES
			
	return (CMP1(diff));
}
