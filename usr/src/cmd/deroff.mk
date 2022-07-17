#	deroff make file
#	SCCS:	@(#)deroff.mk	2.1	
#	NOTE: This makefile is only necessary because of the need for the
#		base register option for cc on u370.

OL = 
TESTDIR = .
UBIN = $(OL)/usr/bin
SL = /usr/src/cmd
RDIR = $(SL)
INSUB = /etc/install -f $(UBIN)
REL = current
CSID = -r`gsid deroff $(REL)`
MKSID = -r`gsid deroff.mk $(REL)`
LIST = lp
B20 =
CFLAGS = -O $(B20)
LDFLAGS = -s
SOURCE = deroff.c
MAKE = make

compile all: deroff

deroff: deroff.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/deroff deroff.o

deroff.o: deroff.c
	$(CC) $(CFLAGS) -c deroff.c

install: all
	$(INSUB) $(TESTDIR)/deroff

clean:
	rm -f *.o

clobber:  
	rm -f $(TESTDIR)/deroff

delete:	clobber
	rm -f $(SOURCE)

build:	bldmk
	get -p $(CSID) s.deroff.c $(REWIRE) > $(RDIR)/deroff.c
bldmk:  ;  get -p $(MKSID) s.deroff.mk > $(RDIR)/deroff.mk

listing:
	pr deroff.mk $(SOURCE) | $(LIST)
listmk: ;  pr deroff.mk | $(LIST)

edit:
	get -e s.deroff.c

delta:
	delta s.deroff.c

mkedit:  ;  get -e s.deroff.mk
mkdelta: ;  delta s.deroff.mk
