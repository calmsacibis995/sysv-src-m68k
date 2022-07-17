#	@(#)fsdb.mk	2.1	

LDFLAGS = -s
TESTDIR = .
OL =
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)

compile all: fsdb

fsdb: fsdb.o
	-if pdp11; \
	then \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/fsdb fsdb.o; \
	else \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/fsdb fsdb.o; \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/fsdb1b fsdb1b.o; \
	fi

fsdb.o: fsdb.c
	-if pdp11; \
	then \
		$(CC) $(CFLAGS) -DFsTYPE=1 -c fsdb.c; \
	else \
		$(CC) $(CFLAGS) -DFsTYPE=2 -c fsdb.c; \
		ln fsdb.c fsdb1b.c; \
		$(CC) $(CFLAGS) -DFsTYPE=1 -c fsdb1b.c; \
		rm -f fsdb1b.c; \
	fi

install: all
	-if pdp11; \
	then \
		$(INSE) $(TESTDIR)/fsdb; \
	else \
		$(INSE) $(TESTDIR)/fsdb; \
		rm -f $(ETC)/fsdb1b; \
		$(INSE) $(TESTDIR)/fsdb1b; \
	fi

clean:
	rm -f fsdb.o fsdb1b.o

clobber:
	rm -f $(TESTDIR)/fsdb $(TESTDIR)/fsdb1b
