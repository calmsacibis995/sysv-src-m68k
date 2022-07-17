# @(#)fscv.mk	2.1	
#
#	fscv.mk: makefile for fscv
#
#	This makefile supports the creation of two
#	fscv(1) utilities:
#
#	1. The standard System V fscv(1) to convert between
#		pdp11 and vax.
#	2. The System V, M68000 fscv(1) to convert between
#		the m68k family and vax.
#
#
#	The source files are organized as follows:
#
#	.../cmd/fscv/pdp11:	contains pdp11<->vax files only
#	.../cmd/fscv/vax:	contains the common (vax) files
#	.../cmd/fscv/m68k:	contains m68k<->vax files only
#
#

all:	fscvm

fscvp:	pdp11/pdp11.mk
	(cd pdp11;make -f pdp11.mk)
	rm -f fscvp
	ln pdp11/fscv fscvp

fscvm:	m68k/m68k.mk
	(cd m68k;make -f m68k.mk)
	rm -f fscvm
	ln m68k/fscv fscvm

install:  all
	cd m68k; make -f m68k.mk install
