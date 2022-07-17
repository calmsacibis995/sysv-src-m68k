#	@(#)makefile	2.1	
#	MC68000 DISASSEMBLER MAKEFILE
#
#	@(#) makefile:	2.1 83/07/08
#
SGS=
OWN=	sgs		#file owner
GRP=	sgs		#file group
PROT=	775		#protections
ROOT=	$(OL)
BASE=	../..

OL = 
BIN = $(OL)/bin
TESTDIR= .
INCM68K= ../m68k
M68INC=	$(BASE)/headers
COMSRC=	../common

LIBLD = -lld

INCLIST=	-I$(COMSRC) -I$(M68INC) -I$(INCM68K)

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O
LDFLAGS= -s
DEFLIST=

ARCH=	AR32W
FLEX=	-DFLEXNAMES
ARFORMAT=	PORTAR

# Other definitions

ODEFLIST=	-D$(ARFORMAT) -DUNIX=4 -DMC68 -D$(ARCH) $(FLEX)

CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(ODEFLIST) $(INCLIST)


OFILES=	dis_extn.o dis_main.o dis_utls.o text_dis.o lists.o
CFILES=	$(COMSRC)/dis_extn.c $(COMSRC)/dis_main.c $(COMSRC)/dis_utls.c \
	  $(COMSRC)/lists.c text_dis.c


#-------------------------

$(TESTDIR)/dis:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/dis $(OFILES) $(LIBLD)

dis_extn.o:	dis.h \
		$(COMSRC)/structs.h \
		$(COMSRC)/dis_extn.c 
		$(CC_CMD) $(COMSRC)/dis_extn.c

lists.o:	dis.h \
		$(COMSRC)/lists.c \
		$(COMSRC)/structs.h \
		$(M68INC)/paths.h \
		$(M68INC)/sgs.h 
		$(CC_CMD) $(COMSRC)/lists.c

dis_main.o:	dis.h \
		$(COMSRC)/structs.h \
		$(COMSRC)/dis_main.c \
		$(M68INC)/paths.h \
		$(M68INC)/sgs.h 
		$(CC_CMD) $(COMSRC)/dis_main.c

dis_utls.o:	dis.h \
		$(COMSRC)/structs.h \
		$(COMSRC)/dis_utls.c \
		$(M68INC)/sgs.h
		$(CC_CMD) $(COMSRC)/dis_utls.c

text_dis.o:	dis.h \
		text_dis.c \
		$(COMSRC)/structs.h \
		$(M68INC)/sgs.h 
		$(CC_CMD) text_dis.c

#-------------------------

install:	all
		strip $(TESTDIR)/dis
		cp $(TESTDIR)/dis $(BIN)
		chmod $(PROT) $(BIN)/dis
		chgrp $(GRP) $(BIN)/dis
		chown $(OWN) $(BIN)/dis

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	
		-rm -f $(TESTDIR)/dis

