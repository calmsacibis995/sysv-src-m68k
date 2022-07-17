#	unpack (pcat) make file
#	SCCS:	@(#)pcat.mk	2.1	

OL = /
SL = /usr/src/cmd
RDIR = $(SL)
INS = :
REL = current
CSID = -r`gsid unpack $(REL)`
MKSID = -r`gsid unpack.mk $(REL)`
LIST = lp
INSDIR = $(OL)usr/bin
IFLAG = -n
CFLAGS = -O
LDFLAGS = -s $(IFLAG)
68LDFLAGS = -s
SOURCE = unpack.c
MAKE = make

compile all: unpack
	:

unpack:	; if m68k; \
	then \
	$(CC) $(CFLAGS) $(68LDFLAGS) -o unpack unpack.c; \
	else \
	$(CC) $(CFLAGS) $(LDFLAGS) -o unpack unpack.c; \
	fi
	$(INS) unpack $(INSDIR)
	rm -f $(INSDIR)/pcat
	ln $(INSDIR)/unpack $(INSDIR)/pcat
	chmod 775 $(INSDIR)/unpack
	@if [ "$(OL)" = "/" ]; \
		then cd $(INSDIR); chown bin unpack; chgrp bin unpack; \
	 fi

pcat:	unpack
	:

install:
	$(MAKE) -f unpack.mk INS=cp OL=$(OL)

build:	bldmk
	get -p $(CSID) s.unpack.c $(REWIRE) > $(RDIR)/unpack.c
bldmk:
	get -p $(MKSID) s.unpack.mk > $(RDIR)/unpack.mk
	rm -f $(RDIR)/pcat.mk
	ln $(RDIR)/unpack.mk $(RDIR)/pcat.mk

listing:
	pr unpack.mk $(SOURCE) | $(LIST)
listmk: ;  pr unpack.mk | $(LIST)

edit:
	get -e s.unpack.c

delta:
	delta s.unpack.c

mkedit:  ;  get -e s.unpack.mk
mkdelta: ;  delta s.unpack.mk

clean:
	:

clobber:
	  rm -f unpack

delete:	clobber
	rm -f $(SOURCE)
