#	@(#)cswitch.m4	2.1	

# This file contains the routines used for context switching on UNIX V/68.
# The 1.1 level was derived from the 68000 cswitch.s level 1.3.

#	The routines contained here are:

	global	setjmp		# set up environment for external goto
	global	longjmp		# non-local goto
	global	save		# saves register sets
	global	resume		# restores register sets & u, then longjmps

	set	NB,ifdef(`M68000',8,4)	# number of bytes in a single move
	set	U_SIZE,2048/NB	# number of NB-bytes in the static u area
	set	HIGHPRI,0x2700	# supervisor state priority 7
	set	REGMASK,0xfefc	# save/restore a7-a1,d7-d2


# setjmp (save_area)
# save (save_area)
#	saves registers and return location in save_area


	text
setjmp:
save:
	mov.l	(%sp)+,%a1	# pop return address of calling routine into %a1
	mov.l	(%sp),%a0	# address of save area
	movm.l	&REGMASK,(%a0)	# save a7-a1,d7-d2
	mov.l	&0,%d0		# return(0)
	jmp	(%a1)


# longjmp (save_area)
#	resets registers from save_area and returns to the location
#	from WHERE setjmp() WAS CALLED.
#	** Never returns 0 - this lets the returned-to environment know that the
#	return was the result of a long jump.

longjmp:
	mov.l	4(%sp),%a0	# get address of save_area
	movm.l	(%a0),&REGMASK	# restore a7-a1,d7-d2
	mov.l	&1,%d0		# return(1)
	jmp	(%a1)



# resume (&u, save_area)
#	switches to another process's "u" area. Returns non-zero.

# This section is unique to machines like the EXORmacs which have no
# mapping in the supervisor state.  The current process's "u" area has been
# saved by a call to the mmu driver.  This section copies the new "u"
# area into the global structure "u".  On other machines, this section would
# be replaced by the necessary code to map the new "u" area to the system
# "u" window

resume:
	mov.l	4(%sp),%a1	# address of new "u" area
	mov.l	8(%sp),%a0	# address of save_area
	mov.l	&u,%a2		# address of system "u"
	mov.w	&U_SIZE-1,%d1	# size to copy
	mov.w	%sr,%d0		# save current PS
	mov.w	&HIGHPRI,%sr	# inhibit interrupts

# copy the "u" structure and the stack
L%mv_u:
	mov.l	(%a1)+,(%a2)+	# "u" is long word aligned
ifdef(`M68000',dnl		# allow loop-mode if not 68000
`	mov.l	(%a1)+,(%a2)+
')dnl
	dbra	%d1,L%mv_u

	movm.l	(%a0),&REGMASK	# restore previous a7-a1,d7-d2
	mov.w	%d0,%sr		# restore priority
	mov.l	&1,%d0		# return (1)
	jmp	(%a1)
