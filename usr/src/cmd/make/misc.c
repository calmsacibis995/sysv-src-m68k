/*	@(#)misc.c	2.1		*/

#include "defs"
#include "ctype.h"

CHARSTAR do_colon();
CHARSTAR do_df();


FSTATIC CHARSTAR nextchar=0;
FSTATIC CHARSTAR lastchar=0;

FSTATIC int *nextint=0;
FSTATIC int *lastint=0;

FSTATIC NAMEBLOCK hashtab[HASHSIZE];
int nhashed=0;


/* simple linear hash.  hash function is sum of
   characters mod hash table size.
*/
hashloc(s)
CHARSTAR s;
{
	register int i;
	register int hashval;
	register CHARSTAR t;

	hashval = 0;

	for(t=s; *t!=CNULL ; ++t)
		hashval += *t;

	hashval %= HASHSIZE;

	for(i=hashval;
		hashtab[i]!=0 && !equal(s,hashtab[i]->namep);
		i = (i+1)%HASHSIZE ) ;

	return(i);
}


NAMEBLOCK
srchname(s)
register CHARSTAR s;
{
	return( hashtab[hashloc(s)] );
}




NAMEBLOCK
makename(s)
register CHARSTAR s;
{
/* make a name entry; `s' is presumed to already to have been saved */

	register NAMEBLOCK p;

	if(nhashed++ > HASHSIZE-3)
		fatal("Hash table overflow");

	p = ALLOC(nameblock);
	p->nextname = firstname;
	p->backname = NULL;

	p->namep = s;
	p->linep = 0;
	p->done = 0;
	p->septype = 0;
	p->rundep  = 0;
	p->modtime = 0;

	firstname = p;

	hashtab[hashloc(s)] = p;

	return(p);
}







#define NOTSHORT sizeof (struct nameblock)

extern CHARSTAR end;		/* loader value of end of core */

CHARSTAR
copys(s)
register CHARSTAR s;
{
	register CHARSTAR t;
	union
	{
		int i_i;
		CHARSTAR i_s;
	} i;

	i.i_i = strlen(s) + 1;
	if(i.i_i > NOTSHORT)
	{
		i.i_i = (i.i_i + 1)&~1;
		t = (CHARSTAR )calloc(i.i_i, sizeof (*s));
		if(t == NULL)
			goto fat;
		i.i_s = t;
		while(*t++ = *s++);
		return(i.i_s);
	}

	if( i.i_i >= (lastchar-nextchar) )
	{
		i.i_i = (i.i_i + 1)&~1;
		if( (nextchar=(CHARSTAR )calloc(i.i_i, sizeof (*s) )) == NULL)
fat:
			fatal("Cannot allocate memory");
		lastchar = nextchar + i.i_i;
	}

	t = nextchar;
	while(*nextchar++ = *s++);
	return(t);
}


CHARSTAR
concat(a,b,c)   /* c = concatenation of a and b */
register CHARSTAR a, b;
register CHARSTAR c;
{
	register CHARSTAR t;
	t = c;

	while(*t = *a++) t++;
	while(*t++ = *b++);
	return(c);
}

suffix(a,b,p)  /* is b the suffix of a?  if so, set p = prefix */
register CHARSTAR a, b, p;
{
	CHARSTAR a0, b0;
	a0 = a;
	b0 = b;

	if(!a || !b)
		return(0);

	while(*a++);
	while(*b++);

	if( (a-a0) < (b-b0) )
		return(0);

	while(b>b0)
		if(*--a != *--b)
			return(0);

	while(a0<a)
		*p++ = *a0++;
	*p = CNULL;

	return(1);
}



int * 
intalloc(n)
register int n;
{
	register INTSTAR p;

	if( p = (int *) calloc(1,n) )
		return(p);

	fatal("out of memory");
}

/* copy string a into b, substituting for arguments */
extern NAMEBLOCK cur_name;

CHARSTAR
subst(a,b)
register CHARSTAR a, b;
{
	register CHARSTAR s;
	static depth=0;
	char vname[100];
	char closer;

	if(++depth > 100)
		fatal("infinitely recursive macro?");
	if(a!=0)
	{
		while(*a)
		{
			if(*a != DOLLAR)
				*b++ = *a++;
			else if(*++a==CNULL || *a==DOLLAR)
				*b++ = *a++;
			else
			{
				s = vname;
				if( *a==LPAREN || *a==LCURLY )
				{
					closer=(*a==LPAREN ? RPAREN : RCURLY);
					++a;
					while(*a == BLANK)
						++a;
					while(	*a!=BLANK  &&
						*a!=closer &&
						*a!=CNULL)
						*s++ = *a++;
					while(*a!=closer && *a!=CNULL)
						++a;
					if(*a == closer)
						++a;
				}
				else
					*s++ = *a++;

				*s = CNULL;
				if(amatch(&vname[0], "*:*=*"))
					b = colontrans(b, vname);
				else if(any("@*<%?", vname[0]) && vname[1])
					b = dftrans(b, vname);
				else
					b = straightrans(b, vname);
				s++;
			}
		}
	}

	*b = CNULL;
	--depth;
	return(b);
}


/*
 *	Translate the $(name:*=*) type things.
 */

CHARSTAR
colontrans(b, vname)
register CHARSTAR b;
char vname[];
{
	register CHARSTAR p;
	register CHARSTAR q = 0;
	char dftype = 0;
	CHARSTAR pcolon;
	VARBLOCK vbp;

	for(p = &vname[0]; *p && *p != KOLON; p++);
	pcolon = p;
	*pcolon = CNULL;

	if(any("@*<%?", vname[0]))
	{
		dftype = vname[1];
		vname[1] = CNULL;
	}
	if((vbp = srchvar(vname)) == NULL)
		return(b);
	p = vbp->varval;
	if(dftype)
	{
		if((q = calloc(strlen(p)+2),1) == NULL)
			fatal("Cannot alloc mem");
		do_df(q, p, vname[1]);		/* D/F trans gets smaller */
		p = q;
	}
	if(p && *p)
		b = do_colon(b, p, pcolon+1);
	*pcolon = KOLON;
	if(dftype)
		vname[1] = dftype;
	if(q)
		cfree(q);
	return(b);
}
/*
 *	Translate the $(name:*=*) type things.
 */

CHARSTAR
do_colon(to, from, trans)
register CHARSTAR to, from;
CHARSTAR trans;
{
	register int i;
	register CHARSTAR  p;
	register int leftlen;
	int len;
	char left[30], right[70];
	char buf[128];
	CHARSTAR pbuf;
	int lwig = 0;
	int rwig = 0;

/*
 *	Mark off the name (up to colon), the from expression (up to '='),
 *	and the to expresion (up to CNULL).
 */
	i = 0;
	while(*trans != EQUALS)
		left[i++] = *trans++;
	if(left[i-1] == WIGGLE)
	{
		lwig++;
		--i;
	}
	left[i] = CNULL;
	leftlen = i;
	i = 0;
	while(*++trans)
		right[i++] = *trans;
	if(right[i-1] == WIGGLE)
	{
		rwig++;
		--i;
	}
	right[i] = CNULL;

/*
 *	Now, tanslate.
 */

	for(; len=getword(from,buf); from += len)
	{
		pbuf = buf;
		if((i = sindex(pbuf, left)) >= 0 && pbuf[i+leftlen] == CNULL &&
			(lwig?((p=sdot(pbuf))!=(char *)0):1) )
		{
			pbuf[i] = CNULL;
			if(!lwig && rwig)
				trysccs(pbuf);
			else if(lwig && !rwig)
				strshift(p, -2);
			to = copstr(to, pbuf);
			to = copstr(to, right);
		}
		else
		{
			to = copstr(to, pbuf);
		}
	}
	return(to);
}
getword(from, buf)
register CHARSTAR from;
register CHARSTAR buf;
{
	register int i = 0;

	if(*from == TAB || *from == BLANK)
	{
		while(*from == TAB || *from == BLANK)
		{
			*buf++ = *from++;
			++i;
		}
		goto out;
	}
	while(*from && *from != TAB && *from != BLANK)
	{
		*buf++ = *from++;
		++i;
	}
out:
	*buf = CNULL;
	return(i);
}

/*
 *	Do the $(@D) type translations.
 */

CHARSTAR
dftrans(b, vname)
register CHARSTAR b;
CHARSTAR vname;
{
	register char c1;
	VARBLOCK vbp;

	c1 = vname[1];
	vname[1] = CNULL;
	vbp = srchvar(vname);
	vname[1] = c1;
	if(vbp != 0 && *vbp->varval != 0)
		b = do_df(b, vbp->varval, c1);
	return(b);
}

#define lastslash(a)		strrchr( (a) , SLASH )
CHARSTAR
do_df(b, str, type)
register CHARSTAR b;
char str[];
char type;
{
	register CHARSTAR p;
	register int i;
	char buf[128];

	*b = CNULL;
	for(; (i=getword(str, buf)); str += i)
	{
		if(buf[0] == BLANK || buf[0] == TAB)
		{
			b = copstr(b, buf);
			continue;
		}
		p = lastslash(buf);
		if(p)
		{
			*p = CNULL;
			b = copstr(b, type=='D'?(buf[0]==CNULL?"/":buf):p+1);
			*p = SLASH;
		}
		else
			b = copstr(b, type=='D'?".":buf);
	}
	return(b);
}

/*
 *	Standard trnaslation, nothing fancy.
 */

CHARSTAR
straightrans(b, vname)
register CHARSTAR b;
char vname[];
{
	register VARBLOCK vbp;
	register CHAIN pchain;
	register NAMEBLOCK pn;

	vbp = srchvar(vname);
	if( vbp != 0)
	{
		if(vbp->v_aflg == YES && vbp->varval)
		{
			pchain = (CHAIN)vbp->varval;
			for(; pchain; pchain = pchain->nextchain)
			{
				pn = (NAMEBLOCK)pchain->datap;
				if(pn->alias)
					b=copstr(b, pn->alias);
				else
					b=copstr(b,pn->namep);
				*b++ = BLANK;
			}
			vbp->used = YES;
		}
		else if(*vbp->varval)
		{
			b = subst(vbp->varval, b);
			vbp->used = YES;
		}
	}
	return(b);
}



/* copy s into t, return the location of the next free character in s */
CHARSTAR
copstr(s, t)
register CHARSTAR s, t;
{
	if(t == 0)
		return(s);
	while (*t)
		*s++ = *t++;
	*s = CNULL;
	return(s);
}

setvar(v,s)
register CHARSTAR v, s;
{
	register VARBLOCK p;

	p = srchvar(v);
	if(p == 0)
	{
		p = varptr(v);
	}
	s = s?s:Nullstr;
	if (p->noreset == NO)
	{
		if(IS_ON(EXPORT))
			p->envflg = YES;
		p->varval = s;
		if(IS_ON(INARGS) || IS_ON(ENVOVER))
			p->noreset = YES;
		else
			p->noreset = NO;
if(IS_ON(DBUG))printf("setvar: %s = %s noreset = %d envflg = %d Mflags = 0%o\n",
	v, p->varval, p->noreset, p->envflg, Mflags);

		if(p->used && !amatch(v, "[@*<?!%]") )
			if(IS_ON(DBUG))
				fprintf(stderr, "Warning: %s changed after being used\n",v);
	}
}


eqsign(a)
register CHARSTAR a;
{
	register CHARSTAR p;

	for(p = ":;=$\n\t"; *p; p++)
		if(any(a, *p))
		{
			callyacc(a);
			return(YES);
		}
	return(NO);
}


VARBLOCK
varptr(v)
register CHARSTAR v;
{
	register VARBLOCK vp;

	if((vp = srchvar(v)) != 0)
		return(vp);

	vp = ALLOC(varblock);
	vp->nextvar = firstvar;
	firstvar = vp;
	vp->varname = copys(v);
	vp->varval = Nullstr;
	return(vp);
}

VARBLOCK
srchvar(vname)
register CHARSTAR vname;
{
	register VARBLOCK vp;

	for(vp=firstvar; vp != 0 ; vp = vp->nextvar)
		if(equal(vname, vp->varname))
			return(vp);
	return(NO);
}

/*VARARGS*/
fatal1(s,t1,t2,t3)
CHARSTAR s;
{
	char buf[100];
	sprintf(buf, s, t1,t2,t3);
	fatal(buf);
}



fatal(s)
CHARSTAR s;
{
	if(s)
		fprintf(stderr, "Make: %s.  Stop.\n", s);
	else
		fprintf(stderr, "\nStop.\n");
#ifdef unix
	exit(1);
#endif
#ifdef gcos
	exit(0);
#endif
}



yyerror(s)
CHARSTAR s;
{
	char buf[50];
	extern int yylineno;

	sprintf(buf, "line %d: %s", yylineno, s);
	fatal(buf);
}



CHAIN
appendq(head,tail)
register CHAIN head;
register CHARSTAR tail;
{
	register CHAIN p;

	p = ALLOC(chain);
	p->datap = tail;
	while(head->nextchain)
		head = head->nextchain;
	head->nextchain = p;
}





CHARSTAR
mkqlist(p)
register CHAIN p;
{
	register CHARSTAR qbufp, s;
	static char qbuf[OUTMAX];

	qbufp = qbuf;

	for( ; p ; p = p->nextchain)
	{
		s = p->datap;
		if(qbufp != qbuf)
			*qbufp++ = BLANK;
		if(qbufp+strlen(s) > &qbuf[OUTMAX-3])
		{
			fprintf(stderr, "$? list too long\n");
			break;
		}
		while (*s)
			*qbufp++ = *s++;
	}
	*qbufp = CNULL;
	return(qbuf);
}

sindex(s1,s2)
CHARSTAR s1;
CHARSTAR s2;
{
	register CHARSTAR p1;
	register CHARSTAR p2;
	register int flag;
	int ii;

	p1 = &s1[0];
	p2 = &s2[0];
	flag = -1;
	for(ii = 0; ; ii++)
	{
		while(*p1 == *p2)
		{
			if(flag < 0)
				flag = ii;
			if(*p1++ == CNULL)
				return(flag);
			p2++;
		}
		if(*p2 == CNULL)
			return(flag);
		if(flag >= 0)
		{
			flag = -1;
			p2 = &s2[0];
		}
		if(*s1++ == CNULL)
			return(flag);
		p1 = s1;
	}
}


#include "sys/types.h"
#include "sys/stat.h"

/*
 *	findfl(name)	(like execvp, but does path search and finds files)
 */
static char fname[128];

CHARSTAR execat();

CHARSTAR
findfl(name)
register CHARSTAR name;
{
	register CHARSTAR p;
	register VARBLOCK cp;

	if(name[0] == SLASH)
		return(name);
	cp = varptr("VPATH");
	if(*cp->varval == 0)
		p = ":";
	else
		p = cp->varval;

	do
	{
		p = execat(p, name, fname);
		if(access(fname, 4) == 0)
			return(fname);
	} while (p);
	return((CHARSTAR )-1);
}

CHARSTAR
execat(s1, s2, si)
register CHARSTAR s1, s2;
CHARSTAR si;
{
	register CHARSTAR s;

	s = si;
	while (*s1 && *s1 != KOLON && *s1 != MINUS)
		*s++ = *s1++;
	if (si != s)
		*s++ = SLASH;
	while (*s2)
		*s++ = *s2++;
	*s = CNULL;
	return(*s1? ++s1: 0);
}


/*
 *	change xx to s.xx or /x/y/z to /x/y/s.z
 */
CHARSTAR
trysccs(str)
register CHARSTAR str;
{
	register CHARSTAR sstr;
	register int i = 2;

	sstr = str;
	for(; *str; str++);
	str[2] = CNULL;
	str--;
	for(;str >= sstr; str--)
	{
		if(*str == SLASH)
			if(i == 2)
			{
				i = 0;
				*(str+2) = DOT;
				*(str+1) = 's';
			}
		*(str+i) = *str;
	}
	if(i == 2)
	{
		*(str+2) = DOT;
		*(str+1) = 's';
	}
	return(sstr);
}


is_sccs(filename)
register CHARSTAR filename;
{
	register CHARSTAR p;

	for(p = filename; *p; p++)
		if(*p == 's')
			if(p == filename && p[1] == DOT)
				return(YES);
			else if(p[-1] == SLASH && p[1] == DOT)
				return(YES);
	return(NO);
}

CHARSTAR
sdot(p)
register char *p;
{
	register char *ps = p;

	for(; *p; p++)
		if(*p == 's')
			if(p == ps && p[1] == DOT)
				return(p);
			else if(p[-1] == SLASH && p[1] == DOT)
				return(p);
	return((char *)0);
}
/*
 *	change pfx to /xxx/yy/*zz.* or *zz.*
 */
CHARSTAR
addstars(pfx)
register CHARSTAR pfx;
{
	register CHARSTAR p1, p2;

	for(p1 = pfx; *p1; p1++);
	p2 = p1 + 3;			/* 3 characters, '*', '.', and '*'. */
	p1--;

	*p2-- = CNULL;
	*p2-- = STAR;
	*p2-- = DOT;
	while(p1 >= pfx)
	{
		if(*p1 == SLASH)
		{
			*p2 = STAR;
			return(pfx);
		}
		*p2-- = *p1--;
	}
	*p2 = STAR;
	return(p2);
}


#define NENV	300
extern CHARSTAR *environ;

/*
 *	This routine is called just before and exec.
 */

setenv()
{
	register CHARSTAR *ea;
	register int nenv = 0;
	register CHARSTAR p;
	CHARSTAR *es;
	VARBLOCK vp;
	int length;

	if(firstvar == 0)
		return;

	es=ea=(CHARSTAR *)calloc(NENV, sizeof *ea);
	if(es == (CHARSTAR *)-1)
		fatal("Cannot alloc mem for envp.");

	for(vp=firstvar; vp != 0; vp=vp->nextvar)
		if(vp->envflg)
		{
			if(++nenv >= NENV)
				fatal("Too many env parameters.");
			length = strlen(vp->varname) + strlen(vp->varval) + 2;
			if((*ea = (CHARSTAR )calloc(length, sizeof **ea)) == (CHARSTAR )-1)
				fatal("Cannot alloc mem for env.");
			p = copstr(*ea++, vp->varname);
			p = copstr(p, "=");
			p = copstr(p, vp->varval);
		}
	*ea = 0;
	if(nenv > 0)
		environ=es;
	if(IS_ON(DBUG))
		printf("nenv = %d\n", nenv);
}


/*
 *	Called in main
 *	If a string like "CC=" occurs then CC is not put in environment.
 *	This is because there is no good way to remove a variable
 *	from the environment within the shell.
 */

readenv()
{
	register CHARSTAR *ea;
	register CHARSTAR p;

	ea=environ;
	for(;*ea; ea++)
	{
		for(p = *ea; *p && *p != EQUALS; p++);
		if(*p == EQUALS)
			if(*(p+1))
				eqsign(*ea);
	}
}


sccstrip(pstr)
register CHARSTAR pstr;
{
	register CHARSTAR p2;
	register CHARSTAR sstr;

	sstr = pstr;
	for(; *pstr ; pstr++)
		if(*pstr == RCURLY)
		{
			if(isdigit(pstr[1]))
			if(pstr != sstr)
			if(pstr[-1] != DOLLAR)
			{
				for(p2 = pstr; *p2 && (*p2 != LCURLY); p2++);
				if(*p2 == CNULL)
					break;
				strshift(pstr, -(int)(p2-pstr+1) );
			}
		}
}

/*
 *	Shift a string `pstr' count places. negative is left, pos is right
 *	Negative shifts cause char's at front to be lost.
 *	Positive shifts assume enough space!
 */
CHARSTAR
strshift(pstr, count)
register CHARSTAR pstr;
register int count;
{
	register CHARSTAR sstr;

	sstr = pstr;
	if(count < 0)
	{
		for(count = -count; *pstr=pstr[count]; pstr++);
		*pstr = 0;
		return(sstr);
	}
	for(; *pstr; pstr++);
	do
	{
		pstr[count] = *pstr;
	} while (pstr--, count--);

	return(sstr);

}


/*
 *	execlp(name, arg,...,0)	(like execl, but does path search)
 *	execvp(name, argv)	(like execv, but does path search)
 */
#include <errno.h>
#define	NULL	0

CHARSTAR execat();
extern	errno;

execlp(name, argv)
CHARSTAR name, argv;
{
	return(execvp(name, &argv));
}

execvp(name, argv)
CHARSTAR name, *argv;
{
	register etxtbsy = 1;
	register eacces = 0;
	register CHARSTAR cp;
	CHARSTAR pathstr;
	CHARSTAR shell;
	char fname[128];

	pathstr = varptr("PATH")->varval;
	if(pathstr == 0 || *pathstr == CNULL)
		pathstr = ":/bin:/usr/bin";
	shell = varptr("SHELL")->varval;
	if(shell == 0 || *shell == CNULL)
		shell = "/bin/sh";
	cp = any(name, SLASH)? "": pathstr;

	do
	{
		cp = execat(cp, name, fname);
	retry:
		execv(fname, argv);
		switch(errno)
		{
		case ENOEXEC:
			*argv = fname;
			*--argv = "sh";
			execv(shell, argv);
			return(-1);
		case ETXTBSY:
			if (++etxtbsy > 5)
				return(-1);
			sleep(etxtbsy);
			goto retry;
		case EACCES:
			eacces++;
			break;
		case ENOMEM:
		case E2BIG:
			return(-1);
		}
	} while (cp);
	if (eacces)
		errno = EACCES;
	return(-1);
}

/*
 *	get() does an SCCS get on the file ssfile.
 *	For the get command, get() uses the value of the variable "GET".
 *	If ssfile has a slash in it, get() does a "chdir" to the appropriate
 *	directory if the cdflag is set to CD. This assures
 *	the program finds the ssfile where it belongs when necessary.
 *	If the rlse string variable is set, get() uses it in the
 *	get command sequence.
 *	Thus a possible sequence is:
 *		set -x;
 *		cd ../sys/head;
 *		get -r2.3.4.5 s.stdio.h
 *
 */

/*
 *	The gothead and gotf structures are used to remember
 *	the names of the files `make' automatically gets so
 *	`make' can remove them upon exit.
 */
GOTHEAD gotfiles;

get(ssfile, cdflag, rlse)
register CHARSTAR ssfile;
int cdflag;
CHARSTAR rlse;
{
	register CHARSTAR pr;
	register CHARSTAR pr1;
	char gbuf[128];
	char sfile[128];
	int retval;
	GOTF gf;

	copstr(sfile, ssfile);
	if(!sdot(sfile))
		trysccs(sfile);
	if(access(sfile, 4) != 0 && IS_OFF(GET))
		return(NO);

	pr = gbuf;
	if(IS_OFF(SIL))
		pr = copstr(pr, "set -x;\n");

	if(cdflag == CD)
	if(any(sfile, SLASH))
	{
		pr = copstr(pr, "cd ");
		for(pr1 = sfile; *pr1; pr1++);
		while(*pr1 != SLASH)
			pr1--;
		*pr1 = CNULL;
		pr = copstr(pr, sfile);
		pr = copstr(pr, ";\n");
		*pr1 = SLASH;
	}

	pr = copstr(pr, varptr("GET")->varval);
	pr = copstr(pr, " ");
	pr = copstr(pr, varptr("GFLAGS")->varval);
	pr = copstr(pr, " ");

	pr1 = rlse;
	if(pr1 != NULL && pr1[0] != CNULL)
	{
		if(pr1[0] != MINUS)	/* RELEASE doesn't have '-r' */
			pr = copstr(pr, "-r");
		pr = copstr(pr, pr1);
		pr = copstr(pr, " ");
	}

	pr = copstr(pr, sfile);
/*
 *	exit codes are opposite of error codes so do the following:
 */
	retval = (system(gbuf) == 0) ? YES : NO ;
	if(retval == YES)
	{
		if(gotfiles == 0)
		{
			gotfiles = ALLOC(gothead);
			gf = (GOTF)gotfiles;
			gotfiles->gnextp = 0;
			gotfiles->endp = (GOTF)gotfiles;
		}
		else
		{
			gf = gotfiles->endp;
			gf->gnextp = ALLOC(gotf);
			gf = gf->gnextp;
			gf->gnextp = 0;
		}
		gf->gnamep = copys(sfile+2);	/* `+2' skips `s.' */
		gotfiles->endp = gf;
	}
	return(retval);
}

/*
 *	subroutine to actually remove to gotten files.
 */
rm_gots()
{
	register GOTF gf;

	if(IS_ON(GF_KEEP))
		return;
	for(gf = (GOTF)gotfiles; gf ; gf=gf->gnextp)
		if(gf->gnamep)
		{
			if(IS_ON(DBUG))printf("rm_got: %s\n", gf->gnamep);
			unlink(gf->gnamep);
		}
}
callyacc(str)
register CHARSTAR str;
{
	CHARSTAR lines[2];
	FILE *finsave;
	CHARSTAR *lpsave;

	finsave = fin;
	lpsave = linesptr;
	fin = 0;
	lines[0] = str;
	lines[1] = 0;
	linesptr = lines;
	yyparse();
	fin = finsave;
	linesptr = lpsave;
}

/*
 *	exit routine for removing the files `make' automatically
 *	got.
 */
exit(arg)
{
	rm_gots();
	if(IS_ON(MEMMAP))
	{
		prtmem();
	}
	_cleanup();
	_exit(arg);
}
