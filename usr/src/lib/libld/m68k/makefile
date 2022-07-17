#	@(#)makefile	2.1	
#	M68000 LIBLD MAKEFILE
#
#	@(#) makefile:	2.3 83/07/08
#
ROOT=
INCROOT=
COMINC= $(INCROOT)/usr/include

ARCH=	AR32W
ARFORMAT=	PORTAR
FLEX=	-DFLEXNAMES

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/usr/lib

COMS=	../common

# Name of object file access routines library

LIBLD=	$(LIBDIR)/libld.a

AR=		ar
ARFLAGS=	r
CC=		cc
CFLAGS=		-O
LINT=		lint
LFLAGS=		-sp
MAKE=		make
MAKEARGS=	ROOT="$(ROOT)" \
		BINDIR="$(BINDIR)" LIBDIR="$(LIBDIR)" \
		INCCOM="$(COMINC)" \
		AS="$(AS)" CC="$(CC)" LINT="$(LINT)" \
		AR="$(AR)" \
		CFLAGS="$(CFLAGS)" LINTFLAGS="$(LFLAGS)" \
		ARFLAGS="$(ARFLAGS)" FLEX=$(FLEX) \
		ARCH="$(ARCH)" ARFORMAT="$(ARFORMAT)"

#------------------------

build:		$(COMS)/libld.a

$(COMS)/libld.a:
		cd $(COMS); \
		$(MAKE) $(MAKEARGS) all

#-------------------------

install:	$(COMS)/libld.a
		-rm -f $(LIBLD)
		cp $(COMS)/libld.a $(LIBLD)

#--------------------------

save:		$(LIBLD)
		-rm -f $(LIBLD).back
		cp $(LIBLD) $(LIBLD).back

#--------------------------

uninstall:	$(LIBLD).back
		-rm -f $(LIBLD)
		cp $(LIBLD).back $(LIBLD)

#--------------------------

clean:
	cd $(COMS); \
	$(MAKE) $(MAKEARGS) clean

#--------------------------

clobber:
		cd $(COMS); \
		$(MAKE) $(MAKEARGS) clobber

#--------------------------

lint:
		cd $(COMS); \
		$(MAKE) $(MAKEARGS) lint
