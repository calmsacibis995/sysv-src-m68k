/*	@(#)params.h	2.1		*/
static char paramsh_ID[] = "@(#) params.h:	2.1 83/07/08";

/***********************************************************************
*   This file contains system dependent parameters for the link editor.
*   There must be a different params.h file for each version of the
*   link editor (e.g. b16, n3b, mac80, etc.)
*
*   THIS COPY IS FOR MC68000
*********************************************************************/


/*
 * Maximum size of a section
 */
#define MAXSCNSIZE	0x1000000L
#define MAXSCNSZ	MAXSCNSIZE


/*
 * Default size of configured memory
 */
#define MEMORG		0x0L
#define MEMSIZE		0x1000000L

/*
 * Size of a region. If USEREGIONS is zero, the link editor will NOT
 * permit the use of REGIONS, nor partition the address space
 * USEREGIONS is defined in system.h
 */
#define REGSIZE 	0x0L

/*
 * Alignment for commons
 */

#define	COM_ALIGN	0x3L		/* double word boundary */

/*
 * Names of special symbols
 */
#define _CSTART	"_start"
#define _MAIN	"main"
