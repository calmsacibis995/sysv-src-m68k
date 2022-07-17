#	@(#)makefile	2.1	
CFLAGS = -O
LDFLAGS = -s
OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSU = /etc/install -f $(UBIN)

compile all: fold ctags strings # mkstr xstr
	
xstr: xstr.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/xstr xstr.o

mkstr: mkstr.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/mkstr mkstr.o

fold:  fold.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/fold fold.o

ctags: ctags.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/ctags ctags.o

strings: strings.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/strings strings.o -lld

xstr.o: xstr.c
	$(CC) $(CFLAGS) -c xstr.c

mkstr.o: mkstr.c
	$(CC) $(CFLAGS) -c mkstr.c

fold.o: fold.c
	$(CC) $(CFLAGS) -c fold.c

ctags.o: ctags.c
	$(CC) $(CFLAGS) -c ctags.c

strings.o: strings.c
	$(CC) $(CFLAGS) -c strings.c

install:  all
	$(INSU) $(TESTDIR)/ctags
	$(INSU) $(TESTDIR)/fold
	$(INSU) $(TESTDIR)/strings

clean:
	rm -f *.o
 
clobber:
	rm -f $(TESTDIR)/fold $(TESTDIR)/ctags $(TESTDIR)/strings

delete: clean clobber
	rm -f *.c
