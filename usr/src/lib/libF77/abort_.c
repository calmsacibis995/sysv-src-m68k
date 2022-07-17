/*	@(#)abort_.c	2.1 	*/
#include <stdio.h>

abort_()
{
fprintf(stderr, "Fortran abort routine called\n");
_cleanup();
abort();
}
