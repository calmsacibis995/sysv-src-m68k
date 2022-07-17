#	text subsystem shells make file
#	SCCS:  @(#)shells.mk	2.1	

OL =
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
HELP = $(ULIB)/help
INSUB = /etc/install -f $(UBIN)

SL = /usr/src/cmd
RDIR = ${SL}/text/shells.d
REL = current
MKSID = -r`gsid shells.mk ${REL}`
MMSID = -r`gsid mm.sh ${REL}`
MMTSID = -r`gsid mmt.sh ${REL}`
MANSID = -r`gsid man.sh ${REL}`
MVTSID = -r`gsid mvt.sh ${REL}`
ORGSID = -r`gsid org.sh ${REL}`
OSDDSID = -r`gsid osdd.sh ${REL}`
GSID = -r`gsid greek.sh ${REL}`
TERMHSID = -r`gsid termhelp ${REL}`
TEXTHSID = -r`gsid texthelp ${REL}`
LIST = lp
FILES = mm.sh mmt.sh man.sh mvt.sh org.sh osdd.sh greek.sh term text
MAKE = make

compile all:  man mm mmt mvt osdd greek
	:

man:
	cp man.sh man

mm:
	cp mm.sh mm

mmt:
	-cp mmt.sh mmt

mvt:	mmt

#org:
#	cp org.sh org

osdd:
	cp osdd.sh osdd

greek:
	cp greek.sh greek

helpdir:
	-if [ ! -d $(ULIB)/help ]; then mkdir $(ULIB)/help; fi

termh:	helpdir

texth:	helpdir

install:	mm man mmt mvt osdd greek instermh instexth
	$(INSUB) mm
	$(INSUB) man
	$(INSUB) mmt
	# $(INSUB) org
	$(INSUB) osdd
	$(INSUB) greek
	-rm -f $(UBIN)/mvt
	ln $(UBIN)/mmt $(UBIN)/mvt

insmm:	mm
	$(INSUB) mm
insman:	man
	$(INSUB) man
insmvt:	mvt
	-rm -f $(UBIN)/mvt
	ln $(UBIN)/mmt $(UBIN)/mvt
insmmt: mmt
	$(INSUB) mmt
insorg:	org
	$(INSUB) org
insosdd:	osdd
	$(INSUB) osdd
insgreek:	greek
	$(INSUB) greek
instermh:	termh
	cp term $(HELP)
	chmod 664 $(HELP)/term
	cd $(HELP); chgrp bin term; chown bin term
instexth:	texth
	cp text $(HELP)
	chmod 664 $(HELP)/text
	cd $(HELP); chgrp bin text; chown bin text

clean:
	:
clobber:  manclobber mmclobber mmtclobber orgclobber \
		osddclobber greekclobber
	:

manclobber:  ;  rm -f man
mmclobber:   ;  rm -f mm
mmtclobber mvtclobber:  ;  rm -f mmt
orgclobber:  ;  rm -f org
osddclobber: ;  rm -f osdd
greekclobber: ; rm -f greek

listing:  ;  pr shells.mk ${FILES}  |  ${LIST}
listmk:    ;  pr shells.mk | ${LIST}
listmm:    ;  pr mm.sh | ${LIST}
listmmt:   ;  pr mmt.sh | ${LIST}
listman:   ;  pr man.sh | ${LIST}
listmvt:   ;  pr -h "linked copy of mmt - mvt" mvt.sh | ${LIST}
listorg:   ;  pr org.sh | ${LIST}
listosdd:  ;  pr osdd.sh | ${LIST}
listgreek: ;  pr greek.sh | ${LIST}
listtermh: ; pr term | ${LIST}
listtexth: ; pr text | ${LIST}

build:  bldmk bldmm bldmmt bldman bldosdd bldgreek bldtermh bldtexth
	:
bldmk:  ;  get -p ${MKSID} s.shells.mk > ${RDIR}/shells.mk
bldmm:	;  get -p ${MMSID} s.mm.sh ${REWIRE} > ${RDIR}/mm.sh
bldmmt bldmvt:	;  get -p ${MMTSID} s.mmt.sh ${REWIRE} > ${RDIR}/mmt.sh
bldman:	;  get -p ${MANSID} s.man.sh ${REWIRE} > ${RDIR}/man.sh
bldorg:	;  get -p ${ORGSID} s.org.sh ${REWIRE} > ${RDIR}/org.sh
bldosdd: ; get -p ${OSDDSID} s.osdd.sh ${REWIRE} > ${RDIR}/osdd.sh
bldgreek: ; get -p ${GSID} s.greek.sh ${REWIRE} > ${RDIR}/greek.sh
bldtermh: ; get -p ${TERMHSID} s.term > ${RDIR}/term
bldtexth: ; get -p ${TEXTHSID} s.text > ${RDIR}/text

edit:	manedit mmedit mmtedit orgedit osddedit greekedit \
		termhedit texthedit mkedit
	:
manedit:  ;  get -e s.man.sh
mmedit:   ;  get -e s.mm.sh
mmtedit mvtedit:  ;  get -e s.mmt.sh
orgedit:  ;  get -e s.org.sh
osddedit: ;  get -e s.osdd.sh
greekedit: ; get -e s.greek.sh
termhedit: ; get -e s.term
texthedit: ; get -e s.text
mkedit:   ;  get -e s.shells.mk

delta:	mandelta mmdelta mmtdelta orgdelta osdddelta greekdelta \
		termhdelta texthdelta  mkdelta
	:
mandelta:  ;  delta s.man.sh
mmdelta:   ;  delta s.mm.sh
mmtdelta mvtdelta:  ;  delta s.mmt.sh
orgdelta:  ;  delta s.org.sh
osdddelta: ;  delta s.osdd.sh
greekdelta: ; delta s.greek.sh
termhdelta: ; delta s.term
texthdelta: ; delta s.text
mkdelta:   ;  delta s.shells.mk

delete:  clean clobber mandelete mmdelete mmtdelete orgdelete osdddelete \
		greekdelete termhdelete texthdelete
	:
mandelete:	manclobber
	rm -f man.sh
mmdelete:	mmclobber
	rm -f mm.sh
mmtdelete mvtdelete:	mmtclobber mvtclobber
	rm -f mmt.sh
orgdelete:	orgclobber
	rm -f org.sh
osdddelete:	osddclobber
	rm -f osdd.sh
greekdelete:	greekclobber
	rm -f greek.sh
termhdelete: ; rm -f term
texthdelete: ; rm -f text
