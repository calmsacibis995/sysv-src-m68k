# @(#)makefile	2.1	
#
OL =
CFLAGS=	-O -DCM_N -DCM_GT -DCM_B -DCM_D
TESTDIR = .

compile all: termcap.a

termcap.a: termcap.o tgoto.o tputs.o
	ar cr $(TESTDIR)/termcap.a `lorder termcap.o tgoto.o tputs.o | tsort`

termcap.o: ../local/uparm.h termcap.c
	$(CC) $(CFLAGS) -c termcap.c

install: all
	rm -f $(OL)/usr/lib/libtermcap.a
	cp $(TESTDIR)/termcap.a $(OL)/usr/lib/libtermcap.a
	rm -f $(OL)/usr/lib/libtermlib.a
	ln $(OL)/usr/lib/libtermcap.a $(OL)/usr/lib/libtermlib.a
	chgrp bin $(OL)/usr/lib/libtermcap.a
	chown bin $(OL)/usr/lib/libtermcap.a

clean:
	rm -f *.o

clobber:
	rm -f $(TESTDIR)/termcap.a

delete: clean clobber
	rm -f *.c
