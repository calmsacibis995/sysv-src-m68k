/* @(#)cnvino.c	2.1		*/
cnvino(tovax,fdo,dblk)
int tovax,fdo;
unsigned short dblk;
{

#include <stdio.h>

#include <sys/param.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/ino.h>
#include <sys/inode.h>

#if m68k || MC68
	ushort	modes;
	int dirent;
#endif
	int in,out,j,k;
#if m68k || MC68
	long i,indadr[13],fsize,size;
#else
	long i,indadr[3],fsize,size;
#endif

	/* Build an array of inodes */
	struct dinode d[INOPB];


	/* Set a variable depending whether or not program is running on a vax */
#ifdef vax
#define ONVAX 1
#else
#define ONVAX 0
#endif

	/* Read in blocks of inodes */

	for (i = 2; i < dblk; i++) {

		/* Point to next inode */
#if m68k || MC68
		lseek(fdo,i*BSIZE,0);

		in = read(fdo,d,BSIZE);
#else
		lseek(fdo,i*512,0);

 		in = read(fdo,d,512);
#endif
		if (in <= 0) {
			perror("Error reading inode");
			return(-1);
		}

		/* Do conversions for each inode */
		for (j=0; j < INOPB; j++) {

#if m68k || MC68
			modes=d[j].di_mode;
			if( ONVAX == tovax )
				swaps( &modes );
			dirent=((modes&IFMT)==IFDIR);

			/* Convert the proper type of inode */
			switch(modes&IFMT) {
#else
 			switch(d[j].di_mode&IFMT) {
#endif
			case 0:
				break;

			case IFDIR:
			case IFREG:
			case IFCHR:
			case IFBLK:
				{

					/*
										 * Set up the list of the three indirect blocks.  This is done
										 * after the inode conversion if program is running on the
										 * machine to which file system is being converted.  (See indino.c)
										 */
					if(ONVAX != tovax) {
#if m68k || MC68
						l3tol(indadr,d[j].di_addr,13);
#else
 						l3tol(indadr,&d[j].di_addr[30],3);
#endif
						fsize = d[j].di_size;
					}
#if m68k || MC68
					swapl(&d[j].di_size);
					inov(d[j].di_addr);
					swaps(&d[j].di_mode);
					swaps(&d[j].di_nlink);
					swaps(&d[j].di_uid);
					swaps(&d[j].di_gid);
					swapl(&d[j].di_atime);
					swapl(&d[j].di_mtime);
					swapl(&d[j].di_ctime);
#else
 					swap(&d[j].di_size);
 					tovax ?
 					(inov(&d[j].di_addr[0])) :
 					(inop(&d[j].di_addr[0])) ;
#endif
					if(ONVAX == tovax) {
#if m68k || MC68
						l3tol(indadr,d[j].di_addr,13);
#else
 						l3tol(indadr,&d[j].di_addr[30],3);
#endif
						fsize = d[j].di_size;
					}
#if m68k || MC68
					/* Convert directory entries */
					if(dirent) {
						for(k = 0; k < 12; k++ )
							dodir(fdo,indadr[k]);
					}
#endif
					/* Convert the indirect blocks */
					for(k = 0; k < 3; k++) {
						/* Pass the size of the file so far */
#if m68k || MC68
						size = BSIZE*10;

						indino(tovax,fdo,fsize,&size,k,indadr[k+10],dirent);
#else
 						size = 5120;

 						indino(tovax,fdo,fsize,&size,k,indadr[k]);
#endif
					}
				}
#if pdp11 || PDP11
 				/*
 				 * Special handling when implemented
 				 * case IFMPC:
 				 * case IFMPB:
 				 * case IFIFO:
 				 */
 				swap(&d[j].di_atime);
 				swap(&d[j].di_mtime);
 				swap(&d[j].di_ctime);
#endif
			}
		}
		/* Write inode block back out */
#if m68k || MC68
		lseek(fdo,i*BSIZE,0);
		out = write(fdo,d,BSIZE);
#else
 		lseek(fdo,i*512,0);
 		out = write(fdo,d,512);
#endif
		if (in != out) {
			printf(stderr,"cnvino: error writing output file\n");
			return(1);
		}
	}
	return(0);
}
