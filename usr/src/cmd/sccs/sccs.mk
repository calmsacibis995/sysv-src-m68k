#	@(#)sccs.mk	2.1	 -- SCCS build

ARGS =
OL = 
TESTDIR = .

compile all: inssrc

inssrc:
	cd lib; $(MAKE)
	cd src; $(MAKE) OL=$(OL) TESTDIR=$(TESTDIR) $(ARGS) 

install:
	cd lib; $(MAKE) install
	cd src; $(MAKE) OL=$(OL) TESTDIR=$(TESTDIR) $(ARGS) install
	cd help.d; $(MAKE) OL=$(OL)

clean:
	cd lib; $(MAKE) clean
	cd src; $(MAKE) clean

clobber:
	cd lib; $(MAKE) clobber
	cd src; $(MAKE) TESTDIR=$(TESTDIR) clobber
