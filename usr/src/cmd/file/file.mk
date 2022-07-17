#	@(#)file.mk	2.1	
#
#	Makefile for file command -- necessary to install /etc/magic

OL = 
TESTDIR=.
CFLAGS=	-O
LDFLAGS = -s
BIN = $(OL)/bin
ETC = $(OL)/etc
INSB = /etc/install -f $(BIN)

compile all: file

file: file.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/file file.o

file.o: file.c
	$(CC) $(CFLAGS) -c file.c

install: all
	$(INSB) $(TESTDIR)/file
	rm -f $(ETC)/magic
	cp magic $(ETC)
	chmod 644 $(ETC)/magic
	chgrp bin $(ETC)/magic
	chown bin $(ETC)/magic

clean:
	rm -f file.o

clobber:
	rm -f $(TESTDIR)/file
