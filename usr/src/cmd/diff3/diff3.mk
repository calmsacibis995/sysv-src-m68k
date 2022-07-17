#	diff3 make file
#	SCCS:	@(#)diff3.mk	2.1	

SL = /usr/src/cmd
RDIR = $(SL)/diff3
REL = current
SSID = -r`gsid diff3 $(REL)`
CSID = -r`gsid diff3prog $(REL)`
MKSID = -r`gsid diff3.mk $(REL)`
LIST = lp

OL =
INSBIN = $(OL)/usr/bin
INSLIB = $(OL)/usr/lib
INSB = /etc/install -f $(INSBIN)
INSL = /etc/install -f $(INSLIB)
CFLAGS = -O
LDFLAGS = -s # -n
TESTDIR = .
SHSOURCE = diff3.sh
CSOURCE = diff3prog.c

compile all: diff3 diff3prog

diff3:
	cp diff3.sh $(TESTDIR)/diff3

diff3prog: diff3prog.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/diff3prog diff3prog.c

install: all
	$(INSB) $(TESTDIR)/diff3
	$(INSL) $(TESTDIR)/diff3prog

clean:

clobber: clean
	rm -f $(TESTDIR)/diff3 $(TESTDIR)/diff3prog

delete:	clobber
	rm -f $(SHSOURCE) $(CSOURCE)

listing:
	pr diff3.mk $(SHSOURCE) $(CSOURCE) | $(LIST)

listdif3:
	pr $(SHSOURCE) | $(LIST)

listdif3p:
	pr $(CSOURCE) | $(LIST)

listmk:
	pr diff3.mk | $(LIST)

edit:
	get -e s.diff3.sh

delta:
	delta s.diff3.sh

diff3pedit:
	get -e s.diff3prog.c

diff3pdelta:
	delta s.diff3prog.c

mkedit:
	get -e s.diff3.mk

mkdelta:
	delta s.diff3.mk

# these targets use "gsid", not part of normal distribution

build:	bldmk blddif3p
	get -p $(SSID) s.diff3.sh $(REWIRE) > $(RDIR)/diff3.sh

blddif3p:
	get -p $(CSID) s.diff3prog.c $(REWIRE) > $(RDIR)/diff3prog.c

bldmk:
	get -p $(MKSID) s.diff3.mk > $(RDIR)/diff3.mk
