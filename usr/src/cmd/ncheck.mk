#	@(#)ncheck.mk	2.1	

OL =
TESTDIR = .
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)
LDFLAGS = -s 
CFLAGS = -O 

compile	all: ncheck

ncheck: ncheck.o
	 $(CC) $(LDFLAGS)  -o $(TESTDIR)/ncheck ncheck.o

ncheck.o: ncheck.c
	$(CC) $(CFLAGS) -c ncheck.c

install: all
	$(INSE)  $(TESTDIR)/ncheck

clean:
	rm -f ncheck.o

clobber:
	rm -f $(TESTDIR)/ncheck

# this target uses "rtest", not in normal distribution

test:
	rtest $(TESTDIR)/ncheck
