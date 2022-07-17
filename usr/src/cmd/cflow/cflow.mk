#	@(#)cflow.mk	2.1	
#	cflow family makefile

FILES = README cflow.mk cflow.sh dag.c flip.c lpfx.c nmf.c

TESTDIR = .
CFLAGS = -O
LDFLAGS = -s # -i
ALL = dag lpfx nmf flip cflow
LINT = ../lint
MIP = ../cc/pcc/mip

OL =
BIN = $(OL)/usr/bin
LIB = $(OL)/usr/lib
INSB = /etc/install -f $(BIN)
INSL = /etc/install -f $(LIB)

compile all: $(ALL)

dag: dag.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/dag dag.c

lpfx: lpfx.c \
	$(LINT)/lerror.h \
	$(LINT)/lmanifest \
	$(LINT)/lpass2.h \
	$(MIP)/manifest
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(LINT) -I$(MIP) -o $(TESTDIR)/lpfx lpfx.c

nmf: nmf.c
	$(CC) $(CFLAGS) -o $(TESTDIR)/nmf nmf.c

flip: flip.c
	$(CC) $(CFLAGS) -o $(TESTDIR)/flip flip.c

cflow:
	cp cflow.sh $(TESTDIR)/cflow

install: all
	$(INSB) $(TESTDIR)/cflow 
	$(INSL) $(TESTDIR)/dag
	$(INSL) $(TESTDIR)/lpfx
	$(INSL) $(TESTDIR)/nmf
	$(INSL) $(TESTDIR)/flip

clean:
	-rm -f *.o *.out core

clobber: 
	-rm -f $(ALL)
