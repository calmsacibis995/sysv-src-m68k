#	@(#)crash.mk	2.1	
OL=
TESTDIR = .
FRC =
INS = /etc/install
INSDIR = /etc
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)
CFLAGS = -O
LDFLAGS =  -s
CC = cc
SYS =
BASE = ..
M68INC = $(BASE)/headers

OFILES = buf.o callout.o file.o inode.o main.o $(SYS)misc.o mount.o proc.o\
		$(SYS)symtab.o stat.o text.o tty.o u.o sysvad.o

compile all:  stest 

stest:
	-if vax; then make -f crash.mk crash SYS=VAX; \
	 elif pdp11; then make -f crash.mk crash  SYS=PDP; \
	 elif m68k; then make -f crash.mk crash  SYS=M68K; fi

crash:	$(OFILES) 
	$(CC) $(LDFLAGS) -o $(TESTDIR)/crash $(OFILES)


$(OFILES):  cmd.h crash.h
	-if vax; then $(CC) $(CFLAGS) -c $*.c; \
	 elif pdp11; then $(CC) $(CFLAGS) -c $*.c;\
	 elif m68k; then $(CC) $(CFLAGS) -I$(M68INC) -c $*.c; fi

install: all
	$(INSE) $(TESTDIR)/crash

clean:
	-rm -f *.o

clobber:
	-rm -f $(TESTDIR)/crash

FRC:
