# @(#)m68k.mk	2.1.1.2	
all:	machine system drivers pwbstuff

machine:
	cd ml; $(MAKE) -f ml.mk "FRC=$(FRC)" "INCRT=$(INCRT)" \
		"ROOT=$(ROOT)" "MACH=$(MACH)" "TYPE=$(TYPE)"\
		"OWN=$(OWN)" "GRP=$(GRP)" "CFLAGS=$(CFLAGS)"

system:
	cd os; $(MAKE) -f os.mk "FRC=$(FRC)" "INCRT=$(INCRT)" \
		"ROOT=$(ROOT)" "MACH=$(MACH)" "TYPE=$(TYPE)"\
		"OWN=$(OWN)" "GRP=$(GRP)" "CFLAGS=$(CFLAGS)"

drivers:
	cd io; $(MAKE) -f io.mk "FRC=$(FRC)" "INCRT=$(INCRT)" \
		"ROOT=$(ROOT)" "MACH=$(MACH)" "TYPE=$(TYPE)"\
		"OWN=$(OWN)" "GRP=$(GRP)" "CFLAGS=$(CFLAGS)"

pwbstuff:
	cd pwb; $(MAKE) -f pwb.mk "FRC=$(FRC)" "INCRT=$(INCRT)"\
		"ROOT=$(ROOT)" "MACH=$(MACH)" "TYPE=$(TYPE)"\
		"OWN=$(OWN)" "GRP=$(GRP)" "CFLAGS=$(CFLAGS)"

clean:
	cd ml; $(MAKE) -f ml.mk clean
	cd os; $(MAKE) -f os.mk clean
	cd io; $(MAKE) -f io.mk clean
	cd pwb; $(MAKE) -f pwb.mk clean

clobber:
	cd ml; $(MAKE) -f ml.mk clobber
	cd os; $(MAKE) -f os.mk clobber
	cd io; $(MAKE) -f io.mk clobber
	cd pwb; $(MAKE) -f pwb.mk clobber

FRC:
