/*	@(#)user.c	2.12		*/
#ifdef m68k
#include "sys/types.h"
#include "sys/param.h"
#include "sys/buf.h"

extern	caddr_t	mmumap();


/*
	This file contains the routines used by UNIX to 
	access the current user area. They used to be in
	machine language prior to the introduction of the
	MMU driver interface. If possible, they should be
	written in machine language for performance reasons.
*/
#define	OK	0
#define	ERROR	(-1)

/*
	fubyte - fetch a byte from user data space
*/

fubyte (address)
caddr_t address;
{
	register unsigned char	*cp;
	if (( cp = (unsigned char *)mmumap (address, sizeof(char), B_READ)) == NULL)
		return (ERROR);
	return ((int) *cp);
}




/*
	fuibyte - fetch a byte from user instruction space
*/

fuibyte (address)
caddr_t address;
{
	register unsigned char	*cp;
	if (( cp = (unsigned char *)mmumap (address, sizeof(char), B_READ)) == NULL)
		return (ERROR);
	return ((int) *cp);
}




/*
	fuword - fetch a word from user data space
*/

fuword (address)
caddr_t address;
{
	register int	*ip;
	if ((( ip = (int *)mmumap (address, sizeof(int), B_READ)) == NULL)
	   ||( (int)ip&1 ))
		return (ERROR);
	return (*ip);
}





/*
	fuiword - fetch a word from user instruction space
*/

fuiword (address)
caddr_t address;
{
	register int	*ip;
	if ((( ip = (int *)mmumap (address, sizeof(int), B_READ)) == NULL)
	   ||( (int)ip&1 ))
		return (ERROR);
	return (*ip);
}


/*
	subyte - store a byte into user data space
*/

subyte (address, byte)
caddr_t address;
char byte;
{
	register char	*cp;
	if (( cp = (char *)mmumap (address, sizeof(char), B_WRITE)) == NULL)
		return (ERROR);
	*cp = byte;
	return(OK);
}





/*
	suibyte - store a byte into user instruction space
*/

suibyte (address, byte)
caddr_t address;
char byte;
{
	register char	*cp;
	if (( cp = (char *)mmumap (address, sizeof(char), B_WRITE)) == NULL)
		return (ERROR);
	*cp = byte;
	return(OK);
}


/*
	suword - store a word into user data space
*/

suword (address, word)
caddr_t address;
int	word;
{
	register int	*ip;
	if ((( ip = (int *)mmumap (address, sizeof(int), B_WRITE)) == NULL)
	   ||( (int)ip&1 ))
		return (ERROR);
	*ip = word;
	return(OK);
}


/*
	suiword - store a word into user instruction space
*/

suiword (address, word)
caddr_t address;
int	word;
{
	register int	*ip;
	if ((( ip = (int *)mmumap (address, sizeof(int), B_WRITE)) == NULL)
	   ||( (int)ip&1 ))
		return (ERROR);
	*ip = word;
	return(OK);
}
#endif
