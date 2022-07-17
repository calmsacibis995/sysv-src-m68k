#if m68k || MC68
/* @(#)superm.c	2.1		*/
superm(fdi,s1,s2)

int fdi /* file descriptor for input */;
#include "super68.h"
#include "super32.h"

struct mc68k *s1;
struct pdp32 *s2;

{
#include <stdio.h>
	/*
		 * This subroutine converts a VAX formatted superblock to a MC68000
		 * superblock.
		 */

	int condi,i;
	char ans[20];

	/* Position file to superblock */
	lseek(fdi,(long)512*1,0);

	/* Read the super block in the VAX format and do conversion */
	condi = read(fdi,s2,512);
	if (condi <= 0) {
		perror("Error reading super block");
		printf("condi %d\n",condi);
		exit(1);
	};

	/* Convert data the hard way */

	s1->b68_isize = s2->b32_isize;		swaps(&s1->b68_isize);
	s1->b68_fsize[1] = s2->b32_fsize[0];	swaps(&s1->b68_fsize[1]);
	s1->b68_fsize[0] = s2->b32_fsize[1];	swaps(&s1->b68_fsize[0]);
	s1->b68_nfree = s2->b32_nfree;		swaps(&s1->b68_nfree);
	for (i = 0; i < 100; i += 2) {
		s1->b68_free[i] = s2->b32_free[i+1];
		swaps(&s1->b68_free[i]);
		s1->b68_free[i+1] = s2->b32_free[i];
		swaps(&s1->b68_free[i+1]);
	};
	s1->b68_ninode = s2->b32_ninode;	swaps(&s1->b68_ninode);
	for (i = 0; i < 100; i++) {
		s1->b68_inode[i] = s2->b32_inode[i];
		swaps(&s1->b68_inode[i]);
	};
	s1->b68_flock = s2->b32_flock;		swaps(&s1->b68_flock);
	s1->b68_ilock = s2->b32_ilock;		swaps(&s1->b68_ilock);
	s1->b68_fmod = s2->b32_fmod;		swaps(&s1->b68_fmod);
	s1->b68_ronly = s2->b32_ronly;		swaps(&s1->b68_ronly);
	s1->b68_time[1] = s2->b32_time[0];	swaps(&s1->b68_time[1]);
	s1->b68_time[0] = s2->b32_time[1];	swaps(&s1->b68_time[0]);
	for (i = 0; i < 4; i++) {
		s1->b68_dinfo[i] = s2->b32_dinfo[i];
		swaps(&s1->b68_dinfo[i]);
	};
	s1->b68_tfree[1] = s2->b32_tfree[0];	swaps(&s1->b68_tfree[1]);
	s1->b68_tfree[0] = s2->b32_tfree[1];	swaps(&s1->b68_tfree[0]);
	s1->b68_tinode = s2->b32_tinode;	swaps(&s1->b68_tinode);
	for (i = 0; i < 6; i++) {
		s1->b68_fname[i] = s2->b32_fname[i];
		s1->b68_fpack[i] = s2->b32_fpack[i];
	};
	s1->b68_magic[1] = s2->b32_magic[0];	swaps(&s1->b68_magic[1]);
	s1->b68_magic[0] = s2->b32_magic[1];	swaps(&s1->b68_magic[0]);
	s1->b68_type[1] = s2->b32_type[0];		swaps(&s1->b68_type[1]);
	s1->b68_type[0] = s2->b32_type[1];		swaps(&s1->b68_type[0]);

	return(0);
}
#endif
