#	@(#)lpr.mk	2.1	
OL = 
CFLAGS = -O
LDFLAGS = -s 
LIBES =
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
INSUB = /etc/install -f $(UBIN)
INSUL = /etc/install -f $(ULIB)
TESTDIR = .

compile all:	lpr lpd vpd vpd.pr vpr 
spoolers:	lpr tpr vpr
daemons:	lpd tpd tpd.pr vpd vpd.pr

lpr:	lpr.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/lpr lpr.o $(LIBES)

lpr.o:	lpr.c spool.c 
	$(CC) -DNMAIL $(CFLAGS) -c lpr.c 

lpd:	lpd.o banner.o 
	$(CC) $(LDFLAGS) -o $(TESTDIR)/lpd lpd.o banner.o $(LIBES)

lpd.o:  lpd.c daemon.c daemon0.c
	$(CC) $(CFLAGS) -c lpd.c

vpd:	vlpd.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/vpd vlpd.o $(LIBES)

vlpd.o:	vlpd.c daemon.c daemon0.c 
	$(CC) -DSPDIR='"/usr/spool/vpd"' -DLOCK='"/usr/spool/vpd/lock"'\
		-DPRT='"/usr/lib/vpd.pr"'\
		$(CFLAGS) -c vlpd.c 

vpd.pr:	vpd.pr.o banner.o 
	$(CC) $(LDFLAGS) -o $(TESTDIR)/vpd.pr vpd.pr.o banner.o $(LIBES)

vpd.pr.o: vpd.pr.c
	$(CC) $(CFLAGS) -c vpd.pr.c

vpr:	lpr.c spool.c 
	$(CC) -DSPDIR='"/usr/spool/vpd"' -DDAEMON='"/usr/lib/vpd"' -DNMAIL\
		-DVPR $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/vpr lpr.c $(LIBES)


tpr:	lpr.c spool.c 
	$(CC) -DSPDIR='"/usr/spool/tpd"' -DDAEMON='"/usr/lib/tpd"' -DNMAIL\
		$(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/tpr lpr.c $(LIBES)


tpd:	vlpd.c daemon.c daemon0.c 
	$(CC) -DSPDIR='"/usr/spool/tpd"' -DLOCK='"/usr/spool/tpd/lock"'\
		-DPRT='"/usr/lib/tpd.pr"'\
		$(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/tpd vlpd.c $(LIBES)

tpd.pr:	prt1200.o banner.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/tpd.pr prt1200.o banner.o

install:  all
	$(INSUL) $(TESTDIR)/lpd
	$(INSUB) $(TESTDIR)/lpr
	$(INSUL) $(TESTDIR)/vpd
	$(INSUL) $(TESTDIR)/vpd.pr
	$(INSUB) $(TESTDIR)/vpr

inspools: $(SPOOLERS)
	$(INSUB) $(TESTDIR)/lpr
	$(INSUB) $(TESTDIR)/tpr
	$(INSUB) $(TESTDIR)/vpr

insdaemons: $(DAEMONS)
	$(INSUL) $(TESTDIR)/lpd
	$(INSUL) $(TESTDIR)/tpd
	$(INSUL) $(TESTDIR)/tpd.pr
	$(INSUL) $(TESTDIR)/vpd
	$(INSUL) $(TESTDIR)/vpd.pr
clean:
	rm -f *.o

clobber:
	rm -f $(TESTDIR)/lpr 
	rm -f $(TESTDIR)/lpd
	rm -f $(TESTDIR)/vpd
	rm -f $(TESTDIR)/vpd.pr
	rm -f $(TESTDIR)/vpr
	rm -f $(TESTDIR)/tpd
	rm -f $(TESTDIR)/tpd.pr
	rm -f $(TESTDIR)/tpr
