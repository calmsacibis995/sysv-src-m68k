#	@(#)makefile	2.1.1.1	
CFLAGS=	-O -DDEBUG
CFLAGS=	-O -DDEBUG=2
CFLAGS=	-O -DFLEXNAMES -DVARU
CC = cc
LDFLAGS = -s
OL =
UBIN = $(OL)/usr/bin
TESTDIR = ..
INSUB = /etc/install -f $(UBIN)
INSDIR =

DIS.C=	dis.h  bits.c 
DIS.O=	bits.o 

OBJS=	main.o re.o fio.o bio.o decode.o docomm.o sub.o  \
	access.o pcs.o runpcs.o setup.o message.o \
	symt.o display.o prvar.o xeq.o version.o \
	opset.o optab.o machdep.o udef.o \
	mjmdebug.o	${DIS.O}

COM= ../com
SOURCE=	$(COM)/main.c $(COM)/re.c $(COM)/fio.c $(COM)/bio.c $(COM)/decode.c  \
	$(COM)/docomm.c $(COM)/sub.c $(COM)/access.c $(COM)/pcs.c  \
	$(COM)/runpcs.c $(COM)/setup.c $(COM)/message.c  \
	$(COM)/symt.c $(COM)/display.c $(COM)/prvar.c $(COM)/xeq.c  \
	$(COM)/version.c $(COM)/opset.c $(COM)/optab.c  \
	$(COM)/machdep.c $(COM)/udef.c $(COM)/mjmdebug.c 

CSOURCE= $(COM)/machdep.h $(COM)/access.c $(COM)/machdep.c $(COM)/opset.c \
	$(COM)/optab.c $(COM)/runpcs.c $(COM)/setup.c $(COM)/xeq.c

compile all: $(TESTDIR)/sdb

$(TESTDIR)/sdb:	$(OBJS)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/sdb $(OBJS)


COFF.H= $(COM)/coff.h $(COM)/sgs.h 
MACHDEP.H= $(COM)/machdep.h 
MODE.H= $(COM)/mode.h
HEAD.H= $(COM)/head.h $(COM)/bio.h $(MODE.H) $(MACHDEP.H) 


decode.o:	$(HEAD.H) $(COM)/decode.c
		$(CC) -c $(CFLAGS) $(COM)/decode.c

docomm.o:	$(HEAD.H) $(COFF.H) $(COM)/docomm.c
		$(CC) -c $(CFLAGS) $(COM)/docomm.c

main.o:		$(HEAD.H) $(COM)/main.c
		$(CC) -c $(CFLAGS) $(COM)/main.c

sub.o:		$(HEAD.H) $(COFF.H) $(COM)/sub.c
		$(CC) -c $(CFLAGS) $(COM)/sub.c

re.o:		$(HEAD.H) $(COM)/re.c
		$(CC) -c $(CFLAGS) $(COM)/re.c

fio.o:		$(HEAD.H) $(COM)/fio.c
		$(CC) -c $(CFLAGS) $(COM)/fio.c

bio.o:		$(COM)/bio.h $(COM)/bio.c
		$(CC) -c $(CFLAGS) $(COM)/bio.c

access.o:	$(HEAD.H) $(COM)/access.c
		$(CC) -c $(CFLAGS) $(COM)/access.c

pcs.o:		$(HEAD.H) $(COM)/pcs.c
		$(CC) -c $(CFLAGS) $(COM)/pcs.c

runpcs.o:	$(HEAD.H) $(COM)/runpcs.c
		$(CC) -c $(CFLAGS) $(COM)/runpcs.c

xeq.o:		$(HEAD.H) $(COFF.H) $(COM)/xeq.c
		$(CC) -c $(CFLAGS) $(COM)/xeq.c

setup.o:	$(HEAD.H) $(COFF.H) $(COM)/setup.c
		$(CC) -c $(CFLAGS) $(COM)/setup.c

message.o:	$(MODE.H) $(COM)/message.c
	$(CC) $(CFLAGS) -S $(COM)/message.c
	ed - <$(COM)/:rofix message.s
	as -o message.o message.s
	rm message.s

symt.o:		$(HEAD.H) $(COFF.H) $(COM)/symt.c
		$(CC) -c $(CFLAGS) $(COM)/symt.c

display.o:	$(HEAD.H) $(COFF.H)  $(COM)/display.c
		$(CC) -c $(CFLAGS) $(COM)/display.c

prvar.o:	$(HEAD.H) $(COFF.H) $(COM)/prvar.c
		$(CC) -c $(CFLAGS) $(COM)/prvar.c

version.o:	$(COM)/version.c
		$(CC) -c $(CFLAGS) $(COM)/version.c

opset.o:	$(HEAD.H) $(COM)/opset.c
		$(CC) -c $(CFLAGS) $(COM)/opset.c

optab.o:	$(MODE.H) $(MACHDEP.H)  $(COM)/optab.c
	$(CC) $(CFLAGS) -S $(COM)/optab.c
	ed - <$(COM)/:rofix optab.s
	as -o optab.o optab.s
	rm optab.s
machdep.o:	$(HEAD.H) $(COFF.H) $(COM)/machdep.c
		$(CC) -c $(CFLAGS) $(COM)/machdep.c

udef.o:		$(HEAD.H) $(COFF.H) $(COM)/udef.c
		$(CC) -c $(CFLAGS) $(COM)/udef.c


bits.o:		dis.h

mjmdebug.o:	$(COM)/head.h $(COFF.H) $(COM)/mjmdebug.c
		$(CC) -c $(CFLAGS) $(COM)/mjmdebug.c

install:	all
		$(INSUB) $(TESTDIR)/sdb 

clean :
		-rm -f $(OBJS)

clobber:
		-rm -f $(TESTDIR)/sdb

