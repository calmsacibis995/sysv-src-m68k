# @(#)pwb.mk	2.1.1.1	
ROOT = /
INCRT = $(ROOT)usr/include
LIBNAME = ../lib3
MACH = M68000
TYPE = MEXOR
CFLAGS = -O -T -D$(MACH) -D$(TYPE)
OWN = bin
GRP = bin
FRC =

FILES =\
	$(LIBNAME)(prof.o)

all:	$(LIBNAME) 

.PRECIOUS:	$(LIBNAME)

$(LIBNAME):	$(FILES)
#	-chmod 664 $(LIBNAME)
#	-chgrp $(GRP) $(LIBNAME)
#	-chown $(OWN) $(LIBNAME)

.c.a:
	M68000= $(CC) -c $(CFLAGS) $<
	chmod 664 $*.o
	chgrp bin $*.o
	chown bin $*.o
	ar rv $@ $*.o
	-rm -f $*.o

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)

$(LIBNAME)(prof.o): prof.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/buf.h\
	$(FRC)

FRC:
