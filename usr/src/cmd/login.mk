#	@(#)login.mk	2.1	

LDFLAGS = -s
TESTDIR = .
OL =
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)

compile all: login

login: login.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/login login.o

login.o: login.c
	$(CC) $(CFLAGS) -DCONSOLE='"/dev/console"' -c login.c

install: all
	$(INSB) $(TESTDIR)/login

clean:
	rm -f login.o

clobber:
	rm -f $(TESTDIR)/login

# this target uses "rtest", not in normal distribution

test:
	rtest $(TESTDIR)/login
