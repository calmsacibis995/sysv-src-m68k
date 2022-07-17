#	@(#)sno.mk	2.1	
OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
IFLAG=-i
CFLAGS = -O
LDFLAGS = -s

compile all:	sno

sno:	sno1.o sno2.o sno3.o sno4.o
	if m68k; then \
	$(CC) $(LDFLAGS) -o $(TESTDIR)/sno \
		sno1.o sno2.o sno3.o sno4.o; \
	else \
	$(CC) $(LDFLAGS) $(IFLAG) -o $(TESTDIR)/sno \
		sno1.o sno2.o sno3.o sno4.o; \
	fi

sno1.o:	sno1.c sno.h
	$(CC) $(CFLAGS) -c sno1.c

sno2.o:	sno2.c sno.h
	$(CC) $(CFLAGS) -c sno2.c

sno3.o:	sno3.c sno.h
	$(CC) $(CFLAGS) -c sno3.c

sno4.o:	sno4.c sno.h
	$(CC) $(CFLAGS) -c sno4.c

install:	all
	$(INSUB) $(TESTDIR)/sno

clean:
	rm -f *.o

clobber:
	rm -f $(TESTDIR)/sno
