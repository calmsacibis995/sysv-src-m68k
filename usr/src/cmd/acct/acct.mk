#	@(#)acct.mk	2.1	 of 7/13/82 #
OL=
TESTDIR = .
FRC =
BIN = $(OL)/bin
INLIB =$(OL)/usr/lib/acct
INSB = /etc/install -f $(BIN)
INSL = /etc/install -f $(INLIB) 
ARGS =
CC = cc
CFLAGS = -O
LFLAGS = -s
LIB = lib/a.a

compile all:	library acctcms acctcom acctcon1\
	acctcon2 acctdisk acctdusg acctmerg accton\
	acctprc1 acctprc2 acctwtmp\
	fwtmp wtmpfix\
	chargefee ckpacct dodisk lastlogin\
	monacct nulladm prctmp prdaily\
	prtacct remove runacct\
	shutacct startup turnacct holidays

library:
		cd lib; make "CC=$(CC)"

acctcms:	$(LIB) acctcms.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctcms.c $(LIB) -o $(TESTDIR)/acctcms

acctcom:	$(LIB) acctcom.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctcom.c $(LIB) -o $(TESTDIR)/acctcom

acctcon1:	$(LIB) acctcon1.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctcon1.c $(LIB) -o $(TESTDIR)/acctcon1

acctcon2:	acctcon2.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctcon2.c -o $(TESTDIR)/acctcon2

acctdisk:	acctdisk.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctdisk.c -o $(TESTDIR)/acctdisk

acctdusg:	acctdusg.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctdusg.c -o $(TESTDIR)/acctdusg

acctmerg:	$(LIB) acctmerg.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctmerg.c $(LIB) -o $(TESTDIR)/acctmerg

accton:		accton.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			accton.c -o $(TESTDIR)/accton

acctprc1:	$(LIB) acctprc1.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctprc1.c $(LIB) -o $(TESTDIR)/acctprc1

acctprc2:	acctprc2.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctprc2.c -o $(TESTDIR)/acctprc2

acctwtmp:	acctwtmp.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			acctwtmp.c -o $(TESTDIR)/acctwtmp

fwtmp:		fwtmp.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			fwtmp.c -o $(TESTDIR)/fwtmp

wtmpfix:	wtmpfix.c $(FRC)
		$(CC) $(CFLAGS) $(LFLAGS) \
			wtmpfix.c -o $(TESTDIR)/wtmpfix

chargefee:	chargefee.sh $(FRC)
		cp chargefee.sh $(TESTDIR)/chargefee

ckpacct:	ckpacct.sh $(FRC)
		cp ckpacct.sh $(TESTDIR)/ckpacct

dodisk:		dodisk.sh $(FRC)
		cp dodisk.sh $(TESTDIR)/dodisk

monacct:	monacct.sh $(FRC)
		cp monacct.sh $(TESTDIR)/monacct

lastlogin:	lastlogin.sh $(FRC)
		cp lastlogin.sh $(TESTDIR)/lastlogin

nulladm:	nulladm.sh $(FRC)
		cp nulladm.sh $(TESTDIR)/nulladm

prctmp:		prctmp.sh $(FRC)
		cp prctmp.sh $(TESTDIR)/prctmp

prdaily:	prdaily.sh $(FRC)
		cp prdaily.sh $(TESTDIR)/prdaily

prtacct:	prtacct.sh $(FRC)
		cp prtacct.sh $(TESTDIR)/prtacct

remove:		remove.sh $(FRC)
		cp remove.sh $(TESTDIR)/remove

runacct:	runacct.sh $(FRC)
		cp runacct.sh $(TESTDIR)/runacct

shutacct:	shutacct.sh $(FRC)
		cp shutacct.sh $(TESTDIR)/shutacct

startup:	startup.sh $(FRC)
		cp startup.sh $(TESTDIR)/startup

turnacct:	turnacct.sh $(FRC)
		cp turnacct.sh $(TESTDIR)/turnacct

holidays:	$(FRC)
		cp holidays $(TESTDIR)/holidays

install:	all
		-if [ ! -d $(INLIB) ]; then mkdir $(INLIB); fi
		-chmod 775 $(INLIB)
		$(INSL) $(TESTDIR)/acctcms
		$(INSB) $(TESTDIR)/acctcom
		$(INSL) $(TESTDIR)/acctcon1
		$(INSL) $(TESTDIR)/acctcon2
		$(INSL) $(TESTDIR)/acctdisk
		$(INSL) $(TESTDIR)/acctdusg
		$(INSL) $(TESTDIR)/acctmerg
		-cp $(TESTDIR)/accton $(INLIB)/accton
		-chmod 4755 $(INLIB)/accton
		-chgrp adm $(INLIB)/accton
		-chown root $(INLIB)/accton
		$(INSL) $(TESTDIR)/acctprc1
		$(INSL) $(TESTDIR)/acctprc2
		$(INSL) $(TESTDIR)/acctwtmp
		$(INSL) $(TESTDIR)/fwtmp
		$(INSL) $(TESTDIR)/wtmpfix
		$(INSL) $(TESTDIR)/chargefee
		$(INSL) $(TESTDIR)/ckpacct
		$(INSL) $(TESTDIR)/dodisk
		$(INSL) $(TESTDIR)/monacct
		$(INSL) $(TESTDIR)/lastlogin
		$(INSL) $(TESTDIR)/nulladm
		$(INSL) $(TESTDIR)/prctmp
		$(INSL) $(TESTDIR)/prdaily
		$(INSL) $(TESTDIR)/prtacct
		$(INSL) $(TESTDIR)/remove
		$(INSL) $(TESTDIR)/runacct
		$(INSL) $(TESTDIR)/shutacct
		$(INSL) $(TESTDIR)/startup
		$(INSL) $(TESTDIR)/turnacct
		-cp holidays $(INLIB)/holidays
		-chmod 644 $(INLIB)/holidays
		-chgrp adm $(INLIB)/holidays
		-chown adm $(INLIB)/holidays
		

clean:
		-rm -f *.o
		cd lib; make clean

clobber:	
		-rm -f $(TESTDIR)/acctcms
		-rm -f $(TESTDIR)/acctcom
		-rm -f $(TESTDIR)/acctcon1
		-rm -f $(TESTDIR)/acctcon2
		-rm -f $(TESTDIR)/acctdisk
		-rm -f $(TESTDIR)/acctdusg
		-rm -f $(TESTDIR)/acctmerg
		-rm -f $(TESTDIR)/accton
		-rm -f $(TESTDIR)/acctprc1
		-rm -f $(TESTDIR)/acctprc2
		-rm -f $(TESTDIR)/acctwtmp
		-rm -f $(TESTDIR)/chargefee
		-rm -f $(TESTDIR)/ckpacct
		-rm -f $(TESTDIR)/dodisk
		-rm -f $(TESTDIR)/fwtmp
		-rm -f $(TESTDIR)/lastlogin
		-rm -f $(TESTDIR)/monacct
		-rm -f $(TESTDIR)/nulladm
		-rm -f $(TESTDIR)/prctmp
		-rm -f $(TESTDIR)/prdaily
		-rm -f $(TESTDIR)/prtacct
		-rm -f $(TESTDIR)/remove
		-rm -f $(TESTDIR)/runacct
		-rm -f $(TESTDIR)/shutacct
		-rm -f $(TESTDIR)/startup
		-rm -f $(TESTDIR)/turnacct
		-rm -f $(TESTDIR)/wtmpfix
		cd lib; make clobber

FRC:
