#	sed make file
#	SCCS:	@(#)sed.mk	2.1	

OL =
TESTDIR = .
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)
CFLAGS = -O
LDFLAGS = -s # -n
SOURCE = sed.h sed0.c sed1.c
FILES = sed0.o sed1.o
LIST = lp

compile all: sed

sed: $(FILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/sed $(FILES)

sed0.o: sed.h sed0.c
	$(CC) $(CFLAGS) -c sed0.c

sed1.o: sed.h sed1.c
	$(CC) $(CFLAGS) -c sed1.c

install: all
	$(INSB) $(TESTDIR)/sed

clean:
	  rm -f $(FILES)

clobber:
	  rm -f $(TESTDIR)/sed

delete:	clobber clean
	rm -f $(SOURCE)

# other targets

listing:
	pr sed.mk $(SOURCE) | $(LIST)

listmk:
	pr sed.mk | $(LIST)

mkedit:
	get -e s.sed.mk

mkdelta:
	delta s.sed.mk

# these targets use utilities not in normal distribution

SL = /usr/src/cmd
RDIR = $(SL)/sed
REL = current
CSID = -r`gsid sed $(REL)`
MKSID = -r`gsid sed.mk $(REL)`

build:	bldmk
	get -p $(CSID) s.sed.src $(REWIRE) | ntar -d $(RDIR) -g

bldmk:
	get -p $(MKSID) s.sed.mk > $(RDIR)/sed.mk

edit:
	get -e -p s.sed.src | ntar -g

delta:
	ntar -p $(SOURCE) > sed.src
	delta s.sed.src
	rm -f $(SOURCE)
