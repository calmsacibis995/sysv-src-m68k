#	@(#)sh.mk	2.1	
#	3.0 SID #	1.2 #	profile.c is not used for some unknown reason -fgc

LDFLAGS = -s # -n
TESTDIR = .
OL =
BIN = $(OL)/bin
INSB = /etc/install -f $(BIN)
CFLAGS= -c -O -DM68000

OFILES1 = args.o \
	blok.o \
	builtin.o \
	defs.o \
	error.o \
	expand.o \
	fault.o \
	name.o \
	print.o \
	service.o \
	setbrk.o \
	stak.o \
	string.o \
	test.o

OFILES = cmd.o \
	ctype.o \
	io.o \
	macro.o \
	main.o \
	msg.o \
	word.o \
	xec.o \
	$(OFILES1)

DEFS = defs.h \
	brkincr.h \
	ctype.h \
	mac.h \
	mode.h \
	name.h \
	stak.h

compile all: sh

sh: $(OFILES)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/sh $(OFILES)

$(OFILES1): $(DEFS)

cmd.o macro.o word.o: $(DEFS) sym.h

ctype.o: $(DEFS)
	sh :fix ctype

io.o: $(DEFS) dup.h

main.o: $(DEFS) sym.h timeout.h dup.h

msg.o: $(DEFS) sym.h
	sh :fix msg

xec.o: $(DEFS) sym.h
	set +e; \
	if u370; \
	then \
		$(CC) $(CFLAGS) -b1,0 -c xec.c; \
	else \
		$(CC) $(CFLAGS) -c xec.c; \
	fi

install:  all
	$(INSB) $(TESTDIR)/sh
	rm -f $(BIN)/rsh
	ln $(BIN)/sh $(BIN)/rsh

clean:
	rm -f $(OFILES)

clobber:
	rm -f $(TESTDIR)/sh
