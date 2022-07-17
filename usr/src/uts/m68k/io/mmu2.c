/*	@(#)mmu2.c	2.78.1.4		*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/signal.h"
#include "sys/dir.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "sys/proc.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/text.h"
#include "sys/conf.h"
#include "sys/mmu.h"
#include "sys/seg.h"
#include "sys/var.h"
#include "sys/buf.h"
#include "sys/map.h"


static	struct	mmu_entry *mapseg = NULL;
static	long	mapsegsize;

extern maxmem;

extern struct shmid_ds	shmem[];	/* Shared Memory Headers */
extern struct shmid_ds	*shm_shmem[];	/* Attached Shared Memory Pointers */
extern struct shmpt_ds	shm_pte[];	/* Attach Points and Flags */
extern struct shminfo	shminfo;	/* Shared Memory Info. */

/*
	mmudetach: detach from a logical segment
*/


mmudetach (pptr, seg, oldsize, flag)
register struct proc *pptr;	/* proc table pointer */
int	seg;			/* segment to swap out */
int	oldsize;		/* for core expansions */
register int	flag;		/* free memory ? ( <0 => partial ) */
{
	register struct mmu_entry *msp;	/* segment entry */
	register struct text *xp;	/* text table pointer */
	register coreaddr;	/* memory address (clicks) */
	register swaddr;	/* disk address on swap */
	register swsize;	/* size of swap */

	msp = &ptom(pptr)->mmu_entry[seg];	/* get entry */

	if( msp->mmu_status & SEG_SWAPPED ) {
		printf("mmudetach: duplicate swap\n");
		return( 0 );
	}

	switch (seg) {

	case SEG_TEXT:

		xp = pptr->p_textp;
		swsize = xp->x_size;	/* newsize */
		coreaddr = btoc (msp->mmu_paddr);

		msp->mmu_status &= ~SEG_VALID;

		/*
		 *	Assuming:
		 *	1. flag == 1 on all mmudetach( ... SEG_TEXT ... );
		 *	2. if xp->x_ccount == 0, then xp->x_caddr == pptr
		 */

		if( flag = (xp->x_ccount == 0)) /* assuming flag == 1 */
			xp->x_caddr = 0;

		if ((xp->x_flag&XWRIT) == 0) {	/* don't swap out */
			if (flag)		/* free memory */
				mfree (coremap, swsize, coreaddr);
			return(0);		/* nothing else to do */
		}

		xp->x_flag &= ~XWRIT;
		if ((swaddr = xp->x_daddr) == 0 )
			xp->x_daddr = swaddr = swapalloc (ctod (swsize), 1);

		/*
		 *	At this point:
		 *	1. We want to swap out the text segment.
		 *	2. We will free memory if flag is true.
		 */
		break;

	case SEG_DATA:

		swsize = pptr->p_size - pptr->p_ssize;	/* size to allocate */
		swaddr = swapalloc (ctod (swsize), 1);
		coreaddr = btoc (msp->mmu_paddr);
		msp->mmu_paddr = (caddr_t) swaddr;
		break;

	case SEG_STACK:

		coreaddr = btoc(msp->mmu_paddr);
		swsize = pptr->p_ssize;
		msp->mmu_paddr = (caddr_t) swapalloc(ctod(swsize),1);
		swaddr = (int)msp->mmu_paddr+ctod(swsize-oldsize);
		break;

	case SEG_SHM:
	default:	/* Shared memory segment */
		msp->mmu_status = 0;	/* disable segment */
		mmuregs(pptr);
		return(0);
	}

	msp->mmu_status |= SEG_SWAPPED;
	swap (pptr, swaddr, coreaddr, oldsize, B_WRITE);	/* out */

	if (flag != 0)
		mfree (coremap, oldsize, coreaddr);	/* free memory */

	return (0);
}


/*
	mmuattach: attach to a logical segment
*/


mmuattach (pptr, seg, flag, mode)
struct proc *pptr;
int	seg;			/* segment identifier */
int	flag;			/* free swap	? */
int	mode;			/* allocate memory ? */
{
	struct text *xp;		/* text table entry */
	register struct mmu_entry *msp;	/* MMU segment entry */
	register struct mmu_table *mtp;	/* MMU table entry */
	register coreaddr;
	register swaddr;
	register swsize;
	register i;
	struct mmu_entry *mspp;		/* control proc entry */

	mtp = ptom(pptr);
	msp = &mtp->mmu_entry[seg];

	if( ! msp->mmu_status&SEG_SWAPPED ) {
		printf("mmuattach: duplicate swap\n" );
		return(0);
	}

	switch (seg) {

	case SEG_TEXT:
		xp = pptr->p_textp;

		if (xp->x_caddr) {
			mspp = &ptom(xp->x_caddr)->mmu_entry[SEG_TEXT];

			if (msp != mspp) {	/* copy entry if different */
				msp->mmu_paddr = mspp->mmu_paddr;
				msp->mmu_vaddr = mspp->mmu_vaddr;
#ifdef MEXOR
				msp->mmu_end = mspp->mmu_end;
#endif
				msp->mmu_size = mspp->mmu_size;
				msp->mmu_status = mspp->mmu_status;
			}
		}
		else
			xp->x_caddr = pptr;

		swaddr = xp->x_daddr;
		swsize = xp->x_size;

		if (xp->x_ccount)	/* incore */
			break;

		/*
			At this point we know that the physical segment is
			out on swap. The disk address is xp->x_daddr.
			if mode == MD_NALLOC, memory has already been
			allocated and resides at mmu_paddr.
		*/

		if (mode == MD_NALLOC)
			coreaddr = btoc (msp->mmu_paddr);
		else
		{
			if ((coreaddr = malloc (coremap, swsize)) == NULL)
				return (-1);
			msp->mmu_paddr = (caddr_t) ctob(coreaddr);
		}

		/*
		 *	if( swaddr == 0 ) then
		 *		This attach is an initial
		 *		swapin of a text segment
		 *		that was too big to allocate
		 *		space for in xexpand (called from
		 *		xalloc() called from getxfile()).
		 *		In this case, this attach
		 *		is being called from swapin().
		 *		The "flag" argument must be
		 *		false in this case.
		 */

		if( swaddr )
			swap (pptr, swaddr, coreaddr, swsize, B_READ);

		/* Set up TEXT segment size */
		msp->mmu_size = ctob (swsize);
#ifdef MEXOR
		msp->mmu_end = ENDREG(msp->mmu_size);
#endif

		if (flag)	/* clear swap address */
			xp->x_daddr = 0;
		break;

	case SEG_DATA:
		swsize = pptr->p_size - pptr->p_ssize;

		if ((coreaddr = malloc (coremap, swsize)) == NULL)
			return (-1);

		swaddr = (int) msp->mmu_paddr;
		msp->mmu_paddr = (caddr_t) ctob (coreaddr);

		swap (pptr, swaddr, coreaddr, swsize, B_READ);

		if (msp->mmu_size < ctob(swsize))
			bzero (msp->mmu_paddr + msp->mmu_size,
				ctob (swsize) - msp->mmu_size);

		msp->mmu_size = ctob (swsize);
#ifdef MEXOR
		msp->mmu_end = ENDREG(msp->mmu_vaddr+msp->mmu_size);
#endif
		break;

	case SEG_STACK:
		swsize = pptr->p_ssize;

		if ((coreaddr = malloc (coremap, swsize)) == NULL)
			return (-1);

		swaddr = (int) msp->mmu_paddr;
		msp->mmu_paddr = (caddr_t) ctob (coreaddr);

		swap (pptr, swaddr, coreaddr, swsize, B_READ);

		if (msp->mmu_size < ctob(swsize))
			bzero (msp->mmu_paddr, ctob(swsize)-msp->mmu_size);
		msp->mmu_size = ctob (swsize);

		pptr->p_addr = msp->mmu_paddr + msp->mmu_size - ctob(USIZE);

		break;

	case SEG_SHM:
	default:	/* Shared memory segments */
		i = (pptr - proc)*shminfo.shmseg+seg-SEG_SHM;

		coreaddr = ctob((*shm_shmem[i]).shm_paddr);
		swaddr = ctob(shm_pte[i].shm_vaddr);
		swsize = (*shm_shmem[i]).shm_segsz;

		if(( swaddr < ( (int)mtp->mmu_entry[SEG_DATA].mmu_vaddr+
				      mtp->mmu_entry[SEG_DATA].mmu_size ))
		||((swaddr+swsize) >= (int)mtp->mmu_entry[SEG_STACK].mmu_vaddr))
			return(-1);

		msp->mmu_paddr = (caddr_t)coreaddr;
		msp->mmu_vaddr = (caddr_t)swaddr;
		msp->mmu_size = swsize;
#ifdef MEXOR
		msp->mmu_end = ENDREG( (int)msp->mmu_vaddr+msp->mmu_size );
#endif
		msp->mmu_status |= SEG_VALID;
		if( shm_pte[i].shm_sflg & SHM_RDONLY )
			msp->mmu_status |= SEG_PROTECT;
		mmuregs(pptr);
		return(0);
	}

	if (flag)
		mfree (swapmap, swsize, swaddr);

	msp->mmu_status |= SEG_VALID;
	msp->mmu_status &= ~SEG_SWAPPED;


	return (0);
}

/*
mmuexpand ()	-	Called from sbreak() to expand
			data segment. Called from other
			places for stack and text
			segments.
*/

mmuexpand (seg, incr)
int	seg;
register incr;	/* amount to increment/decrement in clicks */
{
	register struct mmu_entry *msp;
	register struct mmu_table *mtp;
	register struct proc *p = u.u_procp;
	register sg;
	register newaddr;
	register bincr;

	mtp = ptom(p);
	msp = &mtp->mmu_entry[seg];

	if (incr == 0)
		return (0);

	bincr = incr < 0? -ctob(-incr): ctob(incr);

	switch (seg) {

	case SEG_TEXT:	/* allocate and connect */

		if ((newaddr = ctob(malloc (coremap, incr))) == NULL)
			return (-1);

		msp->mmu_paddr = (caddr_t) newaddr;
		msp->mmu_size = bincr;
		msp->mmu_vaddr = (caddr_t) 0;
		msp->mmu_status = SEG_VALID | SEG_PROTECT;
#ifdef MEXOR
		msp->mmu_end = ENDREG(msp->mmu_vaddr+msp->mmu_size);
#endif

		return (0);

	case SEG_DATA:

		if( chksize( u.u_tsize, u.u_dsize + incr, u.u_ssize ) < 0 ) 
			return (-1);

		/*	Overlap shared memory segment???	*/
		if( p->p_smbeg )
		 	if(((int)msp->mmu_vaddr+msp->mmu_size+bincr)
				> ctob(p->p_smbeg))
			{	u.u_error = ENOMEM;
				return (-1);
			}

		msp->mmu_vaddr = (caddr_t) ctob(u.u_tsize);

		p->p_size += incr;

		if (incr < 0)	/* shrink */ {
			msp->mmu_size += bincr;
#ifdef MEXOR
			msp->mmu_end = ENDREG(msp->mmu_vaddr+msp->mmu_size);
#endif
			if( msp->mmu_size == 0 )
				msp->mmu_status &= ~SEG_VALID;
			mfree (coremap, -incr,
				btoc (msp->mmu_paddr + msp->mmu_size));
			return (0);
		}

		if((newaddr =
		    ctob(malloc(coremap, p->p_size-p->p_ssize))) == NULL) {
			xque( p, SXBRK, incr );
			return(0);
		}
		/* memory exists - so copy */
		bcopy (msp->mmu_paddr, newaddr, msp->mmu_size);
		mfree (coremap, btoc (msp->mmu_size), btoc (msp->mmu_paddr));
		bzero (newaddr + msp->mmu_size, bincr);
		msp->mmu_paddr = (caddr_t) newaddr;
		msp->mmu_size += bincr;
#ifdef MEXOR
		msp->mmu_end = ENDREG(msp->mmu_vaddr+msp->mmu_size);
#endif
		msp->mmu_status |= SEG_VALID;
		return (0);

	case SEG_STACK:	/* expand stack segment */

		if( chksize( u.u_tsize, u.u_dsize, u.u_ssize + incr ) < 0 ) 
			return (-1);

		/*	Overlap shared memory segment???	*/
		if( p->p_smbeg )
		 	if((int)msp->mmu_vaddr-bincr < ctob(p->p_smend))
			{	u.u_error = ENOMEM;
				return (-1);
			}

		p->p_size += incr;
		p->p_ssize += incr;

		if (incr < 0) {
			mfree (coremap, -incr, btoc (msp->mmu_paddr));
			msp->mmu_paddr -= bincr;
			msp->mmu_vaddr -= bincr;
			msp->mmu_size += bincr;
#ifdef MEXOR
			msp->mmu_end = 
			      ENDREG(msp->mmu_vaddr+msp->mmu_size-ctob(USIZE));
#endif
			if( msp->mmu_size <= ctob(USIZE) )
				msp->mmu_status &= ~SEG_VALID;
			return (0);
		}

		if ((newaddr = ctob(malloc(coremap, p->p_ssize))) == NULL) {
			msp->mmu_vaddr -= bincr;
			xque( p, SXSTK, incr );
			return(0);
		}
		bcopy (msp->mmu_paddr, newaddr+bincr, msp->mmu_size);
		bzero (newaddr, bincr);
		mfree (coremap, btoc( msp->mmu_size ), btoc( msp->mmu_paddr ) );
		msp->mmu_paddr = (caddr_t) newaddr;
		msp->mmu_vaddr -= bincr;
		msp->mmu_size += bincr;
		p->p_addr = msp->mmu_paddr + msp->mmu_size - ctob(USIZE);
#ifdef MEXOR
		msp->mmu_end = ENDREG(msp->mmu_vaddr+msp->mmu_size-ctob(USIZE));
#endif
		msp->mmu_status |= SEG_VALID;
		return (0);

	default:
		panic ("M_EXPAND: invalid segment");
	}

	return (-1);
}


/*
mmuregs ()	-	sets up the registers for the EXORmmu.
*/

#ifdef MEXOR
mmuregs (pptr)
struct proc *pptr;
{	register struct mmu_entry *msp;
	register seg;
	register struct mmu_regs *mrp;
	register unsigned vaddr;

	msp = &ptom(pptr)->mmu_entry[0];

	mapseg = NULL;	/* invalidate map cache */

	for(mrp = MMUREGS, seg = MMUSEGS+1; --seg != SEG_TEXT; msp++, mrp++)
		if (mrp->mr_ctrl = msp->mmu_status & (SEG_VALID | SEG_PROTECT)){
			vaddr = (unsigned)msp->mmu_vaddr;
			mrp->mr_offset = LDREG((unsigned)msp->mmu_paddr-vaddr);
			mrp->mr_begin  = LDREG(vaddr);
			mrp->mr_end    = msp->mmu_end;
		}
}
#endif
#ifdef M68451
/*	We know at this point that something in the core-representation
	of mmu mapping has changed.  In the first stage of implementing
	a 68451-based system we will simply kill ALL of the mmu 
	descriptors allowing them to be reestablished by mmuintr.

*/
mmuregs (pptr)
struct proc *pptr;
{
	register	i;
	register struct mmu_regs *mrp = MMUREGS;
	 
	mapseg = NULL;	/* invalidate map cache */

	/* Clear all descriptors except 0 */
	for( i = 1; i < MMUTOTSEGS; i++ )
	{
		mrp->mr_ptr = i;
		*ssrclr = 0;	/* Kills Descriptor */
	}

}
#endif

/*
mmumap () -	given a virtual address, a count, and
		a write flag, returns the physical
		address if the block is inside of a
		single segment and NULL otherwise.
*/

caddr_t mmumap (vaddr, count, rdwr)
register long	vaddr;		/* user virtual address */
register count;			/* byte count */
int	rdwr;			/* read/write flag */

{
	register struct mmu_entry *mp;
	register long	off;
	register int	seg;

	if(( mp = mapseg ) != NULL )
	{	if(((off = vaddr - (long)mp->mmu_vaddr) >= 0)
		&& ((off+count) <= mapsegsize))
		{	if ((!rdwr) && (mp->mmu_status&SEG_PROTECT))
				return (NULL);

			return((caddr_t)((long)mp->mmu_paddr+off));
		}
	}

	mp = &ptom(u.u_procp)->mmu_entry[0];

	if(((off = vaddr - (long)mp[SEG_TEXT].mmu_vaddr) >= 0)
	&& ((off+count) <= (long)mp[SEG_TEXT].mmu_size))
		seg = SEG_TEXT;

   else if(((off = vaddr - (long)mp[SEG_DATA].mmu_vaddr) >= 0)
	&& ((off+count) <= (long)mp[SEG_DATA].mmu_size))
		seg = SEG_DATA;

   else if(((off = vaddr - (long)mp[SEG_STACK].mmu_vaddr) >= 0)
	&& ((off+count) <= ((long)mp[SEG_STACK].mmu_size-ctob(USIZE))))
		seg = SEG_STACK;

   else if(((off = vaddr - (long)mp[SEG_SHM].mmu_vaddr) >= 0)
	&& ((off+count) <= (long)mp[SEG_SHM].mmu_size))
		seg = SEG_SHM;
	else
		return (NULL);

	mp = &mp[seg];
	mapseg = mp;
	mapsegsize = mp->mmu_size;
	if( seg == SEG_STACK )
		mapsegsize -= ctob(USIZE);

	if ((!rdwr) && (mp->mmu_status&SEG_PROTECT))
		return (NULL);

	return ((caddr_t)((long)mp->mmu_paddr + off));
}

#ifdef M68451

/*
mmuintr ()	-	Service an mmu interrupt for the 68451
*/

mmuintr(r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,ps,pc,dummy,vaddr)
register long	vaddr;
{	
	register struct mmu_entry *mp;
	register struct mmu_regs *mrp = MMUREGS;
	register long	off;
	register int	seg;
	static int descno = 0;

	if( vaddr < 0 )
		return( -1 );

	mp = &ptom(u.u_procp)->mmu_entry[0];

	if((off = vaddr) < (long)mp[SEG_TEXT].mmu_size)
		seg = SEG_TEXT;

	else if((off -= (long)mp[SEG_DATA].mmu_vaddr) >= 0 &&
			(off < (long)mp[SEG_DATA].mmu_size))
		seg = SEG_DATA;

	else if((off = vaddr - (long)mp[SEG_STACK].mmu_vaddr) >= 0 &&
			(off < (long)mp[SEG_STACK].mmu_size))
		seg = SEG_STACK;

	else if((off = vaddr - (long)mp[SEG_SHM].mmu_vaddr) >= 0 &&
			(off < (long)mp[SEG_SHM].mmu_size))
		seg = SEG_SHM;

	else return( -1 );	/* Error is NOT a page fault */

	/* Set up the segment descriptor.  Note that the 451 is 
	   unconcerned with the lower 8-bits of addr. information */
	
	mrp->mr_lba = ctob(btoct(vaddr)) >> 8;
	mrp->mr_lam = ~((NBPC-1) >> 8);   /* Range is one click. */
	mrp->mr_pba = (ctob(btoct(off)) + (unsigned)mp[seg].mmu_paddr) >> 8;
	mrp->mr_asn = 1;	/* All procs the same for now */
	mrp->mr_ssr = (mp[seg].mmu_status & (SEG_VALID | SEG_PROTECT));
	mrp->mr_asm = 0xff;	/* Match Exact */
	if( ++descno >= MMUTOTSEGS ) /* Allocation is cyclic except for 0 */
		descno = 1;
	mrp->mr_ptr = descno;
	if(*descwrite == 0) 
		return( 0 );
	return( -1 );
}
#endif
