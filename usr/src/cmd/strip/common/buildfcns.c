/*	@(#)buildfcns.c	2.1		*/
/* UNIX HEADER */
#include	<stdio.h>

/* COMMON SGS HEADERS */
#include	<filehdr.h>
#include	<scnhdr.h>
#include	<reloc.h>
#include	<syms.h>
#include	<ldfcn.h>
#include	<linenum.h>

/* STRIP HEADER */
#include	"defs.h"

/* SGS SPECIFIC HEADER */
#include	"sgs.h"

#ifdef m68k
#if !UNIX
long	rel_skipped;
long	reloc_ptrs[MAXSECTIONS];
#endif 

#if !AR16WR
static	TEMPENT	*basep = NULL;
static	TEMPENT	*oldptr = NULL;
static	TEMPENT	*entptr	= NULL;
#endif

long	start_reloc;
int	havelnums = 0;
#if FLEXNAMES
long	string_size = 0L;
#endif
#endif

    /*  buildfcns.c is a collection of functions that copy parts of an object
     *  file to strip's temporary file
     *
     *  copyopthdr( )
     *
     *  copies the optional header to the temporary file
     *
     *  calls:
     *      - docopy((long) HEADER(ldptr).f_opthdr) to do the copying
     *        after positioning ldptr at the start of the optional header
     *
     *  returns SUCCESS or FAILURE depending on the success of the copying
     */

#ifdef vax
unsigned  short  relno;
int    havelnums = 0;
#endif

int
cpyopthdr( )

{
    /* OBJECT FILE ACCESS ROUTINE CALLED */
    extern int		ldohseek( );

    /* STRIP FUNCTION CALLED */
    extern int		docopy( );

    /* EXTERNAL VARIABLE USED */
    extern LDFILE	*ldptr;
#ifdef vax
    extern int          havelnums;
#endif

    if (ldohseek(ldptr) == SUCCESS) {
	if (docopy((long) HEADER(ldptr).f_opthdr) == SUCCESS) {
	    return(SUCCESS);
	}
    }

    return(FAILURE);
}




    /*  cpyscnhdrs( )
     *
     *  copies each section header to the temporary file
     *  sets number of line numbers and line number pointer to zero
     *
     *  returns SUCCESS or FAILURE depending on the success of the copy
     */


int
cpyscnhdrs( )

{
    /* UNIX FUNCTIONS CALLED */
    extern int		fwrite( );

    /* COMMON ACCESS ROUTINE CALLED */
    extern int		ldshread( );

    /* EXTERNAL VARIABLES USED */
    extern LDFILE	*ldptr;
    extern FILE		*strp1;
     extern	int	sflag;
#ifdef vax 
     extern	unsigned short	relno;
#endif
     extern	int	havelnums;
#ifdef m68k
	extern	long	start_reloc;
	extern	int	fflag;
	extern	int	rflag;
	extern	int	lflag;
	extern	int	xflag;
#if !UNIX
	extern	long	rel_skipped;
	extern	long	reloc_ptrs[];
#endif
#endif

    /* LOCAL VARIABLES */
    SCNHDR		secthead;
    unsigned short	section;

#ifdef m68k
#if !UNIX
	if (fflag) {
		rel_skipped = 0L;
		for (section = 1; section <= HEADER(ldptr).f_nscns; ++section) {
			if (ldshread( ldptr, section, &secthead ) != SUCCESS)
				return( FAILURE );
			if (secthead.s_flags & STYP_COPY)
				reloc_ptrs[section - 1] = secthead.s_relptr - rel_skipped;
			else {
				reloc_ptrs[section - 1] = 0L;
				rel_skipped += (long) secthead.s_nreloc * RELSZ;
			}
		}
	}
#endif

	if (fflag || sflag || rflag || xflag || lflag)
		start_reloc = HEADER(ldptr).f_symptr;
	else
		start_reloc = 0L;

#endif
	for (section = 1; section <= HEADER(ldptr).f_nscns; ++section) {
		if (ldshread(ldptr, section, &secthead) != SUCCESS) {
			return(FAILURE);
		}

#ifdef m68k
		/*
		 * Pick up the earliest section's relocation or line numbers,
		 * so that a gap between the last section and the next
		 * information will be handled correctly, since the next strip
		 * routine will assume a properly seeked pointer to the 
		 * temp file "strp1"
		 */
		if ((secthead.s_relptr != 0L) && (secthead.s_relptr < start_reloc))
			start_reloc = secthead.s_relptr;
		if ((secthead.s_lnnoptr != 0L) && (secthead.s_lnnoptr < start_reloc))
			start_reloc = secthead.s_lnnoptr;

#if !UNIX
		if (fflag) {
			secthead.s_relptr = reloc_ptrs[section - 1];
			if (secthead.s_relptr == 0L)
				secthead.s_nreloc = 0;
		}
#endif
#endif
		if (!sflag) {
			secthead.s_lnnoptr = 0L;
			secthead.s_nlnno = 0;
		} else {
			if ( secthead.s_nlnno != 0 ) {
				havelnums = 1;
#ifdef m68k
#if !UNIX
				if (fflag)
					secthead.s_lnnoptr -= rel_skipped;
#endif
#else
			relno = secthead.s_nreloc;
#endif
			}
		}

		if (fwrite(&secthead, SCNHSZ, 1, strp1) != 1) {
	    	return(FAILURE);
		}
	} /* for loop */

	return(SUCCESS);
}




    /*  copyscns( )
     *
     *  copies the section contents to the temporary file
     *  keeps track of the file address that might be the start of the symbol
     *  table in a partially stripped file
     *
     *  calls:
     *      - docopy(secthead.s_size) to do the copying after positioning 
     *        ldptr at the start of a section's contents and after seeking
     *        to the section content file address in strp1 (to account for
     *        section content alignment in the object file)
     *
     *  returns the address to be used for the start of the symbol table
     *  in a partially stripped file -- if everything goes okay
     *  returns ERROR if anything went wrong
     */


long
copyscns( )

{
    /* UNIX FUNCTION CALLED */
    extern int		fseek( );
#ifdef m68k
    extern int		fwrite( );
#endif

    /* OBJECT FILE ACCESS ROUTINES CALLED */
    extern int		ldshread( ),
			ldsseek( );

    /* STRIP FUNCTION CALLED */
    extern int		docopy( );

    /* EXTERNAL VARIABLES USED */
    extern LDFILE	*ldptr;
    extern FILE		*strp1;
    extern int		sflag;
#ifdef m68k
    extern long		start_reloc;
#endif

    /* LOCAL VARIABLES */
    SCNHDR		secthead;
    unsigned short	section;
    long		fileaddr;
#ifdef m68k
    long		gap;
    char		buf[BUFSIZ];
    register char	*p;

    p = buf;
    while (p < (buf + BUFSIZ))
	*p++ = '\0';

#else
    extern	int	frstln;
    extern	int	sumln;
#endif
    fileaddr = (long) FILHSZ + HEADER(ldptr).f_opthdr +
	       SCNHSZ * HEADER(ldptr).f_nscns;

    for (section = 1; section <= HEADER(ldptr).f_nscns; ++section) {
	if (ldshread(ldptr, section, &secthead) != SUCCESS) {
	    return(ERROR);
	}

	if (secthead.s_scnptr != 0L) {
	    if (ldsseek(ldptr, section) != SUCCESS) {
		return(ERROR);
	    }

	    /* If loader directive BLOCK is used, have to seek to section
	     * address in strp1 to ensure that the section contents appear
	     * in the right place.  Note that the fileheader is not written
	     * to the temporary file so its size must be subtracted from the
	     * seek address. Under UNIX/RT, zeros must be written in the gap.
	     */

#if m68k && RT
	    while (fileaddr < secthead.s_scnptr) {
		gap = secthead.s_scnptr - fileaddr;
		if (gap > BUFSIZ)
		    gap = BUFSIZ;
		if (fwrite( buf, (int) gap, 1, strp1) != 1)
		    return( ERROR );
		fileaddr += gap;
	    }
#else
	   if (fseek(strp1,secthead.s_scnptr - FILHSZ, BEGINNING) !=
		OKFSEEK) {
		return (ERROR);
	   }
#endif


	    if (docopy(secthead.s_size) != SUCCESS) {
		return(ERROR);
	    }
	    fileaddr = secthead.s_scnptr + secthead.s_size;
	}
    }

#ifdef m68k
	/*
	 * If there is a hole in the object file between the last byte of
	 * sections contents and whatever comes next, adjust "fileaddr"
 	 * and the strp1 pointer so that the next strip routine called
	 * after this one will start at the right place in the
	 * temporary file
	 * This is also needed for .m files
	 */
	if ((start_reloc != 0L) && (fileaddr < start_reloc))
		while (fileaddr < start_reloc) {
			gap = start_reloc - fileaddr;
			if (gap > BUFSIZ)
				gap = BUFSIZ;
			if (fwrite( buf, (int) gap, 1, strp1 ) != 1)
				return( ERROR );
			fileaddr += gap;
		}
#endif
    return(fileaddr);
}



    /*  cpyreloc(fileaddr)
     *
     *  cpyreloc is called only if lflag is ON
     *  or if the rflag is ON and local symbols have already been stripped
     *  It copies the relocation information to the temporary file
     *  and increments fileaddr, the address of the symbol table in a partially
     *  stripped file.
     *
     *  calls:
     *      - docopy((long) secthead.s_nreloc * RELSZ) to do the copying
     *	      after ldptr has been positioned at the start of the reloc info
     *
     *  returns the address to be used for the symbol table in a partially
     *  stripped if copy is successful
     *  returns ERROR otherwise.
     */


long
cpyreloc(fileaddr)

long	fileaddr;

{
    /* COMMON OBJECT FILE ACCESS ROUTINES CALLED */
    extern int		ldshread( ),
			ldrseek( );

    /* STRIP FUNCTION CALLED */
    extern int		docopy( );

    /* EXTERNAL VARIABLES USED */
    extern LDFILE	*ldptr;
#if m68k && !UNIX
	extern	int	fflag;
#endif

    /* LOCAL VARIABLES */
    SCNHDR		secthead;
    unsigned short	section;

    for (section = 1; section <= HEADER(ldptr).f_nscns; ++section) {
	if (ldshread(ldptr, section, &secthead) != SUCCESS) {
	    return(ERROR);
	}

	if (secthead.s_nreloc != 0) {
#if m68k && !UNIX
	    if (fflag && !(secthead.s_flags & STYP_COPY))
		continue;
#endif
	    if (ldrseek(ldptr, section) != SUCCESS) {
		return(ERROR);
	    }

	    if (docopy((long) secthead.s_nreloc * RELSZ) != SUCCESS) {
		return(ERROR);
	    }

#ifdef m68k
	    fileaddr += (long) secthead.s_nreloc * RELSZ;
#else
	    fileaddr += secthead.s_nreloc * RELSZ;
#endif
	}
    }

    return(fileaddr);
}
	/*	cpylnums( )
	 *
	 * cpylnums is called only if sflag is ON
	 * it copies the line number information to the temporary file
	 * and resets the symbol table index to the correct index of the
	 * partially stripped symbol table.
	 *
	 * fileaddr will be incremented to point to the spot where the
	 * new symbol table will be. cpylnums returns this value or ERROR
	 */

long
cpylnums(fileaddr)
long	fileaddr;
{
#ifdef m68k
	/* UNIX FUNCTIONS CALLED */
	extern	int	fread( );
	extern	int	fwrite( );
#endif

	/* COMMON OBJECT FILE ACCESS ROUTINES CALLED */
	extern int	ldlseek();
	extern int	ldshread();

	/* EXTERNAL VARIABLES USED */
	extern	LDFILE	*ldptr;
	extern	FILE	*strp1;
#ifdef vax
	extern int	docopy( );
#endif
#ifdef m68k

	/* STRIP FUNCTIONS CALLED */
	extern	long	newndx( );
#endif

	/* LOCAL VARIABLES */
	long		lindex;
	unsigned short	section;
	SCNHDR		secthead;
	LINENO		lnum;
	long		numlnums;

	for (section = 1; section <= HEADER(ldptr).f_nscns; ++section) {
		if (ldshread( ldptr, section, &secthead) != SUCCESS) {
#ifdef m68k
			fprintf( stderr, "cannot read section header for section %s\n", section );
			return(ERROR);
#else
			return(SUCCESS);
#endif
		}

		if (secthead.s_nlnno != 0) {
			if (ldlseek(ldptr, section) != SUCCESS) {
#ifdef m68k
				fprintf( stderr, "could not seek to line numbers\n" );
				return(ERROR);
#else
				return(SUCCESS);
#endif
			}

#ifdef m68k
			for (numlnums = 0L; numlnums < secthead.s_nlnno; ++numlnums) {
				if (FREAD( &lnum, LINESZ, 1, ldptr ) != 1) {
					fprintf( stderr, "could not read line numbers\n" );
					return( ERROR );
				}
				if (lnum.l_lnno == 0) {
					lindex = lnum.l_addr.l_symndx;
					if ((lindex = newndx(lindex)) == ERROR){
						fprintf( stderr, "could not locate new symbol table index\n" );
						return( ERROR );
					}
					lnum.l_addr.l_symndx = lindex;
				}
				if (fwrite( &lnum, LINESZ, 1, strp1) != 1) {
					fprintf( stderr, "could not write line numbers\n" );
					return( ERROR );
				}
			}

			fileaddr += (long) secthead.s_nlnno * LINESZ;
#else
		if ( docopy((long) secthead.s_nlnno * LINESZ) != SUCCESS) {
			return(SUCCESS);
		}

		fileaddr += secthead.s_nlnno * LINESZ;
#endif
		}
	}

	return(fileaddr);
}



    /*  copysyms( )
     *
     *  copies the entire symbol table to the temporary file (if lflag is ON
     *  or if the rflag is on and local symbols have already been stripped)
     *  sets the line number pointer in function auxiliary entries to zero
     *
     *  returns SUCCESS or FAILURE
     */


int
copysyms( )

{
	/* UNIX FUNCTIONS CALLED */
	extern	int	fread( ),
			fwrite( );

	/* COMMON OBJECT FILE ACCESS ROUTINES CALLED */
	extern	int	ldtbseek( );
#if m68k && FLEXNAMES
	extern	int	ldahread( );
#endif

	/* EXTERNAL VARIABLES USED */
	extern	LDFILE	*ldptr;
	extern	FILE	*strp1;
#if m68k && !UNIX
	extern	int	fflag;
	extern	long	reloc_ptrs[];
#else
    extern int		sflag;
#endif

	/* LOCAL VARIABLES */
	SYMENT		symbol;
	long		symindx;
	AUXENT		aux;
#if m68k && FLEXNAMES
	ARCHDR		arhead;
	long		str_size;
	long		size;
	long		str_table;
#endif

	if (ldtbseek(ldptr) != SUCCESS)
		return(FAILURE);

	for (symindx = 0L; symindx < HEADER(ldptr).f_nsyms; ++symindx)
	{
		if (FREAD(&symbol, SYMESZ, 1, ldptr) != 1)
			return(FAILURE);

		if (fwrite(&symbol, SYMESZ, 1, strp1) != 1)
			return(FAILURE);

		if (symbol.n_numaux == 1)
		{
			if (FREAD(&aux, AUXESZ, 1, ldptr) != 1)
				return(FAILURE);
			++symindx;

			if (ISFCN(symbol.n_type))
			{
				aux.x_sym.x_fcnary.x_fcn.x_lnnoptr = 0L;
			}

#ifdef m68k
			if ((symbol.n_sclass == C_STAT) 
			    && (symbol.n_zeroes != 0L)
			    && (strncmp( _TEXT, symbol.n_name, 8 ) == 0))
			{
				aux.x_scn.x_nlinno = 0;
#if !UNIX
				if (fflag && (reloc_ptrs[symbol.n_scnum - 1] == 0L))
					aux.x_scn.x_nreloc = 0L;
#endif
			}

			if ((symbol.n_sclass == C_FCN) || 
			    (symbol.n_sclass == C_BLOCK) ||
			    (ISARY(symbol.n_type)))
				aux.x_sym.x_misc.x_lnsz.x_lnno = 0;

#endif
			if (fwrite(&aux, AUXESZ, 1, strp1) != 1)
			{
				return(FAILURE);
			}
		}
	}

#if m68k && FLEXNAMES
	/* copy the string table, if there is one */
	str_table = STROFFSET(ldptr);
	if (TYPE(ldptr) == USH_ARTYPE)
	{
		if (ldahread( ldptr, &arhead ) != SUCCESS)
		{
			fprintf( stderr, "could not read archive header" );
			return( FAILURE );
		}

		if (arhead.ar_size <= (str_table + 1))
			return( SUCCESS );

		if ((FSEEK( ldptr, str_table, BEGINNING ) != OKFSEEK)
		    || (FREAD( &str_size, sizeof( long ), 1, ldptr ) != 1 ))
		{
			fprintf( stderr, "could not read string table\n" );
			return( FAILURE );
		}
	}
	else if ((FSEEK( ldptr, str_table, BEGINNING ) != OKFSEEK)
		    || (FREAD( &str_size, sizeof( long ), 1, ldptr ) != 1 ))
		return( SUCCESS );

	/* make it even so the archive won't get screwed up */
	size =  str_size + (str_size & 01); 
	if ((fwrite( &size, sizeof( long ), 1, strp1 ) != 1)
			|| (docopy( str_size - 4 ) != SUCCESS ))
		return( FAILURE );

	if (size != str_size)
		putc( '\0', strp1 );
#endif

	return(SUCCESS);
}




/* EXTERNAL VARIABLE DEFINED */
LDFILE	*fwdptr;


    /*  copyexts(filename)
     *
     *  copies external, static and extdef symbols to the temporary file (xflag)
     *  sets the line number pointer in function aux entries to zero
     *  sets the forward file index in function aux entries appropriatly for
     *  the stripped symbol table
     *  sets the index of a structure, union or enumeration tag entry to zero
     *  for any structure, union or enumeration types left in the table
     *
     *  defines:
     *      - fwdptr = ldaopen(filename, ldptr) to read entries between a
     *		       function and a .ef entry to determine the value of the
     *		       forward file index in the function aux entry
     *
     *  calls:
     *      - findndx(aux.x_sym.x_fcnary.x_fcn.x_endndx, oldndx, count) to determine the
     *        new value of aux.x_sym.x_fcnary.x_fcn.x_endndx
     *
     *  returns the number of symbols left in the symbol table after partial
     *  stripping if successful
     *  returns ERROR otherwise
     *
     *  Note that TUS people would like strip to keep .bf and .ef entries around
     *  when locals are stripped from the symbol table but this is not
     *  currently done.
     */


long
copyexts(filename)

char	*filename;

{
	/* UNIX FUNCTIONS CALLED */
	extern	int	fread( ),
#ifdef vax
			fseek( ),
#endif
			fwrite( );

	/* COMMON OBJECT FILE ACCESS ROUTINES CALLED */
	extern	LDFILE	*ldaopen( );
	extern	int	ldaclose( ),
			ldtbseek( );
#ifdef m68k
#ifdef FLEXNAMES
	extern	char	*ldgetname( );
#endif

	/* STRIP FUNCTION CALLED */
	extern	long	findndx( );
#if !AR16WR
	extern	TEMPENT	*getempent( );
#endif

	/* EXTERNAL VARIABLES USED */
	extern	LDFILE	*ldptr,
			*fwdptr;
	extern	FILE	*strp2;
#ifdef AR16WR
	extern	FILE	*readtmp,
			*tempfil;
#else
	extern	TEMPENT	*basep;
#endif
	extern	int	rflag,
			havelnums,
			sflag;
#if !UNIX
	extern	int	fflag;
	extern	long	rel_skipped;
	extern	long	reloc_ptrs[];
#endif
#ifdef FLEXNAMES
	extern	long	string_file;
	extern	char	tmpnam5[];
#endif

	/* LOCAL VARIABLES */
	SYMENT		symbol;
#ifdef AR16WR
	TEMPENT		temp;
#else
	TEMPENT		*temp;
	TEMPENT		*tail;
#endif
	long		oldndx;
	long		count;
	AUXENT		aux;
#ifdef FLEXNAMES
	FILE		*string_file;
	char		*sym_name;
	int		name_length;
#endif

	if ((fwdptr = ldaopen(filename, ldptr)) == NULL) {
		return(ERROR);
	}

	if (ldtbseek(ldptr) != SUCCESS) {
		return(ERROR);
	}

#ifdef FLEXNAMES
	string_size = 0L;
	if ((string_file = fopen( tmpnam5, "w" )) == NULL )
		return( ERROR );
#endif

	count = 0L;
#if !AR16WR
	tail = NULL;
#endif

	for (oldndx = 0L; oldndx < HEADER(ldptr).f_nsyms; ++oldndx)
	{
		if (FREAD(&symbol, SYMESZ, 1, ldptr) != 1)
		{
		 	return(ERROR);
		}

		if ((symbol.n_sclass == C_EXT) || (symbol.n_sclass == C_STAT) ||
				(symbol.n_sclass == C_HIDDEN) ||
	  			(symbol.n_sclass == C_EXTDEF) || 
				(symbol.n_sclass == C_FILE))
		{
#if AR16WR
			temp.new = count;
			temp.old = oldndx;
			if (fwrite(&temp, TEMPSZ, 1, tempfil) != 1)
			{
				fprintf(stderr,"couldn't fwrite to tempfil\n");
				return(ERROR);
			}
			fflush(tempfil);
#else
			temp = getempent( );
			temp->new = count;
			temp->old = oldndx;
			temp->link = NULL;
			if (tail)
				tail->link = temp;
			else
				basep = temp;
			tail = temp;
#endif
	 		++count;

#ifdef FLEXNAMES
			if (symbol.n_zeroes == 0L)
			{
				if ((sym_name = ldgetname( ldptr, &symbol )) == NULL)
					return( ERROR );
	
				name_length = strlen( sym_name ) + 1;
				if (fwrite( sym_name, name_length, 1, string_file ) != 1)
					return( ERROR );

				symbol.n_offset = string_size + 4;
				string_size += name_length;
			}
#endif
			if(symbol.n_sclass == C_FILE && symbol.n_value != 0L)
			{
				if((symbol.n_value = findndx(symbol.n_value, oldndx, count+symbol.n_numaux)) == ERROR)
				return(ERROR);
			}

			if (fwrite(&symbol, SYMESZ, 1, strp2) != 1)
			{
				return(ERROR);
			}

			if (symbol.n_numaux == 1)
			{
				if (FREAD(&aux, AUXESZ, 1, ldptr) != 1)
				{
					return(ERROR);
				}
				++oldndx;
				++count;

				if (ISFCN(symbol.n_type))
				{
					if ( !sflag) 
						aux.x_sym.x_fcnary.x_fcn.x_lnnoptr = 0L;
#if !UNIX
					else if ((havelnums == 1) && fflag && (aux.x_sym.x_fcnary.x_fcn.x_lnnoptr != 0))
						aux.x_sym.x_fcnary.x_fcn.x_lnnoptr -= rel_skipped;
#endif

					if ((aux.x_sym.x_fcnary.x_fcn.x_endndx =
						findndx(aux.x_sym.x_fcnary.x_fcn.x_endndx, oldndx, count)) == ERROR)
							return(ERROR);

				}

				if (!sflag && ISARY(symbol.n_type))
					aux.x_sym.x_misc.x_lnsz.x_lnno = 0;

				if ((symbol.n_sclass == C_STAT) 
				    && (symbol.n_zeroes != 0)
				    && (strncmp(_TEXT, symbol.n_name, 8 ) == 0))
				{
					if (!sflag)
						aux.x_scn.x_nlinno = 0;
#if !UNIX
					if (fflag && (reloc_ptrs[symbol.n_scnum - 1] == 0L))
						aux.x_scn.x_nreloc = 0L;
#endif
				}

				if (((symbol.n_type & N_BTMASK) == T_STRUCT) ||
				    ((symbol.n_type & N_BTMASK) == T_UNION) ||
				    ((symbol.n_type & N_BTMASK) == T_ENUM))
					aux.x_sym.x_tagndx = 0L;

				if (fwrite(&aux, AUXESZ, 1, strp2) != 1)
					return(ERROR);

			}

		}
		else   /* NOT EXT, STAT, EXTDEF, OR FILE	*/
		{
			if (symbol.n_numaux == 1)
			{
				if (FREAD(&aux, AUXESZ, 1, ldptr) != 1)
					return(ERROR);
			++oldndx;
			}
		}
	}

#if AR16WR
	fflush( tempfil );
#endif
#if FLEXNAMES
	if (string_size > 0L)
	{
		if (string_size & 01)
		{
			string_size += 1;
			putc( '\0', string_file );
		}
		string_size += 4;
	}
	fclose( string_file );
#endif

	ldaclose(fwdptr);
	fflush(strp2);
	return(count);
}

#else   /* vax */
	/* STRIP FUNCTION CALLED */
	extern	long	findndx( );

	/* EXTERNAL VARIABLES USED */
	extern	LDFILE	*ldptr,
			*fwdptr;
	extern	FILE	*strp2,
	                *readtmp,
			*tempfil;
	extern	int	rflag,
			havelnums,
			sflag;
	extern unsigned short	relno;

	/* LOCAL VARIABLES */
	SYMENT		symbol;
	LINENO          linum;
	TEMPENT		temp;
	long		oldndx;
	long		count;
	long		offset;
	AUXENT		aux;
	    extern char tmpnam1[ ];
	    FILE       	*fdes;
	    FILE	*fdes2;


    if(sflag) {
	if ((fdes = fopen(tmpnam1, "r+")) == NULL) {
		return(ERROR);
	}

    }


	if ((fwdptr = ldaopen(filename, ldptr)) == NULL) {
		return(ERROR);
	}

	if (ldtbseek(ldptr) != SUCCESS) {
		return(ERROR);
	}


	count = 0L;

	for (oldndx = 0L; oldndx < HEADER(ldptr).f_nsyms; ++oldndx) {
		if (FREAD(&symbol, SYMESZ, 1, ldptr) != 1) {
	 	    return(ERROR);
		}

	/*	This next section leaves in the .bf and .ef symbols. This
	 *	was not the case before.
	 */
	if ((symbol.n_sclass == C_EXT) || (symbol.n_sclass == C_STAT) ||
	    (symbol.n_sclass == C_EXTDEF) || (symbol.n_sclass == C_FILE)) {
	    if (rflag || sflag) {
		temp.new = count;
		temp.old = oldndx;
		if (fwrite(&temp, TEMPSZ, 1, tempfil) != 1) {
			fprintf(stderr,"couldn't fwrite to tempfil\n");
			return(ERROR);
		}
		fflush(tempfil);
	    }
            ++count;

	    if(symbol.n_sclass == C_FILE && symbol.n_value != 0L) {
		if((symbol.n_value = findndx(symbol.n_value, oldndx, count+symbol.n_numaux)) == ERROR) {
		    return(ERROR);
		}
	    }

	    if (fwrite(&symbol, SYMESZ, 1, strp2) != 1) {
		return(ERROR);
            }

	    if (symbol.n_numaux == 1) {
		if (FREAD(&aux, AUXESZ, 1, ldptr) != 1) {
		    return(ERROR);
		}
		++oldndx;
		++count;

		if (ISFCN(symbol.n_type)) {
		    if ( !sflag) 
			    aux.x_sym.x_fcnary.x_fcn.x_lnnoptr = 0L;
		    else {
		      if (havelnums == 1) {
			offset = aux.x_sym.x_fcnary.x_fcn.x_lnnoptr - FILHSZ;
			if ( fseek(fdes, offset, 0) != 0) {
				fprintf(stderr,"couldn't seek to lne info\n");
				return(ERROR);
			}
			if (fread(&linum, LINESZ, 1, fdes) != 1) {
				return(ERROR);
			}
			if (fseek(fdes, offset, 0) != 0) {
				fprintf(stderr,"couldnt seek to lne info\n");
				return(ERROR);
			}
			if (linum.l_lnno != 0 || linum.l_addr.l_symndx == 0L) {
				fprintf(stderr,"invalid line info for symbol %s\n", symbol.n_name);
				return(ERROR);
			}
			/* count must be decremented 2 now because we are 
			 * really looking at the zeroth entry not the second
			 * EXAMPLE  ---
			 * read symbol 0 increment count
			 * check and read aux entry increment count
			 * we are still looking at symbol zero not symbol 2
			 * therefore count must be decremented 2
			 */

			linum.l_addr.l_symndx = count - 2;
			if (fwrite(&linum, LINESZ, 1, fdes) != 1){
				fprintf(stderr,"FWRITE failed to write line info\n");
				return(ERROR);
			}
		      }
		    }

		    if ((aux.x_sym.x_fcnary.x_fcn.x_endndx = 
			findndx(aux.x_sym.x_fcnary.x_fcn.x_endndx, oldndx, count))
			== ERROR) {
			    return(ERROR);
		    }

		}

		if (((symbol.n_type & N_BTMASK) == T_STRUCT) ||
		    ((symbol.n_type & N_BTMASK) == T_UNION) ||
		    ((symbol.n_type & N_BTMASK) == T_ENUM)) {
	            aux.x_sym.x_tagndx = 0L;
                    }

		    if (fwrite(&aux, AUXESZ, 1, strp2) != 1) {
			return(ERROR);
                    }
		}

	    } else {  /* NOT EXT, STAT, EXTDEF, OR FILE	*/
		if (symbol.n_numaux == 1) {
	 	    if (FREAD(&aux, AUXESZ, 1, ldptr) != 1) {
			return(ERROR);
                    }
		    ++oldndx;
		}
	    }
	}


	ldaclose(fwdptr);
	fflush(strp2);
	if ( sflag) {
	    fflush(fdes);
	    fclose(fdes);
	}
	return(count);
}
#endif




    /*  findndx(fwdndx, oldndx, count)
     *
     *  determines the new index of the symbol that used to be at fwdndx
     *  (the entry after a .ef entry)
     *  simply counts the number of external, static or extdef symbols that
     *  appear between the current entry and the symbol at fwdndx
     *
     *  returns new index of symbol that used to be at fwdndx if successful
     *  returns ERROR if the symbol table is messed up
     */


long
findndx(fwdndx, oldndx, count)

#ifdef m68k
register long	fwdndx,
		oldndx,
		count;
#else
long	fwdndx,
	oldndx,
	count;
#endif

{
    /* COMMON OBJECT FILE ACCESS ROUTINE CALLED */
    extern int		ldtbread( );

    /* EXTERNAL VARIABLE USED */
    extern LDFILE	*fwdptr;

    /* LOCAL VARIABLE */
    SYMENT		symbol;


    /*  fwdndx is the old symbol table index of the entry following a .ef entry
     *  oldndx is the old symbol table index of the last symbol read
     *  count is the new index of the last symbol read
     *  the idea is to increment count for each symbol (up to fwdndx) that
     *  will be written to the stripped symbol table:  thus returning the
     *  new symbol table index of the symbol at old fwdndx
     */
    while (++oldndx < fwdndx) {
	if (ldtbread(fwdptr, oldndx, &symbol) != SUCCESS) {
	    return(ERROR);
	}

#ifdef m68k
	if ((symbol.n_sclass == C_EXT) || (symbol.n_sclass == C_STAT) ||
		(symbol.n_sclass == C_HIDDEN) ||
	    (symbol.n_sclass == C_EXTDEF) || (symbol.n_sclass == C_FILE)) {
#else
	if ((symbol.n_sclass == C_EXT) || (symbol.n_sclass == C_STAT) ||
	    (symbol.n_sclass == C_EXTDEF) || (symbol.n_sclass == C_FILE)) {
#endif
	    ++count;
	    if (symbol.n_numaux == 1) {
		++count;
		++oldndx;
	    }
	} else {
	    if (symbol.n_numaux == 1) {
		++oldndx;
	    }
	}
    }

    return(count);
}




    /*  docopy(size)
     *
     *  copies size bytes from the original object file (positioned by calling
     *  routine) to the temporary file (positioned by calling routine or by
     *  default)
     *
     *  docopy simply reads into and out of buffer until copying is done
     *
     *  returns SUCCESS or FAILURE
     */


int
docopy(size)

long	size;

{
    /* UNIX FUNCTIONS CALLED */
    extern int		fread( ),
			fwrite( );

    /* EXTERNAL VARIABLES USED */
    extern LDFILE	*ldptr;
    extern FILE		*strp1;

    /* LOCAL VARIABLES */
    short		buffer[BSIZE];
    long		index;
    short		remaindr;


    for (index = size/BSIZE; index > 0; --index) {
	if (FREAD(buffer, BSIZE, 1, ldptr) != 1) {
	    return(FAILURE);
	}

	if (fwrite(buffer, BSIZE, 1, strp1) != 1) {
	    return(FAILURE);
	}
    }

    if ((remaindr = size % BSIZE) != 0) {
	if (FREAD(buffer, remaindr, 1, ldptr) != 1) {
	    return(FAILURE);
	}

	if (fwrite(buffer, remaindr, 1, strp1) != 1) {
	    return(FAILURE);
	}
    }

    fflush(strp1);
    return(SUCCESS);
}


	/*	newreloc(fileaddr)
	 *
	 *
	 * newreloc will copy each relocation entry to the temporary
	 * file and reset the symbol table index to the correct
	 * index of the partially stripped symbol table.
	 * fileaddr will be incremented to point to the spot where the
	 * new symbol table will be. returns this value.
	 *
	 *
	 */


long
newreloc(fileaddr)

long	fileaddr;

{

	/* UNIX FUNCTIONS CALLED */
	extern	int		fread( ),
				fwrite( );
	/* COMMON OBJECT FILE ACCESS ROUTINES CALLED */
	extern int	ldshread( ),
#ifdef vax
			ldtbseek( ),
#endif
			ldrseek( );

	/* STRIP FUNCTIONS CALLED */
	extern	long	newndx( );

	/* EXTERNAL VARIABLES USED */
	extern LDFILE	*ldptr;
	extern	FILE	*strp1;
#ifdef vax
	extern LDFILE	*ldptr2;
	extern char	tmpnam1[ ];
	extern	char	*filename;
	extern  int	sflag;
#endif
#if m68k && !UNIX
	extern	int	fflag;
#endif

	/* LOCAL VARIABLES */
	SCNHDR		secthead;
	unsigned short	sect;
	long		numreloc;
	RELOC		rloc;
#ifdef vax
	long		rindex,
			rndx,
			offset;
	char		*sym;
	FILE		*fdes;

			if ( sflag) {

				if ((fdes = fopen(tmpnam1, "r+")) == NULL) {
					fprintf(stderr,"couldn't fopen temp\n");
					return(ERROR);
				}
			}
#endif

	for (sect = 1; sect <= HEADER(ldptr).f_nscns; ++sect) {
		if (ldshread(ldptr, sect, &secthead) != SUCCESS) {
			fprintf(stderr,"Could not read reloc info for sect. %d\n",sect);
			return(ERROR);
		}

		if (secthead.s_nreloc != 0) {
#ifdef m68k
#if !UNIX
			if (fflag && !(secthead.s_flags & STYP_COPY))
				continue;
#endif
			if (ldrseek(ldptr,sect) != SUCCESS) {
				fprintf(stderr,"Could not seek to reloc entries in sect: %d\n",sect);
				return(ERROR);
			}

			for(numreloc = 0L; numreloc < secthead.s_nreloc; ++numreloc) {
				if(FREAD(&rloc, RELSZ, 1,ldptr) != 1) {
					fprintf( stderr, "could not read relocation\n" );
					return(ERROR);
				}

				if((rloc.r_symndx = newndx( rloc.r_symndx )) == ERROR)
					return(ERROR);

				if (fwrite(&rloc, RELSZ, 1, strp1) != 1) {
					fprintf(stderr,"couldn't fwrite\n");
					return(ERROR);
				}
			} /* inner for loop */

		fileaddr += (long) secthead.s_nreloc * RELSZ;
		}
	} /* outer for loop */
#else
			if (sflag) {
				offset = secthead.s_relptr -FILHSZ;
				if ( fseek(fdes, secthead.s_relptr - FILHSZ, 0) != 0) {
					fprintf(stderr,"couldn't fseek to reloc\n");
					return(ERROR);
				}
			}else {
			if (ldrseek(ldptr,sect) != SUCCESS) {
				fprintf(stderr,"Could not seek to reloc entries in sect: %d\n",sect);
				return(ERROR);
			}
			}
			for(numreloc = 0L; numreloc < secthead.s_nreloc; ++numreloc) {
				if (sflag) {
				if (fseek(fdes, offset, 0) != 0) {
					fprintf(stderr,"couldn't fseek to relptr\n");
					return(ERROR);
				}
				if (fread(&rloc, RELSZ, 1, fdes) != 1) {
					fprintf(stderr,"could not read rloc\n");
				}
				} else {
				if(FREAD(&rloc, RELSZ, 1,ldptr) != 1) {
					return(FAILURE);
				}
				}
				rindex = rloc.r_symndx;
				if((rndx = newndx(rindex)) == ERROR) {
					return(ERROR);
				}
				rloc.r_symndx = rndx;
				if ( sflag) {
					if (fseek(fdes, offset, 0) != 0) {
						fprintf(stderr," couldn't fseek\n");
						return(ERROR);
					}
					if (fwrite(&rloc, RELSZ, 1, fdes) != 1) {
						fprintf(stderr,"couldn't fwrite reloc\n");
						return(ERROR);
					}
					offset = ftell(fdes);
				} else {
					if (fwrite(&rloc, RELSZ, 1, strp1) != 1) {
						fprintf(stderr,"couldn't fwrite\n");
						return(ERROR);
					}
				}
			}
		fileaddr += secthead.s_nreloc * RELSZ;
		}
	}
	if (sflag)
		fflush(fdes);
#endif
	fflush(strp1);
	return(fileaddr);
}



	/* newndx(index)
	 *
	 *
	 * newndx accepts as input, a symbol index, gotten from the old
	 * symbol table in <fileindex>. This symbol name is then compared
	 * to the symbol indexs from a partially stripped symbol table.
	 * When a match is found, the index of the symbol is returned, else
	 * ERROR
	 *
	 */


long
newndx(rind)

#ifdef m68k
register long	rind;
#else
long	rind;
#endif

{

	/* UNIX FUNCTIONS CALLED */
	extern	int		fseek( );
	extern	int		fread( );

	/* EXTERNAL VARIABLES USED */
#if vax || AR16WR
	extern	FILE		*readtmp;
	extern  FILE		*tempfil;
#else
	extern	TEMPENT		*basep;
#endif
	extern	FILHDR		filehead;

	/* LOCAL VARIABLES */
#ifdef m68k
#if AR16WR
	TEMPENT			tmp;
	register long		count;
#else
	register TEMPENT	*temp;
#endif
#else 
	TEMPENT			tmp;
	long			offset;
	long			count;
#endif

	if (rind == -1L)
		return(-1L);

#if vax || AR16WR
	count = 0L;
	if(fseek(readtmp,0L,0) != 0) {
		fprintf(stderr,"Could not seek to beginning of file.\n");
		return(ERROR);
	}
	for(count=0; count < filehead.f_nsyms; ++count) {
		if(fread(&tmp, TEMPSZ, 1, readtmp) != 1) {
			fprintf(stderr,"Could not read index extry. Index: %ld\n",rind);
			return(ERROR);
		}
		if( rind == tmp.old ) {
			return((long) tmp.new);
		}
	}
#else
	for ( temp = basep; temp; temp = temp->link ) {
		if (temp->old == rind)
			return( (long) temp->new );
	}
#endif

	fprintf(stderr,"No index found for relocation entry: %lo\n",rind);
	return(ERROR);
}

#ifdef m68k
#if !AR16WR
/*
 * getempent will return an item for a TEMPENT linked list. If necessary,
 * malloc() will be called to get space for the item. getempent will
 * get space for multiple items whenever it calls malloc();
 */

TEMPENT *
getempent()
{
	/* UNIX FUNCTIONS CALLED */
	extern	char	*malloc( );

	/* EXTERNAL VARIABLES USED */
	extern	TEMPENT	*entptr;
	extern	TEMPENT	*oldptr;

	/* VARIABLES DEFINED */
	static	int	nument;


	if ((oldptr == NULL) || (nument >= MAXTEMPENTS)) {
		if ((entptr = (TEMPENT *) malloc( sizeof(TEMPENT) * MAXTEMPENTS )) == NULL) {
			fprintf( stderr, "malloc failed, can't reset relocation\n");
			exit(1);
		}

		entptr->link = oldptr;
		oldptr = entptr;
		nument = 1;
		entptr++;
	}
	nument++;
	return( entptr++ );

}

/*
 * freetempent() will free up the space used by the TEMPENT list
 */

freetempent( )
{
	/* EXTERNAL VARIABLES USED */
	extern	TEMPENT	*oldptr;
	extern	TEMPENT	*entptr;

	for ( ; oldptr; oldptr = oldptr->link ) {
		free( oldptr );
	}

	oldptr = entptr = NULL;
}
#endif

/*
 *	static char ID[] = "@(#) buildfcns.c: 1.14 5/1/83";
 */
#endif
