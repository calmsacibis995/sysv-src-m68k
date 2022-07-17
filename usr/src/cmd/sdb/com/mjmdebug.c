	/*	@(#)mjmdebug.c	2.1		*/

#include "head.h"
#include "coff.h"
/*
 * debugging for sdb
 */

prstentry(stp)	/* debug */
register struct syment *stp;
{
#if DEBUG > 1
	fprintf(FPRT2, "debug: print symbol table entry:\n");
#ifdef FLEXNAMES
	if(stp->n_zeroes)
		fprintf(FPRT2, "	n_name=  %.14s;\n", stp->n_name);
	else
		fprintf(FPRT2, "	n_name=  %s;\n", stp->n_offset);
#else
	fprintf(FPRT2, "	n_name=  %.14s;\n", stp->n_name);
#endif
	fprintf(FPRT2, "	n_value= %#lx;\n", stp->n_value);
	fprintf(FPRT2, "	n_scnum= %d;\n", (int)stp->n_scnum);
	fprintf(FPRT2, "	n_type=  %#x;\n", (int)stp->n_type);
	fprintf(FPRT2, "	n_sclass=%d;\n", (int)stp->n_sclass);
	fprintf(FPRT2, "	n_numaux=%d;\n", (int)stp->n_numaux);
#endif
}

prprocaux(axp, name)	/* debug */
AUXENT *axp;
char *name;
{
#if DEBUG > 1
#define AX	axp->x_sym
#define AXF	AX.x_fcnary
	fprintf(FPRT2, " proc auxent for %s:\n", name);
	fprintf(FPRT2, "	x_tagndx= 0x%8.8lx\n", AX.x_tagndx);
	fprintf(FPRT2, "	x_fsize=  0x%8.8lx\n", AX.x_misc.x_fsize);
	fprintf(FPRT2, "	x_lnnoptr=0x%8.8lx\n", AXF.x_fcn.x_lnnoptr);
	fprintf(FPRT2, "	x_endndx= 0x%8.8lx\n", AXF.x_fcn.x_endndx);
#endif
}

#ifdef m68k
prprstk()	/* debug */
{
#if DEBUG > 1
	register int n, i;
	fprintf(FPRT2, "((struct user *)uu)->u.ar0 = %#x\n",
		((struct user *)uu)->u_ar0);
	fprintf(FPRT2, "debug process stack:\n");
	fprintf(FPRT2, "	SDBREG(PC) = %#x\n", SDBREG(PC));

	fprintf(FPRT2, "	SDBREG(PS) = %#x\n", SDBREG(PS));

#endif
}
#else
prprstk()	/* debug */
{
#if DEBUG > 1
	register int n, i;
	fprintf(FPRT2, "((struct user *)uu)->u.ar0 = %#o\n",
		((struct user *)uu)->u_ar0);
	fprintf(FPRT2, "debug process stack:\n");
	fprintf(FPRT2, "	SDBREG(PC) = %#o\n", SDBREG(PC));

#ifdef u3b
	fprintf(FPRT2, "	SDBREG(PS) = %#o\n", SDBREG(PS));
	fprintf(FPRT2, "	SDBREG(PSBR) = %#o\n", SDBREG(PSBR));
	fprintf(FPRT2, "	SDBREG(SSBR) = %#o\n", SDBREG(SSBR));

#else
	fprintf(FPRT2, "	SDBREG(PS) = %#o\n", SDBREG(PS));
#endif

#endif
}
#endif
