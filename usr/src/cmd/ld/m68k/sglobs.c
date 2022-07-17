/*	@(#)sglobs.c	2.1		*/
static char sglob_ID[] = "@(#) sglobs.c:	2.1 83/07/08";

#include <stdio.h>

#include "system.h"
#include "structs.h"
#include "tv.h"
#include "ldtv.h"

char	Xflag = UNIX;      /* generate optional header with "old" */
/*eject*/
/*
 * Structure of information needed about the transfer vector (and the
 * .tv section).  Part of this structure is added to outsclst, so that
 * the list must not be freed before the last use of tvspec.
 */

TVINFO	tvspec = {
	NULL,		/* tvosptr */
	"",		/* tvfnfill: fill name for tv slots	*/
	-1L,		/* tvfill: fill value for tv slots	*/
	NULL,		/* tvinflnm: file containing tv specs	*/
	0,		/* tvinlnno: line nbr of tv directive	*/
	0,		/* tvlength: tv area length		*/
	-1L,		/* tvbndadr: tv area bond address	*/
	0, 0		/* tvrange				*/
	};

unsigned short magic = (unsigned short) MC68MAGIC;
