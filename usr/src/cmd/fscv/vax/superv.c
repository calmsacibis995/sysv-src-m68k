/* @(#)superv.c	2.1		*/
superv(fdi,s1,s2)

int fdi /* file descriptor for input */;
#if m68k || MC68
#include "super68.h"
#else
#include "super16.h"
#endif
#include "super32.h"

#if m68k || MC68
struct mc68k *s1;
#else
struct pdp16 *s1;
#endif
struct pdp32 *s2;

{
#include <stdio.h>
	/*
		 * This subroutine converts a MC68000 formatted (m68k)
		 * superblock to a VAX superblock. (m68k)
		 * This subroutine converts a PDP 11/70 formatted superblock to a VAX 
		 * superblock.
		 */

	int condi,i;
	char ans[20];

	/* Position file to superblock */
	lseek(fdi,(long)512*1,0);

	/* Read the super block in the PDP 11/70 format and do conversion */
	/* Read the super block in the MC68000 format and do conversion (m68k) */
	condi = read(fdi,s1,512);
	if (condi <= 0) {
		perror("Error reading super block");
		exit(1);
	};

#if m68k || MC68
	s2->b32_isize = s1->b68_isize;		swaps(&s2->b32_isize);
	s2->b32_fsize[1] = s1->b68_fsize[0];	swaps(&s2->b32_fsize[1]);
	s2->b32_fsize[0] = s1->b68_fsize[1];	swaps(&s2->b32_fsize[0]);
	s2->b32_nfree = s1->b68_nfree;		swaps(&s2->b32_nfree);
	for (i = 0; i < 100; i += 2) {
		s2->b32_free[i] = s1->b68_free[i+1];
		swaps(&s2->b32_free[i]);
		s2->b32_free[i+1] = s1->b68_free[i];
		swaps(&s2->b32_free[i+1]);
	};
	s2->b32_ninode = s1->b68_ninode;	swaps(&s2->b32_ninode);
	for (i = 0; i < 100; i++) {
		s2->b32_inode[i] = s1->b68_inode[i];
		swaps(&s2->b32_inode[i]);
	};
	s2->b32_flock = s1->b68_flock;		swaps(&s2->b32_flock);
	s2->b32_ilock = s1->b68_ilock;		swaps(&s2->b32_ilock);
	s2->b32_fmod = s1->b68_fmod;		swaps(&s2->b32_fmod);
	s2->b32_ronly = s1->b68_ronly;		swaps(&s2->b32_ronly);
	s2->b32_time[1] = s1->b68_time[0];	swaps(&s2->b32_time[1]);
	s2->b32_time[0] = s1->b68_time[1];	swaps(&s2->b32_time[0]);
	for (i = 0; i < 4; i++) {
		s2->b32_dinfo[i] = s1->b68_dinfo[i];
		swaps(&s2->b32_dinfo[i]);
	};
	s2->b32_tfree[1] = s1->b68_tfree[0];	swaps(&s2->b32_tfree[1]);
	s2->b32_tfree[0] = s1->b68_tfree[1];	swaps(&s2->b32_tfree[0]);
	s2->b32_tinode = s1->b68_tinode;	swaps(&s2->b32_tinode);
	for (i = 0; i < 6; i++) {
		s2->b32_fname[i] = s1->b68_fname[i];
		s2->b32_fpack[i] = s1->b68_fpack[i];
	};
	s2->b32_magic[1] = s1->b68_magic[0];		swaps(&s2->b32_magic[1]);
	s2->b32_magic[0] = s1->b68_magic[1];		swaps(&s2->b32_magic[0]);
	s2->b32_type[1] = s1->b68_type[0];			swaps(&s2->b32_type[1]);
	s2->b32_type[0] = s1->b68_type[1];			swaps(&s2->b32_type[0]);
#else
 	/* Converting to vax, make sure alignment words are nonzero */
 	if((s1->b16_fsize[0] || s1->b16_free[1]) == 0) {
 		printf("Input file appears to be in VAX format\n");
 		printf("If input file is already in VAX format;");
 		printf(" conversion will corrupt the output file\n");
 		do {
 			printf("Do you wish to continue? (y/n): ");
 			scanf("%s",ans);
 		} 
 		while (ans[0] != 'y' && ans[0] != 'n');
 		if(ans[0] == 'n') exit(1);
 	}
 	/* Convert data the hard way */
 
 	s2->b32_isize = s1->b16_isize;
 	s2->b32_fsize[1] = s1->b16_fsize[0];
 	s2->b32_fsize[0] = s1->b16_fsize[1];
 	s2->b32_nfree = s1->b16_nfree;
	for (i = 0; i < 100; i += 2) {
 		s2->b32_free[i] = s1->b16_free[i+1];
 		s2->b32_free[i+1] = s1->b16_free[i];
	};
 	s2->b32_ninode = s1->b16_ninode;
 	for (i = 0; i < 100; i++) s2->b32_inode[i] = s1->b16_inode[i];
 	s2->b32_flock = s1->b16_flock;
 	s2->b32_ilock = s1->b16_ilock;
 	s2->b32_fmod = s1->b16_fmod;
 	s2->b32_ronly = s1->b16_ronly;
 	s2->b32_time[1] = s1->b16_time[0];
 	s2->b32_time[0] = s1->b16_time[1];
 	for (i = 0; i < 4; i++) s2->b32_dinfo[i] = s1->b16_dinfo[i];
 	s2->b32_tfree[1] = s1->b16_tfree[0];
 	s2->b32_tfree[0] = s1->b16_tfree[1];
 	s2->b32_tinode = s1->b16_tinode;
	for (i = 0; i < 6; i++) {
 		s2->b32_fname[i] = s1->b16_fname[i];
 		s2->b32_fpack[i] = s1->b16_fpack[i];
	};
#endif

	return(0);
}
