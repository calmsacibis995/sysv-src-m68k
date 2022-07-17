#	@(#)misc.m4	2.1.1.1	

# This file contains miscellaneous routines callable from C

	text

# addupc - increments profile counter for a given user pc

# addupc (pc, profptr, incr)
# unsigned pc;		/*  8(%fp) */
# struct *profptr;	/* 12(%fp) */
# unsigned incr;	/* 16(%fp) */

# struct {			/* profile arguments */
# 	short	*pr_base;	/*   (%a2) buffer base */
# 	unsigned pr_size;	/*  4(%a2) buffer size */
# 	unsigned pr_off;	/*  8(%a2) pc offset */
# 	unsigned pr_scale;	/* 12(%a2) pc scaling */
# } u_prof;

addupc:	global	addupc
	link	%fp,&-12
	movm.l	&0x0404,-8(%fp)	# save %a2 and %d2
	mov.l	12(%fp),%a2	# profptr (&u.u_prof)
	mov.l	8(%fp),%d0	# pc
	sub.l	8(%a2),%d0	# corrected pc
	blo.b	L%addret
	mov.w	14(%a2),%d2	# scale
	cmp.w	%d2,&2
	blo.b	L%addret	# scale <  2 means profiling turned off
	beq.b	L%add0		# scale == 2 means map to 0
	not.w	%d2		# 0 bits now mean no change
	mov.l	%d0,%d1
L%addl1:			# scale loop
	lsr.l	&1,%d1
	add.w	%d2,%d2
	bcc.b	L%addl2
	sub.l	%d1,%d0
L%addl2:
	tst.w	%d2		# loop until no more changes
	bne.b	L%addl1
	add.l	&1,%d0		# force even
	bclr	&0,%d0
L%addcmp:
	cmp.l	%d0,4(%a2)	# pr_size
	bhs.b	L%addret
	mov.l	(%a2),%a0	# pr_base
	add.l	%d0,%a0		# base
	mov.l	&0,%d0
	mov.l	%d0,(%sp)	# B_WRITE
	mov.l	&2,%d0
	mov.l	%d0,-(%sp)	# sizeof(short)
	mov.l	%a0,-(%sp)	# base
	jsr	mmumap		# mmumap(base, sizeof(short), B_WRITE)
	mov.l	%a0,%d0
	beq.b	L%adderr
	mov.w	18(%fp),%d0	# pr_incr
	add.w	%d0,(%a0)
L%addret:
	movm.l	-8(%fp),&0x0404	# restore %a2 and %d2
	unlk	%fp
	rts
L%adderr:
	clr.w	14(%a2)		# stop profiling
	bra.b	L%addret
L%add0:
	mov.l	&0,%d0		# force 0
	bra.b	L%addcmp

define(	SPL,
`$1:	global	$1		# set ipl to $2
	mov.w	%sr,%d0		# return current ps value
	mov.w	`&0x2'$2`00',%sr	# set new priority level
	rts
')

SPL(spl0, 0)
SPL(spl1, 1)
SPL(spl2, 2)
SPL(spl3, 3)
SPL(spl4, 4)
SPL(spl5, 5)
SPL(spl6, 6)
splhi:	global	splhi
SPL(spl7, 7)

splx:	global	splx		# set ipl to input argument value
	mov.w	%sr,%d0		# return current ps value
	mov.w	6(%sp),%sr	# set new priority level from argument
	rts

	set	OKINTS,0x2000	# enable interrupts
# idle()

idle:	global	idle
	mov.w	&OKINTS,%sr	# enable interrupts
	tas.b	idleflag	# test & set idle escape flag
	beq.b	L%wloc		# skip if escaping the wait
	stop	&OKINTS		# stop & wait for an interrupt
L%wloc:	rts

	data
idleflag:	global	idleflag
	byte	1		# initialize idleflag to idle
	even
waitloc:	global	waitloc
	long	L%wloc
	text

	set	NS,ifdef(`M68000',3,2)	# shift to get nbr of long moves/clears

# bcopy(from, to, bytes)
#	"to" and "from" are both system physical addresses

	global	bcopy
bcopy:	mov.l	4(%sp),%a0	# from
	mov.l	8(%sp),%a1	# to
	mov.l	12(%sp),%d1	# number of bytes
	ble.b	L%done
	mov.w	%a0,%d0		# check alignment
	add.w	%a1,%d0
	lsr.w	&1,%d0
	bcs.b	L%cb3		# branch if must use byte moves
	mov.w	%a0,%d0		# word align addresses
	lsr.w	&1,%d0
	bcc.b	L%cl1		# branch to use long moves
	mov.b	(%a0)+,(%a1)+	# move one byte to get to even address
	sub.l	&1,%d1		# decrement count by one byte
L%cl1:	mov.l	%d1,%d0		# save count
	lsr.l	&NS,%d1		# adjust count for long loop
ifdef(`M68000',
`	bcs.b	L%cl4		# branch if long word left over
')dnl
	bra.b	L%cl5

L%cl2:	swap	%d1		# outer dbra loop
L%cl3:	mov.l	(%a0)+,(%a1)+	# inner dbra loop: move longs
ifdef(`M68000',
`L%cl4:	mov.l	(%a0)+,(%a1)+
')dnl
L%cl5:	dbra	%d1,L%cl3
	swap	%d1
	dbra	%d1,L%cl2

	btst	&1,%d0
	beq.b	L%cl6
	mov.w	(%a0)+,(%a1)+	# move last word
L%cl6:	lsr.w	&1,%d0
	bcc.b	L%done
	mov.b	(%a0)+,(%a1)+	# move last byte
L%done:	mov.l	&0,%d0		# return (0)
	rts

L%cb1:	swap	%d1		# outer dbra loop
L%cb2:	mov.b	(%a0)+,(%a1)+	# inner dbra loop: move bytes
L%cb3:	dbra	%d1,L%cb2
	swap	%d1
	dbra	%d1,L%cb1
	bra.b	L%done


# bzero(addr, size)
#	addr is a physical address
#	size is in bytes

	global	bzero
bzero:	mov.l	4(%sp),%a0	# address
	mov.l	8(%sp),%d1	# number of bytes
	ble.b	L%done
	sub.l	%a1,%a1		# constant zero
	mov.w	%a0,%d0
	lsr.w	&1,%d0		# make sure address is even
	bcc.b	L%zl1		# branch to use long clears
	clr.b	(%a0)+		# clear one byte to get to even address
	sub.l	&1,%d1		# decrement count by one byte
L%zl1:	mov.l	%d1,%d0		# save count
	lsr.l	&NS,%d1		# adjust count for long loop
ifdef(`M68000',
`	bcs.b	L%zl4		# branch if long word left over
')dnl
	bra.b	L%zl5

L%zl2:	swap	%d1		# outer dbra loop
L%zl3:	mov.l	%a1,(%a0)+	# inner dbra loop: clear longs
ifdef(`M68000',
`L%zl4:	mov.l	%a1,(%a0)+
')dnl
L%zl5:	dbra	%d1,L%zl3
	swap	%d1
	dbra	%d1,L%zl2

	btst	&1,%d0
	beq.b	L%zb1
	mov.w	%a1,(%a0)+	# zero last word
L%zb1:	lsr.w	&1,%d0
	bcc.b	L%done
	clr.b	(%a0)+		# zero last byte
	bra.b	L%done



# caddr_t bsmear( addr, nbytes )

# set nbytes of ram to a stable state, starting at addr,
# until a bus error occurs, or nbytes are done.
# nbytes should be a multiple of INCR.

# return the ending address: either the address on which a fault occured
# (or slightly more than that), or addr + nbytes.

# some rams can come up with parity errors.
# each byte must be written before it # is read.
# a tst or clr instruction can fail even though the memory exists.
# hence, this routine actually writes all bytes until an error is met.

	set	NOINTS,0x2700	# sr mask: supv mode, ipl 7
	set	INCR,64		# number of bytes cleared at a time
	set	L2INCR,6	# log base 2 of INCR
	set	STKSIZ,ifdef(`M68000',7,29)*2	# bus error stack size

bsmear:	global	bsmear
	mov.l	4(%sp),%a0	# addr
	mov.l	8(%sp),%d0	# nbytes
	lsr.l	&L2INCR,%d0	# nbytes / INCR
	mov.w	&INCR,%a1	# constant INCR
	mov.w	%sr,%d1 	# save status register
	mov.w	&NOINTS,%sr	# block interrupts
	mov.l	M68Kvec+8,-(%sp)	# save current bus error handler
	mov.l	&L%bs9,M68Kvec+8	# catch bus error exceptions
	bra.b	L%bs3		# go start the loop

L%bs1:	swap	%d0		# outer dbra loop
L%bs2:	movm.l	&0xffff,(%a0)	# inner dbra loop: smear INCR bytes
	add.l	%a1,%a0
L%bs3:	dbra	%d0,L%bs2
	swap	%d0
	dbra	%d0,L%bs1

L%bs4:	mov.l	(%sp)+,M68Kvec+8	# restore bus error handler
	mov.w	%d1,%sr		# restore status register
	mov.l	%a0,%d0		# return pointer in both registers
	rts

# L%bs9 can be reached only by a bus error exception trap

L%bs9:	add.w	&STKSIZ,%sp	# throw away everything on the stack
	bra.b	L%bs4



# int bprobe( addr, byte )
# caddr_t addr;
# int byte;

# if "byte" < 0, read the byte at "addr" to see if a bus error occurs.
# else write (byte) "byte" into address "addr" and see if a bus error occurs.

# return true (1) if the error occurs, else false (0).

bprobe:	global	bprobe
	mov.l	4(%sp),%a0	# "addr"
	mov.w	%sr,%d1 	# save status register
	mov.w	&NOINTS,%sr	# block interrupts
	mov.l	M68Kvec+8,%a1	# save current bus error handler
	mov.l	&L%bpr9,M68Kvec+8	# catch bus error exceptions
	mov.l	8(%sp),%d0	# get the int "byte"
	bge.b	L%bpr1		# skip for write
	tst.b	(%a0)		# read (byte) "byte" at "addr"
	bra.b	L%bpr2
L%bpr1:	mov.b	%d0,(%a0)	# write (byte) "byte" into "addr"
L%bpr2:	mov.l	&0,%d0		# false return value
L%bpr3:	mov.l	%a1,M68Kvec+8	# restore bus error handler
	mov.w	%d1,%sr		# restore status register
	rts

# L%bpr9 can be reached only by a bus error exception trap

L%bpr9:	mov.l	&1,%d0		# true return value
	add.w	&STKSIZ,%sp	# throw away everything on the stack
	bra.b	L%bpr3
ifdef(`MEXOR',
`
# special testing for MACSbug software abort

	set	SWABBIT,0xfee007	# software abort bit on pia
	set	BUGSWAB,0xfe8808	# MACSBUG software abort entry
swabort:	global	swabort
	btst	&7,SWABBIT
	bne.b	L%swab
	rts
L%swab:	add.w	&4+60+6,%sp	# go back to the original interrupt state
	jmp	BUGSWAB		# enter MACSbug
')dnl
