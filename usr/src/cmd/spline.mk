#	spline make file
#	SCCS:	@(#)spline.mk	2.1	
#	NOTE: This makefile is only necessary because of the need for the
#		base register option for cc on u370.

OL = 
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
TESTDIR = .
SL = /usr/src/cmd
RDIR = $(SL)
INS = :
REL = current
CSID = -r`gsid spline $(REL)`
MKSID = -r`gsid spline.mk $(REL)`
LIST = lp
INSDIR = $(OL)usr/bin
IFLAG = -n
B02 =
CFLAGS = -O $(B02)
LDFLAGS = -s 
SOURCE = spline.c
MAKE = make

compile all: spline

spline: spline.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/spline spline.o

spline.o:  spline.c
	$(CC) $(CFLAGS) -c spline.c

install:  all
	$(INSUB) $(TESTDIR)/spline
clean:
	rm -f spline.o

clobber:
	  rm -f $(TESTDIR)/spline

build:	bldmk
	get -p $(CSID) s.spline.c $(REWIRE) > $(RDIR)/spline.c
bldmk:  ;  get -p $(MKSID) s.spline.mk > $(RDIR)/spline.mk

listing:
	pr spline.mk $(SOURCE) | $(LIST)
listmk: ;  pr spline.mk | $(LIST)

edit:
	get -e s.spline.c

delta:
	delta s.spline.c

mkedit:  ;  get -e s.spline.mk
mkdelta: ;  delta s.spline.mk

delete:	clobber
	rm -f $(SOURCE)
