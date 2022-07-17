/* @(#)sysacct.c	2.1	 */
#include <sys/types.h>

/*
 * output accounting info
 */
sysacct(bytes, time)
time_t time;
long bytes;
{

	/*
	 * shuts lint up
	 */
	if (time == bytes)
	return;
}
