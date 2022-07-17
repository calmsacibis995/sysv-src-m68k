#	@(#)ar.mk	2.1.1.1	

#	ar.mk ar makefile

OL=
TESTDIR=.
BIN=$(OL)/bin
INSB=/etc/install -f $(BIN)
INSDIR = /bin
CURDIR = ..

compile all ar:
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
	elif u3b; \
	then \
		cd u3b; \
		$(MAKE) INSDIR=$(CURDIR); \
	else \
		echo 'Cannot make ar command: unknown target procesor.'; \
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
	elif u3b; \
	then \
		cd u3b; \
		$(MAKE) clean; \
	fi


install: ar
	-if vax || pdp11 || u3b; \
	then \
	        cp ar $(INSDIR)/ar; \
	        rm -f ar; \
	elif m68k; \
	then \
		$(INSB) $(TESTDIR)/ar; \
        fi

clobber:
	-if vax || pdp11 || u3b; \
	then \
	        rm -f ar; \
	elif m68k; \
	then \
	        rm -f $(TESTDIR)/ar; \
        fi
