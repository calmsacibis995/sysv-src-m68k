# @(#)io.mk	2.26.1.2 
LIBNAME = ../lib2
ROOT = /
INCRT = $(ROOT)usr/include
MACH = M68000
TYPE = MEXOR
CFLAGS =  -c -O -T -D$(MACH) -D$(TYPE)

FRC =

FILES =\
	$(LIBNAME)(bio.o)\
	$(LIBNAME)(mmuEXOR1.o)\
	$(LIBNAME)(mmuEXOR2.o)\
	$(LIBNAME)(mmu451a.o)\
	$(LIBNAME)(mmu451b.o)\
	$(LIBNAME)(ram.o)\
	$(LIBNAME)(tt0.o)\
	$(LIBNAME)(tty.o)\
	$(LIBNAME)(clist.o)\
	$(LIBNAME)(clk.o)\
	$(LIBNAME)(clk6818.o)\
	$(LIBNAME)(err.o)\
	$(LIBNAME)(mem.o)\
	$(LIBNAME)(sys.o)\
	$(LIBNAME)(partab.o)\
	$(LIBNAME)(ud.o)\
	$(LIBNAME)(wd.o)\
	$(LIBNAME)(acia.o)\
	$(LIBNAME)(lp.o)


all:	$(LIBNAME) 

.PRECIOUS:	$(LIBNAME)

$(LIBNAME):	$(FILES)

.c.a:
	M68000= $(CC) $(CFLAGS) $<
	chmod 664 $*.o
	chgrp bin $*.o
	chown bin $*.o
	ar rv $@ $*.o
	-rm -f $*.o

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)

$(LIBNAME)(bio.o):bio.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/var.h\
	$(FRC)
$(LIBNAME)(mmuEXOR1.o):mmu1.c\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h
	M68000= $(CC) -c -O -T -DM68000 -DMEXOR -I$(INCRT) mmu1.c
	mv -f mmu1.o mmuEXOR1.o
	chmod 664 mmuEXOR1.o
	chgrp bin mmuEXOR1.o
	chown bin mmuEXOR1.o
	ar rv $(LIBNAME) mmuEXOR1.o
	-rm -f mmuEXOR1.o
	$(FRC)
$(LIBNAME)(mmuEXOR2.o):mmu2.c\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h
	M68000= $(CC) -c -O -T -DM68000 -DMEXOR -I$(INCRT) mmu2.c
	mv -f mmu2.o mmuEXOR2.o
	chmod 664 mmuEXOR2.o
	chgrp bin mmuEXOR2.o
	chown bin mmuEXOR2.o
	ar rv $(LIBNAME) mmuEXOR2.o
	-rm -f mmuEXOR2.o
	$(FRC)
$(LIBNAME)(mmu451a.o):mmu1.c\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h
	M68000= $(CC) -c -O -T -DM68010 -DM68451 -I$(INCRT) mmu1.c
	mv -f mmu1.o mmu451a.o
	chmod 664 mmu451a.o
	chgrp bin mmu451a.o
	chown bin mmu451a.o
	ar rv $(LIBNAME) mmu451a.o
	-rm -f mmu451a.o
	$(FRC)
$(LIBNAME)(mmu451b.o):mmu2.c\
	$(INCRT)/sys/mmu.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/proc.h
	M68000= $(CC) -c -O -T -DM68010 -DM68451 -I$(INCRT) mmu2.c
	mv -f mmu2.o mmu451b.o
	chmod 664 mmu451b.o
	chgrp bin mmu451b.o
	chown bin mmu451b.o
	ar rv $(LIBNAME) mmu451b.o
	-rm -f mmu451b.o
	$(FRC)
$(LIBNAME)(ram.o):ram.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/ram.h\
	$(INCRT)/sys/map.h\
	$(FRC)
$(LIBNAME)(clist.o):clist.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/tty.h\
	$(FRC)
$(LIBNAME)(clk.o):clk.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(FRC)
$(LIBNAME)(clk6818.o):clk6818.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(FRC)
$(LIBNAME)(err.o):err.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/erec.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(mem.o):mem.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/systm.h\
	$(FRC)
$(LIBNAME)(partab.o):partab.c\
	$(FRC)
$(LIBNAME)(sys.o):sys.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/proc.h\
	$(FRC)
$(LIBNAME)(tt0.o):tt0.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/termio.h\
	$(INCRT)/sys/crtctl.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/mmu.h\
	$(FRC)
$(LIBNAME)(wd.o):wd.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/systm.h\
	$(FRC)
$(LIBNAME)(ud.o):ud.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/sysmacros.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/systm.h\
	$(FRC)
$(LIBNAME)(tty.o):tty.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/termio.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(acia.o):acia.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/termio.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/sysinfo.h\
	$(FRC)
$(LIBNAME)(lp.o):lp.c\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/types.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/signal.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/errno.h\
	$(INCRT)/sys/lprio.h\
	$(FRC)
FRC:
