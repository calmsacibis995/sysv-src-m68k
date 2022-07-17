#	@(#)makefile	2.1	

#***************************************************************************
#                                                                          *
#			UNIX Lorder (lorder) makefile			   *
#									   *
#***************************************************************************
#                                                                          *
#		Source Code Files					   *
#								           *
#	com/lorder.sh		lorder function shell script		   *
#									   *
#***************************************************************************

TESTDIR = ..

LPT = dpr -mx

CDIR = ../common

INSDIR = .

SFILES = $(CDIR)/lorder.sh


all:	lorder


lorder:	$(SFILES)
	sed 's/SGS//' < $(CDIR)/lorder.sh > lorder
	chmod +x lorder
	cp lorder $(TESTDIR)/lorder


listing:
	pr -n makefile $(SFILES) | $(LPT)
	@echo "files sent to lineprinter"

clean:
	-rm -f lorder

clobber:
	-rm -f $(TESTDIR)/lorder
