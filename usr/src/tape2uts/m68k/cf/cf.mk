# @(#)cf.mk	2.1	
SYS = unix
NODE = mot68
REL = sysV68
VER = 2.0
MACH = M68000
TYPE = MEXOR
OWN = bin
GRP = bin

ROOT = /
INCRT = $(ROOT)usr/include
INS = $(ROOT)etc/install
INSDIR = $(ROOT)
CFLAGS = -O -T -I$(INCRT)
LDFILE =
FRC =
NAME = $(SYS)$(VER)

all:	init ../$(NAME)
	@-chmod 664	conf.o m68kvec.o low.o linesw.o name.o
	@-chgrp $(GRP)	conf.c m68kvec.s low.s linesw.o name.o \
			conf.o m68kvec.o low.o linesw.o name.o
	@-chown $(OWN)	conf.c m68kvec.s low.s linesw.o name.o \
			conf.o m68kvec.o low.o linesw.o name.o

init:
	cd ..; make -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "MACH=$(MACH)"\
		"TYPE=$(TYPE)" "ROOT=$(ROOT)" "OWN=$(OWN)" "GRP=$(GRP)"
	@-$(CC) $(CFLAGS) -c \
		-DSYS=\"`expr $(SYS) : '\(.\{1,8\}\)'`\" \
		-DNODE=\"`expr $(NODE) : '\(.\{1,8\}\)'`\" \
		-DREL=\"`expr $(REL) : '\(.\{1,8\}\)'`\" \
		-DVER=\"`expr $(VER) : '\(.\{1,8\}\)'`\" \
		-DMACH=\"`expr $(MACH) : '\(.\{1,8\}\)'`\" \
		name.c

../$(NAME): $(LDFILE) m68kvec.o low.o ../locore.o conf.o linesw.o ../lib[0-9] name.o
	-ld -o ../$(NAME) $(LDFILE) \
		 m68kvec.o low.o ../locore.o conf.o linesw.o ../lib[0-9] name.o
	@-chmod 664 ../$(NAME)
	@-chgrp $(GRP) ../$(NAME)
	@-chown $(OWN) ../$(NAME)

clean:
	cd ..; make -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "MACH=$(MACH)"\
		"TYPE=$(TYPE)" "ROOT=$(ROOT)" "OWN=$(OWN)" "GRP=$(GRP)"\
		clean
	-rm -f *.o

clobber:	clean
	cd ..; make -f m68k.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "MACH=$(MACH)"\
		"TYPE=$(TYPE)" "ROOT=$(ROOT)" "OWN=$(OWN)" "GRP=$(GRP)"\
		clobber
	-rm -f ../$(NAME) m68kmec.s conf.c low.s

conf.c:\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/callo.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/err.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/io.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/ram.h\
	$(INCRT)/sys/space.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/var.h\
	$(FRC)

name.c:\
	$(INCRT)/sys/utsname.h\
	$(FRC)

m68kmec.s:\
	$(FRC)

low.s:\
	$(FRC)

linesw.c:\
	$(INCRT)/sys/conf.h\
	$(FRC)

install:	all
	$(INS) -f $(INSDIR) "../$(SYS)$(VER)"

FRC:
