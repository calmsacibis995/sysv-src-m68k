/*	@(#)getargs.c	2.1		*/
static char ID[] = "@(#) getargs.c: 1.4 3/2/83";

#include	<stdio.h>
#include	"sgsmacros.h"
#include	"filehdr.h"
#include	"conv.h"

unsigned short	target,
		bootable;
		origin;
unsigned short	ar_target = ASCIIAR;
int		preswab;

extern char	*filev[],
		*malloc();

int
getargs(argc,argv)

register int	argc;
register char	**argv;
{
	char	fbuf[BUFSIZ];
	register int	filec;
	register char	*machine;
	char arch = 0;

	filec = 0;

	while (--argc > 0) {
		++argv;
		if (argv[0][0] == '-') switch (argv[0][1]) {
			case '\0':
				while ( fscanf(stdin, "%s", fbuf) != EOF
						&& filec < MAXFILES ) {
					filev[filec] = malloc(strlen(fbuf)+1);
					if ( filev[filec] == NULL ) {
						converr(1,"***command line***",
								"out of space");
						return(-1);
						}
					strcpy(filev[filec++], fbuf);
					}

				if ( filec == MAXFILES ) {
					converr(1,"***command line***",
							"too many files");
					return(-1);
					}
				break;
			case 'b':
				/* -b means that a 'bootable' a.out
				 * will be produced: only the text and data
				 * segments are written, and the output is
				 * null-padded to a multiple of 1024 bytes.
				 */
				if ( arch ) {
					converr(1,"***command line***",
						"'bootable' & 'archive' inconsistent");
					return(-1);
					}
				bootable++;
				break;
			case 's':
				/* sgsconv -s replaces swab
				 * this means that the user
				 * intends to swab-dump onto
				 * the target machine so only
				 * preswab chars here
				 */
				preswab++;
				break;

			case 't':
				if ( target != 0) {
					converr(1,"***command line***",
						"only one target flag allowed");
					return(-1);
					}
				++argv;
				--argc;
				if (gettarget(machine = *argv)) {
					converr(1,"***command line***",
						  "unknown target %s", machine);
					return(-1);
					}
				break;

			case 'p':
				/* output file should be in portable (5.0) archive format */
				if ( bootable ) {
					converr(1,"***command line***",
						"'bootable' & 'archive' inconsistent");
					return(-1);
					}
				arch++;
				ar_target = RANDOM;
				break;

			case 'o':
				/* output file should be in old (pre 5.0) archive format */
				if ( bootable ) {
					converr(1,"***command line***",
						"'bootable' & 'archive' inconsistent");
					return(-1);
					}
				arch++;
				ar_target = OLD;
				break;

			case 'a':
				/*
				 * output file should be in portable (6.0 ascii
				 * headers) archive format
				 */
				if ( bootable ) {
					converr(1,"***command line***",
						"'bootable' & 'archive' inconsistent");
					return(-1);
					}
				arch++;
				ar_target = ASCIIAR;
				break;

			default:
				converr(1,"***command line***",
					"unknown flag (%c)",argv[0][1]);
				return(-1);
				break;
		} else {
			if ( filec == MAXFILES ) {
				converr(1,"***command line***","too many files");
				return(-1);
				}
			filev[filec++] = *argv;
			}
	} /* while */

	if (target == 0)
		gettarget(machine = TDEFAULT);

	if ((ar_target != OLD) && ( equal( machine, "pdp", 3 ) || equal( machine, "ibm", 3 ) || equal( machine, "i80", 3 ))) {
		converr(0, "***command line***", "portable archive format not supported on %s", machine );
		ar_target = OLD;
	}

	return(filec);
} /* getargc */

int gettarget(mach)
register char *mach;
{
	if ( IS32W(mach) || IS16W(mach) )
		target = F_AR32W;
	else if ( IS32WR(mach) )
		target = F_AR32WR;
	else if ( IS16WR(mach) )
		target = F_AR16WR;
	else
		return(1);
	return(0);
} /* gettarget */
