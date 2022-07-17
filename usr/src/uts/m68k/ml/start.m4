#	@(#)start.m4	2.1	

# The code in this file is executed at system bootstrap.

	set	GRAIN,512		# 1 core click granularity
	set	ROUND,GRAIN-1		# granularity round
	set	GMASK,-GRAIN		# granularity mask
	set	U_SIZE,2048		# user area for proc 0
	set	HIPRI,0x2700		# supervisor state, ipl 7
	set	LOPRI,0x2000		# supervisor state, ipl 0

	data	# these variables can't be in bss space
	global	physmem,maxmem,locore
physmem:	long	0		# total memory
maxmem:		long	0		# available memory
locore:		long	0		# begining of available memory

	text
_start:	global	_start
	mov.w	&HIPRI,%sr		# supervisor state, ipl 7
	mov.l	&u+U_SIZE,%sp		# system stack at top of u area
ifdef(`M68000',,
`	mov.l	&M68Kvec,%d1		# set vector base register
	mov.l	%d1,%vbr
')dnl
	tst.w	%d0			# check for valid boot device
	bmi.b	L%strt
	mov.l	&sysdevs,%a0		# get address of system devices table
	mov.w	(%a0),%d1		# get nbr of system devices (or -1)
	mov.w	&-1,(%a0)+		# force single execution only
	cmp.w	%d0,%d1
	bge.b	L%strt			# bra if either already switched or none
	lsl.w	&4,%d0			# 16 bytes per device
	add.w	%d0,%a0
	mov.w	(%a0)+,rootdev		# move system device info into unix
	mov.w	(%a0)+,pipedev
	mov.w	(%a0)+,dumpdev
	mov.w	(%a0)+,swapdev
	mov.l	(%a0)+,swplo
	mov.l	(%a0),nswap

L%strt:	mov.l	&end+ROUND,%d0		# top of unix
	and.l	&GMASK,%d0
	mov.l	%d0,locore		# save as start of memory
	add.l	&U_SIZE,%d0		# for proc 0 user area

	mov.l	&edata,%a0		# zero bss and u area
	mov.l	&0,%d7			# constant 0
L%clr:	mov.l	%d7,(%a0)+		# assumes edata is long aligned
	cmp.l	%a0,%d0
	blt.b	L%clr

	jsr	mmuinit			# mmuinit()
ifdef(`MEXOR',
`
# set the EXORmacs front panel to C0:  operating system is executing.
# see EXORMACS Operations Manual M68KMACS(D9), Table 1-2, Page 1-13.
	set	DISPLAY,0xfe0005	# display address
	set	MSD_C0,0x13		# strobe MSD with ~C
	set	LSD_C0,0x2f		# strobe LSD with ~0
	set	LATCH,0x3f		# reset strobes
	mov.l	&DISPLAY,%a0
	mov.b	&MSD_C0,(%a0)		# set msd to C
	mov.b	&LSD_C0,(%a0)		# set lsd to 0
	mov.b	&LATCH,(%a0)		# reset strobes

')dnl
	jsr	main			# main()
	mov.l	%d0,%a0			# get address to jump to
	bne.b	L%2nd			# start 2nd proc 0
					# 1st proc 0 returns here
	clr.l	-(%sp)			# set pc to 0
ifdef(`M68000',
`	clr.w	-(%sp)			# set sr to user mode
',`	clr.l	-(%sp)			# set sr to user mode, format to 0
')	rte				# execute user process

L%2nd:	mov.w	&LOPRI,%sr		# supervisor state, ipl 0
	jsr	(%a0)			# jump to 2nd proc 0
L%die:	bra.b	L%die			# never return here
