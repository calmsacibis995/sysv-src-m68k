# @(#)m68k.mk	2.1 
# Description file for the Make command

LIBES =
CFLAGS = -O -I../vax -I. -DFsTYPE=2 -DMC68
TESTDIR = .
INS = /etc/install -n /etc

TYPES = /usr/include/sys/types.h
DIRS = /usr/include/sys/dir.h
STATS = /usr/include/sys/stat.h /usr/include/ustat.h
INODES = /usr/include/sys/ino.h /usr/include/sys/inode.h
STDIO = /usr/include/stdio.h

# Check dependency of fscv on object files
all: fscv

fscv:  	ckmnt.o cnvino.o cpyblk.o dodir.o fscv.o indfm.o indfv.o inov.o\
	indino.o superm.o superv.o swap.o
	$(CC) -s $(LDFLAGS) *.o -o $(TESTDIR)/fscv $(LIBES)


ckmnt.o : $(TYPES)  $(STATS) ../vax/ckmnt.c
	$(CC) $(CFLAGS) -c ../vax/ckmnt.c
cnvino.o : $(TYPES) $(INODES) ../vax/cnvino.c
	$(CC) $(CFLAGS) -c ../vax/cnvino.c
cpyblk.o :  $(STDIO) ../vax/params.h ../vax/cpyblk.c
	$(CC) $(CFLAGS) -c ../vax/cpyblk.c
fscv.o :  $(STDIO) ../vax/params.h super68.h ../vax/super32.h ../vax/fscv.c
	$(CC) $(CFLAGS) -c ../vax/fscv.c
indfv.o :  $(STDIO) indir68.h ../vax/indir32.h ../vax/indfv.c
	$(CC) $(CFLAGS) -c ../vax/indfv.c
inov.o : $(STDIO) ../vax/inov.c
	$(CC) $(CFLAGS) -c ../vax/inov.c
indino.o :  $(STDIO) ../vax/indino.c
	$(CC) $(CFLAGS) -c ../vax/indino.c
superv.o : super68.h ../vax/super32.h ../vax/superv.c
	$(CC) $(CFLAGS) -c ../vax/superv.c
swap.o : $(STDIO) ../vax/swap.c
	$(CC) $(CFLAGS) -c ../vax/swap.c
dodir.o: $(STDIO) $(TYPES) $(DIRS)
indfm.o indfv.o :  $(STDIO) indir68.h ../vax/indir32.h
superm.o: super68.h ../vax/super32.h
clean:
	rm -f *.o
clobber: clean
	rm -f $(TESTDIR)/fscv

install: all
	$(INS) $(TESTDIR)/fscv

test:
	rtest fscv
