/*	@(#)putp2.c	2.1.1.1		*/
#if FAMILY != PCC
	WRONG put FILE !!!!
#endif

#if TARGET==M68K
#include "mfile2.h"
#else
#include "mfile2"
#endif

#undef INT			/* these things are defined */
#undef ICON			/* differently for pcc than */
#undef SZINT			/* for f77 so, include pcc  */
#undef SZSHORT			/* headers, then undef them */
#undef SZLONG

#include "defs"
#include "pccdefs"
Addrp putcall(), putcxeq(), putcx1(), realpart();
expptr imagpart();
ftnint lencat();
NODE *bldtree(), *p2op(), *p2comma(), *p2icon();
NODE *p2reg(), *p2oreg(), *p2mknode(); 
#if TARGET==M68K
NODE *p2auto(), *p2param(), *strarg();
#endif

#define FOUR 4
extern int ops2[];
extern int types2[];

#define P2BUFFMAX 128
static long int p2buff[P2BUFFMAX];
static long int *p2bufp		= &p2buff[0];
static long int *p2bufend	= &p2buff[P2BUFFMAX];


puthead(s, class, entlabel, baklabel, spn)
char *s, *spn;
int class, entlabel, baklabel;
{
#if TARGET!=M68K
  	p2pass("\t.align\t2");
#endif
	if (class == CLMAIN)
#if TARGET == X3B || TARGET==M68K
		p2pass( "MAIN__:" );			/* silly 3b sgs */
#else
  		p2pass( "_MAIN__:" );
#endif
	if (s && strcmp(s, "MAIN__"))
#if TARGET == X3B || TARGET==M68K
		p2ps("%s:", s);
#else
  		p2ps("_%s:",  s);
#endif
	if (spn)
#if TARGET == X3B || TARGET==M68K
		p2pi("%s_:", spn);
#else
  		p2pi("_%s_:", spn);
#endif
	if (class != CLBLOCK)
	{
		prsave();
		putgoto(baklabel);
	}
	if (!headerdone)
	{
		headerdone = YES;
#if TARGET!=M68K
  		baseoff = maxoff = maxtreg =
#endif
		tmpoff = maxtemp = 0;
		ftnno = procno;
#if TARGET!=M68K
  		strcpy(filename, infname);
#endif
	}
}



putbracket()
{
	prhead(textfile);
}



putrbrack(k, p)
int k;
struct Entrypoint *p;
{
	gencode(p2op(P2RBRACKET, k));
#if TARGET==VAX
	putgoto(p->entrylabel);
#endif
}



putnreg()
{
}



puteof()
{
}



/* put out code for if( ! p) goto l  */
putif(p,l)
register expptr p;
int l;
{
	register int k;

	if ((k = (p = fixtype(p))->headblock.vtype) != TYLOGICAL)
	{
		if (k != TYERROR)
			err("non-logical expression in IF statement");
		frexpr(p);
	}
else
	{
	p2x(p);
	nstack(p2icon((long int) l , P2INT));
#ifdef SGS2
	nstack(p2op(P2CBRANCH, P2INT));
#else
  	nstack(p2op(P2CBRANCH, 0));
#endif
	gencode(bldtree());
	}
}



/* put out code for  goto l   */
putgoto(label)
int label;
{
	gencode(p2mknode(P2GOTO, CNULL, label, 0, 1));
}



/* branch to address constant or integer variable */
putbranch(p)
register Addrp p;
{
	p2x(p);
#ifdef SGS2
	nstack(p2mknode(P2GOTO, CNULL, 0, 0, types2[p->vtype]));
#else
  	nstack(p2mknode(P2GOTO, CNULL, 0, 0, 0));
#endif
	gencode(bldtree());
}

/* put out label  l:     */
putlabel(label)
int label;
{
	gencode(p2mknode(P2LABEL, CHNULL, label, 0,  0));
}




putexpr(p)
expptr p;
{
	putex1(p);
}




putcmgo(index, nlab, labs)
expptr index;
int nlab;
struct Labelblock *labs[];
{
#if TARGET != VAX
	int i, labarray, skiplabel;
#endif

	if(! ISINT(index->headblock.vtype) )
	{
		execerr("computed goto index must be integer", CNULL);
		return;
	}

#if TARGET == VAX
	/* use special case instruction */
  	vaxgoto(index, nlab, labs);
#else
#	if TARGET==M68K
	nstack( p2mknode( P2SNODE, CNULL, 0, 0, P2INT ) );
	p2x( index );
	nstack( p2mknode( P2ASSIGN, CNULL, 0, 0, P2INT ) );
	gencode( bldtree() );
	m68kgoto(index, nlab, labs);
#	else
  	labarray = newlabel();
	preven(ALIADDR);
	prlabel(asmfile, labarray);
	prcona(asmfile, (ftnint) (skiplabel = getlab()) );
  	prcona(asmfile, (ftnint) (skiplabel = newlabel()) );
	for(i = 0 ; i < nlab ; ++i)
		if( labs[i] )
			prcona(asmfile, (ftnint)(labs[i]->labelno) );
	prcmgoto(index, nlab, skiplabel, labarray);
	putlabel(skiplabel);
#	endif
#endif
}

putx(p)
expptr p;
{
	p2x(p);
	gencode(bldtree());
}


p2x(p)
expptr p;
{
	char *memname();
	int opc;
	int ncomma;
	int type, k;

	switch (p->tag)
	{
	case TERROR:
		free((charptr) p);
		break;
	case TCONST:
		switch (type = p->constblock.vtype)
		{
		case TYLOGICAL:
			type = tyint;
		case TYLONG:
		case TYSHORT:
			nstack(p2icon(p->constblock.const.ci, types2[type]));
			free( (charptr) p );
			break;
		case TYADDR:
			nstack(p2mknode(P2ICON,
				memname(STGCONST, (int) p->constblock.const.ci),
				0, 0, P2INT | P2PTR));
			free((charptr) p);
			break;
		default:
			p2x(putconst(p));
				break;
			}
		break;

	case TEXPR:
		switch (opc = p->exprblock.opcode)
			{
		case OPCALL:
		case OPCCALL:
			if (ISCOMPLEX(p->exprblock.vtype))
				putcxop(p);
			else
				putcall(p);
			break;
		case OPMIN:
		case OPMAX:
			putmnmx(p);
			break;
		case OPASSIGN:
			if (ISCOMPLEX(p->exprblock.leftp->headblock.vtype)
			   || ISCOMPLEX(p->exprblock.rightp->headblock.vtype))
				frexpr(putcxeq(p));
			else if (ISCHAR(p))
				putcheq(p);
			else
				goto putopp;
			break;
		case OPEQ:
		case OPNE:
			if (ISCOMPLEX(p->exprblock.leftp->headblock.vtype) ||
			    ISCOMPLEX(p->exprblock.rightp->headblock.vtype))
			{
				putcxcmp(p);
				break;
			}
		case OPLT:
		case OPLE:
		case OPGT:
		case OPGE:
			if (ISCHAR(p->exprblock.leftp))
			{
				putchcmp(p);
				break;
			}
			goto putopp;
		case OPPOWER:
			putpower(p);
			break;
		case OPSTAR:
#if FAMILY == PCC
			/*   m * (2**k) -> m<<k   */
			if (INT(p->exprblock.leftp->headblock.vtype) &&
			   ISICON(p->exprblock.rightp) &&
			   ((k = log2(p->exprblock.rightp->constblock.const.ci)) > 0))
			{
				p->exprblock.opcode = OPLSHIFT;
				frexpr(p->exprblock.rightp);
				p->exprblock.rightp = ICON(k);
				goto putopp;
			}
#endif
		case OPMOD:
			goto putopp;
		case OPPLUS:
		case OPMINUS:
		case OPSLASH:
		case OPNEG:
			if (ISCOMPLEX(p->exprblock.vtype))
				putcxop(p);
			else
				goto putopp;
			break;
		case OPCONV:
			if (ISCOMPLEX(p->exprblock.vtype))
				putcxop(p);
			else if (ISCOMPLEX(p->exprblock.leftp->headblock.vtype))
			{
				ncomma = 0;
				p2x(mkconv(p->exprblock.vtype,
					realpart(putcx1(p->exprblock.leftp,
						&ncomma))));
				putcomma(ncomma, p->exprblock.vtype, NO);
				free((charptr) p);
			}
			else
				goto putopp;
			break;
		case OPNOT:
		case OPOR:
		case OPAND:
		case OPEQV:
		case OPNEQV:
		case OPADDR:
		case OPPLUSEQ:
		case OPSTAREQ:
		case OPCOMMA:
		case OPQUEST:
		case OPCOLON:
		case OPBITOR:
		case OPBITAND:
		case OPBITXOR:
		case OPBITNOT:
		case OPLSHIFT:
		case OPRSHIFT:
	putopp:
			putop(p);
			break;
		default:
			badop("putx", opc);
			}
	break;
	case TADDR:
		putaddr(p, YES);
		break;
	default:
		badtag("putx", p->tag);
	}
}



LOCAL
putop(p)
expptr p;
{
	int k;
	expptr lp, tp;
	int pt, lt;
	int comma;

	switch (p->exprblock.opcode)   /* check for special cases and rewrite */
	{
	case OPCONV:
		pt = p->exprblock.vtype;
		lp = p->exprblock.leftp;
		lt = lp->headblock.vtype;
		while (p->tag==TEXPR && p->exprblock.opcode==OPCONV &&
		      ((ISREAL(pt)&&ISREAL(lt)) ||
		      (INT(pt)&&(ONEOF(lt,MSKINT|MSKADDR|MSKCHAR|M(TYSUBR))))))
		{
#if SZINT < SZLONG
			if(lp->tag != TEXPR)
			{
				if(pt==TYINT && lt==TYLONG)
					break;
				if(lt==TYINT && pt==TYLONG)
					break;
			}
#endif
#if TARGET == VAX || TARGET==M68K
			if(pt==TYDREAL && lt==TYREAL)
			{
				if(lp->tag==TEXPR &&
				  lp->exprblock.opcode==OPCONV &&
				  lp->exprblock.leftp->headblock.vtype==TYDREAL)
				{
					p2x(lp->exprblock.leftp);
					nstack(p2op(P2CONV, P2REAL));
					nstack(p2op(P2CONV, P2DREAL));
					free((charptr) p);
					return;
				}
				else
					break;
			}
#endif
			if (pt == TYREAL && lt == TYDREAL)
#ifdef SGS2
		        {
				p2x(lp);
				nstack(p2op(P2CONV, P2REAL));
				free((charptr) p);
				return;
			}
#else
				break;
#endif
			if(lt==TYCHAR && lp->tag==TEXPR && 
			   lp->exprblock.opcode==OPCALL)
			{
				p->exprblock.leftp = (expptr) putcall(lp);
				putop(p);
				nstack(p2comma(pt, NO));
				free((charptr) p);
				return;
			}
#ifdef SGS2
			if( pt == TYCHAR && ISINT( lt ) )
			{	
				p2x(lp);
				nstack(p2op(P2CONV, P2CHAR));
				free((charptr) p);
				return;
			}

			if( ISINT(pt) && ONEOF(lt, MSKADDR) )
			{	
				p2x(lp);
				nstack(p2op(P2CONV, P2INT));
				free((charptr) p);
				return;
			}
#endif
			free((charptr) p);
			p = lp;
			pt = lt;
			lp = p->exprblock.leftp;
			lt = lp->headblock.vtype;
		}
		if(p->tag==TEXPR && p->exprblock.opcode==OPCONV)
			break;
		p2x(p);
		return;
	case OPADDR:
		comma = NO;
		lp = p->exprblock.leftp;
		if(lp->tag != TADDR)
		{
			tp = (expptr)
				mktemp(lp->headblock.vtype,lp->headblock.vleng);
			p2x(mkexpr(OPASSIGN, cpexpr(tp), lp));
			lp = tp;
			comma = YES;
		}
		putaddr(lp, NO);
		if(comma)
			nstack(p2comma(TYINT, NO));
		free((charptr) p);
		return;
	case OPASSIGN:
#ifdef DOUBLE
		break;	/*convert from double to real necessary when */
			/*the sizes of real and double are not equal */
#else
		if ((p->headblock.vtype == TYREAL ||
			p->headblock.vtype == TYDREAL) &&
		    p->exprblock.rightp->headblock.tag == TEXPR &&
		    p->exprblock.rightp->headblock.vtype == TYREAL &&
		    p->exprblock.rightp->exprblock.opcode == OPCONV &&
		    p->exprblock.rightp->exprblock.leftp->headblock.vtype
		      == TYDREAL)
		{
			expptr px;
			px=(expptr)cpexpr(p->exprblock.rightp->exprblock.leftp);
			frexpr(p->exprblock.rightp);
			p->exprblock.rightp = px;
		}
#endif
#if TARGET == VAX
	case OPLT:
	case OPLE:
	case OPGT:
	case OPGE:
	case OPEQ:
	case OPNE:
		if(ISREAL(p->exprblock.leftp->headblock.vtype) &&
		   ISREAL(p->exprblock.rightp->headblock.vtype) &&
		   ISCONST(p->exprblock.rightp) &&
		   p->exprblock.rightp->constblock.const.cd[0]==0)
		{
			p->exprblock.rightp->constblock.vtype = TYINT;
			p->exprblock.rightp->constblock.const.ci = 0;
		}
#endif

	}

	if ((k = ops2[p->exprblock.opcode]) <= 0)
		badop("putop", p->exprblock.opcode);
	p2x(p->exprblock.leftp);
	if(p->exprblock.rightp)
		p2x(p->exprblock.rightp);
	nstack(p2op(k, types2[p->exprblock.vtype]));
	if (p->exprblock.vleng)
		frexpr(p->exprblock.vleng);
	free((charptr) p);
}

putforce(t, p)
int t;
expptr p;
{
#ifdef SGS2
	int type2;
#endif

	p = mkconv(t, fixtype(p));
#ifdef SGS2
	type2 = types2[t];
	nstack(p2mknode(P2RNODE, CNULL, NIL, NIL, type2));
	p2x(p);
	nstack( p2mknode( P2ASSIGN, CNULL, NIL, NIL, type2));
#else
	p2x(p);
	nstack(p2op(P2FORCE, (t==TYSHORT ? P2SHORT : (t==TYLONG ? P2LONG : P2DREAL))));
#endif
	gencode(bldtree());
}



LOCAL
putpower(p)
expptr p;
{
	expptr base;
	Addrp t1, t2;
	ftnint k;
	int type;
	int ncomma;

	if (!ISICON(p->exprblock.rightp) ||
    	    (k = p->exprblock.rightp->constblock.const.ci) < 2)
		fatal("putpower: bad call");
	base = p->exprblock.leftp;
	type = base->headblock.vtype;
	t1 = mktemp(type, PNULL);
	t2 = NULL;
	ncomma = 1;
	putassign(cpexpr(t1), cpexpr(base) );
	for ( ; (k & 1) == 0 && k > 2; k >>= 1)
	{
		++ncomma;
		putsteq(t1, t1);
	}
	if (k == 2)
		p2x(mkexpr(OPSTAR, cpexpr(t1), cpexpr(t1)));
	else
	{
		t2 = mktemp(type, PNULL);
		++ncomma;
		putassign(cpexpr(t2), cpexpr(t1));
		for(k >>= 1; k > 1; k >>= 1)
		{
			++ncomma;
			putsteq(t1, t1);
			if (k & 1)
			{
				++ncomma;
				putsteq(t2, t1);
			}
		}
		p2x(mkexpr(OPSTAR, cpexpr(t2),
		    mkexpr(OPSTAR, cpexpr(t1), cpexpr(t1))));
	}
	putcomma(ncomma, type, NO);
	frexpr(t1);
	if (t2)
		frexpr(t2);
	frexpr(p);
}




LOCAL Addrp
intdouble(p, ncommap)
Addrp p;
int *ncommap;
{
	register Addrp t;

	t = mktemp(TYDREAL, PNULL);
	++*ncommap;
	putassign(cpexpr(t), p);
	return(t);
}





LOCAL Addrp
putcxeq(p)
register expptr p;
{
	register Addrp lp, rp;
	int ncomma;

	if (p->tag != TEXPR)
		badtag("putcxeq", p->tag);
	ncomma = 0;
	lp = putcx1(p->exprblock.leftp, &ncomma);
	rp = putcx1(p->exprblock.rightp, &ncomma);
	putassign(realpart(lp), realpart(rp));
	if (ISCOMPLEX(p->exprblock.vtype))
	{
		++ncomma;
		putassign(imagpart(lp), imagpart(rp));
	}
	putcomma(ncomma, TYREAL, NO);
	frexpr(rp);
	free((charptr) p);
	return(lp);
}



LOCAL
putcxop(p)
expptr p;
{
	Addrp putcx1();
	int ncomma;

	ncomma = 0;
	putaddr(putcx1(p, &ncomma), NO);
	putcomma(ncomma, TYINT, NO);
}



LOCAL Addrp
putcx1(p, ncommap)
register expptr p;
int *ncommap;
{
	expptr q;
	Addrp lp, rp;
	register Addrp resp;
	int opcode;
	int ltype, rtype;
	expptr mkrealcon();

	if(p == NULL)
		return(NULL);
	switch(p->tag)
	{
	case TCONST:
		if (ISCOMPLEX(p->constblock.vtype))
			p = (expptr) putconst(p);
		return ((Addrp) p);
	case TADDR:
		if (!addressable(p))
		{
			++*ncommap;
			resp = mktemp(tyint, PNULL);
			putassign(cpexpr(resp), p->addrblock.memoffset);
			p->addrblock.memoffset = (expptr) resp;
		}
		return((Addrp) p);
	case TEXPR:
		if (ISCOMPLEX(p->exprblock.vtype))
			break;
		++*ncommap;
		resp = mktemp(TYDREAL, NO);
		putassign(cpexpr(resp), p);
		return (resp);
	default:
		badtag("putcx1", p->tag);
	}
	opcode = p->exprblock.opcode;
	if (opcode==OPCALL || opcode==OPCCALL)
	{
		++*ncommap;
		return( putcall(p) );
	}
	else if (opcode == OPASSIGN)
	{
		++*ncommap;
		return (putcxeq(p));
	}
	resp = mktemp(p->exprblock.vtype, PNULL);
	if (lp = putcx1(p->exprblock.leftp, ncommap))
		ltype = lp->vtype;
	if (rp = putcx1(p->exprblock.rightp, ncommap))
		rtype = rp->vtype;
	switch (opcode)
	{
	case OPCOMMA:
		frexpr(resp);
		resp = rp;
		rp = NULL;
		break;
	case OPNEG:
		putassign(realpart(resp), mkexpr(OPNEG, realpart(lp), ENULL));
		putassign(imagpart(resp), mkexpr(OPNEG, imagpart(lp), ENULL));
		*ncommap += 2;
		break;
	case OPPLUS:
	case OPMINUS:
		putassign(realpart(resp),
			mkexpr(opcode, realpart(lp), realpart(rp)));
		if (rtype < TYCOMPLEX)
			putassign(imagpart(resp), imagpart(lp));
		else if (ltype < TYCOMPLEX)
		{
			if (opcode == OPPLUS)
				putassign(imagpart(resp), imagpart(rp));
			else
				putassign(imagpart(resp),
					mkexpr(OPNEG, imagpart(rp), ENULL));
		}
		else
			putassign(imagpart(resp),
				mkexpr(opcode, imagpart(lp), imagpart(rp)));

		*ncommap += 2;
		break;
	case OPSTAR:
		if (ltype < TYCOMPLEX)
		{
			if (ISINT(ltype))
				lp = intdouble(lp, ncommap);
			putassign(realpart(resp),
				mkexpr(OPSTAR, cpexpr(lp), realpart(rp)));
			putassign(imagpart(resp),
				mkexpr(OPSTAR, cpexpr(lp), imagpart(rp)));
		}
		else if (rtype < TYCOMPLEX)
		{
			if (ISINT(rtype))
				rp = intdouble(rp, ncommap);
			putassign(realpart(resp),
				mkexpr(OPSTAR, cpexpr(rp), realpart(lp)));
			putassign(imagpart(resp),
				mkexpr(OPSTAR, cpexpr(rp), imagpart(lp)));
		}
		else
		{
			putassign(realpart(resp), mkexpr(OPMINUS,
				mkexpr(OPSTAR, realpart(lp), realpart(rp)),
				mkexpr(OPSTAR, imagpart(lp), imagpart(rp))));
			putassign(imagpart(resp), mkexpr(OPPLUS,
				mkexpr(OPSTAR, realpart(lp), imagpart(rp)),
				mkexpr(OPSTAR, imagpart(lp), realpart(rp))));
		}
		*ncommap += 2;
		break;
	case OPSLASH:
		/* fixexpr has already replaced all divisions
		 * by a complex by a function call
		 */
		if (ISINT(rtype))
			rp = intdouble(rp, ncommap);
		putassign(realpart(resp),
			mkexpr(OPSLASH, realpart(lp), cpexpr(rp)));
		putassign(imagpart(resp),
			mkexpr(OPSLASH, imagpart(lp), cpexpr(rp)));
		*ncommap += 2;
		break;
	case OPCONV:
		putassign(realpart(resp), realpart(lp));
		if (ISCOMPLEX(lp->vtype))
			q = imagpart(lp);
		else if (rp != NULL)
			q = (expptr) realpart(rp);
		else
			q = mkrealcon(TYDREAL, 0.0);
		putassign(imagpart(resp), q);
		*ncommap += 2;
		break;
	default:
		badop("putcx1", opcode);
	}
	frexpr(lp);
	frexpr(rp);
	free((charptr) p);
	return (resp);
}




LOCAL
putcxcmp(p)
register expptr p;
{
	int opcode;
	int ncomma;
	register Addrp lp, rp;
	expptr q;

	if (p->tag != TEXPR)
		badtag("putcxcmp", p->tag);
	ncomma = 0;
	opcode = p->exprblock.opcode;
	lp = putcx1(p->exprblock.leftp, &ncomma);
	rp = putcx1(p->exprblock.rightp, &ncomma);
	q = mkexpr(opcode==OPEQ ? OPAND : OPOR ,
		mkexpr(opcode, realpart(lp), realpart(rp)),
		mkexpr(opcode, imagpart(lp), imagpart(rp)));
	p2x(fixexpr(q));
	putcomma(ncomma, TYINT, NO);
	free((charptr) lp);
	free((charptr) rp);
	free((charptr) p);
}

LOCAL Addrp
putch1(p, ncommap)
register expptr p;
int * ncommap;
{
	register Addrp t;
	Addrp mktemp();
	Addrp putconst();

	switch (p->tag)
	{
	case TCONST:
		return(putconst(p));
	case TADDR:
		return((Addrp) p);
	case TEXPR:
		++*ncommap;
		switch (p->exprblock.opcode)
		{
			expptr q;

		case OPCALL:
		case OPCCALL:
			t = putcall(p);
			break;
		case OPCONCAT:
			t = mktemp(TYCHAR, ICON(lencat(p)));
			q = (expptr) cpexpr(p->headblock.vleng);
			putcat(cpexpr(t), p);
			/* put the correct length on the block */
			frexpr(t->vleng);
			t->vleng = q;
			break;
		case OPCONV:
			if (!ISICON(p->exprblock.vleng)
			   || p->exprblock.vleng->constblock.const.ci!=1
			   || ! INT(p->exprblock.leftp->headblock.vtype))
				fatal("putch1: bad character conversion");
			t = mktemp(TYCHAR, ICON(1));
			putop(mkexpr(OPASSIGN, cpexpr(t), p));
			break;
		default:
			badop("putch1", p->exprblock.opcode);
		}
		return (t);
	default:
		badtag("putch1", p->tag);
	}
/* NOTREACHED */
}

LOCAL
putchop(p)
expptr p;
{
	int ncomma;

	ncomma = 0;
	putaddr(putch1(p, &ncomma) , NO);
	putcomma(ncomma, TYCHAR, YES);
}




LOCAL
putcheq(p)
register expptr p;
{
	int ncomma;
	expptr lp, rp;

	if (p->tag != TEXPR)
		badtag("putcheq", p->tag);

	ncomma = 0;
	lp = p->exprblock.leftp;
	rp = p->exprblock.rightp;
	if (rp->tag==TEXPR && rp->exprblock.opcode==OPCONCAT)
		putcat(lp, rp);
	else if (ISONE(lp->headblock.vleng) && ISONE(rp->headblock.vleng))
	{
		putaddr(putch1(lp, &ncomma) , YES);
		putaddr(putch1(rp, &ncomma) , YES);
		putcomma(ncomma, TYINT, NO);
		nstack(p2op(P2ASSIGN, P2CHAR));
	}
	else
	{
		p2x(call2(TYINT, "s_copy", lp, rp));
		putcomma(ncomma, TYINT, NO);
	}
	frexpr(p->exprblock.vleng);
	free((charptr) p);
}




LOCAL
putchcmp(p)
register expptr p;
{
	int ncomma;
	expptr lp, rp;

	if (p->tag != TEXPR)
		badtag("putchcmp", p->tag);
	ncomma = 0;
	lp = p->exprblock.leftp;
	rp = p->exprblock.rightp;
	if (ISONE(lp->headblock.vleng) && ISONE(rp->headblock.vleng))
	{
		putaddr(putch1(lp, &ncomma) , YES);
		putaddr(putch1(rp, &ncomma) , YES);
		nstack(p2op(ops2[p->exprblock.opcode], P2INT));
		free((charptr) p);
		putcomma(ncomma, TYINT, NO);
	}
	else
	{
		p->exprblock.leftp = call2(TYINT,"s_cmp", lp, rp);
		p->exprblock.rightp = ICON(0);
		putop(p);
	}
}




LOCAL
putcat(lhs, rhs)
register Addrp lhs;
register expptr rhs;
{
	int n, ncomma;
	Addrp lp, cp;

	ncomma = 0;
	n = ncat(rhs);
	lp = mktmpn(n, TYLENG, PNULL);
	cp = mktmpn(n, TYADDR, PNULL);
	n = 0;
	putct1(rhs, lp, cp, &n, &ncomma);
	p2x(call4(TYSUBR, "s_cat", lhs, cp, lp, mkconv(TYLONG, ICON(n))));
	putcomma(ncomma, TYINT, NO);
}



LOCAL
putct1(q, lp, cp, ip, ncommap)
register expptr q;
register Addrp lp, cp;
int *ip, *ncommap;
{
	int i;
	Addrp lp1, cp1;

	if (q->tag==TEXPR && q->exprblock.opcode==OPCONCAT)
	{
		putct1(q->exprblock.leftp, lp, cp, ip, ncommap);
		putct1(q->exprblock.rightp, lp, cp , ip, ncommap);
		frexpr(q->exprblock.vleng);
		free((charptr) q);
	}
	else
	{
		i = (*ip)++;
		lp1 = (Addrp) cpexpr(lp);
		lp1->memoffset = mkexpr(OPPLUS,lp1->memoffset, ICON(i*SZLENG));
		cp1 = (Addrp) cpexpr(cp);
		cp1->memoffset = mkexpr(OPPLUS, cp1->memoffset, ICON(i*SZADDR));
		putassign(lp1, cpexpr(q->headblock.vleng));
		putassign(cp1, addrof(putch1(q,ncommap)));
		*ncommap += 2;
	}
}

LOCAL
putaddr(p, indir)
register Addrp p;
int indir;
{
	int type, type2, funct;
	ftnint offset, simoffset();
	expptr offp;
#if SZINT < SZLONG
	expptr shorten();
#endif

	if (p->tag==TERROR || (p->memoffset!=NULL && ISERROR(p->memoffset)))
	{
		frexpr(p);
		return;
	}
	type = p->vtype;
	type2 = types2[type];
	funct = (p->vclass==CLPROC ? P2FUNCT << 2 : 0);
	offp = (p->memoffset ? (expptr) cpexpr(p->memoffset) : (expptr) NULL);
#if (FUDGEOFFSET != 1)
	if (offp)
		offp = mkexpr(OPSTAR, ICON(FUDGEOFFSET), offp);
#endif
	offset = simoffset(&offp);
#if SZINT < SZLONG
	if (offp)
		if (shortsubs)
			offp = shorten(offp);
		else
			offp = mkconv(TYINT, offp);
#else
	if (offp)
		offp = mkconv(TYINT, offp);
#endif
	switch (p->vstg)
	{
	case STGAUTO:
		if (indir && !offp)
		{
#ifdef SGS2
			nstack(p2auto(offset, AUTOREG, type2));
#else
  			nstack(p2oreg(offset, AUTOREG, type2));
#endif
			break;
		}
		if (!indir && !offp && !offset)
		{
#ifdef SGS2
			nstack(p2auto(offset,AUTOREG, type2 | P2PTR));
#else
			nstack(p2reg(AUTOREG, type2 | P2PTR));
#endif
			break;
		}
#ifdef SGS2
		nstack(p2auto(offset, AUTOREG, type2 |P2PTR ));
		nstack(p2mknode(UNARY P2BITAND, CNULL, NIL, NIL, P2INT));
#else
		nstack(p2reg(AUTOREG, type2 | P2PTR));
#endif
		if (offp)
		{
			p2x(offp);
			if (offset)
				nstack(p2icon(offset, P2INT));
		}
#ifndef SGS2
		else
			nstack(p2icon(offset, P2INT));
#endif
		if (offp && offset)
			nstack(p2op(P2PLUS, type2 | P2PTR));
#ifndef SGS2
		nstack(p2op(P2PLUS, type2 | P2PTR));
#endif
		if (indir)
			nstack(p2op(P2INDIRECT, type2));
		break;
	case STGARG:
#ifdef SGS2
		nstack(p2param(
#else
  		nstack(p2oreg(
#endif
#ifdef ARGOFFSET
			ARGOFFSET +
#endif
			(ftnint) (FUDGEOFFSET*p->memno),
			ARGREG, type2 | P2PTR | funct));

/* see below on register-based arrays !
 *
 *	based:
 */
		if (offp)
			p2x(offp);
		if (offset)
		{
			nstack(p2icon(offset, P2INT));
#ifdef SGS2
  			nstack(p2op(P2PLUS, (offp ? P2INT :  type2 | P2PTR)));
#else
  			nstack(p2op(P2PLUS, type2 | P2PTR));
#endif
		}
		if (offp)
			nstack(p2op(P2PLUS, type2 | P2PTR));
		if (indir)
			nstack(p2op(P2INDIRECT, type2));
		break;
	case STGLENG:
		if (indir)
		{
#ifdef SGS2
			nstack(p2param(
#else
  			nstack(p2oreg(
#endif
#ifdef ARGOFFSET
				ARGOFFSET +
#endif
				(ftnint) (FUDGEOFFSET*p->memno),
				ARGREG, type2));
		}
		else
		{
			nstack(p2reg(ARGREG, type2 | P2PTR));
			nstack(p2icon(
#ifdef ARGOFFSET
				ARGOFFSET +
#endif
				(ftnint) (FUDGEOFFSET*p->memno), P2INT));
			nstack(p2op(P2PLUS, type2 | P2PTR));
		}
		break;
	case STGBSS:
	case STGINIT:
	case STGEXT:
	case STGCOMMON:
	case STGEQUIV:
	case STGCONST:
		if (offp)
		{
			p2x(offp);
			putmem(p, P2ICON, offset);
			nstack(p2op(P2PLUS, type2 | P2PTR));
			if (indir)
				nstack(p2op(P2INDIRECT, type2));
		}
		else
			putmem(p, (indir ? P2NAME : P2ICON), offset);

		break;
	case STGREG:
		if (indir)
			nstack(p2reg(p->memno, type2));
		else
			fatal("attempt to take address of a register");
		break;

/* for possible future use -- register based arrays
 *
 *	case STGPREG:
 *		if(indir && !offp)
 *			p2oreg(offset, p->memno, type2);
 *		else
 *			{
 *			p2reg(p->memno, type2 | P2PTR);
 *			goto based;
 *			}
 *		break;
 */

	default:
		badstg("putaddr", p->vstg);
	}
	frexpr(p);
}




LOCAL
putmem(p, class, offset)
expptr p;
int class;
ftnint offset;
{
	int type2;
	char *name, *memname();

	type2 = types2[p->headblock.vtype];
	if (p->headblock.vclass == CLPROC)
	 	type2 |= (P2FUNCT << 2);
	name = memname(p->addrblock.vstg, p->addrblock.memno);
	if (class == P2ICON)
		nstack(p2mknode(P2ICON,name, 0, offset, type2|P2PTR));
	else
		nstack(p2mknode(P2NAME, name, 0, offset, type2));
}

LOCAL Addrp
putcall(p)
register Exprp p;
{
	chainp arglist, charsp, cp;
	int n, first;
	Addrp t;
	register expptr q;
	Addrp fval;
	int type, type2, ctype, qtype, indir;
#ifdef SGS2
	int i = 0;	/* parameter offset counter */
	register expptr cpln;
	expptr savq;
	static int blksize[ ] =
	{	0,
		  sizeof(struct Nameblock),
		  sizeof(struct Constblock),
		  sizeof(struct Exprblock),
		  sizeof(struct Addrblock),
		  sizeof(struct Primblock),
		  sizeof(struct Listblock),
		  sizeof(struct Errorblock)
	};
#endif

	type2 = types2[type = p->vtype];
	charsp = NULL;
	indir =  (p->opcode == OPCCALL);
	n = 0;
	first = YES;
	if (p->rightp)
	{
		arglist = p->rightp->listblock.listp;
		free((charptr) (p->rightp));
	}
	else
		arglist = NULL;
	for (cp = arglist; cp; cp = cp->nextp)
		if (indir)
			++n;
		else
		{
			q = (expptr) (cp->datap);
			if (ISCONST(q))
			{
				q = (expptr) putconst(q);
				cp->datap = (tagptr) q;
			}
			if (ISCHAR(q) && q->headblock.vclass!=CLPROC)
			{
				charsp = hookup(charsp,
					mkchain(cpexpr(q->headblock.vleng),
						CHNULL));
			n += 2;
			}
		else
			n += 1;
		}
	if (type == TYCHAR)
	{
		if (ISICON(p->vleng))
		{
			fval = mktemp(TYCHAR, p->vleng);
			n += 2;
		}
		else
		{
			err("adjustable character function");
			return;
		}
	}
	else if (ISCOMPLEX(type))
	{
		fval = mktemp(type, PNULL);
		n += 1;
	}
	else
		fval = NULL;
	ctype = (fval ? P2INT : type2);
	putaddr(p->leftp, NO);
	if(fval)
	{
		first = NO;
		putaddr(cpexpr(fval), NO);
#ifdef SGS2
		nstack( strarg(indir, fval, &i) );
#endif 
		if (type==TYCHAR)
		{
			p2x(mkconv(TYLENG,p->vleng));
			nstack(p2op(P2LISTOP, type2));
		}
	}
	for (cp = arglist; cp; cp = cp->nextp)
	{

#ifdef SGS2
		savq = (expptr) cpblock( blksize[cp->datap->tag], cp->datap );
#endif
		q = (expptr) (cp->datap);
		if (q->tag==TADDR && (indir || q->addrblock.vstg!=STGREG))
			putaddr(q, indir && q->addrblock.vtype!=TYCHAR);
		else if (ISCOMPLEX(q->headblock.vtype))
			putcxop(q);
		else if (ISCHAR(q))
			putchop(q);
		else if (!ISERROR(q))
		{
			if (indir)
				p2x(q);
			else
			{
				t = mktemp(qtype = q->headblock.vtype,
					q->headblock.vleng);
				putassign(cpexpr(t), q);
				putaddr(t, NO);
				putcomma(1, qtype, YES);
			}
		}

#ifdef SGS2
		nstack( strarg( indir, savq, &i));    /*stack FUNARG above arg*/
		free( (charptr) savq );
#endif

		if (first)
			first = NO;
		else
			nstack(p2op(P2LISTOP, type2));
	}
	if (arglist)
		frchain(&arglist);
	for (cp = charsp; cp; cp = cp->nextp)
	{
#ifdef SGS2
		cpln = mkconv(TYLENG,cp->datap);
		p2x( cpln );
		nstack(strarg(indir,cpln, &i));/*stack FUNARG above cp length */
#else
  		p2x(mkconv(TYLENG,cp->datap));
#endif
		nstack(p2op(P2LISTOP, type2));
	}
	frchain(&charsp);
	nstack(p2op(n>0 ? P2CALL : P2CALL0 , ctype));
	free((charptr) p);
	return (fval);
}



LOCAL putmnmx(p)
register expptr p;
{
	int op, type;
	int ncomma;
	expptr qp;
	chainp p0, p1;
	Addrp sp, tp;

	if (p->tag != TEXPR)
		badtag("putmnmx", p->tag);
	type = p->exprblock.vtype;
	op = (p->exprblock.opcode==OPMIN ? OPLT : OPGT );
	p0 = p->exprblock.leftp->listblock.listp;
	free((charptr) (p->exprblock.leftp));
	free((charptr) p);
	sp = mktemp(type, PNULL);
	tp = mktemp(type, PNULL);
	qp = mkexpr(OPCOLON, cpexpr(tp), cpexpr(sp));
	qp = mkexpr(OPQUEST, mkexpr(op, cpexpr(tp),cpexpr(sp)), qp);
	qp = fixexpr(qp);
	ncomma = 1;
	putassign(cpexpr(sp), p0->datap);
	for(p1 = p0->nextp; p1; p1 = p1->nextp)
	{
		++ncomma;
		putassign(cpexpr(tp), p1->datap);
		if(p1->nextp)
		{
			++ncomma;
			putassign(cpexpr(sp), cpexpr(qp));
		}
	else
		p2x(qp);
	}
	putcomma(ncomma, type, NO);
	frtemp(sp);
	frtemp(tp);
	frchain(&p0);
}




LOCAL
putcomma(n, type, indir)
int n, type, indir;
{
	type = types2[type];
	if (indir)
		type |= P2PTR;
	while (--n >= 0)
		nstack(p2op(P2COMOP, type));
}



LOCAL NODE *
p2comma(type, indir)
int type, indir;
{
	type = types2[type];
	if (indir)
		type |= P2PTR;
	return (p2op(P2COMOP, type));
}


ftnint
simoffset(p0)
expptr *p0;
{
	ftnint offset, prod;
	register expptr p, lp, rp;

	offset = 0;
	p = *p0;
	if (p == NULL)
		return(0);
	if (!ISINT(p->headblock.vtype))
		return(0);
	if (p->tag==TEXPR && p->exprblock.opcode==OPSTAR)
	{
		lp = p->exprblock.leftp;
		rp = p->exprblock.rightp;
		if (ISICON(rp) && lp->tag==TEXPR &&
		   lp->exprblock.opcode==OPPLUS && ISICON(lp->exprblock.rightp))
		{
			p->exprblock.opcode = OPPLUS;
			lp->exprblock.opcode = OPSTAR;
			prod = rp->constblock.const.ci *
				lp->exprblock.rightp->constblock.const.ci;
			lp->exprblock.rightp->constblock.const.ci =
				rp->constblock.const.ci;
			rp->constblock.const.ci = prod;
		}
	}
	if (p->tag==TEXPR && p->exprblock.opcode==OPPLUS &&
	    ISICON(p->exprblock.rightp))
	{
		rp = p->exprblock.rightp;
		lp = p->exprblock.leftp;
		offset += rp->constblock.const.ci;
		frexpr(rp);
		free((charptr) p);
		*p0 = lp;
	}
	if (ISCONST(p))
	{
		offset += p->constblock.const.ci;
		frexpr(p);
		*p0 = NULL;
	}
	return (offset);
}

LOCAL NODE *
p2op(op, type)
int op, type;
{
	return (p2mknode(op, CNULL, 0, 0, type));
}


LOCAL NODE *
p2icon(offset, type)
ftnint offset;
int type;
{
	return(p2mknode(P2ICON, CNULL, 0, offset, type));
}


#ifndef SGS2
LOCAL NODE *
p2oreg(offset, reg, type)
ftnint offset;
int reg, type;
{
/* if BACKAUTO defined, stack grows toward low memory, and references
 * off of fp should not have sign reversed (f77 compiler thinks BACKAUTO
 * is "normal" and all offsets from ARGREG are originally negative
 */
 
#ifndef BACKAUTO
	if (reg == AUTOREG)
		offset = -offset;
#endif
	return (p2mknode(P2OREG, CNULL, reg, offset, type));
}
#else

LOCAL NODE *
p2auto(offset, reg, type)
ftnint offset;
int reg, type;
{
/* if BACKAUTO defined, stack grows toward low memory, and references
 * off of fp should not have sign reversed (f77 compiler thinks BACKAUTO
 * is "normal" and all offsets from ARGREG are originally negative
 */
#ifndef BACKAUTO
	if (reg == AUTOREG)
		offset = -offset;
#endif
	return (p2mknode(P2VAUTO, CNULL, reg, offset, type));
}


LOCAL NODE *
p2param(offset, reg, type)
ftnint offset;
int reg, type;
{
/* if BACKAUTO defined, stack grows toward low memory, and references
 * off of fp should not have sign reversed (f77 compiler thinks BACKAUTO
 * is "normal" and all offsets from ARGREG are originally negative
 */
#ifndef BACKAUTO
	if (reg == AUTOREG)
		offset = -offset;
#endif
	return (p2mknode(P2VPARAM, CNULL, reg, offset, type));
}
#endif

LOCAL NODE *
p2reg(reg, type)
int reg, type;
{
	return (p2mknode(P2REG, CNULL, reg, 0, type));
}



LOCAL NODE *
p2mknode(op, name, rval, lval, type)
int op, rval, lval, type;
char *name;
{
	register NODE *np;
	register int i;
#ifdef SGS2
        char     *tstr();
#endif

#ifndef SGS2
  	np->in.su = 0;
  	np->in.rall = NOPREF;
#endif
	np = talloc();
	np->in.op = op;
	if (name)			/* pcc2 in.name is a ptr, not array */
#ifdef SGS2
		np->in.name = tstr( name );
#else
  		strncpy(np->in.name, name, NCHNAM);
#endif
	else
#ifdef SGS2
		np->in.name = (char *) 0;
#else
  		for (i = 0; i < NCHNAM; i++)
  			np->in.name[i] = '\0';
#endif

	np->tn.rval = rval;
	np->tn.lval = lval;
	np->in.type = type;
	return (np);
}


p2pi(s, i)
char *s;
int i;
{
	char buff[100];

	sprintf(buff, s, i);
	p2pass(buff);
}



p2pij(s, i, j)
char *s;
int i, j;
{
	char buff[100];

	sprintf(buff, s, i, j);
	p2pass(buff);
}




p2ps(s, t)
char *s, *t;
{
	char buff[100];

	sprintf(buff, s, t);
	p2pass(buff);
}




p2pass(s)
char *s;
{
	puts(s);
}

#ifdef SGS2
/* the following routines were canabalized from PCC2 code generator
 * routines
 */
NODE *
strarg( indir, p, off )
register expptr p;
register *off;
int indir;
{
	 /* rewrite arguments */
	/* allocate the arguments at *off, and update *off */
	register TWORD t;
	NODE *q;

	t = types2[p->addrblock.vtype];

	if( !indir )
		t |= P2PTR << BTSHIFT;	/* is a pointer to type t*/

	q = p2mknode( FUNARG, NIL, NIL, NIL, t );
	q->fn.cdim = -1;
	q->fn.csiz = tsize( t, q->fn.cdim, q->fn.csiz );

	SETOFF( *off, typealign[p->addrblock.vtype] );
	q->tn.rval = *off;
	*off += tsize( t, q->fn.cdim, q->fn.csiz );
	return( q );
}

talign( ty, s)
register unsigned ty; 
register s; 
{
	/* compute the alignment of an object with type ty, sizeoff index s */

	register i;
# ifdef FUNNYFLDS
	if( s<0 && ty!=INT && ty!=CHAR && ty!=SHORT && ty!=UNSIGNED && ty!=UCHAR && ty!=USHORT 
#ifdef LONGFIELDS
	    && ty!=LONG && ty!=ULONG
#endif
	    )
	{
		return( fldal( ty ) );
	}
# endif

	for( i=0; i<=(SZINT-BTSHIFT-1); i+=TSHIFT )
	{
		switch( (ty>>i)&TMASK )
		{

		case FTN:
			cerror( "compiler takes alignment of function");
		case PTR:
			return( ALPOINT );
		case ARY:
			continue;
		case 0:
			break;
		}
	}

	switch( BTYPE(ty) )
	{

	case CHAR:
	case UCHAR:
		return( ALCHAR );
	case FLOAT:
		return( ALFLOAT );
	case DOUBLE:
		return( ALDOUBLE );
	case LONG:
	case ULONG:
		return( ALLONG );
	case SHORT:
	case USHORT:
		return( ALSHORT );
	default:
		return( ALINT );
	}
}

int
tsize( ty, d, s )
register TWORD ty; 
{
	register int mult,
		     i,
	             sz;

	mult = 1;

	for( i=0; i<=(SZINT-BTSHIFT-1); i+=TSHIFT )
	{
		switch( (ty>>i)&TMASK )
		{
		case FTN:
			cerror( "compiler takes size of function");
		case PTR:
			return( SZPOINT * mult );
		case ARY:
			mult *= sz;
			continue;
		case 0:
			break;
		}
	}
	 
	switch( ty & BTMASK )
	{
		case TNULL:
			sz = 0;
			break;
		case CHAR:
			sz = SZCHAR;
			break;
		case P2INT:
			sz = SZINT;
			break;
		case FLOAT:
			sz = SZFLOAT;
			break;
		case DOUBLE:
			sz = SZDOUBLE;
			break;
		case LONG:
			sz = SZLONG;
			break;
		case SHORT:
			sz = SZSHORT;
			break;
		case UCHAR:
			sz = SZCHAR;
			break;
		case USHORT:
			sz = SZSHORT;
			break;
		case UNSIGNED:
			sz = SZINT;
			break;
		case ULONG:
			sz = SZLONG;
			break;
	}

	if( sz==0 ) 
	{
		uerror( "unknown size");
		return( SZINT );
	}
	return( sz * mult );
}
#endif
