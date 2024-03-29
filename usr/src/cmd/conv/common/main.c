/*	@(#)main.c	2.1		*/
static char ID[] = "@(#) main.c:	2.1 6/2/83";

#include	<stdio.h>

#include	"conv.h"

char	*filev[MAXFILES];
char	curfile[30];
int	errlvl;

main(argc, argv)

int	argc;
char	**argv;
{

	/*
	 * conv -t target file ...
	 *
	 *	converts the common object file "file"
	 *	from one produced on some machine to that produced
	 *	on "target" 
	 */

	extern		fprintf( ),
			exit( ),
			getargs( ),
			convert( );

	int		filec,		/* number of <file> arguments */
			i;

	/*
	 * process command line
	 */

	if ( (filec = getargs(argc,argv)) <= 0 )  {
		fprintf(stderr,
		"Usage: conv [-s] [-] [-p|-o|-a|-b] [-t m68k|pdp|vax|n3b|m32|ibm|i80] files\n");
		exit(1);
		}


	for ( i=0; i < filec; ++i ) {
		strcpy(curfile, sname(filev[i]));
		convert( filev[i] );
		}

	exit(errlvl);
}
