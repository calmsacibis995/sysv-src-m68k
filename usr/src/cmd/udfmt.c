/*	@(#)udfmt.c	1.5		*/
/*
 *	udfmt.c: EXORmacs Universal Disk Controller disk formatter
 */

#define	UDFMT	1

#define	SPT	64	/* Sectors per track */
#define	SPB	4	/* Sectors per block */
#define	BPT	SPT/SPB	/* Blocks per track */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 *	main: udfmt special [blocks]
 */

main( argc, argv )
int	argc;
char	**argv;

{	register int	maxblk;
	register int	curblk;
	register int	fd;
	register int	bpt;
	register char	*estr;
	char		*getenv();
	struct	stat	stbuf;

	if(( estr = getenv("BPT")) == NULL )
		bpt = BPT;
	else	bpt = atoi( estr );

	if( argc > 3 )
	{	fprintf( stderr, "Usage: udfmt special [blocks]\n");
		exit(1);
	}

	if( stat( argv[1], &stbuf ) == -1 )
	{	fprintf( stderr, "%s: Cannot stat %s\n", argv[0], argv[1] );
		exit(1);
	}

	if(( stbuf.st_mode & S_IFMT ) != S_IFCHR )
	{	fprintf( stderr, "%s: raw devices only.\n", argv[0] );
		exit(1);
	}
	
	if(( fd = open( argv[1], 2 )) == -1 )
	{	fprintf( stderr, "%s: Cannot open %s\n", argv[0], argv[1] );
		exit(1);
	}

	if( argc == 3 )
	{	maxblk = atoi( argv[2] );
		
		for( curblk=0; curblk < maxblk; curblk += bpt )
			if( ioctl( fd, UDFMT, curblk ) == -1 )
			{	fprintf( stderr, "%s: error on block %d\n",
					argv[0], curblk );
				break;
			}
	}
	else
		for( curblk=0; ioctl(fd,UDFMT,curblk) != -1; curblk += bpt );

	printf( "%s: formatted %d blocks\n", argv[0], curblk );
}
