char xxxsccsid[] = "@(#)logname.c	2.1	";

#include <stdio.h>
main() {
	char *name, *cuserid();

	name = cuserid((char *)NULL);
	if (name == NULL)
		return (1);
	(void) puts (name);
	return (0);
}
