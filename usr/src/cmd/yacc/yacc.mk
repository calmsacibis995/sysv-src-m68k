#	@(#)yacc.mk	2.1	 OL = 
OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
ULIB = $(OL)/usr/lib
INSUB  = /etc/install -f $(UBIN)
CFLAGS = -O
LDFLAGS = -s
OFILES = y1.o y2.o y3.o y4.o

compile all: yacc

yacc:	$(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/yacc $(OFILES)

$(OFILES): dextern files 


install:   all
	   $(INSUB) $(TESTDIR)/yacc
	   rm -f $(ULIB)/yaccpar
	   cp yaccpar $(ULIB)
	   chmod 644 $(ULIB)/yaccpar
	   chgrp bin $(ULIB)/yaccpar
	   chown bin $(ULIB)/yaccpar

clean:
	   -rm -f *.o

clobber:   
	   -rm -f $(TESTDIR)/yacc

test:
	   rtest $(TESTDIR)/yacc

