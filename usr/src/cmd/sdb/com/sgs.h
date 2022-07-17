	/*	@(#)sgs.h	2.1		*/

#if vax
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)VAXROMAGIC) || \
			  (((unsigned short)x)==(unsigned short)VAXWRMAGIC))

/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default : readonly sharable text segment
 *		AOUT2MAGIC:	     : writable text segment
 */

#define AOUT1MAGIC 0410
#define AOUT2MAGIC 0407

#endif
#if m68k
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)MC68MAGIC))

/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default : readonly sharable text segment
 *		AOUT2MAGIC:	     : writable text segment
 */

#define AOUT1MAGIC 0410
#define AOUT2MAGIC 0407

#endif
#if u3b
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)N3BMAGIC) || \
			(((unsigned short)x)==(unsigned short)NTVMAGIC))

/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear on that header:
 *
 *		AOUT1MAGIC : default : combined text and data segments
 *		AOUT2MAGIC :         : separate text and data segments
 */

#define AOUT1MAGIC	0407
#define AOUT2MAGIC	0410
#endif

#define	SGSNAME	""
#define SGS ""
#define RELEASE "Release 5.0 6/1/82"
