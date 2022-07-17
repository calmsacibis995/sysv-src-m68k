#	@(#)uts.mk	2.1	
all:
	@-if vax ;\
	then cd vax;\
	make -f vax.mk "INS=$(INS)" "INSDIR=$(INSDIR)" "INCRT=$(INCRT)"\
		"FRC=$(FRC)" "SYS=$(SYS)" "NODE=$(NODE)" "REL=$(REL)"\
		"VER=$(VER)" "MACH=$(MACH)" "TYPE=$(TYPE)";\
	else if m68k;\
	then cd m68k;\
	make -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" "INCRT=$(INCRT)"\
		"FRC=$(FRC)" "SYS=$(SYS)" "NODE=$(NODE)" "REL=$(REL)"\
		"VER=$(VER)" "MACH=$(MACH)" "TYPE=$(TYPE)" "$ROOT=$(ROOT)"\
		"OWN=$(OWN)" "GRP=$(GRP)";\
	else cd pdp11;\
	make -f pdp11.mk "INS=$(INS)" "INSDIR=$(INSDIR)" "INCRT=$(INCRT)"\
		"FRC=$(FRC)" "SYS=$(SYS)" "NODE=$(NODE)" "REL=$(REL)"\
		"VER=$(VER)" "MACH=$(MACH)" "TYPE=$(TYPE)";\
	fi;\
	fi

clean:
	@-if vax ; then cd vax; make -f vax.mk clean ;\
	else if m68k ; then cd m68k; make -f m68k.mk clean ;\
	else cd pdp11; make -f pdp11.mk clean ; fi ; fi

clobber:
	@-if vax ; then cd vax; make -f vax.mk clobber "SYS=$(SYS)" "VER=$(VER)" ; \
	else if m68k; then cd m68k; make -f m68k.mk clobber "SYS=$(SYS)" "VER=$(VER)" ;\
	else cd pdp11; make -f pdp11.mk clobber "SYS=$(SYS)" "VER=$(VER)" ; fi
