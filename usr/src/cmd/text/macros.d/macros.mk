#	text Development Support Library (DSL) macros make file
#	SCCS: @(#)macros.mk	2.1	

ULIB = $(OL)/usr/lib
UBIN = $(OL)/usr/bin
INSMAC = $(ULIB)/macros
INSTMAC = $(ULIB)/tmac

SL = /usr/src/cmd
RDIR = ${SL}/text/macros.d
REL = current
NTARSID = -r`gsid ntar ${REL}`
MMNSID = -r`gsid mmn ${REL}`
MMTSID = -r`gsid mmt ${REL}`
VMSID = -r`gsid vmca ${REL}`
ORGSID = -r`gsid org ${REL}`
OSDDSID = -r`gsid osdd ${REL}`
MANSID = -r`gsid manmacs ${REL}`
TMSID = -r`gsid tmacs ${REL}`
MACRUNCHSID = -r`gsid macrunch ${REL}`
MKSID = -r`gsid macros.mk ${REL}`
LIST = lp
TMACFILES = tmac.an tmac.m tmac.ptx tmac.osd tmac.v  #tmac.org
MAKE = make
LDFLAGS = -s # -n

compile all:	ntar mmn mmt vmca osdd man ptx
	:
ntar:	ntar.c
	${CC} ${LDFLAGS} -O  -o ntar ntar.c
mmn:	ntar
	sh ./macrunch -n m ${MINS} -d $(UBIN) $(ULIB) mmn

mmt:	ntar
	sh ./macrunch -t m ${MINS} -d $(UBIN) $(ULIB) mmt

vmca:	ntar
	sh ./macrunch ${MINS} -d $(UBIN) $(ULIB) vmca

#org:	ntar
#	sh ./macrunch ${MINS} -d $(UBIN) $(ULIB) org

osdd:	ntar
	sh ./macrunch ${MINS} -d $(UBIN) $(ULIB) osdd

man:	ntar
	sh ./macrunch -n an ${MINS} -d $(UBIN) $(ULIB) an
	sh ./macrunch -t an ${MINS} -d $(UBIN) $(ULIB) an
ptx:	ntar
	sh ./macrunch ${MINS} -d $(UBIN) $(ULIB) ptx

install:
	${MAKE} -f macros.mk MINS=-m
	cp ${TMACFILES} ${INSTMAC}
	cd ${INSTMAC}; chmod 644 ${TMACFILES}
	cd ${INSTMAC}; chgrp bin ${TMACFILES}; chown bin ${TMACFILES}

insmmn:  ;  ${MAKE} -f macros.mk MINS=-m mmn
insmmt:  ;  ${MAKE} -f macros.mk MINS=-m mmt
insvmca: ;  ${MAKE} -f macros.mk MINS=-m vmca
insorg:  ;  ${MAKE} -f macros.mk MINS=-m org
insosdd: ;  ${MAKE} -f macros.mk MINS=-m osdd
insman:  ;  ${MAKE} -f macros.mk MINS=-m man
insptx:  ;  ${MAKE} -f macros.mk MINS=-m ptx

clean:
	:
clobber:  ntarclobber mmnclobber mmtclobber vmcaclobber \
		orgclobber osddclobber manclobber ptxclobber
	:
ntarclobber: ;  rm -f ntar
mmnclobber:  ;  rm -f mmn
mmtclobber:  ;  rm -f mmt
vmcaclobber: ;  rm -f vmca
orgclobber:  ;  rm -f org
osddclobber: ;  rm -f osdd
manclobber:  ;  rm -f an
ptxclobber:  ;  rm -f ptx

build:  bldmk bldntar bldmmn bldmmt bldvmca bldosdd bldman bldtmac \
		bldmacr bldptx
	:
bldntar: ;  get -p ${NTARSID} s.ntar.c > ${RDIR}/ntar.c
bldmk:   ;  get -p ${MKSID} s.macros.mk > ${RDIR}/macros.mk
bldmmn:  ;  get -p ${MMNSID} s.mmn.src ${REWIRE} > ${RDIR}/mmn.src
bldmmt:  ;  get -p ${MMTSID} s.mmt.src ${REWIRE} > ${RDIR}/mmt.src
bldvmca: ;  get -p ${VMSID} s.vmca.src ${REWIRE} > ${RDIR}/vmca.src
bldorg:  ;  get -p ${ORGSID} s.org.src ${REWIRE} > ${RDIR}/org.src
bldosdd: ;  get -p ${OSDDSID} s.osdd.src ${REWIRE} > ${RDIR}/osdd.src
bldman:  ;  get -p ${MANSID} s.an.src ${REWIRE} > ${RDIR}/an.src
bldptx:  ;  get -p -r`gsid ptxmacs ${REL}` s.ptx.src ${REWIRE} > ${RDIR}/ptx.src
bldtmac: ;  get -p ${TMSID} s.tmacs.src ${REWIRE} | ntar -d ${RDIR} -g
bldmacr: ;  get -p ${MACRUNCHSID} s.macrunch > ${RDIR}/macrunch
	    chmod 755 ${RDIR}/macrunch

listing:  listmk listntar listmmn listmmt listvmca listorg listosdd \
		listman listtmac listmacr listptx
	:
listntar: ; pr ntar.c | ${LIST}
listmk:  ;  pr macros.mk | ${LIST}
listmmn: ;  nl -ba mmn.src | pr -h "mmn.src" | ${LIST}
	    macref -s -t mmn.src | pr -h "macref of mmn.src" | ${LIST}
listmmt: ;  nl -ba mmt.src | pr -h "mmt.src" | ${LIST}
	    macref -s -t mmt.src | pr -h "macref of mmt.src" | ${LIST}
listvmca: ; nl -ba vmca.src | pr -h "vmca.src" | ${LIST}
	    macref -s -t vmca.src | pr -h "macref of vmca.src" | ${LIST}
listorg: ;  nl -ba org.src | pr -h "org.src" | ${LIST}
	    macref -s -t org.src | pr -h "macref of org.src" | ${LIST}
listosdd: ; nl -ba osdd.src | pr -h "osdd.src" | ${LIST}
	    macref -s -t osdd.src | pr -h "macref of osdd.src" | ${LIST}
listman: ;  nl -ba an.src | pr -h "an.src" | ${LIST}
	    macref -s -t an.src | pr -h "macref of an.src" | ${LIST}
listptx: ;  nl -ba ptx.src | pr -h "ptx.src" | ${LIST}
	    macref -s -t ptx.src | pr -h "macref of ptx.src" | ${LIST}
listtmac: ; pr ${TMACFILES} | ${LIST}
listmacr: ; pr macrunch | ${LIST}
edit:	ntaredit mmnedit mmtedit vmcaedit orgedit osddedit manedit \
		tmacsedit macredit mkedit ptxedit
	:
ntaredit: ;  get -e s.ntar.c
mmnedit:  ;  get -e s.mmn.src
mmtedit:  ;  get -e s.mmt.src
vmcaedit: ;  get -e s.vmca.src
orgedit:  ;  get -e s.org.src
osddedit: ;  get -e s.osdd.src
manedit:  ;  get -e s.an.src

ptxedit:  ;  get -e s.ptx.src
tmacsedit: ; get -e -p s.tmacs.src | ntar -g
macredit: ;  get -e s.macrunch
mkedit:   ;  get -e s.macros.mk

delta:	ntardelta mmndelta mmtdelta vmcadelta orgdelta osdddelta \
		mandelta tmacsdelta macrdelta mkdelta ptxdelta
	:
ntardelta: ;  delta s.ntar.c
mmndelta:  ;  delta s.mmn.src
mmtdelta:  ;  delta s.mmt.src
vmcadelta: ;  delta s.vmca.src
orgdelta:  ;  delta s.org.src
osdddelta: ;  delta s.osdd.src
mandelta:  ;  delta s.an.src
ptxdelta:  ;  delta s.ptx.src
tmacsdelta: ; ntar -p ${TMACFILES} > tmacs.src
	      delta s.tmacs.src
	      rm -f ${TMACFILES}
macrdelta: ;  delta s.macrunch
mkdelta:   ;  delta s.macros.mk

delete:	clean clobber ntarclobber mmndelete mmtdelete vmcadelete orgdelete \
		osdddelete mandelete tmacsdelete macrdelete ptxdelete
	:
ntardelete: ntarclobber
	    rm -f ntar.c
mmndelete:  mmnclobber
	    rm -f mmn.src
mmtdelete:  mmtclobber
	    rm -f mmt.src
vmcadelete: vmcaclobber
	    rm -f vmca.src
orgdelete:  orgclobber
	    rm -f org.src
osdddelete: osddclobber
	    rm -f osdd.src
mandelete:  manclobber
	    rm -f an.src
ptxdelete:  ptxclobber
	    rm -f ptx.src
tmacsdelete: ;  rm -f ${TMACFILES}
macrdelete: ;  	rm -f macrunch
