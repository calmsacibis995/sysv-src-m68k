# @(#)dump.m4	2.1	
	set	HIGH,0x2700	# disable all interrupts
	data
	even
	global	_sdata
_sdata:				# start of data
	space	100*4		# seperate stack for dumps
	global	dumpstk
dumpstk: 
	space	16*4		# registers %d0-%d7, %a0-%a7
	space	4		# status register
	space	4		# user stack pointer
ifdef(`M68000',,
`	space	4		# vector base register
')dnl
	text
# Produce a core image dump
	global	dump%
dump%:
	movm.l	&0xffff,dumpstk	# save registers
	mov.l	&dumpstk,%sp	# reinit stack
	mov.w	%sr,%d0
	mov.w	&HIGH,%sr	# disable interrupts
	lea	16*4(%sp),%a1
	mov.l	%d0,(%a1)+	# status register
	mov.l	%usp,%a0
	mov.l	%a0,(%a1)+	# user stack pointer
ifdef(`M68000',,
`	mov.l	%vbr,%a0
	mov.l	%a0,(%a1)+	# vector base register
')dnl
	mov.l	dump,%a0	# get address of configured dump routine
	jsr	(%a0)
L%dump:	stop	&HIGH
	bra.b	L%dump
