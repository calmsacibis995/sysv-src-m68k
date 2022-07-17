/*	@(#)system.h	2.1		*/
static char sysh_ID[] = "@(#) system.h:	2.1 83/07/08";

#define	USEREGIONS	0	/* allows allocation by regions */
#define	TRVEC	0		/* allows transfer vectors	*/
#define	UNIX	1		/* ld used as standard UNIX tool*/
#define	DMERT	0		/* ld used to generate DMERT obj*/
#define	B16	0		/* ld generates basic-16 object	*/
#define X86	0		/* ld generates extended 8086	*/
#define	N3B	0		/* ld generates 3B-20 object	*/
#define MC68	1		/* ld generates MC68 object	*/
#define	M32	0		/* ld generates BELLMAC-32 obj	*/
#define U3B 	0		/* ld generates 3B-20Simplex obj*/
#define	VAX	0		/* ld generates VAX 11/780 obj	*/
#define	PDP	0		/* ld generates PDP 11/70  obj	*/
#define	DEC	0		/* ld generates DEC object	*/

#ifndef DEBUG			/* permit command line override */
#define	DEBUG	0		/* allow debugging flag		*/
#endif

#define	IANDD	0		/* allows separate i and d	*/
#define SWABFMT	0		/* text in reverse order 	*/
#define COMMON	1		/* allow comm in .o files	*/
#define ILDOPT	0		/* can generate .i_l_dxx (extra) sections */

#ifndef TS
#define	TS		1		/* ld executed under UNIX-TS	*/
#endif

#ifndef RT
#define	RT		0		/* ld executed under UNIX-RT	*/
#endif

#ifndef NOSDP
#define	NOSDP		0		/* use Software Demand Paging	*/
#endif

#ifndef ONEPROC
#define	ONEPROC		0		/* defines number of processes	*/
#endif

#ifndef	PORT5AR
#define PORT5AR		0		/* 5.0 archive format 	  */
#endif

#ifndef PORTAR
#define PORTAR		0		/* defines archive format */
#endif
