#	@(#)makefile	2.1	
#	MC68000 OPTIM MAKEFILE
#
#	@(#) makefile:	2.1 83/07/08
#
OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	775		#protections
ROOT=	

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
COMDIR= ../common

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O
LDFLAGS=
DEFLIST=
INCLIST=	-I. -I$(COMDIR)

CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(INCLIST)


OFILES=		scan.o parse.o optab.o rand.o mipsup.o vtrace.o peep.o \
		optim.o inter.o
CFILES=		scan.c parse.c optab.c rand.c mipsup.c vtrace.c peep.c \
		$(COMDIR)/optim.c $(COMDIR)/inter.c

OPTIM_H=	$(COMDIR)/optim.h defs rand.h ops.h

build:	optim

#-------------------------
optim:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o optim $(OFILES)

optim.o:	$(OPTIM_H) $(COMDIR)/optim.c
		$(CC_CMD) $(COMDIR)/optim.c

inter.o:	$(OPTIM_H) $(COMDIR)/inter.c
		$(CC_CMD) $(COMDIR)/inter.c

scan.o:		$(OPTIM_H) scan.c
		$(CC_CMD) scan.c

parse.o:	$(OPTIM_H) parse.c
		$(CC_CMD) parse.c

rand.o:		$(OPTIM_H) rand.c
		$(CC_CMD) rand.c

mipsup.o:	$(OPTIM_H) mipsup.c
		$(CC_CMD) mipsup.c

optab.o:	optab.c ops.h
		$(CC_CMD) optab.c

optab.c:	optab.head optab.base optab.awk
		awk -f optab.awk optab.base | cat optab.head - >optab.c

vtrace.o:	vtrace.c $(OPTIM_H)
		$(CC_CMD) vtrace.c

peep.o:		peep.c $(OPTIM_H)
		$(CC_CMD) peep.c

#-------------------------

install:	optim
		-rm -f $(LIBDIR)/optim
		cp optim $(LIBDIR)
		strip $(LIBDIR)/optim
		chmod $(PROT) $(LIBDIR)/optim
		chgrp $(GRP) $(LIBDIR)/optim
		chown $(OWN) $(LIBDIR)/optim

#--------------------------

save:	$(LIBDIR)/optim
	-rm -f $(LIBDIR)/optim.back
	cp $(LIBDIR)/optim $(LIBDIR)/optim.back

#--------------------------

uninstall:	$(LIBDIR)/optim.back
		-rm -f $(LIBDIR)/optim
		cp $(LIBDIR)/optim.back $(LIBDIR)/optim

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean
		-rm -f optab.c optim

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) $(INCLIST) $(CFILES) >lint.out
