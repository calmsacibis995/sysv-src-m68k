#	@(#)makefile	2.1.1.1 
#
# makefile for libc/m68k
#
#
# The variable PROF is null by default, causing profiled object to be
# maintained.  If profiled object is not desired, the reassignment
# PROF=@# should appear in the make command line.
#

INCROOT =

.SUFFIXES: .p

M4=m4 m4.def
CC=cc
CFLAGS=-O
PROF=
PROT=664
GRP=bin
OWN=bin

# a makefile exists in crt and csu for m68k
#

OBJECTS=\
gen/abs.o         gen/cuexit.o      gen/setjmp.o	    \
gen/memccpy.o     gen/memchr.o      gen/memcmp.o      gen/memcpy.o      \
gen/memset.o \
gen/strcat.o      gen/strchr.o      gen/strcmp.o      gen/strcpy.o      \
gen/strcspn.o     gen/strlen.o      gen/strncat.o     gen/strncmp.o     \
gen/strncpy.o     gen/strpbrk.o     gen/strrchr.o     gen/strspn.o      \
sys/access.o      sys/acct.o        sys/alarm.o       sys/chdir.o	\
sys/chmod.o       sys/chown.o       sys/chroot.o      sys/close.o	\
sys/creat.o       sys/dup.o         \
sys/execve.o      sys/exit.o        sys/fcntl.o	\
sys/fork.o        sys/fstat.o       sys/getegid.o     sys/geteuid.o	\
sys/getgid.o      sys/getpid.o      sys/getppid.o     sys/getuid.o	\
sys/gtty.o        sys/ioctl.o       sys/kill.o        sys/link.o	\
sys/lseek.o       sys/mknod.o       sys/mount.o       \
sys/nice.o        sys/open.o        sys/pause.o       sys/pipe.o	\
sys/plock.o       sys/profil.o      sys/ptrace.o      sys/read.o	\
sys/sbrk.o        sys/setgid.o      sys/setpgrp.o     sys/setuid.o	\
sys/signal.o      sys/stat.o        sys/stime.o	      \
sys/stty.o        sys/sync.o        sys/syscall.o     sys/time.o	\
sys/_sysm68k.o    \
sys/times.o       sys/ulimit.o      sys/umask.o       sys/umount.o	\
sys/uname.o       sys/unlink.o      sys/ustat.o       sys/utime.o	\
sys/wait.o        sys/write.o

POBJECTS=\
gen/abs.p         gen/cuexit.p      gen/setjmp.p	\
gen/memccpy.p     gen/memchr.p      gen/memcmp.p      gen/memcpy.p      \
gen/memset.p \
gen/strcat.p      gen/strchr.p      gen/strcmp.p      gen/strcpy.p      \
gen/strcspn.p     gen/strlen.p      gen/strncat.p     gen/strncmp.p     \
gen/strncpy.p     gen/strpbrk.p     gen/strrchr.p     gen/strspn.p      \
sys/access.p      sys/acct.p        sys/alarm.p       sys/chdir.p	\
sys/chmod.p       sys/chown.p       sys/chroot.p      sys/close.p	\
sys/creat.p       sys/dup.p         \
sys/execve.p      sys/exit.p        sys/fcntl.p	\
sys/fork.p        sys/fstat.p       sys/getegid.p     sys/geteuid.p	\
sys/getgid.p      sys/getpid.p      sys/getppid.p     sys/getuid.p	\
sys/gtty.p        sys/ioctl.p       sys/kill.p        sys/link.p	\
sys/lseek.p       sys/mknod.p       sys/mount.p       \
sys/nice.p        sys/open.p        sys/pause.p       sys/pipe.p	\
sys/plock.p       sys/profil.p      sys/ptrace.p      sys/read.p	\
sys/sbrk.p        sys/setgid.p      sys/setpgrp.p     sys/setuid.p	\
sys/signal.p      sys/stat.p        sys/stime.p	      \
sys/stty.p        sys/sync.p        sys/syscall.p     sys/time.p	\
sys/_sysm68k.p    \
sys/times.p       sys/ulimit.p      sys/umask.p       sys/umount.p	\
sys/uname.p       sys/unlink.p      sys/ustat.p       sys/utime.p	\
sys/wait.p        sys/write.p


all:
#
# compile crt runtime routines, a makefile exists in the directory
#
	cd crt ; make -e $(IGN) nonprof INCROOT=$(INCROOT) 
	$(PROF)cd crt ; make -e $(IGN) prof NONPROF=@#
#
# compile csu start up routines, a makefile exists in the directory
#
	cd csu ; make -e $(IGN) nonprof
	$(PROF)cd csu ; make -e $(IGN) prof 
#
# compile gen and sys directories
#
	make -e $(IGN) nonprof
	$(PROF)make -e $(IGN) prof NONPROF=@#
	
nonprof: $(OBJECTS) gen/sysm68k.o

prof: $(POBJECTS) gen/sysm68k.p

.c.o .c.p:
	@echo $*.c:
	$(NONPROF)$(CC) $(CFLAGS) -c $*.c  && mv $(*F).o $*.o
	$(PROF)$(CC) -p $(CFLAGS) -c $*.c  && mv $(*F).o $*.p

.s.o .s.p:
	@echo $*.s:
	$(NONPROF)$(M4) -DMCOUNT=#  $*.s   > $(*F).s   && \
	$(NONPROF)$(CC) $(CFLAGS) -c $(*F).s  && mv $(*F).o $*.o
	$(PROF)$(M4) mcount.def  $*.s   > $(*F).s   && \
	$(PROF)$(CC) $(CFLAGS) -c $(*F).s  && mv $(*F).o $*.p
	-rm $(*F).s

gen/sysm68k.o gen/sysm68k.p:
	@echo $*.c:
	$(NONPROF)M68000= $(CC) $(CFLAGS) -c $*.c  && mv $(*F).o $*.o
	$(PROF)M68000= $(CC) -p $(CFLAGS) -c $*.c  && mv $(*F).o $*.p
	
archive:
	#
	# Note that "archive" is invoked with libc/object as current directory.
	#
	# figure out the correct ordering for all the archive modules except
	#	cuexit and fakcu, which must go at end of archive.
	chmod $(PROT) *.o 
	chgrp $(GRP)  *.o
	chown $(OWN)  *.o
	mv cuexit.o cuexit.x
	mv fakcu.o fakcu.x
	$(LORDER) *.o | tsort >objlist
	mv cuexit.x cuexit.o
	mv fakcu.x fakcu.o
	#
	# build the archive with the modules in correct order.
	$(AR) q ../lib.libc `cat objlist` cuexit.o fakcu.o
	$(AR) t ../lib.libc >../lib.contents
	rm -f *.o
	$(PROF)#
	$(PROF)# build the profiled library archive, first renaming the
	$(PROF)#	.p (profiled object) modules to .o
	$(PROF)for i in *.p ; do mv $$i `basename $$i .p`.o ; done
	$(PROF)chmod $(PROT) *.o
	$(PROF)chgrp $(GRP)  *.o
	$(PROF)chown $(OWN)  *.o
	$(PROF)$(AR) q ../libp.libc `cat objlist` cuexit.o fakcu.o
	$(PROF)$(STRIP) -r ../libp.libc
	$(PROF)$(AR) ts ../libp.libc >../libp.contents

clean:
	-rm -f *.[ops]

clobber: clean
	-rm -f */*.[op]
