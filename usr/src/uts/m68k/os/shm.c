/* @(#)shm.c	2.1.1.5	 */
#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#ifdef m68k
#include "sys/map.h"
#endif
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/seg.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "sys/proc.h"
#include "sys/page.h"
#include "sys/mtpr.h"
#include "sys/systm.h"

extern struct shmid_ds	shmem[];	/* shared memory headers */
extern struct shmid_ds	*shm_shmem[];	/* ptrs to attached segments */
extern struct shmpt_ds	shm_pte[];	/* segment attach points */
extern struct shminfo	shminfo;	/* shared memory info structure */
int	shmtot;		/* total shared memory currently used */

extern time_t		time;			/* system idea of date */

struct	shmid_ds	*ipcget(),
			*shmconv();

/*
**	shmat - Shmat system call.
*/

shmat()
{
	register struct a {
		int	shmid;
		char	*addr;
		int	flag;
	}	*uap = (struct a *)u.u_ap;
	register struct shmid_ds	*sp;	/* shared memory header ptr */
	register int	shmn;
#ifdef m68k
	register struct shmpt_ds	*sg;	/* shared memory-process ptr */
	register struct shmpt_ds	*se;	/* shared memory-process ptr */
	register struct shmid_ds	**sd;	/* shm_shmem pointer */
	register int			size;	/* size of shared memory seg */
#else
	register int	*seg, *shm;
#endif
	register int	segbeg, segend;
	int		lr; 
#ifndef m68k
	int	pflag;
	struct	pt_entry *ptaddr;
	int		i;
#else
	register int	i;
#endif

	if((sp = shmconv(uap->shmid, SHM_DEST)) == NULL)
		return;
	if(ipcaccess(&sp->shm_perm, SHM_R))
		return;
	if((uap->flag & SHM_RDONLY) == 0)
		if(ipcaccess(&sp->shm_perm, SHM_W))
			return;
	for(shmn = 0;shmn < shminfo.shmseg;shmn++)
		if(shm_shmem[(u.u_procp - proc)*shminfo.shmseg+shmn] == NULL)
			break;
	if(shmn >= shminfo.shmseg) {
		u.u_error = EMFILE;
		return;
	}
	if(uap->flag & SHM_RND)
		uap->addr = (char *)((uint)uap->addr & ~(SHMLBA - 1));

#ifndef m68k
/*
 * Check for page alignment and containment within P0
 */
	if ((int)uap->addr & (ctob(1) - 1) ||
		(int)uap->addr & 0xc0000000 ||
		((int)uap->addr + sp->shm_segsz) & 0xc0000000) {
#else
/*
 * Check for click alignment, minimum size allignment and valid addresses
 */
	if ((int)uap->addr & (ctob(1) - 1)
#if M68000 || M68010
		|| (int)uap->addr & 0xff000000
		|| ((int)uap->addr + sp->shm_segsz) & 0xff000000
#endif
	){
#endif
		u.u_error = EINVAL;
		return;
	}

#ifndef	m68k
/*
 * An address of 0 places the shared memory into a first fit location
 */
	if (uap->addr == NULL) {
		seg = ((int *)mfpr(P0BR)) + (u.u_tsize+u.u_dsize) + shminfo.shmbrk;
		segend = (int)(((int *)mfpr(P0BR)) + mfpr(P0LR));
		segbeg = NULL;

		for (; seg < (int *) segend; seg++) {
			if (*seg & PG_V) {
				if (segbeg && ctob(seg - (int *)segbeg) >= sp->shm_segsz)
					break;
				else
					segbeg = NULL;
			} else
                                if (segbeg == NULL)
                                        segbeg = (int)seg;

		}
		if (segbeg)
			segbeg = (int *)segbeg - (int *)mfpr(P0BR);
		else {
			segbeg = mfpr(P0LR);
			if (u.u_procp->p_smbeg == 0)
				segbeg += shminfo.shmbrk;
		}
	} else {
/*
 * Check to make sure segment does not overlay any valid segments
 */
		segbeg = btoc(uap->addr);
		if (segbeg < (segend = mfpr(P0LR))) {
			if ((segbeg + btoc(sp->shm_segsz)) < segend)
				segend = segbeg + btoc(sp->shm_segsz);
			seg = ((int *)mfpr(P0BR)) + segbeg;
			for (i = 0; i < segend - segbeg; i++) {
				if (*seg++ & PG_V) {
					u.u_error = ENOMEM;
					return;
				}
			}
		}
	}
/*
 * Need to increase the size of the page table, allocate and clear.
 */
	segend = segbeg + btoc(sp->shm_segsz);
	if (mfpr(P0LR) < segend) {
		lr = (int *)mfpr(P0BR) - (int *)mfpr(P1BR);
		lr += segend - mfpr(P1LR);

		if (lr > 0) {
			if (ptexpand((lr+127)/128) == -1) {
				u.u_error = ENOMEM;
				return;
			}
		}
		seg = ((int *)mfpr(P0BR)) + mfpr(P0LR);
		for (i = mfpr(P0LR); i < segend; i++)
			*seg++ = 0;
	}
	mtpr(P0LR, segend);
	u.u_pcb.pcb_p0lr = segend | 0x04000000;
/*
 * Copy the shared memory segment in
 */
	shm = (int *)sp->shm_ptbl;
	seg = ((int *)mfpr(P0BR)) + segbeg;
	pflag = ((uap->flag & SHM_RDONLY) ? PG_URKW : PG_UW);
	for (i=0; i < btoc(sp->shm_segsz); i++) {
		*seg++ = *shm++ | PG_V | pflag;
	}
/*
 * Clear segment on first attach
 */
	if (sp->shm_perm.mode & SHM_CLEAR) {
		ptaddr = ((struct pt_entry *)mfpr(P0BR)) + segbeg;
		i = segend - segbeg;
		while (--i >= 0) {
			clearseg(ptaddr->pg_pfnum);
			ptaddr++;
		}
		sp->shm_perm.mode &= ~SHM_CLEAR;
	}
	shm_shmem[(i = u.u_procp - proc)*shminfo.shmseg+shmn] = sp;
	shm_pte[i*shminfo.shmseg+shmn].shm_spte = segbeg;
	shm_pte[i*shminfo.shmseg+shmn].shm_sflg = pflag;
#else
/*
 * An address of 0 places the shared memory into a first fit location
 */
	i = u.u_procp-proc;
	if (uap->addr == NULL) {
		segbeg = u.u_tsize+u.u_dsize+shminfo.shmbrk;
		se = &shm_pte[(i+1)*shminfo.shmseg];
		for(;;) {
			sg = &shm_pte[i*shminfo.shmseg];
			sd = &shm_shmem[i*shminfo.shmseg];
			for( segend = btoc(USRSTACK); sg<se; sg++, sd++ ) {
				if( *sd == NULL )
					continue;

				if(( segbeg <= sg->shm_vaddr )&&
				   ( sg->shm_vaddr < segend )) {
					segend = sg->shm_vaddr;
					size = (*sd)->shm_segsz;
				}
			}

			if( segbeg+btoc(sp->shm_segsz) <= segend )
			{	segend = segbeg+btoc(sp->shm_segsz);
				break;
			}

			if( segend == btoc(USRSTACK) ) {
				u.u_error = ENOMEM;
				return;
			}

			segbeg = segend+btoc(size);
		}

	} else {
/*
 * Check to make sure segment does not overlay any valid shm segments
 */
		se = &shm_pte[(i+1)*shminfo.shmseg];
		sg = &shm_pte[i*shminfo.shmseg];
		sd = &shm_shmem[i*shminfo.shmseg];
		segbeg = btoc(uap->addr);
		segend = segbeg+btoc(sp->shm_segsz);
		for( ; sg < se; sg++, sd++ ) {
			if( (*sd) == NULL )
				continue;

			if((sg->shm_vaddr < segend) &&
			   (segbeg < sg->shm_vaddr+btoc((*sd)->shm_segsz))) {
				u.u_error = ENOMEM;
				return;
			}
		}
	}

/*
 * Clear segment on first attach
 */
	if (sp->shm_perm.mode & SHM_CLEAR) {
		bzero( ctob(sp->shm_paddr), sp->shm_segsz );
		sp->shm_perm.mode &= ~SHM_CLEAR;
	}
	shm_shmem[i*shminfo.shmseg+shmn] = sp;
	shm_pte[i*shminfo.shmseg+shmn].shm_vaddr = segbeg;
	shm_pte[i*shminfo.shmseg+shmn].shm_sflg = (uap->flag & SHM_RDONLY);
	if( mmuattach( u.u_procp, shmn+SEG_SHM, 0, 0 ) < 0 ) {
		/* overlap with SEG_TEXT, SEG_DATA or SEG_STACK detected */
		shm_shmem[i*shminfo.shmseg+shmn] = NULL;
		shm_pte[i*shminfo.shmseg+shmn].shm_vaddr = 0;
		shm_pte[i*shminfo.shmseg+shmn].shm_sflg = 0;
		u.u_error = ENOMEM;
		return;
	}


#endif
	if (u.u_procp->p_smbeg == 0 || u.u_procp->p_smbeg > segbeg)
		u.u_procp->p_smbeg = segbeg;
	if (u.u_procp->p_smend < segend)
		u.u_procp->p_smend = segend;
	sp->shm_nattch++;
	sp->shm_cnattch++;
	u.u_rval1 = ctob(segbeg);
	sp->shm_atime = time;
	sp->shm_lpid = u.u_procp->p_pid;
}

/*
**	shmconv - Convert user supplied shmid into a ptr to the associated
**		shared memory header.
*/

struct shmid_ds *
shmconv(s, flg)
register int	s;	/* shmid */
int		flg;	/* error if matching bits are set in mode */
{
	register struct shmid_ds	*sp;	/* ptr to associated header */

	sp = &shmem[s % shminfo.shmmni];
	if(!(sp->shm_perm.mode & IPC_ALLOC) || sp->shm_perm.mode & flg
#ifndef vax
		|| s < 0
#endif
		|| s / shminfo.shmmni != sp->shm_perm.seq) {
		u.u_error = EINVAL;
		return(NULL);
	}
	return(sp);
}

/*
**	shmctl - Shmctl system call.
*/

shmctl()
{
	register struct a {
		int		shmid,
				cmd;
		struct shmid_ds	*arg;
	}	*uap = (struct a *)u.u_ap;
	register struct shmid_ds	*sp;	/* shared memory header ptr */
	struct shmid_ds			ds;	/* hold area for IPC_SET */

	if((sp = shmconv(uap->shmid, (uap->cmd == IPC_STAT) ? 0 : SHM_DEST)) ==
		NULL)
		return;
	u.u_rval1 = 0;
	switch(uap->cmd) {

	/* Remove shared memory identifier. */
	case IPC_RMID:
		if(u.u_uid != sp->shm_perm.uid && u.u_uid != sp->shm_perm.cuid
			&& !suser())
			return;
		sp->shm_ctime = time;
		sp->shm_perm.mode |= SHM_DEST;

		/* Change key to private so old key can be reused without
			waiting for last detach.  Only allowed accesses to
			this segment now are shmdt() and shmctl(IPC_STAT).
			All others will give bad shmid. */
		sp->shm_perm.key = IPC_PRIVATE;

		/* Adjust counts to counter shmfree decrements. */
		sp->shm_nattch++;
		sp->shm_cnattch++;
		shmfree(sp);
		return;

	/* Set ownership and permissions. */
	case IPC_SET:
		if(u.u_uid != sp->shm_perm.uid && u.u_uid != sp->shm_perm.cuid
			&& !suser())
			return;
		if(copyin(uap->arg, &ds, sizeof(ds))) {
			u.u_error = EFAULT;
			return;
		}
		sp->shm_perm.uid = ds.shm_perm.uid;
		sp->shm_perm.gid = ds.shm_perm.gid;
		sp->shm_perm.mode = (ds.shm_perm.mode & 0777) |
			(sp->shm_perm.mode & ~0777);
		sp->shm_ctime = time;
		return;

	/* Get shared memory data structure. */
	case IPC_STAT:
		if(ipcaccess(&sp->shm_perm, SHM_R))
			return;
		if(copyout(sp, uap->arg, sizeof(*sp)))
			u.u_error = EFAULT;
		return;

	default:
		u.u_error = EINVAL;
		return;
	}
}

/*
**	shmdt - Shmdt system call.
*/

shmdt()
{
	struct a {
		char	*addr;
	}	*uap = (struct a *)u.u_ap;
	register struct shmid_ds	*sp, **spp;
	register int			*seg;
	int	segbeg;
	register struct shmpt_ds	*pt;
	register i, j;
	register struct proc *p;

/*
 * Check for page alignment
 */
	if ((int)uap->addr & (ctob(1) - 1) || (segbeg = btoc(uap->addr)) == 0) {
		u.u_error = EINVAL;
		return;
	}
/*
 * find segment
 */
	spp = &shm_shmem[i=((p=u.u_procp)-proc)*shminfo.shmseg];
	pt = &shm_pte[i];
	for (i=0; i < shminfo.shmseg; i++, pt++, spp++) {
#ifdef m68k
		if ((sp = *spp) != NULL && pt->shm_vaddr == segbeg)
#else
		if ((sp = *spp) != NULL && pt->shm_spte == segbeg)
#endif
			break;
		sp = NULL;
	}
	if (sp == NULL) {
		u.u_error = EINVAL;
		return;
	}
#ifndef m68k
	i = btoc(sp->shm_segsz);
	seg = (int *)mfpr(P0BR) + segbeg;
	while (i--)
		*seg++ = 0;
	mtpr(TBIA, 0);
#else
	mmudetach( u.u_procp, i+SEG_SHM, sp->shm_segsz, 0 );
#endif
	shmfree(sp);
	sp->shm_dtime = time;
	sp->shm_lpid = p->p_pid;
	*spp = NULL;
#ifdef m68k
	pt->shm_vaddr = 0;
#else
	pt->shm_spte = 0;
#endif
	p->p_smbeg = 0;
	p->p_smend = 0;
	pt = &shm_pte[(p-proc)*shminfo.shmseg];
	for (j=0; j<shminfo.shmseg; j++, pt++) {
#ifdef m68k
		if (i = pt->shm_vaddr) {
#else
		if (i = pt->shm_spte) {
#endif
			if (p->p_smbeg) {
				if (p->p_smbeg > i)
					p->p_smbeg = i;
				if (p->p_smend < i + btoc(sp->shm_segsz))
					p->p_smend = i + btoc(sp->shm_segsz);
			} else {
				p->p_smbeg = i;
				p->p_smend = i + btoc(sp->shm_segsz);
			}
		}
	}
#ifndef m68k
	if (p->p_smbeg == 0) {
		mtpr(P0LR, u.u_tsize + u.u_dsize);
		u.u_pcb.pcb_p0lr = (u.u_tsize + u.u_dsize) | 0x04000000;
	}
#endif
	u.u_rval1 = 0;
}

/*
**	shmexec - Called by setregs(os/sys1.c) to handle shared memory exec
**		processing.
*/

shmexec()
{
	register struct shmid_ds	**spp;	/* ptr to ptr to header */
	register struct shmpt_ds	*sppp;	/* ptr to pte data */
	register int			i;	/* loop control */
	register int	*seg, *segend;		/* ptr's to pte */

	if (u.u_procp->p_smbeg == 0)
		return;
	/* Detach any attached segments. */
	sppp = &shm_pte[i = (u.u_procp - proc)*shminfo.shmseg];
	u.u_procp->p_smbeg = 0;
	u.u_procp->p_smend = 0;
	spp = &shm_shmem[i];
	for(i = 0; i < shminfo.shmseg; i++,spp++,sppp++) {
		if(*spp == NULL)
			continue;
		shmfree(*spp);
#ifdef m68k
		mmudetach( u.u_procp, i+SEG_SHM, (*spp)->shm_segsz, 0 );
		*spp = NULL;
		sppp->shm_vaddr = 0;
#else
		seg = (int *)mfpr(P0BR) + sppp->shm_spte;
		segend = seg + btoc((*spp)->shm_segsz);
		for (; seg < segend ;)
			*seg++ = 0;
		*spp = NULL;
		sppp->shm_spte = 0;
#endif
	}
#ifndef m68k
	mtpr(P0LR, u.u_tsize + u.u_dsize);
	u.u_pcb.pcb_p0lr = (u.u_tsize + u.u_dsize) | 0x04000000;
#endif
}

/*
**	shmexit - Called by exit(os/sys1.c) to clean up on process exit.
*/

shmexit()
{
	/* Same processing as for exec. */
	shmexec();
}

/*
**	shmfork - Called by newproc(os/slp.c) to handle shared memory fork
**		processing.
*/

shmfork(cp, pp)
struct proc	*cp,	/* ptr to child proc table entry */
		*pp;	/* ptr to parent proc table entry */
{
	register struct shmid_ds	**cpp,	/* ptr to child shmem ptrs */
					**ppp;	/* ptr to parent shmem ptrs */
	register struct shmpt_ds	*cppp,
					*pppp;
	register int			i;	/* loop control */

	if (pp->p_smbeg == 0)
		return;
	/* Copy ptrs and update counts on any attached segments. */
	cpp = &shm_shmem[(cp - proc)*shminfo.shmseg];
	ppp = &shm_shmem[(pp - proc)*shminfo.shmseg];
	cppp = &shm_pte[(cp - proc)*shminfo.shmseg];
	pppp = &shm_pte[(pp - proc)*shminfo.shmseg];
	cp->p_smbeg = pp->p_smbeg;
	cp->p_smend = pp->p_smend;
	for(i = 0;i < shminfo.shmseg; i++, cpp++, ppp++, cppp++, pppp++) {
		if(*cpp = *ppp) {
			(*cpp)->shm_nattch++;
			(*cpp)->shm_cnattch++;
#ifdef m68k
			cppp->shm_vaddr = pppp->shm_vaddr;
#else
			cppp->shm_spte = pppp->shm_spte;
#endif
			cppp->shm_sflg = pppp->shm_sflg;
		}
	}
}

/*
**	shmfree - Decrement counts.  Free segment and page tables if
**		indicated.
*/

shmfree(sp)
register struct shmid_ds	*sp;	/* shared memory header ptr */
{
	register int size, ptsize;

	if(sp == NULL)
		return;
	sp->shm_nattch--;
	if(--(sp->shm_cnattch) == 0 && sp->shm_perm.mode & SHM_DEST) {
#ifdef m68k
		mfree( coremap, size=btoc(sp->shm_segsz), sp->shm_paddr);
#else
		memfree(sp->shm_ptbl, size=btoc(sp->shm_segsz));
		sptfree(sp->shm_ptbl, ptsize=btoc(sizeof(struct pt_entry)*size), 1);
#endif
		/* adjust maxmem for amount freed */
#ifdef m68k
		maxmem += size;
#else
		maxmem += (ptsize + size);
#endif
		shmtot -= size;

		sp->shm_perm.mode = 0;
		if(((int)(++(sp->shm_perm.seq)*shminfo.shmmni + (sp - shmem))) < 0)
			sp->shm_perm.seq = 0;
	}
}

/*
**	shmget - Shmget system call.
*/

shmget()
{
	register struct a {
		key_t	key;
		int	size,
			shmflg;
	}	*uap = (struct a *)u.u_ap;
	register struct shmid_ds	*sp;	/* shared memory header ptr */
	int				s;	/* ipcget status */
	int	size, ptsize;

	if((sp = ipcget(uap->key, uap->shmflg, shmem, shminfo.shmmni, sizeof(*sp),
		&s)) == NULL)
		return;
	if(s) {

		/* This is a new shared memory segment.  Allocate memory and
			finish initialization. */
		if(uap->size < shminfo.shmmin || uap->size > shminfo.shmmax) {
			u.u_error = EINVAL;
			sp->shm_perm.mode = 0;
			return;
		}
		size = btoc(uap->size);
		if (shmtot + size > shminfo.shmall) {
			u.u_error = ENOMEM;
			sp->shm_perm.mode = 0;
			return;
		}
		sp->shm_segsz = uap->size;
#ifndef m68k
		ptsize = btoc(sizeof(struct pt_entry) * size);
		sp->shm_ptbl = (struct pt_entry *)sptalloc(ptsize, PG_V|PG_KW, 0);
		if (sp->shm_ptbl == NULL ||
			memall(sp->shm_ptbl, size) == 0) {
			u.u_error = ENOMEM;
			sp->shm_perm.mode = 0;
			if (sp->shm_ptbl)
				sptfree(sp->shm_ptbl, ptsize, 1);
			return;
		}
		/* adjust maxmem for the segment and page table */
		maxmem -= (ptsize + size);
#else
		if( (sp->shm_paddr = mmualloc(size)) == 0 ) {
			u.u_error = ENOMEM;
			sp->shm_perm.mode = 0;
			return;
		}
		/* adjust maxmem for the segment */
		maxmem -= size;
#endif
		shmtot += size;

		sp->shm_perm.mode |= SHM_CLEAR;
		sp->shm_nattch = sp->shm_cnattch = 0;
		sp->shm_atime = sp->shm_dtime = 0;
		sp->shm_ctime = time;
		sp->shm_lpid = 0;
		sp->shm_cpid = u.u_procp->p_pid;
	} else
		if(uap->size && uap->size < sp->shm_segsz) {
			u.u_error = EINVAL;
			return;
		}
	u.u_rval1 = sp->shm_perm.seq * shminfo.shmmni + (sp - shmem);
}

/*
**	shmsys - System entry point for shmat, shmctl, shmdt, and shmget
**		system calls.
*/

shmsys()
{
	register struct a {
		uint	id;
	}	*uap = (struct a *)u.u_ap;
	int		shmat(),
			shmctl(),
			shmdt(),
			shmget();
	static int	(*calls[])() = {shmat, shmctl, shmdt, shmget};

	if(uap->id > 3) {
		u.u_error = EINVAL;
		return;
	}
	u.u_ap = &u.u_arg[1];
	(*calls[uap->id])();
}

#ifndef m68k
shmreset(p, ub, p0br, p0lr)
struct proc *p;
struct user *ub;
int *p0br, p0lr;
{
	register struct shmid_ds **sp;
	register struct shmpt_ds *pt;
	register i,j;
	register int *seg, shm, *pte;

/*
 * do only if there is shared memory attached
 */
	if (p->p_smbeg == 0)
		return;
/*
 * clear unused pte's
 */
	seg = p0br + ub->u_tsize + ub->u_dsize;
	for (i = ub->u_tsize + ub->u_dsize; i < p0lr; i++)
		*seg++ = 0;
/*
 * move in the shared memory segments
 */
	sp = &shm_shmem[i = (p - proc)*shminfo.shmseg];
	pt = &shm_pte[i];
	for (i = 0; i < shminfo.shmseg; i++, sp++, pt++) {
		if (shm = pt->shm_spte) {
			seg = p0br + shm;
			pte = (int *)((*sp)->shm_ptbl);
			for (j = 0; j < btoc((*sp)->shm_segsz); j++)
				*seg++ = *pte++ | PG_V | pt->shm_sflg;
		}
	}
}

dumppte(p0br,p0lr,p1lr,p1br)
int *p0br, p0lr, p1lr, *p1br;
{
	register i;

	printf("tsize %d, dsize %d\n", u.u_tsize, u.u_dsize);
	printf("p0br %x p1br %d\np1br %x p1lr %d\n\n",p0br,p0lr,p1br,p1lr);
	for (i=0; i<p0lr; i++) {
		if ((i%8) == 0)
			printf("\n");
		printf("%x  ",*p0br++);
	}
	printf("\n\n\n");
}
#endif
