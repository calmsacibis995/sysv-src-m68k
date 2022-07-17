#	ed make file
#	SCCS:	@(#)ed.mk	2.1	

OL =
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)
TESTDIR = .
CFLAGS = -O
LDFLAGS = -s # -n
SOURCE = ed.c edfun

compile all: ed

ed: ed.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/ed ed.o

ed.o: ed.c
	if pdp11; \
	then \
		$(CC) $(CFLAGS) -S ed.c; \
		edfun ed.s; \
		$(CC) -c ed.s; \
	else \
		$(CC) $(CFLAGS) -c ed.c; \
	fi

install: all
	$(INSB) $(TESTDIR)/ed
	rm -f $(BIN)/red
	ln $(BIN)/ed $(BIN)/red

clean:
	  rm -f ed.o ed.s

clobber:
	  rm -f $(TESTDIR)/ed

delete:	clobber clean
	rm -f $(SOURCE)

# other targets

LIST = lp

listing:
	pr ed.mk $(SOURCE) | $(LIST)

listmk:
	pr ed.mk | $(LIST)

edit:
	get -e s.ed.c

edfunedit:
	get -e s.edfun

delta:
	delta s.ed.c

edfundelta:
	delta s.edfun

mkedit:
	get -e s.ed.mk

mkdelta:
	delta s.ed.mk

# these targets use "gsid", not in normal distribution

SL = $(OL)/usr/src/cmd
RDIR = $(SL)/ed
REL = current
CSID = -r`gsid ed $(REL)`
FUNSID = -r`gsid edfun $(REL)`
MKSID = -r`gsid ed.mk $(REL)`

build:	bldmk bldedfun
	get -p $(CSID) s.ed.c $(REWIRE) > $(RDIR)/ed.c

bldedfun:
	get -p $(FUNSID) s.edfun $(REWIRE) > $(RDIR)/edfun
	chmod 755 $(RDIR)/edfun

bldmk:
	get -p $(MKSID) s.ed.mk > $(RDIR)/ed.mk
