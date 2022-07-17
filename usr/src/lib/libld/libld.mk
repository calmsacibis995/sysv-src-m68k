#	@(#)libld.mk	2.1 	

#	GLOBAL makefile for libld.a library

#	Target system processors:	vax (VAX 11/780 and VAX 11/750)
#					pdp (PDP 11/70)
#					m68k (Motorola 68000)

OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	664		#protections

INCROOT =
ROOT = 
INSDIR = $(ROOT)/usr/lib
CURDIR = ..


all:	libld

libld:
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
		$(MAKE) INSDIR=$(CURDIR) ROOT=$(ROOT) INCROOT=$(INCROOT) \
			LIBDIR=$(CURDIR); \
	else \
		@echo 'Cannot make libld.a library: unknown target procesor.'; \
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

install: all save move

clobber: clean
	-rm -f libld.a

move:
	#
	# move the library or libraries into the correct directory

	rm -f $(INSDIR)/libld.a
	cp libld.a $(INSDIR)/libld.a ; rm -f libld.a
	chmod $(PROT) $(INSDIR)/libld.a
	chmod -x $(INSDIR)/libld.a	# not executable
	chgrp $(GRP) $(INSDIR)/libld.a
	chown $(OWN) $(INSDIR)/libld.a

#--------------------------

uninstall:	$(INSDIR)/OLDlibld.a 
		
		mv -f $(INSDIR)/OLDlibld.a $(INSDIR)/libld.a

#--------------------------

save:		
	if test -f $(INSDIR)/libld.a ; \
	then \
		rm -f $(INSDIR)/OLDlibld.a ; \
		cp $(INSDIR)/libld.a $(INSDIR)/OLDlibld.a ; \
		chmod $(PROT) $(INSDIR)/OLDlibld.a ; \
		chmod -x $(INSDIR)/OLDlibld.a ; \
		chgrp $(GRP) $(INSDIR)/OLDlibld.a ; \
		chown $(OWN) $(INSDIR)/OLDlibld.a ; \
	fi
