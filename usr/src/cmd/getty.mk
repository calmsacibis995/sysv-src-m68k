#	@(#)getty.mk	2.1	

LDFLAGS = -s 
TESTDIR = .
OL =
ETC = $(OL)/etc 
INSE = /etc/install -f $(ETC)

compile all: getty

getty: getty.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/getty getty.o

getty.o: getty.c
	$(CC) $(CFLAGS) -c getty.c

install: all
	$(INSE) $(TESTDIR)/getty

clean:
	rm -f getty.o

clobber:
	rm -f $(TESTDIR)/getty

# this target uses "rtest", not part of normal distribution

test:
	rtest $(TESTDIR)/getty
