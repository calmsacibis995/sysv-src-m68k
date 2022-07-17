*	@(#)dfile.m4	2.1.1.1	
dnl	Use m4 on dfile.m4 with these defines:
dnl
dnl	REL	for Release system
dnl	MEXOR	for EXORmacs system
dnl	VME10	for VME/10 system
dnl	VAM=	# of vam boards		(0-5, default is 0)	(MEXOR only)
dnl	UDC=	# of udc boards		(1-4, default is 1)	(MEXOR only)
dnl	M400=	# of mvme400 boards	(0-8, default is 0)	(VME10 only)
dnl	DSK=	# of primary disk	(0-7, default is 1)
dnl	MSG=	value of msg flag	(0-1, default is 1)
dnl	SEM=	value of sem flag	(0-1, default is 1)
dnl	SHM=	value of shm flag	(0-1, default is 1)
dnl	NSWAP=	vlaue of nswap		(>1, default is 3291)
dnl	DSK01=	disks 0 & 1 type	(string, default is DSKDEF)
dnl	DSK23=	disks 2 & 3 type	( "" )
dnl	DSK89=	2nd ctlr disks		(string, default is nothing, MEXOR only)
dnl	DSK1011= 2nd ctlr disks		( "" )
dnl	DSK1617= 3rd ctlr disks		( "" )
dnl	DSK1819= 3rd ctlr disks		( "" )
dnl	DSK2425= 4th ctlr disks		( "" )
dnl	DSK2627= 4th ctlr disks		( "" )
dnl
ifdef(`VAM',`ifelse(VAM,0,,VAM,1,,VAM,2,,VAM,3,,VAM,4,,VAM,5,,`errprint(ERR:`VAM'=VAM)m4exit(1)')',`define(VAM,0)')dnl
ifdef(`UDC',`ifelse(UDC,1,,UDC,2,,UDC,3,,UDC,4,,`errprint(ERR:`UDC'=UDC)m4exit(1)')',`define(UDC,1)')dnl
ifdef(`M400',`ifelse(M400,0,,M400,1,,M400,2,,M400,3,,M400,4,,M400,5,,M400,6,,M400,7,,M400,8,,`errprint(ERR:`M400'=M400)m4exit(1)')',`define(M400,0)')dnl
ifdef(`DSK',`ifelse(DSK,0,,DSK,1,,DSK,2,,DSK,3,,DSK,4,,DSK,5,,DSK,6,,DSK,7,,`errprint(ERR:`DSK'=DSK)m4exit(1)')',`define(DSK,1)')dnl
ifdef(`MSG',`ifelse(MSG,0,,MSG,1,,`errprint(ERR:`MSG'=MSG)m4exit(1)')',`define(MSG,1)')dnl
ifdef(`SEM',`ifelse(SEM,0,,SEM,1,,`errprint(ERR:`SEM'=SEM)m4exit(1)')',`define(SEM,1)')dnl
ifdef(`SHM',`ifelse(SHM,0,,SHM,1,,`errprint(ERR:`SHM'=SHM)m4exit(1)')',`define(SHM,1)')dnl
ifdef(`NSWAP',,`define(NSWAP,3291)')dnl
define(DSKDEF,ifdef(`MEXOR',UDFRC1,UDFRC1))dnl
*
* Motorola ifdef(`MEXOR',EXORmacs,`ifdef(`VME10',VME/10)') Development System
*
* dev	vector	addr	bus	count
*
ifdef(`MEXOR',
`acia	22c	fee011	5	2
ifelse(eval(VAM >= 1),1,,*)acia	070	ff3801	4
ifelse(eval(VAM >= 2),1,,*)acia	270	ff3901	4
ifelse(eval(VAM >= 3),1,,*)acia	170	ff5801	4
ifelse(eval(VAM >= 4),1,,*)acia	1f0	ff5901	4
ifelse(eval(VAM >= 5),1,,*)acia	2f0	ff7801	4
disk	3fc	ff0001	4
ifelse(eval(UDC >= 2),1,,*)disk	3f8	ff0201	4
ifelse(eval(UDC >= 3),1,,*)disk	3f4	ff0401	4
ifelse(eval(UDC >= 4),1,,*)disk	3f0	ff0601	4
lp	0	fee009	0
clock	230	0	6
')dnl
ifdef(`VME10',
`con	108	f1a031	5
disk	3fc	ff0001	4
clock	130	0	6
ifelse(eval(M400 >= 1),1,,*)mvme400	110	f1c001	5
ifelse(eval(M400 >= 2),1,,*)mvme400	110	f1c021	5
ifelse(eval(M400 >= 3),1,,*)mvme400	110	f1c041	5
ifelse(eval(M400 >= 4),1,,*)mvme400	110	f1c061	5
ifelse(eval(M400 >= 5),1,,*)mvme400	110	f1c081	5
ifelse(eval(M400 >= 6),1,,*)mvme400	110	f1c0a1	5
ifelse(eval(M400 >= 7),1,,*)mvme400	110	f1c0c1	5
ifelse(eval(M400 >= 8),1,,*)mvme400	110	f1c0e1	5
')dnl
*
* Pseudo Devices
*
prf	0	0	0
*
* Traps - Not to be modified
*
ifdef(`MEXOR',
`buserr	8	0	7
adderr	c	0	7
insterr	10	0	7
zeroerr	14	0	7
chkerr	18	0	7
trapv	1c	0	7
priverr	20	0	7
trce	24	0	7
em1010	28	0	7
em1111	2c	0	7
syscall	80	0	7
brkpt	84	0	7
sigiot	88	0	7
sigemt	8c	0	7
sigfpe	90	0	7
')dnl
ifdef(`VME10',
`buserr	8	0	7
insterr	10	0	7
trce	24	0	7
')dnl
*
* System Devices
*
ifdef(`MEXOR',
`ifdef(`REL',
`root	disk	00, 10, 20, 30
swap	disk	06 1 NSWAP, 16 1 NSWAP, 26 1 NSWAP, 36 1 NSWAP
pipe	disk	00, 10, 20, 30
dump	disk	06, 16, 26, 36
',
`root	disk	00, 10, 20, 30
swap	disk	01 1 NSWAP, 01 1 NSWAP, 01 1 NSWAP, 01 1 NSWAP
pipe	disk	00, 10, 20, 30
dump	disk	06, 06, 06, 06
')',
`root	disk	00, 10
swap	disk	07 1 NSWAP, 17 1 NSWAP
pipe	disk	00, 10
dump	disk	07, 17
')dnl
*
* Tunable Parameters
*
buffers		  50
calls		  50
inodes	          90
files		  90
mounts		   8
procs		  50
texts		  40
clists		 150
sabufs		   0
maxproc		  25
coremap		 100
swapmap		  75
hashbuf		  64
physbuf		   4
power		   0
mesg		   MSG
sema		   SEM
shmem		   SHM
*
* Disk Configurations:
* disk01 specifies devices 0 and 1; disk23 specifies devices 2 & 3.
* See <sys/io.h> for the definitions of DSKDEF, etc.
*
disk01		ifdef(`DSK01',DSK01,DSKDEF)
disk23		ifdef(`DSK23',DSK23,DSKDEF)
ifdef(`DSK89',
`disk89		DSK89
')dnl
ifdef(`DSK1011',
`disk1011		DSK1011
')dnl
ifdef(`DSK1617',
`disk1617		DSK1617
')dnl
ifdef(`DSK1819',
`disk1819		DSK1819
')dnl
ifdef(`DSK2425',
`disk2425		DSK2425
')dnl
ifdef(`DSK2627',
`disk2627		DSK2627
')dnl
ifdef(`VME10',
`*
* m68k Specific Parameters
*
ram	5	000000	180000	60000	* system ram: no parity
ram	4	180000	e00000	10000	* VME BUS: parity, 64k bds 
')dnl
