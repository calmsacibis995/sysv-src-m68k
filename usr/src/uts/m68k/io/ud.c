/* @(#)ud.c	2.1.1.1	 */
/*
 * Motorola Universal Disk Controller (UDC) Driver (M68KVM21)
 * handles a maximum of four Universal Disk Controllers
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/buf.h"
#include "sys/elog.h"
#include "sys/iobuf.h"
#include "sys/systm.h"
#include "sys/map.h"

#ifdef	DEBUG
char	DebugUD = 0;	/* 0 == no debug printouts
			 * 1 == some debug printouts
			 * 2 == verbose debug printouts
			 */
#endif

/*
 * interpretation of (dev_t) device name:
 *   bits 7,6  = controller number, 0..3
 *   bits 5..3 = device on controller, 0..7
 *   bits 2..0 = slice on device, 0..7
 */

struct size
{
	daddr_t nblocks;	/* size of slice in blocks */
	daddr_t blkoff;		/* first physical block in slice */
};

/* udioctl command to format a track */
#define UDFMT		1

#define	MAXDEV		8
#define MAXCTL		4
extern struct size ud_sizes[][MAXDEV][8]; /* ctl, dev, slice - init in io.h */

struct ud_cmdp			/* C - CONST, V - VAR */
{
	unsigned char	ud_STX;		/* C - hard coded ASCII STX */
	unsigned char	ud_pid;		/* V - packet id */
	unsigned char	ud_siz;		/* V - packet size (from STX to ETX) */
	unsigned char	ud_dev;		/* V - device number to UDC */
	unsigned short	ud_cmd;		/* V - command */
	unsigned short	ud_cnt;		/* V - number of sectors to transfer */
	unsigned short	ud_bps;		/* C - bytes per sector */
	unsigned long	ud_adr;		/* V - memory addr/start sec FORMAT*/
	unsigned char	ud_fill1;	/* V - fill/ETX for format */
	unsigned char	ud_fill2;	/* C - fill */
	unsigned long	ud_ssec;	/* V - starting sector */
	unsigned char	ud_ETX;		/* C - hard coded ASCII ETX */
};


struct ud_stat
{
	unsigned char	ud_STX;		/* hard coded ASCII STX */
	unsigned char	ud_pid;		/* pack id */
	unsigned char	ud_siz;		/* packet size (from STX to ETX) */
	unsigned char	ud_dev;		/* device number to UDC */
	unsigned short	ud_stype;	/* command & status type */
	unsigned short	ud_scode;	/* status code */
	union {
		struct {
			unsigned short	ud_cnt;	/* # of sectors transferred */
			unsigned short	ud_bps;	/* bytes per sector */
			unsigned long	ud_adr;	/* memory address */
			unsigned short	ud_chk;	/* unused or checksum */
			unsigned char	ud_ETX;	/* hard coded ASCII ETX */
		} st19;
		struct {
			unsigned short	ud_dst;	/* device status */
			unsigned long	ud_nsec;/* number of sectors */
			unsigned char	ud_ETX;	/* hard coded ASCII ETX */
		} st15;
		struct {
			unsigned long	ud_nsec;/* number of sectors */
			unsigned char	ud_ETX;	/* hard coded ASCII ETX */
		} st13;
		struct {
			unsigned char	ud_ETX;	/* hard coded ASCII ETX */
		} st9;
	} ud_un;
};

struct ud_io
{
	unsigned char	ud_cf0;		/* character fill */
	unsigned char	ud_fill0[10];	/* fill */
	unsigned char	ud_cf1;		/* character fill */
	unsigned char	ud_intr;	/* UDC interrupt */
	unsigned char	ud_cf2;		/* character fill */
	unsigned char	ud_reset;	/* UDC reset */
	unsigned char	ud_fill1[240];	/* fill */
	unsigned char	ud_cf3;		/* character fill */
	unsigned char	ud_cflg;	/* command flag: message to UDC */
	unsigned char	ud_cf4;		/* character fill */
	unsigned char	ud_san;		/* status ack/nak to UDC */
	unsigned char	ud_cmd[42];	/* command packet */
	unsigned char	ud_fill3[82];	/* fill */
	unsigned char	ud_cf5;		/* character fill */
	unsigned char	ud_sflg;	/* status flag: message from UDC */
	unsigned char	ud_cf6;		/* character fill */
	unsigned char	ud_can;		/* command ack/nak from UDC  */
	unsigned char	ud_stat[38];	/* status packet */
	unsigned char	ud_fill4[86];	/* fill */
};

extern int ud_addr[];		/* controller base addrs - init in conf.c */
extern ud_cnt;			/* number of devices - init in conf.c */

/* number of device registers saved in error logging */
#define NUDREGS		((sizeof(struct ud_stat)+1)/sizeof(short))

#define CTL(x)		((x>>6)&3)
#define	DEV(x)		((x>>3)&7)
#define	SLICE(x)	(x&7)
#define	STOD(x)		(x<<3)
#define	POS(x)		((x>>3)&037)
#define	MKPOS(x,y)	((x<<3)|y)
#define	PTOC(x)		(x>>3)
#define	PTOD(x)		(x&7)

#define	ERRCNT	10
#define	ERRFLAG	1
#define	NOERR	0

#define	CYL	128

int	errlog[MAXCTL] = {
	ERRCNT, ERRCNT, ERRCNT, ERRCNT
};	/* for error handling */

/* following three data structures are defined in space.h */
extern	struct	iotime	udstat[];	/* one per device */
extern	struct	iobuf	udtab[];	/* uninit'ed iobufs for controllers */
extern	struct	iobuf	udutab[];	/* init'ed iobufs for devices */

#define	CLR	0x00	/* CLEAR command */
#define STX	0x02	/* ASCII STX */
#define ETX	0x03	/* ASCII ETX */
#define	ACK	0x06	/* ACK code */
#define	NAK	0x15	/* NAK code */
#define	FLAG	0x80	/* used several places to test/write high bit */


#define	READ	0x1001	/* READ with RETRY */
#define	WRITE	0x2002	/* WRITE */
#define	FORMAT	0x4006	/* FORMAT track */

#define	BPS	0x0100	/* Bytes Per Sector */
#define	RWPS	0x15	/* Read Write Packet Size */

#define	OK0	0x7000	/* No errors */
#define	SOL00	0x7200	/* Solicited Device Status */
#define	UNS00	0x8000	/* Unsolicited Device Status */
#define	UNS01	0x8001	/* Unsolicited DMA Channel Error */

/*
 * The following array contains the most recent
 *	command given by udstart for each controller.
 *	It's also used by udintr to re-issue a command.
 *	PKTSIZ is defined because sizeof(cmdbuf[n]) will
 *	include padding which is not to be output by putp().
 *	FPKTSIZ is the size of a format command packet.
 */

#define PKTSIZ	21
#define FPKTSIZ	15
struct ud_cmdp cmdbuf[MAXCTL] =
	{ STX, 0, RWPS, 0, 0, 0, BPS, 0L, 0, 0, 0L, ETX,
	  STX, 1, RWPS, 0, 0, 0, BPS, 0L, 0, 0, 0L, ETX,
	  STX, 2, RWPS, 0, 0, 0, BPS, 0L, 0, 0, 0L, ETX,
	  STX, 3, RWPS, 0, 0, 0, BPS, 0L, 0, 0, 0L, ETX };


#define	acts	io_s1
#define	qcnt	io_s2

#define	pblk	av_back
#define	cylin	b_resid

/*
 * The controller is considered busy while it has an outstanding acknowledge
 * to a command.  A device is considered busy while it has io requests in
 * queue.
 */

udopen(dev)
{
	register struct iobuf *dp;
	register pos;
	int	unit, ctl;

	ctl = CTL(dev);
	if (dev >= ud_cnt)
	{
		u.u_error = ENXIO;
		return;
	}
	if ((udtab[ctl].b_flags&B_ONCE)==0) 
	{
		udtab[ctl].b_flags |= B_ONCE;
	}
	unit = DEV(dev);
	pos = POS(dev);
	dp = &udutab[pos];
	dp->io_addr = (physadr)ud_addr[ctl];
	dp->io_nreg = NUDREGS;
	dp->io_stp = &udstat[pos].ios;
}

udclose()
{
}


udstrategy(bp)
register struct buf *bp;
{
	register struct iobuf *dp;
	register struct buf *ap;
	register struct size *sp;
	register struct iotime *ip;
	daddr_t	last;
	register unit;
	register ctl;
	register pos;

	unit = DEV(bp->b_dev);
	ctl = CTL(bp->b_dev);
	pos = POS(bp->b_dev);
#ifdef	DEBUG
	if( DebugUD == 2 )
		udprint(bp->b_dev, "ud:strategy ");
#endif
	sp = &ud_sizes[ctl][unit][SLICE(bp->b_dev)];
	last = sp->nblocks;
	if (bp->b_blkno < 0 || bp->b_blkno >= last) {
		if (bp->b_blkno == last && (bp->b_flags&B_READ))
			bp->b_resid = bp->b_bcount;
		else {
			bp->b_flags |= B_ERROR;
			bp->b_error = ENXIO;
		}
		iodone(bp);
		return;
	}
	bp->av_forw = NULL;	/* buffers are added to the end of the queue */
	bp->b_start = lbolt;
	/* physical disk block */
	bp->pblk = (struct buf *)(bp->b_blkno + sp->blkoff);
	bp->cylin = ((int)bp->pblk)/CYL;	     /* used only for sorting */
	ip = &udstat[pos];
	ip->io_cnt++;
	ip->io_bcnt += btoc(bp->b_bcount);
	dp = &udutab[pos];

	/* put the io request into the device queue */

	spl4();
	dp->qcnt++;
	if (dp->b_actf == NULL) {
		dp->b_actf = bp;
		dp->b_actl = bp;
		dp->acts = (int)bp;
	} else {
		register struct buf *cp;

		if (((int)ip->io_cnt&07) == 0)
			dp->acts = (int)dp->b_actl;
		for (ap = (struct buf *)dp->acts; cp = ap->av_forw; ap = cp) {
			int s1, s2;

			if ((s1 = ap->cylin - bp->cylin)<0) s1 = -s1;
			if ((s2 = ap->cylin - cp->cylin)<0) s2 = -s2;
			if (s1 < s2)
				break;
		}
		ap->av_forw = bp;
		if ((bp->av_forw = cp) == NULL)
			dp->b_actl = bp;
	}
	if (dp->b_active == 0) {	/* if device is not being serviced */
		dp->b_active++;
		udins(dp, ctl);		/* turn it over to the controller */
	}
	spl0();
}

udstart(ctl)
{
	register struct buf *bp;
	register struct iobuf *dp;
	register struct ud_io *udc;
	register struct ud_cmdp *cmdp;
	register int sz;

	while((dp = (struct iobuf *)udtab[ctl].b_actf) != NULL) {

		bp = dp->b_actf;

		/* remove dev from controller q but leave it marked active */
		udtab[ctl].b_actf = dp->b_forw;

		if (bp == NULL) {		/* if device has no io queued */
			dp->b_active = 0;	/* make it inactive */
			continue;		/* go get another device */
		}

		/* give a command */
		/* set up error count and time */
		udtab[ctl].b_errcnt = ERRCNT;
		dp->io_start = lbolt;		/* for actual I/O time */

		/* start I/O */
		cmdp = &cmdbuf[ctl];		/* point to command buffer */
		cmdp->ud_ssec = (int)bp->pblk*(BSIZE/BPS); /* sector address */
		/* # sectors */
		cmdp->ud_cnt = ((bp->b_bcount+(BSIZE-1))/BSIZE)*(BSIZE/BPS);
		cmdp->ud_dev = DEV(bp->b_dev);		/* device number */
		cmdp->ud_adr = (long)bp->b_un.b_addr;	/* memory address */
		cmdp->ud_siz = PKTSIZ;			/* normal packet */
		cmdp->ud_fill1 = 0;			/* format clean up */
		if( bp->b_flags&B_READ )
			cmdp->ud_cmd = READ;		/* READ */
		else if( bp->b_flags&B_FORMAT ) {
			cmdp->ud_cmd = FORMAT;		/* FORMAT */
			cmdp->ud_adr = cmdp->ud_ssec;
			cmdp->ud_fill1 = ETX;
			cmdp->ud_siz = FPKTSIZ;
		} else 
			cmdp->ud_cmd = WRITE;		/* WRITE */


#ifdef	DEBUG
		if( DebugUD )
			printf("ud: cmd=%x drv=%d cnt=%x pblk=%x adr=%x\n",
			cmdp->ud_cmd, cmdp->ud_dev,
			cmdp->ud_cnt/2, cmdp->ud_ssec/2, cmdp->ud_adr);
#endif

		blkacty |= (1<<ctl);
		udtab[ctl].b_active++;			/* controller active */
		udc = (struct ud_io *)ud_addr[ctl];

		/* send packet */
		udputp( cmdp, udc->ud_cmd, cmdp->ud_siz);
		udc->ud_can = CLR;			/* clear can */
		udc->ud_cflg = FLAG;			/* command flag */
		udc->ud_intr = FLAG;			/* interrupt UDC */
		return;			/* only one command per invocation */
	}
}

udintr(ctl)
register ctl;
{
	struct ud_stat statbuf;
	register struct iobuf *dp;
	register struct ud_io *udc;
	register unsigned unit;
	register unsigned char can;
	register unsigned char *etx;
	register unsigned short status;
	register physadr ioa;

	ctl = CTL(ctl);		/* fix for udxint for multiple udcs */

	udc = (struct ud_io *)ud_addr[ctl];

	if (udtab[ctl].b_active && (udc->ud_cflg == CLR)) {
		/* do ack/nak processing */
		can = udc->ud_can;
		udc->ud_can = CLR;
		if (can != ACK ) {
			/* format error for logging later */
			if ( !(dp = (struct iobuf *)udtab[ctl].b_actf) )
				dp = &udutab[MKPOS(ctl,0)];
			bzero(&statbuf, sizeof(statbuf));
			ioa = dp->io_addr;
			dp->io_addr = (physadr)&statbuf;
			fmtberr(dp,0);
			dp->io_addr = ioa;
			if ( !udtab[ctl].b_actf && dp->io_erec )
				logberr(dp,B_ERROR);
		}
		if (can == ACK) {
			udtab[ctl].b_active = 0;	/* no outstanding ack */
			blkacty &= ~(1<<ctl);
			udstart(ctl);		/* get next command started */
		} else if (can == NAK) {
			if( --udtab[ctl].b_errcnt ) {
				udputp( &cmdbuf[ctl], udc->ud_cmd,
					cmdbuf[ctl].ud_siz);
				udc->ud_can = CLR;
				udc->ud_cflg = FLAG;
				udc->ud_intr = FLAG;
			} else {
				udtab[ctl].b_active = 0;
				blkacty &= ~(1<<ctl);
				udcomp(&udutab[MKPOS(ctl,cmdbuf[ctl].ud_dev)],
				ERRFLAG);
			}
		} else {
			printf("ERROR: no ack/nak from UDC %d\n",ctl);
		}
	}

	/* ACK/NAK processing complete */

	if ( (udc->ud_sflg&FLAG) == 0 )	/* status packet present ? */
		return;			/* no - continue */

	if ( udc->ud_san != CLR ) {
		printf("ERROR: ack/nak not cleared by UDC %d\n",ctl);
		/* format error for logging later */
		if ( !(dp = (struct iobuf *)udtab[ctl].b_actf) )
			dp = &udutab[MKPOS(ctl,0)];
		bzero(&statbuf, sizeof(statbuf));
		ioa = dp->io_addr;
		dp->io_addr = (physadr)&statbuf;
		fmtberr(dp,0);
		dp->io_addr = ioa;
		if ( !udtab[ctl].b_actf && dp->io_erec )
			logberr(dp,B_ERROR);
	}

	udgetp( &statbuf, udc->ud_stat, sizeof(statbuf));

	unit = statbuf.ud_dev;	/* hold device number in "unit" */
	if ( unit  >= MAXDEV ) {
		printf("ERROR: garbled device number from UDC %d\n",ctl);
		/* format error for logging later */
		if ( !(dp = (struct iobuf *)udtab[ctl].b_actf) )
			dp = &udutab[MKPOS(ctl,0)];
		ioa = dp->io_addr;
		dp->io_addr = (physadr)&statbuf;
		fmtberr(dp,0);
		dp->io_addr = ioa;
		if ( !udtab[ctl].b_actf && dp->io_erec )
			logberr(dp,B_ERROR);
		return;
	}
	dp = &udutab[MKPOS(ctl,unit)];

	etx = &statbuf.ud_STX;
	if ((*etx != STX) || (etx[statbuf.ud_siz-1] != ETX)) {
		/* format error for logging later */
		ioa = dp->io_addr;
		dp->io_addr = (physadr)&statbuf;
		fmtberr(dp,0);
		dp->io_addr = ioa;
		if ( !udtab[ctl].b_actf && dp->io_erec )
			logberr(dp,B_ERROR);
		/* bump error counter */
		if (--errlog[ctl]) {
			udc->ud_san = NAK;
			udc->ud_sflg = CLR;
			udc->ud_intr = FLAG;
			return;
		}
		else {  /* unintelligible message from UDC */
			/* report error */
			printf("ERROR: unintelligible message from UDC %d\n",
				ctl);
			errlog[ctl] = ERRCNT;
			return;
		}
	}

	errlog[ctl] = ERRCNT;
			
	/* if we get here we have a valid status packet */

	udc->ud_san = ACK;		/* acknowledge valid packet */
	udc->ud_sflg = CLR;	/* clear status flag */
	udc->ud_intr = FLAG;	/* interrupt UDC */

	status = (statbuf.ud_stype<<8) + statbuf.ud_scode;
	switch(status) {
	case OK0:
			udcomp(dp,NOERR);
#ifdef	DEBUG
			if( DebugUD == 2 )
				printf("ud: good io on UDC %d drive %d\n",
				ctl, unit);
#endif
			break;
	case UNS00:
			/* ready change in device not currently used */
			break;
	case UNS01:
			/* report unsolicited message */
			printf("ERROR: UI on UDC %d drive %d\n", ctl, unit);
			/* format and log error */
			ioa = dp->io_addr;
			dp->io_addr = (physadr)&statbuf;
			fmtberr(dp,0);
			dp->io_addr = ioa;
			logberr(dp,B_ERROR);
			break;
	case SOL00:
			/* handle solicited status */
	default:
			/* an error has occurred */
			printf("ERROR %x on UDC %d drive %d\n",
				status, ctl, unit);
			/* format error for logging later */
			ioa = dp->io_addr;
			dp->io_addr = (physadr)&statbuf;
			fmtberr(dp,0);
			dp->io_addr = ioa;
			udcomp(dp,ERRFLAG);
			break;
	}
}

udcomp(dp,errflag)		/* I/O complete processing */
register struct iobuf *dp;
register errflag;
{
	register struct buf *bp;
	register struct iotime *ip;


	bp = dp->b_actf;			/* get buf */
	dp->b_errcnt = 0;
	dp->b_actf = bp->av_forw;	/* take serviced request off io queue */
	dp->qcnt--;
	if (errflag) {
		bp->b_resid = bp->b_bcount;
		bp->b_flags |= B_ERROR;
	}
	else	bp->b_resid = 0;
	if (dp->io_erec)
		logberr(dp,bp->b_flags&B_ERROR);

	/* if this is the buf the insert starts at, pick a new starting buf */
	if (bp == (struct buf *)dp->acts)
		dp->acts = (int)dp->b_actf;

	ip = &udstat[POS(bp->b_dev)];
	ip->io_resp += lbolt - bp->b_start;
	ip->io_act += lbolt - dp->io_start;
	iodone(bp);	/* release buffer */

	/*
	 * Now put device into controller queue and call udstart
	 *	if the controller is not busy.
	 */

	udins( dp, CTL(bp->b_dev) );
}


udins( dp, ctl )		/* insert device into controller queue */
struct iobuf *dp;
{
	dp->b_forw = NULL;
	if (udtab[ctl].b_actf == NULL)
		udtab[ctl].b_actf = (struct buf *)dp;
	else
		udtab[ctl].b_actl->b_forw = (struct buf *)dp;
	udtab[ctl].b_actl = (struct buf *)dp;

	/* if not busy then start */
	if (udtab[ctl].b_active == 0)
		udstart(ctl);
}
		
udread(dev)
{
	/* guarantee block alignment of both offset and count */
	if (u.u_offset & BMASK || u.u_count & BMASK) {
		u.u_error = EINVAL;
		return;
	}
	if (physck(ud_sizes[CTL(dev)][DEV(dev)][SLICE(dev)].nblocks, B_READ))
		physio(udstrategy, 0, dev, B_READ);
}

udwrite(dev)
{
	/* guarantee block alignment of both offset and count */
	if (u.u_offset & BMASK || u.u_count & BMASK) {
		u.u_error = EINVAL;
		return;
	}
	if (physck(ud_sizes[CTL(dev)][DEV(dev)][SLICE(dev)].nblocks, B_WRITE))
		physio(udstrategy, 0, dev, B_WRITE);
}

udclr()
{
	register ctl;

	for (ctl=0; ctl<MAXCTL; ctl++ )
	{
		udtab[ctl].b_active = 0;
		udtab[ctl].b_actf = NULL;	/* no devices in queue */
	}
}

udprint(dev, str)
register dev;
char *str;
{
	printf("%s on UDC drive %d,", str, DEV(dev) );
	if (ud_cnt >64)
		printf(" ctl %d,", CTL(dev) );
	printf(" slice %d\n", SLICE(dev));
}

udputp( buf, ctl, n )
register char	*buf;
register char	*ctl;
register n;

{	for( ctl++; n--; ctl += 2 )
		*ctl = *buf++;
}

udgetp( buf, ctl, n )

register char	*buf;
register char	*ctl;
register n;

{	for( ctl++; n--; ctl += 2 )
		*buf++ = *ctl;
}

udioctl(dev, cmd, arg, mode)
{
	static struct buf b;
	register struct iobuf *dp;

	switch(cmd) {
	default:
		u.u_error = EINVAL;
		break;
	case UDFMT:		/* format a track */
		if (dev >= ud_cnt) {
			u.u_error = EINVAL;
			break;
		}
		if ((arg < 0) ||
		(arg > ud_sizes[CTL(dev)][DEV(dev)][SLICE(dev)].nblocks-1)) {
			u.u_error = EINVAL;
			break;
		}
		dp = &udutab[POS(dev)];
		spl6();
		while(b.b_flags & B_BUSY) {
			b.b_flags |= B_WANTED;
			sleep((caddr_t)&b, PRIBIO+1);
		}
		/* set up buffer header b */
		b.b_error = 0;
		b.b_proc = u.u_procp;
		b.b_un.b_addr = 0;
		b.b_flags = B_BUSY | B_FORMAT;
		b.b_dev = dev;
		b.b_blkno = arg;
		b.b_bcount = 0;
		if (dp->b_active != 0) {
			u.u_error = EIO;
			break;
		}
		udstrategy(&b);
		spl6();
		while ((b.b_flags&B_DONE) == 0)
			sleep((caddr_t)&b, PRIBIO);
		if (b.b_flags&B_WANTED)
			wakeup((caddr_t)&b);
		spl0();
		b.b_flags &= ~(B_BUSY|B_WANTED);
		if (b.b_flags&B_ERROR)
			if ((u.u_error = b.b_error)==0)
				u.u_error = EIO;
		break;
	}
}


uddump()
{
	extern int	nswap;
	extern daddr_t	swplo;
	extern dev_t	swapdev;
	extern struct map initmap[];
	int	ndump = nswap;		/* number of dump blocks available */
	daddr_t	skip = swplo;		/* starting block of dump */
	daddr_t	dmplo = swplo;		/* next block to dump to */
	long	bcnt;			/* number of bytes to dump */
	int	ctl = CTL(swapdev);	/* controller to write to */
	struct ud_cmdp	*cmdp;		/* pointer to command buffer */
	struct ud_stat	statbuf;	/* status buffer */
	register struct ud_io *udc;	/* pointer to controller interface */
	int	imn;			/* index into initmap map */

	/* calculate where to start dump to end at top of swap area */
	bcnt = ((ctob(initmap[1].m_addr)+BSIZE-1)/BSIZE)*BSIZE;
	for (imn = 1; initmap[imn].m_size; imn++)
		bcnt += ((ctob(initmap[imn].m_size)+BSIZE-1)/BSIZE)*BSIZE;
	if (BSIZE * ndump > bcnt)
		skip = dmplo = swplo + nswap - bcnt/BSIZE;

	udc = (struct ud_io *)ud_addr[ctl];
	cmdp = &cmdbuf[ctl];		/* point to command buffer */
	imn = 1;			/* initmap index */

	/* dump memory */
	while (initmap[imn].m_size && ndump) {
		bcnt = ((ctob(initmap[imn].m_size)+BSIZE-1)/BSIZE)*BSIZE;
		if (imn == 1)
			bcnt = ((ctob(initmap[1].m_size +
				initmap[1].m_addr)+BSIZE-1)/BSIZE)*BSIZE;
		bcnt = BSIZE*ndump>bcnt?bcnt:BSIZE*ndump;
					/* calculate how much to save */
		cmdp->ud_ssec =
			(ud_sizes[ctl][DEV(swapdev)][SLICE(swapdev)].blkoff+
			dmplo)*(BSIZE/BPS);	/* sector address */
		cmdp->ud_cnt = ((bcnt+(BSIZE-1))/BSIZE)*(BSIZE/BPS);
						/* # sectors */
		cmdp->ud_dev = DEV(swapdev);	/* device number */
		cmdp->ud_adr = (long)ctob(initmap[imn].m_addr);
		if (imn == 1)
			cmdp->ud_adr = (long)0;
						/* memory address */
		cmdp->ud_siz = PKTSIZ;		/* normal packet */
		cmdp->ud_fill1 = 0;		/* format clean up */
		cmdp->ud_cmd = WRITE;		/* WRITE */
		udc->ud_can = CLR;		/* clear can */
		while (udc->ud_cflg)		/* wait while udc busy */
			;
		udputp( cmdp, udc->ud_cmd, cmdp->ud_siz);
		udc->ud_can = CLR;		/* clear can */
		udc->ud_cflg = FLAG;		/* command flag */
		udc->ud_intr = FLAG;		/* interrupt UDC */
		while (udc->ud_can == 0)	/* wait for ACK */
			;
		if (udc->ud_can != ACK) {
			udc->ud_can = CLR;	/* clear can */
			printf("I/O error during dump.\n");
			return;
			}
		udc->ud_can = CLR;		/* clear can */
		while (udc->ud_sflg != FLAG)	/* wait for status */
			;
		udgetp( &statbuf, udc->ud_stat, sizeof(statbuf));
		udc->ud_sflg = CLR;		/* acknowledge status */
		udc->ud_san = ACK;
		udc->ud_intr = FLAG;
		if (((short)((statbuf.ud_stype<<8)+statbuf.ud_scode)) != OK0) {
			printf("I/O error during dump.\n");
			return;
			}
		imn++;				/* prepare for next loop */
		ndump -= ((cmdp->ud_cnt*BPS)/BSIZE);
		dmplo += ((cmdp->ud_cnt*BPS)/BSIZE);
	}

	/* done */
	printf("\nDump complete.  dd skip = %d, dd count = %d.\n",
		skip, dmplo-skip);
}
