#	@(#)lint.mk	2.1	
OL =
TESTDIR = .
MIP = ../cc/pcc/mip
LINT = lint
LINT1 = lint1
LINT1V = lint1v
INS = /etc/install
TMPDIR = /usr/tmp
LDFLAGS = -s
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
INSUL = /etc/install -f $(ULIB)
INSUB = /etc/install -f $(UBIN)

#  -DBUG4 is necessary to turn off pcc debugging tools (these tools cannot
#   be used with the lint shell script since their output conflicts)
CFLAGS = -O -DLINT -I$(MIP) -I. -DBUG4
FRC =
MFILES = macdefs $(MIP)/manifest $(MIP)/mfile1

#object files for the first and second passes
OFILES1 = trees.o pftn.o scan.o comm1.o messages.o optim.o xdefs.o cgram.o\
	lerror.o msgbuf.o lint.o
OFILES2 = lpass2.o lerror2.o messages.o msgbuf2.o

compile all:	lint1 lint2 llib-lc.ln llib-port.ln llib-lm.ln

#makes for the first pass of lint
lint1:	$(OFILES1) $(FRC)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/lint1 $(OFILES1)

lint1v:	$(OFILES1) $(FRC)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/lint1v $(OFILES1)

trees.o:	$(MIP)/messages.h $(MFILES) $(MIP)/trees.c
			$(CC) $(CFLAGS) -c  $(MIP)/trees.c
pftn.o:		$(MIP)/messages.h $(MFILES) $(MIP)/pftn.c
			$(CC) $(CFLAGS) -c  $(MIP)/pftn.c
scan.o:		$(MIP)/messages.h $(MFILES) $(MIP)/scan.c
			$(CC) $(CFLAGS) -c  $(MIP)/scan.c
comm1.o:	$(MIP)/common $(MFILES)     $(MIP)/comm1.c
			$(CC) $(CFLAGS) -c  $(MIP)/comm1.c
messages.o:	$(MIP)/messages.h           $(MIP)/messages.c
			$(CC) $(CFLAGS) -c  $(MIP)/messages.c
optim.o:	$(MFILES)                   $(MIP)/optim.c
			$(CC) $(CFLAGS) -c  $(MIP)/optim.c
xdefs.o:	$(MFILES)                   $(MIP)/xdefs.c
			$(CC) $(CFLAGS) -c  $(MIP)/xdefs.c
cgram.o:	$(MIP)/messages.h $(MFILES) 
cgram.c:	$(MIP)/cgram.y
		yacc $(MIP)/cgram.y ; mv y.tab.c cgram.c
lerror.o:	lerror.h $(MIP)/messages.h 
msgbuf.o:	lerror.h $(MIP)/messages.h
lint.o:		lerror.h lmanifest $(MIP)/messages.h $(MFILES)

#makes for the lint libraries
llib-lc.ln:  llib-lc $(LINT1) $(FRC)
	$(CC) -E -C -Dlint llib-lc | $(LINT1) -vx -H$(TMPDIR)/hlint >llib-lc.ln 
	rm $(TMPDIR)/hlint
llib-port.ln:  llib-port $(LINT1) $(FRC)
	$(CC) -E -C -Dlint llib-port | $(LINT1) -vxp -H$(TMPDIR)/hlint >llib-port.ln
	rm $(TMPDIR)/hlint 
llib-lm.ln:  llib-lm $(LINT1) $(FRC)
	$(CC) -E -C -Dlint llib-lm | $(LINT1) -vx -H$(TMPDIR)/hlint >llib-lm.ln
	rm $(TMPDIR)/hlint 

#makes for the second pass of lint
lint2:	$(OFILES2) $(FRC)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/lint2 $(OFILES2)

msgbuf2.o:	lerror.h 
lpass2.o:	lerror.h lmanifest lpass2.h $(MIP)/manifest
lerror2.o:	lerror.h lmanifest lpass2.h $(MIP)/manifest $(MIP)/messages.h

#miscellaneous utilities

# to reinstall libraries after clobber has been used, try :
#	make inslibs LINT1=/usr/lib/lint1
#  (this uses the installed version of lint pass 1)
inslibs: llib-lc.ln llib-port.ln llib-lm.ln
	$(INSUL) $(TESTDIR)/llib-lc
	$(INSUL) $(TESTDIR)/llib-lc.ln
	$(INSUL) $(TESTDIR)/llib-port
	$(INSUL) $(TESTDIR)/llib-port.ln
	$(INSUL) $(TESTDIR)/llib-lm
	$(INSUL) $(TESTDIR)/llib-lm.ln
install: all inslibs
	$(INSUL) $(TESTDIR)/lint1
	$(INSUL) $(TESTDIR)/lint2
	cp $(TESTDIR)/lint.sh $(TESTDIR)/lint
	chmod 0755 $(TESTDIR)/lint
	$(INSUB) $(TESTDIR)/lint

lintall:
	$(LINT1) -DLINT  -I. -I$(MIP)  $(MIP)/xdefs.c $(MIP)/scan.c \
	$(MIP)/pftn.c $(MIP)/trees.c $(MIP)/optim.c lint.c $(MIP)/messages.c \
	$(MIP)/comm1.c msgbuf.c lerror.c -b cgram.c 
	$(LINT1) -DLINT  -I$(MIP) -I. lpass2.c lerror2.c msgbuf2.c \
	$(MIP)/messages.c
clean:
	rm -f *.o cgram.c
clobber: 
	rm -f $(TESTDIR)/lint $(TESTDIR)/lint1 $(TESTDIR)/lint2 \
	$(TESTDIR)/llib-lc.ln $(TESTDIR)/llib-port.ln $(TESTDIR)/llib-lm.ln
FRC:
