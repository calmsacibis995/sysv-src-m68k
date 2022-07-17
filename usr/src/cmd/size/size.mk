#	@(#)size.mk	2.1	

#	size.mk size makefile

OL =
TESTDIR = ..
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)

INSDIR = /bin
CURDIR = ..

compile all size :
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
		@echo 'Cannot make size command: unknown target procesor.'; \
	fi


install: size
	-if vax || pdp11; \
	then \
		cp size $(INSDIR)/size; \
		rm -f size; \
	elif m68k; \
	then \
		$(INSB) size; \
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
		rm -f size; \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) clobber; \
	fi

