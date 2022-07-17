/* @(#)prefix.c	2.1	 */
#include "uucp.h"

/*
 * check s for prefix pre
 *	pre	-> prefix string
 *	s	-> string to be checked
 * returns:
 *	FALSE	-> no match
 *	TRUE	-> match
 */
prefix(pre, s)
register char *pre, *s;
{
	register char c;

	while ((c = *pre++) == *s++)
		if (c == '\0')
			return(TRUE);
	return((c == '\0')?TRUE:FALSE);
}
