#	@(#)makefile	2.1.1.1	
#	MC68000 AR MAKEFILE
#
#
OL=
TESTDIR=..
OWN=bin		#file owner
GRP=bin		#file group
PROT=775		#protections
ROOT=$(OL)
BASE=../..

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
USRINC=../../../../include

CC=	cc
LINT=	lint
CFLAGS=	-O
LFLAGS=	-p
LIBLD= -lld
LDFLAGS=
DEFLIST=
INCLIST=	-I$(USRINC)

FLEX=	-DFLEXNAMES

CC_CMD=	$(CC) $(CFLAGS) $(DEFLIST) $(FLEX) $(INCLIST) -c

OFILES=	ar.o
CFILES=	../common/ar.c


build:	$(TESTDIR)/ar

#-------------------------
$(TESTDIR)/ar:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/ar $(OFILES) $(LIBLD)

ar.o:		../common/ar.c \
		$(USRINC)/ar.h $(USRINC)/filehdr.h $(USRINC)/syms.h
		$(CC_CMD) ../common/ar.c


#-------------------------

install:	$(TESTDIR)/ar
		-rm -f $(BINDIR)/$(TESTDIR)/ar
		cp $(TESTDIR)/ar $(BINDIR)
		strip $(BINDIR)/$(TESTDIR)/ar
		chmod $(PROT) $(BINDIR)/$(TESTDIR)/ar
		chgrp $(GRP) $(BINDIR)/$(TESTDIR)/ar
		chown $(OWN) $(BINDIR)/$(TESTDIR)/ar

#--------------------------

save:	$(BINDIR)/$(TESTDIR)/ar
	-rm -f $(BINDIR)/$(TESTDIR)/ar.back
	cp $(BINDIR)/$(TESTDIR)/ar $(BINDIR)/$(TESTDIR)/ar.back

#--------------------------

uninstall:	$(BINDIR)/$(TESTDIR)/ar.back
		-rm -f $(BINDIR)/$(TESTDIR)/ar
		cp $(BINDIR)/$(TESTDIR)/ar.back $(BINDIR)/$(TESTDIR)/ar

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	-rm -f $(TESTDIR)/ar

#--------------------------

lint:	$(USRINC)/ar.h \
	$(CFILES)
	$(LINT) $(LFLAGS) -I$(USRINC) $(CFILES) >lint.out
