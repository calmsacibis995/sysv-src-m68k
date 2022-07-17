#	model make file
#	SCCS:	@(#)model.mk	2.1	

OL =
SL	= /usr/src/cmd
RDIR	= $(SL)/lp/model
REL	= current
OWN	= bin
GRP	= bin
LIST	= lp
INSDIR	= $(OL)/usr/spool/lp/model
INS = /etc/install -f $(INSDIR)
SOURCE	= 1640 dumb f450 hp prx pprx
MAKE	= make

compile all:

new:
	if [ ! -d $(INSDIR) ]; then mkdir $(INSDIR); fi

install:	all new
	-cp 1640 dumb f450 hp prx pprx $(INSDIR);
	-chmod 775 $(INSDIR)/*;
	-chown $(OWN) $(INSDIR)/*;
	-chgrp $(GRP) $(INSDIR)/*;

#######################################################################
#################################DSL only section - for development use

build:	bldmk
	get -p -r`gsid model $(REL)` s.model.src $(REWIRE) | ntar -d $(RDIR) -g
bldmk:  ;  get -p -r`gsid model.mk $(REL)` s.model.mk > $(RDIR)/model.mk

listing:
	pr model.mk $(SOURCE) | $(LIST)
listmk: ;  pr model.mk | $(LIST)

edit:
	get -e -p s.model.src | ntar -g

delta:
	ntar -p $(SOURCE) > model.src
	delta s.model.src
	rm -f $(SOURCE)

mkedit:  ;  get -e s.model.mk
mkdelta: ;  delta s.model.mk
#######################################################################

clean:	; :
clobber:  ; :
delete:	; :
	rm -f $(SOURCE)
