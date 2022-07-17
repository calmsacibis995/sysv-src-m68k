#	@(#)makefile	2.1 
#
# makefile for libc/port
#
#
# The macro PROF is null by default, causing profiled object to be maintained.
# If profiled object is not desired, the reassignment PROF=@# should appear in
# the make command line.
#

INCROOT = 

.SUFFIXES: .p
CC=cc
CFLAGS=-O
PROF=

all:
	cd gen; INCROOT=$(INCROOT) make -e $(IGN) nonprof
	cd print; INCROOT=$(INCROOT) make -e $(IGN) nonprof
	cd stdio; INCROOT=$(INCROOT) make -e $(IGN) nonprof
	cd sys; INCROOT=$(INCROOT) make -e $(IGN) nonprof 
	cd x25; INCROOT=$(INCROOT) make -e $(IGN) nonprof

	$(PROF)cd gen; INCROOT=$(INCROOT) make -e $(IGN) prof
	$(PROF)cd print; INCROOT=$(INCROOT) make -e $(IGN) prof
	$(PROF)cd stdio; INCROOT=$(INCROOT) make -e $(IGN) prof
	$(PROF)cd sys; INCROOT=$(INCROOT) make -e $(IGN) prof
	$(PROF)cd x25; INCROOT=$(INCROOT) make -e $(IGN) prof

clean:

clobber: clean
	-rm -f */*.[op]
