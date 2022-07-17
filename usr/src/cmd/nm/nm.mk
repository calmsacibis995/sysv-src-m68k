#	@(#)nm.mk	2.1	
#	nm.mk - nm makefile

OL =
TESTDIR = .
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)

INSDIR = $(OL)/bin
INS = /etc/install -f $(INSDIR)

compile all: nm

nm:
	-if m68k; \
	then \
		cd m68k; \
		$(MAKE) TESTDIR=..; \
	elif vax; \
	then \
		cd vax; \
		$(MAKE) TESTDIR=..; \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) TESTDIR=..; \
	else \
		@echo 'Cannot make nm command: unknown target procesor.'; \
	fi

install: all
	$(INSB) $(TESTDIR)/nm

clean:
	-if m68k; \
	then \
		cd m68k; \
		$(MAKE) clean; \
	elif vax; \
	then \
		cd vax; \
		$(MAKE) clean; \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) clean; \
	fi

clobber:
	-if vax || pdp11; \
	then \
                rm -f nm; \
	elif m68k; \
	then \
		rm -f $(TESTDIR)/nm; \
	fi
