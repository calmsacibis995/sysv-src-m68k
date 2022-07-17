# @(#)config.mk	2.1.1.1

TESTDIR = .
INSDIR = /usr/bin
INS = /etc/install -f $(INSDIR)
CFLAGS = -O
LDFLAGS= -s

compile all: config

config: config.*.c
	if vax; \
	then \
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/config config.vax.c; \
	elif pdp11; \
	then \
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/config config.pdp.c; \
	elif m68k; \
	then \
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/config config.68k.c; \
	fi

install: all
	$(INS) $(TESTDIR)/config

clean:
	rm -f *.o

clobber: clean
	rm -f $(TESTDIR)/config
