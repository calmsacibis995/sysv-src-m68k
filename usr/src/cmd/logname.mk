#	logname make file
#	SCCS:	@(#)logname.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSU = /etc/install -f $(UBIN)
CFLAGS = -O
LDFLAGS = -s # -n
SOURCE = logname.c
LIST = lp

compile all: logname

logname: logname.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/logname logname.o -lPW

logname.o: logname.c
	$(CC) $(CFLAGS) -c logname.c

install: all
	$(INSU) $(TESTDIR)/logname

clean:
	rm -f logname.o

clobber:
	rm -f $(TESTDIR)/logname

delete:	clean clobber
	rm -f $(SOURCE)

# other targets

listing:
	pr logname.mk $(SOURCE) | $(LIST)

listmk:
	pr logname.mk | $(LIST)

edit:
	get -e s.logname.c

delta:
	delta s.logname.c

mkedit:
	get -e s.logname.mk

mkdelta:
	delta s.logname.mk

# these targets use utilities not in normal distribution

SL = /usr/src/cmd
RDIR = $(SL)
REL = current
CSID = -r`gsid logname $(REL)`
MKSID = -r`gsid logname.mk $(REL)`

build:	bldmk
	get -p $(CSID) s.logname.c $(REWIRE) > $(RDIR)/logname.c

bldmk:
	get -p $(MKSID) s.logname.mk > $(RDIR)/logname.mk
