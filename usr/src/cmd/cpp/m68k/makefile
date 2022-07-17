#	@(#)makefile	2.1	
#	MC68000 CPP MAKEFILE
#
#	@(#) makefile:	2.2 83/07/08
#
OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	775		#protections
ROOT=

BINDIR=	$(ROOT)/bin
LIBDIR=	$(ROOT)/lib
COMSRC=	../common

CC=	cc
LINT=	lint
LFLAGS=	-p
CFLAGS=	-O
LDFLAGS=
DEFLIST=
INCLIST=	-I$(COMSRC)

MACHINES= vax pdp11 u3b u3b5 m68k
CROSSMACH=

FLEX=	-DFLEXNAMES

CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(FLEX) $(INCLIST)


OFILES=	cpp.o cpy.o rodata.o
CFILES=	$(COMSRC)/cpp.c cpy.c rodata.c


build:	$(CROSSMACH).do cpp $(CROSSMACH).od

#-------------------------
cpp:	$(OFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o cpp $(OFILES)

cpy.c rodata.c:	$(COMSRC)/cpy.y
	$(YACC) $(COMSRC)/cpy.y
	-if pdp11 || vax || m68k; \
	then \
		chmod +x $(COMSRC)/:yyfix; \
		$(COMSRC)/:yyfix >rodata.c; \
	fi
	mv y.tab.c cpy.c

cpy.o:	cpy.c $(COMSRC)/yylex.c
	-if pdp11; then $(CC_CMD) -UFLEXNAME	-c ./cpy.c; \
	else		$(CC_CMD)		-c ./cpy.c; fi

cpp.o:	$(COMSRC)/cpp.c
	-if test "$(CROSSMACH)" = "" ; \
	then \
		if pdp11; then $(CC_CMD) -Dunix -Dpdp11 -UFLEXNAME -c $(COMSRC)/cpp.c; \
		elif vax; then $(CC_CMD) -Dunix -Dvax -c $(COMSRC)/cpp.c; \
		elif u3b; then $(CC_CMD) -Dunix -Du3b              -c $(COMSRC)/cpp.c; \
		elif u3b5; then $(CC_CMD) -Dunix -Du3b5            -c $(COMSRC)/cpp.c; \
		elif m68k; then $(CC_CMD) -Dunix -Dm68k	-c $(COMSRC)/cpp.c; \
		else       echo 'Unknown machine???'; exit 2; \
		fi ; \
	else \
		$(CC_CMD) -Dunix -DCROSSMACH=\"$(CROSSMACH)\" \
			-c $(COMSRC)/cpp.c ; \
	fi

rodata.o:	rodata.c
		$(CC_CMD) -S rodata.c
		-if test "$(CROSSMACH)" = "" ; \
		then \
			if pdp11 || vax; \
			then \
				chmod +x :rofix; \
				:rofix rodata.s; \
			elif m68k; \
			then \
				chmod +x :rofix68; \
				:rofix68 rodata.s; \
			fi ; \
		else \
			if /bin/pdp11 || /bin/vax ; \
			then \
				chmod +x ../vax/:rofix; \
				../vax/:rofix rodata.s; \
			elif /bin/m68k; \
			then \
				chmod +x :rofix68; \
				:rofix68 rodata.s; \
			fi ; \
		fi
		$(CC_CMD) rodata.s

$(CROSSMACH).do:
		if test "$(CROSSMACH)" = "" ; \
		then \
			: ; \
		else \
			echo "true" > _$(CROSSMACH) ; \
			mv _$(CROSSMACH) $(CROSSMACH) ; \
			for i in $(MACHINES) ; \
			do \
				if test $$i != "$(CROSSMACH)" ; \
				then \
					echo "false" > _$$i ; \
					mv _$$i $$i ; \
				fi ; \
			done ; \
		fi

$(CROSSMACH).od:
		-rm -f $(MACHINES)


#-------------------------
install:	cpp
		-rm -f $(LIBDIR)/cpp
		cp cpp $(LIBDIR)
		strip $(LIBDIR)/cpp
		chmod $(PROT) $(LIBDIR)/cpp
		chgrp $(GRP) $(LIBDIR)/cpp
		chown $(OWN) $(LIBDIR)/cpp

#--------------------------

save:	$(LIBDIR)/cpp
	-rm -f $(LIBDIR)/cpp.back
	cp $(LIBDIR)/cpp $(LIBDIR)/cpp.back

#--------------------------

uninstall:	$(LIBDIR)/cpp.back
		-rm -f $(LIBDIR)/cpp
		cp $(LIBDIR)/cpp.back $(LIBDIR)/cpp

#--------------------------

clean:
		-rm -f $(OFILES) cpy.c rodata.c rodata.s

#--------------------------

clobber:	clean
		-rm -f cpp

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) -I$(M68INC) $(CFILES) >lint.out
