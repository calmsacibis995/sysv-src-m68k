#	mv/cp/ln make file
#	SCCS:	@(#)mv.mk	2.1.1.1	

LDFLAGS = -s # -n
TESTDIR = .
OL =
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)

compile all: mv

mv: mv.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/mv mv.o

mv.o: mv.c
	$(CC) $(CFLAGS) -c mv.c

install: all
	$(INSB) $(TESTDIR)/mv
	cp $(TESTDIR)/mv $(TESTDIR)/ln
	$(INSB) $(TESTDIR)/ln
	rm -f  $(BIN)/cp
	ln $(BIN)/ln $(BIN)/cp

clean:
	rm -f mv.o

clobber:
	rm -f $(TESTDIR)/mv $(TESTDIR)/ln

delete:	clean clobber
	rm -f mv.c

# other targets

listing:
	pr mv.mk mv.c | lp

listmk:
	pr mv.mk | lp

edit:
	get -e s.mv.c

delta:
	delta s.mv.c
	rm -f mv.c

mkedit:
	get -e s.mv.mk

mkdelta:
	delta s.mv.mk

# these targets use "gsid", not in normal distribution

SL = /usr/src/cmd
RDIR = $(SL)
REL = current
CSID = -r`gsid mv $(REL)`
MKSID = -r`gsid mv.mk $(REL)`

build:	bldmk
	get -p $(CSID) s.mv.c $(REWIRE) > $(RDIR)/mv.c

bldmk:
	get -p $(MKSID) s.mv.mk > $(RDIR)/mv.mk
	cd $(RDIR); rm -f ln.mk cp.mk
	cd $(RDIR); ln mv.mk cp.mk; ln mv.mk ln.mk
