#	eqn make file (text subsystem)
#	SCCS:  @(#)eqn.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
UPUB = $(OL)/usr/pub
INSUB = /etc/install -f ${UBIN}
CFLAGS = -O
LDFLAGS = -s # -n

SL = /usr/src/cmd
RDIR = ${SL}/text/eqn.d
REL = current
CSID = -r`gsid eqn ${REL}`
MKSID = -r`gsid eqn.mk ${REL}`
LIST = lp
B20 =
YFLAGS = -d
SOURCE = e.y e.h diacrit.c eqnbox.c font.c fromto.c funny.c glob.c integral.c \
	 io.c lex.c lookup.c mark.c matrix.c move.c over.c paren.c \
	 pile.c shift.c size.c sqrt.c text.c
OFILES =  diacrit.o eqnbox.o font.o fromto.o funny.o glob.o integral.o \
	 io.o lex.o lookup.o mark.o matrix.o move.o over.o paren.o \
	 pile.o shift.o size.o sqrt.o text.o
FILES =  ${OFILES} e.o
MAKE = make

compile all: eqn eqnch
	:

eqn:	$(FILES)
	$(CC) ${LDFLAGS} -o $(TESTDIR)/eqn $(FILES) -ly

$(OFILES):: e.h e.def
	:

e.def:	  y.tab.h
	  -cmp -s y.tab.h e.def || cp y.tab.h e.def

y.tab.h:  e.o
	:

eqnch:	eqnchar

install: all
	${INSUB} $(TESTDIR)/eqn
	cp eqnchar ${UPUB}
	chmod 664 ${UPUB}/eqnchar
	cd ${UPUB}; chgrp bin eqnchar; chown bin eqnchar

clean:
	  rm -f *.o y.tab.h e.def

clobber:
	  rm -f $(TESTDIR)/eqn

build:	bldmk bldeqnch
	get -p ${CSID} s.eqn.src ${REWIRE} | ntar -d ${RDIR} -g
bldeqnch: ; get -p -r`gsid eqnchar ${REL}` s.eqnchar > ${RDIR}/eqnchar
bldmk:  ;  get -p ${MKSID} s.eqn.mk > ${RDIR}/eqn.mk

listing:
	pr eqn.mk eqnchar ${SOURCE} | ${LIST}
listeqnch: ; pr eqnchar | ${LIST}
listmk: ;  pr eqn.mk | ${LIST}

edit:
	get -e -p s.eqn.src | ntar -g

delta:
	ntar -p ${SOURCE} > eqn.src
	delta s.eqn.src
	rm -f ${SOURCE}

eqnchedit: ; get -e s.eqnchar
eqnchdelta: ; delta s.eqnchar

mkedit:  ;  get -e s.eqn.mk
mkdelta: ;  delta s.eqn.mk


delete:	clean clobber
	rm -f ${SOURCE} eqnchar
