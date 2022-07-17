#	@(#)strip.mk	2.1	

#	strip.mk strip makefile

OL=
TESTDIR=.
BIN=$(OL)/bin
INSB=/etc/install -f $(BIN)
INSDIR = /bin
CURDIR = ..

compile all strip:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) TESTDIR=$(CURDIR); \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) INSDIR=$(CURDIR); \
	else \
		@echo 'Cannot make strip command: unknown target procesor.'; \
	fi


install: strip
	-if vax || pdp11; \
	then \
	        cp strip $(INSDIR)/strip; \
	        rm -f strip; \
	elif m68k; \
	then \
		$(INSB) $(TESTDIR)/strip; \
        fi
clean:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) clean; \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) clean; \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) clean; \
	fi

clobber:
	-if vax || pdp11; \
	then \
	        rm -f strip; \
	elif m68k; \
	then \
	        rm -f $(TESTDIR)/strip; \
        fi


