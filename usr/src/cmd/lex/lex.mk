#	@(#)lex.mk	2.1	
OL =
TESTDIR = .
UBIN = $(OL)/usr/bin
INSUB  = /etc/install -f $(UBIN)
CFLAGS = -O
LDFLAGS = -s
FILES = main.o sub1.o sub2.o header.o

compile all: lex

lex:	$(FILES) y.tab.o
	$(CC) $(LDFLAGS) -s $(FILES) y.tab.o -ly -o $(TESTDIR)/lex

$(FILES): ldefs.c 
main.o:	  once.c 
y.tab.c:  parser.y 
	  $(YACC) parser.y

install: all
	 $(INSUB) $(TESTDIR)/lex 

clean:
	 -rm -f *.o y.tab.c

clobber: 
	 -rm -f $(TESTDIR)/lex

#  The following targets may contain commands that are not part
#   of the normal distribution

test:
	rtest $(TESTDIR)/lex

lint:	main.c sub1.c sub2.c header.c y.tab.c once.c ldefs.c
	lint -p main.c sub1.c sub2.c header.c y.tab.c once.c ldefs.c

