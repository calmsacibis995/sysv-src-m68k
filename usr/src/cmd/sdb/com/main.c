	/*	@(#)main.c	2.1		*/

/*
 *	UNIX debugger
 */

#include	"head.h"
#include	<sys/stat.h>
#include <fcntl.h>

extern INT		wtflag;
extern INT		mkfault;
extern INT		executing;
extern ADDR		sigint;
extern ADDR		sigqit;
int	fpe();

main(argc, argv)
REG STRING	*argv;
REG INT		argc;
{
	register char *p;
	register int xflag;
	register char *name;
	struct stat stbuf;

	/* use unbuffered output */
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	name = argv[0];
	argc--, argv++;
	while (argc>0 && argv[0][0] == '-' && argv[0][1] != '\0')
	{   
		for (p = &argv[0][1]; *p; p++)
		{
			switch (*p)
			{
			case 's':
				sflag++;
				break;
			case 'x':
				xflag++;
				break;
			case 'w':
				wtflag = 2;  /* allow write to files,ISP */
				break;
			case 'd':
#if DEBUG > 1
				mjmdflag++;
#else
				fprintf(FPRT1,
				"Warn: Sdb not compiled with DEBUG #define;\n");
#endif
				break;
			case 'W':
				Wflag++;
				break;
			default:
				fprintf(FPRT1, "%s: unknown flag %c\n",
					name, *p);
				exit(4);
			}
		}
		argc--, argv++;
	}

	if (argc>0) symfil = argv[0];
	if (argc>1) corfil = argv[1];

	/* put directory name in beginning of filework (if directory given) */
	fwp = filework;
	if (argc > 2) {
		for (p = argv[2]; *p; *fwp++ = *p++)
			;
		*fwp++ = '/';
	}
	argcount=argc;

	if (symfil[0] == '-')	/* name "-" ==> ignore a.out */
		fprintf(FPRT1, "Warning: `%s' does not exist\n", symfil);
	else if (stat(symfil, &stbuf) == -1) {
		fprintf(FPRT1, "`%s' does not exist\n", symfil);
		exit(4);
	}
#ifdef FLEXNAMES
	symfilsize = stbuf.st_size;	/* save size of symfil in bytes */
#endif

	/* Check that core file is newer than symbol file */
	symtime = stbuf.st_mtime;
	if (stat(corfil, &stbuf) != -1) {
		if (symtime > stbuf.st_mtime)
			fprintf(FPRT1, "Warning: `%s' newer than `%s'\n",
				symfil, corfil);
	}

	/* initialize sdb data structures */
#if (m68k && VARU)		/* calculate ubase if no memory mgt */
	if(setuaddr() == 0L) {
		error("Address of u cannot be found in file /unix");
		sdbexit();
	}
#endif
	setsym(); setcor();
	initfp();

	/* set up variables for user */
	ioctl(FDTTY, TCGETA, &sdbttym);	   /* save initial state of terminal */
	sdbttyf = fcntl(FDIN, F_GETFL, 0); /* save initial status bits */
	/* if user was ignoring interrupts, fine; else trap to fault() */
	if ((sigint = (ADDR) signal(SIGINT,SIG_IGN)) != (ADDR) SIG_IGN)
	{
		sigint= (ADDR) fault;
		signal(SIGINT,fault);
	}
	sigqit= (ADDR) signal(SIGQUIT,SIG_IGN);	/* ignore "quit" signals */
	signal(SIGILL, fpe);		/* fpe() handles illegal instructions */

	sdbenter(xflag);

	setjmp(env);

	sdbtty();	/* save user tty modes and restore sdb tty modes */

#if DEBUG
	if (debug) fprintf(FPRT2, "Sdb restarted\n");
#endif
	if (executing)
	{
		delbp();
		executing = FALSE;
	}

	/* print prompt (*), read user command, and execute it */
	for (;;) {
		mkfault = 0;
		printf("*");
		if (decode(readline(inputfile)) == 1) {
			fprintf(FPRT1, "Error; Try again\n");
			continue;
		}

#if DEBUG
	if (debug) {
		fprintf(FPRT2, "cmd %c:\n", cmd);
		fprintf(FPRT2, "%s:%s\n", proc, var);
		fprintf(FPRT2, "args-%s;re-%s;integ-%d;integ2-%d\n",
				args, re, integ, integ2);
		fprintf(FPRT2, "scallf-%d;reflg-%d\n", scallf, reflag);
		fprintf(FPRT2, "colonflag-%d;ncolonflag-%d\n\n",
				colonflag, ncolonflag);
 	}
#endif

		docommand();
	}
}


fault(a)
int a;
{
	signal(a,fault);	/* cancel pending signal "a" */
	mkfault++;
	if(pid)
		delbp();
	printf("\n");
	fflush(stdout);
	setbuf(stdout, NULL);
	longjmp(env, 0);
}

fpe() {
	signal(SIGILL, fpe);	/* cancel pending signal SIGILL */
	error("Illegal floating constant");
	longjmp(env, 0);
}

