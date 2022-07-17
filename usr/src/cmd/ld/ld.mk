#	@(#)ld.mk	2.1

#	ld.mk ld makefile

INSDIR = /bin
CURDIR = ..

ld:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) INSDIR=$(CURDIR); \
	else \
		@echo 'Cannot make ld command: unknown target procesor.'; \
	fi



clean:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) clean; \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) clean; \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) clean; \
	fi


install:  ld
	-if m68k; \
	then \
		cp m68k/ld $(INSDIR)/ld \
		-rm -f m68k/ld \
	else \
		cp ld $(INSDIR)/ld \
		-rm -f ld \
	fi

clobber: clean
	-rm -f ld
