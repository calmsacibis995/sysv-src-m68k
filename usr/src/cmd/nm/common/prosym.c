/*	@(#)prosym.c	2.1		*/
/* UNIX HEADER */
#include	<stdio.h>

/* COMMON SGS HEADERS */
#include	<filehdr.h>
#include	<syms.h>
#include	<ldfcn.h>

/* NAMELIST HEADERS */
#include	"defs.h"
#include	"prosym.h"


    /*  prosym(section)
     *
     *  finds the symbol table (ldtbseek( ))
     *  prints a heading
     *  prints out each symbol table entry
     *  the way that the entry is printed depends mostly on its storage class
     *
     *  calls:
     *      - decotype(outfile, &symbol, &aux) to print the type field
     *
     *  prints:
     *      - the symbol name, value, storage class, tv and
     *        (after decotype prints the type) the size, line number and section
     *        name of each symbol
     *      - uses format strings defined in prosym.h
     *
     *  returns SUCCESS or FAILURE
     *  FAILURE is caused by a messed up symbol table
     */

int
prosym(section)

char	(*section)[8];


{
    /* UNIX FUNCTIONS CALLED */
    extern		fprintf( );
    extern int		strncmp( );

    /* COMMON OBJECT FILE ACCESS ROUTINES CALLED */
    extern int		ldtbseek( );
#ifdef m68k
    extern char		*ldgetname();
#endif

    /* NAMELIST FUNCTIONS CALLED */
    extern int		decotype( );

    /* EXTERNAL VARIABLES USED */
    extern LDFILE	*ldptr;
    extern FILE		*formout,
			*sortout;
    extern int		numbase;
    extern int		fullflag,
			hflag,
			uflag;
#ifdef m68k
    extern int          Tflag;
#endif

    /* LOCAL VARIABLES */
    FILE		*outfile;
    SYMENT		symbol;
    AUXENT		aux;
    long		symindx;
    int			tvaux;
    char		*istv = ".tv";
   
#ifdef m68k
    char		*ld_name;
    int			name_err = 0;
#endif


    if (ldtbseek(ldptr) != SUCCESS) {
	return(FAILURE);
    }

    if (uflag == OFF && hflag == OFF)
	fprintf(formout, prtitle[numbase]);

    for (symindx = 0L; symindx < HEADER(ldptr).f_nsyms; ++symindx) {
	outfile = formout;
	tvaux = 0;
	if (FREAD(&symbol, SYMESZ, 1, ldptr) != 1) {
	    return(FAILURE);
	}
	if ( symbol.n_numaux == 1 ) {
	    if (FREAD(&aux, AUXESZ, 1, ldptr) != 1) {
		return(FAILURE);
		}
	    ++symindx;
	    }

	if ( uflag == ON )  { 

#ifdef m68k
	    if ( symbol.n_scnum == 0 ) {
		if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
		{
		    ld_name = "<<bad symbol name>>";
		    name_err = 1;
		}
#ifdef FLEXNAMES
		if (Tflag && ld_name[20] != '\0')
		{
			ld_name[19] = '*';
			ld_name[20] = '\0';
		}
#endif
		fprintf(formout,"    %s\n", ld_name);
	    }
#else
            if ( symbol.n_scnum == 0) 
		fprintf(formout,"     %s\n", symbol.n_name);
#endif
	    continue;
	}

	/*
	 * Two special cases: C_FILE for the main
	 * symbol table entry and _TV for the aux entry
	 */
	if (symbol.n_sclass == C_FILE) {
	    if (symbol.n_numaux == 1) {
		fprintf(formout, prfile[numbase], aux.x_file.x_fname);
	    } else {
		fprintf(formout, prfile[numbase], "");
	    }
	    continue;
	}

	if ( strncmp(symbol.n_name, istv, 8) == 0 ) {
	    tvaux = 1;
	}


	/* the remaining storage classes: */
	switch(symbol.n_sclass) {
	    case C_AUTO:
	    case C_REG:
	    case C_MOS:
	    case C_ARG:
	    case C_MOU:
	    case C_MOE:
	    case C_REGPARM:
	    case C_FIELD:
		/* print value field as an offset */

#ifdef m68k
		if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
		{
			ld_name = "<<bad symbol name>>";
			name_err = 1;
		}
#ifdef FLEXNAMES
		if (Tflag && ld_name[20] != '\0')
		{
			ld_name[19] = '*';
			ld_name[20] = '\0';
		}
#endif
		fprintf(formout, proffset[numbase], ld_name,
			VALMASK(symbol.n_value),sclass[symbol.n_sclass]);
#else
		fprintf(formout, proffset[numbase], symbol.n_name,
			VALMASK(symbol.n_value), sclass[symbol.n_sclass]);
#endif
		break;

	    case C_EXT:
		/* external functions stay with their local symbols;
		     * other externals may get sorted (depends on what
		     * process( ) did with sortout).
		     */
		if (ISFCN(symbol.n_type)) {
			outfile = formout;
		} else {
			outfile = sortout;
		}
		/* print value as an address */

#ifdef m68k
		if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
		{
			ld_name = "<<bad symbol name>>";
			name_err = 1;
		}
#ifdef FLEXNAMES
		if (Tflag && ld_name[20] != '\0')
		{
			ld_name[19] = '*';
			ld_name[20] = '\0';
		}
#endif
		fprintf(outfile, praddress[numbase], ld_name,
		    VALMASK(symbol.n_value), sclass[symbol.n_sclass]);
#else
		fprintf(outfile, praddress[numbase], symbol.n_name,
		    VALMASK(symbol.n_value), sclass[symbol.n_sclass]);
#endif
		break;

	    case C_STAT:
	    case C_USTATIC:
	    case C_LABEL:
	    case C_BLOCK:
	    case C_FCN:
#ifdef m68k
	    case C_HIDDEN:
#endif
		/*
		 * if .text, .data or .bss and
		 * fullflag is OFF then don't print
		 */
		if (fullflag == OFF)
		    if ( symbol.n_sclass == C_STAT &&
			    symbol.n_type == T_NULL && (
			    strncmp(symbol.n_name, ".text", 8) == 0 ||
			    strncmp(symbol.n_name, ".data", 8) == 0 ||
			    strncmp(symbol.n_name, ".bss",  8) == 0) )
			continue;

		/* print value as an address */

#ifdef m68k
		if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
		{
			ld_name = "<<bad symbol name>>";
			name_err = 1;
		}
#ifdef FLEXNAMES
		if (Tflag && ld_name[20] != '\0')
		{
			ld_name[19] = '*';
			ld_name[20] = '\0';
		}
#endif
		fprintf(outfile, praddress[numbase], ld_name,
		    VALMASK(symbol.n_value), SCLASS(symbol.n_sclass));
#else
		fprintf(outfile, praddress[numbase], symbol.n_name,
		    VALMASK(symbol.n_value), SCLASS(symbol.n_sclass));
#endif
		break;

	    case C_NULL:
	    case C_EXTDEF:
	    case C_ULABEL:
	    case C_STRTAG:
	    case C_UNTAG:
	    case C_TPDEF:
	    case C_ENTAG:
	    case C_EOS:
		/* value field is meaningless; don't print it */

#ifdef m68k
		if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
		{
			ld_name = "<<bad symbol name>>";
			name_err = 1;
		}
#ifdef FLEXNAMES
		if (Tflag && ld_name[20] != '\0')
		{
			ld_name[19] = '*';
			ld_name[20] = '\0';
		}
#endif
		fprintf(formout, prnoval[numbase], ld_name,
		    SCLASS(symbol.n_sclass));
#else
		fprintf(formout, prnoval[numbase], symbol.n_name,
		    SCLASS(symbol.n_sclass));
#endif
		break;

	    default:

#ifdef m68k
		if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
		{
			ld_name = "<<bad symbol name>>";
			name_err = 1;
		}
#ifdef FLEXNAMES
		if (Tflag && ld_name[20] != '\0')
		{
			ld_name[19] = '*';
			ld_name[20] = '\0';
		}
#endif
		fprintf(formout, praddress[numbase], ld_name,
		    VALMASK(symbol.n_value), "??????");
#else
		fprintf(formout, praddress[numbase], symbol.n_name,
		    VALMASK(symbol.n_value), "??????");
#endif
		break;

	}	/* switch */

#ifdef TRVEC
	if ((symbol.n_numaux == 1) && (aux.x_sym.x_tvndx != 0)) {
	    fprintf(outfile, "|tv");
	} else {
	    fprintf(outfile, "|  ");
	}

#endif
	if (decotype(outfile, &symbol, &aux) == FAILURE) {
	    return(FAILURE);
	}

	if (tvaux == 1) {
	    fprintf(outfile, prsize[numbase], ((aux.x_tv.x_tvran[1]
			- aux.x_tv.x_tvran[0]) * sizeof(long)));
	    fprintf(outfile, "|     |\n");
	    continue;
	}

	if ( ISFCN(symbol.n_type) ) {
	    /* print function size information */
	    if ((symbol.n_numaux == 1) && (aux.x_sym.x_misc.x_fsize != 0L)) {
		fprintf(outfile, prfsize[numbase], aux.x_sym.x_misc.x_fsize);
	    } else {
		fprintf(outfile, prnofsize[numbase]);
	    }
	    fprintf(outfile, "|     ");
	} else {
	    /* print regular size information */
	    if ((symbol.n_numaux == 1) && (aux.x_sym.x_misc.x_lnsz.x_size != 0)) {
		fprintf(outfile, prsize[numbase], aux.x_sym.x_misc.x_lnsz.x_size);
	    } else {
		fprintf(outfile, prnosize[numbase]);
	    }

	    if ((symbol.n_numaux == 1) && (aux.x_sym.x_misc.x_lnsz.x_lnno != 0)) {
		fprintf(outfile, "|%5d", aux.x_sym.x_misc.x_lnsz.x_lnno);
	    } else {
		fprintf(outfile, "|     ");
	    }
	}

	if ((symbol.n_scnum > 0) && 
		(symbol.n_scnum <= HEADER(ldptr).f_nscns)) {
	    fprintf(outfile, "|%-.8s\n", section[symbol.n_scnum - 1 ]);
	} else {
	    fprintf(outfile, "|\n");
	}
    }	/* for */

#ifdef m68k
    if (name_err)
	return (FAILURE);
#endif
    return(SUCCESS);
}




    /*  proext(section)
     *
     *  is almost identical to prosym( ) except it only prints information
     *  about static and external symbols.  It also prints the source file
     *  associated with static symbols.
     */


int
proext(section)

char	(*section)[8];


{
    /* UNIX FUNCTION CALLED */
    extern		fprintf( );
    extern int		strncmp( );

    /* COMMON OBJECT FILE ACCESS ROUTINE CALLED */
    extern int		ldtbseek( );
#ifdef m68k
    extern char		*ldgetname();
#endif

    /* NAMELIST FUNCTION CALLED */
    extern int		decotype( );

    /* EXTERNAL VARIABLES USED */
    extern LDFILE	*ldptr;
    extern int		numbase;
    extern int		fullflag,
			hflag;
#ifdef m68k
    extern int          Tflag;
#endif

    /* LOCAL VARIABLES */
    extern FILE		*formout,
			*sortout;

    FILE		*outfile;
    SYMENT		symbol;
    AUXENT		aux,
			faux;
    long		symindx;
    int			tvaux = 0;
    char		*iftv = ".tv";
#ifdef m68k
    char		*ld_name;
    int			name_err = 0;
#endif


    if (ldtbseek(ldptr) != SUCCESS) {
	return(FAILURE);
    }

    if (hflag == OFF)
    fprintf(formout, pretitle[numbase]);

    faux.x_file.x_fname[0] = '\0';

    for (symindx = 0L; symindx < HEADER(ldptr).f_nsyms; ++symindx) {
	if (FREAD(&symbol, SYMESZ, 1, ldptr) != 1) {
	    return(FAILURE);
	}

	if ( symbol.n_numaux == 1 ) {
	    if ( FREAD(&aux, AUXESZ, 1, ldptr) != 1) {
		return(FAILURE);
	    }
	    ++symindx;
	}

	if (symbol.n_sclass == C_FILE)
		continue;

	if ( strncmp(symbol.n_name,iftv,8) == 0)
	    ++tvaux;

	if ((symbol.n_sclass == C_STAT) || (symbol.n_sclass == C_EXT)) {

	    /*
	     * if .text, .data or .bss and
	     * fullflag is OFF then don't print
	     */
	    if (fullflag == OFF)
		if ( symbol.n_sclass == C_STAT &&
			symbol.n_type == T_NULL && (
			strncmp(symbol.n_name, ".text", 8) == 0 ||
			strncmp(symbol.n_name, ".data", 8) == 0 ||
			strncmp(symbol.n_name, ".bss",  8) == 0) )
		    continue;

	    outfile = formout;
	    if (symbol.n_sclass == C_EXT) {
		/* external symbols may get sorted */
		outfile = sortout;
	    }

	    /* print all values as addresses */
#ifdef m68k
	    if ((ld_name = ldgetname(ldptr, &symbol)) == NULL)
	    {
		ld_name = "<<bad symbol name>>";
		name_err = 1;
	    }
#ifdef FLEXNAMES
	    if (Tflag && ld_name[20] != '\0')
	    {
		ld_name[19] = '*';
		ld_name[20] = '\0';
	    }
#endif
	    fprintf(outfile, praddress[numbase], ld_name,
		    VALMASK(symbol.n_value), sclass[symbol.n_sclass]);
#else
            fprintf(outfile, praddress[numbase], symbol.n_name,
		    VALMASK(symbol.n_value), sclass[symbol.n_sclass]);
#endif
#ifdef TRVEC
	    if ((symbol.n_numaux == 1) && (aux.x_sym.x_tvndx != 0)) {
		fprintf(outfile, "|tv");
	    } else {
		fprintf(outfile, "|  ");
	    }
#endif

	    if (decotype(outfile, &symbol, &aux) == FAILURE) {
		return(FAILURE);
	    }

	    if ( ISFCN(symbol.n_type) ) {
		/* print function size information */
		if ((symbol.n_numaux == 1) && (aux.x_sym.x_misc.x_fsize != 0L)) {
		    fprintf(outfile, prfsize[numbase], aux.x_sym.x_misc.x_fsize);
		} else {
		    fprintf(outfile, prnofsize[numbase]);
		}

	    } else {
		/* print regular size information */
		if (symbol.n_numaux == 1 && tvaux) {
		    fprintf(outfile, prsize[numbase], ((aux.x_tv.x_tvran[1] 			- aux.x_tv.x_tvran[0]) * sizeof(long)));
		} else {
		    if ((symbol.n_numaux == 1) && (aux.x_sym.x_misc.x_lnsz.x_size != 0)) {
			fprintf(outfile, prsize[numbase], aux.x_sym.x_misc.x_lnsz.x_size);
		    } else {
			fprintf(outfile, prnosize[numbase]);
		    }
		}
	    }

	    if ((symbol.n_numaux == 1) && (aux.x_sym.x_misc.x_lnsz.x_lnno != 0)
			&& (! tvaux)) {
		fprintf(outfile, "|%5d", aux.x_sym.x_misc.x_lnsz.x_lnno);
	    } else {
		fprintf(outfile, "|     ");
	    }

	    if ((symbol.n_scnum > 0) &&
		    (symbol.n_scnum <= HEADER(ldptr).f_nscns)) {
		fprintf(outfile, "|%-8.8s", section[symbol.n_scnum - 1 ]);
	    } else {
		fprintf(outfile, "|        ");
	    }

	    if (symbol.n_sclass == C_STAT) {
		fprintf(outfile, "|%.14s\n", faux.x_file.x_fname);
	    } else {
		fprintf(outfile, "|\n");
	    }
	}
    }

#ifdef m68k
    if (name_err)
	return (FAILURE);
#endif
    return(SUCCESS);
}

/*
 *	static char ID[] = "@(#) prosym.c: 1.6 6/1/83";
 */
