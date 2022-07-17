#	@(#)makefile	2.1	
#	MC68000 LD MAKEFILE
#
#	@(#) makefile:	2.2 83/07/08
#
SGS=
OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	775		#protections
ROOT=	
BASE=	../..

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
COMINC=	$(ROOT)/usr/include
M68INC=	$(BASE)/headers
COMSRC=	$(BASE)/ld/common
SDPINC=	$(BASE)/ld

ARCH=	AR32WR
ARFORMAT=	PORTAR
FLEX=	-DFLEXNAMES
NPROC=ONEPROC
SDP=NOSDP

CC=	cc
LINT=	lint
LFLAGS=	-abhxp
CFLAGS=	-O
INCLIST=	-I. -I$(COMSRC) -I$(M68INC) -I$(COMINC) -I$(SDPINC)
DEFLIST=
ODEFLIST=	-D$(NPROC) -D$(SDP) -D$(ARCH) -D$(ARFORMAT) $(FLEX)
LDFLAGS=	-i
#
# NOTE: for a cross 'ld' LIBLD needs to refer to a version of the 
#       target library routines compiled for the host machine.
#
LIBLD=	$(ROOT)/usr/lib/libld.a
LIBS=	-ll -ly 
LEX=	lex
YACC=	yacc

CC_CMD=	$(CC) -c $(CFLAGS) $(INCLIST) $(DEFLIST) $(ODEFLIST)

CFILES=	$(COMSRC)/alloc.c $(COMSRC)/dump.c $(COMSRC)/expr0.c \
	$(COMSRC)/expr1.c $(COMSRC)/globs.c $(COMSRC)/instr.c\
	$(COMSRC)/ld00.c $(COMSRC)/ld01.c $(COMSRC)/ld1.c $(COMSRC)/ld2.c\
	$(COMSRC)/lists.c $(COMSRC)/output.c $(COMSRC)/sdp.c $(COMSRC)/syms.c\
	$(COMSRC)/slotvec.c $(COMSRC)/tv.c $(COMSRC)/util.c \
	$(COMSRC)/version.c \
	lex.yy.c maps.c y.tab.c sglobs.c \
	special1.c special2.c specsyms.c
#
#
# Specify the *.h files making up ld:
#
HFILES1=	y.tab.h params.h system.h\
		$(COMSRC)/attributes.h $(COMSRC)/list.h\
		$(COMSRC)/structs.h $(COMSRC)/extrns.h $(COMSRC)/ldtv.h\
		$(COMSRC)/slotvec.h $(COMSRC)/sdp1.h $(COMSRC)/instr.h
HFILES2=	$(COMINC)/aouthdr.h $(COMINC)/filehdr.h $(COMINC)/scnhdr.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h \
		$(COMINC)/ldfcn.h $(COMINC)/reloc.h\
		$(COMINC)/linenum.h 
HFILES3=	$(M68INC)/sgs.h $(M68INC)/paths.h $(M68INC)/tv.h \
		$(M68INC)/patch.h $(M68INC)/bool.h $(M68INC)/sgsmacros.h 

#
#
# Specify the *.o files making up ld:
#
OFILES1=	expr0.o globs.o sglobs.o ld00.o lex.yy.o lists.o\
	 	util.o version.o y.tab.o special1.o
OFILES2=	alloc.o specsyms.o dump.o expr1.o globs.o sglobs.o \
		ld01.o ld1.o ld2.o lists.o maps.o output.o sdp.o \
		slotvec.o syms.o tv.o util.o version.o instr.o special2.o

OFILES=	$(OFILES1) $(OFILES2)

build:	$(SGS)ld

#-------------------------
$(SGS)ld:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(SGS)ld $(OFILES) \
			$(LIBS) $(LIBLD)

y.tab.c y.tab.h:	$(COMSRC)/ld.yac
		@echo "Expect to see message 'conflicts: 6 shift/reduce'";
		$(YACC) -d $(COMSRC)/ld.yac

y.tab.o:	$(COMSRC)/attributes.h $(COMSRC)/list.h $(COMSRC)/structs.h\
		$(COMSRC)/ldmacros.h $(COMSRC)/extrns.h $(COMSRC)/ldtv.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/bool.h $(M68INC)/sgsmacros.h\
		$(M68INC)/sgs.h $(M68INC)/tv.h\
		params.h\
		y.tab.c
		$(CC_CMD) y.tab.c

lex.yy.c:	$(COMSRC)/ld.lex
		$(LEX) $(COMSRC)/ld.lex

lex.yy.o:	$(COMSRC)/structs.h $(COMSRC)/extrns.h $(COMSRC)/ldtv.h\
		$(COMSRC)/instr.h \
		$(COMINC)/aouthdr.h $(COMINC)/filehdr.h $(COMINC)/scnhdr.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h $(M68INC)/bool.h\
		$(M68INC)/paths.h $(M68INC)/sgs.h $(M68INC)/tv.h\
		system.h y.tab.h\
		lex.yy.c
		$(CC_CMD) lex.yy.c

expr0.o:	$(COMSRC)/ldmacros.h $(COMSRC)/extrns.h $(COMSRC)/structs.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		y.tab.h\
		$(COMSRC)/expr0.c
		$(CC_CMD) $(COMSRC)/expr0.c

expr1.o:	$(COMSRC)/ldmacros.h $(COMSRC)/extrns.h $(COMSRC)/structs.h \
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		system.h y.tab.h\
		$(COMSRC)/expr1.c
		$(CC_CMD) $(COMSRC)/expr1.c

ld00.o:		$(COMSRC)/extrns.h $(COMSRC)/list.h $(COMSRC)/structs.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h $(COMINC)/ldfcn.h \
		$(M68INC)/paths.h $(M68INC)/sgs.h \
		system.h params.h y.tab.h\
		$(COMSRC)/ld00.c
		$(CC_CMD) $(COMSRC)/ld00.c

ld01.o:		$(COMSRC)/extrns.h $(COMSRC)/structs.h $(COMSRC)/list.h\
		$(COMSRC)/ldmacros.h system.h $(COMSRC)/ldtv.h $(COMSRC)/instr.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		$(M68INC)/tv.h\
		$(COMSRC)/ld01.c
		$(CC_CMD) $(COMSRC)/ld01.c

ld1.o:		$(COMSRC)/extrns.h $(COMSRC)/list.h $(COMSRC)/ldtv.h\
		$(COMSRC)/ldmacros.h $(COMSRC)/structs.h \
		$(COMINC)/aouthdr.h $(COMINC)/filehdr.h $(COMINC)/scnhdr.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h $(COMINC)/ldfcn.h \
		$(COMINC)/reloc.h $(M68INC)/sgsmacros.h \
		$(SDPINC)/sdpsrc/hd/define2.h\
		$(M68INC)/sgs.h $(M68INC)/tv.h \
		system.h params.h\
		$(COMSRC)/ld1.c
		$(CC_CMD) $(COMSRC)/ld1.c

ld2.o:		$(COMSRC)/ldtv.h $(COMSRC)/extrns.h $(COMSRC)/list.h\
		$(COMSRC)/structs.h \
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(M68INC)/sgsmacros.h $(COMINC)/storclass.h\
		$(COMINC)/reloc.h $(COMINC)/linenum.h $(M68INC)/patch.h\
		$(M68INC)/tv.h\
		system.h y.tab.h params.h\
		$(COMSRC)/ld2.c
		$(CC_CMD) $(COMSRC)/ld2.c

alloc.o:	$(COMSRC)/extrns.h $(COMSRC)/list.h $(COMSRC)/structs.h\
		$(COMSRC)/ldmacros.h $(COMSRC)/ldtv.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		$(SDPINC)/sdpsrc/hd/define2.h\
		$(M68INC)/tv.h\
		system.h params.h\
		$(COMSRC)/alloc.c
		$(CC_CMD) $(COMSRC)/alloc.c

output.o:	$(COMSRC)/structs.h $(COMSRC)/extrns.h system.h\
		$(COMSRC)/ldtv.h $(COMSRC)/slotvec.h $(COMINC)/aouthdr.h\
		$(COMSRC)/ldmacros.h $(COMSRC)/instr.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(M68INC)/sgsmacros.h $(COMINC)/storclass.h\
		$(COMINC)/reloc.h $(COMINC)/linenum.h $(COMINC)/ldfcn.h\
		$(M68INC)/patch.h\
		$(M68INC)/sgs.h $(M68INC)/tv.h\
		$(COMSRC)/output.c
		$(CC_CMD) $(COMSRC)/output.c

maps.o:		$(COMSRC)/extrns.h $(COMSRC)/structs.h $(COMSRC)/attributes.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		system.h maps.c
		$(CC_CMD) maps.c

util.o:		$(COMSRC)/structs.h $(COMSRC)/extrns.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		$(M68INC)/sgs.h\
		$(COMSRC)/util.c
		$(CC_CMD) $(COMSRC)/util.c

lists.o: 	$(COMSRC)/list.h $(COMSRC)/structs.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(M68INC)/sgsmacros.h $(COMINC)/storclass.h\
		$(COMSRC)/lists.c
		$(CC_CMD) $(COMSRC)/lists.c

dump.o:		$(COMSRC)/structs.h $(COMSRC)/extrns.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h\
		$(COMSRC)/dump.c
		$(CC_CMD) $(COMSRC)/dump.c

syms.o:		$(COMSRC)/structs.h $(COMSRC)/extrns.h $(COMSRC)/ldtv.h\
		$(COMSRC)/ldmacros.h system.h $(COMSRC)/sdp1.h $(COMSRC)/instr.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(M68INC)/sgsmacros.h\
		$(SDPINC)/sdpsrc/hd/define2.h $(SDPINC)/sdpsrc/hd/define4.h\
		$(SDPINC)/sdpsrc/hd/struct.h \
		$(M68INC)/paths.h $(M68INC)/tv.h\
		$(COMSRC)/syms.c
		$(CC_CMD) $(COMSRC)/syms.c

sdp.o:		$(COMSRC)/sdp1.h system.h $(COMSRC)/structs.h\
		$(COMSRC)/instr.h $(COMSRC)/extrns.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h\
		$(SDPINC)/sdpsrc/hd/define2.h $(SDPINC)/sdpsrc/hd/define3.h\
		$(SDPINC)/sdpsrc/hd/define4.h $(SDPINC)/sdpsrc/hd/struct.h\
		$(SDPINC)/sdpsrc/hd/globals.h $(SDPINC)/sdpsrc/lo/delist.c\
		$(SDPINC)/sdpsrc/lo/enlist.c $(SDPINC)/sdpsrc/lo/fatal.c\
		$(SDPINC)/sdpsrc/lo/getnxtpage.c\
		$(SDPINC)/sdpsrc/lo/readpage.c\
		$(SDPINC)/sdpsrc/lo/writeframe.c\
		$(SDPINC)/sdpsrc/lo/sojourn.c\
		$(SDPINC)/sdpsrc/lo/rpartition.c\
		$(SDPINC)/sdpsrc/hi/allocate.c $(SDPINC)/sdpsrc/hi/flush.c\
		$(SDPINC)/sdpsrc/hi/init.c $(SDPINC)/sdpsrc/hi/lock.c\
		$(SDPINC)/sdpsrc/hi/unlock.c\
		$(M68INC)/paths.h\
		$(COMSRC)/sdp.c
		$(CC_CMD) $(COMSRC)/sdp.c

tv.o:		$(COMSRC)/extrns.h $(COMSRC)/structs.h $(COMSRC)/ldtv.h\
		$(COMSRC)/ldmacros.h system.h $(COMSRC)/list.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(SDPINC)/sdpsrc/hd/define2.h $(M68INC)/bool.h $(COMINC)/storclass.h\
		$(COMINC)/reloc.h $(COMINC)/ldfcn.h $(M68INC)/sgsmacros.h\
		$(M68INC)/tv.h\
		$(COMSRC)/tv.c
		$(CC_CMD) $(COMSRC)/tv.c

globs.o:	$(COMSRC)/structs.h $(COMSRC)/ldtv.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h\
		$(M68INC)/paths.h $(M68INC)/tv.h\
		$(COMSRC)/globs.c
		$(CC_CMD) $(COMSRC)/globs.c

instr.o:	$(COMSRC)/instr.c
		$(CC_CMD) $(COMSRC)/instr.c

slotvec.o:	$(COMSRC)/slotvec.h $(COMSRC)/structs.h $(COMSRC)/extrns.h\
		$(COMSRC)/sdp1.h system.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
		$(COMINC)/storclass.h $(SDPINC)/sdpsrc/hd/define2.h\
		$(SDPINC)/sdpsrc/hd/define4.h\
       		$(M68INC)/paths.h\
		$(COMSRC)/slotvec.c
		$(CC_CMD) $(COMSRC)/slotvec.c

version.o:	$(COMSRC)/version.c
		$(CC_CMD) $(COMSRC)/version.c

sglobs.o:	$(COMSRC)/structs.h $(COMSRC)/ldtv.h \
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h\
		$(M68INC)/tv.h \
		system.h sglobs.c
		$(CC_CMD) sglobs.c

special1.o:	system.h special1.c
		$(CC_CMD) special1.c

special2.o:	$(COMSRC)/structs.h $(COMSRC)/extrns.h $(COMSRC)/ldtv.h \
		$(COMSRC)/slotvec.h $(COMSRC)/list.h\
		$(COMINC)/aouthdr.h $(COMINC)/filehdr.h $(COMINC)/reloc.h \
		$(COMINC)/scnhdr.h $(COMINC)/syms.h $(COMINC)/storclass.h \
		$(COMINC)/ldfcn.h $(M68INC)/sgsmacros.h\
		$(M68INC)/tv.h $(M68INC)/sgs.h \
		system.h special2.c
		$(CC_CMD) special2.c

specsyms.o:	$(COMSRC)/extrns.h $(COMSRC)/structs.h $(COMSRC)/ldtv.h \
		$(COMINC)/filehdr.h $(M68INC)/sgsmacros.h \
		$(COMINC)/scnhdr.h $(COMINC)/syms.h $(COMINC)/storclass.h\
		$(M68INC)/tv.h \
		system.h \
		specsyms.c
		$(CC_CMD) specsyms.c

#-------------------------

install:	$(SGS)ld
		-rm -f $(BINDIR)/$(SGS)ld
		cp $(SGS)ld $(BINDIR)
		strip $(BINDIR)/$(SGS)ld
		chmod $(PROT) $(BINDIR)/$(SGS)ld
		chgrp $(GRP) $(BINDIR)/$(SGS)ld
		chown $(OWN) $(BINDIR)/$(SGS)ld

#--------------------------

save:	$(BINDIR)/$(SGS)ld
	-rm -f $(BINDIR)/$(SGS)ld.back
	cp $(BINDIR)/$(SGS)ld $(BINDIR)/$(SGS)ld.back

#--------------------------

uninstall:	$(BINDIR)/$(SGS)ld.back
		-rm -f $(BINDIR)/$(SGS)ld
		cp $(BINDIR)/$(SGS)ld.back $(BINDIR)/$(SGS)ld

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean
		-rm -f $(SGS)ld y.tab.c y.tab.h lex.yy.c

#--------------------------

lint:	$(CFILES) $(HFILES1) $(HFILES2) $(HFILES3) $(HFILES4) $(LIBLD)
	$(LINT) $(LFLAGS) $(INCLST)  $(CFILES) > lint.out
