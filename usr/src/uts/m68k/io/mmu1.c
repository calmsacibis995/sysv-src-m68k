/*	@(#)mmu1.c	2.55.1.3		*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/signal.h"
#include "sys/proc.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/conf.h"
#include "sys/map.h"
#include "sys/errno.h"
#include "sys/mmu.h"
#include "sys/seg.h"
#include "sys/reg.h"
#include "sys/buf.h"


#define	ERR	(-1)


extern caddr_t mmumap();

#ifdef MEXOR
	int	EXORmmu;
#endif

#ifdef M68451
	int	m68451;
#endif

/*
mmuinit	() -	Initializes the internal MMU
		table. Called before "jsr _main".
		Changes memory sizes to reflect
		mmu clicks. Sets up entry for
		proc[0]. Maps physical/virtual
		for proc[0].
*/

mmuinit	()
{
	extern long	locore;		/* location of proc[0]'s u area */
	extern long	physmem;	/* Top of memory */
	extern long	maxmem;		/* available user memory */
	register int	i;

	/* initialize and size memory */
	raminit();
	/* set maxmem to physical memory less:
	 *	area for Kernel (btoc(locore))
	 *	and for three U-vectors.
	 *		The U-vectors are for proc's 0, 1 and 2 (sec proc 0)
	 *		Proc 1's U-vector will be freed later but
	 *		Proc 2's is never freed.
	 */
	maxmem = physmem - btoct(locore) - 3*USIZE;
	malloc (coremap, USIZE);	/* allocate for proc[0]	*/

	curproc = &proc[0];
	proc[0].p_size = USIZE;
	proc[0].p_ssize = USIZE;
	proc[0].p_mmuent = (caddr_t) &mmu_table[0];
	proc[0].p_addr = (caddr_t) locore;

	u.u_ssize = USIZE;

	mmu_table[0].mmu_proc = (struct proc *)&proc[0]; /* initialize table */
	mmu_table[0].mmu_entry[SEG_STACK].mmu_paddr = (caddr_t)locore;
	mmu_table[0].mmu_entry[SEG_STACK].mmu_vaddr = (caddr_t)USRSTACK;
	mmu_table[0].mmu_entry[SEG_STACK].mmu_size = ctob(USIZE);

#ifdef M68451

	/* Set up MMU Address Space Numbers	*/
	addrspace[SYSTEM_PROG] = 0;
	addrspace[SYSTEM_DATA] = 0;

	/* This part will be done in cswitch.s when #s are different */
	addrspace[USER_PROG] = 1;
	addrspace[USER_DATA] = 1;

	/* Clear all descriptors */
	mmuregs(0);	/* For now */
#endif
}


/*
copyin	() -	Called from various places in the
		code to transfer bytes from the
		user segment to the system.
*/


copyin	(useradd, sysadd, count )
char	*useradd;		/* address in user segment */
char	*sysadd;		/* address in system space */
register count;			/* number of bytes to copy */
{
	register caddr_t start;

	if ((start = mmumap( useradd, count, B_READ )) == NULL ) {
		u.u_error = EFAULT;
		return (ERR);
	}
	return (bcopy (start, sysadd, count));
}




/*
copyout () -	Called from various places to
		transfer bytes from the system
		space to a user segment.
*/



copyout (sysadd, useradd, count)
char	*sysadd;		/* address in system space */
char	*useradd;		/* address in user segment */
register count;			/* number of bytes to copy */
{
	register caddr_t start;

	if(( start = mmumap( useradd, count, B_WRITE )) == NULL ) {
		u.u_error = EFAULT;
		return (ERR);
	}

	return (bcopy (sysadd, start, count));
}


/*
mmufork	() -	Called from newproc() to set up a
		new process. Copies the parent
		segments to the child and sets up
		virtual/physical mapping by making
		entry in internal MMU table.

		NOTE: At this point, the u area
		belongs to the parent, but
		u.u_procp points to the child's
		proc table entry.
*/


mmufork	(pptr)
struct proc *pptr;		/* parent process pointer */
{
	register ns, nd;
	register ms, md;
	struct mmu_table *mtp, *mmtp;
	register struct mmu_entry *mp, *mpp;

	/* assumes that whenever called at least one entry is free */
	/* search for free entry */
	for ( mtp = mmu_table; mtp->mmu_proc != NULL; mtp++);

	mmtp = ptom(pptr);

	/* copy	entries	*/
	*mtp = *mmtp;

	/* initialize pointers */
	mtp->mmu_proc = (struct proc *)u.u_procp;/* set proc table pointer */

	u.u_procp->p_mmuent = (caddr_t) mtp;	/* set mmu table pointer */
	mp = &mtp->mmu_entry[0];		/* get child's entry */
	mpp = &mmtp->mmu_entry[0];		/* get parent's entry */

	/*
		Allocate memory
		If memory cannot be found, we call mmucreate
		which will swap out the child process.
		As we have set all the segment physical pointers
		to the parent process, the swap will contain a
		duplicate copy. Thus when we swap back in,
		the child will look like the parent.
	*/

	if ((nd = mp[SEG_DATA].mmu_size))	/* there is data */
		if ((md = malloc (coremap, btoc(nd))) == NULL) {
			mmucreate( mmtp, pptr );
			return;
		}

	if ((ns = mp[SEG_STACK].mmu_size))	/* there is stack */
		if ((ms = malloc (coremap, btoc(ns))) == NULL) {
			if (nd)	/* clean up */
				mfree (coremap, btoc(nd), md );
			mmucreate( mmtp, pptr );
			return;
		}

	/* Copy the data segment */

	if (nd)
		bcopy (mpp[SEG_DATA].mmu_paddr,
			mp[SEG_DATA].mmu_paddr = (caddr_t) ctob(md), nd);

	/* Copy the stack segment - including u-area */

	mp[SEG_STACK].mmu_paddr = (caddr_t) ctob(ms);

	u.u_procp->p_addr = mp[SEG_STACK].mmu_paddr +
			    mp[SEG_STACK].mmu_size - ctob(USIZE);

	if (ns) {
		ns -= ctob(USIZE);
		if (ns)
			/*	user stack from user area	*/
			bcopy (mpp[SEG_STACK].mmu_paddr,
			       mp[SEG_STACK].mmu_paddr, ns);

		/*	u-area from active process	*/
		bcopy (&u, u.u_procp->p_addr, ctob(USIZE));
		}

}

/*
mmucreate () - Called to swap out child process from the
		parent's image.
*/

mmucreate ( mmtp, pptr )

struct	mmu_table *mmtp;			/* parent's mmutable pointer */

{	register struct	proc	*cptr	= u.u_procp;

	/* copy system u-area to process u-area */
	bcopy (&u, cptr->p_addr, ctob(USIZE));

	cptr->p_stat = SIDL;
	xswap(cptr, 0, 0, 0);

	/* set mmureg back to parent */
	mmuregs(pptr);

	cptr->p_stat = SRUN;
}


/*
 *	mmuexec: perform exec processing
 */

mmuexec( ip, ts, ds, ss )
struct	inode	*ip;
int	ts;
register int	ds;
register int	ss;

{	mmuexpand( SEG_DATA, - u.u_dsize );
	u.u_dsize = 0;

	mmuexpand( SEG_STACK, -(u.u_ssize-USIZE));
	u.u_ssize = USIZE;

	/* Text segment allocation will occur within xalloc() */
	u.u_tsize = ts;

	mmuexpand( SEG_STACK, ss-USIZE );
	u.u_ssize = ss;

	mmuexpand( SEG_DATA, ds );
	u.u_dsize = ds;

	xalloc(ip);
	mmuregs(u.u_procp);
}

/*
mmuexit () -	Called to remove entry from the
		internal MMU table.
*/


mmuexit (pptr)
register struct proc *pptr;
{
	register struct mmu_table *mtp;

	mtp = ptom(pptr);

	mfree( coremap, btoc (mtp->mmu_entry[SEG_DATA].mmu_size),
			btoc (mtp->mmu_entry[SEG_DATA].mmu_paddr));
	mfree( coremap, btoc (mtp->mmu_entry[SEG_STACK].mmu_size),
			btoc (mtp->mmu_entry[SEG_STACK].mmu_paddr));
	pptr->p_mmuent = NULL;		/* remove mmu_table entry */
	mtp->mmu_proc = NULL;		/* zap the identifier */
}


/*
 *	mmualloc: allocate system physical memory
 */

mmualloc( msize )
int	msize;

{	return( malloc(coremap, msize));
}

/*
 *	useracc: check access to user address space
 */

useracc (vaddr, msize, flag)
caddr_t	vaddr;
int	msize;
int	flag;
{	return( (int)mmumap(vaddr, msize, flag) != 0 );
}


/*
 *	mmuprot: changes protection of segment
 *			replaces "chgprot"
 */

mmuprot( seg, mode )
int	seg;
int	mode;

{	register struct mmu_entry *msp;

	msp = &ptom(u.u_procp)->mmu_entry[0];
	if (mode == RO)
		msp[seg].mmu_status |= SEG_PROTECT;
	else
		msp[seg].mmu_status &= ~SEG_PROTECT;

	mmuregs(u.u_procp);
}

/*
 *	mmuio: perform segmented I/O
 */

mmuio( strat, bp )
int	(*strat)();
register struct buf *bp;

{	/*
		The work of breaking up a segment is already
		done for us in physio(). Hence, all we are
		required to do is convert the virtual address
		in bp->b_un_addr to a physical one and call
		the strategy routine passed in m_paddr.
	*/

	if(( bp->b_un.b_addr = mmumap (bp->b_un.b_addr,0,B_READ)) == NULL )
		return(ERR);

	(*strat)(bp);
	return(0);
}

/*
 *	mmucswitch: set a context switch
 */

mmucswitch( currp, newp )
struct	proc	*currp;
struct	proc	*newp;

{	/*
		we call mmuregs() to set up the registers
		for the new process. We must also copy the
		u area to the process' space to allow a new
		process' u area to be loaded.
	*/

	/* copy u area and stack for "from" process */
	bcopy(&u, currp->p_addr, ctob(USIZE));

	/* set up registers for "to" process */
	mmuregs (newp);
}

#ifdef m68k			/*  Otherwise see ..os/machdep.c  */
chksize(text, data, stack)
{
	register n;

	n = text + data + stack;
	if (n > MAXMEM || n > maxmem) {
		u.u_error = ENOMEM;
		return(-1);
	}
	return(0);
}
#endif
