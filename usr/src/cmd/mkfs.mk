#	@(#)mkfs.mk	2.1	

LDFLAGS = -s # -n -i
TESTDIR = .
OL =
ETC = $(OL)/etc
INSE = /etc/install -f $(ETC)

compile all: mkfs

mkfs: mkfs.o 
	-if pdp11; \
	then \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/mkfs mkfs.o; \
	else \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/mkfs mkfs.o; \
		$(CC) $(LDFLAGS) -o $(TESTDIR)/omkfs omkfs.o; \
	fi

mkfs.o: mkfs.c
	-if pdp11; \
	then \
		$(CC) $(CFLAGS) -c mkfs.c; \
	else \
		$(CC) $(CFLAGS) -c mkfs.c; \
		ln mkfs.c omkfs.c; \
		$(CC) $(CFLAGS) -DFsTYPE=1 -c omkfs.c; \
		rm -f omkfs.c; \
	fi

install: all
	-if pdp11 ; \
	then \
		$(INSE) $(TESTDIR)/mkfs; \
	else \
		$(INSE) $(TESTDIR)/mkfs; \
		rm -f $(ETC)/omkfs; \
		$(INSE) $(TESTDIR)/omkfs; \
	fi

clean:
	rm -f mkfs.o omkfs.o

clobber:
	rm -f $(TESTDIR)/mkfs $(TESTDIR)/omkfs
