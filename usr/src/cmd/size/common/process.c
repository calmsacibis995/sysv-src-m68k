/*	@(#)process.c	2.1		*/
/* UNIX HEADER */
#include	<stdio.h>
#include	<ar.h>

/* COMMON SGS HEADERS */
#include	"filehdr.h"
#include	"scnhdr.h"
#include	"ldfcn.h"
#include	"sgs.h"

/* SIZE HEADERS */
#include	"defs.h"
#include	"process.h"

/* STATIC VARIABLE USED */
static char	*prtotal[3] = {
			"\n\nSize of %s:  0x%lx\n",
			"\n\nSize of %s:  0%lo\n",
			"\n\nSize of %s:  %ld\n"
		};


    /*  process(filename)
     *
     *  prints out the sum total of section sizes along with the size
     *  information (size, physical and virtual addresses) for each section
     *  in the object file
     *  uses static format strings to do the printing (see process.h).
     *
     *  calls:
     *      - findtotal( ) to find the sum total of section sizes
     *      - error(file, string) if the object file is somehow messed up
     *
     *  simply returns
     */


process(filename)

char	*filename;

{
    /* UNIX FUNCTIONS CALLED */
    extern		printf( );

    /* COMMON OBJECT FILE ACCESS ROUTINE CALLED */
    extern int		ldshread( );

    /* SIZE FUNCTIONS CALLED */
    extern long		findtotal( );
    extern		error( );

    /* EXTERNAL VARIABLES USED */

    extern int		numbase;
    extern LDFILE	*ldptr;

    /* LOCAL VARIABLES */
    long		size;
    unsigned short	section;
    SCNHDR		secthead;
    ARCHDR		arhead;
    extern int		ldahread( );
    long		ssize[10];
    char		*sname[10];
    long		sum = 0L;

    if ((size = findtotal( )) == TROUBLE) {
	error(filename, "cannot compute total size\n");
	return;
    }

#ifndef UNIX
    printf(prtotal[numbase], filename, size);
    printf(prhead);
#else
	if (ISARCHIVE(TYPE(ldptr))) {
		if (ldahread(ldptr, &arhead) == SUCCESS) {
			printf("%s:", arhead.ar_name);
		} else {
			printf("%s: ", filename);
		}
	}
#endif

    for (section = 1; section <= HEADER(ldptr).f_nscns; ++section) {
	if (ldshread(ldptr, section, &secthead) != SUCCESS) {
	    error(filename, "cannot read section header");
	    return;
	}
#ifdef UNIX
	printf(prusect[numbase], secthead.s_size);
	if (HEADER(ldptr).f_nscns > 3)
	    printf("\(%s\)", secthead.s_name);
	if (HEADER(ldptr).f_nscns != section)
	    printf(" + ");
	sum += secthead.s_size;
#else
	printf(prsect[numbase], secthead.s_name, secthead.s_size,
	       secthead.s_paddr, secthead.s_vaddr);
#endif

	}
#ifdef UNIX
	printf(prusum[numbase], sum);
    return;
#endif
}
