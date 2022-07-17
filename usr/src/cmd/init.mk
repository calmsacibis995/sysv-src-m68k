#	@(#)init.mk	2.1	

LDFLAGS = -s
TESTDIR = .
CFLAGS = -O
OL =
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)
BIN = $(OL)/bin

compile all: init

init: init.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/init init.o

init.o: init.c
	$(CC) $(CFLAGS) -c init.c

install: all
	$(INSE) $(TESTDIR)/init
	rm -f $(BIN)/telinit
	ln $(ETC)/init $(BIN)/telinit

clean:
	rm -f init.o

clobber:
	rm -f $(TESTDIR)/init

# this target uses "rtest", not in normal distribution

test:
	rtest $(TESTDIR)/init
