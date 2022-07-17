#	@(#)ml.mk	2.13.1.1	

MACH = M68000
TYPE = MEXOR

MFLAGS = -D$(MACH) -D$(TYPE)
FRC =

MFILES = start.m4 cswitch.m4 dump.m4 math.m4 misc.m4 end.m4


../locore.o:	$(MFILES) $(FRC)
	-rm -f locore.s ../locore.o
	m4 $(MFLAGS) $(MFILES) > locore.s
	$(AS) -o ../locore.o locore.s

clean:
	-rm -f locore.s *.o

clobber:	clean
	-rm -f $(LIBNAME) ../locore.o

save:	../locore.o locore.s
	rm -f locore.o.bak locore.s.bak
	cp ../locore.o locore.o.bak
	cp locore.s locore.s.bak

FRC:
