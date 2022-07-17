#	checkmm make file
#	SCCS:	@(#)checkmm.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
CFLAGS = -O 
LDFLAGS = -s # -n

SL = /usr/src/cmd
RDIR = ${SL}/text/checkmm
REL = current
CSID = -r`gsid checkmm ${REL}`
MKSID = -r`gsid checkmm.mk ${REL}`
LIST = lp
B10 =
SOURCE = chekl.l chekmain.c
FILES = chekl.o chekmain.o
MAKE = make

compile all: checkmm
	:

checkmm:	$(FILES)
	$(CC) ${LDFLAGS} -o $(TESTDIR)/checkmm $(FILES) -ll -lPW

$(FILES)::
	:

install: all
	${INSUB} $(TESTDIR)/checkmm

clean:
	-rm -f $(FILES)

clobber:
	-rm -f $(TESTDIR)/checkmm

build:	bldmk
	get -p ${CSID} s.checkmm.src ${REWIRE} | ntar -d ${RDIR} -g
bldmk:  ;  get -p ${MKSID} s.checkmm.mk > ${RDIR}/checkmm.mk

listing:
	pr checkmm.mk ${SOURCE} | ${LIST}
listmk: ;  pr checkmm.mk | ${LIST}

edit:
	get -e -p s.checkmm.src | ntar -g

delta:
	ntar -p ${SOURCE} > checkmm.src
	delta s.checkmm.src
	rm -f ${SOURCE}

mkedit:  ;  get -e s.checkmm.mk
mkdelta: ;  delta s.checkmm.mk

delete:	clean clobber
	rm -f ${SOURCE}
