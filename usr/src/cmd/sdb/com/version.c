	/*	@(#)version.c	2.1		*/

version() {
#ifdef u3b
	error("BELL 3B-20 sdb Version 5.0");
#endif
#ifdef vax
	error("VAX-11/780 sdb Version 5.0");
#endif
#ifdef m68k
	error("MC68000  sdb");
#endif
}
