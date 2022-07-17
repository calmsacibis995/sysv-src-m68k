# @(#)os.mk	2.25.1.3 
# @(#)os.mk	3.2 
LIBNAME = ../lib1
ROOT = /
INCRT = $(ROOT)usr/include
MACH = M68000
TYPE = MEXOR
CFLAGS = -O -T -D$(MACH) -D$(TYPE)
FRC =
FILES =\
	$(LIBNAME)(main.o)\
	$(LIBNAME)(trap.o)\
	$(LIBNAME)(sysent.o)\
	$(LIBNAME)(sys1.o)\
	$(LIBNAME)(sysm68k.o)\
	$(LIBNAME)(sys2.o)\
	$(LIBNAME)(sys3.o)\
	$(LIBNAME)(sys4.o)\
	$(LIBNAME)(acct.o)\
	$(LIBNAME)(alloc.o)\
	$(LIBNAME)(clock.o)\
	$(LIBNAME)(errlog.o)\
	$(LIBNAME)(fio.o)\
	$(LIBNAME)(iget.o)\
	$(LIBNAME)(lock.o)\
	$(LIBNAME)(machdep.o)\
	$(LIBNAME)(malloc.o)\
	$(LIBNAME)(msg.o)\
	$(LIBNAME)(nami.o)\
	$(LIBNAME)(pipe.o)\
	$(LIBNAME)(prf.o)\
	$(LIBNAME)(rdwri.o)\
	$(LIBNAME)(sem.o)\
	$(LIBNAME)(shm.o)\
	$(LIBNAME)(ipc.o)\
	$(LIBNAME)(sig.o)\
	$(LIBNAME)(slp.o)\
	$(LIBNAME)(subr.o)\
	$(LIBNAME)(text.o)\
	$(LIBNAME)(user.o)\
	$(LIBNAME)(utssys.o)

all:		$(LIBNAME)

.PRECIOUS:	$(LIBNAME)

$(LIBNAME):	$(FILES)

.c.a:
	M68000= $(CC) -c $(CFLAGS) $<
	chmod 664 $*.o
	chgrp bin $*.o
	chown bin $*.o
	ar rv $@ $*.o
	-rm -f $*.o

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)

$(LIBNAME)(acct.o): acct.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/inode.h\
	$(FRC)
$(LIBNAME)(alloc.o): alloc.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/fblk.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/ino.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(clock.o): clock.c \
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/callo.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(errlog.o): errlog.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/mba.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/erec.h\
	$(INCRT)/sys/err.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(fio.o): fio.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(iget.o): iget.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/ino.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(ipc.o): ipc.c\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/ipc.h\
	$(FRC)
$(LIBNAME)(lock.o): lock.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/lock.h\
	$(FRC)
$(LIBNAME)(machdep.o): machdep.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/clock.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(macherr.o): macherr.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(main.o): main.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/clock.h\
	$(FRC)
$(LIBNAME)(malloc.o): malloc.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/map.h\
	$(FRC)
$(LIBNAME)(msg.o): msg.c\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/ipc.h\
	$(INCRT)/sys/msg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/mmu.h
$(LIBNAME)(nami.o): nami.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(pipe.o): pipe.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/file.h\
	$(FRC)
$(LIBNAME)(prf.o): prf.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/iobuf.h\
	$(FRC)
$(LIBNAME)(rdwri.o): rdwri.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(sem.o): sem.c\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/ipc.h\
	$(INCRT)/sys/sem.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/mmu.h
$(LIBNAME)(shm.o): shm.c\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/ipc.h\
	$(INCRT)/sys/shm.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/page.h\
	$(INCRT)/sys/mtpr.h\
	$(INCRT)/sys/systm.h
$(LIBNAME)(sig.o): sig.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/psl.h\
	$(FRC)
$(LIBNAME)(slp.o): slp.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(subr.o): subr.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(FRC)
$(LIBNAME)(sys1.o): sys1.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(sysm68k.o): sysm68k.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/reg.h\
	$(FRC)
$(LIBNAME)(sys2.o): sys2.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(sys3.o): sys3.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/ino.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/stat.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(sys4.o): sys4.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/clock.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(sysent.o): sysent.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(FRC)
$(LIBNAME)(text.o): text.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(trap.o): trap.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/trap.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(utssys.o): utssys.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/var.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(user.o): user.c\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
FRC:


