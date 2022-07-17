#	@(#)libPW.mk	2.1.1.1	
OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	664		#protections

ROOT = 
LIB = $(ROOT)/lib
LNAME = libPW.a

OBJ1 = abspath.o anystr.o bal.o curdir.o fdfopen.o giveup.o
OBJ2 = imatch.o index.o lockit.o logname.o move.o patoi.o
OBJ3 = patol.o regcmp.o regex.o rename.o repeat.o repl.o satoi.o
OBJ4 = setsig.o sname.o strend.o substr.o trnslat.o userdir.o
OBJ5 = username.o verify.o any.o xalloc.o xcreat.o xlink.o
OBJ6 = xopen.o xpipe.o xunlink.o xwrite.o xmsg.o alloca.o
OBJ7 = cat.o dname.o fatal.o clean.o userexit.o zero.o zeropad.o

init:
	@-if m68k;\
	then\
		cp m68k/move.c move.c;\
		cp m68k/alloca.s alloca.s;\
	elif vax;\
	then\
		cp vax/move.s move.s;\
		cp vax/alloca.s alloca.s;\
	elif u370;\
	then\
		cp u370/move.c move.c;\
		cp u370/alloca.c alloca.c;\
	elif u3b;\
	then\
		cp u3b/move.c move.c;\
		cp u3b/alloca.s alloca.s;\
	else\
		cp pdp11/move.c move.c;\
		cp pdp11/alloca.s alloca.s;\
	fi

all:		$(LNAME)

$(LNAME):	init $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7)
	-rm -f $(LNAME)
	chmod $(PROT) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7)
	chgrp $(GRP)  $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7)
	chown $(OWN)  $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7)
	ar r $(LNAME) $(OBJ1)
	ar r $(LNAME) $(OBJ2)
	ar r $(LNAME) $(OBJ3)
	ar r $(LNAME) $(OBJ4)
	ar r $(LNAME) $(OBJ5)
	ar r $(LNAME) $(OBJ6)
	ar r $(LNAME) $(OBJ7)
	-if pdp11 ; \
		then strip $(LNAME) ; \
		elif m68k ; \
		then : ; \
		else strip -r $(LNAME) ; \
		     ar ts $(LNAME) ; \
	fi

clean:
	-rm -f $(OBJ1)
	-rm -f $(OBJ2)
	-rm -f $(OBJ3)
	-rm -f $(OBJ4)
	-rm -f $(OBJ5)
	-rm -f $(OBJ6)
	-rm -f $(OBJ7)
	-rm -f move.c move.s alloca.c alloca.s $(LNAME)

clobber:	clean

move:
	rm -f $(LIB)/$(LNAME)
	cp $(LNAME) $(LIB)/$(LNAME)
	chmod $(PROT) $(LIB)/$(LNAME)
	chmod -x $(LIB)/$(LNAME)	# not executable
	chgrp $(GRP) $(LIB)/$(LNAME)
	chown $(OWN) $(LIB)/$(LNAME)

install: all save move

#--------------------------

uninstall:	$(LIB)/OLD$(LNAME)
		
		mv -f $(LIB)/OLD$(LNAME) $(LIB)/$(LNAME)

#--------------------------

save:		
	if test -f $(LIB)/$(LNAME) ; \
	then \
		rm -f $(LIB)/OLD$(LNAME)  ; \
		cp $(LIB)/$(LNAME) $(LIB)/OLD$(LNAME) ; \
		chmod $(PROT) $(LIB)/OLD$(LNAME) ; \
		chmod -x $(LIB)/OLD$(LNAME) ; \
		chgrp $(GRP) $(LIB)/OLD$(LNAME) ; \
		chown $(OWN) $(LIB)/OLD$(LNAME) ; \
	fi
