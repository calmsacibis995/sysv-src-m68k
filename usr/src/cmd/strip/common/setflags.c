/*	@(#)setflags.c	2.1		*/
/* UNIX HEADER */
#include	<stdio.h>

/* SGS SPECIFIC HEADER */
#include	"sgs.h"

/* STRIP HEADER */
#include	"defs.h"

/* EXTERNAL VARIABLES DEFINED */
int	lflag = OFF;
int	xflag = OFF;
int	rflag = OFF;
int	sflag = OFF;
#ifdef m68k
int	fflag = OFF;
#endif


    /*  setflags(flagc, flagv)
     *
     *  scans the command line for flags (arguments beginning with a '-')
     *  sets the appropriate flags and removes the flag arguments from the
     *  command line
     *
     *  defines:
     *      - lflag = OFF by default
     *		    = ON if -l appears in the command line:  strip line nos only
     *      - xflag = OFF by default
     *		    = ON if -x appears in command line:  strip local symbols and
     *		      line numbers only
     *      - rflag = OFF by default
     *		= ON if -r appears in command line: strip all but static
     *		  externs, .bf, and .ef
     *	    - fflag = OFF by default
     *		= ON if -f appears in the command line, same as xflag plus
     *		  strip relocation information for sections not of type
     *		  type copy, for DMERT only
     *
     *  prints an error message if any unknown flag shows up
     *
     *  returns the number of arguments remaining in argv (they are repositioned
     *  in filev and are presumed to be object files).
     */


int
setflags(flagc, flagv)

int	flagc;
char	**flagv;

{
    /* UNIX FUNCTIONS CALLED */
    extern	fprintf( );

    /* EXTERNAL VARIABLES USED */
    extern int		lflag;
    extern int		xflag;
    extern int		rflag;
    extern int		sflag;
#if m68k && !UNIX
    extern int		fflag;
#endif

    /* LOCAL VARIABLES */
    int		filec;
    char	**filev;


    for (filec = 0, filev = flagv; flagc > 0; --flagc, ++flagv) {
	if (**flagv == '-') {
	    while(*++*flagv != '\0') {
		switch(**flagv) {
		    case 'l':
			if (xflag == OFF) {
			    lflag = ON;
			    rflag = OFF;
			}
			break;

		    case 'x':
			xflag = ON;
			lflag = OFF;
			rflag = OFF;
			break;

		    case 'r':
			xflag = OFF;
			lflag = OFF;
			rflag = ON;
			break;
#if vax || !UNIX
		    case 's':
			xflag = OFF;
			lflag = OFF;
			rflag = OFF;
			sflag = ON;
			break;
#endif

#if m68k && !UNIX
		    case 'f':
			fflag = ON;
			rflag = ON;
			break;
#endif
		    case 'V':
			fprintf(stderr,"%s: strip -%s\n",SGSNAME,RELEASE);
			break;

		    default:
			fprintf(stderr,
				"%sstrip:  unknown option \"%c\" ignored\n",
				SGS, **flagv);
			break;
		}
	    }
	} else {
	    *filev++ = *flagv;
	    ++filec;
	}
    }

    return(filec);
}
/*
 *	static char ID[] = "@(#) setflags.c: 1.4 1/19/83";
 */
