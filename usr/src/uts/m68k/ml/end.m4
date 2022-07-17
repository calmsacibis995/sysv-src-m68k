#	@(#)end.m4	2.1	

# This	is the code executed by proc(1) to load /etc/init.
# The 1.1 level was derived from the 68000 end.s level 1.4.

	data
	global	cputype
cputype:
	short	ifdef(`M68000',680,ifdef(`M68010',681,ifdef(`M68020',682,*)))
	global	icode		# location of boot code
	global	szicode		# size of icode in bytes

# define user area virtual address

	set	usize,4		# size of user area, in pages
	comm	u,usize*512

# Bootstrap program executed in user mode
# to bring up the system.

	set	exec%,11

icode:	mov.l	&I_stack,%sp
	mov.l	&exec%,%d0
	trap	&0			# exec( "/etc/init" );
L%here:	bra.b	L%here

L%arg:	long	I_ifile
	long	0
L%ifile:
	byte	'/,'e,'t,'c,'/,'i,'n,'i,'t,0
	even
L%stack:
	long	0
	long	I_ifile
	long	L%arg-icode
szicode:
	long	szicode-icode		# size of init code

	set	I_stack,L%stack-icode
	set	I_ifile,L%ifile-icode
