#	@(#)makefile	2.1	
#	MC68000 NM MAKEFILE
#
#	@(#) makefile:	2.2 83/07/08
#
TESTDIR= ..
OWN=	sgs		#file owner
GRP=	sgs		#file group
PROT=	775		#protections
OL =
ROOT=	$(OL)
BASE=	../..

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
M68INC=$(BASE)/headers
COMSRC=	../common

ARFORMAT=	PORTAR
FLEX=	-DFLEXNAMES

CC=	cc
LINT=	lint
LFLAGS=	-sp
CFLAGS=	-O 
LDFLAGS=
DEFLIST= -DINT32BIT
ARCH= AR32W
USRINC= $(BASE)/inc/common
# other definitions
ODEFLIST=	-D$(ARFORMAT) $(FLEX) -DUNIX
INCLIST=	-I$(M68INC) -I$(COMSRC) 

LIBLD=  -lld
CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(ODEFLIST) $(INCLIST)


OFILES=	main.o setflags.o process.o prosym.o decotype.o fcns.o
CFILES=	$(COMSRC)/main.c $(COMSRC)/setflags.c $(COMSRC)/process.c \
	$(COMSRC)/prosym.c $(COMSRC)/decotype.c $(COMSRC)/fcns.c


build:	$(TESTDIR)/nm

#-------------------------

$(TESTDIR)/nm:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/nm $(OFILES) $(LIBLD)

main.o:		$(COMSRC)/main.c \
		$(M68INC)/sgs.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/main.c

setflags.o:	$(COMSRC)/setflags.c $(M68INC)/sgs.h \
		$(M68INC)/paths.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/setflags.c

process.o:	$(COMSRC)/process.c \
		$(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/process.c

prosym.o:	$(COMSRC)/prosym.c \
		$(COMSRC)/defs.h \
		$(COMSRC)/prosym.h
		$(CC_CMD) $(COMSRC)/prosym.c

decotype.o:	$(COMSRC)/decotype.c \
	        $(COMSRC)/decotype.h \
		$(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/decotype.c

fcns.o:		$(COMSRC)/fcns.c \
		$(M68INC)/sgs.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/fcns.c

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	-rm -f $(TESTDIR)/nm

#-------------------------

install:	$(TESTDIR)/nm
		-rm -f $(BINDIR)/$(TESTDIR)/nm
		cp $(TESTDIR)/nm $(BINDIR)
		strip $(BINDIR)/$(TESTDIR)/nm
		chmod $(PROT) $(BINDIR)/$(TESTDIR)/nm
		chgrp $(GRP) $(BINDIR)/$(TESTDIR)/nm
		chown $(OWN) $(BINDIR)/$(TESTDIR)/nm

#--------------------------

save:	$(BINDIR)/$(TESTDIR)/nm
	-rm -f $(BINDIR)/$(TESTDIR)/nm.back
	cp $(BINDIR)/$(TESTDIR)/nm $(BINDIR)/$(TESTDIR)/nm.back

#--------------------------

uninstall:	$(BINDIR)/$(TESTDIR)/nm.back
		-rm -f $(BINDIR)/$(TESTDIR)/nm
		cp $(BINDIR)/$(TESTDIR)/nm.back $(BINDIR)/$(TESTDIR)/nm

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) $(INCLIST) $(CFILES) >lint.out
