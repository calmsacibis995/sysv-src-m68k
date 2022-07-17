/* @(#)machdep.c	2.18.1.2	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#ifndef m68k
#include "sys/seg.h"
#endif
#include "sys/map.h"
#include "sys/reg.h"
#include "sys/psl.h"
#include "sys/utsname.h"
#ifndef m68k
#include "sys/mtpr.h"
#include "sys/clock.h"
#include "sys/page.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#endif

/*
 * Machine-dependent startup code
 */
startup()
{

#if m68k && !UNIX
	printf("\nM68000/%s: %s%s\n", utsname.release, utsname.sysname, utsname.version);
#else
	printf("\nUNIX/%s: %s%s\n", utsname.release, utsname.sysname, utsname.version);
#endif
#ifdef DEBUG
	printf("14:38:58 3/14/84\n");
#endif
	printf("real mem  = %d\n", physmem*ctob(1) );
	printf("avail mem = %d\n", maxmem*ctob(1));
	mfree(swapmap, nswap, 1);
	swplo--;
#ifndef m68k
	mbainit();		/* setup mba mapping regs map */
	ubainit();		/* setup uba mapping regs map */
#endif
}

#ifndef m68k
/*
 * Start clock
 */
clkstart()
{
	mtpr(NICR, -16667);	/* 16.667 milli-seconds */
	mtpr(ICCS,ICCS_RUN+ICCS_IE+ICCS_TRANS+ICCS_INT+ICCS_ERR);
}

clkset(oldtime)
time_t	oldtime;
{
	time = udiv(mfpr(TODR),100);
	while (time < oldtime)
		time += SECYR;
}

clkreld(on)
{
	if (on)
		mtpr(ICCS, ICCS_RUN + ICCS_IE + ICCS_INT + ICCS_ERR);
	else
		mtpr(ICCS, ICCS_INT);
}

timepoke()
{
	mtpr(SIRR, 0xf);
}
#endif

/*
 * Send an interrupt to process
 */
sendsig(hdlr, signo, arg)
{
#ifdef m68k
	register *usp, *regs;

	regs = u.u_ar0;
	usp = (int *)regs[SP];
	grow((unsigned)(usp-5));
	/* simulate an interrupt on the user's stack */
	suword((caddr_t)--usp, regs[PC]);
	suword((caddr_t)--usp, (regs[PS] & 0xffff) | (u.u_traptype << 16));
	suword((caddr_t)--usp, arg);
	suword((caddr_t)--usp, signo);
	regs[SP] = (int)usp;
	regs[PC] = (int)hdlr;
#else
	register *usp, *regs;
	extern sigcode();

	regs = u.u_ar0;
	usp = (int *)regs[SP];
	grow((unsigned)(usp-5));
	/* simulate an interrupt on the user's stack */
	suword((caddr_t)--usp, regs[PS]);
	suword((caddr_t)--usp, regs[PC]);
	/* with three parameters */
	suword((caddr_t)--usp, hdlr);
	suword((caddr_t)--usp, arg);
	suword((caddr_t)--usp, signo);
	regs[PS] &= ~(PS_CM|PS_FPD);
	regs[SP] = (int)usp;
	regs[PC] = (int)sigcode;
#endif
}

#ifndef m68k
mtpr(regno, value)
{
	asm("	mtpr	8(ap),4(ap)");
}

mfpr(regno)
{
	asm("	mfpr	4(ap),r0");
}

/*
 * copy count bytes from from to to.
 */
bcopy(from, to, count)
caddr_t from, to;
{
	asm("	movc3	12(ap),*4(ap),*8(ap)");
}
/*
 * Zero sections of memory.
 */
bzero(addr, size)
{
	asm("	movc5	$0,*4(ap),$0,8(ap),*4(ap)");
}

/*
 * create a duplicate copy of a process
 */
procdup(p)
register struct proc *p;
{
	register *ip, *kp;
	register i, n, *dspt;
	extern struct user *uservad;

	n = p->p_nspt;
	if ((dspt = (int *)sptalloc(n, PG_V | PG_KW, 0)) == NULL)
		return(NULL);
	if (u.u_dsize)
		if (memall(&dspt[u.u_tsize],u.u_dsize) == 0) {
			sptfree(dspt, n, 1);
			return(NULL);
		}
	if (u.u_ssize)
		if (memall(&dspt[n*128-u.u_ssize],u.u_ssize) == 0)  {
			memfree(&dspt[u.u_tsize],u.u_dsize);
			sptfree(dspt, n, 1);
			return(NULL);
		}

	/* copy the data segment */
	ip = p->p_spt + u.u_tsize;
	kp = dspt + u.u_tsize;
	for (i = u.u_dsize; --i >= 0; ) {
		copyseg(*ip++&PG_PFNUM, *kp);
		*kp++ |= PG_V | PG_UW;
	}

	/* copy stack segment */
	ip = p->p_spt + n*128 - u.u_ssize;
	kp = dspt + n*128 - u.u_ssize;
	for (i = u.u_ssize - USIZE; --i >= 0; ) {
		copyseg(*ip++&PG_PFNUM, *kp);
		*kp++ |= PG_V | PG_UW;
	}
	for (i = USIZE; --i >= 0; ) {
		copyseg(*ip++&PG_PFNUM, *kp);
		*kp++ |= PG_V | PG_KW;
	}

	/* copy text page table entries */
	bcopy(p->p_spt, dspt, sizeof(struct pt_entry)*u.u_tsize);
	/* set up new u area address */
	uaccess(&dspt[n*128 - USIZE]);
	uservad->u_pcb.pcb_p0br = (int)dspt;
	uservad->u_pcb.pcb_p1br = (int)(dspt + n*128 - 0x200000);
	p->p_spt = dspt;

	/* Copy shared memory page table entries */
	shmreset(p, uservad, uservad->u_pcb.pcb_p0br, uservad->u_pcb.pcb_p0lr&0x003fffff);
	return(1);
}

/*
 * change protection codes of text
 */
chgprot(text)
{
	register *ptaddr, i;

	if (text) {
		ptaddr = (int *)mfpr(P0BR);
		for(i=0; i<u.u_tsize; i++) {
			ptaddr[i] &= ~PG_PROT;	/* clear prot bits */
			ptaddr[i] |= PG_V | text;
			mtpr(TBIS, ctob(i));
		}
	}
}

chksize(text, data, stack)
{
	register n;

	n = text + data + stack;
	n += (n + 127)/128;
	if (n > MAXMEM || n > maxmem) {
		u.u_error = ENOMEM;
		return(-1);
	}
	return(0);
}

/*
 * expand a page table
 */
ptexpand(change)
register change;
{
	register struct proc *p = u.u_procp;
	register int *p1, *p2, i;
	register *dspt;

	if (change <= 0)
		return(change);
	if ((change > maxmem/128) || (change > MAXMEM/128))
		return(-1);
	if ((dspt = (int *)sptalloc(p->p_nspt + change, PG_V | PG_KW, 0)) == 0)
		return(-1);
	p1 = p->p_spt;
	p2 = dspt;
	for (i = mfpr(P0LR); --i>=0; )
		*p2++ = *p1++;
	p1 = p->p_spt + p->p_nspt*128;
	p2 = dspt + (p->p_nspt + change)*128;
	for (i = 0x200000 - mfpr(P1LR); --i>=0; )
		*--p2 = *--p1;
	p1 = p->p_spt;
	p->p_spt = dspt;
	p->p_nspt += change;
	u.u_pcb.pcb_p0br = (int)dspt;
	mtpr(P0BR, u.u_pcb.pcb_p0br);
	u.u_pcb.pcb_p1br = (int)(dspt + p->p_nspt*128 - 0x200000);
	mtpr(P1BR, u.u_pcb.pcb_p1br);
	mtpr(TBIA,0);
	sptfree(p1, p->p_nspt - change, 1);
	return(change);
}

int	*memvad;
int	*sbrpte;
int	*copypte, *copyvad;
int	*userpte;
struct user *uservad;
int	*mmpte, *mmvad;
int	*pmapte, *pmavad;
int	*memcvad;

mlsetup(lastaddr, startpc)
{
	register i;
	register int *sbr, *spte;
	int	nspte, nmem, ubase;
	extern	end, etext;
	extern sigcode();

	physmem = btoc(lastaddr);
	nspte = physmem/128;	/* pages for one spte per page of physmem */
	nmem = (physmem + 0xfff) >> 12;
	sbr = (int *)&end;
	mtpr(SBR, (int)sbr - SYSVA);
	mtpr(SLR, nspte*128);
	sbrpte = sbr;
	for (i = 0; i < btoc((int)(&etext) - SYSVA); i++)
		*sbr++ = PG_V | PG_KR | i;
	for ( ; i < (btoc((int)(&end) - SYSVA) + nspte); i++)
		*sbr++ = PG_V | PG_KW | i;
	mfree(sptmap, nspte*128 - i, i);
	mtpr(TBIA, 0);
	mtpr(MAPEN, 1);
	sbrpte[((int)sigcode - SYSVA)>>9] |= PG_URKR;
	memvad = (int *)sptalloc(nmem, PG_V | PG_KW, i);
	ubase = i + nmem;
	spte = (int *)sptalloc(1, PG_V | PG_KW, ubase++);
	mtpr(P0BR, spte);
	mtpr(P0LR, 0);
	curproc = &proc[0];
	proc[0].p_spt = spte;
	proc[0].p_nspt = 1;
	proc[0].p_addr = ubase;
	proc[0].p_size = USIZE;
	proc[0].p_ssize = USIZE;
	spte += 128 - USIZE;
	for (i = 0; i < USIZE; i++)
		*spte++ = PG_V | PG_KW | ubase++;
	mtpr(P1BR, spte - 0x200000);
	mtpr(P1LR, 0x200000 - USIZE);
	meminit(ubase, physmem);
	u.u_pcb.pcb_ksp = 0x80000000;
	u.u_pcb.pcb_esp = -1;
	u.u_pcb.pcb_ssp = -1;
	u.u_pcb.pcb_usp = (int)&u;
	u.u_pcb.pcb_p0br = mfpr(P0BR);
	u.u_pcb.pcb_p0lr = mfpr(P0LR) | (4<<24);
	u.u_pcb.pcb_p1br = mfpr(P1BR);
	u.u_pcb.pcb_p1lr = mfpr(P1LR);
	u.u_pcb.pcb_pc = startpc;
	u.u_pcb.pcb_psl = 0;
	u.u_ssize = USIZE;
	mtpr(PCBB, ctob(proc[0].p_addr));
	copyvad = (int *)sptalloc(2, 0, -1);
	copypte = svtopte(copyvad);
	uservad = (struct user *)sptalloc(btoc(sizeof u), 0, -1);
	userpte = svtopte(uservad);
	mmvad = (int *)sptalloc(1, 0, -1);
	mmpte = svtopte(mmvad);
	pmavad = (int *)sptalloc(1, 0, -1);
	pmapte = svtopte(pmavad);
	if (cputype == 780)
		memcvad = (int *)sptalloc(1, PG_V | PG_KW, btoc(0x20002000));
	else
		memcvad = (int *)sptalloc(1, PG_V | PG_KW, btoc(0xf20000));
}

sptalloc(size, mode, base)
{
	register i, sp;

	if ((sp = malloc(sptmap, size)) == 0)
		return(NULL);
	if (base == 0 && memall(&sbrpte[sp], size) == 0) {
		mfree(sptmap, size, sp);
		return(NULL);
	}
	for (i = 0; i < size; i++) {
		if (base > 0)
			sbrpte[sp + i] = mode | base++;
		else
			sbrpte[sp + i] |= mode;
		mtpr(TBIS, ctosv(sp + i));
	}
	return(ctosv(sp));
}

sptfree(vaddr, size, flag)
{
	register i, sp;

	sp = svtoc(vaddr);
	if (flag)
		memfree(&sbrpte[sp], size);
	for (i = 0; i < size; i++) {
		sbrpte[sp + i] = 0;
		mtpr(TBIS, ctosv(sp + i));
	}
	mfree(sptmap, size, sp);
}

uaccess(spte)
register *spte;
{
	register i, v;

	v = (int)uservad;
	for (i=0; i<btoc(sizeof u); i++) {
		userpte[i] = *spte++;
		mtpr(TBIS, v + ctob(i));
	}
}
#endif
