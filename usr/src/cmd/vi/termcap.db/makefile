# @(#)makefile	2.1	
# reorder gives an editor command for most common terminals
# (in reverse order from n'th to 1'st most commonly used)
# to move them to the front of termcap
#
OL =
ETC = $(OL)/etc
ULIB = $(OL)/usr/lib

compile all: termcap.obj

termcap.obj: termcap.src reorder
	ed - termcap.src < reorder

install: all
	rm -f $(ETC)/termcap
	cp termcap.obj $(ETC)/termcap
	chgrp bin $(ETC)/termcap
	chown bin $(ETC)/termcap

tabsets:	
	-mkdir $(ULIB)/tabset # don't care if these fail
	-cp tabset/* $(ULIB)/tabset
	-chgrp bin $(ULIB)/tabset/*
	-chown bin $(ULIB)/tabset/*
	-chgrp bin $(ULIB)/tabset
	-chown bin $(ULIB)/tabset

clean:

clobber:

delete:
	rm -f termcap.obj termcap.src reorder
#
#
#
VGRIND = pr
vgrind:
	$(VGRIND) -h "Terminal Capibility Data Base" \
	termcap.src reorder makefile | lp
#
# VGRIND=	csh /usr/ucb/vgrind
# vgrind:
# 	$(VGRIND) -n -h "Terminal Capibility Data Base" \
# 	termcap.src reorder makefile
