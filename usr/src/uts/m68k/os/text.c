/* @(#)text.c	2.24	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/map.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/text.h"
#include "sys/inode.h"
#include "sys/buf.h"
#ifdef m68k
#include "sys/mmu.h"
#endif
#include "sys/seg.h"
#include "sys/var.h"
#include "sys/sysinfo.h"
#ifndef m68k
#include "sys/page.h"
#endif

/*
 * Swap out process p.
 * The ff flag causes its core to be freed--
 * it may be off when called to create an image for a
 * child process in newproc.
 * On a partial swap ff is the negative number of blocks to be swapped.
 * Os is the old size  of the process,
 * and is supplied during core expansion swaps.
 * Ss is the old stack size for core expansion swaps.
 */


xswap(p, ff, os, ss)
register struct proc *p;
{
#ifndef m68k
	register a, ntsize, gap;
	register int *ip;
#endif

	p->p_flag |= SLOCK;
	if (ff && (p == u.u_procp))
		printf("xswap %x\n",p);
	if (os == 0)
		os = p->p_size;
	if (ss == 0)
		ss = p->p_ssize;

#ifndef m68k
	gap = 128*p->p_nspt - p->p_tsize - os;
	if ((p->p_flag & SSPART) == 0) {
		a = swapalloc(ctod(p->p_size), 1);
		if (p->p_textp)
			xccdec(p->p_textp, p);
		p->p_swaddr = a;
		p->p_swsize = 0;
		ip = p->p_spt + p->p_tsize;
		bcopy(ip, ip + gap, sizeof(int *)*(os - ss));
	}
	ntsize = p->p_tsize + gap;
	if (os == p->p_size) {
		if (ff >= 0) {	/* do a complete swap */
			swap(p, p->p_swaddr, ntsize, p->p_size, B_WRITE);
			p->p_swsize = p->p_size;
		} else {
			p->p_flag |= SSPART;	/* partial  swap */
			swap(p, p->p_swaddr+p->p_swsize, ntsize + p->p_swsize,
			    -ff, B_WRITE);
			p->p_swsize += -ff;		/* new swap total */
			if (p->p_swsize == p->p_size) {
				a = p->p_swaddr;
				p->p_flag &= ~SSPART;
			}
		}
	} else {
		swap(p, a, ntsize, os-ss, B_WRITE);
		swap(p, a+p->p_size-ss, ntsize+os-ss, ss, B_WRITE);
		p->p_swsize = p->p_size;
	}
	p->p_flag &= ~SLOAD;
	if (ff) {
		if (ff > 0)
			memfree(p->p_spt + ntsize, os);
		else {
			memfree(p->p_spt + (ntsize+p->p_swsize+ff), -ff);
		}
		if ((p->p_flag & SSPART) == 0)
			sptfree(p->p_spt, p->p_nspt, 1);
	} else {
		ip = p->p_spt + p->p_tsize;
		bcopy(ip + gap, ip, sizeof(int *)*(os-ss));
		ip += os - ss;
		while (--gap >= 0)
			*ip++ = 0;
	}
	p->p_flag &= ~SLOCK;
#else
	if ((p->p_flag & SSPART) == 0) {
		p->p_swsize = 0;
		if (p->p_textp)	/* take care of text */
			xccdec(p->p_textp, p);
	}
	/*
	 * Any decision regarding partial swapouts are made
	 * in the MMU driver.
	 */
	mmudetach(p, SEG_DATA, os-ss, ff);
	mmudetach(p, SEG_STACK, ss, ff);
	p->p_flag &= ~(SLOAD|SLOCK);
#endif
	p->p_time = 0;
	if (runout) {
		runout = 0;
		wakeup((caddr_t)&runout);
	}
}

/*
 * relinquish use of the shared text segment
 * of a process.
 */
xfree()
{
	register struct text *xp;
	register struct inode *ip;
	register struct proc *p = u.u_procp;

	if ((xp = p->p_textp) == NULL)
		return;
	xlock(xp);
	xp->x_flag &= ~XLOCK;
#ifndef m68k
	p->p_textp = NULL;
#endif
	p->p_flag &= ~STEXT;
	ip = xp->x_iptr;
	if (--xp->x_count==0 && (ip->i_mode&ISVTX)==0) {
		xp->x_iptr = NULL;
		if (xp->x_daddr)
			mfree(swapmap, ctod(xp->x_size), xp->x_daddr);
#ifdef m68k
		xp->x_ccount = 0;
		xp->x_flag &= ~XWRIT;
		mmudetach( p, SEG_TEXT, 0, 1 );
#else
		memfree(p->p_spt, u.u_tsize);
#endif
		ip->i_flag &= ~ITEXT;
		if (ip->i_flag&ILOCK)
			ip->i_count--;
		else
			iput(ip);
	} else
		xccdec(xp, p);
#ifdef m68k
	p->p_textp = NULL;
#endif
}

/*
 * Attach to a shared text segment.
 * If there is no shared text, just return.
 * If there is, hook up to it:
 * if it is not currently being used, it has to be read
 * in from the inode (ip); the written bit is set to force it
 * to be written out as appropriate.
 * If it is being used, but is not currently in core,
 * a swap has to be done to get it back.
 */
xalloc(ip)
register struct inode *ip;
{
	register struct text *xp;
	register ts;
	register struct text *xp1;
	register struct proc *p = u.u_procp;

	if (u.u_exdata.ux_tsize == 0)
		return;
	xp1 = NULL;
loop:
	for (xp = &text[0]; xp < (struct text *)v.ve_text; xp++) {
		if (xp->x_iptr == NULL) {
			if (xp1 == NULL)
				xp1 = xp;
			continue;
		}
		if (xp->x_iptr == ip) {
			xlock(xp);
			xp->x_count++;
#ifdef m68k
			p->p_textp = xp;
#else
			u.u_procp->p_textp = xp;
#endif
			if (xp->x_ccount == 0)
				xexpand(xp);
			else {
				xp->x_ccount++;
#ifdef m68k
				p->p_flag |= STEXT;
				/* attach: save swap, don't allocate memory */
				mmuattach(p, SEG_TEXT, 0, 0);
#else
				u.u_procp->p_flag |= STEXT;
				if (xp->x_caddr == 0)
					panic("lost text");
				bcopy(xp->x_caddr->p_spt, p->p_spt, sizeof(int *)*xp->x_size);
#endif
			}
			xunlock(xp);
			return;
		}
	}
	if ((xp=xp1) == NULL) {
		printf("out of text\n");
		syserr.textovf++;
		if (xumount(NODEV))
			goto loop;
#ifdef m68k
		psignal(p, SIGKILL);
#else
		psignal(u.u_procp, SIGKILL);
#endif
		return;
	}
	xp->x_flag = XLOAD|XLOCK;
	xp->x_count = 1;
	xp->x_ccount = 0;
	xp->x_iptr = ip;
	ip->i_flag |= ITEXT;
	ip->i_count++;
	ts = btoc(u.u_exdata.ux_tsize);
	xp->x_size = ts;
	xp->x_daddr = 0;	/* defer swap alloc til later */
#ifdef m68k
	p->p_textp = xp;
#else
	u.u_procp->p_textp = xp;
#endif
	xexpand(xp);
#ifdef m68k
	mmuprot(SEG_TEXT, RW);
#else
	chgprot(RW);
#endif
	u.u_count = u.u_exdata.ux_tsize;
	u.u_offset = u.u_exdata.ux_tstart;
	u.u_base = 0;
	u.u_segflg =  SEG_TEXT;
#ifdef m68k
	p->p_flag |= SLOCK;
#else
	u.u_procp->p_flag |= SLOCK;
#endif
	readi(ip);
#ifdef m68k
	mmuprot(SEG_TEXT, RO);
	p->p_flag &= ~SLOCK;
#else
	chgprot(RO);
	u.u_procp->p_flag &= ~SLOCK;
#endif
	u.u_segflg = SEG_DATA;
	xp->x_flag = XWRIT;
}

/*
 * Assure core for text segment
 * Text must be locked to keep someone else from
 * freeing it in the meantime.
 * x_ccount must be 0.
 */
xexpand(xp)
register struct text *xp;
{
	register struct proc *p = u.u_procp;

#ifdef m68k
	if (mmuexpand(SEG_TEXT, btoc(u.u_exdata.ux_tsize)) == 0) {
		xp->x_caddr = p;
		if ((xp->x_flag&XLOAD)==0)
			/* attach: save swap, don't allocate memory */
			mmuattach(p, SEG_TEXT, 0, 0);
#else
	if (memall(p->p_spt, btoc(u.u_exdata.ux_tsize)) != NULL) {
		xp->x_caddr = u.u_procp;
		if ((xp->x_flag&XLOAD)==0)
			swap(u.u_procp, xp->x_daddr, 0, xp->x_size, B_READ);
#endif
		xp->x_ccount++;
		u.u_procp->p_flag |= STEXT;
#ifdef m68k
		mmuprot( SEG_TEXT, RO );
#else
		chgprot(RO);
#endif
		xunlock(xp);
		return;
	}
	xque(p, SXTXT, 0);
}

/*
 * Lock and unlock a text segment from swapping
 */
xlock(xp)
register struct text *xp;
{

	while(xp->x_flag&XLOCK) {
		xp->x_flag |= XWANT;
		sleep((caddr_t)xp, PSWP);
	}
	xp->x_flag |= XLOCK;
}

xunlock(xp)
register struct text *xp;
{

	if (xp->x_flag&XWANT)
		wakeup((caddr_t)xp);
	xp->x_flag &= ~(XLOCK|XWANT);
}

/*
 * Decrement the in-core usage count of a shared text segment.
 * When it drops to zero, free the core space.
 */
xccdec(xp, p)
register struct text *xp;
register struct proc *p;
{
	if (xp==NULL || xp->x_ccount==0)
		return;
	xlock(xp);
	p->p_flag &= ~STEXT;
	if (--xp->x_ccount==0) {
#ifdef m68k
		mmudetach (p, SEG_TEXT, xp->x_size, 1);
#else
		if (xp->x_flag&XWRIT) {
			if (xp->x_daddr == 0)
				xp->x_daddr = swapalloc(ctod(xp->x_size), 1);
			xp->x_flag &= ~XWRIT;
			swap(p, xp->x_daddr, 0, xp->x_size, B_WRITE);
		}
		memfree(p->p_spt, p->p_tsize);
		xp->x_caddr = 0;
#endif
	} else if (xp->x_caddr == p) {
		xp->x_caddr = 0;	/* Table no longer valid */
		for (p= &proc[1]; p<(struct proc *)v.ve_proc; p++)
			if (p->p_textp==xp && (p->p_flag&STEXT)) {
				xp->x_caddr = p;
				break;
			}
	}
	xunlock(xp);
}

/*
 * free the swap image of all unused saved-text text segments
 * which are from device dev (used by umount system call).
 */
xumount(dev)
register dev_t dev;
{
	register struct inode *ip;
	register struct text *xp;
	register count = 0;

	for (xp = &text[0]; xp < (struct text *)v.ve_text; xp++) {
		if ((ip = xp->x_iptr) == NULL)
			continue;
		if (dev != NODEV && dev != ip->i_dev)
			continue;
		if (xuntext(xp))
			count++;
	}
	return(count);
}

/*
 * remove a shared text segment from the text table, if possible.
 */
xrele(ip)
register struct inode *ip;
{
	register struct text *xp;

	if ((ip->i_flag&ITEXT) == 0)
		return;
	for (xp = &text[0]; xp < (struct text *)v.ve_text; xp++)
		if (ip==xp->x_iptr)
			xuntext(xp);
}

/*
 * remove text image from the text table.
 * the use count must be zero.
 */
xuntext(xp)
register struct text *xp;
{
	register struct inode *ip;

	xlock(xp);
	if (xp->x_count) {
		xunlock(xp);
		return(0);
	}
	ip = xp->x_iptr;
	xp->x_flag &= ~XLOCK;
	xp->x_iptr = NULL;
	if (xp->x_daddr)
		mfree(swapmap, ctod(xp->x_size), xp->x_daddr);
	ip->i_flag &= ~ITEXT;
	if (ip->i_flag&ILOCK)
		ip->i_count--;
	else
		iput(ip);
	return(1);
}

struct {
	int	x_w;
	int	x_c;
	struct proc *x_h, *x_t;
} xsp;

xsched()
{
	register struct proc *p;
	register arg;

	bcopy("xsched", u.u_comm, 7);
	for (;;) {
		if ((p = xsp.x_h) == NULL) {
			xsp.x_w++;
			sleep(&xsp.x_w, PSWP);
			continue;
		}
		xsp.x_c++;
		arg = p->p_arg;
		switch(p->p_stat) {
		case SXBRK:
			xswap(p, 1, p->p_size - arg, 0);
			break;
		case SXSTK:
			xswap(p, 1, p->p_size - arg, p->p_ssize - arg);
			break;
		case SXTXT:
			xswap(p, 1, 0, 0);
			xunlock(p->p_textp);
			break;
		}
		xsp.x_h = p->p_link;
		p->p_link = 0;
		p->p_arg = 0;
		if (p == xsp.x_t)
			xsp.x_t = 0;
		setrun(p);
	}
}

xque(p, st, arg)
register struct proc *p;
{
	p->p_stat = st;
	p->p_arg = arg;
	p->p_link = 0;
	if (xsp.x_t)
		xsp.x_t->p_link = p;
	else
		xsp.x_h = p;
	xsp.x_t = p;
	if (xsp.x_w) {
		xsp.x_w = 0;
		wakeup(&xsp.x_w);
	}
	swtch();
}

/*
 * allocate swap blocks, freeing and sleeping as necessary
 */
swapalloc(size, sflg)
{
	register addr;

	for (;;) {
		if (addr = malloc(swapmap, size))
			return(addr);
		if (swapclu()) {
			printf("\nWARNING: swap space running out\n");
			printf("  needed %d blocks\n", size);
			continue;
		}
		printf("\nDANGER: out of swap space\n");
		printf("  needed %d blocks\n", size);
		if (sflg) {
			mapwant(swapmap)++;
			sleep((caddr_t)swapmap, PSWP);
		} else
			return(0);
	}
}

/*
 * clean up swap used by text
 */
swapclu()
{
	register struct text *xp;
	register ans = 0;

	for (xp = text; xp < (struct text *)v.ve_text; xp++) {
		if (xp->x_iptr == NULL)
			continue;
		if (xp->x_flag&XLOCK)
			continue;
		if (xp->x_daddr == 0)
			continue;
		if (xp->x_count) {
			if (xp->x_ccount) {
				mfree(swapmap, ctod(xp->x_size), xp->x_daddr);
				xp->x_flag |= XWRIT;
				xp->x_daddr = 0;
				ans++;
			}
		} else {
			xuntext(xp);
			ans++;
		}
	}
	return(ans);
}
