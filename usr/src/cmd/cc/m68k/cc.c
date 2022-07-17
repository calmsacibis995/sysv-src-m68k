/*	@(#)cc.c	2.1		*/
static	char sccsid[] = "@(#) cc.c:	2.3 83/07/08";
/*
 * cc - front end for C compiler
 */
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/dir.h>
#include "paths.h"

#ifndef	BINDIR				/* default /bin directory */
#define	BINDIR	""
#endif

#ifndef	LIBDIR				/* default /lib directory */
#define	LIBDIR	""
#endif

#ifndef	NAMEcpp
#define	NAMEcpp	"cpp"			/* base preprocessor name */
#endif
#ifndef	NAMEccom
#define	NAMEccom	"ccom"		/* base compiler name */
#endif
#ifndef	NAMEas
#define	NAMEas	"as"			/* base assembler name */
#endif
#ifndef	NAMEoptim
#define	NAMEoptim	"optim"		/* base optimizer name */
#endif
#ifndef	NAMEld
#define	NAMEld	"ld"			/* base loader name */
#endif
#ifndef	NAMEcrt0
#define	NAMEcrt0	"crt0.o"	/* base startup routine name */
#endif
#ifndef	NAMEmcrt0
#define	NAMEmcrt0	"mcrt0.o"
#endif

char	*cpp;				/* pointer to true cpp name */
char	*ccom;				/* same for compiler */
char	*as;				/* ... etc. */
char	*c2;
char	*ld;
char	*crt0;

char	*prefix;			/* prefixing string in cc command;
					** i.e., "mc68" in "mc68cc"
					*/
#define	Xc0	0			/* numbers for passes */
#define	Xc1	1
#define	Xc2	2
#define	Xcp	3
#define	Xas	4
#define	Xld	5
#define	NPASS	6

char	*tmp0;				/* holds temp. file template */
char	*tmp1, *tmp2, *tmp3, *tmp4, *tmp5;
char	*outfile;
char	*savestr(), *strspl(), *setsuf(), *getpref(), *makename();
char	*strtok();
int	idexit();
char	**av, **clist, **llist, **plist;
char	**list[NPASS];
int	nlist[NPASS];
int	cflag, eflag, gflag, pflag, sflag, wflag, Rflag, exflag, proflag, oflag, debug;
int truncflag;				/* non-zero to truncate flexnames */
#ifdef	BLIT
int	jflag, mflag=1, rflag=1;	/* Used for jerq. Rob Pike (read comment as you will) */
#endif
	/* Note: default is to compile for mpx(1) */
char	*dflag;
int	exfail;
char	*chpass;
char	*npassname;
extern	int	optind;
extern	int	opterr;
extern	char	*optarg;
extern	int	optopt;

int	nc, nl, np, nxo, na;

#define	cunlink(x)	if (x) unlink(x)

main(argc, argv)
	char **argv;
{
	char *t;
	char *assource;
	int i, j, c, ic;
	int nargs = argc + 1;

	/* build initially assumed program names */

	prefix = getpref(argv[0]);	/* get prefix string */

	cpp = makename(LIBDIR,prefix,NAMEcpp);	/* C preprocessor */
	ccom = makename(LIBDIR,prefix,NAMEccom); /* compiler */
	as = makename(BINDIR,prefix,NAMEas);	/* assembler */
	c2 = makename(LIBDIR,prefix,NAMEoptim);	/* optimizer */
	ld = makename(BINDIR,prefix,NAMEld);	/* loader */
#ifdef	BLIT
	crt0 = "/usr/jerq/lib/notsolow.o";
#else
	crt0 = makename(LIBDIR,prefix,NAMEcrt0);/* run-time start-up */
#endif


	/* ld currently adds upto 5 args; 10 is room to spare */
	/* I upped it anyway -- rob */
	av = (char **)calloc(argc+20, sizeof (char **));

	/* initialize arrays for extra arguments (-W) */

	for (i = NPASS; i-- > 0; ) {
	    nlist[i] = 0;
	    list[i] = (char **) calloc(nargs, sizeof(char **));
	}
	clist = (char **)calloc(argc, sizeof (char **));
	llist = (char **)calloc(argc, sizeof (char **));
	plist = (char **)calloc(argc, sizeof (char **));
	opterr = 0;

#ifdef	BLIT
#define	OPTS	"jmrsSTo:ROXP#gwEPcD:I:U:C:t:B:l:W:"
#else
#define	OPTS	"psSTo:ROXP#gwEPcD:I:U:C:t:B:l:W:"
#endif

	while (optind<argc) switch (c = getopt(argc, argv, OPTS)) {
	case 'S':
		sflag++;
		cflag++;
		continue;
#ifdef	BLIT
	case 'j':
		if(mflag==0)
			error("only use one of -j and -m", "");
		else{
			jflag++;
			rflag=0;
			mflag=0;
			crt0 = "/usr/jerq/lib/l.o";
		}
		continue;
	case 'm':
		if(jflag)
			error("only use one of -j and -m", "");
		else{
			crt0="/usr/jerq/lib/crt0.o";
			mflag=0;
			rflag=0;
		}
		continue;
	case 'r':
		rflag++;
		continue;
#endif	/* def BLIT */
	case 'l':
		llist[nl++] = strspl("-l", optarg);
		continue;
	case 'o':
		outfile = optarg;
		switch (getsuf(outfile)) {

		case 'c':
		case 'o':
			error("-o would overwrite %s", outfile);
			exit(8);
		}
		continue;
	case 'R':
		Rflag++;
		continue;
	case 'X':
#ifdef	BLIT
		ccom="/usr/scj/mcc/comp";
#endif
		continue;
	case 'O':
		oflag++;
		continue;
	case 'p':
		proflag++;
		continue;
	case '#':
		debug = 1;
		continue;
	case 'g':
		gflag++;
		continue;
	case 'w':
		wflag++;
		continue;
	case 'E':
		exflag++;
	case 'P':
		pflag++;
		if (optarg != 0)
		    plist[np++] = optarg;
	case 'c':
		cflag++;
		continue;
	case 'D':
	case 'I':
	case 'U':
	case 'C':
		plist[np] = strspl("-X", optarg);
		plist[np++][1] = c;
		continue;
	case 't':
		if (chpass)
			error("-t overwrites earlier option", 0);
		chpass = optarg;
		if (chpass[0]==0)
			chpass = "012p";
		continue;
	case 'B':
		if (npassname)
			error("-B overwrites earlier option", 0);
		npassname = optarg;
		if (npassname[0]==0)
#ifdef	BLIT
			npassname = "/usr/scj/mcc";
#else
			npassname = makename(LIBDIR,"o",prefix);
#endif
		continue;
	case 'W':
		if((optarg[1] != ',')
			|| ((t = strtok(optarg, ",")) != optarg)){
			error("Invalid subargument: -W%s", optarg);
			continue;
		}
		if((i = getXpass((ic = *t), "-W")) == -1)
			continue;
		while((t = strtok(NULL, ",")) != NULL) {
			if(nlist[i] >= nargs){
				error("Too many arguments for pass -W%c", ic);
				continue;
			}
			list[i][nlist[i]++] = t;
		}
		continue;

	case 'd':
		dflag = optarg;
		continue;

	case '?':
		if ( optopt=='o' ) {
			error("No output file found for '-o' option\n",outfile);
			exit(8);
		}
	case 's':
		t = strspl("-", "x");
		t[1] = optopt;
		llist[nl++] = t;
		continue;

	case 'T':			/* to truncate flexnames */
		truncflag++;
		continue;

	case EOF:
		t = argv[optind];
		optind++;
		c = getsuf(t);
		if (c=='c' || c=='s' || exflag) {
			clist[nc++] = t;
			t = setsuf(t, 'o');
		}
		if (nodup(llist, t)) {
			llist[nl++] = t;
			if (getsuf(t)=='o')
				nxo++;
		}
	}
	if (npassname && chpass ==0)
		chpass = "012p";
	if (chpass && npassname==0)
#ifdef	BLIT
		npassname = "/usr/new";
#else
		npassname = makename(LIBDIR,"n",prefix);
#endif
	if (chpass)
	for (t=chpass; *t; t++) {
		switch (*t) {

		case '0':
			ccom = strspl(npassname, NAMEccom);
			continue;
		case '2':
			c2 = strspl(npassname, NAMEoptim);
			continue;
		case 'p':
			cpp = strspl(npassname, NAMEcpp);
			continue;
		case 'a':
			as = strspl(npassname, NAMEas);
			continue;
		case 'l':
			ld = strspl(npassname, NAMEld);
			continue;
		}
	}
	if ( oflag && gflag ) {
	        error("Warning, -g overrides -O", "");
		oflag = 0;
		eflag--;	/* don't cause error exit */
		cflag--;
		exfail--;
	}
	if (proflag)
		crt0 = makename(LIBDIR,prefix,NAMEmcrt0);
	if (nc==0)
		goto nocom;
	if (eflag)
		dexit();
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, idexit);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, idexit);
	tmp0 = tempnam(NULL,"ctm");	/* form temporary filename template */
	if (pflag==0) {
		tmp1 = strspl(tmp0, "1");
		tmp2 = strspl(tmp0, "2");
		tmp3 = strspl(tmp0, "3");
		tmp4 = strspl(tmp0, "4");
	}
	if (oflag)
		tmp5 = strspl(tmp0, "5");
	for (i=0; i<nc; i++) {
		if (nc > 1) {
			printf("%s:\n", clist[i]);
			fflush(stdout);
		}
		if (getsuf(clist[i]) == 's' && !pflag) {
			assource = clist[i];
			goto assemble;
		} else
			assource = tmp3;
		if (pflag)
			tmp4 = setsuf(clist[i], 'i');
		na = 0;			/* start building arguments */
		av[na++] = NAMEcpp;
#ifdef  PORTAR 
		av[na++] = "-DPORTAR";		/* Archive version */
#endif

#ifdef	USR_INC
		av[na++] = USR_INC;	/* default search dirs for cpp */
#endif
#ifdef	INT16BIT
		av[na++] = "-Dmc68k16";		/* signify 16-bit version */
#else
		av[na++] = "-Dmc68k32";		/* signify 32-bit version */
#endif
#ifdef	BLIT
		if(mflag)	/* rob -- define MPX */
			av[na++] = "-DMPX";
#endif
		for (j = 0; j < np; j++)
			av[na++] = plist[j];
#ifdef	BLIT
		av[na++] = "-I/usr/jerq/include";
#endif
		if (truncflag)			/* pass -XT if truncating */
		    av[na++] = "-T";

		av[na++] = clist[i];
		av[na++] = exflag ? "-" : tmp4;
		for (j = 0; j < nlist[Xcp]; j++)
		    av[na++] = list[Xcp][j];
		av[na++] = 0;
		switch(callsys(cpp, av)){
		case 0:
			break;
#define	CLASS 27
		case CLASS:
			if(callsys("/lib/cpre", av)==0)
				break;
			/* fall through */
		default:
			exfail++;
			eflag++;
		}
		if (pflag || exfail) {
			cflag++;
			continue;
		}
		if (sflag)
			assource = tmp3 = setsuf(clist[i], 's');
		na = 0;
		av[na++] = NAMEccom;
		if (gflag)		/* must be first arg */
		    av[na++] = "-g";
		if (truncflag)		/* if truncating names */
		    av[na++] = "-XT";	/* pass flag */
		for (j = 0; j < nlist[Xc0]; j++)
		    av[na++] = list[Xc0][j];
		av[na++] = tmp4;
		av[na++] = oflag ? tmp5 : tmp3;
		if (proflag)
			av[na++] = "-XP";
		if (wflag)
			av[na++] = "-w";
		av[na] = 0;
		if (callsys(ccom, av)) {
			cflag++;
			eflag++;
			continue;
		}
		if (oflag) {
			na = 0;
			av[na++] = NAMEoptim;
			av[na++] = tmp5;
			av[na++] = tmp3;
			for (j = 0; j < nlist[Xc2]; j++)
			    av[na++] = list[Xc2][j];
			av[na] = 0;

			if (callsys(c2, av)) {
				unlink(tmp3);
				tmp3 = assource = tmp5;
			} else
				unlink(tmp5);
		}
		if (sflag)
			continue;
	assemble:
		cunlink(tmp1); cunlink(tmp2); cunlink(tmp4);
		na = 0;
		av[na++] = NAMEas;
		for (j = 0; j < nlist[Xas]; j++)
		    av[na++] = list[Xas][j];
		av[na++] = "-o";
		av[na++] = setsuf(clist[i], 'o');
		if (Rflag)
			av[na++] = "-R";
		if (dflag)
			av[na++] = dflag;
		av[na++] = assource;
		av[na] = 0;
		if (callsys(as, av) > 0) {
			cflag++;
			eflag++;
			continue;
		}
	}
nocom:
	if (cflag==0 && nl!=0) {
		i = 0;
		av[0] = NAMEld; na=1;
#ifdef	BLIT
		if(!mflag){
			av[na++] = "-b";
			av[na++] = "256";
		}
		if(!rflag || mflag)
#endif
			av[na++] = crt0;
		if (outfile) {
			av[na++] = "-o";
			av[na++] = outfile;
		}
		if ( proflag )
			av[na++] = strspl("-L",strspl(LIBDIR,"/libp"));
		for (j = 0; j < nlist[Xld]; j++)
		    av[na++] = list[Xld][j];

		if(gflag)		/* sdb needs "libg.a" */
			llist[nl++] = "-lg";

		while (i < nl)
			av[na++] = llist[i++];
#ifdef	BLIT
		if(rflag || mflag){
			av[na++] = "-R";
			av[na++] = "-M";
		}
		if(mflag)
			av[na++]= "-d";
		if(jflag)
			av[na++] = "/usr/jerq/lib/libsys.a";
		if(mflag)
			av[na++] = "/usr/jerq/lib/libmj.a";
		av[na++] = "/usr/jerq/lib/libj.a";
#endif
		av[na++] = "-lc";
		av[na++] = 0;
		eflag |= callsys(ld, av);
		if (nc==1 && nxo==1 && eflag==0)
			unlink(setsuf(clist[0], 'o'));
	}
	dexit();
}

idexit()
{

	eflag = 100;
	dexit();
}

dexit()
{

	if (!pflag) {
		cunlink(tmp1);
		cunlink(tmp2);
		if (sflag==0)
			cunlink(tmp3);
		cunlink(tmp4);
		cunlink(tmp5);
	}
	exit(eflag);
}

error(s, x)
	char *s, *x;
{
	FILE *diag = exflag ? stderr : stdout;

	fprintf(diag, "%scc:  ", prefix);
	fprintf(diag, s, x);
	putc('\n', diag);
	exfail++;
	cflag++;
	eflag++;
}

getsuf(as)
char as[];
{
	register int c;
	register char *s;
	register int t;

	s = as;
	c = 0;
	while (t = *s++)
		if (t=='/')
			c = 0;
		else
			c++;
	s -= 3;
	if (c <= DIRSIZ && c > 2 && *s++ == '.')
		return (*s);
	return (0);
}

char *
setsuf(as, ch)
	char *as;
{
	register char *s, *s1;

	s = s1 = savestr(as);
	while (*s)
		if (*s++ == '/')
			s1 = s;
	s[-1] = ch;
	return (s1);
}

callsys(f, v)
	char *f, **v;
{
	int t, status;
	char	**p;

	if (debug) {
		printf("callsys %s:", f);
		for (p=v; *p != NULL; p++)
			printf(" '%s'", *p);
		printf(" .\n");
		return(0);
	}

	t = fork();
	if (t == -1) {
		printf("No more processes\n");
		return (100);
	}
	if (t == 0) {
		execv(f, v);
		printf("Can't find %s\n", f);
		fflush(stdout);
		_exit(100);
	}
	while (t != wait(&status))
		;
	if ((t=(status&0377)) != 0 && t!=14) {
		if (t!=2) {
			printf("Fatal error in %s\n", f);
			eflag = 8;
		}
		dexit();
	}
	return ((status>>8) & 0377);
}

nodup(l, os)
	char **l, *os;
{
	register char *t, *s;
	register int c;

	s = os;
	if (getsuf(s) != 'o')
		return (1);
	while (t = *l++) {
		while (c = *s++)
			if (c != *t++)
				break;
		if (*t==0 && c==0)
			return (0);
		s = os;
	}
	return (1);
}

#define	NSAVETAB	1024
char	*savetab;
int	saveleft;

char *
savestr(cp)
	register char *cp;
{
	register int len;

	len = strlen(cp) + 1;
	if (len > saveleft) {
		saveleft = NSAVETAB;
		if (len > saveleft)
			saveleft = len;
		savetab = (char *)malloc(saveleft);
		if (savetab == 0) {
			fprintf(stderr, "ran out of memory (savestr)\n");
			exit(1);
		}
	}
	strncpy(savetab, cp, len);
	cp = savetab;
	savetab += len;
	saveleft -= len;
	return (cp);
}

char *
strspl(left, right)
	char *left, *right;
{
	char buf[BUFSIZ];

	strcpy(buf, left);
	strcat(buf, right);
	return (savestr(buf));
}

char *
getpref(s)				/* get prefix of "cc" */
char * s;				/* cc command name */
{
    static char prefix[15];		/* enough room for prefix and \0 */
    char * last = s + strlen(s) - 1;	/* point at last char */
    char * cp;				/* used later */

    prefix[0] = '\0';			/* initialize prefix static */

/* verify we have "cc" */

    if (*last == 'c' && *--last == 'c')
    {
	cp = last - 1;			/* point at char. preceding cc */
	while (cp >= s && *cp != '/')	/* back up to / or beginning */
	    cp--;
	
	cp++;				/* compensate for going too far */
	(void) strncpy(prefix,cp,last-cp); /* copy prefix string */
    }
    return(prefix);			/* return prefix */
}

/* make absolute program name from path, prefix, and base program name */

char *
makename(path,prefix,name)
char * path;				/* initial path name */
char * prefix;				/* string to prepend to name */
char * name;				/* command name, as in "ccom" */
{
    static buf[BUFSIZ];		/* for building name */

    (void) strcpy(buf,path);		/* concatenate pieces */
    (void) strcat(buf,"/");
    (void) strcat(buf,prefix);
    (void) strcat(buf,name);
    return( savestr(buf) );		/* return completed string */
}

getXpass(c, opt)
char	c, *opt;
{
	switch (c) {
	default:
		error("Unrecognized pass name: '%s%c'", opt, c);
		return(-1);
	case '0':
	case '1':
	case '2':
		return(c - '0');
	case 'p':
		return(Xcp);
	case 'a':
		return(Xas);
	case 'l':
		return(Xld);
	}
}
