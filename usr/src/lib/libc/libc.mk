#	@(#)libc.mk	2.1	
#
# makefile for libc
#
#
# The variable PROF is null by default, causing both the standard C library
# and a profiled library to be maintained.  If profiled object is not 
# desired, the reassignment PROF=@# should appear in the make command line.
#
# The variable IGN may be set to -i by the assignment IGN=-i in order to
# allow a make to complete even if there are compile errors in individual
# modules.
#
# See also the comments in the lower-level machine-dependent makefiles.
#

OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	664		#protections

VARIANT=
CFLAGS=-O
PCFLAGS=
PFX=
CC=$(PFX)cc
AR=$(PFX)ar
STRIP=$(PFX)strip
LORDER=$(PFX)lorder
ROOT=					# default root directory
INCROOT=				# default include parent directory
LIB=$(ROOT)/lib
LIBP=$(ROOT)/lib/libp

PROF=
DONE=

all:
	if m68k ; then make -e -f libc.mk specific MACHINE=m68k \
		INCROOT=$(INCROOT) ; fi
	if pdp11 ; then make -e -f libc.mk specific MACHINE=pdp11 ; fi
	if vax ;   then make -e -f libc.mk specific MACHINE=vax ; fi
	if u3b ;   then make -e -f libc.mk specific MACHINE=u3b ; fi
	if u370 ;  then make -e -f libc.mk specific MACHINE=u370 ; fi

specific:
	#
	# compile portable library modules
	cd port; make -e CC=$(CC) INCROOT=$(INCROOT)
	#
	# compile machine-dependent library modules
	cd $(MACHINE); make -e all CC=$(CC) INCROOT=$(INCROOT)
	#
	# place portable modules in "object" directory, then overlay
	# 	the machine-dependent modules.
	#
	# The following two commands had to be changed as the command lines
	# became to long.  The replacement command lines follow, uncommented.
	# cp port/*/*.[op] object
	# cp $(MACHINE)/*/*.[op] object
	#
	-rm -rf object
	mkdir object
	cp port/*/*.o object
	$(PROF) cp port/*/*.p object
	cp $(MACHINE)/*/*.o object
	$(PROF) cp $(MACHINE)/*/*.p object
	#
	# delete temporary libraries
	-rm -f lib.libc
	$(PROF)-rm -f libp.libc
	#
	# set aside run-time modules, which don't go in library archive!
	cd object; for i in *crt0.o ; do mv $$i .. ; done
	#
	# build archive out of the remaining modules.
	cd object; make -e -f ../$(MACHINE)/makefile archive \
		AR=$(AR) STRIP=$(STRIP) LORDER=$(LORDER) PROF=$(PROF)
	-rm -rf object
	#
	$(DONE)

move:
	#
	# move the library or libraries into the correct directory
	for i in *crt0.o ; \
		do rm -f $(LIB)/$(VARIANT)$$i ; \
		cp $$i $(LIB)/$(VARIANT)$$i ; \
		chmod $(PROT) $(LIB)/$(VARIANT)$$i ; \
		chmod -x $(LIB)/$(VARIANT)$$i ; \
		chgrp $(GRP) $(LIB)/$(VARIANT)$$i ; \
		chown $(OWN) $(LIB)/$(VARIANT)$$i ; \
		rm -f $$i ; \
		done

	rm -f $(LIB)/lib$(VARIANT)c.a
	cp lib.libc $(LIB)/lib$(VARIANT)c.a
	chmod $(PROT) $(LIB)/lib$(VARIANT)c.a
	chmod -x $(LIB)/lib$(VARIANT)c.a	# not executable
	chgrp $(GRP) $(LIB)/lib$(VARIANT)c.a
	chown $(OWN) $(LIB)/lib$(VARIANT)c.a

	$(PROF)if test -d $(LIBP) ; \
		then : ; else mkdir $(LIBP) ; fi 
	$(PROF)rm -f $(LIBP)/lib$(VARIANT)c.a
	$(PROF)cp libp.libc $(LIBP)/lib$(VARIANT)c.a
	$(PROF)chmod $(PROT) $(LIBP)/lib$(VARIANT)c.a
	$(PROF)chmod -x $(LIBP)/lib$(VARIANT)c.a	# not executable
	$(PROF)chgrp $(GRP) $(LIBP)/lib$(VARIANT)c.a
	$(PROF)chown $(OWN) $(LIBP)/lib$(VARIANT)c.a

install: all save move

#--------------------------

uninstall:	$(LIB)/OLDlib$(VARIANT)c.a $(LIBP)/OLDlib$(VARIANT)c.a \
		$(LIB)/OLD$(VARIANT)mcrt0.o $(LIB)/OLD$(VARIANT)crt0.o
		
		mv -f $(LIB)/OLDlib$(VARIANT)c.a $(LIB)/lib$(VARIANT)c.a
		mv -f $(LIBP)/OLDlib$(VARIANT)c.a $(LIBP)/lib$(VARIANT)c.a
		mv -f $(LIB)/OLD$(VARIANT)mcrt0.o $(LIB)/$(VARIANT)mcrt0.o
		mv -f $(LIB)/OLD$(VARIANT)crt0.o $(LIB)/$(VARIANT)crt0.o

#--------------------------

save:		
	if test -f $(LIB)/lib$(VARIANT)c.a ; \
	then \
		rm -f $(LIB)/OLDlib$(VARIANT)c.a ; \
		cp $(LIB)/lib$(VARIANT)c.a $(LIB)/OLDlib$(VARIANT)c.a ; \
		chmod $(PROT) $(LIB)/OLDlib$(VARIANT)c.a ; \
		chmod -x $(LIB)/OLDlib$(VARIANT)c.a ; \
		chgrp $(GRP) $(LIB)/OLDlib$(VARIANT)c.a ; \
		chown $(OWN) $(LIB)/OLDlib$(VARIANT)c.a ; \
	fi

	$(PROF)if test -f $(LIBP)/lib$(VARIANT)c.a ; \
	then \
		rm -f $(LIBP)/OLDlib$(VARIANT)c.a ; \
		cp $(LIBP)/lib$(VARIANT)c.a $(LIBP)/OLDlib$(VARIANT)c.a ; \
		chmod $(PROT) $(LIBP)/OLDlib$(VARIANT)c.a ; \
		chmod -x $(LIBP)/OLDlib$(VARIANT)c.a ; \
		chgrp $(GRP) $(LIBP)/OLDlib$(VARIANT)c.a ; \
		chown $(OWN) $(LIBP)/OLDlib$(VARIANT)c.a ; \
	fi

	for i in *crt0.o ; \
		do \
		    if test -f $(LIB)/$(VARIANT)$$i ; \
		    then \
			    rm -f $(LIB)/OLD$(VARIANT)$$i ; \
			    cp $(LIB)/$(VARIANT)$$i $(LIB)/OLD$(VARIANT)$$i ; \
			    chmod $(PROT) $(LIB)/OLD$(VARIANT)$$i ; \
			    chmod -x $(LIB)/OLD$(VARIANT)$$i ; \
			    chgrp $(GRP) $(LIB)/OLD$(VARIANT)$$i ; \
			    chown $(OWN) $(LIB)/OLD$(VARIANT)$$i ; \
		    fi ; \
		done

clean:
	#
	# remove intermediate files except object modules and temp library
	-rm -rf lib*.contents obj*
	cd port ;  make clean
	if m68k ; then cd m68k ; make clean ; fi
	if pdp11 ; then cd pdp11 ; make clean ; fi
	if vax ;   then cd vax ;   make clean ; fi
	if u3b ;   then cd u3b ;   make clean ; fi
	if u370 ;  then cd u370 ;  make clean ; fi

clobber:
	#
	# remove intermediate files
	-rm -rf *.o lib*.libc lib*.contents obj*
	cd port ;  make clobber
	if m68k ; then cd m68k ; make clobber ; fi
	if pdp11 ; then cd pdp11 ; make clobber ; fi
	if vax ;   then cd vax ;   make clobber ; fi
	if u3b ;   then cd u3b ;   make clobber ; fi
	if u370 ;  then cd u370 ;  make clobber ; fi
