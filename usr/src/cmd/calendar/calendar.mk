#	calendar make file
#	SCCS:	@(#)calendar.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
INSUB = /etc/install -f $(UBIN)
INSUL = /etc/install -f $(ULIB)
LDFLAGS = -s
CFLAGS = -O
SHSOURCE = calendar.sh
PSOURCE = calprog.c
SHFILES = calendar

compile all: calendar calprog

calendar:
	cp calendar.sh $(TESTDIR)/calendar

calprog: calprog.o
	$(CC) $(LDLAGS) -o $(TESTDIR)/calprog calprog.o

calprog.o: calprog.c
	$(CC) $(CFLAGS) -c calprog.c

install: all
	$(INSUB) $(TESTDIR)/calendar
	$(INSUL) $(TESTDIR)/calprog

clean: 
	rm -f calendar
	rm -f calprog.o

clobber:
	rm -f $(TESTDIR)/calendar
	rm -f $(TESTDIR)/calprog
calclobber:
	rm -f $(TESTDIR)/calendar
calpclobber:
	rm -f $(TESTDIR)/calprog

delete:	clobber
	rm -f $(SHSOURCE) $(PSOURCE)
caldelete:	calclobber
	rm -f $(SHSOURCE)
calpdelete:	calpclobber
	rm -f $(PSOURCE)

SL = /usr/src/cmd
RDIR = $(SL)/calendar
REL = current
LIST = lp

build:	bldmk bldcalp
	get -p -r`gsid calendar $(REL)` s.calendar.sh $(REWIRE) > $(RDIR)/calendar.sh
bldcalp:
	get -p -r`gsid calprog $(REL)` s.calprog.c $(REWIRE) > $(RDIR)/calprog.c
bldmk:  ;  get -p -r`gsid calendar.mk $(REL)` s.calendar.mk > $(RDIR)/calendar.mk

listing:
	pr calendar.mk $(SHSOURCE) $(PSOURCE) | $(LIST)
lstcal: ; pr $(SHSOURCE) | $(LIST)
lstcalp: ; pr $(PSOURCE) | $(LIST)
listmk: ;  pr calendar.mk | $(LIST)

caledit: ; get -e s.calendar.sh
calpedit: ; get -e s.calprog.c

caldelta: ; delta s.calendar.sh
calpdelta: ; delta s.calprog.c

mkedit:  ;  get -e s.calendar.mk
mkdelta: ;  delta s.calendar.mk

