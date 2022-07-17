#
#	@(#)sdb.mk	2.1.1.1	

##	MJM: makefile for sdb
CFLAGS=	-O -DDEBUG
CFLAGS=	-O -DDEBUG=2
CFLAGS=	-O
CC = cc
OL =
TESTDIR = ..
INSDIR =
INS = /etc/install -n /usr/bin
#		Hardware:	(mjm: not using)
#			vaxsdb - sdb for DEC VAX-11 
#			3b20sdb - sdb for BELL 3B20
#			m68ksdb - sdb for Motorola MC68000
#		Operating Systems:	(mjm: not using)
#			VAX135 - Department 1353; Bell Labs
#			UCBVAX - VM/UNIX 3.x and beyond Berkeley version
#			SYSTEM V/68 - System V
#			VFORK - Use vfork call of VM/UNIX Berkeley version
#		---->	STD - UNIX/32V, VM/UNIX 2.n, UNIX/TS, PWB UNIX, U3B
#	VM/UNIX 3.x uses new trap format which is shared with
#		VAX135 systems (head.h, setup.h)
##	-DDEBUG[=1]:	original sdb debugging
##	-DDEBUG=2:	mjm debugging


#Depends on make_all, not sdb and libg.a, so that cd and make not done twice
compile all:	make_all

make_all:
		@if vax ; then cd vax ; \
		exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" ;\
		fi;
		@if u3b ; then cd u3b ; \
		exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" ;\
		fi;
		@if m68k ; then cd m68k ; \
		exec make -f makefile  TESTDIR="$(TESTDIR)" OL=$(OL) ;\
		fi;

sdb:
	@if vax ; then cd vax ; \
	exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" \
	../sdb ; fi;
	@if u3b ; then cd u3b ; \
	exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" \
	../sdb ; fi;
	@if m68k ; then cd m68k ; \
	exec make -f makefile  TESTDIR="$(TESTDIR)" OL=$(OL) \
	../sdb ; fi;

libg.a:
	@if vax ; then cd vax ; \
	exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" \
	../libg.a ; fi;
	@if u3b ; then cd u3b ; \
	exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" \
	../libg.a ; fi;
	@if m68k ; then cd m68k ; \
	exec make -f makefile  TESTDIR="$(TESTDIR)" OL=$(OL) \
	../libg.a ; fi;

install:
	@if vax ; then cd vax ; \
	exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" \
	INSDIR="$(INSDIR)" install ; fi ;
	@if u3b ; then cd u3b ; \
	exec make -f makefile CFLAGS="$(CFLAGS)" TESTDIR="$(TESTDIR)" \
	INSDIR="$(INSDIR)" install ; fi ;
	@if m68k ; then cd m68k ; \
	exec make -f makefile TESTDIR="$(TESTDIR)" OL=$(OL) \
	install ; fi ;

clean :
	@if vax ; then cd vax ; exec make -f makefile TESTDIR="$(TESTDIR)" \
	clean ; fi ;
	@if u3b ; then cd u3b ; exec make -f makefile TESTDIR="$(TESTDIR)" \
	clean ; fi ;
	@if m68k ; then cd m68k ; exec make -f makefile \
	clean ; fi ;

clobber:
	@if vax ; then cd vax ; exec make -f makefile TESTDIR="$(TESTDIR)" \
	clobber ; fi ;
	@if u3b ; then cd u3b ; exec make -f makefile TESTDIR="$(TESTDIR)" \
	clobber ; fi ;
	@if m68k ; then cd m68k ; exec make -f makefile TESTDIR="$(TESTDIR)" \
	clobber ; fi ;
