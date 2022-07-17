#	factor make file
#	SCCS:	@(#)factor.mk	2.1	

OL =
TESTDIR=.
INSUB = /etc/install -f $(UBIN)
UBIN = $(OL)/usr/bin
CFLAGS = -O
LDFLAGS = -s
SOURCE = factor.c

compile all: factor
factor:	factor.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/factor factor.o -lm
factor.o: factor.c
	$(CC) $(CFLAGS) -c factor.c 

install: all
	$(INSUB) $(TESTDIR)/factor

clean:
	rm -f factor.o

clobber:
	  rm -f $(TESTDIR)/factor
