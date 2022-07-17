/*	@(#)convert.c	2.1		*/
static char ID[] = "@(#) convert.c: 1.5 2/25/83";

#include	<stdio.h>
#include	<ar.h>

#include	"filehdr.h"
#include	"ldfcn.h"
#include	"conv.h"

extern	char	outfile[];
extern	FILE	*outstrm;
extern	unsigned short bootable;

convert( infile )

char	*infile;
{
	/* functions called */
	extern		sprintf( );
	extern int	fwrite( );

	/* libld functions called */
	extern LDFILE	*ldaopen( );
	extern int	ldaclose( ),
			ldclose( ),
			ldahread( );

	/* convert functions called */
	extern LDFILE	*openobject( );

	/* local variables */
	int		status;
	LDFILE		*ldptr,
			*newptr;
	NEWARHDR	arhead;
	char		name[513];

	if ( (ldptr = openobject(infile)) == NULL)
		return(FAILURE);

	if (TYPE(ldptr) == ARTYPE) 
	{
		if ( bootable )
		{
			converr(1,infile,"not executable");
			status = FAILURE;
		}
		if ( (newptr=ldaopen(infile, ldptr)) == NULL ) 
		{
			converr(1,infile,"cannot ldaopen");
			status = FAILURE;
		}
		else if ((status = convar( ldptr, infile )) == FAILURE )
		{
			converr( 1, infile, "cannot convert archive header");
		}
		else
		{
			do
			{
				/*
				 * convert each .o file in the archive
				 */
				if ((status=ldahread(newptr, &arhead)) == SUCCESS)
				{
					sprintf(name, "%s[%.14s]", infile, arhead.ar_name);
					if ((status = callfunc(newptr,name)) != SUCCESS)
					{
						ldaclose(newptr);
					}
				} else {
					ldaclose(newptr);
				}
			} while(ldclose(newptr) == FAILURE);
		}
	} else {
		status = callfunc(ldptr,infile);
	}

	if ( status != SUCCESS )
		unlink( outfile );

	ldaclose(ldptr);
	fclose(outstrm);
	return(status);
}

int
convar( ldptr, infile )
LDFILE *ldptr;
char *infile;

{
	/* functions called */
	extern long		time();

	/* external variables */
	extern unsigned short	target;
	extern unsigned short	ar_target;
	extern unsigned short	ar_origin;

	/* local variables */
	GEN			new;
	extern	long		nsyms;
	RANDARCH		nar;

	if (ar_target == OLD)
	{
		/* 
		 * write out the archive magic number 
		 */
		if ( target == F_AR32W )
		{
			new.l = (long) TYPE(ldptr);
			if (F_CURARCH == F_AR32WR)
				new.l = (new.l << 16) & 0xffff0000;
			if (ISDEC(F_CURARCH))
				swabchar( (char *)&new.l, 4);
			if (fwrite((char *)&new.l,sizeof(new.l),1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");

		}
		else if (target == F_AR32WR )
		{
			new.l = (long) TYPE(ldptr);
			if (F_CURARCH != F_AR32WR )
				new.l = (new.l << 16) & 0xffff0000;
			if (ISNONDEC(F_CURARCH))
				swabchar( (char *) &new.l, 4 );
			if (fwrite( (char *) &new.l, sizeof(long), 1, outstrm ) != 1)
				converr( 1, outfile, "cannot fwrite()" );

		}
		else
		{
			new.s[0] = TYPE(ldptr);
			if (ISNONDEC(F_CURARCH))
				swabchar( (char *)&new.s[0],2);
			if (fwrite(&new.s[0],sizeof(new.s[0]),1,outstrm) != 1)
				converr(1, outfile, "cannot fwrite()");
		}

	/*
	 * create a 5.0 archive
	 */
	}
	else if (ar_target == RANDOM)
	{
		strncpy( nar.ar_magic, RAND_MAGIC, RMAGSIZE );
		charconv( nar.ar_magic, RMAGSIZE );
		strncpy( nar.ar_name, infile, 16 );
		charconv( nar.ar_name, 16 );
		sputl( time(0), nar.ar_date );
		sputl( 0L, nar.ar_syms );
		fwrite( &nar, sizeof( RANDARCH ), 1, outstrm );
		converr( 0, outfile, "has no symbol table, use ar ts on target machine to create archive symbol table");

	/*
	 * create a 6.0 archive
	 */
	}
	else
	{
		if (fwrite( ASCII_MAGIC, AMAGSIZE, 1, outstrm ) != 1)
		{
			converr( 1, outfile, "cannot fwrite()" );
			return( FAILURE );
		}
		converr( 0, outfile, "has no symbol table, use ar ts on target machine to create archive symbol table" );
	}

	return( SUCCESS );
}
