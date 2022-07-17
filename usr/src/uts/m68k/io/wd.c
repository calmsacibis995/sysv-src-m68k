/* @(#)wd.c	2.1	 */
/*
 * Motorola Winchester Disk Controller (WDC) Driver (M68KRWIN1)
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

#define DEBUG	2


struct size
{
	daddr_t nblocks;	/* size of slice in blocks */
	daddr_t blkoff;		/* first physical block in slice */
};

extern struct size ud_sizes[][8];


struct ud_cmdp			/* command packet */
{
	unsigned long	ud_cmd;		/*  command */
	unsigned long	ud_cnt;		/*  number of sectors to transfer */
	unsigned long	ud_adr;		/*  memory address */
	unsigned long	ud_ssec;	/*  starting sector for io */
	unsigned long	ud_dev;		/*  device number */
};

extern int ud_addr[];

#define	MAXDEV		7
#define	DEV(x)		((x>>3)&07)
#define	SLICE(x)	(x&07)

#define	ERRCNT	10
#define	ERRFLAG	1
#define	NOERR	0

#define	CYL	128

int	errlog	= ERRCNT;		/* for error handling */

struct	iotime	udstat[8];	/* one per device */
struct	iotime	udcon[1];	/* one per controller */

struct	iobuf	udtab = tabinit(UD0,&udcon[0].ios);
#define	sa(X)	tabinit(UD0,&udstat[X].ios)
struct	iobuf	udutab[8] = {
	sa(0), sa(1), sa(2), sa(3), sa(4), sa(5), sa(6), sa(7)
};


#define	READ	0	/* READ */
#define	WRITE	1	/* WRITE */

#define	BPS	0x0100	/* Bytes Per Sector */


/*
 * The following variable contains the most recent command given by udstart.
 */

struct ud_cmdp cmdbuf;


#define	acts	io_s1
#define	qcnt	io_s2

#define	pblk	av_back
#define	cylin	b_resid

/*
 * The controller is considered busy while it is working on a request and
 * not all data is transferred.
 */

udopen(dev)
{
}

udclose(dev)
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

	unit = DEV(bp->b_dev);
#if DEBUG == 2
	udprint(bp->b_dev, "ud:strategy ");
#endif
	sp = &ud_sizes[unit][SLICE(bp->b_dev)];
	last = sp->nblocks;
	if (bp->b_blkno < 0 || 
	   (bp->b_blkno+(bp->b_bcount+BSIZE-1)/BSIZE) > last) {
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
	ip = &udstat[unit];
	ip->io_cnt++;
	ip->io_bcnt += btoc(bp->b_bcount);
	dp = &udutab[unit];

	/* put the io request into the device queue */

	dp->qcnt++;		/* increment count of io requests in queue */
	if (dp->b_actf == NULL) {
		dp->b_actf = bp;	/* if empty queue, add at front */
		dp->b_actl = bp;
		dp->acts = (int)bp;
	} else {		/* else in head motion optimized order */
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
	udstart(dp);		/* start io (temporarily pass dp) */
}

udstart(dp)
	register struct iobuf *dp;
{
	register struct buf *bp;

	/* if device pointer is bogus or device has no io queued */
	if ( (dp == NULL) || ( (bp = dp->b_actf) == NULL) )
		{
		printf("udstart: no iobuf or buf pointer\n");
		while(1);		/* fatal, hang here */
	}

	bp = dp->b_actf;

	/* give a command */

	/* save device iobuf pointer for udintr */
	udtab.b_errcnt = ERRCNT;

	/* start I/O */
	cmdbuf.ud_dev = (long)DEV(bp->b_dev);	/* drive */

	 /* first sector to read or write */
	cmdbuf.ud_ssec = (int)bp->pblk*(BSIZE/BPS);

	 /* count of sectors to read or write */
	cmdbuf.ud_cnt = ((bp->b_bcount+(BSIZE-1))/BSIZE)*(BSIZE/BPS);

	 /* command */
	if( bp->b_flags&B_READ )
		cmdbuf.ud_cmd = READ;		/* READ */
	else 
		cmdbuf.ud_cmd = WRITE;		/* WRITE */
	cmdbuf.ud_adr = (long)bp->b_un.b_addr;	/* memory address */

#ifdef DEBUG
	printf("ud: cmd=%x drv=%d cnt=%x pblk=%x adr=%x",
	cmdbuf.ud_cmd, cmdbuf.ud_dev, cmdbuf.ud_cnt/2,
	cmdbuf.ud_ssec/2, cmdbuf.ud_adr);
#endif

	blkacty |= (1<<UD0);
	udtab.b_active++;			/* controller active */

	/* send packet */
	if (wdio( &cmdbuf))
		{
		udcomp( dp, NOERR);
#ifdef DEBUG
		printf(" good io\n");
#endif
		}
	else
		{
		udcomp( dp, ERRFLAG);
#ifdef DEBUG
		printf(" bad io\n");
#endif
		}
}

udintr(dev)		/* stubbed out for temporary driver */
{
}



udcomp(dp,errflag)		/* I/O complete processing */
register struct iobuf *dp;
register errflag;
{
	register struct buf *bp;
	register struct iotime *ip;


	bp = dp->b_actf;			/* get buf */
	if (dp->io_erec)
		logberr(dp,bp->b_flags&B_ERROR);
	if (udtab.io_erec)
		logberr(&udtab,bp->b_flags&B_ERROR);
	dp->b_errcnt = 0;
	dp->b_actf = bp->av_forw;	/* take serviced request off io queue */
	dp->qcnt--;
	if (errflag) {
		bp->b_resid = bp->b_bcount;
		bp->b_flags |= B_ERROR;
	}
	else	bp->b_resid = 0;

	/* if this is the buf the insert starts at, pick a new starting buf */
	if (bp == (struct buf *)dp->acts)
		dp->acts = (int)dp->b_actf;

	ip = &udstat[DEV(bp->b_dev)];
	ip->io_resp += lbolt - bp->b_start;
	ip->io_act += lbolt - dp->io_start;
	iodone(bp);	/* release buffer */

}


		
udread(dev)
{
	/* guarantee block allignment of both offset and count */
	if (u.u_offset & BMASK || u.u_count & BMASK) {
		u.u_error = EINVAL;
		return;
	}
	if (physck(ud_sizes[DEV(dev)][SLICE(dev)].nblocks, B_READ))
		physio(udstrategy, 0, dev, B_READ);
}

udwrite(dev)
{
	/* guarantee block allignment of both offset and count */
	if (u.u_offset & BMASK || u.u_count & BMASK) {
		u.u_error = EINVAL;
		return;
	}
	if (physck(ud_sizes[DEV(dev)][SLICE(dev)].nblocks, B_WRITE))
		physio(udstrategy, 0, dev, B_WRITE);
}


udclr()
{
	register unit;

	udtab.b_active = 0;
	udtab.b_actf = NULL;	/* no devices in queue */
}


udprint(dev, str)
char *str;
{
	printf("%s on UDC drive %d, slice %d\n", str, DEV(dev), SLICE(dev));
}


uddump()
{
}
