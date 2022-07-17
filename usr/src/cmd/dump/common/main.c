/*	@(#)main.c	2.1.1.3		*/
#include	<stdio.h>
#include	"filehdr.h"
#include	"ldfcn.h"
#include	"filedefs.h"
#ifdef m68k
#include	"dumpmap.h"
#endif
#include	"sgs.h"

char    *origname;

main(argc, argv) 

int	argc;
char	**argv;

{
    extern		fprintf( ),
			exit( ),
			cfree( );

    extern int		ldaclose( );

    extern int		setflags( );
    extern FILELIST	*openobject( );
    extern 		process( );

    FILELIST		*listhead,
			*listptr,
			*nextptr;


    if ((argc = setflags(--argc, ++argv)) == 0) {
	fprintf(stderr, "usage:  %sdump [flags] _f_i_l_e ...\n", SGS);
	exit(0);
    }

#ifdef vax
    origname = *argv;
#endif
    listhead = NULL;

    for (	; (argc > 0) && ((listhead = openobject(*argv)) == NULL);
	 --argc, ++argv);

    --argc;
    ++argv;

    for (listptr = listhead; argc > 0; --argc, ++argv) {
	if ((listptr->nextitem = openobject(*argv)) != NULL) {
	    listptr = listptr->nextitem;
	}
    }

    if (listhead != NULL) {
	process(listhead);

	for (listptr = listhead; listptr != NULL; listptr = nextptr) {
	    nextptr = listptr->nextitem;

	    ldaclose(listptr->ldptr);
	    cfree(listptr);
	}
    }

    exit(0);
}

/*
*     static char	ID[ ] = "@(#) main.c: 1.4 3/31/83";
*/
