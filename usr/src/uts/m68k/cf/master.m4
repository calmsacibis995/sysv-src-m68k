*	@(#)master.m4	2.1	
dnl	The master file is generated with the use of the m4 preprocessor.
dnl	Use m4 on dfile.m4 with these defines:
dnl
dnl	MEXOR	for EXORmacs system
dnl	VME10	for VME/10 system
dnl
*
* The following devices are those that can be specified in the system
* description file.  The name specified must agree with the name shown.
*
ifdef(`MEXOR',
`clock	4	0	302	clk	0	0	0	1	6
acia	4	137	4	acia	4	0	0	4	5	tty
lp	0	33	44	lp	4	0	1	1	5
disk	4	37	14	ud	4	0	5	64	5
')dnl
ifdef(`VME10',
`clock	4	0	302	clk	0	0	0	1	6
con	4	137	304	con	4	0	0	1	4	tty
disk	4	36	14	ud	4	0	5	64	5
')dnl
*
* The following devices must not be specified in the system description
* file.  They are here to supply information to the config program.
*
ifdef(`MEXOR',
`tty	0	27	324	sy	0	0	2	1	0
memory	0	6	324	mm	0	0	3	1	0
errlog	0	34	324	err	0	0	4	1	0
prf	0	7	304	prf	0	0	6	1	0
')dnl
ifdef(`VME10',
`tty	0	27	324	sy	0	0	2	1	0
memory	0	6	324	mm	0	0	3	1	0
errlog	0	34	324	err	0	0	4	1	0
prf	0	7	304	prf	0	0	6	1	0
')dnl
*
* The following are system trap locations that should not be modified
* or specified in the system description file.
*
buserr	4	0	300	bus	0	0	0	1	7
adderr	4	0	300	addr	0	0	0	1	7
insterr	4	0	300	inst	0	0	0	1	7
zeroerr	4	0	300	zero	0	0	0	1	7
chkerr	4	0	300	chk	0	0	0	1	7
trapv	4	0	300	trpv	0	0	0	1	7
priverr	4	0	300	priv	0	0	0	1	7
em1010	4	0	300	L101	0	0	0	1	7
em1111	4	0	300	L111	0	0	0	1	7
syscall	4	0	300	sys	0	0	0	1	7
trce	4	0	300	trce	0	0	0	1	7
brkpt	4	0	300	bkpt	0	0	0	1	7
sigiot	4	0	300	iot	0	0	0	1	7
sigemt	4	0	300	emt	0	0	0	1	7
sigfpe	4	0	300	fpe	0	0	0	1	7
$$$
*
* The following entries form the alias table.
*
*	no aliases
$$$
*
* The following entries form the tunable parameter table.
*
buffers	NBUF
inodes	NINODE
files	NFILE
mounts	NMOUNT
swapmap	SMAPSIZ
calls	NCALL
procs	NPROC
texts	NTEXT
clists	NCLIST
sabufs	NSABUF	0
power	POWER	0
maxproc	MAXUP	25
* hashbuf must be a power of 2
hashbuf	NHBUF	64
physbuf	NPBUF	4
mesg	MESG	0
msgmap	MSGMAP	100
msgmax	MSGMAX	8192
msgmnb	MSGMNB	16384
msgmni	MSGMNI	50
msgssz	MSGSSZ	8
msgtql	MSGTQL	40
msgseg	MSGSEG	1024
sema	SEMA	0
semmap	SEMMAP	10
semmni	SEMMNI	10
semmns	SEMMNS	60
semmnu	SEMMNU	30
semmsl	SEMMSL	25
semopm	SEMOPM	10
semume	SEMUME	10
semvmx	SEMVMX	32767
semaem	SEMAEM	16384
shmem	SHMEM	0
shmmax	SHMMAX	(128*1024)
shmmin	SHMMIN	1
shmmni	SHMMNI	100
shmseg	SHMSEG	1
shmbrk	SHMBRK	16
shmall	SHMALL	512
coremap	CMAPSIZ	100
disk01	DISK01	UDFRC1
disk23	DISK23	UDFRC1
$$$
*
* m68k specific parameters
*
ifdef(`MEXOR',
`mpu	68000
force	EXORclk
force	EXORmmu
force	EXORcon
dup	10	22c	lpintr	0
dup	0	24	swabort
alien	10	fe9e6e	* MACSBUG breakpoint (illegal instruction)
probe	ff020f	80	* UDC 1
probe	ff040f	80	* UDC 2
probe	ff060f	80	* UDC 3
probe	ff080f	80	* UDC 4
probe	ff0a0f	80	* UDC 5
probe	ff0c0f	80	* UDC 6
probe	ff0e0f	80	* UDC 7
probe	ff100f	80	* MCCM 0
probe	ff120f	80	* MCCM 1
probe	ff140f	80	* MCCM 2
probe	ff160f	80	* MCCM 3
probe	ff180f	80	* MCCM 4
probe	ff1a0f	80	* MCCM 5
probe	ff1c0f	80	* MCCM 6
probe	ff1e0f	80	* MCCM 7
probe	ff3801	0	* VAM 0
probe	ff3901	0	* VAM 1
probe	ff5801	0	* VAM 2
probe	ff5901	0	* VAM 3
probe	ff7801	0	* VAM 4
')dnl
ifdef(`VME10',
`mpu	68010
force	m68451
force	MC146818
dup	4	8	mmuintr
alien	10	f02680	* TENBUG breakpoint (illegal instruction)
alien	24	f02544	* TENBUG trace
alien	138	f01524	* TENBUG software abort
probe	f1c3e9	0	* Mag tape
')dnl
