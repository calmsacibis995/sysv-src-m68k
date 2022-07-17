/* @(#)trap.c	2.1.1.2	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#include "sys/reg.h"
#include "sys/psl.h"
#include "sys/trap.h"
#ifndef m68k
#include "sys/seg.h"
#endif
#include "sys/sysinfo.h"

#define	NSYSENT	64
#ifndef m68k
#define	USER	040	/* user-mode flag added to type */
#else
#define	USER	512	/* user-mode flag added to type */

/*
 * Called from the trap handler when a processor trap occurs.
 */
#ifdef m68k
#ifdef M68000
trap(sp, type, r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14, adr, ps, pc)
#else	/* M68010 */
trap(sp, r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14, ps, pc, kind, adr)
	/* "adr" is defined only for BUSERR & ADDRERR */
#endif
#else	/* vax */
trap(sp, type, code, pc, ps)
#endif
{
	register i;
	time_t syst;
#ifdef m68k
	register code = pc;			/* pc is the most common use */
#ifdef M68010
	register type = (int)(kind>>18)&255;	/* trap type */
#endif
#endif

	syst = u.u_stime;
#ifdef m68k
	u.u_traptype = TRAPNORM;
	u.u_ar0 = &r0;
#else
	u.u_ar0 = (int *)(0x80000000 - 18*4);
#endif

	if (USERMODE(ps))
		type |= USER;
	switch (type) {

	/*
	 * Trap not expected.
	 * Usually a kernel mode bus error.
	 */
	default:
		panicstr = "trap";	/* fake it for printfs */
		printf("user = ");
		printf("%x ", u.u_procp->p_addr);
		printf("\n");
		printf("ps = %x\n", ps);
		printf("pc = %x\n", pc);
		printf("trap type %x\n", type);
		printf("code = %x\n", code);
		panic(NULL);

#ifndef m68k
	case PROTFLT + USER:	/* protection fault */
		i = SIGBUS;
		break;

	case PRIVFLT + USER:	/* privileged instruction fault */
	case RSADFLT + USER:	/* reserved addressing fault */
	case RSOPFLT + USER:	/* reserved operand fault */
		i = SIGILL;
		break;
#endif

	case SYSCALL + USER:	/* sys call */
	{
		register *a;
		register struct sysent *callp;

		sysinfo.syscall++;
		u.u_error = 0;
		ps &= ~PS_C;
#ifdef m68k
		code = r0;		/* system call number */
		a = (int *)sp;		/* point to first arg */
#else
		a = (int *)u.u_ar0[AP];
#endif
		a++;		/* skip word with param count */
		i = code&0377;
		if (i >= NSYSENT)
			i = 0;
		else if (i==0) {	/* indirect */
			i = fuword(a++)&0377;
			if (i >= NSYSENT)
				i = 0;
		}
		callp = &sysent[i];
		for(i=0; i<callp->sy_narg; i++) {
			u.u_arg[i] = fuword(a++);
		}
		u.u_dirp = (caddr_t)u.u_arg[0];
		u.u_rval1 = 0;
#ifdef m68k
		u.u_rval2 = r1;
#else
		u.u_rval2 = u.u_ar0[R1];
#endif
		u.u_ap = u.u_arg;
		if (setjmp(u.u_qsav)) {
			if (u.u_error==0)
				u.u_error = EINTR;
		} else {
			(*callp->sy_call)();
		}
		if (u.u_error) {
#ifdef m68k
			r0 = u.u_error;
#else
			u.u_ar0[R0] = u.u_error;
#endif
			ps |= PS_C;	/* carry bit */
			if (++u.u_errcnt > 16) {
				u.u_errcnt = 0;
				runrun++;
			}
		} else {
#ifdef m68k
			r0 = u.u_rval1;
			r1 = u.u_rval2;
#else
			u.u_ar0[R0] = u.u_rval1;
			u.u_ar0[R1] = u.u_rval2;
#endif
		}
	}
#ifdef m68k
	/*
	********************************************************
	If the system call trap was executed with the T bit set,
	the trace trap will be ignored.  So, the bit is reset
	and the SIGTRAP signal is sent to the process.
	********************************************************
	*/
	if (ps & PS_T) {	/* was trace bit set ? */
		i = SIGTRAP;
		ps &= ~PS_T;	/* reset trace bit */
		break;		/* send signal to process */
	}
#endif

	calcpri:
	{
		register struct proc *pp;

		pp = u.u_procp;
		pp->p_pri = (pp->p_cpu>>1) + PUSER + pp->p_nice - NZERO;
		curpri = pp->p_pri;
		if (runrun == 0)
			goto out;
	}

	case RESCHED + USER:	/* Allow process switch */
		qswtch();
		goto out;

#ifdef m68k
	/*
	******************************************************
	This condition will happen when a TRAP instruction is
	executed with the T bit set. This will only be taken
	care of by the system if it happens while doing a
	system call. All other occurences of this phenomenon
	will cause the breakpoint to be ignored.
	******************************************************
	*/
	case TRCTRAP:		/* trace trap in kernel mode */
		return;		/* ignore this trap */

	case FPETRAP + USER:	/* floating point exception */
		i = SIGFPE;
		code = r0;
		break;

	case ZDVDERR + USER:	/* zero divide check */
		i = SIGFPE;
		code = KINTDIV;
		break;

	case CHKTRAP + USER:	/* CHK trap */
		i = SIGFPE;
		code = KSUBRNG;
		break;

	case ADDRERR + USER:	/* address error */
		i = SIGBUS;
		code = adr;
		u.u_traptype = TRAPADDR;
		break;

	case TRAPVFT + USER:	/* TRAPV instruction */
		i = SIGFPE;
		code = KINTOVF;
		break;

	case IOTTRAP + USER:	/* IOT trap */
		i = SIGIOT;
		code = r0;
		break;

	case EMTTRAP + USER:	/* EMT trap */
		i = SIGEMT;
		code = r0;
		break;

	case STRAYFT:		/* spurious interrupt */
	case STRAYFT + USER:
		stray(type<<2);
		return;

#else	/* vax */
	case ARTHTRP + USER:
		i = SIGFPE;
		break;
#endif

	/*
	 * If the user SP is below the stack segment,
	 * grow the stack automatically.
	 */
#ifndef m68k
	case SEGFLT + USER:	/* segmentation exception */
		if (grow(u.u_ar0[SP]) || grow(code))
			goto calcpri;
#else /* m68k */
	case BUSERR + USER:	/* user bus error */
		code = adr;
#ifndef M68000
		if (grow(sp) || grow(code))
			goto calcpri;
#endif
		u.u_traptype = TRAPBUS;
#endif
		i = SIGSEGV;
		break;

	case BPTFLT + USER:	/* bpt instruction fault */
#ifdef m68k
		pc -= 2;	/* back up pc for simulated breakpoint */
#endif
	case TRCTRAP + USER:	/* trace trap */
		ps &= ~PS_T;	/* turn off trace bit */
		i = SIGTRAP;
		break;

#ifndef m68k
	case XFCFLT + USER:	/* xfc instruction fault */
		i = SIGEMT;
		break;

	case CMPTFLT + USER:	/* compatibility mode fault */
				/* so far, just send a SIGILL signal */
#else	/* m68k */
	case PRIVFLT + USER:	/* privileged instruction fault */
	case L1111FT + USER:	/* Line 1111 emulator trap */
	case L1010FT + USER:	/* Line 1010 emulator trap */
	case INSTERR + USER:	/* Illegal instruction */
#endif
		i = SIGILL;
		break;
	}
	psignal(u.u_procp, i);

out:
	if (u.u_procp->p_sig && issig())
		psig(code);
	if (u.u_prof.pr_scale)
		addupc((caddr_t)u.u_ar0[PC], &u.u_prof, (int)(u.u_stime-syst));
}

/*
 * nonexistent system call-- signal bad system call.
 */
nosys()
{
	psignal(u.u_procp, SIGSYS);
}

/*
 * Ignored system call
 */
nullsys()
{
}

stray(addr)
{
	logstray(addr);
	printf("stray interrupt at %x\n", addr);
}
