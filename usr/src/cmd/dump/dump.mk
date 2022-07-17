#	@(#)dump.mk	2.1.1.1	

#	dump.mk dump makefile

OL=
TESTDIR= ..
BIN= $(OL)/bin
INSB= /etc/install -f $(BIN)

INSDIR = /bin
CURDIR = ..

compile all dump:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) TESTDIR=$(TESTDIR); \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) INSDIR=$(CURDIR); \
	else \
		@echo 'Cannot make dump command: unknown target procesor.'; \
	fi

install:  dump
	-if vax || pdp11; \
	then \
		cp dump $(INSDIR)/dump; \
		rm -f dump; \
	elif m68k; \
	then \
		$(INSB) dump; \
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
		rm -f dump; \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) clobber; \
	fi
