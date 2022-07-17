#	@(#)makefile	2.1	
#
#	MC68000 AS MAKEFILE
#

SGS=
OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	755		#protections
ROOT=	

BINDIR=	$(ROOT)/bin
M68INC=	../../headers
COMINC=	/usr/include
COMAS=	../common

ARCH=	AR32WR
FLEX=	-DFLEXNAMES

CC=	cc
LINT=	lint
YACC=	yacc
LFLAGS=	-p
CFLAGS=	-O
INCLIST=	-I. -I$(M68INC) -I$(COMINC) -I$(COMAS)
DEFLIST=	-DINT32BIT
ODEFLIST=	-DONEPROC $(FLEX) -D$(ARCH)
LDFLAGS=
YFLAGS=		-d

CC_CMD=	$(CC) -c $(CFLAGS) $(INCLIST) $(DEFLIST) $(ODEFLIST)
YACC_CMD=	$(YACC) $(YFLAGS)


OFILES=	pass0.o instab.o parse.o gencode.o pass1.o symbols.o \
	code.o expand1.o expand2.o errors.o addr2.o pass2.o  \
	addr1.o codeout.o getstab.o obj.o symlist.o

CFILES=	$(COMAS)/pass0.c $(COMAS)/instab.c parse.c gencode.c \
	$(COMAS)/pass1.c $(COMAS)/symbols.c $(COMAS)/code.c  \
	$(COMAS)/expand1.c expand2.c $(COMAS)/errors.c	     \
	addr2.c $(COMAS)/pass2.c $(COMAS)/addr1.c	     \
	$(COMAS)/codeout.c $(COMAS)/getstab.c $(COMAS)/obj.c \
	$(COMAS/symlist.c

HFILES=	$(COMAS)/codeout.h gendefs.h instab.h ops.out	     \
	$(COMAS)/symbols.h $(M68INC)/paths.h		     \
	$(COMAS)/expand.h expand2.h parse.h		     \
	$(COMINC)/reloc.h $(COMINC)/syms.h		     \
	$(COMINC)/storclass.h $(COMINC)/filehdr.h	     \
	$(COMINC)/scnhdr.h $(COMINC)/linenum.h		     \
	$(M68INC)sgs.h

build:	$(SGS)as

#-------------------------
$(SGS)as:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(SGS)as $(OFILES)

pass0.o:	$(COMAS)/pass0.c pass0.h $(M68INC)/paths.h   \
		$(M68INC)/sgs.h systems.h
		$(CC_CMD) $(COMAS)/pass0.c

# The baroque code here tries to keep file times in sync to
# avoid unnecessary re-compilations.

parse.o:	parse.c $(COMAS)/symbols.h instab.h gendefs.h
		$(CC_CMD) parse.c

instab.o:	$(COMAS)/instab.c instab.h ops.out parse.h \
		$(COMAS)/symbols.h systems.h
		$(CC_CMD) $(COMAS)/instab.c

parse.c:	y.tab.c
		if cmp -s y.tab.c parse.c;then :;else cp y.tab.c parse.c;fi
parse.h:	y.tab.h
		if cmp -s y.tab.h parse.h;then :;else cp y.tab.h parse.h;fi

y.tab.c y.tab.h:	parse.y
		$(YACC_CMD) parse.y

# end baroque code

code.o:		$(COMAS)/code.c $(COMAS)/symbols.h	     \
		$(COMAS)/codeout.h gendefs.h systems.h
		$(CC_CMD) $(COMAS)/code.c

errors.o:	$(COMAS)/errors.c gendefs.h systems.h
		$(CC_CMD) $(COMAS)/errors.c

pass1.o:	$(COMAS)/pass1.c $(M68INC)/paths.h gendefs.h \
		$(COMAS)/symbols.h systems.h
		$(CC_CMD) $(COMAS)/pass1.c

gencode.o:	gencode.c $(COMAS)/symbols.h instab.h	     \
		gendefs.h $(COMAS)/expand.h expand2.h
		$(CC_CMD) gencode.c

expand1.o:	$(COMAS)/expand1.c $(COMAS)/expand.h	     \
		$(COMAS)/symbols.h gendefs.h systems.h
		$(CC_CMD) $(COMAS)/expand1.c

expand2.o:	expand2.c $(COMAS)/expand.h expand2.h	     \
		$(COMAS)/symbols.h
		$(CC_CMD) expand2.c

addr1.o:	$(COMAS)/addr1.c $(COMINC)/reloc.h gendefs.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h	     \
		$(COMINC)/linenum.h $(COMINC)/filehdr.h	     \
		$(COMAS)/symbols.h $(COMAS)/codeout.h	     \
		systems.h
		$(CC_CMD) $(COMAS)/addr1.c

addr2.o:	addr2.c $(COMINC)/reloc.h $(COMAS)/symbols.h \
		instab.h gendefs.h $(COMAS)/codeout.h
		$(CC_CMD) addr2.c

codeout.o:	$(COMAS)/codeout.c $(COMAS)/symbols.h	     \
		$(COMAS)/codeout.h gendefs.h systems.h
		$(CC_CMD) $(COMAS)/codeout.c

getstab.o:	$(COMAS)/getstab.c $(COMAS)/symbols.h	     \
		gendefs.h systems.h
		$(CC_CMD) $(COMAS)/getstab.c

pass2.o:	$(COMAS)/pass2.c gendefs.h systems.h	     \
		$(COMAS)/symbols.h
		$(CC_CMD) $(COMAS)/pass2.c

obj.o:		$(COMAS)/obj.c $(COMINC)/filehdr.h	     \
		$(COMINC)/linenum.h $(COMINC)/reloc.h	     \
		$(COMINC)/scnhdr.h $(COMINC)/syms.h	     \
		$(COMINC)/storclass.h $(COMAS)/symbols.h     \
		$(COMAS)/codeout.h gendefs.h systems.h	     \
		$(M68INC)/sgs.h
		$(CC_CMD) $(COMAS)/obj.c

symlist.o:	$(COMAS)/symlist.c $(COMAS)/symbols.h	     \
		$(COMINC)/syms.h $(COMINC)/storclass.h	     \
		gendefs.h systems.h
		$(CC_CMD) $(COMAS)/symlist.c

symbols.o:	$(COMAS)/symbols.c $(COMAS)/symbols.h	     \
		symbols2.h systems.h
		$(CC_CMD) $(COMAS)/symbols.c

#-------------------------

install:	$(SGS)as
		-rm -f $(BINDIR)/$(SGS)as
		cp $(SGS)as $(BINDIR)
		strip $(BINDIR)/$(SGS)as
		chmod $(PROT) $(BINDIR)/$(SGS)as
		chgrp $(GRP) $(BINDIR)/$(SGS)as
		chown $(OWN) $(BINDIR)/$(SGS)as

#--------------------------

save:	$(BINDIR)/$(SGS)as
	-rm -f $(BINDIR)/$(SGS)as.back
	cp $(BINDIR)/$(SGS)as $(BINDIR)/$(SGS)as.back

#--------------------------

uninstall:	$(BINDIR)/$(SGS)as.back
		-rm -f $(BINDIR)/$(SGS)as
		cp $(BINDIR)/$(SGS)as.back $(BINDIR)/$(SGS)as

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean
		-rm -f $(SGS)as parse.c parse.h y.tab.c y.tab.h

#--------------------------

lint:	$(COMINC)/a.out.h $(COMINC)/ar.h \
	$(CFILES)
	$(LINT) $(LFLAGS) -I$(M68INC) -I$(COMINC) $(CFILES) >lint.out
