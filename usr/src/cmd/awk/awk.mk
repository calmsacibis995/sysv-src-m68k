#	awk make file
#	SCCS:	@(#)awk.mk	2.1	

OL =
SL = /usr/src/cmd
UBIN = $(OL)/usr/bin
RDIR = $(SL)/awk
YACCRM=-rm
TESTDIR = .
INS = /etc/install -f $(UBIN)
B10 =
B11 =
YFLAGS = -d
LDFLAGS = -s
CFLAGS = -O
REL = current
LIST = lp
SOURCE = EXPLAIN README awk.def awk.g.y awk.h awk.lx.l b.c lib.c main.c \
		parse.c proc.c run.c token.c tokenscript tran.c
FILES = proctab.o awk.lx.o b.o main.o token.o tran.o lib.o run.o parse.o
MAKE = make

compile all:	awk
	:

awk:	$(FILES) awk.g.o
	$(CC) $(LDFLAGS) awk.g.o $(FILES) -lm -o $(TESTDIR)/awk

install:	awk
	$(INS) $(TESTDIR)/awk

$(FILES):	awk.h awk.def
run.o:	awk.h awk.def
	cc $(CFLAGS) $(B11) -c run.c
token.c:	awk.h 
	ed - <tokenscript
proc:   awk.h proc.c token.c
	cc -s -O -o proc proc.c token.c
proctab.c: proc
	-./proc > proctab.c
awk.g.o:	awk.def awk.g.c
	$(CC) $(CFLAGS) $(B10) -c awk.g.c

awk.g.h awk.g.c:	awk.g.y
	-$(YACC) $(YFLAGS) awk.g.y && mv y.tab.h awk.g.h && mv y.tab.c awk.g.c

awk.h:	awk.g.h 
	-cmp -s awk.g.h awk.h || cp awk.g.h awk.h

awk.lx.o:	awk.lx.c
	$(CC) $(CFLAGS) -c awk.lx.c

awk.lx.c:	awk.lx.l
	$(LEX) $(LFLAGS) awk.lx.l
	mv lex.yy.c awk.lx.c

build:	bldmk
	get -p -r`gsid awk $(REL)` s.awk.src $(REWIRE) | ntar -d $(RDIR) -g
	cd $(RDIR); $(YACC) $(YFLAGS) awk.g.y
	cd $(RDIR); mv y.tab.c awk.g.c; rm y.tab.h

bldmk:;	get -p -r`gsid awk.mk $(REL)` s.awk.mk > $(RDIR)/awk.mk

listing:;	pr awk.mk $(SOURCE) | $(LIST)

listmk:;	pr awk.mk | $(LIST)

edit:;	get -e -p s.awk.src | ntar -g

delta:;	ntar -p $(SOURCE) > awk.src
	delta s.awk.src
	rm -f $(SOURCE)

mkedit:;	get -e s.awk.mk
mkdelta:;	delta s.awk.mk

clean:;	-rm -f *.o temp* core proc proctab.c
	$(YACCRM) -f awk.lx.c awk.g.c awk.g.h

clobber:
	-rm -f $(TESTDIR)/awk

delete:	clobber clean
	rm -f $(SOURCE)
