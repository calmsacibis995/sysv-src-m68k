#	@(#)m4.mk	2.1	

OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSU = /etc/install -f $(UBIN)
CFLAGS = -O
LDFLAGS = -s # -i
YFLAGS =

compile all: m4

m4:	m4.o m4ext.o m4macs.o m4y.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/m4 m4.o m4ext.o m4macs.o m4y.o -ly

m4.o: m4.h m4.c
	$(CC) $(CFLAGS) -c m4.c

m4ext.o: m4.h m4ext.c
	$(CC) $(CFLAGS) -c m4ext.c

m4macs.o: m4.h m4macs.c
	$(CC) $(CFLAGS) -c m4macs.c

m4y.o: m4y.y
	$(YACC) $(YFLAGS) m4y.y
	$(CC) $(CFLAGS) -c y.tab.c
	rm y.tab.c
	mv y.tab.o m4y.o

install: all
	$(INSU) $(TESTDIR)/m4

clean:
	-rm -f *.o

clobber:
	-rm -f $(TESTDIR)/m4

# this target uses a utility not in normal distribution

test:
	rtest $(TESTDIR)/m4
