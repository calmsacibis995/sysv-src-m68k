/* @(#)super68.h	2.1		*/
/* structure of a super block on a MC68000 family machine */
struct mc68k
{
	unsigned short b68_isize;
	short 	b68_fsize[2];
	short 	b68_nfree;
	short 	b68_free[100];
	short	b68_ninode;
	unsigned short	b68_inode[100];
	char	b68_flock;
	char	b68_ilock;
	char	b68_fmod;
	char	b68_ronly;
	short	b68_time[2];
	short	b68_dinfo[4];
	short	b68_tfree[2];
	unsigned short	b68_tinode;
	char	b68_fname[6];
	char	b68_fpack[6];
	short	b68_fill[30];
	short	b68_magic[2];
	short	b68_type[2];
};
