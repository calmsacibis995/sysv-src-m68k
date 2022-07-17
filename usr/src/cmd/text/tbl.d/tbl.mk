#	text Development Support Library (DSL) tbl make file
#	SCCS: @(#)tbl.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
CFLAGS = -O ${B20}
LDFLAGS = -s
SL = /usr/src/cmd
RDIR = ${SL}/text/tbl.d
REL = current
CSID = -r`gsid tbl ${REL}`
MKSID = -r`gsid tbl.mk ${REL}`
LIST = lp
B20 =
SFILES = t..c t[0-9].c t[bcefgimrstuv].c
OFILES = t0.o t1.o t2.o t3.o t4.o t5.o t6.o t7.o t8.o t9.o tb.o tc.o\
	te.o tf.o tg.o ti.o tm.o tr.o ts.o tt.o tu.o tv.o
MAKE = make

compile all: tbl
	:

tbl:	$(OFILES) 
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(TESTDIR)/tbl $(OFILES)

$(OFILES):: t..c
	:

install: all
	$(INSUB) $(TESTDIR)/tbl

clean:
	rm -f *.o

clobber:
	rm -f $(TESTDIR)/tbl

build:	bldmk
	get -p ${CSID} s.tbl.src ${REWIRE} | ntar -d ${RDIR} -g
bldmk: ; get -p ${MKSID} s.tbl.mk > ${RDIR}/tbl.mk

listing:
	pr tbl.mk ${SFILES} | ${LIST}
listmk:  ;  pr tbl.mk | ${LIST}

edit:
	get -p -e s.tbl.src | ntar -g

delta:
	ntar -p ${SFILES} > tbl.src
	delta s.tbl.src
	rm -f ${SFILES}

mkedit:	;  get -e s.tbl.mk
mkdelta: ; delta s.tbl.mk


delete:	clean clobber
	rm -f ${SFILES}
