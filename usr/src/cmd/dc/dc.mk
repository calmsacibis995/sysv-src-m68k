#	dc make file
#	SCCS:	@(#)dc.mk	2.1	

SL = /usr/src/cmd
RDIR = $(SL)/dc
REL = current
CSID = -r`gsid dc $(REL)`
MKSID = -r`gsid dc.mk $(REL)`
LIST = lp

OL =
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
TESTDIR = .
LDFLAGS = -s # 
CFLAGS = -O
SOURCE = dc.h dc.c

compile all: dc

dc:	dc.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/dc dc.o

dc.o:   dc.h dc.c
	$(CC) $(CFLAGS) -c dc.c

install: all
	$(INSUB) $(TESTDIR)/dc

clean:
	rm -f *.o

clobber: 
	  rm -f $(TESTDIR)/dc

delete:	clobber
	rm -f $(SOURCE)

listing:
	pr dc.mk $(SOURCE) | $(LIST)

listmk:
	pr dc.mk | $(LIST)

mkedit:
	get -e s.dc.mk

mkdelta:
	delta s.dc.mk

# these targets use "gsid" and "ntar", not part of normal distribution

build:	bldmk
	get -p $(CSID) s.dc.src $(REWIRE) | ntar -d $(RDIR) -g

bldmk:
	get -p $(MKSID) s.dc.mk > $(RDIR)/dc.mk

edit:
	get -e -p s.dc.src | ntar -g

delta:
	ntar -p $(SOURCE) > dc.src
	delta s.dc.src
	rm -f $(SOURCE)
