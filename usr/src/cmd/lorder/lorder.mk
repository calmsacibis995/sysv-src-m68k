#	@(#)lorder.mk	2.1	

#	lorder.mk lorder makefile

OL =
TESTDIR = ..
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)

INSDIR = /bin
CURDIR = ..

compile all: lorder
lorder:
	-if m68k; \
	then \
		cd m68k; \
		$(MAKE) TESTDIR=$(TESTDIR); \
	elif vax; \
	then \
		cd vax; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) INSDIR=$(CURDIR); \
	else \
		@echo 'Cannot make lorder command: unknown target procesor.'; \
	fi

install: lorder
	-if vax || pdp11; \
	then \
	        cp lorder $(INSDIR)/lorder; \
        	rm -f lorder; \
	elif m68k; \
	then \
		$(INSB) lorder; \
	fi

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
	        rm -f lorder; \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) clobber; \
	fi
