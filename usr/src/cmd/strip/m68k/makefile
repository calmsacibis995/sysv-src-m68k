#	@(#)makefile	2.1.1.1	
#	MC68000 STRIP MAKEFILE
#
#	@(#) makefile:	2.3 83/07/08
#
OL=
TESTDIR= ..
OWN=	sgs		#file owner
GRP=	sgs		#file group
PROT=	775		#protections
ROOT=	$(OL)
BASE=	../..

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
M68INC=	$(BASE)/headers
COMINC=	../../../../include
COMSRC=	../common

LIBLD = -lld

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O -I$(INC)/common -I$(INC)/m68k
LDFLAGS=
# don't use the strip we just built
STRIP=	/bin/strip

ARCH=		AR32W
ARFORMAT=	PORTAR
USRINC=  $(BASE)/inc/common
FLEX=		-DFLEXNAMES

DEFLIST=        -DINT32BIT

# other definitions

ODEFLIST=	-D$(ARCH) -D$(ARFORMAT) $(FLEX)

INCLIST=	-I$(COMINC) -I$(COMSRC) -I$(M68INC)
CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(ODEFLIST) $(INCLIST)


OFILES=	main.o setflags.o process.o checkflags.o buildtmp.o buildfcns.o \
	 copytmp.o fcns.o
CFILES=	$(COMSRC)/main.c $(COMSRC)/setflags.c $(COMSRC)/process.c \
	$(COMSRC)/checkflags.c $(COMSRC)/buildtmp.c $(COMSRC)/buildfcns.c \
	$(COMSRC)/copytmp.c $(COMSRC)/fcns.c


build:	$(TESTDIR)/strip

#-------------------------

$(TESTDIR)/strip:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/strip $(OFILES) $(LIBLD)

main.o:		$(COMSRC)/main.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
		$(M68INC)/paths.h $(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/main.c

setflags.o:	$(COMSRC)/setflags.c $(M68INC)/sgs.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/setflags.c

process.o:	$(COMSRC)/process.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMSRC)/defs.h \
		$(M68INC)/paths.h $(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/process.c

checkflags.o:	$(COMSRC)/checkflags.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h
		$(CC_CMD) $(COMSRC)/checkflags.c

buildtmp.o:	$(COMSRC)/buildtmp.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/buildtmp.c

buildfcns.o:	$(COMSRC)/buildfcns.c \
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/reloc.h \
		$(COMINC)/syms.h $(COMINC)/ldfcn.h $(COMINC)/storclass.h \
		$(COMSRC)/defs.h $(M68INC)/sgs.h $(COMINC)/linenum.h
		$(CC_CMD) $(COMSRC)/buildfcns.c

copytmp.o:	$(COMSRC)/copytmp.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/copytmp.c

fcns.o:		$(COMSRC)/fcns.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
		$(M68INC)/sgs.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/fcns.c

#-------------------------

install:	$(TESTDIR)/strip
		-rm -f $(BINDIR)/$(TESTDIR)/strip
		cp $(TESTDIR)/strip $(BINDIR)
		$(STRIP) $(BINDIR)/$(TESTDIR)/strip
		chmod $(PROT) $(BINDIR)/$(TESTDIR)/strip
		chgrp $(GRP) $(BINDIR)/$(TESTDIR)/strip
		chown $(OWN) $(BINDIR)/$(TESTDIR)/strip

#--------------------------

save:	$(BINDIR)/$(TESTDIR)/strip
	-rm -f $(BINDIR)/$(TESTDIR)/strip.back
	cp $(BINDIR)/$(TESTDIR)/strip $(BINDIR)/$(TESTDIR)/strip.back

#--------------------------

uninstall:	$(BINDIR)/$(TESTDIR)/strip.back
		-rm -f $(BINDIR)/$(TESTDIR)/strip
		cp $(BINDIR)/$(TESTDIR)/strip.back $(BINDIR)/$(TESTDIR)/strip

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean
		-rm -f $(TESTDIR)/strip

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) $(INCLIST) $(CFILES) >lint.out
