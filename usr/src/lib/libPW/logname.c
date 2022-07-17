/* @(#)logname.c	2.1		*/
char *
logname()
{
	return((char *)getenv("LOGNAME"));
}
