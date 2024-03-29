#   	@(#)bootvec.m4	2.1	
#
#	This module provides a few constants expected by the
#	standard Motorola rom based monitor boot command (bo).
#	It may also define a set of media dependent parameters.
#
# 	The bo command loads SECTORS contiguous sectors into memory
# 	starting with sector FSTSECT.
#
#-----------------------------------------------------------------------


# Constants
	set	FILL,0
	set	VID,0			# volume id
	set	FSTSECT,4		# first boot sector (256 byte)
	set	SECTORS,4		# boot size in sectors
	set	BOOTADR,0x30000		# position in memory to load boot
					# BOOTADR is ignored on the EXORmacs 

ifdef(`WD',
`
	set	MPAR,1			# first media parameter sector
	set	MPARLEN,1		# number of media parameter sectors

# media parameters for 15mb winchester
# or for 5 1/4 inch floppy double sided double density

	set	ATM,ifdef(`FL',0xf,0)		# attributes mask
	set	PRM,0x2d0	# parameters mask
	set	ATW,ifdef(`FL',0xf	,0x10)	# attributes word
	set	SPT,ifdef(`FL',0x10,0x20)	# sectors per track on media
	set	HDS,ifdef(`FL',2,6)		# number of heads on drive
	set	CYL,ifdef(`FL',0x50,0x132)	# number of cylinders on media
	set	ILV,1		# interleave factor on media
	set	SOF,0		# spiral offset on media
	set	PSS,0x100	# physical sector size on media
	set	SHD,0		# starting head number on drive
	set	CYLD,ifdef(`FL',0x50,0x132)	# number of cylinders on drive
	set	PCOM,ifdef(`FL',0x28,0x99)	# precompensation cyl no.(CYL/2)
	set	SPTD,ifdef(`FL',0x10,0	)	# sectors per track on drive
	set	SRTD,ifdef(`FL',1,0)		# stepping rate code on head
	set	RWCC,ifdef(`FL',0x28,0	)	# reduced write current cyl no.
	set	ECC,0		# Ecc data burst length
	set	DRSV,0		# reserved pointer to future blk

# The last five items above, SPTD-DRSV, may not be right for the winchester.
# It is unknown at this time who uses these and exactly what they should be.
# The winchester received for testing has zeros in these locations.
')dnl

# Info Expected by the VMEbug monitor boot command (bo)
# in the first 256 byte sector on the disk.

	data
	org	0
	long	VID
	long	FILL
	long	FILL
	long	FILL

	long	FILL
	long	FSTSECT			# first boot sector (256 byte)
	short	SECTORS			# boot size in sectors
	long	FILL
	long	BOOTADR			# position in memory to load boot

ifdef(`WD',
`
	org	0x90
	long	MPAR			# first media parameter sector
	byte	MPARLEN			# number of media parameter sectors
')dnl


	org	0xf8			# EXORmacs peculiarity
	byte	'E,'X,'O,'R,'M,'A,'C,'S

ifdef(`WD',
`
# Media parameters

	long	FILL			# defined under VERSAdos only
	short	ATM			# attributes mask
	short	PRM			# parameters mask
	short	ATW			# attributes word
	short	FILL			# defined under VERSAdos only
	long	FILL			# defined under VERSAdos only
	long	FILL			# defined under VERSAdos only
	long	FILL			# defined under VERSAdos only
	byte	SPT			# sectors per track on media
	byte	HDS			# number of heads on drive
	short	CYL			# number of cylinders on media
	byte	ILV			# interleave factor on media
	byte	SOF			# spiral offset on media
	short	PSS			# physical sector size on media
	short	SHD			# starting head number on drive
	short	CYLD			# number of cylinders on drive
	short	PCOM			# precompensation cyl no. (CYL/2)
	byte	SPTD			# sectors per track on drive
	byte	SRTD			# stepping rate code on head
	short	RWCC			# reduced write current cyl no.
	short	ECC			# Ecc data burst length
	short	DRSV			# reserved pointer to future block
')dnl

# Currently the remainder of the first 512 bytes are filled 
# with zero by post processing done on the a.out generated by this module.
# The space is reserved for disk and media parameters.
