#	@(#)makefile	2.1	
#	MC68000 DUMP MAKEFILE
#
#	@(#) makefile:	2.2 83/07/08
#

#*****************************************************************************
#									     *
#		UNIX Mach Lang File examination (dump) makefile 	     *
#									     *
#*****************************************************************************
#									     *
#		Header/Data Files				   	     *
#									     *
#		common/process.h					     *
#		common/filedefs.h					     *
#		common/flagdefs.h					     *
#									     *
#*****************************************************************************
#									     *
#		Source Code Files					     *
#									     *
#		common/main.c						     *
#		common/setflags.c					     *
#		common/openobject.c					     *
#		common/process.c					     *
#		common/fcns.c						     *
#									     *
#*****************************************************************************
#									     *
#		Library Files						     *
#									     *
#		lib/libld.a						     *
#									     *
#*****************************************************************************

TESTDIR = ..
BASE=	../..

M68INC=	$(BASE)/headers
M68SRC= ../m68k
COMSRC=	../common

LIBLD = -lld

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS= -O
LDFLAGS= -s

ARCH=		AR32W
ARFORMAT=	PORTAR
FLEX=		-DFLEXNAMES

DEFLIST=

# other definitions

ODEFLIST=	-D$(ARCH) -D$(ARFORMAT) $(FLEX)

INCLIST=	-I$(COMSRC) -I$(M68INC)
CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(ODEFLIST) $(INCLIST)


CFILES=	$(COMSRC)/main.c $(COMSRC)/setflags.c $(COMSRC)/openobject.c \
	$(COMSRC)/process.c $(M68SRC)/fcns.c

HFILES=	$(COMSRC)/process.h $(COMSRC)/filedefs.h $(COMSRC)/flagdefs.h \
	$(M68INC)/sgs.h


OFILES=	main.o setflags.o openobject.o process.o fcns.o

#-------------------------

build:	$(TESTDIR)/dump

$(TESTDIR)/dump:	$(OFILES)
		$(CC) $(CFLAGS) -o $(TESTDIR)/dump $(OFILES) $(LIBLD)

main.o:		$(COMSRC)/main.c $(COMSRC)/filedefs.h \
		$(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/main.c

setflags.o:	$(COMSRC)/setflags.c $(COMSRC)/flagdefs.h $(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/setflags.c

openobject.o:	$(COMSRC)/openobject.c $(COMSRC)/filedefs.h \
	     	$(M68INC)/sgs.h 
		$(CC_CMD) $(COMSRC)/openobject.c

process.o:	$(COMSRC)/filedefs.h $(COMSRC)/process.h $(COMSRC)/flagdefs.h \
		$(COMSRC)/process.c \
		$(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/process.c

fcns.o:		$(M68SRC)/fcns.c \
	 	$(M68INC)/sgs.h
		$(CC_CMD) $(M68SRC)/fcns.c
#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:
		-rm -f $(TESTDIR)/dump

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) $(INCLIST) $(CFILES) >lint.out
