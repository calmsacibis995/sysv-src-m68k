/*	@(#)fcns.c	2.1		*/
static char ID[] = "@(#) fcns.c: 1.18 5/1/83";

#include	<stdio.h>
#include	<ar.h>
#include	"filehdr.h"
#include	"conv.h"
#include	"aouthdr.h"
#include	"patch.h"
#include	"scnhdr.h"
#include	"reloc.h"
#include	"linenum.h"
#include	"syms.h"
#include	"ldfcn.h"

#if DMERT
#include	"magic.h"
#include	"ncps.h"
#include	"dtype.h"
#include	"bool.h"
#include	"pev.h"
#include	"ldpdefs.h"
#include	"utilid.h"
#include	"phdr.h"
#endif

static long	filoffst;	/* where we were when we finished */

unsigned short	source = F_CURARCH;	/* where we are (machine-wise)	*/

extern unsigned short	origin,		/* where the file originated	*/
			bootable,	/* true if writing bootable objects */
			target;		/* where the file is going	*/
extern unsigned short	ar_target;	/* archive format of output file */
extern int		cnvtyp;		/* how to get it there		*/

extern	FILE	*outstrm;
extern	int	preswab, swab;
extern	char	outfile[];


extern	char	*malloc();

extern
	convahdr(),
	convfhdr(),
	convohdr(),
	convshdr(),
	convscns(),
	convrel(),
	convline(),
	convsyms();
#ifdef FLEXNAMES
extern	int	convstrings( );
#endif

static	int	(*(funcs[]))() = {
	0,
	convahdr,
	convfhdr,
	convohdr,
	convshdr,
	convscns,
	convrel,
	convline,
	convsyms,
#ifdef FLEXNAMES
	convstrings,
#endif
	0
	} ;

static char *funcom[] = {
	"",
	"archive header",
	"file header",
	"optional header",
	"section header(s)",
	"raw section(s)",
	"relocation information",
	"line number entries",
	"symbol table",
#ifdef FLEXNAMES
	"string table",
#endif
	""
	};

static	long	member_size;

callfunc(ldptr,infile)

LDFILE	*ldptr;
register char	*infile;
{
	register int fundx;
	void boot_pad();

	for ( fundx = 1; funcs[fundx] != NULL; ++fundx) {
		if ( (*funcs[fundx])(ldptr,infile) != SUCCESS ) {
			converr(1,infile,"failed converting %s", funcom[fundx]);
			return(FAILURE);
			}

		}
	if ( bootable )
		boot_pad();
	return(SUCCESS);
}

void
boot_pad()
{
	register long n;

	(void) fflush( outstrm );
	if( n = ftell( outstrm ) % 1024 )
		for( n = 1024 - n ; n ; n-- )
			putc( '\0', outstrm );
}


convahdr(ldptr,infile)

register LDFILE	*ldptr;
register char	*infile;
{
	/* functions called */
	extern int	fwrite( );

	/* libld functions called */
	extern int	ldahread( );

	/* local variables */
	NEWARHDR	arhead;
	RANDARF		arfbuf;
	PORT_HEAD	porthead;
	char		c;
	short		s;

	if (bootable || TYPE(ldptr) != USH_ARTYPE)
		return(SUCCESS);

	if (ldahread(ldptr, &arhead) == SUCCESS)
	{
#ifdef FLEXNAMES
		member_size = arhead.ar_size;
#endif

		if ( ar_target == RANDOM )
		{
			strncpy( arfbuf.arf_name, arhead.ar_name,  16 );
			sputl( arhead.ar_date, arfbuf.arf_date );
			sputl( (long) arhead.ar_uid, arfbuf.arf_uid);
			sputl( (long) arhead.ar_gid, arfbuf.arf_gid );
			sputl( arhead.ar_mode, arfbuf.arf_mode );
			sputl( arhead.ar_size, arfbuf.arf_size );
			if (fwrite( &arfbuf, sizeof(RANDARF), 1, outstrm ) != 1)
				converr(1, outfile, "cannot fwrite()");

		}
		else if ( ar_target == ASCIIAR )
		{
			int	i;

			strncpy( porthead.ar_name, arhead.ar_name, FILNMLEN );
			i = strlen( arhead.ar_name );
			porthead.ar_name[i] = '/';
			for ( i++; i < 16; i++ )
				porthead.ar_name[i] = ' ';

			sprintf( porthead.ar_date, "%-12ld", arhead.ar_date );
			sprintf( porthead.ar_uid, "%-6u", arhead.ar_uid );
			sprintf( porthead.ar_gid, "%-6u", arhead.ar_gid );
			sprintf( porthead.ar_mode, "%-8o", arhead.ar_mode );
			sprintf( porthead.ar_size, "%-10ld", arhead.ar_size );
			strncpy( porthead.ar_fmag, ARFMAG, ARFMAGSIZE );
			if (fwrite( &porthead, sizeof( PORT_HEAD ), 1, outstrm ) != 1)
				converr( 1, outfile, "cannot fwrite()" );

		}
		else 
		{
			if (target != F_CURARCH)
			{
				charconv( arhead.ar_name, FILNMLEN );
				longconv( &arhead.ar_date );
				longconv( &arhead.ar_size );
			}

			if (fwrite(arhead.ar_name,sizeof(char),FILNMLEN,outstrm) != FILNMLEN)
				converr(1, outfile, "cannot fwrite()");
			if ( target != F_AR16WR )
				fseek(outstrm, 2L, 1);	/* padding */
			if (fwrite(&arhead.ar_date,sizeof(long),1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
			c = (char) arhead.ar_uid;
			if (fwrite( &c, sizeof(char),1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
			c = (char) arhead.ar_gid;
			if (fwrite( &c, sizeof(char),1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
			if ( target != F_AR16WR )
				fseek(outstrm, 2L, 1);	/* padding */

			if ( target == F_AR16WR )
			{
				s = (short) arhead.ar_mode;
				if (target != F_CURARCH)
					shortconv( &s );
				fwrite( (char *) &s, sizeof(short),1,outstrm);
			} else  {
				if (target != F_CURARCH)
					longconv( &arhead.ar_mode );
				if (fwrite((char *) &arhead.ar_mode,sizeof(long),1,outstrm) != 1)
					converr(1, outfile, "cannot fwrite()");
			}

			if (fwrite((char *) &arhead.ar_size,sizeof(long),1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
		}
	} else {
		converr(1,infile, "cannot ldahread");
		return(FAILURE);
	}

	return(SUCCESS);
}

convfhdr(ldptr,infile)

LDFILE	*ldptr;
char	*infile;
{
	extern int	fwrite( );
	FILHDR	filehead;

	if (ldfhread(ldptr, &filehead) == SUCCESS) {
		/*
		 * convert file header
		 */

		if ( cnvtyp == 0 ) {
			if ( origin & filehead.f_flags )
				filehead.f_flags ^= source;
			filehead.f_flags |= target;
			if ( preswab )
				filehead.f_flags |= F_SWABD;
			}

		shortconv(&filehead.f_magic);
		ushortconv(&filehead.f_nscns);
		longconv(&filehead.f_timdat);
		longconv(&filehead.f_symptr);
		longconv(&filehead.f_nsyms);
		ushortconv(&filehead.f_opthdr);
		ushortconv(&filehead.f_flags);

		if ( cnvtyp != 0 ) {
                        if ( origin & filehead.f_flags )
                                filehead.f_flags ^= origin;
                        filehead.f_flags |= target;
                        if ( preswab )
                                filehead.f_flags |= F_SWABD;
                        }

		if ( !bootable )
			if (fwrite((char *) &filehead,FILHSZ,1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
	} else {
		converr(1,infile,"cannot ldfhread");
		return(FAILURE);
	}

	filoffst = (long) FILHSZ;

	return(SUCCESS);
}

convohdr(ldptr,infile)

LDFILE	*ldptr;
char	*infile;
{

	extern	int	fwrite( );
	char	*opthead, *tmp;
	short	magic;
	int	hdrsize, num;
	long	count;

	if ( HEADER(ldptr).f_opthdr == 0 )
		return(SUCCESS);

	if ( ldohseek(ldptr) == SUCCESS ) {
		/*
		 * convert optional header
		 */

		opthead = malloc(HEADER(ldptr).f_opthdr);
		FREAD(opthead, HEADER(ldptr).f_opthdr, 1, ldptr);

		/*
		 * deal with three cases
		 *	(1) standard unix a.out headers
		 *	(2) pfile headers
		 *	(3) publib headers
		 * all of these may also have an optional
		 * "patch" list, the existence of which we
		 * can, hopefully, deduce. this is the shakiest,
		 * most difficult, and least clean part of the
		 * conversion process
		 */

		magic = *((short *)opthead);
		ldushortconv(&magic);

		hdrsize = 0;

		switch ( magic ) {

		case CNMAGIC:
		case COMAGIC:
		case CPMAGIC:
			hdrsize = AOUTSIZE;
			aoutconv(opthead);
			break;
#if DMERT
		case PLIBMAGIC:
		case DLIBMAGIC:
			hdrsize = sizeof(struct lhead) - FILHSZ;
			lhdrconv(opthead);
			break;

		case KMAGIC:
		case SMAGIC:
		case USRMAGIC:
			hdrsize = PHDRSIZE - FILHSZ;
			phdrconv(opthead);
			break;

		case BOOTMAGIC:
			/* don't know how to convert */
			break;
#endif
		default:
			break;		/* assume it is a patch list for now */
		} /* switch */

		/*
		 * If any more opthdr remains to be converted, see if it
		 * is a patch list. If a couple of simple tests determine
		 * that treating it as such would create problems, print
		 * a warning and just copy the rest of section.
		 */

		if ( (HEADER(ldptr).f_opthdr - hdrsize) > 0 ) {
			count = *((long *)(opthead + hdrsize));
			ldlongconv(&count);
			if ( (count * (long)PESIZE + sizeof(long) + hdrsize) !=
						HEADER(ldptr).f_opthdr) {
				converr(0,infile,"optional header of unknown structure...copying");
			} else {
				/* convert patch list */
				longconv(opthead+hdrsize);
				tmp = opthead + hdrsize + sizeof(long);
				for ( num=0; count--; num += PESIZE ) {
					longconv(tmp+num);
					longconv(tmp+num+sizeof(long));
					ushortconv( tmp + num + 2*sizeof(long));
					charconv((tmp+num+ 2*sizeof(long) +sizeof(short)), 2);
					}
				}
			}

		if ( ! bootable )
			if (fwrite(opthead, HEADER(ldptr).f_opthdr, 1, outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
		free(opthead);
	} else {
		converr(1,infile,"cannot ldohseek");
		return(FAILURE);
		}

	filoffst += (long) HEADER(ldptr).f_opthdr;

	return(SUCCESS);
}

convshdr(ldptr,infile)

LDFILE	*ldptr;
char	*infile;
{
	extern	int	fwrite( );

	int	num;
	SCNHDR	sechead;

	for ( num = 1; num <= HEADER(ldptr).f_nscns; ++num ) {
		if ( ldshread(ldptr, num, &sechead) == SUCCESS ) {

			filoffst += (long) SCNHSZ;

			/*
			 * convert section header
			 */

			charconv(sechead.s_name,8);
			longconv(&sechead.s_paddr);
			longconv(&sechead.s_vaddr);
			longconv(&sechead.s_size);
			longconv(&sechead.s_scnptr);
			longconv(&sechead.s_relptr);
			longconv(&sechead.s_lnnoptr);
			ushortconv(&sechead.s_nreloc);
			ushortconv(&sechead.s_nlnno);
			longconv(&sechead.s_flags);

			if ( !bootable )
				if (fwrite(&sechead,SCNHSZ,1,outstrm) != 1)
					converr(1, outfile, "cannot fwrite()");
		} else {
			converr(1,infile,"cannot ldshread section %d", num);
			return(FAILURE);
			}
		}
	return(SUCCESS);
}

convscns( ldptr, infile )

LDFILE	*ldptr;
char	*infile;
{
	int	num;
	SCNHDR	sechead;

	for ( num = 1; num <= HEADER(ldptr).f_nscns; ++num ) {
		if ( ldshread(ldptr, num, &sechead) == SUCCESS ) {

			/*
			 * convert raw sections
			 */

			ldlongconv(&sechead.s_scnptr);
			ldlongconv(&sechead.s_size);

			if ( sechead.s_scnptr == 0L )
				continue;

			if ( sechead.s_scnptr != filoffst ) {
				FSEEK(ldptr, filoffst, BEGINNING);
				copystrms(ldptr, outstrm, sechead.s_scnptr-filoffst);
				filoffst = sechead.s_scnptr;
				}

			if ( ldsseek(ldptr, num) == SUCCESS ) {

				/* handle odd byte sections for the 3B */
				if (IS3B(HEADER(ldptr).f_magic))
					sechead.s_size += sechead.s_size % 2;
				filoffst += sechead.s_size;

				if ( copystrms(ldptr,outstrm,sechead.s_size) != SUCCESS )
					return(FAILURE);
			} else {
				converr(1,infile,"cannot ldsseek section %d",num);
				return(FAILURE);
				}
		} else {
			converr(1,infile,"cannot ldshread section %d",num);
			return(FAILURE);
			}

		} /* for */

	return(SUCCESS);
}

convrel( ldptr, infile )

LDFILE	*ldptr;
char	*infile;
{
	extern	int	fwrite( );

	int	numsec, numrel;
	SCNHDR	sechead;
	RELOC	relentry;

	for ( numsec = 1; numsec <= HEADER(ldptr).f_nscns; ++numsec ) {
		if ( ldshread(ldptr, numsec, &sechead) != SUCCESS ) {
			converr(1,infile,"cannot ldshread section %d",numsec);
			return(FAILURE);
			}

		ldushortconv(&sechead.s_nreloc);
		ldlongconv(&sechead.s_relptr);

		if ( sechead.s_nreloc == 0 )
			continue;

		if ( bootable ) {
			converr(1,infile,"file not completely linked");
			return(FAILURE);
			}

		if ( ldrseek(ldptr, numsec) != SUCCESS ) {
			converr(1,infile,"cannot ldrseek section %d",numsec);
			return(FAILURE);
			}

		if ( filoffst != sechead.s_relptr ) {
			FSEEK(ldptr, filoffst, BEGINNING);
			copystrms(ldptr, outstrm, sechead.s_relptr-filoffst);
			filoffst = sechead.s_relptr;
			}

		filoffst += (long)(sechead.s_nreloc * (long) RELSZ);

		for ( numrel = 0 ; numrel < sechead.s_nreloc ; ++numrel) {
			if ( FREAD(&relentry, RELSZ, 1, ldptr) != SUCCESS ) {
				converr(1,infile,"cannot read reloc entry from\
							section %d",numsec);
				return(FAILURE);
				}


			/*
			 * convert relocation entries
			 */
	
			longconv(&relentry.r_vaddr);
			longconv(&relentry.r_symndx);
			ushortconv(&relentry.r_type);

			if ( !bootable )
				if (fwrite( &relentry, RELSZ, 1, outstrm) != 1)
					converr(1, outfile, "cannot fwrite()");
			}
		}

	return(SUCCESS);
}

convline( ldptr, infile )

LDFILE	*ldptr;
char	*infile;
{
	extern	int	fwrite( );

	int	numsec, numline;
	SCNHDR	sechead;
	LINENO	linentry;

	for ( numsec = 1; numsec <= HEADER(ldptr).f_nscns; ++numsec ) {

		if ( ldshread(ldptr, numsec, &sechead) != SUCCESS ) {
			converr(1,infile,"cannot ldshread section %d",numsec);
			return(FAILURE);
			}

		ldushortconv(&sechead.s_nlnno);
		ldlongconv(&sechead.s_lnnoptr);

		if ( sechead.s_nlnno == 0 )
			continue;

		if ( ldlseek(ldptr, numsec) != SUCCESS ) {
			converr(1,infile,"cannot ldlseek section %d",numsec);
			return(FAILURE);
			}

		if ( filoffst != sechead.s_lnnoptr ) {
			FSEEK(ldptr, filoffst, BEGINNING);
			copystrms(ldptr, outstrm, sechead.s_lnnoptr-filoffst);
			filoffst = sechead.s_lnnoptr;
			}

		filoffst += (long) (sechead.s_nlnno * (long) LINESZ);

		for ( numline = 0 ; numline < sechead.s_nlnno ; ++numline) {
			if ( FREAD(&linentry, LINESZ, 1, ldptr) != SUCCESS ) {
				converr(1,infile,"cannot read lineno entry from\
							section %d", numsec);
				return(FAILURE);
				}

			/*
			 * convert line number entries
			 */

			longconv(&linentry.l_addr.l_symndx);
			ushortconv(&linentry.l_lnno);

			if ( !bootable )
				if (fwrite( &linentry, LINESZ, 1, outstrm) != 1)
					converr(1, outfile, "cannot fwrite()");
			}
		}

	return(SUCCESS);
}

convsyms( ldptr, infile )

LDFILE	*ldptr;
char	*infile;
{
	extern int	fwrite( );

	long		numsym;
	unsigned short	type;
	char		class,
			numaux,
			name[SYMNMLEN];
	SYMENT		sym,
			csym;
	AUXENT		aux;

	if ( HEADER(ldptr).f_nsyms == 0L )
		return(SUCCESS);

	if ( ldtbseek(ldptr) != SUCCESS ) {
		converr(1,infile,"cannot ldtbseek");
		return(FAILURE);
		}

	if ( HEADER(ldptr).f_symptr != filoffst ) {
		FSEEK(ldptr, filoffst, BEGINNING);
		copystrms(ldptr, outstrm, HEADER(ldptr).f_symptr-filoffst);
		filoffst = HEADER(ldptr).f_symptr;
		}

	for ( numsym=0L; numsym < HEADER(ldptr).f_nsyms;  ) {

		if ( ldtbread(ldptr, numsym, &sym) != SUCCESS ) {
			converr(1,infile,"cannot ldtbread symbol %ld",numsym);
			return(FAILURE);
			}

		++numsym;

		/*
		 * save needed information
		 */

		csym = sym;
		ldushortconv(&csym.n_type);
		strncpy(name, csym.n_name, SYMNMLEN);
		class = csym.n_sclass;
		type  = csym.n_type;
		numaux= csym.n_numaux;

		/*
		 * convert syment
		 */

#ifdef FLEXNAMES
		if (sym.n_zeroes == 0L)
			longconv( &sym.n_offset );
		else
#endif
			charconv(sym.n_name, SYMNMLEN);
		longconv(&sym.n_value);
		ushortconv(&sym.n_scnum);
		ushortconv(&sym.n_type);
		/*
		 * next stmt converts both n_sclass and n_numaux
		 */
		charconv(&sym.n_sclass, 2);

		if ( !bootable )
			if (fwrite(&sym, SYMESZ, 1, outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");

		/*
		 * loop through auxiliary entries
		 */

		while ( numaux-- > 0 ) {

			if ( ldtbread(ldptr, numsym, &aux) != SUCCESS ) {
				converr(1,infile,"cannot ldtbread symbol %ld",
								numsym);
				return(FAILURE);
				}

			++numsym;

			switch(class) {

			case C_FILE:
				/*
				 * convert char x_fname[FILNMLEN]
				 */
				charconv(aux.x_file.x_fname, FILNMLEN);
				break;

			case C_STAT:
			case C_HIDDEN:
				/*
				 * is it .data, .bss, or .text ?
				 */
				if ( type == T_NULL) {
#if DMERT
					if ( ! strncmp( ".bt", name, SYMNMLEN )) {
						/*
						 * convert .bt aux entry
						 */
						longconv( &aux.x_tv.x_tvfill );
						ushortconv( &aux.x_tv.x_tvlen );
						ushortconv( &aux.x_tv.x_tvran[0] );
						ushortconv( &aux.x_tv.x_tvran[1] );
						break;
					} else {
#endif
						/*
					 	 * convert x_scn
					 	 */
						longconv(&aux.x_scn.x_scnlen);
						ushortconv(&aux.x_scn.x_nreloc);
						ushortconv(&aux.x_scn.x_nlinno);
						break;
					}
#if DMERT
				}
#endif

			case C_EXT:
				/*
				 * is it .tv?
				 */
				if ( (type == T_NULL) && ( ! strncmp( _TV, name, SYMNMLEN ))) {
					longconv( &aux.x_tv.x_tvfill );
					ushortconv( &aux.x_tv.x_tvlen );
					ushortconv( &aux.x_tv.x_tvran[0] );
					ushortconv( &aux.x_tv.x_tvran[1] );
					break;
				}

			default:
				/*
				 * it is not a .file, .text, .data, .bss,
				 *  or a .tv so it is some form of an x_sym
				 */

				longconv(&aux.x_sym.x_tagndx);

				/*
				 * is it x_fsize or x_lnsz ?
				 * is it x_fcn or x_ary ?
				 */

				if ( ISFCN(type) ) {
					/*
					 * convert x_misc.x_fsize
					 */
					longconv(&aux.x_sym.x_misc.x_fsize);
				} else {
					/*
					 * convert x_misc.x_lnsz
					 */
					ushortconv(&aux.x_sym.x_misc.x_lnsz.x_lnno);
					ushortconv(&aux.x_sym.x_misc.x_lnsz.x_size);
					}

				if ( ! ISARY(type) ) {
					/*
					 * convert x_fcnary.x_fcn
					 */
					longconv(&aux.x_sym.x_fcnary.x_fcn.x_lnnoptr);
					longconv(&aux.x_sym.x_fcnary.x_fcn.x_endndx);
				} else {
					/*
					 * convert x_fcnary.x_ary
					 */
					ushortconv(&aux.x_sym.x_fcnary.x_ary.x_dimen[0]);
					ushortconv(&aux.x_sym.x_fcnary.x_ary.x_dimen[1]);
					ushortconv(&aux.x_sym.x_fcnary.x_ary.x_dimen[2]);
					ushortconv(&aux.x_sym.x_fcnary.x_ary.x_dimen[3]);
					}

					ushortconv(&aux.x_sym.x_tvndx);

				break;
			} /* switch */

			if ( !bootable )
				if (fwrite(&aux, AUXESZ, 1, outstrm) != 1)
					converr(1, outfile, "cannot fwrite()");

			} /* while */
		} /* for */

#ifdef FLEXNAMES
	filoffst += HEADER(ldptr).f_nsyms * SYMESZ;
#endif
	return(SUCCESS);
}

#ifdef FLEXNAMES
int
convstrings( ldptr, infile )

LDFILE	*ldptr;
char	*infile;

{
	/* local variables */
	long	str_address;
	long	string_size;
	long	size_buf;
	int	swabsave;


	str_address = STROFFSET(ldptr);
	if ( str_address != filoffst )
	{
		FSEEK( ldptr, filoffst, BEGINNING );
		copystrms( ldptr, outstrm, str_address - filoffst );
		filoffst = str_address;
	}

	if (TYPE(ldptr) == USH_ARTYPE)
	{
		if (str_address == member_size)
			return( SUCCESS );
		else if (FREAD( &size_buf, sizeof(long), 1, ldptr ) != 1)
		{
			converr( 1, infile, "cannot read size of string table");
			return( FAILURE );
		}
	} else
		if (FREAD( &size_buf, sizeof(long), 1, ldptr ) != 1)
			return( SUCCESS );

	string_size = size_buf;
	ldlongconv( &string_size );
	longconv( &size_buf );
	if ( !bootable )
		if (fwrite( &size_buf, sizeof(long), 1, outstrm ) != 1)
		{
			converr( 1, outfile, "cannot fwrite()" );
			return( FAILURE );
		}

	swabsave = swab;
	swab = 0;
	if ( copystrms( ldptr, outstrm, string_size - 4 ) == FAILURE )
		return( FAILURE );
	swab = swabsave;

	/* for archives, force string table to be even length */

	if (TYPE(ldptr) == USH_ARTYPE)
		if ( fflush(outstrm) ||
		( (ftell(outstrm) & 1) && (fwrite("", 1, 1, outstrm) != 1) ) )
		{
			converr( 1, outfile, "cannot pad string table" );
			return( FAILURE );
		}
	return( SUCCESS );
}
#endif
