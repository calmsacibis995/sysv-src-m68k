#	 @(#)boots.mk	2.1.1.1	

UDDEV = /dev/rrp3
WDDEV = /dev/rrp5

wdall: wdboot wdlist
wfall: wfboot wflist
udall: udboot udlist

# winchester disk bootloader for RWIN1 controller
wdboot: wdboot.s wdbvec
	as -o wdboot.o wdboot.s
	ld wdboot.o
	nm -x a.out >wdboot.nm
	cvout -s a.out wdboot.v #convert from vax to 68K

# floppy disk bootloader for RWIN1 controller
wfboot: wdboot.s wfbvec
	as -o wdboot.o wdboot.s
	ld wdboot.o
	nm -x a.out >wdboot.nm
	cvout -s a.out wdboot.v #convert from vax to 68K

# disk bootloader for M68KVM21 universal disk controller
udboot: udboot.s udbvec
	as -o udboot.o udboot.s
	ld udboot.o
	nm -x a.out >udboot.nm
	cvout -s a.out udboot.v #convert from vax to 68K

# boot vectors and winchester disk media parameters for RWIN1 controller 
wdbvec: bootvec.m4
	m4 -DWD bootvec.m4 >wdbvec.s
	as -o wdbvec.o wdbvec.s
	ld wdbvec.o
	nm -x a.out >wdbvec.nm
	cvout -s a.out wdbvec.v #convert from vax to 68K

# boot vectors and floppy disk media parameters for RWIN1 controller 
wfbvec: bootvec.m4
	m4 -DWD -DFL bootvec.m4 >wfbvec.s
	as -o wfbvec.o wfbvec.s
	ld wfbvec.o
	nm -x a.out >wfbvec.nm
	cvout -s a.out wfbvec.v #convert from vax to 68K

# boot vectors for M68KVM21 universal disk controller
udbvec: bootvec.m4
	m4 -DUD bootvec.m4 >udbvec.s
	as -o udbvec.o udbvec.s
	ld udbvec.o
	nm -x a.out >udbvec.nm
	cvout -s a.out udbvec.v #convert from vax to 68K

wdinstall:
	dd if=wdboot.v of=$(WDDEV) seek=2 count=2
	dd if=wdbvec.v of=$(WDDEV) count=1

wfinstall:
	dd if=wdboot.v of=$(WDDEV) seek=2 count=2
	dd if=wfbvec.v of=$(WDDEV) count=1

udinstall:
	dd if=udboot.v of=$(UDDEV) seek=2 count=2
	dd if=udbvec.v of=$(UDDEV) count=1

wdlist:
	xd  wdboot.v >odout
	xd -c wdbvec.v >>odout
	pr wdboot.s wdbvec.s odout wd*.nm|lp

wflist:
	xd  wdboot.v >odout
	xd -c wfbvec.v >>odout
	pr wdboot.s wfbvec.s odout wf*.nm|lp

udlist:
	xd  udboot.v >odout
	xd -c udbvec.v >>odout
	pr udboot.s udbvec.s odout ud*.nm|lp

clean:
	rm -f a.out odout [wu][df]*.o *bvec.o

clobber:	clean
	rm -f [wu][df]*.v [wu][df]*.nm
