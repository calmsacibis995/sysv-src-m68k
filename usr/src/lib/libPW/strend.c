static char Sccsid[]="@(#)strend.c	2.1 ";
char *strend(p)
register char *p;
{
	while (*p++)
		;
	return(--p);
}
