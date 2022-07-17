#	spell make file
#	SCCS:  @(#)spell.mk	2.1	

OL =
BIN = $(OL)/usr/bin
PINSDIR = $(OL)/usr/lib/spell
INSB = /etc/install -f $(BIN)
INSUL = /etc/install -f $(PINSDIR)
TESTDIR = .
CFLAG = -O
LDFLAG = -s

SL = /usr/src/cmd
RDIR = ${SL}/spell
REL = current
CSID = -r`gsid spellcode ${REL}`
DSID = -r`gsid spelldata ${REL}`
SHSID = -r`gsid spell.sh ${REL}`
CMPRSID = -r`gsid compress.sh ${REL}`
MKSID = -r`gsid spell.mk ${REL}`
LIST = lp
SFILES = spellprog.c spellin.c
DFILES = american british local list extra stop
MAKE = make

compile all: spell compress spellprog spellin hashcheck hashmake alldata

spell:	spellprog spell.sh
	cp spell.sh $(TESTDIR)/spell

spelldir:
	-if test ! \( -d $(PINSDIR) \) ; then mkdir $(PINSDIR); fi

compress:  compress.sh
	cp compress.sh $(TESTDIR)/compress

spellprog: spellprog.c hash.c hashlook.c huff.c malloc.c
	$(CC) $(CFLAG) $(LDFLAG) spellprog.c hash.c hashlook.c huff.c malloc.c -o $(TESTDIR)/spellprog

spellin: spellin.c huff.c
		$(CC) $(CFLAG) $(LDFLAG) spellin.c huff.c -o $(TESTDIR)/spellin;

hashcheck: hashcheck.c hash.c huff.c
	$(CC) $(CFLAG) $(LDFLAG) hashcheck.c hash.c huff.c -o $(TESTDIR)/hashcheck

hashmake: hashmake.c hash.c
		$(CC) $(CFLAG) $(LDFLAG) hashmake.c hash.c -o $(TESTDIR)/hashmake;

alldata: hlista hlistb hstop
	rm htemp1

htemp1:	list local extra hashmake
	-cat list local extra | ./hashmake >htemp1

hlista: american hashmake spellin htemp1
	-./hashmake <american |sort -u - htemp1 >htemp2
	-./spellin `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlista
	rm htemp2

hlistb: british hashmake spellin htemp1
	-./hashmake <british |sort -u - htemp1 >htemp2
	-./spellin `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlistb
	rm htemp2

hstop:	stop spellin hashmake
	-./hashmake <stop | sort -u >htemp2
	-./spellin `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hstop
	rm htemp2

install:  spelldir all inssh inscomp inscode insdata

inssh:
	$(INSB) spell

inscomp:
	$(INSUL) compress

inscode:
	$(INSUL) $(TESTDIR)/spellprog
	$(INSUL) $(TESTDIR)/spellin
	$(INSUL) $(TESTDIR)/hashcheck
	$(INSUL) $(TESTDIR)/hashmake

insdata:
	cp hlista $(PINSDIR); cp hlistb $(PINSDIR); cp hstop $(PINSDIR)
	cd $(PINSDIR); \
	chmod 644 hlista; chgrp bin hlista; chown bin hlista; \
	chmod 644 hlistb; chgrp bin hlistb; chown bin hlistb; \
	chmod 644 hstop; chgrp bin hstop; chown bin hstop; \
	>spellhist; \
	chmod 666 spellhist; chgrp bin spellhist; chown bin spellhist;

clean:
	rm -f *.o

clobber: 
	rm -f $(TESTDIR)/spellprog
	rm -f $(TESTDIR)/spellin
	rm -f $(TESTDIR)/hashcheck
	rm -f $(TESTDIR)/hashmake
	rm -f $(TESTDIR)/spell
	rm -f $(TESTDIR)/compress

shclobber:
	rm -f spell

compclobber:
	rm -f compress

delete:	clobber shdelete compdelete
	rm -f ${SFILES} ${DFILES}

shdelete: shclobber
	rm -f spell.sh

compdelete: compclobber
	rm -f compress.sh

listing:  ;  pr spell.mk spell.sh compress.sh ${SFILES} ${DFILES} | ${LIST}
listmk:   ;  pr spell.mk | ${LIST}
listsh:	  ;  pr spell.sh | ${LIST}
listcomp: ;  pr compress.sh | ${LIST}
listcode: ;  pr ${SFILES} | ${LIST}
listdata: ;  pr ${DFILES} | ${LIST}

build:  bldmk bldsh bldcomp bldcode blddata
	:
bldcode:  ;  get -p ${CSID} s.spell.src ${REWIRE} | ntar -d ${RDIR} -g
blddata:  ;  get -p ${DSID} s.spell.data | ntar -d ${RDIR} -g
bldsh:	  ;  get -p ${SHSID} s.spell.sh ${REWIRE} > ${RDIR}/spell.sh
bldcomp:  ;  get -p ${CMPRSID} s.compress.sh ${REWIRE} > ${RDIR}/compress.sh
bldmk:    ;  get -p ${MKSID} s.spell.mk > ${RDIR}/spell.mk

edit:	sedit dedit mkedit shedit compedit
	:
sedit:	;  get -p -e s.spell.src | ntar -g
dedit:	;  get -p -e s.spell.data | ntar -g
shedit:	;  get -e s.spell.sh
compedit: ; get -e s.compress.sh

delta:	sdelta ddelta mkdelta shdelta compdelta
	:
sdelta:
	ntar -p ${SFILES} > spell.src
	delta s.spell.src
	rm -f ${SFILES}
ddelta:
	ntar -p ${DFILES} > spell.data
	delta s.spell.data
	rm -f ${DFILES}
shdelta:
	delta s.spell.sh
compdelta: ; delta s.compress.sh

mkedit:	;  get -e s.spell.mk
mkdelta: ; delta s.spell.mk
