# @(#)profiler.mk	2.1	
CC = cc

CFLAGS = -O
LDFLAGS = -s
INSE = /etc/install -f $(ETC)
ETC = $(OL)/etc

compile all:	prfld prfdc prfpr prfsnap prfstat

prfld:		prfld.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/prfld prfld.o

prfdc:		prfdc.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/prfdc prfdc.o

prfpr:		prfpr.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/prfpr prfpr.o

prfsnap:	prfsnap.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/prfsnap prfsnap.o

prfstat:	prfstat.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/prfstat prfstat.o

prfld.o:	prfld.c
	$(CC) $(CFLAGS) -c prfld.c

prfdc.o:	prfdc.c
	$(CC) $(CFLAGS) -c prfdc.c

prfpr.o:	prfpr.c
	$(CC) $(CFLAGS) -c prfpr.c

prfsnap.o:	prfsnap.c
	$(CC) $(CFLAGS) -c prfsnap.c

prfstat.o:	prfstat.c
	$(CC) $(CFLAGS) -c prfstat.c

install:   all
	$(INSE) $(TESTDIR)/prfld
	$(INSE) $(TESTDIR)/prfdc
	$(INSE) $(TESTDIR)/prfpr
	$(INSE) $(TESTDIR)/prfsnap
	$(INSE) $(TESTDIR)/prfstat


clean:
	rm -f *.o

clobber:
	rm -f $(TESTDIR)/prfld
	rm -f $(TESTDIR)/prfdc
	rm -f $(TESTDIR)/prfpr
	rm -f $(TESTDIR)/prfsnap
	rm -f $(TESTDIR)/prfstat
