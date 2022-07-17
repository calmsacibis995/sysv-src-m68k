#	@(#)Makefile	2.18.1.2	
SYS = unix
NODE = mot68
REL = sysV68
VER = 2.1
MACH = M68000
TYPE = MEXOR
OWN = bin
GRP = bin
DFOPTS =

ROOT = /
INCRT = $(ROOT)usr/include
INS = $(ROOT)etc/install
INSDIR = $(ROOT)
CFLAGS = -c -O -T -D$(MACH) -D$(TYPE) -I$(INCRT)
MFLAGS = -D$(MACH) -D$(TYPE)
LDFILE =
FRC =
NAME = $(SYS)$(VER)

.c.o:
	rm -f $*.o
	M68000= $(CC) -c $(CFLAGS) $<
	chmod 664 $*.o
	chgrp bin $*.o
	chown bin $*.o

all:	init ../$(NAME)

init:   
	cd ..; $(MAKE) -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" "CFLAGS=$(CFLAGS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "MACH=$(MACH)"\
		"TYPE=$(TYPE)" "ROOT=$(ROOT)" "OWN=$(OWN)" "GRP=$(GRP)"
	@-rm -f name.o
	@-M68000= $(CC) $(CFLAGS) \
		-DSYS=\"`expr $(SYS) : '\(.\{1,8\}\)'`\" \
		-DNODE=\"`expr $(NODE) : '\(.\{1,8\}\)'`\" \
		-DREL=\"`expr $(REL) : '\(.\{1,8\}\)'`\" \
		-DVER=\"`expr $(VER) : '\(.\{1,8\}\)'`\" \
		-DMACH=\"`expr $(MACH) : '\(.\{1,8\}\)'`\" \
		name.c
	@-chmod 664 name.o
	@-chgrp bin name.o
	@-chown bin name.o

../$(NAME): m68kvec.o low.o ../locore.o conf.o linesw.o ../lib[0-9] name.o
	-rm -f ../$(NAME)
	-ld -o ../$(NAME) $(LDFILE) m68kvec.o ../lib2 low.o ../locore.o \
		 conf.o linesw.o ../lib[0-9] name.o

clean:
	cd ..; $(MAKE) -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" "CFLAGS=$(CFLAGS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "MACH=$(MACH)"\
		"TYPE=$(TYPE)" "ROOT=$(ROOT)" "OWN=$(OWN)" "GRP=$(GRP)" clean
	-rm -f *.o

clobber:	clean
	cd ..; $(MAKE) -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" "CFLAGS=$(CFLAGS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "MACH=$(MACH)"\
		"TYPE=$(TYPE)" "ROOT=$(ROOT)" "OWN=$(OWN)" "GRP=$(GRP)" clobber
	-rm -f ../$(NAME) m68kvec.s low.s conf.c

master:	master.m4
	rm -f master
	m4 $(MFLAGS) master.m4 > master
	
dfile:	dfile.m4
	rm -f dfile
	m4 $(MFLAGS) $(DFOPTS) dfile.m4 > dfile
	
m68kvec.s low.s:	master dfile	
	rm -f m68kvec.s low.s conf.c
	config -m master dfile

conf.c: master\
	dfile\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/callo.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/io.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/ram.h\
	$(INCRT)/sys/space.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/var.h
	config -m master dfile

name.c:\
	$(INCRT)/sys/utsname.h\
	$(FRC)

linesw.c:\
	$(INCRT)/sys/conf.h\
	$(FRC)

install:	all
	$(INS) -f $(INSDIR) "../$(SYS)$(VER)"

FRC:
