#	nroff/troff make file (text subsystem)
#	SCCS:   @(#)roff.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
INSUB = /etc/install -f $(UBIN) 
SL = /usr/src/cmd
RDIR = ${SL}/text/roff.d
REL = current
MKSID = -r`gsid roff.mk ${REL}`
LIST = lp
MAKE = make

compile all:  nroff troff suftab term font
	:

nroff:
	${MAKE} nroff 

troff:
	${MAKE} PTAG= troff

suftab:
	${MAKE} suftab

term:
	cd terms.d; ${MAKE} -f terms.mk ${ARGS}
font:
	cd fonts.d; ${MAKE} -f fonts.mk ${ARGS}

listing:  listmk listnroff listterms listfonts
	:
listmk:     ;  pr roff.mk | ${LIST}
listnroff listtroff:  ;  ${MAKE} -i LIST="${LIST}" listing
listterms:  ;  cd terms.d; ${MAKE} -i -f terms.mk LIST="${LIST}" listing
listfonts:  ;  cd fonts.d; ${MAKE} -i -f fonts.mk LIST="${LIST}" listing

build:  bldmk bldnroff bldterms bldfonts
	:
bldmk:   ; get -p ${MKSID} s.roff.mk > ${RDIR}/roff.mk
bldnroff bldtroff:
	${MAKE} RDIR=${RDIR} REL=${REL} REWIRE='${REWIRE}' build
bldterms:
	cd terms.d; ${MAKE} -f terms.mk RDIR=${RDIR}/terms.d \
			REL=${REL} REWIRE='${REWIRE}' build
bldfonts:
	cd fonts.d; ${MAKE} -f fonts.mk RDIR=${RDIR}/fonts.d \
			REL=${REL} REWIRE='${REWIRE}' build

install: nroff troff inssuftab insterm insfont
	$(INSUB) $(TESTDIR)/nroff
	$(INSUB) $(TESTDIR)/troff

inssuftab: suftab
	cp suftab $(ULIB)
	chmod 644 $(ULIB)/suftab
	chgrp bin $(ULIB)/suftab
	chown bin $(ULIB)/suftab

insterm:
	cd terms.d; ${MAKE} -f terms.mk OL=$(OL) $(ARGS)

insfont:
	cd fonts.d; ${MAKE} -f fonts.mk OL=$(OL) $(ARGS)


edit:	mkedit
	${MAKE} edit
delta:	mkdelta
	${MAKE} delta

mkedit:	;  get -e s.roff.mk
mkdelta: ; delta s.roff.mk

ntedit:	;  ${MAKE} ntedit
ntdelta: ; ${MAKE} ntdelta

bedit:	;  ${MAKE} bedit
bdelta:	;  ${MAKE} bdelta

makeedit:  ;  ${MAKE} makeedit
makedelta: ;  ${MAKE} makedelta

clean:	nclean
	cd terms.d; ${MAKE} -f terms.mk clean
	cd fonts.d; ${MAKE} -f fonts.mk clean
nclean:	;  ${MAKE} clean

clobber: nclobber
	cd terms.d; ${MAKE} -f terms.mk clobber
	cd fonts.d; ${MAKE} -f fonts.mk clobber
nclobber: ;  ${MAKE} clobber
bclobber: ;  ${MAKE} bclobber

delete:	ndelete
	cd terms.d; ${MAKE} -f terms.mk delete
	cd fonts.d; ${MAKE} -f fonts.mk delete
ndelete: ;  ${MAKE} delete
bdelete: ;  ${MAKE} bdelete
