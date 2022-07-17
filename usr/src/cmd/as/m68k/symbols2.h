/*	@(#)symbols2.h	2.1		*/
/*	static	char	sccsid[] = "@(#) symbols2.h: 2.1 6/4/83";			*/


/*
 *	NINSTRS	= number of legal assembler instructions (from ops.out)
 *	NHASH	= the size of the hash symbol table
 *	NSYMS	= the number of symbols allowed in a user program
 */

#define	NINSTRS	166

#define NHASH	4001

#define NSYMS	(NHASH - NINSTRS - 1)
