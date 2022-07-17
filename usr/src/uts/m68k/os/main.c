/* @(#)main.c	2.20.1.2	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/filsys.h"
#include "sys/mount.h"
#include "sys/proc.h"
#include "sys/inode.h"
#include "sys/seg.h"
#include "sys/conf.h"
#include "sys/buf.h"
#include "sys/iobuf.h"
#include "sys/tty.h"
#include "sys/var.h"
#ifndef m68k
#include "sys/mtpr.h"
#include "sys/page.h"
#include "sys/clock.h"
#endif

/*
 * Initialization code.
 * Called from cold start routine as
 * soon as a stack and segmentation
 * have been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *
 * loop at low address in user mode -- /etc/init
 *	cannot be executed.
 */

struct inode *rootdir;
int	maxmem, physmem;
char	*bufstart;
struct buf *sbuf;

main()
{
	register i;
	register int (**initptr)();
	extern int (*init_tbl[])();
	extern icode[], szicode;
#ifdef m68k
	extern int (*dev_init[])();
#endif

	startup();

	/*
	 * set up system process
	 */

	proc[0].p_stat = SRUN;
	proc[0].p_flag |= SLOAD|SSYS;
	proc[0].p_nice = NZERO;
	u.u_procp = &proc[0];
	u.u_cmask = CMASK;
	u.u_limit = CDLIMIT;

	/*
	 * initialize devices and system tables
	 */

#ifdef m68k
	for (initptr= &dev_init[0]; *initptr; initptr++)
		(**initptr)();
#endif
	for (initptr= &init_tbl[0]; *initptr; initptr++)
		(**initptr)();

	msginit();
	seminit();

	rootdir = iget(rootdev, ROOTINO);
	rootdir->i_flag &= ~ILOCK;
	u.u_cdir = iget(rootdev, ROOTINO);
	u.u_cdir->i_flag &= ~ILOCK;
	u.u_rdir = NULL;
	u.u_start = time;

	/*
	 * create initial processes
	 * start scheduling task
	 */

	if (newproc(0)) {
#ifdef m68k
		mmuexpand( SEG_DATA, u.u_dsize = btoc(szicode));
		mmuregs(u.u_procp);
#else
		expand(btoc(szicode), P0BR);
		u.u_dsize = btoc(szicode);
#endif
		copyout((caddr_t)icode, (caddr_t)0, szicode);
		return(0);
	}
	if (newproc(0)) {
		register struct proc *p;
		extern	xsched();

		p = u.u_procp;
		maxmem -= (p->p_size + 1);
		p->p_flag |= SLOAD|SSYS;
		p->p_pid = 0;
		return((int)xsched);
	}
	sched();
}

/*
 * iinit is called once (from main) very early in initialization.
 * It reads the root's super block and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super block.
 * Usually because of an IO error.
 */
iinit()
{
	register struct buf *cp;
	register struct filsys *fp;
	struct inode iinode;

	(*bdevsw[bmajor(rootdev)].d_open)(minor(rootdev), 1);
	(*bdevsw[bmajor(pipedev)].d_open)(minor(pipedev), 1);
	(*bdevsw[bmajor(swapdev)].d_open)(minor(swapdev), 1);
	cp = geteblk();
	fp = cp->b_un.b_filsys;
	iinode.i_mode = IFBLK;
	iinode.i_rdev = rootdev;
	u.u_offset = SUPERBOFF;
	u.u_count = sizeof(struct filsys);
	u.u_base = (caddr_t)fp;
	u.u_segflg = SEG_SYS;
	readi(&iinode);
	if (u.u_error)
		panic("iinit");
	mount[0].m_bufp = cp;
	mount[0].m_flags = MINUSE;
	mount[0].m_dev = brdev(rootdev);
	if (fp->s_magic != FsMAGIC)
		fp->s_type = Fs1b;	/* assume old file system */
	if (fp->s_type == Fs2b)
		mount[0].m_dev |= Fs2BLK;
	rootdev = mount[0].m_dev;
	if (brdev(pipedev) == brdev(rootdev))
		pipedev = rootdev;
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_ronly = 0;
	fp->s_ninode = 0;
	fp->s_inode[0] = 0;

	clkset(fp->s_time);
}

/*
 * Initialize clist by freeing all character blocks.
 */
struct chead cfreelist;
cinit()
{
	register n;
	register struct cblock *cp;

	for(n = 0, cp = &cfree[0]; n < v.v_clist; n++, cp++) {
		cp->c_next = cfreelist.c_next;
		cfreelist.c_next = cp;
	}
	cfreelist.c_size = CLSIZE;
}

/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device hash buffer lists to empty.
 */
binit()
{
	register struct buf *bp;
	register struct buf *dp;
	register unsigned i;
	register char *buffers;
	int	bufsiz, hdsiz;

	dp = &bfreelist;
	dp->b_forw = dp->b_back =
	    dp->av_forw = dp->av_back = dp;
	hdsiz = btoc(sizeof(struct buf) * v.v_buf);
#ifdef m68k
	bp = (struct buf *) ctob(mmualloc( hdsiz ));
#else
	bp = (struct buf *)sptalloc(hdsiz, PG_V | PG_KW, 0);
#endif
	sbuf = bp;
	bufsiz = btoc(SBUFSIZE * v.v_buf);
#ifdef m68k
	buffers = (char *) ctob(mmualloc( bufsiz ));
#else
	buffers = (char *)sptalloc(bufsiz, PG_V | PG_KW, 0);
#endif
	if (bp == NULL || buffers == NULL)
		panic("binit");
	bufstart = buffers;
	maxmem -= (bufsiz + hdsiz);

	for (i=0; i<v.v_buf; i++,bp++,buffers += SBUFSIZE) {
		bp->b_dev = NODEV;
		bp->b_un.b_addr = buffers;
		bp->b_back = dp;
		bp->b_forw = dp->b_forw;
		dp->b_forw->b_back = bp;
		dp->b_forw = bp;
		bp->b_flags = B_BUSY;
		bp->b_bcount = 0;
		brelse(bp);
	}
	pfreelist.av_forw = bp = pbuf;
	for (; bp < &pbuf[v.v_pbuf-1]; bp++)
		bp->av_forw = bp+1;
	bp->av_forw = NULL;
	for (i=0; i < v.v_hbuf; i++)
		hbuf[i].b_forw = hbuf[i].b_back = (struct buf *)&hbuf[i];
}
