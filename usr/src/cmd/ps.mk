#	ps make file
#	SCCS:	@(#)ps.mk	2.1	

OL =
TESTDIR=.
INSUB = /etc/install -f $(UBIN)
UBIN = $(OL)/bin
CFLAGS = -O
LDFLAGS = -s
SOURCE = ps.c

compile all: ps
ps:	ps.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/ps ps.o
ps.o:	ps.c
	if m68k; \
	then \
	$(CC) $(CFLAGS) -DMEXOR -c ps.c; \
	else \
	$(CC) $(CFLAGS) -c ps.c; \
	fi

install: all
	$(INSUB) $(TESTDIR)/ps

clean:
	rm -f ps.o

clobber:
	  rm -f $(TESTDIR)/ps
