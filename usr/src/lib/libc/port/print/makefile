#	@(#)makefile	2.1 
#
# makefile for libc/port/print
#
#

INCROOT =

.SUFFIXES: .p
CC=cc
CFLAGS=-O

OBJECTS=\
doprnt.o    fprintf.o   printf.o    sprintf.o

POBJECTS=\
doprnt.o    fprintf.o   printf.o    sprintf.o

nonprof: $(OBJECTS)

prof: $(POBJECTS)

doprnt.o doprnt.p: print.h
doprnt.o doprnt.p: $(INCROOT)/usr/include/ctype.h
doprnt.o doprnt.p fprintf.o fprintf.p \
	printf.o printf.p \
	sprintf.o sprintf.p: $(INCROOT)/usr/include/stdio.h
doprnt.o doprnt.p \
	sprintf.o sprintf.p: $(INCROOT)/usr/include/values.h
doprnt.o doprnt.p fprintf.o fprintf.p \
	printf.o printf.p \
	sprintf.o sprintf.p: $(INCROOT)/usr/include/varargs.h

.c.o .c.p:
	@echo $*.c:
	$(PROF)$(CC) -p $(CFLAGS) -c $*.c    && mv $(*F).o $*.p
	$(CC) $(CFLAGS) -c $*.c
