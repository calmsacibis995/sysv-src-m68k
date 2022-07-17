#	@(#)cc.mk	2.1.1.1	
#
#	This makefile checks which machine (currently vax or pdp11)
#	it is used on, and builds a C compiler by wandering down to
#	the assumed directories, and invoking the makefiles for each
#	piece.  For the pdp11, it builds both cc and pcc.
#
#	The directory structure (global multi-machine view) assumed is:
#
#			       /cpp --- cpp.mk
#			       /
#		              /    /c2 --- c2.mk	(only for vax)
#			     /     /
#			    /     /
#			   /     / /optim --- optim.mk  (only for m68k)
#			  /     /  /
#			 /     /  /  /cc --- cc.mk  (Ritchie's pdp11 C compiler)
#			/     /  /   /
#		       /     /  /   /    /--cc.c
#		      /	    /  /   / /m68k
#		     /	   /  /   /  /
#		    /	  /  /   /  /
#	/usr/src/cmd --- /cc ------- cc.mk	(this makefile)
#			   \  \   \
#			    \  \   \   /cc.c
#			     \  \   \  /
#			      \  \  /vax
#			       \  \       /common
#			        \  \      /
#			         \  \    /
#			          \  \  /
#			           \ /pcc2
#			            \   \
#				     \   \
#				      \  /m68k --- makefile
#				       \
#			                \       /mip
#			                 \      /
#			                  \    /   /vax --- pcc.mk
#			                   \  /    /
#				          /pcc ------- pcc.mk
#				              \       
#				               \
#				              /pdp11 --- pcc.mk
#
OWN = bin
GRP = bin
PROT = 0775

TESTDIR = .
FRC =
INSDIR =
CFLAGS = -O
IFLAG = -i
LDFLAGS = -n -s
FFLAG =
YACCRM=-rm

all:
	if pdp11; \
	then \
		cd cc; \
		$(MAKE) -$(MAKEFLAGS) -f cc.mk CFLAGS="$(CFLAGS)" \
			LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
			TESTDIR=$(TESTDIR) all; \
	fi
	if vax; \
	then \
		cd pcc; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk CFLAGS="$(CFLAGS)" \
			IFLAG=$(IFLAG) LDFLAGS="$(LDFLAGS)" \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) all ; \
	fi
	if m68k; \
	then \
		cd pcc2/m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) build ; \
		cd ../../m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) build ; \
		cd ../optim/m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) build ; \
	fi
	-cd ../cpp; \
	$(MAKE) -$(MAKEFLAGS) -f cpp.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" TESTDIR=$(TESTDIR)

install:
	if pdp11; \
	then \
		cd cc; \
		$(MAKE) -$(MAKEFLAGS) -f cc.mk CFLAGS="$(CFLAGS)" \
			LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
			TESTDIR=$(TESTDIR) install; \
	fi
	if vax || pdp11; \
	then \
		cd pcc; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk CFLAGS="$(CFLAGS)" \
			IFLAG=$(IFLAG) LDFLAGS="$(LDFLAGS)" \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
	fi
	if m68k; \
	then \
		cd pcc2/m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
		cd ../../m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
		cd ../optim/m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
	fi
	cd ../cpp; \
	$(MAKE) -$(MAKEFLAGS) -f cpp.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" TESTDIR=$(TESTDIR) install

justinstall:
	#
	# justinstall acts just like install, except that in pcc.mk,
	# the C compiler interface (cc or pcc) is assumed to be built
	# and up to date - this is only necessary when a change in the
	# functionality of the whole compilation package forces installs
	# to only do installs, and no compiles, etc.
	#
	-if pdp11; \
	then \
		cd cc; \
		$(MAKE) -$(MAKEFLAGS) -f cc.mk CFLAGS="$(CFLAGS)" \
			LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
			TESTDIR=$(TESTDIR) install; \
	fi
	if vax || pdp11 ; \
	then \
		cd pcc; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk CFLAGS="$(CFLAGS)" \
			IFLAG=$(IFLAG) LDFLAGS="$(LDFLAGS)" \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) justinstall ; \
	fi
	if m68k; \
	then \
		cd pcc2/m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
		cd ../../m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
		cd ../optim/m68k; \
		$(MAKE) -$(MAKEFLAGS) CFLAGS="$(CFLAGS)" \
			OWN="$(OWN)" GRP="$(GRP)" PROT="$(PROT)" \
			IFLAG=$(IFLAG) \
			TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
	fi
	cd ../cpp; \
	$(MAKE) -$(MAKEFLAGS) -f cpp.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" TESTDIR=$(TESTDIR) install

clean:
	if pdp11; \
	then \
		cd cc; \
		$(MAKE) -$(MAKEFLAGS) -f cc.mk YACCRM=$(YACCRM) \
			TESTDIR=$(TESTDIR) clean; \
	fi
	if vax || pdp11 ; \
	then \
		cd pcc; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) \
			YACCRM=$(YACCRM) clean ; \
	fi
	if m68k; \
	then \
		cd pcc2/m68k; \
		$(MAKE) -$(MAKEFLAGS) \
			TESTDIR=$(TESTDIR) clean ; \
		cd ../../m68k; \
		$(MAKE) -$(MAKEFLAGS) \
			TESTDIR=$(TESTDIR) clean ; \
		cd ../optim/m68k; \
		$(MAKE) -$(MAKEFLAGS) \
			TESTDIR=$(TESTDIR) clean ; \
	fi
	cd ../cpp; \
	$(MAKE) -$(MAKEFLAGS) -f cpp.mk TESTDIR=$(TESTDIR) clean

clobber:
	if pdp11; \
	then \
		cd cc; \
		$(MAKE) -$(MAKEFLAGS) -f cc.mk YACCRM=$(YACCRM) \
			TESTDIR=$(TESTDIR) clobber; \
	fi
	if vax || pdp11 ; \
	then \
		cd pcc; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) \
			YACCRM=$(YACCRM) clobber ; \
	fi
	if m68k; \
	then \
		cd pcc2/m68k; \
		$(MAKE) -$(MAKEFLAGS) \
			TESTDIR=$(TESTDIR) clobber ; \
		cd ../../m68k; \
		$(MAKE) -$(MAKEFLAGS) \
			TESTDIR=$(TESTDIR) clobber ; \
		cd ../optim/m68k; \
		$(MAKE) -$(MAKEFLAGS) \
			TESTDIR=$(TESTDIR) clobber ; \
	fi
	cd ../cpp; \
	$(MAKE) -$(MAKEFLAGS) -f cpp.mk TESTDIR=$(TESTDIR) clobber
