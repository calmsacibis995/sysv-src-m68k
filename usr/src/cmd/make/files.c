/*	@(#)files.c	2.1.1.2		*/
/*	@(#)/usr/src/cmd/make/files.c	3.4	*/

#include "defs"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <pwd.h>
#include <ar.h>
/* UNIX DEPENDENT PROCEDURES */

/*  common object versions of routines that can read
    the archive header                                */
#ifdef PORTAR

#include <filehdr.h>
#include <ldfcn.h>

#endif

#define equaln		!strncmp



TIMETYPE
exists(pname)
NAMEBLOCK pname;
{
	register CHARSTAR s;
	struct stat buf;
	TIMETYPE lookarch();
	CHARSTAR filename;

	filename = pname->namep;

	if(any(filename, LPAREN))
		return(lookarch(filename));

	if(stat(filename,&buf) < 0) 
	{
		s = findfl(filename);
		if(s != (CHARSTAR )-1)
		{
			pname->alias = copys(s);
			if(stat(pname->alias, &buf) == 0)
				return(buf.st_mtime);
		}
		return(0);
	}
	else
		return(buf.st_mtime);
}


TIMETYPE
prestime()
{
	TIMETYPE t;
	time(&t);
	return(t);
}



FSTATIC char n15[15];
FSTATIC CHARSTAR n15end = &n15[14];



DEPBLOCK
srchdir(pat, mkchain, nextdbl)
register CHARSTAR pat;		/* pattern to be matched in directory */
int mkchain;			/* nonzero if results to be remembered */
DEPBLOCK nextdbl;		/* final value for chain */
{
	FILE * dirf;
	int i, nread;
	CHARSTAR dirname, dirpref, endir, filepat, p;
	char temp[100];
	char fullname[100];
	CHARSTAR p1, p2;
	NAMEBLOCK q;
	DEPBLOCK thisdbl;
	OPENDIR od;
	int dirofl = 0;
	static opendirs = 0;
	PATTERN patp;

	struct direct entry[32];


	thisdbl = 0;

	if(mkchain == NO)
		for(patp=firstpat ; patp!=0 ; patp = patp->nextpattern)
			if(equal(pat,patp->patval))
				return(0);

	patp = ALLOC(pattern);
	patp->nextpattern = firstpat;
	firstpat = patp;
	patp->patval = copys(pat);

	endir = 0;

	for(p=pat; *p!=CNULL; ++p)
		if(*p==SLASH)
			endir = p;

	if(endir==0)
	{
		dirname = ".";
		dirpref = "";
		filepat = pat;
	}
	else
	{
		*endir = CNULL;
		dirpref = concat(pat, "/", temp);
		filepat = endir+1;
		dirname = temp;
	}

	dirf = NULL;

	for(od=firstod ; od!=0; od = od->nextopendir)
		if(equal(dirname, od->dirn))
		{
			dirf = od->dirfc;
			fseek(dirf,0L,0); /* start over at the beginning  */
			break;
		}

	if(dirf == NULL)
	{
		dirf = fopen(dirname, "r");
		if(++opendirs < MAXODIR)
		{
			od = ALLOC(opendir);
			od->nextopendir = firstod;
			firstod = od;
			od->dirfc = dirf;
			od->dirn = copys(dirname);
		}
		else
			dirofl = 1;
	}

	if(dirf == NULL)
	{
		fprintf(stderr, "Directory %s: ", dirname);
		fatal("Cannot open");
	}

	else	do
		{
			nread = fread(entry,sizeof(entry[0]),32,dirf) ;
			for(i=0; i<nread; ++i)
				if(entry[i].d_ino!= 0)
				{
					p1 = entry[i].d_name;
					p2 = n15;
					while( (p2<n15end) &&
					  (*p2++ = *p1++)!=CNULL );
					if( amatch(n15,filepat) )
					{
						concat(dirpref,n15,fullname);
						if( (q=srchname(fullname)) ==0)
							q = makename(copys(fullname));
						if(mkchain)
						{
							thisdbl = ALLOC(depblock);
							thisdbl->nextdep = nextdbl;
							thisdbl->depname = q;
							nextdbl = thisdbl;
						}
					}
				}
		} while(nread==32);

	if(endir != 0)
		*endir = SLASH;
	if(dirofl)
		fclose(dirf);

	return(thisdbl);
}

/* stolen from glob through find */

amatch(s, p)
CHARSTAR s, p;
{
	register int cc, scc, k;
	int c, lc;

	scc = *s;
	lc = 077777;
	switch (c = *p)
	{

	case LSQUAR:
		k = 0;
		while (cc = *++p)
		{
			switch (cc)
			{

			case RSQUAR:
				if (k)
					return(amatch(++s, ++p));
				else
					return(0);

			case MINUS:
				k |= lc <= scc & scc <= (cc=p[1]);
			}
			if(scc==(lc=cc))
				k++;
		}
		return(0);

	case QUESTN:
	caseq:
		if(scc)
			return(amatch(++s, ++p));
		return(0);
	case STAR:
		return(umatch(s, ++p));
	case 0:
		return(!scc);
	}
	if(c==scc)
		goto caseq;
	return(0);
}

umatch(s, p)
register CHARSTAR s, p;
{
	if(*p==0)
		return(1);
	while(*s)
		if(amatch(s++,p))
			return(1);
	return(0);
}


/* look inside archives for notations a(b) and a((b))
	a(b)	is file member   b   in archive a
	a((b))	is entry point  _b  in object archive a
*/


/*
 *	New common object version of files.c
 */
char		archmem[64];
char		archname[64];		/* name of archive library */

struct ar_hdr	head;		/* archive file header */

/*  if not using the latest archive header format, use these structures  */
#ifndef PORTAR

struct ar_sym	symbol;		/* archive file symbol table entry */
struct arf_hdr	fhead;		/* archive file object file header */

#endif


CHARSTAR	strchr();
TIMETYPE	afilescan();
TIMETYPE	entryscan();
FILE		*arfd;
char		BADAR[] = "BAD ARCHIVE";

#ifdef PORTAR
CHARSTAR	malloc();

#define  TABSZ  2000          	/* maximum number of symbols in an archive
				   symbol directory			     */
#endif

TIMETYPE
lookarch(filename)
register CHARSTAR filename;
{
	register int i;
	CHARSTAR p, q;
#if FLEXNAMES
	char s[BUFSIZ];
#else
	char s[15];
#endif
	int nc, objarch;
	TIMETYPE la();

	for(p = filename; *p!= LPAREN ; ++p);
	i = p - filename;
	strncpy(archname, filename, i);
	archname[i] = CNULL;
	if(archname[0] == CNULL)
		fatal1("Null archive name `%s'", filename);
	p++;
	if(*p == LPAREN)
	{
		objarch = YES;
		++p;
		if((q = strchr(p, RPAREN)) == NULL)
			q = p + strlen(p);
		strncpy(s,p,q-p);
		s[q-p] = CNULL;
	}
	else
	{
		objarch = NO;
		if((q = strchr(p, RPAREN)) == NULL)
			q = p + strlen(p);
		i = q - p;
		strncpy(archmem, p, i);
		archmem[i] = CNULL;
		nc = 14;
		if(archmem[0] == CNULL)
			fatal1("Null archive member name `%s'", filename);
		if(q = strrchr(archmem, SLASH))
			++q;
		else
			q = archmem;
		strncpy(s, q, nc);
		s[nc] = CNULL;
	}
	return(la(s, objarch));
}
TIMETYPE
la(am,flag)
register char *am;
register int flag;
{
	TIMETYPE date = 0L;

#ifdef PORTAR

	if(flag)
		{
		if ( openarch(archname) == -1 )
		  	fatal1("Cannot open archive `%s' ",archname);
		date = entryscan(am);
		clarch();
		}
	else
		date = afilescan(am);

#else

	if(openarch(archname) == -1)
		return(0L);
	if(flag)
		date = entryscan(am);	/* fatals if cannot find entry */
	else
		date = afilescan(am);
	clarch();
#endif

	return(date);
}

TIMETYPE
afilescan(name)
register char *name;
{

#ifdef PORTAR

LDFILE	*ldopen();

	LDFILE	*ldptr, *temp;
	ARCHDR	arhead;

	TIMETYPE   date = 0L;

	ldptr = NULL;

	do  {	
		if (( ldptr = ldopen(archname, ldptr)) != NULL )
		{
			temp = ldptr;
			if ( ldahread (temp, &arhead) != SUCCESS )
				fatal1("Can't read archive header %s\n", name);
			if ( equal( name, arhead.ar_name ) )
				date = arhead.ar_date;
		}
	}  while ( ldclose(ldptr) == FAILURE );
	return( date );
}

#else

	long date;
	long nsyms;
	long ptr;

	nsyms = sgetl(head.ar_syms);
	if(fseek(arfd, (long)( nsyms*sizeof(symbol)+sizeof(head) ), 0) == -1)
		fatal1("Seek error on archive file %s", archname);
	for(;;)
	{
		if(fread(&fhead, sizeof(fhead), 1, arfd) != 1)
			if(feof(arfd))
				break;
			else
				fatal1("Read error on archive %s", archname);
		if(equaln(fhead.arf_name, name, 14))
		{
			date = sgetl(fhead.arf_date);
			return(date);
		}
		ptr = sgetl(fhead.arf_size);
		ptr = (ptr+1)&(~1);
		fseek(arfd, ptr, 1);
	}
	return( 0L );
}

#endif

TIMETYPE
entryscan(name)
register char *name;
{
	register int i;
	long date;

#ifdef PORTAR

typedef union {	long	l;
		char	c[sizeof(long)/sizeof(char)];
	      } longchar;

	longchar     nsyms, symptrtab[TABSZ];
	char	     *strtabptr;
	unsigned     strtabsz;
	long         symarsz;
	register  char    *ptr;
	register  char    *q;
	register  int      j;

	
	if ( fread(&nsyms, sizeof(nsyms), 1, arfd) != 1)
		fatal1("Can't read # of symbols - %s\n",archname );
	nsyms.l = sgetl(nsyms.c);
	if (nsyms.l <= 0)
		fatal1("archive symbol table is empty - %s\n",archname);
	if ( fread(symptrtab, sizeof(longchar), nsyms.l, arfd) != nsyms.l )
		fatal1("Can't read symbol directory %s\n",archname);
	sscanf( head.ar_size, "%ld", &symarsz );
	strtabsz = (unsigned)( symarsz  - (nsyms.l + 1)*sizeof(longchar) );
	strtabptr = malloc ( strtabsz );
	if (( fread( strtabptr, strtabsz, 1, arfd ) != 1) ||
		     strtabptr[strtabsz - 1] != '\0')
	      fatal1("Can't read string table - %s\n",archname);

	 /*  scan through the string table for a match for "name"    */

	ptr = strtabptr;
	for ( i=0; i<nsyms.l; i++ )
	{
		if( ptr > (strtabptr + strtabsz - 1))
			fatal1("Too few symbol names %s",archname);
		if ( equal(ptr, name))
			/*  found a match - get the date    */
			{
			symptrtab[i].l = sgetl( symptrtab[i].c );
			if ( fseek(arfd, symptrtab[i].l, 0) == -1 ||
			     fread(&head, sizeof(head), 1, arfd) != 1)
				fatal1("Read error on archive %s\n",archname);
			sscanf( head.ar_date, "%ld", &date);
			if (( q = strchr(head.ar_name,SLASH)) == NULL )
				q = head.ar_name + strlen(head.ar_name);
			j = q - head.ar_name;
			strncpy ( archmem, head.ar_name, j);
			archmem[j] = CNULL;
			free( strtabptr );
			return( date );
			}
		else	{
			/*  advance to the next symbol in the string table  */
			while ( *++ptr != CNULL );
			ptr++;
			}
	}
	fatal1("Can't find symbol `%s'", name );
}

#else

	long ptr;
	long nsyms;

	nsyms = sgetl(head.ar_syms);
	for(i = 0; i < nsyms; i++)
	{
		if(fread(&symbol, sizeof(symbol), 1, arfd) != 1)
badread:
			fatal1("Read error on archive %s", archname);
		if ( equaln(symbol.sym_name, name, strlen(name) ))
		{
			ptr = sgetl(symbol.sym_ptr);
			if(fseek(arfd, ptr, 0) == -1)
				fatal1("Seek error on archive file %s",archname);
			if(fread(&fhead, sizeof(fhead), 1, arfd) != 1)
				goto badread;
			date = sgetl(fhead.arf_date);
			strncpy ( archmem, fhead.arf_name, 16 );
			return(date);
		}
	}
	fatal1("Cannot find symbol %s in archive %s", name, archname);
}

#endif


openarch(f)
register CHARSTAR f;
{
	int word = 0;

#ifdef PORTAR
	char  buf[SARMAG];
#endif

	arfd = fopen(f, "r");
	if(arfd == NULL)
		return(-1);
#ifdef PORTAR
	if(fread(buf, SARMAG, 1, arfd) != 1 )
		return( -1 );
	if( !equaln(buf, ARMAG, SARMAG) )
		fatal1("%s is not an archive\n", f);
	if(fread(&head, sizeof(head), 1, arfd) != 1)
		return(-1);
#else
	if(fread(&head, sizeof(head), 1, arfd) != 1)
		return(-1);
	if(!equaln(head.ar_magic, ARMAG,4))
		fatal1("%s is not an archive", f);
#endif
	return(0);
}

clarch()
{
	if(arfd != NULL)
		fclose(arfd);
}

/*
 *	Used when unlinking files. If file cannot be stat'ed or it is
 *	a directory, then do not remove it.
 */
isdir(p)
char *p;
{
	struct stat statbuf;

	if(stat(p, &statbuf) == -1)
		return(1);		/* no stat, no remove */
	if((statbuf.st_mode&S_IFMT) == S_IFDIR)
		return(1);
	return(0);
}
