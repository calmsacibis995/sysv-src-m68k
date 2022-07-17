/*	@(#)special1.c	2.1.1.1		*/
static char spec1_ID[] = "@(#) special1.c:	2.1 83/07/08";

#include <stdio.h>

#include "system.h"


static char tvreferr[] = "tv reference to non-tv symbol: addr %0.1lx, index %ld, file %s";

#if TRVEC
void
chktvorg(org, tvbndadr)

long	org, *tvbndadr;
{
	/*
	 * check user-supplied .tv origin for legality
	 *  if illegal, side-effect tvspec.tvbndadr
	 *   and issue warning message
	 *	FOR m68k, origin can be anywhere. 
	 */

	if((org % 0x04L) != 0)
		yyerror("supplied tv origin (%10.11x) must be on a 4-byte boundary!",org);


	*tvbndadr = org;

}

#endif

void
specflags(flgname, argptr)
char *flgname;
char **argptr;
{

/*
 * process special flag specifications for m68k
 * these flags have fallen through switch of argname in ld00.c
 */

	switch ( *flgname ) {

		/* ignore -n flag to stay compatible with PDP-11 ld */
		case 'n':
			break;

		default:
			yyerror("unknown flag: %s", flgname);

		}
}
