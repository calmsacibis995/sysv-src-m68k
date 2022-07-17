#	bfs make file
#	SCCS:	@(#)bfs.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
CFLAGS = -O
LDFLAGS = -s
LIBES = -lPW
SOURCE = bfs.c

compile all: bfs
	
bfs: bfs.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/bfs bfs.o $(LIBES)
bfs.o: bfs.c
	$(CC) $(CFLAGS) -c bfs.c

install: all
	$(INSUB) $(TESTDIR)/bfs

clean:
	rm -f bfs.o

clobber:
	rm -f $(TESTDIR)/bfs

SL = /usr/src/cmd
RDIR = $(SL)
REL = current
LIST = lp

delete:	clobber
	rm -f $(SOURCE)

listing:
	pr bfs.mk $(SOURCE) | $(LIST)

listmk:
	pr bfs.mk | $(LIST)

edit:
	get -e s.$(SOURCE)

delta:
	delta s.$(SOURCE)

mkedit:
	get -e s.bfs.mk

mkdelta:
	delta s.bfs.mk

# these targets use "gsid", not present in normal distribution

build:	bldmk
	get -p -r`gsid bfs $(REL)` s.$(SOURCE) $(REWIRE) > $(RDIR)/$(SOURCE)

bldmk:
	get -p -r`gsid bfs.mk $(REL)` s.bfs.mk > $(RDIR)/bfs.mk
