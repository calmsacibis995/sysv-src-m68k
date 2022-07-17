/*	@(#)space.h	2.10	 */
#define	KERNEL
#include "sys/acct.h"
struct	acct	acctbuf;
struct	inode	*acctp;

#ifdef m68k
#include "sys/mmu.h"
struct	mmu_table	mmu_table[NPROC];	/* internal table */
#endif

#include "sys/tty.h"
struct	cblock	cfree[NCLIST];	/* free list for character block */

#include "sys/buf.h"
struct	buf	bfreelist;	/* head of the free list of buffers */
struct	pfree	pfreelist;	/* Head of physio header pool */
struct	buf	pbuf[NPBUF];	/* Physical io header pool */

struct	hbuf	hbuf[NHBUF];	/* buffer hash table */

#include "sys/file.h"
struct	file	file[NFILE];	/* file table */

#include "sys/inode.h"
struct	inode	inode[NINODE];	/* inode table */

#include "sys/proc.h"
struct	proc	proc[NPROC];	/* process table */

#include "sys/text.h"
struct	text text[NTEXT];	/* text table */

#include "sys/map.h"
struct map swapmap[SMAPSIZ] = {mapdata(SMAPSIZ)};
struct map coremap[CMAPSIZ] = {mapdata(CMAPSIZ)};
#ifdef m68k
struct map initmap[CMAPSIZ] = {mapdata(CMAPSIZ)};
#endif

#include "sys/callo.h"
struct callo callout[NCALL];	/* Callout table */

#include "sys/mount.h"
struct mount mount[NMOUNT];	/* Mount table */

#include "sys/elog.h"
#include "sys/err.h"
struct	err	err = {		/* Error slots */
	NESLOT,
};

#include "sys/sysinfo.h"
struct sysinfo sysinfo;
struct syswait syswait;
struct syserr syserr;

#include "sys/opt.h"

#include "sys/var.h"
struct var v = {
	NBUF,
	NCALL,
	NINODE,
	(char *)(&inode[NINODE]),
	NFILE,
	(char *)(&file[NFILE]),
	NMOUNT,
	(char *)(&mount[NMOUNT]),
	NPROC,
	(char *)(&proc[1]),
	NTEXT,
	(char *)(&text[NTEXT]),
	NCLIST,
	NSABUF,
	MAXUP,
	SMAPSIZ,
	NHBUF,
	NHBUF-1,
	NPBUF,
#if m68k
	CMAPSIZ,
#endif
};

#include "sys/init.h"

#ifndef	PRF_0
prfintr() {}
int	prfstat;
#endif

#ifdef	VP_0
#include "sys/vp.h"
#endif

#ifndef	m68k
#ifdef	DISK_0
#define	RM05_0
#define	RP06_0
#define	RM80_0
#define	RP07_0
#ifndef	DISK_1
#define	DISK_1	0
#endif
#ifndef	DISK_2
#define	DISK_2	0
#endif
#ifndef	DISK_3
#define	DISK_3	0
#endif
#include "sys/iobuf.h"
#define	DISKS	(DISK_0+DISK_1+DISK_2+DISK_3)
struct iobuf gdtab[DISKS];
struct iobuf gdutab[DISKS*8];
int	gdindex[DISKS*8];
short	gdtype[DISKS*8];
struct iotime gdstat[DISKS*8];
#endif
#else
#ifdef	MEXOR
#ifdef	DISK_0
#ifndef	DISK_1
#define	DISK_1	0
#endif
#ifndef	DISK_2
#define	DISK_2	0
#endif
#ifndef	DISK_3
#define	DISK_3	0
#endif
#include "sys/iobuf.h"
#define	CTLS	((DISK_0+DISK_1+DISK_2+DISK_3)/64)
struct iotime udstat[CTLS*8];
struct iobuf udtab[CTLS];
struct iobuf udutab[CTLS*8] = {
	tabinit(0,&udstat[0].ios), tabinit(0,&udstat[1].ios),
	tabinit(0,&udstat[2].ios), tabinit(0,&udstat[3].ios),
	tabinit(0,&udstat[4].ios), tabinit(0,&udstat[5].ios),
	tabinit(0,&udstat[6].ios), tabinit(0,&udstat[7].ios),
#if CTLS>1
	tabinit(0,&udstat[8].ios), tabinit(0,&udstat[9].ios),
	tabinit(0,&udstat[10].ios), tabinit(0,&udstat[11].ios),
	tabinit(0,&udstat[12].ios), tabinit(0,&udstat[13].ios),
	tabinit(0,&udstat[14].ios), tabinit(0,&udstat[15].ios),
#if CTLS>2
	tabinit(0,&udstat[16].ios), tabinit(0,&udstat[17].ios),
	tabinit(0,&udstat[18].ios), tabinit(0,&udstat[19].ios),
	tabinit(0,&udstat[20].ios), tabinit(0,&udstat[21].ios),
	tabinit(0,&udstat[22].ios), tabinit(0,&udstat[23].ios),
#if CTLS>3
	tabinit(0,&udstat[24].ios), tabinit(0,&udstat[25].ios),
	tabinit(0,&udstat[26].ios), tabinit(0,&udstat[27].ios),
	tabinit(0,&udstat[28].ios), tabinit(0,&udstat[29].ios),
	tabinit(0,&udstat[30].ios), tabinit(0,&udstat[31].ios),
#endif
#endif
#endif
};
#endif
#endif
#endif

#ifdef	TRACE_0
#include "sys/trace.h"
struct trace trace[TRACE_0];
#endif

#ifdef  CSI_0
#include "sys/csi.h"
#include "sys/csihdw.h"
struct csi csi_csi[CSI_0];
int csibnum = CSIBNUM;
struct csibuf *csibpt[CSIBNUM];
#endif

#ifdef	VPM_0
#include "sys/vpmt.h"
struct vpmt vpmt[VPM_0];
struct csibd vpmtbd[VPM_0*(XBQMAX + EBQMAX)];
struct vpminfo vpminfo =
	{XBQMAX, EBQMAX, VPM_0*(XBQMAX + EBQMAX)};
int vpmbsz= VPMBSZ;
#endif

#ifdef	DMK_0
#define MAXDMK 4
#include	"sys/dmk.h"
struct dmksave dmksave[MAXDMK];
#endif

#ifdef	X25_0
#include "sys/x25.h"
struct x25slot x25slot[X25_0];
struct x25tab x25tab[X25_0];
struct x25timer x25timer[X25_0];
struct x25link x25link[X25LINKS];
struct x25timer *x25thead[X25LINKS];
struct x25lntimer x25lntimer[X25LINKS];
struct csibd x25bd[X25BUFS];
struct csibuf x25buf;
struct x25info x25info =
	{X25_0, X25_0, X25LINKS, X25BUFS, X25BYTES};
#endif

#ifdef PCL11B_0
#include "sys/pcl.h"
#endif

#if MESG==1
#include	"sys/ipc.h"
#include	"sys/msg.h"

struct map	msgmap[MSGMAP];
struct msqid_ds	msgque[MSGMNI];
struct msg	msgh[MSGTQL];
struct msginfo	msginfo = {
	MSGMAP,
	MSGMAX,
	MSGMNB,
	MSGMNI,
	MSGSSZ,
	MSGTQL,
	MSGSEG
};
#endif

#if SEMA==1
#	ifndef IPC_ALLOC
#	include	"sys/ipc.h"
#	endif
#include	"sys/sem.h"
struct semid_ds	sema[SEMMNI];
struct sem	sem[SEMMNS];
struct map	semmap[SEMMAP];
struct	sem_undo	*sem_undo[NPROC];
#define	SEMUSZ	(sizeof(struct sem_undo)+sizeof(struct undo)*SEMUME)
int	semu[((SEMUSZ*SEMMNU)+NBPW-1)/NBPW];
union {
	short		semvals[SEMMSL];
	struct semid_ds	ds;
	struct sembuf	semops[SEMOPM];
}	semtmp;

struct	seminfo seminfo = {
	SEMMAP,
	SEMMNI,
	SEMMNS,
	SEMMNU,
	SEMMSL,
	SEMOPM,
	SEMUME,
	SEMUSZ,
	SEMVMX,
	SEMAEM
};
#endif

#if SHMEM==1
#	ifndef	IPC_ALLOC
#	include	"sys/ipc.h"
#	endif
#include	"sys/shm.h"
struct shmid_ds	*shm_shmem[NPROC*SHMSEG];
struct shmid_ds	shmem[SHMMNI];
struct	shmpt_ds	shm_pte[NPROC*SHMSEG];
struct	shminfo	shminfo	= {
	SHMMAX,
	SHMMIN,
	SHMMNI,
	SHMSEG,
	SHMBRK,
	SHMALL
};
#endif
#ifdef	NSC_0
#include "sys/nscdev.h"
#endif
#ifdef ST_0
#include "sys/st.h"
struct stbhdr	stihdrb[STIHBUF];
struct stbhdr	stohdrb[STOHBUF];
struct ststat	ststat = {
	STIBSZ,		/* input buffer size */
	STOBSZ,		/* output buffer size */
	STIHBUF,	/* # of buffer headers */
	STOHBUF,	/* # of buffer headers */
	STNPRNT		/* # of printer	channels */
};

#endif
