# @(#)makefile	2.1.1.1	
#
# Ex skeletal makefile for USG Unix 4.0 on a VAX.
#
# NB: This makefile doesn't indicate any dependencies on header files.
#
# Ex is very large - this version will not fit on PDP-11's without overlay
# software.  Things that can be turned off to save
# space include LISPCODE (-l flag, showmatch and lisp options), UCVISUAL
# (visual \ nonsense on upper case only terminals), CHDIR (the undocumented
# chdir command.)  CRYPT includes the code to edit encrypted files (the -x
# option, like ed.)  VMUNIX makes ex considerably larger, raising many limits
# and improving speed and simplicity of maintenance.  It is suitable only
# for a VAX or other large machine, and then probably only in a paged system.
#
# Don't define VFORK unless your system has the VFORK system call,
# which is like fork but the two processes have only one data space until the
# child execs. This speeds up ex by saving the memory copy.
#
# If your system expands tabs to 4 spaces you should -DTABS=4 below
#
VERSION=3.7
OL =
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
INSU = /etc/install -f $(UBIN)
INSUL = /etc/install -f $(ULIB)
TESTDIR = .
LBIN = ../misc
FOLD = $(LBIN)/fold
MKSTR = $(LBIN)/mkstr
XSTR = $(LBIN)/xstr
CTAGS = $(LBIN)/ctags
DEBUGFLAGS = -DTRACE -g
NONDEBUGFLAGS = -O
DEB = $(NONDEBUGFLAGS)	# or $(DEBUGFLAGS) to debug
OPTIONS = -DLISPCODE -DCHDIR -DUCVISUAL -DUSG3TTY -DSTDIO
CFLAGS =  -DTABS=8 $(OPTIONS) $(DEB)
LDFLAGS = -s # -n # or -i or -z
TERMLIB = ../termlib/termcap.a # -ltermcap

OBJS = ex.o ex_addr.o ex_cmds.o ex_cmds2.o ex_cmdsub.o \
	ex_data.o ex_extern.o ex_get.o ex_io.o ex_put.o ex_re.o \
	ex_set.o ex_subr.o ex_temp.o ex_tty.o ex_unix.o \
	ex_v.o ex_vadj.o ex_vget.o ex_vmain.o ex_voper.o \
	ex_vops.o ex_vops2.o ex_vops3.o ex_vput.o ex_vwind.o \
	printf.o bcopy.o strings.o       # "strings.o" not same as in ../misc

HDRS = ex.h ex_argv.h ex_re.h ex_temp.h ex_tty.h ex_tune.h ex_vars.h ex_vis.h

SRC1 = ex.c ex_addr.c ex_cmds.c ex_cmds2.c ex_cmdsub.c
SRC2 = ex_data.c ex_extern.o ex_get.c ex_io.c ex_put.c ex_re.c
SRC3 = ex_set.c ex_subr.c ex_temp.c ex_tty.c ex_unix.c
SRC4 = ex_v.c ex_vadj.c ex_vget.c ex_vmain.c ex_voper.c
SRC5 = ex_vops.c ex_vops2.c ex_vops3.c ex_vput.c ex_vwind.c
SRC6 = printf.c bcopy.c expreserve.c exrecover.c

compile all: ex ex$(VERSION)recover ex$(VERSION)preserve

ex: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(TERMLIB) -o $(TESTDIR)/ex

.c.o:
	$(MKSTR) - ex$(VERSION)strings x $*.c
	$(CC) -E $(CFLAGS) x$*.c | $(XSTR) -c -
	$(CC) $(CFLAGS) -c x.c 
	mv x.o $*.o
	rm -f x*.c

$(OBJS): ex_vars.h

# ex_vars.h:
# 	csh makeoptions $(CFLAGS) # unused for m68k - FGC, 9/27/83

bcopy.o:	bcopy.c
	$(CC) -c $(CFLAGS) bcopy.c

# xstr: hands off!
strings.o: strings rofix
	$(XSTR)
	$(CC) -c -S xs.c
	ed - <rofix xs.s
	$(AS) -o strings.o xs.s
	rm -f xs.[cs]
	
ex$(VERSION)recover: exrecover.o ex_extern.o
	$(CC) $(LDFLAGS) exrecover.o ex_extern.o -o $(TESTDIR)/ex$(VERSION)recover

exrecover.o: exrecover.c
	$(CC) $(CFLAGS) -c exrecover.c

ex$(VERSION)preserve: expreserve.o
	$(CC) $(LDFLAGS) expreserve.o -o $(TESTDIR)/ex$(VERSION)preserve

expreserve.o:
	$(CC) $(CFLAGS) -c expreserve.c

install: all
	rm -f $(UBIN)/ex $(UBIN)/vi $(UBIN)/view $(UBIN)/edit
	$(INSU) $(TESTDIR)/ex
	ln $(UBIN)/ex $(UBIN)/edit
	ln $(UBIN)/ex $(UBIN)/vi
	ln $(UBIN)/ex $(UBIN)/view
	rm -f $(ULIB)/ex$(VERSION)strings
	cp ex$(VERSION)strings $(ULIB)
	chgrp bin $(ULIB)/ex$(VERSION)strings
	chown bin $(ULIB)/ex$(VERSION)strings
	rm -f $(ULIB)/ex$(VERSION)recover $(ULIB)/ex$(VERSION)preserve
	$(INSUL) $(TESTDIR)/ex$(VERSION)recover
	$(INSUL) $(TESTDIR)/ex$(VERSION)preserve
	-mkdir $(OL)/usr/preserve # normally fails.  that is OK.

clean:
#	If we don't have ex we can't make it so don't rm ex_vars.h
	rm -f strings core errs trace
	rm -f *.o x*.[cs]

clobber:
	rm -f $(TESTDIR)/ex
	rm -f $(TESTDIR)/ex$(VERSION)preserve $(TESTDIR)/ex$(VERSION)recover

delete: clean clobber
	-rm -f *.[chs] rofix
#
# other targets
#
tags:
	$(CTAGS) -w ex.[hc] ex_*.[hc]

lint:
	lint $(CFLAGS) ex.c ex_?*.c
	lint $(CFLAGS) -u exrecover.c
	lint $(CFLAGS) expreserve.c

CXREF = cxref
PR = pr
INCLUDE = /usr/include

print:
	@$(PR) READ* BUGS
	@$(PR) makefile*
	@$(PR) /etc/termcap
	@(size -l ex ; size *.o) | $(PR) -h sizes
	@$(PR) -h errno.h $(INCLUDE)/errno.h
	@$(PR) -h setjmp.h $(INCLUDE)/setjmp.h
	@$(PR) -h sgtty.h $(INCLUDE)/sgtty.h
	@$(PR) -h signal.h $(INCLUDE)/signal.h
	@$(PR) -h sys/stat.h $(INCLUDE)/sys/stat.h
	@$(PR) -h sys/types.h $(INCLUDE)/sys/types.h
	@ls -ls | $(PR)
	@$(CXREF) *.c | $(PR) -h XREF
	@$(PR) *.h *.c
#
# this target uses stuff not in the VAX UNIX distribution from Bell
#
VGRIND = csh /usr/ucb/vgrind
VHDR = "Ex Version $(VERSION)"
MISC = makefile READ_ME rofix

vgrind:
	tee index < /dev/null
	$(VGRIND) -h $(VHDR) $(HDRS)
	$(VGRIND) -h $(VHDR) $(SRC1)
	$(VGRIND) -h $(VHDR) $(SRC2)
	$(VGRIND) -h $(VHDR) $(SRC3)
	$(VGRIND) -h $(VHDR) $(SRC4)
	$(VGRIND) -h $(VHDR) $(SRC5)
	$(VGRIND) -h $(VHDR) $(SRC6)
	$(VGRIND) -n -h $(VHDR) $(MISC)
	$(VGRIND) -i -h $(VHDR) index
