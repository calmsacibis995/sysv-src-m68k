#	regcmp make file
#	SCCS:	@(#)regcmp.mk	2.1	

OL = 
TESTDIR = .
INSUB = /etc/install -f $(UBIN)
UBIN = $(OL)/usr/bin
CFLAGS = -O
LDFLAGS = -s 
SOURCE = regcmp.c

compile all: regcmp

regcmp: regcmp.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/regcmp regcmp.o -lPW
regcmp.o: regcmp.c
	$(CC) $(CFLAGS) -c regcmp.c

install: all
	$(INSUB) $(TESTDIR)/regcmp

clean:
	  rm -f regcmp.o

clobber:
	  rm -f $(TESTDIR)/regcmp

delete:	clobber
	rm -f $(SOURCE)

SL = /usr/src/cmd
RDIR = $(SL)
REL = current
CSID = -r`gsid regcmp $(REL)`
MKSID = -r`gsid regcmp.mk $(REL)`
LIST = lp
 
build:	bldmk
	get -p $(CSID) s.regcmp.c $(REWIRE) > $(RDIR)/regcmp.c
bldmk:  ;  get -p $(MKSID) s.regcmp.mk > $(RDIR)/regcmp.mk

listing:
	pr regcmp.mk $(SOURCE) | $(LIST)
listmk: ;  pr regcmp.mk | $(LIST)

edit:
	get -e s.regcmp.c

delta:
	delta s.regcmp.c

mkedit:  ;  get -e s.regcmp.mk
mkdelta: ;  delta s.regcmp.mk
