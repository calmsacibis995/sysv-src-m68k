# @(#)fuser.mk	2.1     
OL =
TESTDIR = .

CC = cc

CFLAGS = -O -DMEXOR
LDFLAGS = -s
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)

compile all:	fuser

fuser:		fuser.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/fuser fuser.o

fuser.o:	fuser.c
	$(CC) $(CFLAGS) -c fuser.c

install:   all
	$(INSE) $(TESTDIR)/fuser

clean:
	rm -f fuser.o

clobber:
	rm -f $(TESTDIR)/fuser
