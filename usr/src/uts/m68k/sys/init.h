/*	@(#)init.h	2.1	 */
extern int clkstart(),cinit(),binit(),errinit(),iinit(),inoinit();
extern int finit();
#ifdef X25_0
extern x25init();
#endif
#ifdef ST_0
extern	stinit();
#endif
#ifdef	VPM_0
extern	vpminit();
#endif

/*	Array containing the addresses of the various initializing	*/
/*	routines executed by "main" at boot time.			*/

int (*init_tbl[])() = {
	inoinit,
	clkstart,
	cinit,
	binit,
	errinit,
	finit,
	iinit,
#ifdef	VPM_0
	vpminit,
#endif
#ifdef X25_0
	x25init,
#endif
#ifdef ST_0
	stinit,
#endif
	0
};
