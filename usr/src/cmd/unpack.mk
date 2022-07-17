#	unpack (pcat) make file
#	SCCS:	@(#)unpack.mk	2.1	

OL = 
TESTDIR = .
INSUB = /etc/install -f $(UBIN)
UBIN = $(OL)/usr/bin
CFLAGS = -O
LDFLAGS = -s
SOURCE = unpack.c

compile all: unpack

unpack:	unpack.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/unpack unpack.o
unpack.o: unpack.c
	$(CC) $(CFLAGS) -c unpack.c

pcat:	unpack

install: all
	$(INSUB) $(TESTDIR)/unpack
	rm -f $(UBIN)/pcat
	ln $(UBIN)/unpack $(UBIN)/pcat

clean:
	rm -f unpack.o

clobber:
	  rm -f $(TESTDIR)/unpack

delete:	clobber
	rm -f $(SOURCE)
SL = /usr/src/cmd
RDIR = $(SL)
REL = current
CSID = -r`gsid unpack $(REL)`
MKSID = -r`gsid unpack.mk $(REL)`
LIST = lp

build:	bldmk
	get -p $(CSID) s.unpack.c $(REWIRE) > $(RDIR)/unpack.c
bldmk:
	get -p $(MKSID) s.unpack.mk > $(RDIR)/unpack.mk
	rm -f $(RDIR)/pcat.mk
	ln $(RDIR)/unpack.mk $(RDIR)/pcat.mk

listing:
	pr unpack.mk $(SOURCE) | $(LIST)
listmk: ;  pr unpack.mk | $(LIST)

edit:
	get -e s.unpack.c

delta:
	delta s.unpack.c

mkedit:  ;  get -e s.unpack.mk
mkdelta: ;  delta s.unpack.mk

