#	@(#)Makefile	2.1	 -- SCCS command build and installation

HDR = ../hdr
#	Directory containing SCCS header files
LIBS = ../lib/comobj.a ../lib/mpwlib.a ../lib/cassi.a 
#	SCCS common routine archive
TESTDIR = .
#	Directory where executable SCCS commands are stored
CFLAGS = -O 
#	Flags used by C compiler
LDFLAGS = -s 
#	Load flags used by C compiler
SYSLIBS = -lPW
#	Libraries used during loading

OL = 
UBIN = $(OL)/usr/bin
INSUB = /etc/install -f $(UBIN)
#  	installation instruction and destination
ARGS =
 

REGCMDS = admin bdiff comb delta get help prs \
	unget val vc what

ALLCMDS = $(REGCMDS) cdc rmdel sact sccsdiff

OFILES = admin.o bdiff.o comb.o delta.o get.o help.o prs.o \
	 rmchg.o unget.o val.o vc.o what.o

DOTC1 = admin.c bdiff.c rmchg.c comb.c delta.c get.c
DOTC2 = help.c prs.c sccsdiff.sh unget.c val.c vc.c what.c


compile all:	$(ALLCMDS)

$(REGCMDS):	$(OFILES) $(LIBS)
	$(CC) $(LDFLAGS) $@.o $(LIBS) $(SYSLIBS) -o $(TESTDIR)/$@

$(OFILES):	$(HDR)/defines.h $(HDR)/had.h

cdc:	rmdel

rmdel:	rmchg.o
	$(CC) $(LDFLAGS) rmchg.o $(LIBS) $(SYSLIBS) -o $(TESTDIR)/rmdel

sact:	unget

sccsdiff:	sccsdiff.sh
	cp sccsdiff.sh $(TESTDIR)/sccsdiff

$(LIBS):
	cd ../lib; $(MAKE)

install:  all
	$(INSUB) $(TESTDIR)/admin
	$(INSUB) $(TESTDIR)/bdiff
	$(INSUB) $(TESTDIR)/comb
	$(INSUB) $(TESTDIR)/delta
	$(INSUB) $(TESTDIR)/get
	$(INSUB) $(TESTDIR)/help
	$(INSUB) $(TESTDIR)/prs
	$(INSUB) $(TESTDIR)/rmdel
	-rm -f $(UBIN)/cdc
	ln $(UBIN)/rmdel $(UBIN)/cdc
	$(INSUB) $(TESTDIR)/sccsdiff
	$(INSUB) $(TESTDIR)/unget
	-rm -f $(UBIN)/sact
	ln $(UBIN)/unget $(UBIN)/sact
	$(INSUB) $(TESTDIR)/val
	$(INSUB) $(TESTDIR)/vc
	$(INSUB) $(TESTDIR)/what

clean:
	-rm -f *.o

clobber:
	-rm -f $(TESTDIR)/admin
	-rm -f $(TESTDIR)/bdiff
	-rm -f $(TESTDIR)/cdc
	-rm -f $(TESTDIR)/comb
	-rm -f $(TESTDIR)/delta
	-rm -f $(TESTDIR)/get
	-rm -f $(TESTDIR)/help
	-rm -f $(TESTDIR)/prs
	-rm -f $(TESTDIR)/rmdel
	-rm -f $(TESTDIR)/sact
	-rm -f $(TESTDIR)/sccsdiff
	-rm -f $(TESTDIR)/unget
	-rm -f $(TESTDIR)/val
	-rm -f $(TESTDIR)/vc
	-rm -f $(TESTDIR)/what


Bigclob:	clobber
	-rm -f $(DOTC1)
	-rm -f $(DOTC2)
