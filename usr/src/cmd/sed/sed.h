/*	@(#)sed.h	2.1		*/
/*
 * sed -- stream  editor
 *
 * Copyright 1975 Bell Telephone Laboratories, Incorporated
 *
 */

/*
 * define some macros for rexexp.h
 */

#define INIT	extern char *cp, *badp;	/* cp points to RE string */\
		register char *sp = cp;
#define GETC()		(*sp++)
#define PEEKC()		(*sp)
#define UNGETC(c)	(--sp)
#define RETURN(c)	cp = sp; return(ep);
#define ERROR(c)	{ cp = sp; return(badp); }

#define CEND	16
#define CLNUM	14

#define NLINES  256
#define DEPTH   20
#define PTRSIZE 100
#define RESIZE  5000
#define ABUFSIZE        20
#define LBSIZE  4000
#define ESIZE   256
#define LABSIZE 50

extern union reptr     *abuf[];
extern union reptr **aptr;
extern char    genbuf[];
extern char    *lbend;
extern char	*lcomend;
extern long    lnum;
extern char    linebuf[];
extern char    holdsp[];
extern char    *spend;
extern int     nflag;
extern long    tlno[];

#define ACOM    01
#define BCOM    020
#define CCOM    02
#define CDCOM   025
#define CNCOM   022
#define COCOM   017
#define CPCOM   023
#define DCOM    03
#define ECOM    015
#define EQCOM   013
#define FCOM    016
#define GCOM    027
#define CGCOM   030
#define HCOM    031
#define CHCOM   032
#define ICOM    04
#define LCOM    05
#define NCOM    012
#define PCOM    010
#define QCOM    011
#define RCOM    06
#define SCOM    07
#define TCOM    021
#define WCOM    014
#define CWCOM   024
#define YCOM    026
#define XCOM    033


union   reptr {
        struct reptr1 {
                char    *ad1;
                char    *ad2;
                char    *re1;
                char    *rhs;
                FILE    *fcode;
                char    command;
                char    gfl;
                char    pfl;
                char    inar;
                char    negfl;
        } r1;
        struct reptr2 {
                char    *ad1;
                char    *ad2;
                union reptr     *lb1;
                char    *rhs;
                FILE    *fcode;
                char    command;
                char    gfl;
                char    pfl;
                char    inar;
                char    negfl;
        } r2;
};
extern union reptr ptrspace[];



struct label {
        char    asc[9];
        union reptr     *chain;
        union reptr     *address;
};



extern int     eargc;

extern union reptr     *pending;
extern char    *badp;
char    *compile();
char    *ycomp();
char    *address();
char    *text();
char    *compsub();
struct label    *search();
char    *gline();
char    *place();
