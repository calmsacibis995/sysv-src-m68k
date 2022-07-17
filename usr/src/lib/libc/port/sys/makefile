#	@(#)makefile	2.1 
#
# makefile for libc/port/sys
#
#
INCROOT =

.SUFFIXES: .p
CC=cc
CFLAGS=-O

OBJECTS=\
execl.o       execle.o      execv.o       msgsys.o	\
semsys.o      shmsys.o      sigcall.o

POBJECTS=\
execl.p       execle.p      execv.p       msgsys.p	\
semsys.p      shmsys.p      sigcall.p

nonprof: $(OBJECTS)

prof: $(POBJECTS)

msgsys.o msgsys.p semsys.o semsys.p \
	shmsys.o shmsys.p: $(INCROOT)/usr/include/sys/ipc.h
msgsys.o msgsys.p: $(INCROOT)/usr/include/sys/msg.h
semsys.o semsys.p: $(INCROOT)/usr/include/sys/sem.h
shmsys.o shmsys.p: $(INCROOT)/usr/include/sys/shm.h
msgsys.o msgsys.p semsys.o semsys.p \
	shmsys.o shmsys.p: $(INCROOT)/usr/include/sys/types.h
sigcall.o sigcall.p: $(INCROOT)/usr/include/signal.h
sigcall.o sigcall.p: $(INCROOT)/usr/include/errno.h

.c.o .c.p:
	@echo $*.c:
	$(PROF)$(CC) -p $(CFLAGS) -c $*.c    && mv $(*F).o $*.p
	$(CC) $(CFLAGS) -c $*.c
