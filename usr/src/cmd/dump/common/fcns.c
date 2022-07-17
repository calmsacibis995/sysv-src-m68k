/*	@(#)fcns.c	2.1		*/
#include	<stdio.h>
#include	<ar.h>
#include	"filehdr.h"
#include	"scnhdr.h"
#include	"reloc.h"
#include	"linenum.h"
#include	"syms.h"
#include	"ldfcn.h"
#include	"sgs.h"


int
ahread(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			ldahread( );

    extern char		*ctime( );
    extern int		vflag;
    ARCHDR		arhead;
    int			uid,
			gid;


#if PORTAR
    if (TYPE(ldptr) == (unsigned short) ARTYPE) {
#else
    if (TYPE(ldptr) == (unsigned short) ARMAG) {
#endif
	underline(filename);
	if (ldahread(ldptr, &arhead) == SUCCESS) {
#if !PORTAR
	    uid = (int) arhead.ar_uid & 0xffL;
	    gid = (int) arhead.ar_gid & 0xffL;
#endif

	    if (!vflag) {
#if PORTAR
		printf("\t%-16.16s  0x%08lx  %6d  %6d  0%06ho  0x%08lx\n\n",
		    arhead.ar_name, arhead.ar_date, uid, gid, arhead.ar_mode,
		    arhead.ar_size);
	    } else {
		printf("\t%-16.16s  %.20s  %6d  %6d  0%06ho  0x%08lx\n\n",
		    arhead.ar_name, (ctime(&arhead.ar_date) + 4), uid, gid, arhead.ar_mode,
		    arhead.ar_size);
	    }
#else
		printf("\t%-14.14s  0x%08lx  %3d  %3d  0%06ho  0x%08lx\n\n",
		    arhead.ar_name, arhead.ar_date, uid, gid, arhead.ar_mode,
		    arhead.ar_size);
	    } else {
		printf("\t%-14.14s  %.20s  %3d  %3d  0%06ho  0x%08lx\n\n",
		    arhead.ar_name, (ctime(&arhead.ar_date) + 4), uid, gid, arhead.ar_mode,
		    arhead.ar_size);
	    }
#endif
	} else {
	    fprintf(stderr, "%sdump:  cannot ldahread %s\n", SGS, filename);
	    return(FAILURE);
	}
    }

    return(SUCCESS);
}




fhread(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			ldfhread( );
    extern  char	*ctime( );
    extern  int	vflag;

    FILHDR		filehead;


    underline(filename);
    if (ldfhread(ldptr, &filehead) == SUCCESS) {

	if (!vflag) {
	   printf("\t0%06ho  %5hu  0x%8lx  0x%08lx  %10ld  0x%04hx  0x%04hx\n\n",
		filehead.f_magic, filehead.f_nscns, filehead.f_timdat, filehead.f_symptr,
		filehead.f_nsyms, filehead.f_opthdr, filehead.f_flags);
	} else {
		printf("0%06ho  %5hu  ", filehead.f_magic, filehead.f_nscns);
		printf("%.20s  0x%08lx  %6ld  0x%04hx  ",
				(ctime(&filehead.f_timdat) + 4), filehead.f_symptr,
				filehead.f_nsyms, filehead.f_opthdr); 
		if (HEADER(ldptr).f_flags & F_AR16WR){
		  printf("DEC16");
		} else { 
		  if (HEADER(ldptr).f_flags & F_AR32WR){
		    printf("DEC32");
		  } else {
		    printf("NON DEC");
		  }
		}
		if (HEADER(ldptr).f_flags & F_RELFLG){
		  printf("\n\t\t\t\t\t\t\t\t  RELFLG");
		} if (HEADER(ldptr).f_flags & F_EXEC){
		  printf("\n\t\t\t\t\t\t\t\t  EXEC");
		} if (HEADER(ldptr).f_flags & F_LNNO){
		  printf("\n\t\t\t\t\t\t\t\t  LNNO");
		} if (HEADER(ldptr).f_flags & F_LSYMS){
		  printf("\n\t\t\t\t\t\t\t\t   LSYMS");
		}if(HEADER(ldptr).f_flags & F_MINMAL){
		  printf("\n\t\t\t\t\t\t\t\t MINMAL");
		}if(HEADER(ldptr).f_flags & F_UPDATE){
		  printf("\n\t\t\t\t\t\t\t\t    UPDATE");
		}if(HEADER(ldptr).f_flags & F_SWABD){
		  printf("\n\t\t\t\t\t\t\t\t  SWABD");
		}if(HEADER(ldptr).f_flags & F_PATCH){
		   printf("\n\t\t\t\t\t\t\t\t  PATCH");
		}
	printf("\n\n");
	}  /* else */
	return(SUCCESS);
    }

    fprintf(stderr, "%sdump:  cannot ldfhread %s\n", SGS, filename);
    return(FAILURE);
}



int
ohseek(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			fread( );

    unsigned short	buffer;
    int			i,
			count;


    if ((HEADER(ldptr).f_opthdr) != 0) {
	underline(filename);
	if (ldohseek(ldptr) != SUCCESS) {
	    fprintf(stderr, "%sdump:  cannot ldohseek %s\n", SGS, filename);
	    return(FAILURE);
	}

	count = 1;
	printf("\t");
	for (i = HEADER(ldptr).f_opthdr / sizeof(short); i > 0; --i, ++count) {
	    if (FREAD(&buffer, sizeof(short), 1, ldptr) != 1) {
		fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
		return(FAILURE);
	    }
	    printf("%04hx  ", buffer);

	    if (count == 12) {
		printf("\n\t");
		count = 0;
	    }
	}

	if ((HEADER(ldptr).f_opthdr & 0x1) == 1) {
	    if (FREAD(&buffer, sizeof(char), 1, ldptr) != 1) {
		fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
	    }
	    printf("%04hx", buffer);
	}
	printf("\n\n");
    }

    return(SUCCESS);
}




int
shread(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			ldshread( ),
			ldnshread( );

    extern char		*name;
    extern  int		vflag;
    extern long		dstart,
			dstop;

    SCNHDR		secthead;
    unsigned short	first,
			last;
    static  char	*forms[17] = {
			"                  REG                     ",
			"                  DSECT                   ",
			"                  NOLOAD                  ",
			"					   ",
			"                  GROUP                   ",
			"					   ",
			"					   ",
			"					   ",
			"                  PAD                     ",
			"					   ",
			"					   ",
			"					   ",
			"					   ",
			"					   ",
			"					   ",
			"					   ",
			"                  COPY                    "
	    };



    underline(filename);
    first = 0;
    last = 0;

    if (name != NULL) {
	if (ldnshread(ldptr, name, &secthead) == SUCCESS) {
	    printf("\t%-8.8s  0x%08lx  0x%08lx  0x%08lx  0x%08lx  0x%08lx\n",
		secthead.s_name, secthead.s_paddr, secthead.s_vaddr,
		secthead.s_scnptr, secthead.s_relptr, secthead.s_lnnoptr);
	    if (!vflag) {
		printf("\t          0x%08lx              0x%08lx    %5hu       %5hu\n\n",
		   secthead.s_flags,secthead.s_size,secthead.s_nreloc,secthead.s_nlnno);
	    } else {
		printf("%s",forms[secthead.s_flags]);
		printf("0x%08lx    %5hu       %5hu\n\n", secthead.s_size, 
		    secthead.s_nreloc, secthead.s_nlnno);
	    }

	} else {
	    fprintf(stderr,"section:  %s    not in file:  %s\n\n", name, filename);
	}
    } else {
	first = 1;
	last = HEADER(ldptr).f_nscns;
    }

    if (dstart != 0L) {
	first = (unsigned short) dstart;
	last = first;
    }

    if (dstop != 0) {
	last = (unsigned short) dstop;
	if (dstart == 0L) {
	    first = 1;
	}
    }

    if (first != 0) {
	for (	; first <= last;  ++first) {
	    if (ldshread(ldptr, first, &secthead) != SUCCESS) {
		fprintf(stderr, "%sdump:  cannot ldshread section %d\n", SGS, first);
		return(FAILURE);
	    }
	    printf("\t%-8.8s  0x%08lx  0x%08lx  0x%08lx  0x%08lx  0x%08lx\n",
		secthead.s_name, secthead.s_paddr, secthead.s_vaddr,
		secthead.s_scnptr, secthead.s_relptr, secthead.s_lnnoptr);
	    if (!vflag) {
		    printf("\t          0x%08lx              0x%08lx    %5hu       %5hu\n\n",
		    secthead.s_flags,secthead.s_size,secthead.s_nreloc,secthead.s_nlnno);
	    } else {
		printf("%s",forms[secthead.s_flags]);
		printf("0x%08lx     %5hu     %5hu\n\n", secthead.s_size, 
		    secthead.s_nreloc, secthead.s_nlnno);
	    }

	}
    }

    return(SUCCESS);
}



int
sseek(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			fread( );

    extern int		ldshread( ),
			ldnshread( ),
			ldnsseek( ),
			ldsseek( );

    extern char		*name;
    extern long		dstart,
			dstop;

    SCNHDR		secthead;
    unsigned short	first,
			last,
			buffer;
    long		i;
    int			count;


    underline(filename);
    printf("\n");
    first = 0;
    last = 0;

    if (name != NULL) {
	if (ldnshread(ldptr, name, &secthead) == SUCCESS) {
	    underline(name);
	    if (secthead.s_scnptr != 0L) {
		if (ldnsseek(ldptr, secthead.s_name) != SUCCESS) {
		    fprintf(stderr, "%sdump:  cannot ldnsseek section %s\n", SGS,
			    secthead.s_name);
		    return(FAILURE);
		}

		count = 1;
		printf("\t");
		for (i = secthead.s_size/sizeof(buffer); i > 0; --i, ++count) {
		    if (FREAD(&buffer, sizeof(short), 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    printf("%04hx  ", buffer);

		    if (count == 12) {
			printf("\n\t");
			count = 0;
		    }
		}

		if ((secthead.s_size & 0x1L) == 1L) {
		    if (FREAD(&buffer, 1, 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    printf("%04hx", buffer);
		}
		printf("\n");
	    }
	    printf("\n");
	} else {
	    fprintf(stderr,"section:  %s    not in file:  %s\n\n", name, filename);
	}
    } else {
	first = 1;
	last = HEADER(ldptr).f_nscns;
    }

    if (dstart != 0L) {
	first = (unsigned short) dstart;
	last = first;
    }

    if (dstop != 0) {
	last = (unsigned short) dstop;
	if (dstart == 0L) {
	    first = 1;
	}
    }

    if (first != 0) {
	for (	; first <= last;  ++first) {
	    if (ldshread(ldptr, first, &secthead) != SUCCESS) {
		fprintf(stderr, "%sdump:  cannot ldshread section %d\n", SGS, first);
		return(FAILURE);
	    }
	    underline(secthead.s_name);
	    if (secthead.s_scnptr != 0L) {
		if (ldsseek(ldptr, first) != SUCCESS) {
		    fprintf(stderr, "%sdump:  cannot ldsseek section %d\n", SGS,
			    first);
		    return(FAILURE);
		}

		count = 1;
		printf("\t");
		for (i = secthead.s_size/sizeof(buffer); i > 0; --i, ++count) {
		    if (FREAD(&buffer, sizeof(short), 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    printf("%04hx  ", buffer);

		    if (count == 12) {
			printf("\n\t");
			count = 0;
		    }
		}

		if ((secthead.s_size & 0x1L) == 1L) {
		    if (FREAD(&buffer, 1, 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    printf("%04hx", buffer);
		}
		printf("\n");
	    }
	    printf("\n");
	}
    }

    return(SUCCESS);
}




int
rseek(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			ldshread( ),
			ldnshread( ),
			ldtbread( ),
			ldnrseek( ),
			ldrseek( );

    extern char		*name;
    extern char		*origname;
    extern long		dstart,
			dstop;
    extern int		vflag;
    static  char	*rtype[15] = {
			"ABS",
			"DIR16",
			"REL16",
			"IND16",
			"DIR24",
			"REL24",
			"DIR32",
			"OFF8",
			"OFF16",
			"SEG12",
			"DIR32S",
			"AUX",
			"OPT16",
			"IND24",
			"IND32"
	     };


    SCNHDR		secthead;
    RELOC		relent;
    SYMENT		symbol;
    LDFILE		*ldptr2;
    unsigned short	first,
			last;
    int			i;


    underline(filename);
    printf("\n");
    first = 0;
    last = 0;

    if (vflag) {
	if ((ldptr2 = ldaopen(origname, ldptr)) == NULL) {
	    fprintf(stderr,"cannot open %s\n", filename);
	    return(FAILURE);
	}
    }
    if (name != NULL) {
	if (ldnshread(ldptr, name, &secthead) == SUCCESS) {
	    underline(secthead.s_name);
	    if (secthead.s_nreloc != 0) {
		if (ldnrseek(ldptr, name) != SUCCESS) {
		  fprintf(stderr, "%sdump:  cannot ldrseek section %s\n", SGS, name);
		  return(FAILURE);
		}

		for (i = secthead.s_nreloc; i > 0; --i) {
		    if (FREAD(&relent, RELSZ, 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    if (!vflag) {
			printf("\t0x%08lx  %10ld  %6d\n", relent.r_vaddr,
			    relent.r_symndx, relent.r_type);
		    } else {
			if (ldtbread(ldptr2, relent.r_symndx, &symbol) != SUCCESS) {
			    fprintf(stderr,"cannot ldtbread at %ld", relent.r_symndx);
			    return(FAILURE);
			}
			printf("\t0x08lx  %10ld  \n", relent.r_vaddr,
			    relent.r_symndx);
			if (relent.r_type >= 0 && relent.r_type <= 14)
			    printf("%s", rtype[relent.r_type]);
			printf("     %s\n", symbol.n_name);
		    }
		}
	   }
	   printf("\n");
	} else {
	    fprintf(stderr,"section:  %s    not in file:  %s\n\n", name, filename);
	}
    } else {
	first = 1;
	last = HEADER(ldptr).f_nscns;
    }

    if (dstart != 0L) {
	first = (unsigned short) dstart;
	last = first;
    }

    if (dstop != 0) {
	last = (unsigned short) dstop;
	if (dstart == 0L) {
	    first = 1;
	}
    }

    if (first != 0) {
	for (	; first <= last;  ++first) {
	    if (ldshread(ldptr, first, &secthead) != SUCCESS) {
		fprintf(stderr, "%sdump:  cannot ldshread section %d\n", SGS, first);
		return(FAILURE);
	    }
	    underline(secthead.s_name);
	    if (secthead.s_nreloc != 0) {
		if (ldrseek(ldptr, first) != SUCCESS) {
		  fprintf(stderr, "%sdump:  cannot ldrseek section %d\n", SGS,first);
		  return(FAILURE);
		}

		for (i = secthead.s_nreloc; i > 0; --i) {
		    if (FREAD(&relent, RELSZ, 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    if (!vflag) {
			printf("\t0x%08lx  %10ld  %6d\n", relent.r_vaddr,
			    relent.r_symndx, relent.r_type);
		    } else { 
			if (ldtbread(ldptr2, relent.r_symndx, &symbol) != SUCCESS) {
			    fprintf(stderr,"cannot ldtbread at %ld", relent.r_symndx);
			    return(FAILURE);
			}
			printf("\t0x%08lx  %10ld  ", relent.r_vaddr, 
			    relent.r_symndx);
			if (relent.r_type >= 0 && relent.r_type <= 14)
			    printf("%s", rtype[relent.r_type]);
			printf("     %s\n", symbol.n_name);
		    } 
		}
	    }
	    printf("\n");
	}
    }

    return(SUCCESS);
}




int
olseek(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		printf( ),
			fprintf( ),
			ldshread( ),
			ldnshread( ),
			ldnlseek( ),
			ldlseek( ),
			ldtbseek( ),
			ldtbread( );
    extern int		vflag;

    extern char		*name;
    extern char		*origname;
    extern long		dstart,
			dstop;

    SCNHDR		secthead;
    LINENO		linent;
    SYMENT		symbol;
    LDFILE		*ldptr2;

    unsigned short	first,
			last;
    int			i;


    underline(filename);
    printf("\n");
    first = 0;
    last = 0;

    if (vflag) {
	if ((ldptr2 = ldaopen(origname, ldptr)) == NULL) {
	    fprintf(stderr,"cannot open %s\n", filename);
	    return (FAILURE);
	}
    }

    if (name != NULL) {
	if (ldnshread(ldptr, name, &secthead) == SUCCESS) {
	    underline(name);
	    if (secthead.s_nlnno != 0) {
		if (ldnlseek(ldptr, name) != SUCCESS) {
		    fprintf(stderr, "%sdump:  cannot ldlseek section %s\n", SGS, name);
		    return(FAILURE);
		}

		for (i = secthead.s_nlnno; i > 0; --i) {
		    if (FREAD(&linent, LINESZ, 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    if (linent.l_lnno == 0) {
			if (!vflag) {
			    printf("\t %10ld   %5hu\n", linent.l_addr.l_symndx,
				linent.l_lnno);
			} else {
			    if (ldtbread(ldptr2, linent.l_addr.l_symndx, &symbol) != SUCCESS) {
				fprintf(stderr,"cannot ldtbread at %ld", linent.l_addr.l_symndx);
				return(FAILURE);
				}
				printf("\t %10ld   %5hu    %s\n",
				    linent.l_addr.l_symndx, linent.l_lnno, symbol.n_name);
			}
		    } else {
			printf("\t 0x%08lx   %5hu\n", linent.l_addr.l_paddr,
			    linent.l_lnno);
		    }
		}
	   }
	   printf("\n");
	} else {
	    fprintf(stderr,"section:  %s    not in file:  %s\n\n", name, filename);
	}
    } else {
	first = 1;
	last = HEADER(ldptr).f_nscns;
    }

    if (dstart != 0L) {
	first = (unsigned short) dstart;
	last = first;
    }

    if (dstop != 0) {
	last = (unsigned short) dstop;
	if (dstart == 0L) {
	    first = 1;
	}
    }

    if (first != 0) {
	for (	; first <= last;  ++first) {
	    if (ldshread(ldptr, first, &secthead) != SUCCESS) {
		fprintf(stderr, "%sdump:  cannot ldshread section %d\n", SGS, first);
		return(FAILURE);
	    }
	    underline(secthead.s_name);
	    if (secthead.s_nlnno != 0) {
		if (ldlseek(ldptr, first) != SUCCESS) {
		  fprintf(stderr, "%sdump:  cannot ldlseek section %d\n", SGS,first);
		  return(FAILURE);
		}

		for (i = secthead.s_nlnno; i > 0; --i) {
		    if (FREAD(&linent, LINESZ, 1, ldptr) != 1) {
			fprintf(stderr, "%sdump:  cannot FREAD %s\n", SGS, filename);
			return(FAILURE);
		    }
		    if (linent.l_lnno == 0) {
			if (!vflag) {
			    printf("\t %10ld   %5hu\n", linent.l_addr.l_symndx,
				linent.l_lnno);
			} else {
			    if (ldtbread(ldptr2,linent.l_addr.l_symndx, &symbol) != SUCCESS) {
				fprintf(stderr,"cannot ldtbread at %ld ", linent.l_addr.l_symndx);
				return(FAILURE);
			    }
			    printf("\t %10ld   %5hu    %s\n",
				linent.l_addr.l_symndx, linent.l_lnno, symbol.n_name);
			}
		    } else {
			printf("\t 0x%08lx   %5hu\n", linent.l_addr.l_paddr,
			    linent.l_lnno);
		    }
		}
	    }
	    printf("\n");
	}
    }

    return(SUCCESS);
}




int
tbread(ldptr, filename)

LDFILE	*ldptr;
char	*filename;

{
    extern int		fprintf( ),
			printf( ),
			sprintf( ),
			ldtbseek( ),
			strncmp( ),
			ldtbread( );
    extern long		ldtbindex( );
    extern  int		vflag;

    extern char		*name;
    extern long		tstart,
			tstop;

    SYMENT		symbol;
    AUXENT		aux;
    int			sclass,
			numaux;
    long		last,
			index;
    char		indstr[14];


    if ((last = HEADER(ldptr).f_nsyms) == 0) {
	fprintf(stderr,"%s:  no symbols\n", filename);
	printf("\n");
	return(SUCCESS);
    } 
    --last;

    underline(filename);

    if ((tstop == 0) && (tstart != 0)) {
	tstop = tstart;
    }

    if ((tstop != 0L) && (tstop < last)) {
	last = tstop;
    }

    if (ldtbseek(ldptr) != SUCCESS) {
	fprintf(stderr, "%sdump:  cannot ldtbseek %s\n", SGS, filename);
	return(FAILURE);
    }

    for (index = tstart; index <= last; ++index) {
	if (ldtbread(ldptr, index, &symbol) != SUCCESS) {
	    fprintf(stderr, "%sdump:  cannot ldtbread entry %ld\n", SGS, index);
	    return(FAILURE);
	}

    if (name != NULL) {
	if (strncmp(symbol.n_name, name, SYMNMLEN) != 0){
	    continue;
	}
    }
	sclass = (int) symbol.n_sclass & 0xff;
	numaux = (int) symbol.n_numaux & 0xff;
	sprintf(indstr, "[%ld]", index);
	if (!vflag) {
	    printf("%12.12s  %-8.8s  0x%08lx  %6hd  0x%04hx   0x%02x    %3d\n",
		indstr, symbol.n_name, symbol.n_value, symbol.n_scnum, symbol.n_type,
		symbol.n_sclass, symbol.n_numaux);
	} else {
		printf("%12.12s  %-8.8s  0x%08lx  %6hd", indstr, symbol.n_name,
		    symbol.n_value, symbol.n_scnum);
		det_type(symbol);
		det_sclass(symbol);
		printf("  %3d\n",symbol.n_numaux);
	}

	for (	; numaux > 0; --numaux, ++index) {
	    if (ldtbread(ldptr, index + 1, &aux) != SUCCESS) {
		fprintf(stderr, "%sdump:  cannot ldtbread entry %d\n", SGS, index + 1);
		return(FAILURE);
	    }

	    if (!vflag) {
		if ( ISFCN(symbol.n_type) ) {
		    printf("aux\t     %10ld    0x%08lx   0x%08lx  0x%08lx  %5hu\n",
			aux.x_sym.x_tagndx, aux.x_sym.x_misc.x_fsize, aux.x_sym.x_fcnary.x_fcn.x_lnnoptr,
			aux.x_sym.x_fcnary.x_fcn.x_endndx, aux.x_sym.x_tvndx);
		} else {
		    printf("aux\t     %10ld   0x%04hx  0x%04hx  0x%08lx  0x%08lx  %5hu\n",
			aux.x_sym.x_tagndx, aux.x_sym.x_misc.x_lnsz.x_lnno, aux.x_sym.x_misc.x_lnsz.x_size, aux.x_sym.x_fcnary.x_fcn.x_lnnoptr,
			aux.x_sym.x_fcnary.x_fcn.x_endndx, aux.x_sym.x_tvndx);
		}
	    } else {
		det_auxent(symbol, aux, ldptr);
	    }
	}
    }

    printf("\n");
    return(SUCCESS);
}




int
zread(ldptr, filename)

LDFILE		*ldptr;
char		*filename;

{
    extern int		printf( ),
			fprintf( ),
			strncmp( ),
			fread( ),
			ldtbseek( ),
			ldtbread( ),
			ldlread( ),
			ldlinit( ),
			ldlitem( );

    extern char		*zname;
    extern unsigned short	zstart,
				zstop;

    SYMENT		symbol;
    AUXENT		aux;
    LINENO		line;
    long		fcnindx,
			symindx;
    unsigned short	zitem;


    underline(filename);
    printf("\n");

    if (ldtbseek(ldptr) != SUCCESS) {
	fprintf(stderr, "%sdump:  cannot ldtbseek %s\n", SGS, filename);
	return(FAILURE);
    }

    symindx = 0L;
    fcnindx = 0L;
    while ((fcnindx == 0L) && (symindx < HEADER(ldptr).f_nsyms)) {
	if (ldtbread(ldptr, symindx, &symbol) != SUCCESS) {
	    fprintf(stderr, "%sdump:  cannot ldtbread entry %ld\n", SGS, symindx);
	    return(FAILURE);
	}

	if ((ISFCN(symbol.n_type)) && (strncmp(zname, symbol.n_name, SYMNMLEN) == 0)) {
	    fcnindx = symindx;
	} else if (symbol.n_numaux == 1) {
	    if (ISFCN(symbol.n_type) || ISTAG(symbol.n_type)) {
		if (FREAD(&aux, AUXESZ, 1, ldptr) != 1) {
		    fprintf(stderr, "%sdump:  cannot fread aux entry %ld\n", symindx + 1);
		    return(FAILURE);
		}

		symindx = (symindx < aux.x_sym.x_fcnary.x_fcn.x_endndx) ? aux.x_sym.x_fcnary.x_fcn.x_endndx : symindx + 2;
	    } else {
		symindx += 2;
	    }
	} else {
	    ++symindx;
	}
    }

    if (fcnindx == 0L) {
	fprintf(stderr,"no function \"%s\" in symbol table\n\n", zname);
	return(SUCCESS);
    }

    underline(zname);

    if ((zstart != 0) && (zstart >= zstop)) {
	if (ldlread(ldptr, fcnindx, zstart, &line) == SUCCESS) {
	    printf("\t 0x%08lx   %5hu\n", line.l_addr.l_paddr, line.l_lnno);
	} else {
	    fprintf(stderr,"\t no line number >= %5hu\n", zstart);
	}
    } else {
	if (ldlinit(ldptr, fcnindx) != SUCCESS) {
	    fprintf(stderr,"\t no line numbers for function\n");
	} else {
	    if ((zitem = zstart) == 0) {
		if (ldlitem(ldptr, zitem, &line) != SUCCESS) {
		    fprintf(stderr, "%sdump:  cannot ldlitem entry %hu\n", SGS, zitem);
		    return(FAILURE);
		}
		printf("\t %10ld   %5hu\n", line.l_addr.l_symndx, line.l_lnno);
		++zitem;
	    }

	    if (zstop == 0) {
		while (ldlitem(ldptr, zitem, &line) == SUCCESS) {
		    printf("\t 0x%08lx   %5hu\n", line.l_addr.l_paddr, line.l_lnno);
		    zitem = line.l_lnno + 1;
		}
	    } else {
		while ((zitem <= zstop) && (ldlitem(ldptr, zitem, &line) == SUCCESS)) {
		    printf("\t 0x%08lx   %5hu\n", line.l_addr.l_paddr, line.l_lnno);
		    zitem = line.l_lnno + 1;
		}
	    }
	}
    }

    printf("\n");
    return(SUCCESS);

}




int
underline(string)

char	*string;

{
    extern int		printf( );

    for (	; *string != '\0'; ++string) {
	printf("_%c", *string);
    }

    printf(":\n");

    return;
}


/*  det_type is used to find out what type will
    be printed.
*/

int
det_type(syment)

  SYMENT	syment;

{

	extern int	sprintf( ),
			fprintf( );

	static char	*typelist[16] = {
			"null   ",
			"arg    ",
			"char   ",
			"short  ",
			"int    ",
			"long   ",
			"float  ",
			"double ",
			"struct ",
			"union  ",
			"enum   ",
			"enmem  ",
			"Uchar  ",
			"Uint   ",
			"Ulong  "
	};

	unsigned short  type,
			dertype,
			dtype;
	char		*name;


    type = syment.n_type & N_BTMASK;
    name = typelist[type];
    dertype = syment.n_type;
    dtype = ((dertype & N_TMASK) >> N_BTSHFT);

	/* switch on the different cases of types */

	switch(dtype) {

	case DT_PTR:
		printf("  *%s", typelist[type]);
		break;

	case DT_FCN:
		printf(" ()%s", typelist[type]);
		break;

	case DT_ARY:
		printf(" []%s", typelist[type]);
		break;

	default:
		printf("   %s", typelist[type]);
		break;
		} /* end of switch */
return;
}


/*  det_sclass determines what type of storage 
 *  class to use.
 */



int
det_sclass(syment)

    SYMENT	syment;

{
#define SCLAS(x)	x <= C_FIELD ? sclas[(x+1)] : sclas[(x) - 80]

    extern int		printf( );

    static char		*sclas[24] = {
			"efcn  ",
			"null  ",
			"auto  ",
			"extern",
			"static",
			"reg   ",
			"extdef",
			"label ",
			"ulabel",
			"strmem",
			"argm't",
			"strtag",
			"unmem ",
			"untag ",
			"typdef", 
			"ustat ",
			"entag ",
			"enmem ",
			"regprm",
			"bitfld",
			"block ",
			"fcn   ",
			"endstr",
			"file  "
	};


	if ((syment.n_sclass >= (char) C_EFCN && syment.n_sclass <= (char) C_FIELD)
		|| (syment.n_sclass >= (char) C_BLOCK && syment.n_sclass <= (char) C_FILE)){
	    printf("  %s",SCLAS(syment.n_sclass));
	} else 
	if (syment.n_sclass == C_HIDDEN) {
		printf("  hidden");
	} else {
	    printf("  ???????");
	}
return;
}

/* det_auxent determines what kind of aux entry
 * should be printed for that symbol.
 */



int
det_auxent(syment, aux, ldptr)

SYMENT		syment;
AUXENT		aux;
LDFILE		*ldptr;

{
#define IS3B(x)		(x == N3BMAGIC || x == NTVMAGIC)
#define ISM80(x)	(x == M80MAGIC)

	extern int 		printf( ),
				strncmp( );

	int			tmp;
	/* Based on the sclass print a aux entry */
		tmp = 0;
		switch (syment.n_sclass) {
			case C_FILE:
				printf("aux[1]       %14s\n", aux.x_file.x_fname);
				break;
			case C_STAT:
			case C_HIDDEN:
				if (syment.n_type == T_NULL) {
				    printf("aux[3]       0x%08lx   0x%04hx  0x%04hx\n",
					aux.x_scn.x_scnlen, aux.x_scn.x_nreloc,
					aux.x_scn.x_nlinno);
				    break;
				}
			case C_EXT:
				if (syment.n_type == T_NULL &&
					!strncmp(_TV,syment.n_name,SYMNMLEN)){
				    printf("aux[2]       0x%08lx  0x%04hx  0x%04hx  0x%04hx\n",
					aux.x_tv.x_tvfill, aux.x_tv.x_tvlen,
					aux.x_tv.x_tvran[0], aux.x_tv.x_tvran[1]);
				    break;
				}
			default:
			    if (ISFCN(syment.n_type) && 
				(IS3B(HEADER(ldptr).f_magic) ||
				ISM80(HEADER(ldptr).f_magic) ) ) {
				    tmp = 4;
			    } else {
				tmp = 6;
			    }

			    if (ISARY(syment.n_type) ) 
				tmp += 1;
			    printf("aux[%d]    %10ld", tmp, aux.x_sym.x_tagndx);
					break;
				} /* switch */
		if ( tmp  > 0) {
		    if ( tmp <= 5) {
			printf("  0x%08lx", aux.x_sym.x_misc.x_fsize);
			if ( tmp == 4) {
			    printf("  0x%08lx  0x%08lx",
				aux.x_sym.x_fcnary.x_fcn.x_lnnoptr,
				aux.x_sym.x_fcnary.x_fcn.x_endndx);
			    printf("          %5hu\n", aux.x_sym.x_tvndx);
			} else {
			    printf("  0x%04hx  0x%04hx  0x%04hx  0x%04hx",
				aux.x_sym.x_fcnary.x_ary.x_dimen[0],
				aux.x_sym.x_fcnary.x_ary.x_dimen[1],
				aux.x_sym.x_fcnary.x_ary.x_dimen[2],
				aux.x_sym.x_fcnary.x_ary.x_dimen[3]);
			    printf("  %5hu\n", aux.x_sym.x_tvndx);
		    }
		} else {
			printf("  0x%04hx", aux.x_sym.x_misc.x_lnsz.x_lnno);
			printf("  0x%04hx", aux.x_sym.x_misc.x_lnsz.x_size);

			if(tmp == 6) {
			    printf("  0x%08lx  0x%08lx",
				aux.x_sym.x_fcnary.x_fcn.x_lnnoptr,
				aux.x_sym.x_fcnary.x_fcn.x_endndx);
			    printf("      %5hu\n", aux.x_sym.x_tvndx);
			} else {
			    printf("  0x%04hx  0x%04hx  0x%04hx  0x%04hx",
				aux.x_sym.x_fcnary.x_ary.x_dimen[0],
				aux.x_sym.x_fcnary.x_ary.x_dimen[1],
				aux.x_sym.x_fcnary.x_ary.x_dimen[2],
				aux.x_sym.x_fcnary.x_ary.x_dimen[3]);
			    printf("%5hu\n", aux.x_sym.x_tvndx);
			}
		}
	    }
return;
}
