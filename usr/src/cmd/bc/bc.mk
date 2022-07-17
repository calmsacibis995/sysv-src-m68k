#	bc make file
#	SCCS:	@(#)bc.mk	2.1	

SL = /usr/src/cmd
RDIR = $(SL)/bc
REL = current
CSID = -r`gsid bc $(REL)`
MKSID = -r`gsid bc.mk $(REL)`
LIST = lp

OL =
TESTDIR = .
INSDIR = $(OL)/usr/bin
INSLIB = $(OL)/usr/lib
INSB = /etc/install -f $(INSDIR)
INSL = /etc/install -f $(INSLIB)
CFLAGS = -O
LDFLAGS = -s # -n
SOURCE = bc.y lib.b.sh
IFILE = bc.c

compile all: bc lib.b

bc:	$(IFILE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/bc $(IFILE)

$(IFILE):
	$(YACC) bc.y && mv y.tab.c $(IFILE)

lib.b:
	cp lib.b.sh $(TESTDIR)/lib.b

install: all
	$(INSB) $(TESTDIR)/bc
	$(INSL) $(TESTDIR)/lib.b

clean:
	rm -f $(IFILE)

clobber:
	rm -f $(TESTDIR)/bc $(TESTDIR)/lib.b

delete:	clobber
	rm -f $(SOURCE)

listing:
	pr bc.mk $(SOURCE) | $(LIST)

listmk:
	pr bc.mk | $(LIST)

mkedit:
	get -e s.bc.mk

mkdelta:
	delta s.bc.mk

# these targets use "gsid" and "ntar", not present in normal distribution

build:	bldmk
	get -p $(CSID) s.bc.src $(REWIRE) | ntar -d $(RDIR) -g
	cd $(RDIR); $(YACC) bc.y; mv y.tab.c bc.c

bldmk:
	get -p $(MKSID) s.bc.mk > $(RDIR)/bc.mk

edit:
	get -e -p s.bc.src | ntar -g

delta:
	ntar -p $(SOURCE) > bc.src
	delta s.bc.src
	rm -f $(SOURCE)
