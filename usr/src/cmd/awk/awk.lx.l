%Start A str chc sc reg comment

%{
#include	"awk.h"
#include	"awk.def"
#undef	input	/* defeat lex */
extern int	yylval;
extern int	mustfld;

int	lineno	= 1;
#ifdef	DEBUG
#	define	RETURN(x)	{if (dbg) ptoken(x); return(x); }
#else
#	define	RETURN(x)	return(x)
#endif
#define	CADD	cbuff[clen++]=yytext[0]; if(clen>=CBUFLEN-1) {yyerror("string too long", cbuff); BEGIN A;}
#define	CBUFLEN	150
char	cbuff[CBUFLEN];
int	clen, cflag;
%}

A	[a-zA-Z_]
B	[a-zA-Z0-9_]
D	[0-9]
WS	[ \t]

%%
	switch (yybgin-yysvec-1) {	/* witchcraft */
	case 0:
		BEGIN A;
		break;
	case sc:
		BEGIN A;
		RETURN('}');
	}

<A>^\n		lineno++;
<A>^{WS}*#.*\n	lineno++;	/* strip comment lines */
<A>{WS}		;
<A,reg>"\\"\n	lineno++;
<A>"||"		RETURN(BOR);
<A>BEGIN	RETURN(XBEGIN);
<A>END		RETURN(XEND);
<A>PROGEND	RETURN(EOF);
<A>"&&"		RETURN(AND);
<A>"!"		RETURN(NOT);
<A>"!="		{ yylval = NE; RETURN(RELOP); }
<A>"~"		{ yylval = MATCH; RETURN(MATCHOP); }
<A>"!~"		{ yylval = NOTMATCH; RETURN(MATCHOP); }
<A>"<"		{ yylval = LT; RETURN(RELOP); }
<A>"<="		{ yylval = LE; RETURN(RELOP); }
<A>"=="		{ yylval = EQ; RETURN(RELOP); }
<A>">="		{ yylval = GE; RETURN(RELOP); }
<A>">"		{ yylval = GT; RETURN(RELOP); }
<A>">>"		{ yylval = APPEND; RETURN(RELOP); }
<A>"++"		{ yylval = INCR; RETURN(INCR); }
<A>"--"		{ yylval = DECR; RETURN(DECR); }
<A>"+="		{ yylval = ADDEQ; RETURN(ASGNOP); }
<A>"-="		{ yylval = SUBEQ; RETURN(ASGNOP); }
<A>"*="		{ yylval = MULTEQ; RETURN(ASGNOP); }
<A>"/="		{ yylval = DIVEQ; RETURN(ASGNOP); }
<A>"%="		{ yylval = MODEQ; RETURN(ASGNOP); }
<A>"="		{ yylval = ASSIGN; RETURN(ASGNOP); }

<A>"$"{D}+	{	if (atoi(yytext+1)==0) {
				yylval = (int)lookup("$record", symtab, 0);
				RETURN(STRING);
			} else {
				yylval = fieldadr(atoi(yytext+1));
				RETURN(FIELD);
			}
		}
<A>"$"{WS}*	{ RETURN(INDIRECT); }
<A>NF		{ mustfld=1; yylval = (int)setsymtab(yytext, NULL, 0.0, NUM, symtab); RETURN(VAR); }
<A>({D}+("."?){D}*|"."{D}+)((e|E)("+"|-)?{D}+)?	{
		yylval = (int)setsymtab(yytext, NULL, atof(yytext), CON|NUM, symtab); RETURN(NUMBER); }
<A>"}"{WS}*\n	{ BEGIN sc; lineno++; RETURN(';'); }
<A>"}"		{ BEGIN sc; RETURN(';'); }
<A>;\n		{ lineno++; RETURN(';'); }
<A>\n		{ lineno++; RETURN(NL); }
<A>while	RETURN(WHILE);
<A>for		RETURN(FOR);
<A>if		RETURN(IF);
<A>else		RETURN(ELSE);
<A>next		RETURN(NEXT);
<A>exit		RETURN(EXIT);
<A>break	RETURN(BREAK);
<A>continue	RETURN(CONTINUE);
<A>print	{ yylval = PRINT; RETURN(PRINT); }
<A>printf	{ yylval = PRINTF; RETURN(PRINTF); }
<A>sprintf	{ yylval = SPRINTF; RETURN(SPRINTF); }
<A>split	{ yylval = SPLIT; RETURN(SPLIT); }
<A>substr	RETURN(SUBSTR);
<A>index	RETURN(INDEX);
<A>in		RETURN(IN);
<A>getline	RETURN(GETLINE);
<A>length	{ yylval = FLENGTH; RETURN(FNCN); }
<A>log		{ yylval = FLOG; RETURN(FNCN); }
<A>int		{ yylval = FINT; RETURN(FNCN); }
<A>exp		{ yylval = FEXP; RETURN(FNCN); }
<A>sqrt		{ yylval = FSQRT; RETURN(FNCN); }
<A>{A}{B}*	{ yylval = (int)setsymtab(yytext, tostring(""), 0.0, STR|NUM, symtab); RETURN(VAR); }
<A>\"		{ BEGIN str; clen=0; }

<A>#		{ BEGIN comment; }
<comment>\n	{ BEGIN A; lineno++; RETURN(NL); }
<comment>.	;

<A>.		{ yylval = yytext[0]; RETURN(yytext[0]); }

<reg>"["	{ BEGIN chc; clen=0; cflag=0; }
<reg>"[^"	{ BEGIN chc; clen=0; cflag=1; }

<reg>"?"	RETURN(QUEST);
<reg>"+"	RETURN(PLUS);
<reg>"*"	RETURN(STAR);
<reg>"|"	RETURN(OR);
<reg>"."	RETURN(DOT);
<reg>"("	RETURN('(');
<reg>")"	RETURN(')');
<reg>"^"	RETURN('^');
<reg>"$"	RETURN('$');
<reg>\\{D}{D}{D}	{ sscanf(yytext+1, "%o", &yylval); RETURN(CHAR); }
<reg>\\.	{	if (yytext[1]=='n') yylval = '\n';
			else if (yytext[1] == 't') yylval = '\t';
			else yylval = yytext[1];
			RETURN(CHAR);
		}
<reg>"/"	{ BEGIN A; unput('/'); }
<reg>\n		{ yyerror("newline in regular expression"); lineno++; BEGIN A; }
<reg>.		{ yylval = yytext[0]; RETURN(CHAR); }

<str>\"		{ char *s; BEGIN A; cbuff[clen]=0; s = tostring(cbuff);
		cbuff[clen] = ' '; cbuff[++clen] = 0;
		yylval = (int)setsymtab(cbuff, s, 0.0, CON|STR, symtab); RETURN(STRING); }
<str>\n		{ yyerror("newline in string"); lineno++; BEGIN A; }
<str>"\\\""	{ cbuff[clen++]='"'; }
<str,chc>"\\"n	{ cbuff[clen++]='\n'; }
<str,chc>"\\"t	{ cbuff[clen++]='\t'; }
<str,chc>"\\\\"	{ cbuff[clen++]='\\'; }
<str>.		{ CADD; }

<chc>"\\""]"	{ cbuff[clen++]=']'; }
<chc>"]"	{ BEGIN reg; cbuff[clen]=0; yylval = (int)tostring(cbuff);
		if (cflag==0) { RETURN(CCL); }
		else { RETURN(NCCL); } }
<chc>\n		{ yyerror("newline in character class"); lineno++; BEGIN A; }
<chc>.		{ CADD; }

%%

/* @(#)awk.lx.l	2.1		*/
input()
{
	register c;
	extern char *lexprog;

	if (yysptr > yysbuf)
		c = U(*--yysptr);
	else if (yyin == NULL)
		c = *lexprog++;
	else
		c = getc(yyin);
	if (c == '\n')
		yylineno++;
	else if (c == EOF)
		c = 0;
	return(c);
}

startreg()
{
	BEGIN reg;
}
