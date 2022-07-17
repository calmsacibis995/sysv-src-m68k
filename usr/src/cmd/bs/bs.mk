#	@(#)bs.mk	2.1	
TESTDIR = .
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)
INSDIR =
CFLAGS = -O
FFLAG =
OFILES = atof.o bs.o string.o
OL =
LDFLAGS = -s

compile all: bs

all: bs

bs:	$(OFILES)
	$(CC) $(LDFLAGS) $(FFLAG)  -o $(TESTDIR)/bs $(OFILES) -lm 

atof.o:	atof.c 
bs.o:	bs.c 
string.o: string.c 

test:
	bs testall

install: all
	$(INSB) $(TESTDIR)/bs

clean:
	-rm -f *.o

clobber: 
	-rm -f $(TESTDIR)/bs
