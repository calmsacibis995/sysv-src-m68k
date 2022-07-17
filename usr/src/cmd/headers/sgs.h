/*	@(#)sgs.h	2.1		*/

/*	The symbol MC68MAGIC is defined in filehdr.h	*/

#ifndef MAGIC
#define MAGIC	MC68MAGIC
#endif

#define ISMAGIC(x)	(((unsigned short)x)==(unsigned short)MAGIC)
#ifdef ARTYPE	
#define ISARCHIVE(x)	((((unsigned short)x)==(unsigned short)ARTYPE))
#define BADMAGIC(x)	((((x)>>8) < 7) && !ISMAGIC(x) && !ISARCHIVE(x))
#else
#define BADMAGIC(x)	((((x)>>8) < 7) && !ISMAGIC(x))
#endif


/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default : readonly sharable text segment
 *		AOUT2MAGIC:	     : writable text segment
 */

#define AOUT1MAGIC 0410
#define AOUT2MAGIC 0407

#define	SGSNAME	""
#define SGS ""
#define RELEASE "Release 5.0 sgs.h"
