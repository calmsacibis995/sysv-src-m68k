#	@(#)cerror.s	2.1 
# C return sequence which
# sets errno, returns -1.

	file	"cerror.s"
	global	cerror%
	global	errno

	data
errno:	long	0

	text
cerror%:
	mov.l	%d0,errno
	mov.l	&-1,%d0
	mov.l	%d0,%a0
	rts
