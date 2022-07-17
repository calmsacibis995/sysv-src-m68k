#	@(#)ratfor.mk	2.1	
OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
CFLAGS = -O
YACCRM=-rm
LDFLAGS=-s
OFILES = r0.o r1.o r2.o rio.o rlook.o rlex.o

compile all: ratfor

ratfor:    $(OFILES) r.g.o
	   $(CC) $(LDFLAGS) $(OFILES) r.g.o -o $(TESTDIR)/ratfor -ly

$(OFILES): r.h r.g.h 
r.g.c r.g.h:   r.g.y 
	   $(YACC) -d r.g.y
	mv y.tab.c r.g.c
	mv y.tab.h r.g.h

install:   all
	   $(INSUB)  $(TESTDIR)/ratfor 

clean:
	   -rm -f *.o
	   $(YACCRM) -f r.g.c r.g.h

clobber:  
	   -rm -f $(TESTDIR)/ratfor

test:
	   rtest $(TESTDIR)/ratfor

