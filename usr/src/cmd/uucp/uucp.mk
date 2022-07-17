#	/* @(#)uucp.mk	2.1	 */
OL=
TESTDIR=.
CC=cc
CFLAGS=-O 
LDFLAGS=-s
OWNER=uucp
UBIN=$(OL)/usr/bin
ULIBUU=$(OL)/usr/lib/uucp
PUBDIR=$(OL)/usr/spool/uucppublic
SPOOL=$(OL)/usr/spool/uucp
XQTDIR=$(ULIBUU)/.XQTDIR
PKON=pkon.o
PKONSRC=pkon.c
LINE=line.o
LINESRC=line.c
IOCTL=
LINTOP=
COMMANDS=uucp uux uuxqt uucico uulog uuclean uuname uustat uusub
OFILES=assert.o cpmv.o expfile.o gename.o getpwinfo.o uucpdefs.o \
	prefix.o shio.o ulockf.o xqt.o logent.o versys.o gnamef.o rkill.o
LFILES=assert.c cpmv.c expfile.c gename.c getpwinfo.c uucpdefs.c \
	prefix.c shio.c ulockf.c xqt.c logent.c versys.c gnamef.c rkill.c
OUUCP=uucp.o gwd.o chkpth.o getargs.o uucpname.o us_crs.o
LUUCP=uucp.c gwd.c chkpth.c getargs.c uucpname.c us_crs.c
OUUX=uux.o gwd.o anyread.o chkpth.o getargs.o getprm.o uucpname.o
LUUX=uux.c gwd.c anyread.c chkpth.c getargs.c getprm.c uucpname.c
OUUXQT=uuxqt.o mailst.o getprm.o uucpname.o chkpth.o getargs.o anyread.o
LUUXQT=uuxqt.c mailst.c getprm.c uucpname.c chkpth.c getargs.c anyread.c
OUUCICO=cico.o conn.o cntrl.o pk0.o pk1.o gio.o xio.o anyread.o \
	anlwrk.o chkpth.o getargs.o gnsys.o gnxseq.o \
	$(PKON) pkdefs.o imsg.o fwdchk.o sysacct.o systat.o gtcfile.o \
	us_crs.o mailst.o uucpname.o us_rrs.o us_sst.o ub_sst.o $(LINE)
LUUCICO=cico.c conn.c cntrl.c pk0.c pk1.c gio.c xio.c anyread.c \
	anlwrk.c chkpth.c getargs.c gnsys.c gnxseq.c \
	$(PKONSRC) pkdefs.c imsg.c fwdchk.c sysacct.c systat.c gtcfile.c \
	us_crs.c mailst.c uucpname.c us_rrs.c us_sst.c ub_sst.c $(LINESRC)
OUULOG=uucpdefs.o uulog.o prefix.o xqt.o ulockf.o gnamef.o assert.o logent.o
LUULOG=uucpdefs.c uulog.c prefix.c xqt.c ulockf.c gnamef.c assert.c logent.c
OUUCLEAN=uuclean.o mailst.o getargs.o
LUUCLEAN=uuclean.c mailst.c getargs.c
OUUNAME=uuname.o uucpname.o uucpdefs.o getpwinfo.o
LUUNAME=uuname.c uucpname.c uucpdefs.c getpwinfo.c
OUUSTAT=uustat.o
LUUSTAT=uustat.c
OUUSUB=uucpdefs.o uusub.o getpwinfo.o xqt.o
LUUSUB=uucpdefs.c uusub.c getpwinfo.c xqt.c 

compile all:	$(COMMANDS) 

uucp:	$(OUUCP) $(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uucp $(OUUCP) $(OFILES)

uux:	$(OUUX) $(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uux $(OUUX) $(OFILES)

uuxqt:	$(OUUXQT) $(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uuxqt $(OUUXQT) $(OFILES)

uucico:	$(OUUCICO) $(OFILES) $(IOCTL)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uucico $(OUUCICO) $(OFILES) $(IOCTL)

uulog:	$(OUULOG)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uulog $(OUULOG)

uuclean:  $(OUUCLEAN) $(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uuclean $(OUUCLEAN) $(OFILES)

uuname:	$(OUUNAME)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uuname $(OUUNAME)
 
uustat:	$(OUUSTAT) $(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uustat $(OUUSTAT) $(OFILES)

uusub:	$(OUUSUB)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/uusub $(OUUSUB)


ub_sst.o uusub.o:	uusub.h

cico.o:	uusub.h uust.h uucp.h

anlwrk.o cntrl.o us_crs.o us_rrs.o\
	gtcfile.o rkill.o us_sst.o uucp.o uustat.o:	uust.h uucp.h

anyread.o assert.o chkpth.o conn.o cpmv.o expfile.o gename.o\
	getpwinfo.o fwdchk.o xio.o gnamef.o gnsys.o gnxseq.o gwd.o imsg.o $(IOCTL)\
	getargs.o prefix.o logent.o mailst.o $(LINE) shio.o\
	systat.o ulockf.o uuclean.o uucpdefs.o uucpname.o uulog.o uuname.o\
	uux.o uuxqt.o versys.o xqt.o:	uucp.h

gio.o: pk.p pk.h uucp.h

pk0.o pk1.o pkdefs.o: pk.p pk.h

install:	all mkdirs
	cp $(TESTDIR)/uucp $(UBIN)
	chmod 4111 $(UBIN)/uucp
	chgrp bin $(UBIN)/uucp
	chown $(OWNER) $(UBIN)/uucp
	cp $(TESTDIR)/uux $(UBIN)
	chmod 4111 $(UBIN)/uux
	chgrp bin $(UBIN)/uux
	chown $(OWNER) $(UBIN)/uux
	cp $(TESTDIR)/uuxqt $(ULIBUU)
	chmod 4111 $(ULIBUU)/uuxqt
	cp $(TESTDIR)/uucico $(ULIBUU)
	chmod 4111 $(ULIBUU)/uucico
	cp $(TESTDIR)/uulog $(UBIN)
	chmod 4111 $(UBIN)/uulog
	chgrp bin $(UBIN)/uulog
	chown $(OWNER) $(UBIN)/uulog
	cp $(TESTDIR)/uuclean $(ULIBUU)
	chmod 4111 $(ULIBUU)/uuclean
	cp $(TESTDIR)/uuname $(UBIN)
	chmod 4111 $(UBIN)/uuname
	chgrp bin $(UBIN)/uuname
	chown $(OWNER) $(UBIN)/uuname
	cp $(TESTDIR)/uustat $(UBIN)
	chmod 4111 $(UBIN)/uustat
	chgrp bin $(UBIN)/uustat
	chown $(OWNER) $(UBIN)/uustat
	cp $(TESTDIR)/uusub $(ULIBUU)
	chmod 100 $(ULIBUU)/uusub
	cp L-devices $(ULIBUU)
	chmod 444 $(ULIBUU)/L-devices
	cp L.cmds $(ULIBUU)
	chmod 444 $(ULIBUU)/L.cmds
	cp L.sys $(ULIBUU)
	chmod 400 $(ULIBUU)/L.sys
	cp USERFILE $(ULIBUU)
	chmod 400 $(ULIBUU)/USERFILE
	cp uudemon.day $(ULIBUU)
	chmod 755 $(ULIBUU)/uudemon.day
	cp uudemon.hr $(ULIBUU)
	chmod 755 $(ULIBUU)/uudemon.hr
	cp uudemon.wk $(ULIBUU)
	chmod 755 $(ULIBUU)/uudemon.wk
	>$(ULIBUU)/L_stat
	>$(ULIBUU)/R_stat
	>$(ULIBUU)/L_sub
	>$(ULIBUU)/R_sub
	chgrp bin $(ULIBUU)/*
	chown $(OWNER) $(ULIBUU)/*
	-chmod 755 $(ULIBUU)
	-chgrp bin $(ULIBUU)
	-chown $(OWNER) $(ULIBUU)

mkdirs:
	-mkdir $(ULIBUU)
	-mkdir $(SPOOL)
	-chmod 777 $(SPOOL)
	-chgrp bin $(SPOOL)
	-chown $(OWNER) $(SPOOL)
	-mkdir $(PUBDIR)
	-chmod 777 $(PUBDIR)
	-chgrp bin $(PUBDIR)
	-chown $(OWNER) $(PUBDIR)
	-mkdir $(XQTDIR)
	-chmod 777 $(XQTDIR)
	-chgrp bin $(XQTDIR)
	-chown $(OWNER) $(XQTDIR)

clean:
	rm -f *.o

clobber:
	rm -f $(TESTDIR)/uucp
	rm -f $(TESTDIR)/uux
	rm -f $(TESTDIR)/uuxqt
	rm -f $(TESTDIR)/uucico
	rm -f $(TESTDIR)/uulog
	rm -f $(TESTDIR)/uuclean
	rm -f $(TESTDIR)/uuname
	rm -f $(TESTDIR)/uustat
	rm -f $(TESTDIR)/uusub

save:	all
	-mv $(UBIN)/uucp $(UBIN)/OLDuucp
	cp $(TESTDIR)/uucp $(UBIN)
	chmod 4111 $(UBIN)/uucp
	chgrp bin $(UBIN)/uucp
	chown $(OWNER) $(UBIN)/uucp
	-mv $(UBIN)/uux $(UBIN)/OLDuux
	cp $(TESTDIR)/uux $(UBIN)
	chmod 4111 $(UBIN)/uux
	chgrp bin $(UBIN)/uux
	chown $(OWNER) $(UBIN)/uux
	-mv $(ULIBUU)/uuxqt $(ULIBUU)/OLDuuxqt
	cp $(TESTDIR)/uuxqt $(ULIBUU)
	chmod 4111 $(ULIBUU)/uuxqt
	-mv $(ULIBUU)/uucico $(ULIBUU)/OLDuucico
	cp $(TESTDIR)/uucico $(ULIBUU)
	chmod 4111 $(ULIBUU)/uucico
	-mv $(UBIN)/uulog $(UBIN)/OLDuulog
	cp $(TESTDIR)/uulog $(UBIN)
	chmod 4111 $(UBIN)/uulog
	chgrp bin $(UBIN)/uulog
	chown $(OWNER) $(UBIN)/uulog
	-mv $(ULIBUU)/uuclean $(ULIBUU)/OLDuuclean
	cp $(TESTDIR)/uuclean $(ULIBUU)
	chmod 4111 $(ULIBUU)/uuclean
	-mv $(UBIN)/uuname $(UBIN)/OLDuuname
	cp $(TESTDIR)/uuname $(UBIN)
	chmod 4111 $(UBIN)/uuname
	chgrp bin $(UBIN)/uuname
	chown $(OWNER) $(UBIN)/uuname
	-mv $(UBIN)/uustat $(UBIN)/OLDuustat
	cp $(TESTDIR)/uustat $(UBIN)
	chmod 4111 $(UBIN)/uustat
	chgrp bin $(UBIN)/uustat
	chown $(OWNER) $(UBIN)/uustat
	-mv $(ULIBUU)/uusub $(ULIBUU)/OLDuusub
	cp $(TESTDIR)/uusub $(ULIBUU)
	chmod 100 $(ULIBUU)/uusub
	-mv $(ULIBUU)/L-devices $(ULIBUU)/OLDL-devices
	cp L-devices $(ULIBUU)
	chmod 444 $(ULIBUU)/L-devices
	-mv $(ULIBUU)/L.cmds $(ULIBUU)/OLDL.cmds
	cp L.cmds $(ULIBUU)
	chmod 444 $(ULIBUU)/L.cmds
	-mv $(ULIBUU)/L.sys $(ULIBUU)/OLDL.sys
	cp L.sys $(ULIBUU)
	chmod 400 $(ULIBUU)/L.sys
	-mv $(ULIBUU)/USERFILE $(ULIBUU)/OLDUSERFILE
	cp USERFILE $(ULIBUU)
	chmod 400 $(ULIBUU)/USERFILE
	-mv $(ULIBUU)/uudemon.day $(ULIBUU)/OLDuudemon.day
	cp uudemon.day $(ULIBUU)
	chmod 755 $(ULIBUU)/uudemon.day
	-mv $(ULIBUU)/uudemon.hr $(ULIBUU)/OLDuudemon.hr
	cp uudemon.hr $(ULIBUU)
	chmod 755 $(ULIBUU)/uudemon.hr
	-mv $(ULIBUU)/uudemon.wk $(ULIBUU)/OLDuudemon.wk
	cp uudemon.wk $(ULIBUU)
	chmod 755 $(ULIBUU)/uudemon.wk
	-mv $(ULIBUU)/L_stat $(ULIBUU)/OLDL_stat
	>$(ULIBUU)/L_stat
	-mv $(ULIBUU)/R_stat $(ULIBUU)/OLDR_stat
	>$(ULIBUU)/R_stat
	-mv $(ULIBUU)/L_sub $(ULIBUU)/OLDL_sub
	>$(ULIBUU)/L_sub
	-mv $(ULIBUU)/R_sub $(ULIBUU)/OLDR_sub
	>$(ULIBUU)/R_sub
	chgrp bin $(ULIBUU)/*
	chown $(OWNER) $(ULIBUU)/*
	-chmod 755 $(ULIBUU)
	-chgrp bin $(ULIBUU)
	-chown $(OWNER) $(ULIBUU)

restore:
	-chmod u+w $(UBIN)/uucp
	-mv $(UBIN)/OLDuucp $(UBIN)/uucp
	chmod 4111 $(UBIN)/uucp
	chgrp bin $(UBIN)/uucp
	chown $(OWNER) $(UBIN)/uucp
	-chmod u+w $(UBIN)/uux
	-mv $(UBIN)/OLDuux $(UBIN)/uux
	chmod 4111 $(UBIN)/uux
	chgrp bin $(UBIN)/uux
	chown $(OWNER) $(UBIN)/uux
	-chmod u+w $(ULIBUU)/uuxqt
	-mv $(ULIBUU)/OLDuuxqt $(ULIBUU)/uuxqt
	chmod 4111 $(ULIBUU)/uuxqt
	-chmod u+w $(ULIBUU)/uucico
	-mv $(ULIBUU)/OLDuucico $(ULIBUU)/uucico
	chmod 4111 $(ULIBUU)/uucico
	-chmod u+w $(UBIN)/uulog
	-mv $(UBIN)/OLDuulog $(UBIN)/uulog
	chmod 4111 $(UBIN)/uulog
	chgrp bin $(UBIN)/uulog
	chown $(OWNER) $(UBIN)/uulog
	-chmod u+w $(ULIBUU)/uuclean
	-mv $(ULIBUU)/OLDuuclean $(ULIBUU)/uuclean
	chmod 4111 $(ULIBUU)/uuclean
	-chmod u+w $(UBIN)/uuname
	-mv $(UBIN)/OLDuuname $(UBIN)/uuname
	chmod 4111 $(UBIN)/uuname
	chgrp bin $(UBIN)/uuname
	chown $(OWNER) $(UBIN)/uuname
	-chmod u+w $(UBIN)/uustat
	-mv $(UBIN)/OLDuustat $(UBIN)/uustat
	chmod 4111 $(UBIN)/uustat
	chgrp bin $(UBIN)/uustat
	chown $(OWNER) $(UBIN)/uustat
	-chmod u+w $(ULIBUU)/uusub
	-mv $(ULIBUU)/OLDuusub $(ULIBUU)/uusub
	chmod 100 $(ULIBUU)/uusub
	-chmod u+w $(ULIBUU)/L-devices
	-mv $(ULIBUU)/OLDL-devices $(ULIBUU)/L-devices
	chmod 444 $(ULIBUU)/L-devices
	-chmod u+w $(ULIBUU)/L.cmds
	-mv $(ULIBUU)/OLDL.cmds $(ULIBUU)/L.cmds
	chmod 444 $(ULIBUU)/L.cmds
	-chmod u+w $(ULIBUU)/L.sys
	-mv $(ULIBUU)/OLDL.sys $(ULIBUU)/L.sys
	chmod 400 $(ULIBUU)/L.sys
	-chmod u+w $(ULIBUU)/USERFILE
	-mv $(ULIBUU)/OLDUSERFILE $(ULIBUU)/USERFILE
	chmod 400 $(ULIBUU)/USERFILE
	-mv $(ULIBUU)/OLDuudemon.day $(ULIBUU)/uudemon.day
	chmod 755 $(ULIBUU)/uudemon.day
	-mv $(ULIBUU)/OLDuudemon.hr $(ULIBUU)/uudemon.hr
	chmod 755 $(ULIBUU)/uudemon.hr
	-mv $(ULIBUU)/OLDuudemon.wk $(ULIBUU)/uudemon.wk
	chmod 755 $(ULIBUU)/uudemon.wk
	-mv $(ULIBUU)/OLDL_stat $(ULIBUU)/L_stat
	-mv $(ULIBUU)/OLDR_stat $(ULIBUU)/R_stat
	-mv $(ULIBUU)/OLDL_sub $(ULIBUU)/L_sub
	-mv $(ULIBUU)/OLDR_sub $(ULIBUU)/R_sub
	chgrp bin $(ULIBUU)/*
	chown $(OWNER) $(ULIBUU)/*

burn:
	-rm -f $(UBIN)/OLDuucp
	-rm -f $(UBIN)/OLDuux
	-rm -f $(ULIBUU)/OLDuuxqt
	-rm -f $(ULIBUU)/OLDuucico
	-rm -f $(UBIN)/OLDuulog
	-rm -f $(ULIBUU)/OLDuuclean
	-rm -f $(UBIN)/OLDuuname
	-rm -f $(UBIN)/OLDuustat
	-rm -f $(ULIBUU)/OLDuusub
	-rm -f $(ULIBUU)/OLDL-devices
	-rm -f $(ULIBUU)/OLDL.cmds
	-rm -f $(ULIBUU)/OLDL.sys
	-rm -f $(ULIBUU)/OLDUSERFILE
	-rm -f $(ULIBUU)/OLDuudemon.day
	-rm -f $(ULIBUU)/OLDuudemon.hr
	-rm -f $(ULIBUU)/OLDuudemon.wk
	-rm -f $(ULIBUU)/OLDL_stat
	-rm -f $(ULIBUU)/OLDR_stat
	-rm -f $(ULIBUU)/OLDL_sub
	-rm -f $(ULIBUU)/OLDR_sub

#  lint procedures

lint:	lintuucp lintuucico lintuux lintuuxqt lintuulog lintuuclean\
	lintuuname lintuustat lintuusub
lintuucp:
	lint $(LINTOP) $(LUUCP) $(LFILES)

lintuucico:
	lint $(LINTOP) $(LUUCICO) $(LFILES)

lintuux:
	lint $(LINTOP) $(LUUX) $(LFILES)

lintuuxqt:
	lint $(LINTOP) $(LUUXQT) $(LFILES)

lintuulog:
	lint $(LINTOP) $(LUULOG)

lintuuclean:
	lint $(LINTOP) $(LUUCLEAN) $(LFILES)

lintuuname:
	lint $(LINTOP) $(LUUNAME)

lintuustat:
	lint $(LINTOP) $(LUUSTAT) $(LFILES)

lintuusub:
	lint $(LINTOP) $(LUUSUB)

