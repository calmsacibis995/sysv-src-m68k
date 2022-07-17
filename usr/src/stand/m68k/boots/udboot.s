#   @(#)udboot.s	2.1 
#
#	This is the system bootloader.  It implements the functionality
#	specified in the System V/68 General Disk Bootloader specification
#	with the following exceptions:
#
#	1) The MACSbug software abort functionality is supported for
#	debugging purposes.
#
#	2) An object file name may be passed from the MACSbug boot
#	command to the bootloader. Either a full path name or a path name
#	which assumes the present working directory is root, may be used.
#
#	3) The default object file need not be in the root directory
#	if its path is specified as follows:
#		[<up to 13 chars>,0 ]...<up to 13 chars> 0,0
#	For example:  name: byte 's,'t,'a,'n,'d,0,'s,'a,'s,'h,0,0
#	might be used to specify the standalone shell.
#
#	4) The drive number and controller number being booted from 
#	are passed to the operating system.
#
#	This revision is compatible with the SGS2 a.out file format.
#	It expects to be placed between the superblock and the inodes
#	and requires bootvec1.s to provide the information the EXORmacs
#	monitor boot command needs to find and load this module.
#
#  Bootloader Algorithm
#
#	 Relocated boot to high memory
#	 Fetch object file name
#	 Repeat while components left in search path
#	    Execute get_inode for directory 
#	    Repeat while no match && blocks left in directory
#	       Execute read_next_block to get directory data block
#	       Repeat while no match && not out of directory entries
#	          If inode number is null
#		     error terminate
#	          Compare name to directory entry
#	       end repeat
#	    end repeat
#	    If no match 
#	       error terminate
#	 end repeat
#	 Execute get_inode for load module
#	 Read header into buffer
#	 If magic is MC68MAGIC
#	    Move BLKSIZ-HDRSIZ bytes from buffer to text_start
#	    Load remaining blocks into memory
#	    If data_start != textsize + text_start
#	       move datasize bytes into position at data_start
#	    Set entry address from header
#	 Else
#	    Load all blocks into memory beginning at address 0
#	    Set entry address to 0
#	 Jump to entry address
#
#-----------------------------------------------------------------------

# Conventions & Assumptions
#
#	Block numbers run from 0-n on the disk device.
#	Inode numbers run from 1-n beginning in block 1.
#
# Register conventions:
#	a6		- frame pointer
#	a5		- buffer pointer
#	a4		- target name pointer
#	a3		- directory entry pointer
#	d7		- long block number
#	d6		- long current inode number
#	d5		- long file block number
#	d0-d4,a0-a2	- scratch registers

# Data Structures
	set	BLKSIZ,1024		# no. bytes/block
	set	HDRSIZ,168		# bytes in header
	set	MC68MAGIC,0520		# expected file type
	set	BOOTSIZ,1024
	set	INOSIZ,64		# no. bytes/inode entry
	set	INOBLK,BLKSIZ/INOSIZ	# no. inodes/disc block
	set	INOMSK,0xf		# inode to block mask
	set	NAMSIZ,14		# no. bytes in name field of dir entry
	set	DIRSIZ,16		# size of directory entry, bytes
	set	ROOTINO,2		# root dir inode no.
	set	PATHLIM,18		# char limit for specified path
	set	FILL,0
# Inode structure
	set	inode,0
	set	mode,inode
	set	nlink,mode+2
	set	uid,nlink+2
	set	gid,uid+2
	set	size,gid+2
	set	faddr,size+4
	set	time,faddr+40
# Header structure
	set	filehdr,0
	set	f_magic,filehdr		# long magic number
	set	tsize,f_magic+24	# long size of text
	set	dsize,tsize+4		# long size of data
	set	entry,dsize+8		# long entry point
	set	text_start,entry+4	# long base of text
	set	data_start,text_start+4	# long base of data
					# other header info not used
# EXORmacs Specific Constants
	set	HIGH,0x30000
	set	ILLEGAL,0xfe9e6e	# illegal instruction service address
	set	SWABORT,0xfe8808	# software abort service address
	set	DISPLY,0xfe0004		# front panel hex display address
	set	LNIB,0x20		# strobe 0xF into display low nibble
	set	HNIB,0x14		# strobe 0xB into high nibble & latch lo
	set	LATCH,0x3f		# latch both high and low nibble

# Universal Disk Constants		(udc = universal disk controller)
	set	DISK,0xff0000		# udc location (controller 0)
	set	OPC,0x200		# offset per controller
	set	DELAY,0x200		# inter command delay count
	set	CIO,0x100		# command I/O area
	set	EPKT,0x2d		# end of packet offset
	set	INT,0xd			# udc interrupt
	set	FLTO,0x101		# message to udc flag
	set	FLFR,0x181		# message from udc flag	
	set	ANTO,0x103		# ACK/NAK to udc	
	set	ANFR,0x183		# ACK/NAK from udc	
	set	SSTX,0x185		# status packet STX
	set	SPLN,0x189		# status packet length field
	set	STAT,0x193		# command status field
	set	SETX,0x1a9		# status packet ETX
	set	ACK,0x6
	set	NAK,0x15
	set	FLAG,0x80
	set	CLR,0
	set	STX,0x2
	set	PID,1			# process id (space holder only)
	set	PSIZE,0x15		# command packet size
	set	CMD,0x1001		# 10 = read, 01 = with retry
	set	SBPS,0x40100		# 4 = 4 sectors, 100 = 256 bytes/sector
	set	ETX,0x3
	set	SPLEN,0x13		# status packet length
	set	SUM,0x18		# checksum of STX + SPLEN + ETX

# Other Constants, Variables and Offsets 
	set	STACK,HIGH
	set	BUF,HIGH+BOOTSIZ
	set	T,BOOTSIZ+BLKSIZ	# Text offset from frame pointer
	set	NT,4*266		# blkno[null terminator]


# Bootloader Entry Info Expected by MACSbug

	org	0
boot:
	long	STACK			# initial stack pointer
	long	boot1			# bootloader entry

# Two vectors used by MACSbug for breakpoints, tracing and software
# abort are included for debugging purposes.

	org	0x10
	long	ILLEGAL			# illegal instruction vector

# Default load module name

name:	byte	's,'t,'a,'n,'d,0
	byte	'u,'n,'i,'x,0,0

	org	0x24
	long	SWABORT			# software abort vector

# Disk I/O image

pkt:	byte	FLAG			# message-to-udc flag
	byte	CLR			# ACK/NAK mail box
	byte	STX
	byte	PID
	byte	PSIZE			# packet size from STX to ETX
dev:	byte	CLR
	short	CMD			# read with retry
	long	SBPS			# four 256 byte sectors
baddr:	long	BUF			# buffer address
	short	FILL			# not used
sect:	long	CLR			# starting sector
epkt:	byte	ETX
ctlr:	byte	CLR			# controller number
	even

# Bootloader entry

# Move boot to high memory without destroying d0, d1, a5 & a6

boot1:
	mov.l	%a5,%a3			# save name pointer
	mov.l	%a6,%a4			# save end of name pointer
	sub.l	%a0,%a0			# source pointer
	mov.l	&HIGH,%a5		# destination pointer
	mov.l	&BOOTSIZ/4-1,%d2	# longs to move (-1 for dbra)
boot2:	mov.l	(%a0)+,(%a5)+
	dbra	%d2,boot2		# move them all
	jmp	bootup+HIGH		# go to high memory

# Code beyond this point expects to run in high memory.
#	a5 is left pointing to the buffer area

bootup:
	
	lea.l	BLKSIZ(%a5),%a6		# Set frame pointer above buffer
	mov.b	%d0,dev-T(%a6)		# store drive number
	mov.b	%d1,ctlr-T(%a6)		# Save controller number

# Get object file name if passed from boot command

	cmp.l	%a3,%a4			# if default name is OK
	beq.b	boot8			#    go get started
	cmp.b	(%a3),&'/		# skip leading /
	bne.b	boot3
	add.l	&1,%a3
boot3:
	mov.w	&PATHLIM-1,%d0		# move up to PATHLIM chars (-1 for dbcc)
	lea.l	name+HIGH,%a0		# point to search key area

boot4:
	mov.b	(%a3)+,%d1		# get character
	cmp.b	%d1,&'/			# change / to null
	bne.b	boot5
	clr.b	%d1
boot5:
	cmp.b	%d1,&'A			# map UPPER to lower
	blo.b	boot6
	cmp.b	%d1,&'Z
	bhi.b	boot6
	add.b	&'a-'A,%d1
boot6:
	mov.b	%d1,(%a0)+		# save this one
	cmp.l	%a3, %a4		# until all are fetched
	dbhs	%d0, boot4		#    || count run out

	clr.b	(%a0)+			# double null terminate path
	clr.b	(%a0)

boot8:
	mov.l	&ROOTINO,%d6		# inode for root directory (inode 2)
	lea.l	name+HIGH,%a4		# Point to search key

# Repeat while components left in search path

boot10:	bsr	iget			# Get_inode for directory
	clr.l	%d5			# Clear file_block_number

#    Repeat while no match && blocks left in directory

boot11:	tst.l	0(%a6,%d5.l)		# If blkno[file_block_no] is null
err11:	beq.b	err11			#    out of blocks & out of luck
	bsr	rdnblk			# read_next_block to get directory data block
	mov.l	%a5,%a3			# reset directory entry pointer

#       Repeat while no match && not out of directory entries

boot12:	cmp.l	%a3,%a6			# If entry pointer beyond buffer
	bhs.b	boot11			#    go try another block
	mov.w	(%a3)+,%d6		# Get inode number
	mov.l	%a3,%a0			# Point to directory entry
	add.l	&NAMSIZ,%a3
	tst.w	%d6			# If inode number is null
	beq.b	boot12			#    go try another entry

#          Compare name to directory entry

	mov.w	&NAMSIZ-1,%d0		# Check up to NAMSIZ chars (-1 for dbcc)
	mov.l	%a4,%a1			# Point to start of current component
boot13:	cmp.b	(%a0)+,(%a1)+		# If characters not equal
	bne.b	boot12			#    go try another entry
	tst.b	-1(%a1)			# If null terminator found
boot14:	dbeq	%d0,boot13		#    || count run out
					#         drop thru we have a match

#       end repeat
#    end repeat
# until end of path is reached

	ext.l	%d6			# Make inode number long
	mov.l	%a1,%a4			# Advance current component ptr
	tst.b	(%a4)			# Check for second null terminator
	bne.b	boot10			#   at end of path

# Found the object module

	bsr	iget			# Get_inode for load module
	clr.l	%d5			# Set file_block_number to 0
	bsr	rdnblk			# Get first block
	cmp.w	f_magic(%a5),&MC68MAGIC	# If magic number OK
	beq.b	boot30			#    go use header info

# Assume image with entry at location 0

	clr.l	%d5			# Reset file_block_number to 0
	sub.l	%a5,%a5			# Set bufptr to location zero
	bsr.b	load			# Load all blocks into memory
	sub.l	%a2,%a2			# Set entry address to loc 0
	bra.b	boot50			# Go execute image

# Magic number recognized, use header info

boot30:	# Move all except the header to text_start

	mov.w	&(BLKSIZ-HDRSIZ)/4-1,%d0 # longs to move (-1 for dbra)
	mov.l	text_start(%a5),%a0	# to text_start
	lea.l	HDRSIZ(%a5),%a1		# from buffer

boot31:	mov.l	(%a1)+,(%a0)+		# move them all
	dbra	%d0,boot31

	# move remaining blocks into memory

	mov.l	%a5,%a4			# save buffer ptr
	mov.l	%a0,%a5			# starting where we left off
	bsr.b	load			# load remaining blocks
	mov.l	%a4,%a5			# restore buffer ptr

	mov.l	text_start(%a5),%a0	# if text_start + textsize
	add.l	tsize(%a5),%a0
	mov.l	data_start(%a5),%a1
	cmp.l	%a0,%a1			#  == data_start
	beq.b	boot37			#    go skip data movement

	# move data into position

	mov.l	dsize(%a5),%d0
	add.l	%d0,%a0			# from text_start + tsize + dsize
	add.l	%d0,%a1			#   to data_start + dsize
	lsr.l	&2,%d0			# dsize/4 longs to move
	sub.w	&1,%d0			#    (-1 for dbra)
boot35:	mov.l	-(%a0),-(%a1)		# in reverse order
	dbra	%d0,boot35

boot37:	mov.l	entry(%a5),%a2		# get entry address
	
# Change front panel display (EXORmacs only)
# Sets display to show code 0xBF, operating system initialization in progress

boot50:

	lea.l	DISPLY,%a0		# Point to display control register
	mov.b	&LNIB,(%a0)		# strobe low nibble 
	mov.b	&HNIB,(%a0)		# strobe high nibble & latch low nibble
	mov.b	&LATCH,(%a0)		# latch high nibble

# pass drive and controller number to operating system

	clr.l	%d0
	mov.b	dev-T(%a6),%d0		# boot device drive number
	clr.l	%d1
	mov.b	ctlr-T(%a6),%d1		# controller number

# Jump to entry address

	jmp	(%a2)			# Jump to entry address


# Subroutine load
#
# Loads file blocks specified by array blkno[] starting with block
# blkno[file_block_no], into successive locations in memory beginning
# at start_loc.
#		d5 = long file_block_no
#		a5 = start_loc
#		d5, a5 modified
#

load:					# Repeat while not out of blocks
	bsr	rdnblk			#    Execute read_next_block 
	tst.l	0(%a6,%d5.l)
	beq.b	load1
	add.l	&BLKSIZ,%a5		#    bufptr = bufptr + BLKSIZ
	bra.b	load
load1:					# end repeat
	rts				# return

# Subroutine get_inode
#
#  Gets inode whose number is in d6
#  It assumes a5 is pointing to the buffer to be used.
#  Generates a list of the block numbers in the file (266 max),
#  fetching the first indirect block if necessary.
# 		d6 = unsigned long inodenumber and is modified
#		a5 = buffer pointer

iget:

# Get block containing specified inode

	add.l	&31,%d6			# inode_blk = (inode_no + 31) / 16
	mov.l	%d6,%d7
	lsr.l	&4,%d7
	bsr.b	rdblk			# read_block(inode_blk  buffer_ptr)
	
# extract first 11 block numbers from inode disk address field

	and.l	&INOMSK,%d6		# offset = (inode_no+31)mod 16)*64
	lsl.l	&6,%d6		
	lea.l	faddr(%a5,%d6.l),%a0	# faddr + buffer addr +  offset
	mov.l	%a6,%a1			# point to blkno[0] 
	mov.l	&10,%d0			# 11 addresses to unpack (-1 for dbra)
iget1:
	clr.b	(%a1)+			# convert 3 byte field to a long
	mov.b	(%a0)+,(%a1)+
	mov.b	(%a0)+,(%a1)+
	mov.b	(%a0)+,(%a1)+
	dbra	%d0,iget1		# til done

# if an indirect block exists, read( indirect_block  blkno[10])

	mov.l	-(%a1),%d7		# if blkno[10] != 0
	beq.b	iget2
	mov.l	%a5,-(%sp)		#  save buffer pointer
	mov.l	%a1,%a5			#  point to &blkno[10]
	bsr.b	rdblk			#  read( indirect_block  blkno[10])
	mov.l	(%sp)+,%a5		#  restore buffer pointer
	clr.l	NT(%a6)			#  set terminating null in blkno[NT]
iget2:
	rts				# return

# Subroutine read_next_block
#
#	Reads block number blkno[file_block_no] if it is not null.
#	It assumes a5 is pointing to the buffer to be used.
#	It postincrements file_block_no.
#		d5 = long file_block_no
#		a5 = buffer pointer
#		d7 modified

rdnblk:
	mov.l	0(%a6,%d5.l),%d7	# get blkno[file_block_no] 
	bne.b	rdn1			# exit if null
	rts
rdn1:	add.l	&4,%d5			# increment file_block_no
#	bra.b	rdblk			# read block and return

# Subroutine read_block
#
#	Reads block number d7 into buffer pointed to by a5
#		 d7 = long block number
#	(block numbers begin at 0 and each block contains 1024 bytes)
#		 a5 = long buffer pointer
#	(buffer size is 1024 bytes)

rdblk:					# udc = universal disk controller
	clr.l	%d0
	mov.b	ctlr-T(%a6),%d0		# Calculate controller address
	mulu.w	&OPC,%d0		#  offset per ctlr * ctlr number
	mov.l	&DISK,%a1
	add.l	%d0,%a1			#  plus base address

rb0:	clr.b	ANFR(%a1)		# clear previous ACK/NAK from udc
rb1:	tst.b	FLTO(%a1)		# verify command packet is writable
	bne.b	rb1

# The rb2 and rb3 code may not be required but has been used before

rb2:	tst.b	ANTO(%a1)		# wait for udc to clear previous ACK/NAK
	bne.b	rb2
	mov.l	&DELAY,%d0		# inter-command delay (may not be req'd)
rb3:	sub.l	&1,%d0
	bne.b	rb3

# put sector number and buffer pointer into I/O image

	mov.l	%a5,baddr-T(%a6)	# put buffer address in I/O image
	mov.l	%d7,%d0
	lsl.l	&2,%d0			# sector = block * 4
	mov.l	%d0,sect-T(%a6)		# put sector number in I/O image

# move image to command packet I/O area

rb4:	mov.w	&EPKT,%d1		# end of packet I/O area
	lea	epkt+1-T(%a6),%a0	# end of image (+1 for predecrement)
	lea.l	CIO(%a1),%a2		# set base pointer closer
rb5:	mov.b	-(%a0),0(%a2,%d1.w)	# move image byte to I/O area
	sub.w	&2,%d1			# step over even byte
	bhs.b	rb5

# send command and handle response

	mov.b	&FLAG,INT(%a1)		# interrupt udc
rb6:	mov.b	ANFR(%a1),%d0		# check for response to command
	cmp.b	%d0,&NAK
	beq.b	rb0			# if NAK resend command
	cmp.b	%d0,&ACK		# if ACK proceed
	bne.b	rb6			# else check for response again

	clr.b	ANFR(%a1)		# clear ACK/NAK from udc
rb11:	mov.b	FLFR(%a1),%d0		# wait for command completion
	cmp.b	%d0,&FLAG		# status ready flag?
	bne.b	rb11			# if not, wait some more

# read complete, examine status packet for correctness and command status

	mov.b	STAT(%a1),%d1		# check command status word
	bne.b	rb30			# exit if not OK
	mov.b	&SUM,%d1		# SUM = STX + SPLEN + ETX
	sub.b	SSTX(%a1),%d1		# check STX = 2
	sub.b	SPLN(%a1),%d1		# check status pkt length = 19
	sub.b	SETX(%a1),%d1		# check ETX = 3

# respond to udc and exit

rb30:	clr.b	FLFR(%a1)		# release status packet area
	mov.b	&ACK,ANTO(%a1)		# acknowledge valid status packet
	mov.b	&FLAG,INT(%a1)		# interrupt udc

	tst.b	%d1			# die if not successful
rb31:	bne.b	rb31
	rts				# otherwise return


