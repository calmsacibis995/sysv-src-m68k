# @(#)vi.mk	2.1	
#
OL = 
TESTDIR = .

compile all: cmd libtermlib termcap src

cmd:
	@echo "Making commands"
	cd misc; make compile; cd ..

libtermlib:
	@echo "Making libtermlib.a"
	cd termlib; make compile; cd ..

termcap:
	@echo "Making termcap"
	cd termcap.db; make compile; cd ..

src:
	@echo "Making src"
	cd vax; make compile; cd ..

install:
	@echo "Installing cmd"
	cd misc; make install TESTDIR=$(TESTDIR) OL=$(OL); cd ..
	@echo "Installing libtermlib.a"
	cd termlib; make install TESTDIR=$(TESTDIR) OL=$(OL); cd ..
	@echo "Installing termcap and tabset"
	cd termcap.db; make install TESTDIR=$(TESTDIR) OL=$(OL);\
		make tabsets TESTDIR=$(TESTDIR) OL=$(OL); cd ..
	@echo "Installing src"
	cd vax; make install TESTDIR=$(TESTDIR) OL=$(OL); cd ..

clean:
	@echo "Cleaning subdirectories"
	cd misc; make clean; cd ..
	cd termlib; make clean; cd ..
	cd termcap.db; make clean; cd ..
	cd vax; make clean; cd ..

clobber:
	@echo "Clobbering subdirectories"
	cd misc; make clobber TESTDIR=$(TESTDIR); cd ..
	cd termlib; make clobber TESTDIR=$(TESTDIR); cd ..
	cd termcap.db; make clobber TESTDIR=$(TESTDIR); cd ..
	cd vax; make clobber TESTDIR=$(TESTDIR); cd ..

delete:
	cd misc; make delete; cd ..
	cd termlib; make delete; cd ..
	cd termcap.db; make delete; cd ..
	cd vax; make delete; cd ..
#
# info for this target not present in this distribution
#
manpages:
	@echo "Installing man pages"
	cd doc/man; make install -f man.mk
