/*	@(#)ftrees.c	2.1.1.1		*/
#include "mfile1.h"
#undef SZINT
#undef SZSHORT
#undef SZLONG
#undef INT
#undef ICON

#include "pccdefs"


/* ftrees - tree translating primitives used by onepass f77
 *
 * The functions which generate code gen nodes do so in a postfix order.
 * This makes life very easy for a rebuilder (get the node; if it's got
 * a right side, get that; if it's got a left side get that; return node;)
 *
 *
 * The stacking/building routines are:
 *
 * nstack - stack a node
 * bldtree - build tree from stacked nodes
 * npop - used by bldtree (kept hidden)
 *
 */

#define NNODES 512

static NODE *nlist[NNODES], **curnode = nlist;

void
nstack(np)
NODE *np;
{
	if (curnode == (nlist + NNODES))
		fatal("out of node storage");
	else
		*curnode++ = np;
}


static NODE *
npop()
{
	if (curnode == nlist)
		return (NULL);
	else
		return (*--curnode);
}


NODE *
bldtree()
{
	register NODE *np;

	if ((np = npop()) != NULL)
		switch (np->in.op)
		{
		case P2GOTO:
			if (np->in.type == 1)
				goto leaf;		/* goto 10 */
			else
				goto unary;		/* goto i,(10,11,12) */
		case P2PLUS:
		case P2PLUSEQ:
		case P2MINUS:
		case P2STAR:
		case P2STAREQ:
		case P2BITAND:
		case P2BITOR:
		case P2BITXOR:
		case P2QUEST:
		case P2COLON:
		case P2ANDAND:
		case P2OROR:
		case P2LISTOP:
		case P2ASSIGN:
		case P2COMOP:
		case P2SLASH:
		case P2MOD:
		case P2LSHIFT:
		case P2RSHIFT:
		case P2CALL:
		case P2BITNOT:
		case P2EQ:
		case P2NE:
		case P2LE:
		case P2LT:
		case P2GE:
		case P2GT:
		case P2CBRANCH:			/* binary ops */
			np->in.right = bldtree();

	unary:
		case P2NOT:
		case P2INDIRECT:
		case P2NEG:
		case P2CONV:
#ifndef SGS2
		case P2FORCE:
#endif
		case P2CALL0:			/* unary ops */
		case P2FUNARG:
#ifdef SGS2
		case UNARY AND:
#endif
			np->in.left = bldtree();

	leaf:
#ifdef SGS2
		case P2RNODE:
#endif
		case P2LABEL:
		case P2NAME:
		case P2ICON:
		case P2REG:
		case P2VPARAM:			/* leaves */
		case P2VAUTO:
		case P2SNODE:
			break;
		
		default:
			fatali("unrecognized node op %d", np->in.op);
			/* NOTREACHED */
		}
	else
		fatal("popping nonexistent node in translation");
	return (np);
}

getlab()
{
	static crslab = 10;
	return( ++crslab );
}

tprint( t )
register TWORD t; 
{
	 /* output a nice description of the type of t */
	static char * tnames[] = 
	{
		"undef",
		"farg",
		"char",
		"short",
		"int",
		"long",
		"float",
		"double",
		"strty",
		"unionty",
		"enumty",
		"moety",
		"uchar",
		"ushort",
		"unsigned",
		"ulong",
		"?", "?"
	};

	for(;; t = DECREF(t) )
	{
		if( ISPTR(t) ) printf( "PTR " );
		else if( ISFTN(t) ) printf( "FTN " );
		else if( ISARY(t) ) printf( "ARY " );
		else 
		{
			printf( "%s", tnames[t] );
			return;
		}
	}
}

eobl2db() {}

ispow2( c ) 
register CONSZ c; 
{
	register i;
	if( c <= 0 || (c&(c-1)) ) return(-1);
	for( i=0; c>1; ++i) c >>= 1;
	return(i);
}

ecode( p )
register NODE *p;
{
	/* standard version of writing the tree nodes */
/*	if( nerrors ) return;

# ifdef SDB
	sdbline();
# endif
*/

/*
 *	BEGIN: ported from VAX pcc to make two pass version
 */

# ifndef FONEPASS
	printf( ".%d\t%s\n", lineno, ftitle );
	prtree( p );
# else			/* else we have a ONEPASS */
	p2tree( p );
	p2compile( p );
# endif TWOPASS

/*
 *	END: ported from VAX pcc to make two pass version
 */
}

# ifdef STDPRTREE /****************************************/

# ifndef RNODNAME
# define RNODNAME LABFMT
# endif

# ifndef FONEPASS

prtree(p) register NODE *p; {

	register struct symtab *q;
	register ty;

# ifdef MYPRTREE
	MYPRTREE(p);  /* local action can be taken here; then return... */
#endif

	ty = optype(p->in.op);

	printf( "%d\t", p->in.op );

	if( ty == LTYPE ) {
		printf( CONFMT, p->tn.lval );
		printf( "\t" );
		}
	if( ty != BITYPE ) {
		if( p->in.op == NAME || p->in.op == ICON ) printf( "0\t" );
		else printf( "%d\t", p->tn.rval );
		}

	printf( "%o\t", p->in.type );

	/* handle special cases */

	switch( p->in.op ){

	case NAME:
	case ICON:
		/* print external name */
		if( p->tn.rval == NONAME ) printf( "\n" );
		else if( p->tn.rval >= 0 ){
			q = &stab[p->tn.rval];
			printf(  "%.8s\n", exname(q->sname) );
			}
		else { /* label */
			printf( LABFMT, -p->tn.rval );
			}
		break;

	case STARG:
	case STASG:
	case STCALL:
	case UNARY STCALL:
		/* print out size */
		/* use lhs size, in order to avoid hassles with the structure `.' operator */

		/* note: p->in.left not a field... */
		printf( CONFMT, (CONSZ) tsize( STRTY, p->in.left->fn.cdim, p->in.left->fn.csiz ) );
		printf( "\t%d\t\n", talign( STRTY, p->in.left->fn.csiz ) );
		break;

	default:
		printf(  "\n" );
		}

	if( ty != LTYPE ) prtree( p->in.left );
	if( ty == BITYPE ) prtree( p->in.right );

	}

# endif	not defined FONEPASS

char *
exname( p )
char *p;
{
	return( p );
}

p2tree(p)
register NODE *p;
{
	register ty;
	register NODE *l;
	register o;
	char temp[32];			/* place to dump label stuff */

# ifdef MYP2TREE
	MYP2TREE(p);  /* local action can be taken here; then return... */
# endif

	/* this routine sits painfully between pass1 and pass2 */
	ty = optype(o=p->in.op);
	p->tn.goal = 0;  /* an illegal goal, just to clear it out */
	p->tn.type = ttype( p->tn.type );  /* type gets second-pass (bits) */

	switch( o )
	{

	case TEMP:
	case NAME:
	case P2ICON:
	case VAUTO:
	case VPARAM:
		if( p->tn.rval == NONAME )
			p->in.name = (char *) 0;
		else if( p->tn.rval >= 0 )
		{
			 /* copy name from exname */
			register char *cp;
/*			cp = exname( p->tn.rval );
			p->in.name = tstr( cp );
			*/
		}
/*    strftn == 1 indicates return of struct, not needed for f77
		else if( p->tn.rval == - strftn )
		{
			sprintf( temp, RNODNAME, -p->tn.rval );
			p->in.name = tstr( temp );
		}
*/
		else
		{
			sprintf( temp, LABFMT, -p->tn.rval );
			p->in.name = tstr( temp );
		}
		break;

	case STARG:
	case STASG:
	case STCALL:
	case UNARY STCALL:
		/* set up size parameters */
		l = p->in.left;
		p->stn.stsize = tsize(STRTY,l->fn.cdim,l->fn.csiz);
		p->stn.stalign = talign(STRTY,l->fn.csiz);
		break;

# ifdef FONEPASS
		/* this should do something only if temporary regs are
		/* built into the tree by machine-dependent actions */
	case REG:
		rbusy( p->tn.rval, p->in.type );
# endif FONEPASS
	default:
		p->in.name = (char *) 0;
	}

	if( ty != LTYPE ) p2tree( p->in.left );
	if( ty == BITYPE ) p2tree( p->in.right );
}

# endif /******************************************/
