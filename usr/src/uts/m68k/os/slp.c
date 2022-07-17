/* @(#)slp.c	2.16	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/text.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/map.h"
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/buf.h"
#include "sys/var.h"
#ifndef m68k
#include "sys/page.h"
#include "sys/mtpr.h"
#endif
#include "sys/ipc.h"
#ifndef m68k
#include "sys/shm.h"
#endif
#include "sys/errno.h"

#define	NHSQUE	64	/* must be power of 2 */
#define	sqhash(X)	(&hsque[((int)X >> 3) & (NHSQUE-1)])
struct proc *hsque[NHSQUE];
char	runin, runout, runrun, curpri;
struct proc *curproc, *runq;

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * The most important effect of pri is that when
 * pri<=PZERO a signal cannot disturb the sleep;
 * if pri>PZERO signals will be processed.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
#define	TZERO	10
sleep(chan, disp)
caddr_t chan;
{
	register struct proc *rp = u.u_procp;
	register struct proc **q = sqhash(chan);
	register s;

	s = splhi();
	if (panicstr) {
		spl0();
		splx(s);
		return(0);
	}
	rp->p_stat = SSLEEP;
	rp->p_wchan = chan;
	rp->p_link = *q;
	*q = rp;
	if (rp->p_time > TZERO)
		rp->p_time = TZERO;
	if ((rp->p_pri = (disp&PMASK)) > PZERO) {
		if (rp->p_sig && issig()) {
			rp->p_wchan = 0;
			rp->p_stat = SRUN;
			*q = rp->p_link;
			spl0();
			goto psig;
		}
		spl0();
		if (runin != 0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
		swtch();
		if (rp->p_sig && issig())
			goto psig;
	} else {
		spl0();
		swtch();
	}
	splx(s);
	return(0);

	/*
	 * If priority was low (>PZERO) and there has been a signal,
	 * if PCATCH is set, return 1, else
	 * execute non-local goto to the qsav location.
	 */
psig:
	splx(s);
	if (disp&PCATCH)
		return(1);
	longjmp(u.u_qsav);
	/* NOTREACHED */
}

/*
 * Wake up all processes sleeping on chan.
 */
wakeup(chan)
register caddr_t chan;
{
	register struct proc *p;
	register struct proc **q;
	register s;


	s = splhi();
	for (q = sqhash(chan); p = *q; )
		if (p->p_wchan==chan && p->p_stat==SSLEEP) {
			p->p_stat = SRUN;
			p->p_wchan = 0;
			/* take off sleep queue, put on run queue */
			*q = p->p_link;
			p->p_link = runq;
			runq = p;
			if (!(p->p_flag&SLOAD)) {
				p->p_time = 0;
				/* defer setrun to avoid breaking link chain */
				if (runout > 0)
					runout = -runout;
			} else if (p->p_pri < curpri)
				runrun++;
		} else
			q = &p->p_link;
	if (runout < 0) {
		runout = 0;
		setrun(&proc[0]);
	}
	splx(s);
}

setrq(p)
register struct proc *p;
{
	register struct proc *q;
	register s;

	s = splhi();
	for(q=runq; q!=NULL; q=q->p_link)
		if (q == p) {
			printf("proc on q\n");
			goto out;
		}
	p->p_link = runq;
	runq = p;
out:
	splx(s);
}

/*
 * Set the process running;
 * arrange for it to be swapped in if necessary.
 */
setrun(p)
register struct proc *p;
{
	register struct proc **q;
	register s;

	s = splhi();
	if (p->p_stat == SSLEEP) {
		/* take off sleep queue */
		for (q = sqhash(p->p_wchan); *q != p; q = &(*q)->p_link) ;
		*q = p->p_link;
		p->p_wchan = 0;
	} else if (p->p_stat == SRUN) {
		/* already on run queue - just return */
		splx(s);
		return;
	}
	/* put on run queue */
	p->p_stat = SRUN;
	p->p_link = runq;
	runq = p;
	if (!(p->p_flag&SLOAD)) {
		p->p_time = 0;
		if (runout > 0) {
			runout = 0;
			setrun(&proc[0]);
		}
	} else if (p->p_pri < curpri)
		runrun++;
	splx(s);
}

/*
 * The main loop of the scheduling (swapping) process.
 * The basic idea is:
 *  see if anyone wants to be swapped in;
 *  swap out processes until there is room;
 *  swap him in;
 *  repeat.
 * The runout flag is set whenever someone is swapped out.
 * Sched sleeps on it awaiting work.
 *
 * Sched sleeps on runin whenever it cannot find enough
 * memory (by swapping out or otherwise) to fit the
 * selected swapped process.  It is awakened when the
 * memory situation changes and in any case once per second.
 */

sched()
{
	register struct proc *rp, *p;
	register outage, inage;
	register struct proc *inp;
	int maxbad;
	int tmp;

	/*
	 * find user to swap in;
	 * of users ready, select one out longest
	 */

loop:
	spl6();
	outage = -20000;
	for (rp = &proc[0]; rp < (struct proc *)v.ve_proc; rp++)
	if (rp->p_stat==SRUN && (rp->p_flag&SLOAD) == 0 &&
	    rp->p_time > outage) {
		p = rp;
		outage = rp->p_time;
	}
	/*
	 * If there is no one there, wait.
	 */
	if (outage == -20000) {
		runout++;
		sleep((caddr_t)&runout, PSWP);
		goto loop;
	}
	spl0();

	/*
	 * See if there is memory for that process;
	 * if so, swap it in.
	 */

	if (swapin(p))
		goto loop;

	/*
	 * none found.
	 * look around for memory.
	 * Select the largest of those sleeping
	 * at bad priority; if none, select the oldest.
	 */

	spl6();
	inp = p;		/* we are trying to swap this guy in */
	p = NULL;
	maxbad = 0;
	inage = 0;
	for (rp = &proc[0]; rp < (struct proc *)v.ve_proc; rp++) {
		if (rp->p_stat==SZOMB)
			continue;
		if (rp == inp)
			continue;
		if ((rp->p_flag&(SSPART|SLOCK)) == SSPART) {
			p = rp;
			maxbad = 1;
			break;
		}
		if ((rp->p_flag&(SSYS|SLOCK|SLOAD))!=SLOAD)
			continue;
		if (rp->p_textp && rp->p_textp->x_flag&XLOCK)
			continue;
		if (rp->p_stat==SSLEEP || rp->p_stat==SSTOP) {
			tmp = rp->p_pri - PZERO + rp->p_time;
			if (maxbad < tmp) {
				p = rp;
				maxbad = tmp;
			}
		} else if (maxbad<=0 && rp->p_stat==SRUN) {
			tmp = rp->p_time + rp->p_nice - NZERO;
			if (tmp > inage) {
				p = rp;
				inage = tmp;
			}
		}
	}
	spl0();
	/*
	 * Swap found user out if sleeping at bad pri,
	 * or if he has spent at least 2 seconds in memory and
	 * the swapped-out process has spent at least 2 seconds out.
	 * Otherwise wait a bit and try again.
	 */
	if (maxbad>0 || (outage>=2 && inage>=2)) {
		int	left;

		p->p_flag &= ~SLOAD;
		left = p->p_size;
		if (p->p_flag & SSPART)
			left -= p->p_swsize;
		if (left > SWAPSIZE)
			left = SWAPSIZE;
		xswap(p, -left, 0, 0);
		goto loop;
	}
	spl6();
	runin++;
	sleep((caddr_t)&runin, PSWP);
	goto loop;
}
/*
 * Swap a process in.
 */
swapin(p)
register struct proc *p;
{
#ifdef m68k
	register struct text *xp;

	/*
	 *	If there is sharing of text, swapin or connect to it.
	 */
	if (xp = p->p_textp) {
		xlock(xp);
		if (mmuattach( p, SEG_TEXT, 0, 1 ))	/* save swap space */
							/* allocate memory */
			goto nomem;
		xp->x_caddr = p;	/* this becomes control proc */
	}
	if (mmuattach(p, SEG_DATA, 1, 1)) {	/* free up swap space */
						/* allocate memory */
		if (xp)
			mmudetach (p, SEG_TEXT, xp->x_size, 1);
		goto nomem;
	}
	if (mmuattach(p, SEG_STACK, 1, 1)) {	/* free of swap space */
						/* allocate memory */
		mmudetach (p, SEG_DATA, p->p_size-p->p_ssize, 1);
		if (xp)
			mmudetach (p, SEG_TEXT, xp->x_size, 1);
		goto nomem;
	}
	if (xp) {
		xp->x_ccount++;
		p->p_flag |= STEXT;
		xunlock(xp);
	}

	p->p_flag |= SLOAD;
	p->p_time = 0;
	return(1);

nomem:
	xunlock(xp);	/* something must be xswapped */
	return(0);
#else
	extern struct user *uservad;
	register struct text *xp;
	register int i, *ip, *jp;
	int pt, tsize, tmem;
	int *utl;

	if (xp = p->p_textp) {
		xlock(xp);
		tsize = xp->x_size;
		tmem = xp->x_ccount == 0 ? tsize : 0;
	} else {
		tsize = p->p_tsize;
		tmem = 0;
	}	
	if (p->p_flag & SSPART) {
		pt = 0;
	} else {
		if (p->p_smbeg)
			pt = (p->p_smend + p->p_ssize + 127)>>7;
		else
			pt = (p->p_swsize + tsize + 127)>>7;	/*pages of page table */
		if ((ip = (int *)sptalloc(pt, PG_V | PG_KW, 0)) == NULL)
			goto nomem;
		p->p_nspt = pt;
		p->p_spt = ip;
	}
	ip = p->p_spt + p->p_nspt*128 - (p->p_size + tmem);
	utl = ip;
	if (memall(ip, p->p_swsize + tmem) == 0) {
		if (pt)
			sptfree(p->p_spt, p->p_nspt, 1);
		goto nomem;
	}
	for (i=tmem; --i>=0; )
		*ip++ |= PG_V|PG_URKR;
	for (i=p->p_size-USIZE; --i>=0; )
		*ip++ |= PG_V|PG_UW;
	for (i = USIZE; --i >= 0; )
		*ip++ |= PG_V | PG_KW;
	if (tmem) {
		bcopy(utl, p->p_spt, sizeof(int *)*tmem);
		utl += tmem;
	}
	if (xp) {
		if (xp->x_ccount == 0) {
			xp->x_caddr = p;	/* make link to loaded proc */
			if ((xp->x_flag&XLOAD) == 0)
				swap(p, xp->x_daddr, 0, xp->x_size, B_READ);
		} else {
			if (xp->x_caddr == 0)
				panic("lost text");
			bcopy(xp->x_caddr->p_spt, p->p_spt, sizeof(int *)*xp->x_size);
		}
		xp->x_ccount++;
		p->p_flag |= STEXT;
		xunlock(xp);
	}
	swap(p, p->p_swaddr, utl - p->p_spt, p->p_swsize, B_READ);
	mfree(swapmap, ctod(p->p_size), p->p_swaddr);
	p->p_flag &= ~SSPART;
	ip = p->p_spt + tsize;
	i = p->p_size - p->p_ssize;
	bcopy(utl, ip, sizeof(int *)*i);
	ip += i;
	jp = p->p_spt + p->p_nspt*128 - p->p_ssize;
	while (ip < jp)
		*ip++ = 0;
	jp = p->p_spt + p->p_nspt*128 - USIZE;
	p->p_addr = *jp;
	uaccess(jp);
	uservad->u_pcb.pcb_p0br = (int)p->p_spt;
	uservad->u_pcb.pcb_p1br = (int)(p->p_spt + p->p_nspt*128 - 0x200000);

	/* reattach shared memory */
	shmreset(p, uservad, uservad->u_pcb.pcb_p0br, uservad->u_pcb.pcb_p0lr&0x003fffff);
	p->p_flag |= SLOAD;
	p->p_time = 0;
	return(1);

nomem:
	if (xp)
		xunlock(xp);
	return(0);
#endif
}

/*
 * put the current process on
 * the Q of running processes and
 * call the scheduler.
 */
qswtch()
{

	setrq(u.u_procp);
	swtch();
}

/*
 * This routine is called to reschedule the CPU.
 * if the calling process is not in RUN state,
 * arrangements for it to restart must have
 * been made elsewhere, usually by calling via sleep.
 * There is a race here. A process may become
 * ready after it has been examined.
 * In this case, idle() will be called and
 * will return in at most 1HZ time.
 * i.e. its not worth putting an spl() in.
 */
swtch()
{
	register n;
	register struct proc *p, *q, *pp, *pq;

	/*
	 * If not the idle process, resume the idle process.
	 */
	sysinfo.pswitch++;
	if (u.u_procp != &proc[0]) {
		if (save(u.u_rsav)) {
			return;
		}
#ifdef m68k
		mmucswitch( u.u_procp, &proc[0] );
#endif
		resume(proc[0].p_addr, u.u_qsav);
	}
	/*
	 * The first save returns nonzero when proc 0 is resumed
	 * by another process (above); then the second is not done
	 * and the process-search loop is entered.
	 *
	 * The first save returns 0 when swtch is called in proc 0
	 * from sched().  The second save returns 0 immediately, so
	 * in this case too the process-search loop is entered.
	 * Thus when proc 0 is awakened by being made runnable, it will
	 * find itself and resume itself at rsav, and return to sched().
	 */
	if (save(u.u_qsav) == 0 && save(u.u_rsav))
		return;
	p = curproc;
	switch(p->p_stat) {
	case SZOMB:
#ifndef m68k
		memfree(p->p_spt + p->p_nspt*128 - USIZE, USIZE);
		sptfree(p->p_spt, p->p_nspt, 1);
#endif
		break;
	}
loop:
	spl6();
	runrun = 0;
	pp = NULL;
	q = NULL;
	n = 128;
	/*
	 * Search for highest-priority runnable process
	 */
	if (p = runq) do {
		if ((p->p_flag&SLOAD) && p->p_pri <= n) {
			pp = p;
			pq = q;
			n = p->p_pri;
		}
		q = p;
	} while (p = p->p_link);
	/*
	 * If no process is runnable, idle.
	 */
	p = pp;
	if (p == NULL) {
		curpri = PIDLE;
		curproc = &proc[0];
		idle();
		goto loop;
	}
	q = pq;
	if (q == NULL)
		runq = p->p_link;
	else
		q->p_link = p->p_link;
	curpri = n;
	curproc = p;
	spl0();
	/*
	 * The rsav (ssav) contents are interpreted in the new address space
	 */
	n = p->p_flag&SSWAP;
	p->p_flag &= ~SSWAP;
#ifdef m68k
	mmucswitch( &proc[0], p );
#endif
	resume(p->p_addr, n? u.u_ssav: u.u_rsav);
}

/*
 * Create a new process-- the internal version of
 * sys fork.
 * It returns 1 in the new process, 0 in the old.
 */
newproc(i)
{
	register struct proc *rpp, *rip;
	register n;
	register a;
	struct proc *pend;
	static mpid;

	/*
	 * First, just locate a slot for a process
	 * and copy the useful info from this process into it.
	 * The panic "cannot happen" because fork has already
	 * checked for the existence of a slot.
	 */
	rpp = NULL;
retry:
	mpid++;
	if (mpid >= MAXPID) {
		mpid = 0;
		goto retry;
	}
	rip = &proc[0];
	n = (struct proc *)v.ve_proc - rip;
	a = 0;
	do {
		if (rip->p_stat == NULL) {
			if (rpp == NULL)
				rpp = rip;
			continue;
		}
		if (rip->p_pid==mpid)
			goto retry;
		if (rip->p_uid == u.u_ruid)
			a++;
		pend = rip;
	} while(rip++, --n);
	if (rpp==NULL) {
		if ((struct proc *)v.ve_proc >= &proc[v.v_proc]) {
			if (i) {
				syserr.procovf++;
				u.u_error = EAGAIN;
				return(-1);
			} else
				panic("no procs");
		}
		rpp = (struct proc *)v.ve_proc;
	}
	if (rpp > pend)
		pend = rpp;
	pend++;
	v.ve_proc = (char *)pend;
	if (u.u_uid && u.u_ruid) {
		if (rpp == &proc[v.v_proc-1] || a > v.v_maxup) {
			u.u_error = EAGAIN;
			return(-1);
		}
	}
	/*
	 * make proc entry for new proc
	 */

	rip = u.u_procp;
	rpp->p_stat = SRUN;
	rpp->p_clktim = 0;
	rpp->p_flag = SLOAD;
	rpp->p_uid = rip->p_uid;
	rpp->p_suid = rip->p_suid;
	rpp->p_pgrp = rip->p_pgrp;
	rpp->p_nice = rip->p_nice;
	rpp->p_textp = rip->p_textp;
	rpp->p_pid = mpid;
	rpp->p_ppid = rip->p_pid;
	rpp->p_time = 0;
	rpp->p_cpu = rip->p_cpu;
	rpp->p_pri = PUSER + rip->p_nice - NZERO;
#ifndef m68k
	rpp->p_spt = rip->p_spt;
	rpp->p_nspt = rip->p_nspt;
#endif
	rpp->p_addr = rip->p_addr;
	rpp->p_tsize = rip->p_tsize;
	rpp->p_ssize = rip->p_ssize;

	/*
	 * make duplicate entries
	 * where needed
	 */

	for(n=0; n<NOFILE; n++)
		if (u.u_ofile[n] != NULL)
			u.u_ofile[n]->f_count++;
	if (rpp->p_textp != NULL) {
		rpp->p_textp->x_count++;
		rpp->p_textp->x_ccount++;
		rpp->p_flag |= STEXT;
	}
	u.u_cdir->i_count++;
	if (u.u_rdir)
		u.u_rdir->i_count++;

	shmfork(rpp, rip);

	/*
	 * Partially simulate the environment
	 * of the new process so that when it is actually
	 * created (by copying) it will look right.
	 */
	u.u_procp = rpp;
	curproc = rpp;
	rpp->p_size = rip->p_size;
	/*
	 * When the resume is executed for the new process,
	 * here's where it will resume.
	 */
	if (save(u.u_ssav)) {
		return(1);
	}
#ifdef m68k
	/*
	 * Call mmufork to duplicate the process.
	 */
	mmufork(rip);
#else
	/*
	 * If there is not enough memory for the
	 * new process, swap out the current process to generate the
	 * copy.
	 */
	if (procdup(rpp) == NULL) {
		rip->p_stat = SIDL;
		xswap(rpp, 0, 0, 0);

		/* reset shared memory page table */
		shmreset(rip, &u, mfpr(P0BR), mfpr(P0LR));
		rip->p_stat = SRUN;
	}
	rpp->p_addr = rpp->p_spt[rpp->p_nspt*128 - USIZE] & PG_PFNUM;
#endif
	u.u_procp = rip;
	curproc = rip;
	setrq(rpp);
	rpp->p_flag |= SSWAP;
	u.u_rval1 = rpp->p_pid;		/* parent returns pid of child */
	return(0);
}

#ifndef m68k
/*
 * SETUP VAX REGIONS
 * If there is no memory, arrange for the process to be swapped
 * out after adjusting the size requirement-- when it comes
 * in, enough memory will be allocated.
 *
 * After the expansion, the caller will take care of copying
 * the user's stack towards or away from the data area.
 */
expand(change, region)
register change;
{
	register lr, mc;
	register int *base;
	register struct proc *p = u.u_procp;

	if (change == 0)
		return;
	lr = (int *)mfpr(P0BR) - (int *)mfpr(P1BR);
	lr += (mfpr(P0LR) - mfpr(P1LR)) + change;
	if (lr > 0)
		mc = ptexpand((lr + 127)/128);
	else
		mc = 1;
	switch(region) {
	case P0BR:
		p->p_size += change;
		base = (int *)mfpr(P0BR);
		lr = mfpr(P0LR);
		base += lr;
		lr += change;
		mtpr(P0LR, lr);
		u.u_pcb.pcb_p0lr = lr | 0x04000000;
		if (change < 0) {
			base += change;
			memfree(base, -change);
			mtpr(TBIA, 0);
			return;
		}
		goto com;
	case P1BR:
		p->p_size += change;
		p->p_ssize += change;
		base = (int *)mfpr(P1BR);
		lr = mfpr(P1LR);
		base += lr;
		lr -= change;
		mtpr(P1LR, lr);
		u.u_pcb.pcb_p1lr = lr;
		if (change < 0) {
			memfree(base, -change);
			mtpr(TBIA, 0);
			return;
		}
		base -= change;
	com:
		if (mc > 0)
			mc = memall(base, change);
		if (mc <= 0) {
			xque(p, (region==P0BR)?SXBRK:SXSTK, change);
			return;
		}
		while (--change >= 0)
			*base++ |= PG_V | PG_UW;
		return;
	case P0LR:
		p->p_tsize += change;
		lr = mfpr(P0LR) + change;
		mtpr(P0LR, lr);
		u.u_pcb.pcb_p0lr = lr | 0x04000000;
		if (mc <= 0)
			xque(p, SXBRK, 0);
		return;
	}
}
#endif
