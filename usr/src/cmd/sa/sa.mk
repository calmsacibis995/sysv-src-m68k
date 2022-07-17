#/*	@(#)sa.mk	2.1		*/
#
# The sadc and sadp modules must be able to read /dev/kmem,
# which standardly has restricted read permission.
# They must have set-group-ID mode
# and have the same group as /dev/kmem.
# The chmod and chgrp commmands below ensure this.
#
OL =
TESTDIR = .
ROOT =
ALL = sa1 sa2 sadc sadp sag sar timex
SA = $(OL)/usr/lib/sa
UBIN = $(OL)/usr/bin
INSS = $(ROOT)/etc/install -f $(SA)
INSUB = $(ROOT)/etc/install -f $(UBIN)
CFLAGS = -O 
LDFLAGS = -s

compile all: $(ALL)

sa1:
	cp sa1.sh $(TESTDIR)/sa1

sa2:
	cp sa2.sh $(TESTDIR)/sa2

sadc: sadc.c sa.h 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/sadc sadc.c 

sadp: sadp.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/sadp sadp.c

sag: saga.o sagb.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/sag saga.o sagb.o 

saga.o:	saga.c saghdr.h
	$(CC) $(CFLAGS) -c saga.c

sagb.o:	sagb.c saghdr.h
	$(CC) $(CFLAGS) -c sagb.c

sar: sar.c sa.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/sar sar.c

timex:	timex.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/timex timex.c 

install: $(ALL)
	$(INSS) $(TESTDIR)/sa1
	$(INSS) $(TESTDIR)/sa2
	$(INSS) $(TESTDIR)/sadc
	-chmod 02775 $(SA)/sadc
	-chgrp sys $(SA)/sadc
	$(INSUB) $(TESTDIR)/sadp
	-chmod 02775 $(UBIN)/sadp
	-chgrp sys $(UBIN)/sadp
	$(INSUB) $(TESTDIR)/sar
	$(INSUB) $(TESTDIR)/timex

clean:
	-rm -f *.o
 
clobber: 
		-rm -f $(TESTDIR)/sa1
		-rm -f $(TESTDIR)/sa2
		-rm -f $(TESTDIR)/sadc
		-rm -f $(TESTDIR)/sadp
		-rm -f $(TESTDIR)/sag
		-rm -f $(TESTDIR)/sar
		-rm -f $(TESTDIR)/timex

test: testai

testbi:		#test for before installing
	sh $(TESTDIR)/runtest new $(ROOT)/usr/src/cmd/sa

testai:		#test for after install
	sh $(TESTDIR)/runtest new
