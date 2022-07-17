#	@(#)makefile	2.1	
#	MC68000 SIZE MAKEFILE
#
#	@(#) makefile:	2.1 83/07/08
#
TESTDIR = ..
BASE=	../..


M68INC=	$(BASE)/headers
COMSRC=	../common

LIBLD = -lld

INCLIST=	-I$(COMSRC) -I$(M68INC)

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O
LDFLAGS= -s
DEFLIST=

ARFORMAT=	PORTAR

# Other definitions

ODEFLIST=	-D$(ARFORMAT) -DUNIX

CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(ODEFLIST) $(INCLIST)


OFILES=	main.o process.o fcns.o
CFILES=	$(COMSRC)/main.c $(COMSRC)/process.c $(COMSRC)/fcns.c


build:	$(TESTDIR)/size

#-------------------------

$(TESTDIR)/size:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/size $(OFILES) $(LIBLD)

main.o:		$(COMSRC)/main.c \
		$(M68INC)/sgs.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/main.c

process.o:	$(COMSRC)/process.c \
		$(COMSRC)/process.h $(COMSRC)/defs.h $(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/process.c

fcns.o:		$(COMSRC)/fcns.c \
	 	$(M68INC)/sgs.h $(COMSRC)/defs.h
		$(CC_CMD) $(COMSRC)/fcns.c

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:
		-rm -f $(TESTDIR)/size

#--------------------------

