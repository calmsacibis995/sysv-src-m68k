
# line 3 "gram.y"
#include "defs"

# line 8 "gram.y"
typedef union 
	{
	SHBLOCK yshblock;
	DEPBLOCK ydepblock;
	NAMEBLOCK ynameblock;
	CHARSTAR ycharstring;
	} YYSTYPE;
# define NAME 257
# define SHELLINE 258
# define START 259
# define COLON 260
# define DOUBLECOLON 261
# define EQUAL 262
# define A_STRING 263
# define VERSION 264
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;

# line 25 "gram.y"
DEPBLOCK pp;
FSTATIC SHBLOCK prevshp;

FSTATIC NAMEBLOCK lefts[NLEFTS];
NAMEBLOCK leftp;
FSTATIC int nlefts;

LINEBLOCK lp, lpp;
FSTATIC DEPBLOCK prevdep;
FSTATIC int sepc;
# define YYERRCODE 256

# line 140 "gram.y"


/*	@(#)lex.c	3.1	*/

#include "ctype.h"
CHARSTAR zznextc;	/* zero if need another line; otherwise points to next char */
int yylineno;
static char inmacro = NO;

yylex()
{
	register CHARSTAR p;
	register CHARSTAR q;
	static char word[128];
	CHARSTAR pword;

	pword = word;
	if(zznextc == 0)
		return( nextlin() );

	while( isspace(*zznextc) )
		++zznextc;

	if(inmacro == YES)
	{
		inmacro = NO;
		yylval.ycharstring = copys(zznextc);
		zznextc = 0;
		return(A_STRING);
	}

	if(*zznextc == CNULL)
		return( nextlin() );

	if(*zznextc == KOLON)
	{
		if(*++zznextc == KOLON)
		{
			++zznextc;
			return(DOUBLECOLON);
		}
		else
			return(COLON);
	}

	if(*zznextc == EQUALS)
	{
		inmacro = YES;
		++zznextc;
		return(EQUAL);
	}

	if(*zznextc == SKOLON)
		return( retsh(zznextc) );

	p = zznextc;
	q = word;

	while( ! ( funny[*p] & TERMINAL) )
		*q++ = *p++;

	if(p != zznextc)
	{
		*q = CNULL;
		yylval.ycharstring = copys(pword);
		if(*p == RCURLY)
		{
			zznextc = p+1;
			return(VERSION);
		}
		if(*p == LCURLY)
			p++;
		zznextc = p;
		return(NAME);
		}

	else
	{
		fprintf(stderr,"Bad character %c (octal %o), line %d",
			*zznextc,*zznextc,yylineno);
		fatal(Nullstr);
	}
	return(0);	/* never executed */
}


retsh(q)
register CHARSTAR q;
{
	register CHARSTAR p;
	register int c;
	extern CHARSTAR *linesptr;
	SHBLOCK sp;

	for(p=q+1 ; *p==BLANK||*p==TAB ; ++p)  ;

	sp = ALLOC(shblock);
	sp->nextsh = 0;
	sp->shbp = (fin == NULL ? p : copys(p) );
	yylval.yshblock = sp;
	zznextc = 0;
/*
 *	The following if-else "thing" eats up newlines within
 *	shell blocks.
 */
	if(fin == NULL)
	{
		if(linesptr[0])
			while(linesptr[1] && equal(linesptr[1], "\n"))
			{
				yylineno++;
				linesptr++;
			}
	}
	else
	{
		while((c = GETC()) == NEWLINE)
			yylineno++;
		if(c != EOF)
			ungetc(c, fin);
	}
	return(SHELLINE);
}

nextlin()
{
	register char c;
	register CHARSTAR p, t;
	static char yytext[INMAX];
	static CHARSTAR yytextl = yytext+INMAX;
	CHARSTAR text;
	char templin[INMAX];
	char lastch;
	CHARSTAR lastchp;
	extern CHARSTAR *linesptr;
	int incom;
	int kc;
	int nflg;
	int poundflg;

again:
	incom = 0;
	zznextc = 0;
	poundflg = 0;

	if(fin == NULL)
	{
		if( (text = *linesptr++) == 0)
			return(0);
		++yylineno;
		copstr(yytext, text);
	}

	else
	{
		yytext[0] = CNULL;
		for(p=yytext ; ; ++p)
		{
			if(p > yytextl)
				fatal("line too long");
			kc = GETC();
			if(kc == EOF)
			{
				*p = CNULL;
				return(0);
			}
			else if(kc == SKOLON)
				++incom;
			else if (kc == TAB && p == yytext)
				++incom;
			else if (kc==POUND && !incom && yytext[0] != TAB)
			{
				poundflg++;
				kc = CNULL;
			}
			else if (kc == NEWLINE)
			{
				++yylineno;
				if(p==yytext || p[-1]!=BACKSLASH)
					break;
				if(incom || yytext[0] == TAB)
					*p++ = NEWLINE;
				else
					p[-1] = BLANK;
				nflg = YES;
				while( kc = GETC())
				{
					if(kc != TAB && kc != BLANK && kc != NEWLINE)
						break;
					if(incom || yytext[0] == TAB)
					{
						if(nflg == YES && kc == TAB)
						{
							nflg = NO;
							continue;
						}
						if(kc == NEWLINE)
						{
							nflg = YES;
						}

						*p++ = kc;
					}
					if(kc == NEWLINE)
						++yylineno;
				}

				if(kc == EOF)
				{
					*p = CNULL;
					return(0);
				}
			}
			*p = kc;
		}
		*p = CNULL;
		text = yytext;
	}

	c = text[0];

	if(c == TAB)  
		return( retsh(text) );

/*
 *	DO include FILES HERE.
 */
	if(sindex(text, "include") == 0 && (text[7] == BLANK || text[7] == TAB))
	{
		CHARSTAR pfile;

		for(p = &text[8]; *p != CNULL; p++)
			if(*p != TAB ||
			   *p != BLANK)
				break;
		pfile = p;
		for(;	*p != CNULL	&&
			*p != NEWLINE	&&
			*p != TAB	&&
			*p != BLANK; p++);
		if(*p != CNULL)
			*p = CNULL;

/*
 *	Start using new file.
 */
		fstack(pfile, &fin, &yylineno);
		goto again;
	}
	if(isalpha(c) || isdigit(c) || c==BLANK || c==DOT)
		for(p=text+1; *p!=CNULL; p++)
			if(*p == KOLON || *p == EQUALS)
				break;

/* substtitute for macros on dependency line up to the semicolon if any */
	if(*p != EQUALS)
	{
		for(t = yytext ; *t!=CNULL && *t!=SKOLON ; ++t);

		lastchp = t;
		lastch = *t;
		*t = CNULL;

		subst(yytext, templin);	/* Substitute for macros on dep lines */

		if(lastch)
		{
			for(t = templin ; *t ; ++t);
			*t = lastch;
			while( *++t = *++lastchp ) ;
		}

		p = templin;
		t = yytext;
		while( *t++ = *p++ );
	}

	if(poundflg == 0 || yytext[0] != CNULL)
	{
		zznextc = text;
		return(START);
	}
	else
		goto again;
}

#include "stdio.h"

/*
 *	GETC automatically unravels stacked include files. That is,
 *	during include file processing, when a new file is encountered
 *	fstack will stack the FILE pointer argument. Subsequent
 *	calls to GETC with the new FILE pointer will get characters
 *	from the new file. When an EOF is encountered, GETC will
 *	check to see if the file pointer has been stacked. If so,
 *	a character from the previous file will be returned.
 *	The external references are "GETC()" and "fstack(fname,stream,lno)".
 *	"Fstack(stfname,ream,lno)" is used to stack an old file pointer before
 *	the new file is assigned to the same variable. Also stacked are the
 *	file name and the old current lineno, generally, yylineno.
 */



static int morefiles;
static struct sfiles
{
	char sfname[64];
	FILE *sfilep;
	int syylno;
} sfiles[20];

GETC()
{
	register int c;

	c = getc(fin);
	while(c == EOF && morefiles)
	{
		fclose(fin);
		yylineno = sfiles[--morefiles].syylno;
		fin = sfiles[morefiles].sfilep;
		c = getc(fin);
	}
	return(c);
}
fstack(newname, oldfp, oldlno)
register char *newname;
register FILE **oldfp;
register int *oldlno;
{
	if(access(newname, 4) != 0)
/*
 *	This get line can be removed if used elsewhere than make.
 */
	if(get(newname, CD, Nullstr) == NO)
		fatal1("Cannot read or get %s", newname);
	if(IS_ON(DBUG))
		printf("Include file: \"%s\"\n", newname);
/*
 *	Stack the new file name, the old file pointer and the
 *	old yylineno;
 */
	strcat(sfiles[morefiles].sfname, newname);
	sfiles[morefiles].sfilep = *oldfp;
	sfiles[morefiles++].syylno = *oldlno;
	yylineno = 0;
	if((*oldfp=fopen(newname, "r")) == NULL)
		fatal1("Cannot open %s", newname);
}

yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 22
# define YYLAST 26
yytabelem yyact[]={

    16,    17,    17,    22,    12,    12,     4,    14,    15,     3,
    23,    20,     7,     8,    13,     5,     2,     1,    11,     9,
    10,     6,    18,    19,     0,    21 };
yytabelem yypact[]={

 -1000,  -250, -1000,  -245, -1000, -1000,  -253,  -262, -1000,  -247,
 -1000,  -252,  -263, -1000, -1000, -1000,  -260, -1000, -1000,  -248,
 -1000, -1000, -1000, -1000 };
yytabelem yypgo[]={

     0,    23,    22,    21,    13,    19,    18,    17,    16,    15,
    14 };
yytabelem yyr1[]={

     0,     7,     7,     8,     8,     8,     8,     9,     3,     3,
     4,     4,     5,     5,     6,     6,    10,    10,     2,     2,
     1,     1 };
yytabelem yyr2[]={

     0,     0,     4,     2,     4,     9,     2,     7,     3,     5,
     3,     5,     1,     2,     3,     5,     3,     3,     1,     3,
     3,     5 };
yytabelem yychk[]={

 -1000,    -7,    -8,   259,   256,    -9,    -3,   257,    -4,    -5,
    -4,    -6,   257,   -10,   260,   261,   262,   264,    -2,    -1,
   258,    -4,   263,   258 };
yytabelem yydef[]={

     1,    -2,     2,     3,     6,     4,    12,    10,     8,    18,
     9,    13,    10,    14,    16,    17,     0,    11,     5,    19,
    20,    15,     7,    21 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"NAME",	257,
	"SHELLINE",	258,
	"START",	259,
	"COLON",	260,
	"DOUBLECOLON",	261,
	"EQUAL",	262,
	"A_STRING",	263,
	"VERSION",	264,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"file : /* empty */",
	"file : file comline",
	"comline : START",
	"comline : START macrodef",
	"comline : START namelist deplist shellist",
	"comline : error",
	"macrodef : NAME EQUAL A_STRING",
	"namelist : name",
	"namelist : namelist name",
	"name : NAME",
	"name : NAME VERSION",
	"deplist : /* empty */",
	"deplist : dlist",
	"dlist : sepchar",
	"dlist : dlist name",
	"sepchar : COLON",
	"sepchar : DOUBLECOLON",
	"shellist : /* empty */",
	"shellist : shlist",
	"shlist : SHELLINE",
	"shlist : shlist SHELLINE",
};
#endif /* YYDEBUG */
/*	@(#)yaccpar	1.9	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** global variables used by the parser
*/
YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
int yys[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 5:
# line 44 "gram.y"
 {
	    if(mainname == NULL && IS_OFF(INTRULE))
		if(lefts[0]->namep[0] != DOT || any(lefts[0]->namep, SLASH) )
			mainname = lefts[0];
	    while( --nlefts >= 0)
	    {
		leftp = lefts[nlefts];
		if(leftp->septype == 0)
			leftp->septype = sepc;
		else if(leftp->septype != sepc)
			fprintf(stderr, "Inconsistent rules lines for `%s'\n",
				leftp->namep);
		else if(sepc==ALLDEPS && *(leftp->namep)!=DOT && yypvt[-0].yshblock!=0)
		{
			for(lp=leftp->linep; lp->nextline!=0; lp=lp->nextline)
			    if(lp->shp)
				fprintf(stderr, "Multiple rules lines for `%s'\n",
				    leftp->namep);
		}

		lp = ALLOC(lineblock);
		lp->nextline = 0;
		lp->depp = yypvt[-1].ydepblock;
		lp->shp = yypvt[-0].yshblock;

		if(equal(leftp->namep, ".SUFFIXES") && yypvt[-1].ydepblock==0)
			leftp->linep = 0;
		else if(leftp->linep == 0)
			leftp->linep = lp;
		else
		{
			for(lpp = leftp->linep; lpp->nextline!=0;
				lpp = lpp->nextline) ;
				if(sepc==ALLDEPS && leftp->namep[0]==DOT)
					lpp->shp = 0;
			lpp->nextline = lp;
		}
	    }
	} break;
case 7:
# line 86 "gram.y"

	{
		setvar(yypvt[-2].ynameblock, yypvt[-0].ycharstring);
	} break;
case 8:
# line 92 "gram.y"
 { lefts[0] = yypvt[-0].ynameblock; nlefts = 1; } break;
case 9:
# line 93 "gram.y"
 { lefts[nlefts++] = yypvt[-0].ynameblock;
	    	if(nlefts>NLEFTS) fatal("Too many lefts"); } break;
case 10:
# line 97 "gram.y"

	{
	if((yyval.ynameblock = srchname(yypvt[-0].ynameblock)) == 0)
		yyval.ynameblock = makename(yypvt[-0].ynameblock);
	} break;
case 11:
# line 102 "gram.y"
 
	{
	if((yyval.ynameblock = srchname(yypvt[-1].ynameblock)) == 0)
		yyval.ynameblock = makename(yypvt[-1].ynameblock);
	} break;
case 12:
# line 109 "gram.y"
 { fatal1("Must be a separator on rules line %d", yylineno); } break;
case 14:
# line 113 "gram.y"
 { prevdep = 0;  yyval.ydepblock = 0; } break;
case 15:
# line 114 "gram.y"
 {
			  pp = ALLOC(depblock);
			  pp->nextdep = 0;
			  pp->depname = yypvt[-0].ynameblock;
			  if(prevdep == 0) yyval.ydepblock = pp;
			  else  prevdep->nextdep = pp;
			  prevdep = pp;
			  } break;
case 16:
# line 124 "gram.y"
 { sepc = ALLDEPS; } break;
case 17:
# line 125 "gram.y"
 { sepc = SOMEDEPS; } break;
case 18:
# line 128 "gram.y"
 {yyval.yshblock = 0; } break;
case 19:
# line 129 "gram.y"
 { yyval.yshblock = yypvt[-0].yshblock; } break;
case 20:
# line 132 "gram.y"
 { yyval.yshblock = yypvt[-0].yshblock;  prevshp = yypvt[-0].yshblock; } break;
case 21:
# line 133 "gram.y"
 { yyval.yshblock = yypvt[-1].yshblock;
			prevshp->nextsh = yypvt[-0].yshblock;
			prevshp = yypvt[-0].yshblock;
			} break;
	}
	goto yystack;		/* reset registers in driver code */
}
