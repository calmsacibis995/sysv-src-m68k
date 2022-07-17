#
#	MC68000 C STARTUP MAKEFILE
#
#	@(#)makefile	2.1	
#

CC=	cc
CFLAGS=	-O

OFILES=		crt0.o
PFILES=		mcrt0.o
FILES=		$(OFILES) $(PFILES)

prof:	$(PFILES)

nonprof:	$(OFILES)

all:		$(FILES)

#-------------------------

.s.o:
	@echo $*.s:
	$(CC) $(CFLAGS) -c $*.s

#-------------------------

clean:
		rm -f *.o

#--------------------------

clobber:	clean
