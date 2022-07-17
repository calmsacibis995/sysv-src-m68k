#	@(#)makefile	2.1	
#	MC68000 CONVERT MAKEFILE
#
#	@(#) makefile:	2.2 83/07/08
#
SGS=
OWN=	sgs		#file owner
GRP=	sgs		#file group
PROT=	775		#protections
ROOT=	../../../..
BASE=	../..

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
M68INC=	$(BASE)/inc/mc68
COMINC=	$(BASE)/inc/common
COMSRC=	$(BASE)/convert/common

LIBLD = $(BASE)/libld/common/libld.a

INCLIST=	-I$(COMINC) -I$(COMSRC)

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O
LDFLAGS=
DEFLIST=

ARCH=		AR32W
ARFORMAT=	PORTAR
FLEX=		-DFLEXNAMES

# Other definitions

ODEFLIST=	-D$(ARFORMAT) -D$(ARCH) $(FLEX)

CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(ODEFLIST) $(INCLIST)


OFILES=	main.o getargs.o convert.o fcns.o openobject.o util.o appl.o
CFILES=	$(COMSRC)/main.c $(COMSRC)/getargs.c $(COMSRC)/convert.c \
	$(COMSRC)/fcns.c $(COMSRC)/openobject.c $(COMSRC)/util.c \
	$(COMSRC)/appl.c


build:	$(SGS)conv

#-------------------------

$(SGS)conv:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(SGS)conv $(OFILES) $(LIBLD)

main.o:		$(COMSRC)/main.c $(COMSRC)/conv.h 
		$(CC_CMD) $(COMSRC)/main.c

getargs.o:	$(COMSRC)/getargs.c \
		$(COMSRC)/conv.h $(COMINC)/sgsmacros.h $(COMINC)/filehdr.h
		$(CC_CMD) $(COMSRC)/getargs.c

convert.o:	$(COMSRC)/convert.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMSRC)/conv.h
		$(CC_CMD) $(COMSRC)/convert.c

fcns.o:		$(COMSRC)/fcns.c \
		$(COMSRC)/conv.h $(COMINC)/filehdr.h $(COMINC)/ldfcn.h\
		$(COMINC)/linenum.h $(COMINC)/patch.h $(COMINC)/reloc.h\
		$(COMINC)/scnhdr.h $(COMINC)/storclass.h $(COMINC)/syms.h
		$(CC_CMD) $(COMSRC)/fcns.c

openobject.o:	$(COMSRC)/openobject.c \
		$(COMSRC)/conv.h $(COMINC)/filehdr.h $(COMINC)/ldfcn.h
		$(CC_CMD) $(COMSRC)/openobject.c

util.o:		$(COMSRC)/util.c \
		$(COMSRC)/conv.h $(COMINC)/filehdr.h $(COMINC)/ldfcn.h
		$(CC_CMD) $(COMSRC)/util.c

appl.o:		$(COMSRC)/appl.c \
		$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMSRC)/conv.h
		$(CC_CMD) $(COMSRC)/appl.c

#-------------------------

install:	$(SGS)conv
		-rm -f $(BINDIR)/$(SGS)conv
		cp $(SGS)conv $(BINDIR)
		strip $(BINDIR)/$(SGS)conv
		chmod $(PROT) $(BINDIR)/$(SGS)conv
		chgrp $(GRP) $(BINDIR)/$(SGS)conv
		chown $(OWN) $(BINDIR)/$(SGS)conv

#--------------------------

save:	$(BINDIR)/$(SGS)conv
	-rm -f $(BINDIR)/$(SGS)conv.back
	cp $(BINDIR)/$(SGS)conv $(BINDIR)/$(SGS)conv.back

#--------------------------

uninstall:	$(BINDIR)/$(SGS)conv.back
		-rm -f $(BINDIR)/$(SGS)conv
		cp $(BINDIR)/$(SGS)conv.back $(BINDIR)/$(SGS)conv

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean
		-rm -f $(SGS)conv

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) $(INCLIST) $(CFILES) >lint.out
