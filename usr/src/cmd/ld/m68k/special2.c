/*	@(#)special2.c	2.1		*/
/*
static char spec2_ID[] = "@(#) special2.c:	2.4 83/07/08";
*/

#include <stdio.h>

#include "system.h"
#include "aouthdr.h"
#include "structs.h"
#include "ldfcn.h"
#include "tv.h"
#include "ldtv.h"
#include "slotvec.h"
#include "reloc.h"
#include "extrns.h"
#include "list.h"
#include "sgs.h"
#include "sgsmacros.h"

unsigned short swapb2();

#if AR32WR && !defined(m68k)
#define SWAPB4(x)	swapb4(x,0)
#else
#define SWAPB4(x)	x
#endif

#define MASK24 0x0ffffff0L
#define SHIFT24 0x4

static char tvreferr[] = "tv reference to non-tv symbol: addr %0.1lx, index %ld, file %s";

#if TRVEC
#if (AR32WR || AR16WR) && !defined(m68k)
static union {
	long l;
	unsigned short u[2];
	char c[4];
	} tmpslot;
static unsigned short tmp_us;
#endif
#endif

void
adjneed( need, osp, sap)

ADDRESS	*need;
OUTSECT	*osp;
ANODE	*sap;
{
}

void
undefine()
{
	/*
	 * for m68k version of ld01.c, the symbol "_start" does
	 * need to be undefined
	 */

	undefsm("_start");
}


void
dfltsec()
{

#define M68_ALIGN 512

	long lint = M68_ALIGN; 
	register long *lptr = &lint;
	register ACTITEM *aiptr, *grptr;

/*
 * If any SECTIONS directives have been input, they take priority,
 * and no default action is taken
 */

	if( bldoutsc.head )
		return;

/*
 * Generate a series of action itmes, as if the following had been
 * input to ld:
 *
 *	SECTIONS {
 *		.text : {}
 *		GROUP ALIGN(M68_ALIGN) : {
 *					.data : {}
 *					.bss  : {}
 *					}
 *		 }
 *
 *
 * Steps:
 *	1. Define background variables
 *	2. Process the .text definition
 *	3. Process the GROUP definition
 *	4. Process the .data definition
 *	5. Process the .bss definition
 *
 *
 * The logic used was obtained by simulating the action of the parser.
 */

	curfilnm = "*default.file*";

	lineno = 2;
	aiptr = dfnscngrp( AIDFNSCN, NULL, NULL, NULL );
	aiptr->dfnscn.aisctype = STYP_REG;
	copy( aiptr->dfnscn.ainame, _TEXT, 8 );
	aiptr->dfnscn.aifill = 0;
	aiptr->dfnscn.aifillfg = 0;
	listadd( l_AI, &bldoutsc, aiptr );

	lineno = 3;
	grptr = dfnscngrp( AIDFNGRP, NULL, lptr, NULL );
	copy( grptr->dfnscn.ainame, "*group*", 7);

	lineno = 4;
	aiptr = dfnscngrp( AIDFNSCN, NULL, NULL, NULL );
	aiptr->dfnscn.aisctype = STYP_REG;
	copy( aiptr->dfnscn.ainame, _DATA, 8 );
	aiptr->dfnscn.aifill = 0;
	aiptr->dfnscn.aifillfg = 0;
	listadd( l_AI, &grptr->dfnscn.sectspec, aiptr );

	lineno = 5;
	aiptr = dfnscngrp( AIDFNSCN, NULL, NULL, NULL );
	aiptr->dfnscn.aisctype = STYP_REG;
	copy( aiptr->dfnscn.ainame, _BSS, 8 );
	aiptr->dfnscn.aifill = 0;
	aiptr->dfnscn.aifillfg = 0;
	listadd( l_AI, &grptr->dfnscn.sectspec, aiptr );
	 
	listadd( l_AI, &bldoutsc, grptr );

}

void
procspecobj(fdes, ap)
LDFILE *fdes;
ACTITEM *ap;
{
	/*
	 * For the m68k SGS, the only time this function should
	 * be called is when an illegal input file is encountered.
	 */

	lderror(1, ap->ldlbry.aiinlnno, ap->ldlbry.aiinflnm,
			"file %s is of unknown type: magic number = %06.1x",
			ap->ldlbry.aifilnam, TYPE(fdes));
}

void
adjsize(osp)
OUTSECT *osp;
{
		/*
		 * No action for m68k
		*/
}

long reloc24();

void
adjaout(aout)
AOUTHDR *aout;
{
/*
 * NULL for m68k
 */

}

void
relocate(ifd, rfd, infl, isp, osp, fdes, rdes, paddiff, relocfg, buf)
LDFILE *ifd, *rfd;
INFILE *infl;
INSECT *isp;
OUTSECT *osp;
FILE *fdes, *rdes;
long *paddiff;
int *relocfg;
char *buf;
{
		/*
		 * Process the section
		 */

		int oddflag, numbytes;
		long ndx, rdif, highaddr, vaddiff;
		union {
			long	 l;
			unsigned short u[2];
			short	 i[2];
			char	 c[4];
			} value;
		RELOC rentry;
		SLOTVEC *svp;

		highaddr = isp->ishdr.s_vaddr + isp->ishdr.s_size;
		vaddiff = isp->isnewvad - isp->ishdr.s_vaddr;
		*paddiff = isp->isnewpad - isp->ishdr.s_paddr;
		for( ndx = isp->ishdr.s_vaddr; ndx < highaddr; ) {

			/*
			 * Read a relocation entry, if any left, and
			 * determine the size of the relocation field, in bytes
			 */
			if( *relocfg ) {
				if (fread(&rentry, RELSZ, 1, IOPTR(rfd)) != 1)
					lderror(2,0,NULL,"fail to read the reloc entries of section %s of %s",
					isp->ishdr.s_name,infl->flname);
				(*relocfg)--;
				switch (rentry.r_type) {

				case R_ABS:
					/*
					 * No relocation, so skip to the
					 * next entry
					 */
					if( rflag )
					    if( (svp = svread(rentry.r_symndx)) == NULL ) {
						lderror(1,0,NULL, "reloc entry (%10.1lx, %ld %d) found for non-relocatable symbol, in section %s of file %s",
							rentry, isp->ishdr.s_name, infl->flname);
						goto nextentry;
						}
					    else
						goto nullentry;
					goto nextentry;

				case R_RELBYTE:
				case R_PCRBYTE:
					numbytes = 1;
					break;
				case R_RELWORD:
				case R_PCRWORD:
					numbytes = 2;
					break;

				case R_RELLONG:
					numbytes = 4;
					break;

				default:
					lderror(2,0,NULL,
						"Illegal relocation type %d found in section %s in file %s",
						rentry.r_type, isp->ishdr.s_name, infl->flname);
				}
				}
			/*
			 * There are no (more) relocation entries, so 
			 * copy the rest of the section
			 */
			else {
				while( (rdif = highaddr - ndx) > 0 ) {
#if WINDOW
					rdif = min(SCNHSZ, rdif);
					fwrite(getwrec((int) rdif,IOPTR(ifd)), sizeof(char), (int) rdif, fdes);
#else
					rdif = min(BUFSIZ, rdif);
					if (FREAD(buf,(int) rdif,1,ifd) != 1 ||
					    fwrite(buf,(int) rdif,1,fdes) != 1)
						lderror(2,0,NULL,"fail to copy the rest of section %s of file %s",
						isp->ishdr.s_name,infl->flname);
#endif
					ndx += rdif;
					}
				continue;
				}

			/*
			 * Copy bytes that need no relocation. Copy only
			 * an even number of bytes
			 */
			oddflag = 0;
			while( (rdif = rentry.r_vaddr - ndx) > oddflag ) {
#if WINDOW
				rdif = min(SCNHSZ, rdif);
#else
				rdif = min(BUFSIZ, rdif);
#endif
				if( (oddflag = rdif % 2) ) {
					numbytes += 2;
					if( rdif-- == 1 )
						break;
					}
#if WINDOW
				fwrite(getwrec((int) rdif, IOPTR(ifd)), sizeof(char), (int) rdif, fdes);
#else
				if (FREAD(buf,(int) rdif,1,ifd) != 1 ||
				    fwrite(buf,(int) rdif,1,fdes) != 1)
					lderror(2,0,NULL,"fail to copy the bytes that need no reloc of section %s of file %s",
					isp->ishdr.s_name,infl->flname);
#endif
				ndx += rdif;
				}
			if( rdif < 0 ) {
				lderror(2,0,NULL, "Reloc entries out of order in section %s of file %s",
					isp->ishdr.s_name, infl->flname);
				}

			/*
			 * Read and process the field to be relocated
			 */
			if (fread(&value.l, sizeof(char), numbytes, IOPTR(ifd)) != numbytes)
				lderror(2,0,NULL,"fail to read the field to be relocated of section %s of file %s",
				isp->ishdr.s_name,infl->flname);

			if( (svp = svread(rentry.r_symndx)) == NULL ) {
				lderror(1,0,NULL, "No reloc entry found for symbol: index %ld, section %s, file %s",
					rentry.r_symndx, isp->ishdr.s_name, infl->flname);
					goto nextentry;
					}

			rdif = svp->svnvaddr - svp->svovaddr;

			/*
			 * Special processing for COPY sections:
			 *  do not touch the raw data (no relocation)
			 *  but process relocation entries and lineno
			 *  entries as usual
			 */

			if ( osp->oshdr.s_flags & STYP_COPY )
				goto copysect;

			switch( rentry.r_type ) {

			case R_RELLONG:

				/* In some cases, "value.l", the relocatable
				   field read from an input file, has a value
				   other than zero. This must be swabbed before
				   being used internally. */

				value.l = swapb4(value.l, 0);

				if(value.l >= 0x1000000)
					lderror(2,0,NULL,"32-Bit TV relocation value is out of bounds! ( > 16Mbytes)\n");

				/* Now, after making sure the 32-bit value
				   does not have any more than the lower
				   24 bits utilized, it is time to calculate
				   the relocated field, swab it, and write
				   it to the output file. NOTE that if 
				   seperate I&D are implemented in the
				   hardware, it may be necessary to turn on
				   the 25th bit (bit position 24). */

				value.l = swapb4(value.l + rdif , 0);
				break;

			}

	copysect:	ndx += numbytes;
			fwrite(&value.l, numbytes, 1, fdes);

	nullentry:	/* 
			 * Preserve relocation entries
			 */
			if( rflag ) {
				rentry.r_vaddr += vaddiff;
				rentry.r_symndx = svp->svnewndx;
				fwrite(&rentry, RELSZ, 1, rdes);
				}

	nextentry:	;
		}

}

long
reloc24(da, reval, signflag)
long da,		/* da field containing the 24-bit value */
     reval;		/* relocation value to be added to da field */
char signflag;		/* flag set when da field is to be treated as signed
			   quantity */
{
	return( da );
}

long
swapb4(val, type)
long val;
int type;
{
	register long result;

/*
 * Convert long to/from VAX/11/70 format, if necessary
 *
 *	val	: w x y z
 *
 *	vax	: y z w x
 *
 */

	if (type) 
		return(val);
	else {
#if AR32WR && !defined(m68k)
		result = (val >> 16) & 0x0000FFFFL;
		result |=(val << 16) & 0xFFFF0000L;
		return(result);
#else
		return(val);
#endif
		}


}


unsigned short
swapb2(val)
unsigned short val;
{

	/* If this link editor is built for a vax or an 11/70, swap
	   bytes within shorts. */

#if (AR32WR || AR16WR) && !defined(m68k)
	unsigned short result;

	result = (val >> 8) & 0x00ff;
	result |= ( (val << 8) & 0xff00);
	return(result);
#else
	return(val);
#endif
}



void
procext(tvndx, secnum, sltval1, sltval2, ovaddr, nvaddr, gsp, sm)
int *tvndx, secnum;
long *sltval1, *sltval2, *ovaddr, *nvaddr;
register SYMTAB *gsp;
SYMENT *sm;
{
			if( (*tvndx = tvslot(gsp, NULL,NULL, -1)) != -1 ) {
				*sltval1 = 0L;
				*sltval2 = *tvndx * TVENTSZ;
				}
			else  {
				*sltval1 = sm->n_value;
				*sltval2 = gsp->smnewvirt;
				}
}

void
proclocstat(sm, infd, segment, newscnum, tvndx, ovaddr, nvaddr, opaddr, npaddr, ndx, sltval1, sltval2, count)
SYMENT *sm;
LDFILE *infd;
int *segment, *newscnum, *tvndx;
long *ovaddr, *nvaddr, *opaddr, *npaddr, ndx, *sltval1, *sltval2, count;
{

			if( (*tvndx = tvslot(NULL, sm, IOPTR(infd), 0)) != -1 ) {
				*sltval1 = 0L;
				*sltval2 = (long) *tvndx * TVENTSZ;
				}
			else {
				*sltval2 = sm->n_value;
				*sltval1 = *sltval2 - (npaddr[sm->n_scnum] - opaddr[sm->n_scnum]);
				}
			svcreate(count, *sltval1, *sltval2, ndx, newscnum[sm->n_scnum],
				(*tvndx != -1) ? SV_TV : 0);

}

#if TRVEC

void
setslot(pglob, psect, outslot)
SYMTAB *pglob;
SCNHDR *psect;
TVENTRY *outslot;
{

	char tmpchar;

		outslot->tv_addr = pglob->smnewval;


#if AR32WR && !defined(m68k)
/*
 * Vax and 11/70 differ in byte ordering of longs
 */

	tmpslot.l = outslot->tv_addr;

	tmp_us = tmpslot.u[0];
	tmpslot.u[0] = tmpslot.u[1];
	tmpslot.u[1] = tmp_us;
	outslot->tv_addr = tmpslot.l ;

#endif

#if (AR16WR || AR32WR) && !defined(m68k)
/* If being run on a Vax or an 11/70, swap bytes within shorts. */

	tmpslot.l = outslot->tv_addr;
	tmpchar = tmpslot.c[0];
	tmpslot.c[0] = tmpslot.c[1];
	tmpslot.c[1] = tmpchar;


	tmpchar = tmpslot.c[2];
	tmpslot.c[2] = tmpslot.c[3];
	tmpslot.c[3] = tmpchar;
	outslot->tv_addr = tmpslot.l;

#endif
}

void
slotassign(tvent, pglob)
TVENTRY *tvent;
SYMTAB *pglob;
{
	long	paddr;
	char	tmpchar;



	paddr = pglob->smnewval;

	tvent->tv_addr = paddr;

#if AR32WR && !defined(m68k)
	/*
	 * VAX and 11/70 differ in byte ordering of longs
	 */

	tmpslot.l = tvent->tv_addr;
	tmp_us = tmpslot.u[0];
	tmpslot.u[0] = tmpslot.u[1];
	tmpslot.u[1] = tmp_us;
	tvent->tv_addr = tmpslot.l;
#endif

#if (AR16WR || AR32WR) && !defined(m68k)

/* If being done on a Vax or an 11/70, swap bytes within shorts. */

	tmpslot.l = tvent->tv_addr;
	tmpchar = tmpslot.c[0];
	tmpslot.c[0] = tmpslot.c[1];
	tmpslot.c[1] = tmpchar;


	tmpchar = tmpslot.c[2];
	tmpslot.c[2] = tmpslot.c[3];
	tmpslot.c[3] = tmpchar;


	tvent->tv_addr = tmpslot.l;

#endif

}

void
settventry(tvent, psym)
TVENTRY *tvent;
struct syment *psym;
{

	char tmpchar;


		tvent->tv_addr = psym->n_value;

#if AR32WR && !defined(m68k)
	/*
	 * VAX and 11/70 differ in byte ordering of longs
	 */

	tmpslot.l = tvent->tv_addr;
	tmp_us = tmpslot.u[0];
	tmpslot.u[0] = tmpslot.u[1];
	tmpslot.u[1] = tmp_us;
	tvent->tv_addr = tmpslot.l;
#endif


#if (AR16WR || AR32WR) && !defined(m68k)

/* If being done on a Vax or 11/70, swap bytes within shorts. */

	tmpslot.l = tvent->tv_addr;
	tmpchar = tmpslot.c[0];
	tmpslot.c[0] = tmpslot.c[1];
	tmpslot.c[1] = tmpchar;

	tmpchar = tmpslot.c[2];
	tmpslot.c[2] = tmpslot.c[3];
	tmpslot.c[3] = tmpchar;


	tvent->tv_addr = tmpslot.l;

#endif


}

void
filltvassign(psym)
SYMTAB *psym;
{

	char tmpchar;

			tvspec.tvfill.tv_addr = psym->smnewval;

#if (AR16WR || AR32WR) && !defined(m68k)
	
/* If being done on a Vax or 11/70, swap bytes within shorts. */

	tmpslot.l = tvspec.tvfill.tv_addr;

	tmpchar = tmpslot.c[0];
	tmpslot.c[0] = tmpslot.c[1];
	tmpslot.c[1] = tmpchar;


	tmpchar = tmpslot.c[2];
	tmpslot.c[2] = tmpslot.c[3];
	tmpslot.c[3] = tmpchar;

	tvspec.tvfill.tv_addr = tmpslot.l;

#endif

}

#endif
