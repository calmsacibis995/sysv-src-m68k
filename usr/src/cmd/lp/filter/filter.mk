#	filter make file
#	SCCS:	@(#)filter.mk	2.1	

OL = 
SL	= /usr/src/cmd
RDIR	= $(SL)/lp/filter
REL	= current
OWN	= bin
GRP	= bin
LIST	= lp
TESTDIR = .
ULIB	= $(OL)/usr/lib
CFLAGS	= -O
LDFLAGS	= -s # -n
SOURCE	= hp2631a.c prx.c pprx.c
FILES	= hp2631a.o prx.o pprx.o
MAKE	= make
INSL = /etc/install -f $(ULIB)

compile all: hp2631a prx pprx
	:

hp2631a:	hp2631a.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/hp2631a hp2631a.o
prx:	prx.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/prx prx.o
pprx:	pprx.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/pprx pprx.o

hp2631a.o:	hp2631a.c
	$(CC) $(CFLAGS) -c hp2631a.c
prx.o:	prx.c
	$(CC) $(CFLAGS) -c prx.c
pprx.o:	pprx.c
	$(CC) $(CFLAGS) -c pprx.c

install:	all
	$(INSL) $(TESTDIR)/hp2631a
	$(INSL) $(TESTDIR)/prx
	$(INSL) $(TESTDIR)/pprx

#######################################################################
#################################DSL only section - for development use

build:	bldmk
	get -p -r`gsid filter $(REL)` s.filter.src $(REWIRE) | ntar -d $(RDIR) -g
bldmk:  ;  get -p -r`gsid filter.mk $(REL)` s.filter.mk > $(RDIR)/filter.mk

listing:
	pr filter.mk $(SOURCE) | $(LIST)
listmk: ;  pr filter.mk | $(LIST)

edit:
	get -e -p s.filter.src | ntar -g

delta:
	ntar -p $(SOURCE) > filter.src
	delta s.filter.src
	rm -f $(SOURCE)

mkedit:  ;  get -e s.filter.mk
mkdelta: ;  delta s.filter.mk
#######################################################################

clean:
	rm -f $(FILES)

clobber:
	rm -f $(TESTDIR)/hp2631a $(TESTDIR)/prx $(TESTDIR)/pprx

delete:	clobber clean
	rm -f $(SOURCE)
