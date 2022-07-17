/*	@(#)nlist.c	2.1 	*/
/*LINTLIBRARY*/

#ifdef DEBUG
#	include <stdio.h>
#endif

#include "a.out.h"
#include "sgs.h"

#define	COFF	( vax || u3b || m68k )
#if !COFF
#define SPACE 100		/* number of symbols read at a time */
#endif

extern long lseek();
extern int open(), read(), close(), strncmp();

int
nlist(name, list)
char *name;
#if COFF
struct syment *list;
#else
struct nlist *list;
#endif
{
#if COFF
	struct	filehdr	buf;
	struct	syment	sym;
	struct	auxent	aux;
	long n;
	int	nlen=SYMNMLEN, bufsiz=FILHSZ;
	register struct syment *p, *q;
#if FLEXNAMES
	register int	strfd;	/* string table pointer for new COFF */
	int strstart;		/* start of strings */
#endif
#else
	struct nlist space[SPACE];
	struct exec buf;
	int	nlen=sizeof(space[0].n_name), bufsiz=(sizeof(buf));
	unsigned n, m; 
	register struct nlist *p, *q;
#endif
	long	sa;
	int	fd;

#if !FLEXNAMES
	for(p = list; p->n_name[0]; p++) {
#else
	for(p = list; p->n_zeroes || p->n_nptr; p++) {
#endif
		p->n_type = 0;
		p->n_value = 0;
#if COFF
		p->n_value = 0L;
		p->n_scnum = 0;
		p->n_sclass = 0;
#endif
	}
	
	if((fd = open(name, 0)) < 0)
		return(-1);
	(void) read(fd, (char *)&buf, bufsiz);
#if COFF
	if(BADMAGIC(buf.f_magic)) {
#else
	if(BADMAGIC(buf)) {
#endif
		(void) close(fd);
		return(-1);
	}

#if FLEXNAMES
	if((strfd = open(name, 0)) < 0)
		return(-1);
	strstart = buf.f_symptr+(buf.f_nsyms*sizeof(struct syment));
#endif

#if COFF
	sa = buf.f_symptr;	/* direct pointer to sym tab */
	lseek(fd, sa, 0);
	q = &sym;
	n = buf.f_nsyms;	/* num. of sym tab entries */
#else
	sa = buf.a_text;
	sa += buf.a_data;
#if u370
	sa += (long)(buf.a_trsize + buf.a_drsize);
#endif
#if pdp11
	if(buf.a_flag != 1)
		sa += sa;
	else if( buf.a_magic == A_MAGIC5 )
		sa += (long)buf.a_hitext << 16; /* remainder of text size for system overlay a.out */
#endif
	sa += (long)sizeof(buf);
	(void) lseek(fd, sa, 0);
	n = buf.a_syms;
#endif

	while(n) {
#if COFF
		read(fd, (char *)&sym, SYMESZ);
		n -= (q->n_numaux + 1L);
			/* read past aux ent , if there is one */
		if (q->n_numaux != 0)	read(fd, (char *) &aux, AUXESZ);
#else
		m = (n < sizeof(space))? n: sizeof(space);
		(void) read(fd, (char*)space, m);
		n -= m;
		for(q=space; (int)(m -= sizeof(space[0])) >= 0; ++q) {
#endif
#if !FLEXNAMES
			for(p=list; p->n_name[0]; ++p) {
				if(strncmp(p->n_name, q->n_name, nlen))
					continue;
				p->n_value = q->n_value;
				p->n_type = q->n_type;
#if COFF
				p->n_scnum = q->n_scnum;
				p->n_sclass = q->n_sclass;
#endif
				break;
			}
#else
#define FBUFSIZ 1024
			for(p=list; p->n_zeroes || p->n_nptr; ++p) 
			{
				char flexnamebuf[FBUFSIZ], *pfbuf;
				if( p->n_zeroes && q->n_zeroes )
				{
					if(strncmp(p->n_name,q->n_name,SYMNMLEN))
						continue;
#ifdef DEBUG
	printf("nlist:\tlooking for p->n_name = %8.8s\n", p->n_name);
	printf("\told COFF q->n_name = %8.8s\n", q->n_name);
#endif
				}
				else if( p->n_nptr && q->n_offset && !q->n_zeroes  && !p->n_zeroes)
				{
					pfbuf = flexnamebuf;
					lseek(strfd,strstart+q->n_offset,0);
					do
						read(strfd,pfbuf,1);
					while(*pfbuf++);
					if(strcmp(p->n_nptr,flexnamebuf))
						continue;
#ifdef DEBUG
	printf("nlist:\tlooking for p->n_nptr = %s\n", p->n_nptr);
	printf("\tflex name = %s\n", flexnamebuf);
#endif
				}
				else continue;

				p->n_value = q->n_value;
				p->n_type = q->n_type;
				p->n_scnum = q->n_scnum;
				p->n_sclass = q->n_sclass;
				break;
			}
#endif
#if !COFF
		}
#endif
	}
#ifdef FLEXNAMES
	(void) close(strfd);
#endif
	(void) close(fd);
	return(0);
}
