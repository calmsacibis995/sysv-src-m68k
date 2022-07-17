	/*	@(#)coff.h	2.1		*/

/*		common object file format #include's */

/*	requires a.out.h (which is #included by head.h) */
#include "sgs.h"
#ifdef u3b
#define OMAGIC	AOUT1MAGIC	/* combined text and data segments */
#define NMAGIC	AOUT2MAGIC	/* separate text and data segments */

#else
#define OMAGIC	AOUT2MAGIC	/* combined text and data segments */
#define NMAGIC	AOUT1MAGIC	/* separate text and data segments */
#endif

/*	 added for convenience */
#define ISTELT(c)	((c==C_MOS) || (c==C_MOU) || (c==C_MOE) || (c==C_FIELD))
#define ISREGV(c)	((c==C_REG) || (c==C_REGPARM))
#define ISARGV(c)	((c==C_ARG) || (c==C_REGPARM))
#define ISTRTYP(c)	((c==T_STRUCT) || (c==T_UNION) || (c==T_ENUM))
#define ISSTTAG(c)	((c==C_STRTAG) || (c==C_UNTAG) || (c==C_ENTAG))

#define MAXAUXENT	1	/* max number auxilliary entries */
