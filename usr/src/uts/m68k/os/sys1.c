/* @(#)sys1.c	2.26.1.3	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/map.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#include "sys/buf.h"
#include "sys/reg.h"
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/seg.h"
#include "sys/acct.h"
#include "sys/sysinfo.h"
#include "sys/var.h"
#ifndef m68k
#include "sys/page.h"
#include "sys/mtpr.h"
#endif
#include "sys/ipc.h"
#ifndef m68k
#include "sys/shm.h"
#endif


/*
 * exec system call, with and without environments.
 */
struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

exec()
{
	((struct execa *)u.u_ap)->envp = NULL;
	exece();
}

#define	NCABLK	(NCARGS+BSIZE-1)/BSIZE
exece()
{
	register unsigned nc;
	register char *cp;
	register struct buf *bp;
	register struct execa *uap;
	int na, ne, ucp, ap, c;
	unsigned bno;
	struct inode *ip;
	extern struct inode *gethead();

	sysinfo.sysexec++;
	if ((ip = gethead()) == NULL)
		return;
	bp = 0;
	na = nc = ne = 0;
	uap = (struct execa *)u.u_ap;
	/* collect arglist */
	if ((bno = swapalloc(NCABLK, 0)) == 0) {
		printf("No swap space for exec args\n");
		iput(ip);
		u.u_error = ENOMEM;
		return;
	}
	if (uap->argp) for (;;) {
		ap = NULL;
		if (uap->argp) {
			ap = fuword((caddr_t)uap->argp);
			uap->argp++;
		}
		if (ap==NULL && uap->envp) {
			uap->argp = NULL;
			if ((ap = fuword((caddr_t)uap->envp)) == NULL)
				break;
			uap->envp++;
			ne++;
		}
		if (ap==NULL)
			break;
		na++;
		if (ap == -1)
			u.u_error = EFAULT;
		do {
			if (nc >= NCARGS-1)
				u.u_error = E2BIG;
			if ((c = fubyte((caddr_t)ap++)) < 0)
				u.u_error = EFAULT;
			if (u.u_error)
				goto bad;
			if ((nc&BMASK) == 0) {
				if (bp)
					bdwrite(bp);
				bp = getblk(swapdev, swplo+bno+(nc>>BSHIFT));
				cp = bp->b_un.b_addr;
			}
			nc++;
			*cp++ = c;
		} while (c>0);
	}
	if (bp)
		bdwrite(bp);
	bp = 0;
	nc = (nc + NBPW-1) & ~(NBPW-1);
	getxfile(ip, nc + sizeof(char *)*na);
	if (u.u_error) {
		psignal(u.u_procp, SIGKILL);
		goto bad;
	}

	/* copy back arglist */

	ucp = USRSTACK - nc - NBPW;
	ap = ucp - na*NBPW - 3*NBPW;
	u.u_ar0[SP] = ap;
	suword((caddr_t)ap, na-ne);
	nc = 0;
	for (;;) {
		ap += NBPW;
		if (na==ne) {
			ap += NBPW;
		}
		if (--na < 0)
			break;
		suword((caddr_t)ap, ucp);
		do {
			if ((nc&BMASK) == 0) {
				if (bp)
					brelse(bp);
				bp = bread(swapdev, swplo+bno+(nc>>BSHIFT));
				bp->b_flags |= B_AGE;
				bp->b_flags &= ~B_DELWRI;
				cp = bp->b_un.b_addr;
			}
			subyte((caddr_t)ucp++, (c = *cp++));
			nc++;
		} while (c&0377);
	}
	setregs();
	if (bp)
		brelse(bp);
	iput(ip);
	mfree(swapmap, NCABLK, bno);
	return;
bad:
	if (bp)
		brelse(bp);
	iput(ip);
	for (nc = 0; nc < NCABLK; nc++) {
		bp = getblk(swapdev, swplo+bno+nc);
		bp->b_flags |= B_AGE;
		bp->b_flags &= ~B_DELWRI;
		brelse(bp);
	}
	mfree(swapmap, NCABLK, bno);
}

struct inode *
gethead()
{
	register struct inode *ip;
	register unsigned ds, ts;
	register sep;

	struct	naout	{
		short	magic;
		short	vstamp;
		long	tsize,
			dsize,
			bsize,
			entry,
			ts,
			ds;
	};

	struct	filhd	{
		unsigned short	magic;
		unsigned short	nscns;
		long		timdat,
				symptr,
				nsyms;
		unsigned short	opthdr,
				flags;
	} ;

	struct	scnhdr	{
		char		s_name[8];
		long		s_paddr,
				s_vaddr,
				s_size,
				s_scnptr,
				s_relptr,
				s_lnnoptr;
		unsigned short	s_nreloc,
				s_nlnno;
		long		s_flags;
	};

	if ((ip = namei(uchar, 0)) == NULL)
		return(NULL);
	if (access(ip, IEXEC) ||
	   (ip->i_mode & IFMT) != IFREG ||
	   (ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0) {
		u.u_error = EACCES;
		goto bad;
	}
	/*
	 * read in first few bytes of file for segment sizes
	 * ux_mag = 407/410/411
	 *  407 is plain executable
	 *  410 is RO text
	 *  411 is separated ID
	 *  570 Common object
	 *  575 "
	 *  set ux_tstart to start of text portion
	 */
	u.u_base = (caddr_t)&u.u_exdata;
	u.u_count = sizeof(u.u_exdata);
	u.u_offset = 0;
	u.u_segflg = SEG_SYS;
	readi(ip);
#ifdef m68k
	if (u.u_exdata.ux_mag == 0520) {
#else
	if (u.u_exdata.ux_mag == 0570 || u.u_exdata.ux_mag == 0575) {
#endif
		u.u_base = (caddr_t)&u.u_exdata;
		u.u_count = sizeof(u.u_exdata);
		u.u_offset = sizeof(struct filhd);
		u.u_segflg = SEG_SYS;
		readi(ip);
		u.u_exdata.ux_tstart = sizeof(struct naout) +
			sizeof(struct filhd) + (3 * sizeof(struct scnhdr));
		u.u_exdata.ux_entloc = u.u_exdata.ux_ssize;
	} else {
		u.u_exdata.ux_tstart = sizeof(u.u_exdata);
	}
	u.u_segflg = SEG_DATA;
	if (u.u_count!=0)
		u.u_exdata.ux_mag = 0;
	sep = 0;
	if (u.u_exdata.ux_mag == 0407) {
		ds = btoc((long)u.u_exdata.ux_tsize +
			(long)u.u_exdata.ux_dsize +
			(long)u.u_exdata.ux_bsize);
		ts = 0;
		u.u_exdata.ux_dsize += u.u_exdata.ux_tsize;
		u.u_exdata.ux_tsize = 0;
	} else {
		ts = btoc(u.u_exdata.ux_tsize);
		ds = btoc(u.u_exdata.ux_dsize+u.u_exdata.ux_bsize);
		if ((ip->i_flag&ITEXT)==0 && ip->i_count!=1) {
			register struct file *fp;

			for (fp = file; fp < (struct file *)v.ve_file; fp++)
				if (fp->f_count && fp->f_inode == ip &&
				    (fp->f_flag&FWRITE)) {
					u.u_error = ETXTBSY;
					goto bad;
				}
		}
		if (u.u_exdata.ux_mag == 0411)
			sep++;
		else if (u.u_exdata.ux_mag != 0410) {
			u.u_error = ENOEXEC;
			goto bad;
		}
	}
	chksize(ts,ds,SSIZE+btoc(NCARGS-1));
bad:
	if (u.u_error) {
		iput(ip);
		ip = NULL;
	}
	return(ip);
}

/*
 * Read in and set up memory for executed file.
 */
getxfile(ip, nargc)
register struct inode *ip;
{
	register unsigned ds;
	register i;
	register struct pt_entry *ptaddr;
	register unsigned ts, ss;

	shmexec();
	punlock();
	xfree();
	{
		ts = btoc(u.u_exdata.ux_tsize);
		ds = btoc(u.u_exdata.ux_dsize + u.u_exdata.ux_bsize);
		ss = SSIZE + USIZE + btoc(nargc);
		u.u_prof.pr_scale = 0;
#ifndef m68k
		expand(-u.u_dsize, P0BR);
		u.u_dsize = 0;
		expand(-(u.u_ssize-USIZE), P1BR);
		u.u_ssize = USIZE;
		expand(ts-u.u_tsize, P0LR);
		u.u_tsize = ts;
		expand(ss-USIZE, P1BR);
		u.u_ssize = ss;
		expand(ds, P0BR);
		u.u_dsize = ds;
		mtpr(TBIA,0);
		ptaddr = (struct pt_entry *)mfpr(P0BR);
		ptaddr += ts + btoct(u.u_exdata.ux_dsize);
		i = ds - btoct(u.u_exdata.ux_dsize);
		while (--i >= 0) {
			clearseg(ptaddr->pg_pfnum);	/* clear bss segment */
			ptaddr++;
		}
		ptaddr = (struct pt_entry *)(mfpr(P1BR) + 4*mfpr(P1LR));
		for (i=ss-USIZE; --i>=0; ) {
			clearseg(ptaddr->pg_pfnum);	/* clear stack */
			ptaddr++;
		}
		xalloc(ip);
#else
		mmuexec( ip, ts, ds, ss );
#endif
		/* read in data segment */
		u.u_base = (char *)ctob(ts);
		u.u_offset = u.u_exdata.ux_tstart + u.u_exdata.ux_tsize;
		u.u_count = u.u_exdata.ux_dsize;
		readi(ip);
		if (u.u_count!=0)
			u.u_error = EFAULT;
		/*
		 * set SUID/SGID protections, if no tracing
		 */
		if ((u.u_procp->p_flag&STRC)==0) {
			if (ip->i_mode&ISUID)
				u.u_uid = ip->i_uid;
			if (ip->i_mode&ISGID)
				u.u_gid = ip->i_gid;
			u.u_procp->p_suid = u.u_uid;
		} else
			psignal(u.u_procp, SIGTRAP);
	}
}

/*
 * Clear registers on exec
 */
setregs()
{
	register int *rp;
	register i;

	for (rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++)
		if ((*rp & 1) == 0)
			*rp = 0;
#ifndef m68k
	u.u_ar0[AP] = 0;
	u.u_ar0[FP] = 0;
	u.u_ar0[PC] = u.u_exdata.ux_entloc + 2; /* skip over entry mask */
#else
	for (i = 0; i < 15; i++)
		u.u_ar0[i] = 0;
	u.u_ar0[PC] = u.u_exdata.ux_entloc;
	/* so 68000 user can know stack limit */
	u.u_rval1 = u.u_ar0[R8] = USRSTACK - ctob(u.u_procp->p_ssize - USIZE);
	u.u_rval2 = 0;
#endif
	for (i=0; i<NOFILE; i++) {
		if ((u.u_pofile[i]&EXCLOSE) && u.u_ofile[i] != NULL) {
			closef(u.u_ofile[i]);
			u.u_ofile[i] = NULL;
		}
	}
	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag &= ~AFORK;
	bcopy((caddr_t)u.u_dent.d_name, (caddr_t)u.u_comm, DIRSIZ);
}

/*
 * exit system call:
 * pass back caller's arg
 */
rexit()
{
	register struct a {
		int	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0377) << 8);
}

/*
 * Release resources.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
exit(rv)
{
	register int i;
	register struct proc *p, *q;

	p = u.u_procp;
	p->p_flag &= ~(STRC);
	p->p_clktim = 0;
	for (i=0; i<NSIG; i++)
		u.u_signal[i] = 1;
	if ((p->p_pid == p->p_pgrp)
	 && (u.u_ttyp != NULL)
	 && (*u.u_ttyp == p->p_pgrp)) {
		*u.u_ttyp = 0;
		signal(p->p_pgrp, SIGHUP);
	}
	p->p_pgrp = 0;
	for (i=0; i<NOFILE; i++) {
		if (u.u_ofile[i] != NULL)
			closef(u.u_ofile[i]);
	}
	punlock();
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	xfree();

	semexit();
	shmexit();

	acct(rv);
#ifdef m68k
	mmuexit (p);
#else
	memfree(p->p_spt + u.u_tsize, u.u_dsize);
	memfree(p->p_spt + p->p_nspt*128 - u.u_ssize, u.u_ssize - USIZE);
#endif

	p->p_stat = SZOMB;
	((struct xproc *)p)->xp_xstat = rv;
	((struct xproc *)p)->xp_utime = u.u_cutime + u.u_utime;
	((struct xproc *)p)->xp_stime = u.u_cstime + u.u_stime;
	for (q = &proc[1]; q < (struct proc *)v.ve_proc; q++) {
		if (p->p_pid == q->p_ppid) {
			q->p_ppid = 1;
			if (q->p_stat == SZOMB)
				psignal(&proc[1], SIGCLD);
			if (q->p_stat == SSTOP)
				setrun(q);
		} else
		if (p->p_ppid == q->p_pid)
			psignal(q, SIGCLD);
		if (p->p_pid == q->p_pgrp)
			q->p_pgrp = 0;
	}
	resume(proc[0].p_addr, u.u_qsav);
	/* no deposit, no return */
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
wait()
{
	register f;
	register struct proc *p;

loop:
	f = 0;
	for (p = &proc[1]; p < (struct proc *)v.ve_proc; p++)
	if (p->p_ppid == u.u_procp->p_pid) {
		f++;
		if (p->p_stat == SZOMB) {
			freeproc(p, 1);
			return;
		}
		if (p->p_stat == SSTOP) {
			if ((p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_rval1 = p->p_pid;
				u.u_rval2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if (f) {
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

/*
 * Remove zombie children from the process table.
 */
freeproc(p, flag)
register struct proc *p;
{

	if (flag) {
		register n;

		n = u.u_procp->p_cpu + p->p_cpu;
		if (n > 80)
			n = 80;
		u.u_procp->p_cpu = n;
		u.u_rval1 = p->p_pid;
		u.u_rval2 = ((struct xproc *)p)->xp_xstat;
	}
	u.u_cutime += ((struct xproc *)p)->xp_utime;
	u.u_cstime += ((struct xproc *)p)->xp_stime;
	p->p_stat = NULL;
	p->p_pid = 0;
	p->p_ppid = 0;
	p->p_sig = 0L;
	p->p_flag = 0;
	p->p_wchan = 0;
}

/*
 * fork system call.
 */
fork()
{
	register a;

	sysinfo.sysfork++;
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned; or
	 *  not su and would take last slot; or
	 *  not su and no space on swap.
	 * Part of check done in newproc().
	 */
	if (u.u_uid && u.u_ruid) {
		if ((a = malloc(swapmap, ctod(MAXMEM))) == 0) {
			u.u_error = ENOMEM;
			goto out;
		}
		mfree(swapmap, ctod(MAXMEM), a);
	}
	switch( newproc(1) ) {
		case 1: /* child  -- successful newproc */
			u.u_rval1 = u.u_procp->p_ppid;
			u.u_rval2 = 1;  /* child */
			u.u_start = time;
			u.u_ticks = lbolt;
			u.u_mem = u.u_procp->p_size;
			u.u_ior = u.u_iow = u.u_ioch = 0;
			u.u_cstime = 0;
			u.u_stime = 0;
			u.u_cutime = 0;
			u.u_utime = 0;
			u.u_acflag = AFORK;
			return;
		case 0: /* parent -- successful newproc */
			/* u.u_rval1 = pid-of-child; */
			break;
		default: /* unsuccessful newproc */
			u.u_error = EAGAIN;
			break;
	}
out:
	u.u_rval2 = 0; /* parent */
}

/*
 * break system call.
 *  -- bad planning: "break" is a dirty word in C.
 */
sbreak()
{
	struct a {
		unsigned nsiz;
	};
	register  n, d;
#ifndef m68k
	register struct pt_entry *ptaddr;
#endif

	/*
	 * set n to new data size
	 * set d to new-old
	 */

	n = btoc(((struct a *)u.u_ap)->nsiz);
	if (!u.u_sep)
		n -= ctos(u.u_tsize) * stoc(1);
	if (n < 0)
		n = 0;
	if ((d = n - u.u_dsize) == 0)
		return;

#ifndef m68k
	if (u.u_procp->p_smbeg) {
		ptaddr = (struct pt_entry *)mfpr(P0BR) + u.u_tsize + u.u_dsize;
		if (d<0) {
			ptaddr += d;
			memfree(ptaddr, -d);
			u.u_dsize += d;
			u.u_procp->p_size += d;
			while (d++)
				*(int *)ptaddr++ = 0;
			return;
		}
		if ((u.u_tsize + u.u_dsize + d) > u.u_procp->p_smbeg) {
			u.u_error = ENOMEM;
			return;
		}
		if (memall(ptaddr, d) == 0) {
			u.u_error = ENOMEM;
			return;
		} else {
			u.u_dsize += d;
			u.u_procp->p_size += d;
			while (d--) {
				*(int *)ptaddr |= PG_V | PG_UW;
				clearseg(ptaddr++->pg_pfnum);
			}
		}
		return;
	}

	if (chksize(u.u_tsize, u.u_dsize+d, u.u_ssize))
		return;
	u.u_dsize += d;
	expand(d, P0BR);
	if (d>0) {	/* bigger */
		ptaddr = (struct pt_entry *)mfpr(P0BR);
		ptaddr += u.u_tsize + u.u_dsize - d;	/* point at first new page */
		while (d--)
			clearseg(ptaddr++->pg_pfnum);
	}
}
#else

	if (mmuexpand (SEG_DATA, d) < 0) {
		u.u_error = ENOMEM;
		return;
	}
	mmuregs(u.u_procp);

	u.u_dsize += d;
#endif
}
