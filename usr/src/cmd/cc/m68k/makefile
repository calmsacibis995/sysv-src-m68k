#	@(#)makefile	2.1.1.1	
#	MC68000 COMMAND MAKEFILE
#
#	@(#) makefile:	2.1 83/07/08
#
OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	775		#protections
ROOT=	

SGSI= ../../headers
CROSSSGSI= /port/port/src/work/inc/mc68
COMI= ../../../head
BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O
LDFLAGS=
DEFLIST=
CROSSDEFLIST= -DUSR_INC=\"-I/port/port/usr/include\" -DPORTAR
INCLIST= -I$(SGSI) -I$(COMI)
CROSSINCLIST= -I$(CROSSSGSI) -I$(COMI)


CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(INCLIST)
CROSSCC_CMD= $(CC) -c $(CFLAGS) $(CROSSDEFLIST) $(CROSSINCLIST)


CROSSOFILES= crosscc.o		# just temporary out of crosscc.c
CROSSCFILES= crosscc.c		# to avoid mistakes!

OFILES=	cc.o
CFILES=	cc.c


build:	cc

buildcross:	crosscc

#-------------------------
cc:	$(OFILES)
		-rm -f cc
		$(CC) $(CFLAGS) $(LDFLAGS) -o cc $(OFILES)

cc.o:		cc.c $(SGSI)/paths.h
		-rm -f cc
		$(CC_CMD) cc.c

crosscc:	$(CROSSOFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o crosscc $(CROSSOFILES)

crosscc.o:	cc.c $(CROSSSGSI)/paths.h
		cp cc.c crosscc.c
		$(CROSSCC_CMD) crosscc.c
		-rm crosscc.c


#-------------------------

install:	cc
		-rm -f $(BINDIR)/cc
		cp cc $(BINDIR)/cc
		strip $(BINDIR)/cc
		chmod $(PROT) $(BINDIR)/cc
		chgrp $(GRP) $(BINDIR)/cc
		chown $(OWN) $(BINDIR)/cc

#--------------------------

save:	$(BINDIR)/cc
	-rm -f $(BINDIR)/cc.back
	cp $(BINDIR)/cc $(BINDIR)/cc.back

#--------------------------

uninstall:	$(BINDIR)/cc.back
		-rm -f $(BINDIR)/cc
		cp $(BINDIR)/cc.back $(BINDIR)/cc

#--------------------------

clean:
	-rm -f $(OFILES) $(CROSSOFILES)

#--------------------------

clobber:	clean
		-rm -f cc

#--------------------------

lint:	$(SGSI)/paths.h
	$(CFILES)
	$(LINT) $(LFLAGS) $(CFILES) >lint.out
