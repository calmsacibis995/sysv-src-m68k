	/*	@(#) bio.h: @(#)bio.h	2.1  */

#define BRSIZ	512
struct brbuf {
	int	nl, nr;
	char	*next;
	char	b[BRSIZ];
	int	fd;
};
long lseek();
