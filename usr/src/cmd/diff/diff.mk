#	diff make file
#	SCCS:	@(#)diff.mk	2.1	

TESTDIR = .
OL =
BIN= $(OL)/bin
ULIB = $(OL)/usr/lib
INSB = /etc/install -f $(BIN)
INSUL = /etc/install -f $(ULIB)
CFLAGS = -O
LDFLAGS = -s 
SOURCE = diff.c diffh.c

compile all: diff diffh

diff: diff.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/diff diff.o

diffh: diffh.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/diffh diffh.o

diff.o: diff.c
	$(CC) $(CFLAGS) -c diff.c

diffh.o: diffh.c
	$(CC) $(CFLAGS) -c diffh.c

install:  all
	$(INSB) $(TESTDIR)/diff
	rm -f $(ULIB)/diffh
	$(INSUL) $(TESTDIR)/diffh

clean:
	rm -f diff.o diffh.o

clobber:
	  rm -f $(TESTDIR)/diff $(TESTDIR)/diffh

delete:	clobber
	rm -f $(SOURCE)

# other targets

SL = $(OL)/usr/src/cmd
RDIR = $(SL)/diff
REL = current
CSID = -r`gsid diff $(REL)`
MKSID = -r`gsid diff.mk $(REL)`
LIST = lp

listing:
	pr diff.mk $(SOURCE) | $(LIST)

listmk:
	pr diff.mk | $(LIST)

edit:
	get -e -p s.diff.src | ntar -g

delta:
	ntar -p $(SOURCE) > diff.src
	delta s.diff.src
	rm -f $(SOURCE)

mkedit:
	get -e s.diff.mk

mkdelta:
	delta s.diff.mk

# the following targets use utilities not in the normal distribution

build:	bldmk
	get -p $(CSID) s.diff.src $(REWIRE) | ntar -d $(RDIR) -g

bldmk:
	get -p $(MKSID) s.diff.mk > $(RDIR)/diff.mk
