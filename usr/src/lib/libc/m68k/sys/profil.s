#	@(#)profil.s	2.1 
# profil

	file	"profil.s"
	set	prof%,44
	global	profil

profil:
	MCOUNT
	mov.l	&prof%,%d0
	trap	&0
	rts
