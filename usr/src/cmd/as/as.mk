#	@(#)as.mk	2.1.1.1	

#	as.mk as makefile

INSDIR = /bin
CURDIR = ..

as:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) INSDIR=$(CURDIR); \
		cd ../lj_vax; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif m68k; \
	then \
		cd m68k; \
		$(MAKE) INSDIR=$(CURDIR); \
	elif pdp11; \
	then \
		cd pdp11; \
		$(MAKE) INSDIR=$(CURDIR); \
	else \
		@echo 'Cannot make as command: unknown target procesor.'; \
	fi



clean:
	-if vax; \
	then \
		cd vax; \
		$(MAKE) clean; \
		cd ../lj_vax; \
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


install: as
	-if vax; \
	then \
		cp as $(INSDIR)/as; \
		cp ljas $(INSDIR)/ljas; \
	elif m68k; \
	then \
		cp m68k/as $(INSDIR)/as; \
		rm -f $(INSDIR)/ljas; \
		ln $(INSDIR)/as $(INSDIR)/ljas; \
	elif pdp11; \
	then \
		cp as2 /lib/as2; \
	fi

clobber: clean
	-rm -f ljas as as2
