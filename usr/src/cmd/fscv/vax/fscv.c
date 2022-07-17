/* @(#)fscv.c	2.1		*/
main(argc, argv)
int argc;
char *argv[];

{
	/*
	 * use: fscv -v|p input [output]
	 * use: fscv -v|m input [output] (m68k version)
	 *
 	 *	-v	convert to VAX file structure from 11
 	 *	-p	convert to PDP 11 file structure from VAX
	 *
	 *	-v	convert to VAX file structure from MC68000 family (m68k)
	 *	-m	convert to MC68000 file structure from VAX (m68k)
	 *
	 *	input	input file structure (from file)
	 *	output 	output file structure (to file)
	 *	Note:	If output file is not specified an in-place update is done
	 */

	/*
 	 * This program converts file systems between VAX and PDP 11s.
	 * This program converts file systems between VAX and MC68000 family. (m68k)
	 * Only the file structure is converted, not the data included in
	 * any files.  This conversion is due to the difference in internal
 	 * storage of Long integers and boundary alignments on the VAX.
	 * This conversion is due to the difference in internal (m68k)
	 * storage of integers and longs on the VAX and the MC68000 family. (m68k)
	 */


	/*
	 * The file system contains no information concerning the type of
	 * machine on which it was created.  A rudimentary check of the
	 * file structure is made after exmaning the command line.
	 * The operator is questioned if conversion appears risky.
	 * The conversion should never be made from a mounted file system as
	 * any file activity on the input file will in all probability destroy
	 * the output file.
	 */

#include "params.h"
#include <stdio.h>

	/***** data *****/
	long numbks;
	int fdi,fdo,tovax,notinp,rc;

	/* m68k
	 * The following two structures are hardcoded copies of the (m68k)
	 * superblocks for the VAX and MC68000 family machines with (m68k)
	 * longs = short[2] * and boundary alignments included so compilation (m68k)
	 * will be * independent of the sort of machine. (m68k)
	 */

	/*
 	 * The following two structures are hardcoded copies of the superblocs
 	 * for the VAX and PDP 11 machines with longs = short[2] and boundary
 	 * alignments included so compilation will be independent of the sort of 
 	 * machine.
	 */

	/*
	 * The subroutines superv and superm load the following two (m68k)
	 * buffers with the VAX and MC68000 super blocks respectively. (m68k)
	 */

	/*
 	 * The subroutines superv and superp load the following two buffers with
 	 * the VAX and PDP 11 super blocks respectively.
	 */

#if m68k || MC68
#include "super68.h"
#else
#include "super16.h"
#endif
#include "super32.h"
#if m68k || MC68
	static union un1 {
		struct  mc68k s1;
		char super68[512];
	} 
	sup68;
#else
	static union un1 {
 		struct pdp16 s1;
 		char super16[512];
	} 
 	sup16;
#endif

	static union un2 {
		struct pdp32 s2;
		char super32[512];
	} 
	sup32;


	/***** program *****/

	/* initialization section */

	/* check command line */

	if (*argv[1] != '-' || *(argv[1]+2) != 0) {
#if m68k || MC68
		fprintf(stderr,"Use : fscv -v|m input [output]\n");
#else
 		fprintf(stderr,"Use : fscv -v|p input [output]\n");
#endif
		exit(1);
	}
	/*
	 * See if an in-place update is being done
	 */

	if(argc == 3) notinp = 0;
	else if(argc == 4) notinp = 1;
	else {
#if m68k || MC68
		fprintf(stderr,"Use : fscv -v|m input [output]");
#else
 		fprintf(stderr,"Uset - fscv -v|p input [output]");
#endif
		exit(1);
	}

	/*
	 * Set variable tovax to true if converting to a VAX file structure
	 * else set to false making sure -v or -m was used. (m68k)
 	 * else set to false making sure -v or -p was used.
	 */
	if (tovax = *(argv[1]+1) == 'v') ;
#if m68k || MC68
	else if (*(argv[1]+1) != 'm' ) {
#else
 	else if (*(argv[1]+1) != 'p' ) {
#endif
		fprintf(stderr,"Flag error: -v converts to VAX\n");
#if m68k || MC68
		fprintf(stderr,"            -m converts to MC68000\n");
#else
 		fprintf(stderr,"            -p converts to PDP 11\n");
#endif
		exit(1);
	};


	/* Open input and output files */

	fdi = open(argv[2],0);
	if (fdi == -1) {
		perror("Error opening input file");
		exit(1);
	};
	fdo = open(argv[2+notinp],2);
	if (fdo == -1) {
		perror("Error opening output file");
		exit(1);
	};

	/* Check to see if conversion should be done */
	ckmnt(argv[2]);
	if(notinp) ckmnt(argv[3]);

	/* Read the super block in two formats  */
	tovax ?
#if m68k || MC68
	(superv(fdi,sup68.super68,sup32.super32)) :
	(superm(fdi,sup68.super68,sup32.super32)) ;
#else
 	(superv(fdi,sup16.super16,sup32.super32)) :
 	(superp(fdi,sup16.super16,sup32.super32)) ;
#endif

	/*
	 * If doing an in-place update just convert necessary blocks, else
	 * copy entire file system first.
	 */
	if(notinp) {
		/* 
		 * Reposition file to beginning
		 */
		lseek(fdi,(long)0,0);
		printf("Copying file system\n");
#ifdef vax
		numbks = *(long *)sup32.s2.b32_fsize;
#else
#ifdef m68k
		numbks = *(long *)sup68.s1.b68_fsize;
#else
 		numbks = *(long *)sup16.s1.b16_fsize;
#endif
#endif
		while(numbks-BUFNOS > 0) {
			cpyblk(BUFNOS,fdi,fdo);
			numbks -= BUFNOS;
		}
		cpyblk((int)numbks,fdi,fdo);
	}

	/*
	 * Write the proper super block to disk
	 */
	lseek(fdo,(long)512*1,0);
	tovax ?
	(rc = write(fdo,sup32.super32,512)) :
#if m68k || MC68
	(rc = write(fdo,sup68.super68,512)) ;
#else
 	(rc = write(fdo,sup16.super16,512)) ;
#endif
	if(rc < 512) perror("Error writing super block");

	/* Convert the free list */
 	/* The first indirect block number must be passed as a long integer */
	printf("Converting free list\n");
#ifdef vax
	tovax ?
 	indfv(fdo,&sup32.s2.b32_free[0]) :
#ifdef MC68
 	indfm(fdo,&sup32.s2.b32_free[0]) ;
#else
 	indfp(fdo,&sup32.s2.b32_free[0]) ;
#endif
#else
	tovax ?
#ifdef m68k
	indfv(fdo,&sup68.s1.b68_free[0]) :
	indfm(fdo,&sup68.s1.b68_free[0]) ;
#else
 	indfv(fdo,&sup16.s1.b16_free[0]) :
 	indfp(fdo,&sup16.s1.b16_free[0]) ;
#endif
#endif

	/* Convert the indodes */
	/*
	 * Isize is passed as the third argument so the number of blocks
	 * containing inodes can be calculated.
	 */
	printf("Converting Inodes\n");
#ifdef vax
	cnvino(tovax,fdo,sup32.s2.b32_isize);
#else
#ifdef m68k
	cnvino(tovax,fdo,sup68.s1.b68_isize);
#else
 	cnvino(tovax,fdo,sup16.s1.b16_isize);
#endif
#endif
	/*
	 * Sync in case block special device converted
	 */
	sync();

}
