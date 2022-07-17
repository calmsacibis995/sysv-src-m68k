#	@(#)fsck.mk	2.1	

LDFLAGS = -s
TESTDIR = .
OL =
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)

compile all: fsck

fsck: fsck.o
	-if pdp11; \
	then \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/fsck fsck.o; \
	else \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/fsck fsck.o; \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/fsck1b fsck1b.o; \
	fi

fsck.o: fsck.c
	-if pdp11; \
	then \
		$(CC) $(CFLAGS) -DFsTYPE=1 -c fsck.c; \
	else \
		$(CC) $(CFLAGS) -DFsTYPE=2 -c fsck.c; \
		ln fsck.c fsck1b.c; \
		$(CC) $(CFLAGS) -DFsTYPE=1 -c fsck1b.c; \
		rm -f fsck1b.c; \
	fi

install: all
	-if pdp11; \
	then \
		$(INSE) $(TESTDIR)/fsck; \
	else \
		$(INSE) $(TESTDIR)/fsck; \
		rm -f $(ETC)/fsck1b; \
		$(INSE) $(TESTDIR)/fsck1b; \
	fi

clean:
	rm -f fsck.o fsck1b.o

clobber:
	rm -f $(TESTDIR)/fsck $(TESTDIR)/fsck1b
