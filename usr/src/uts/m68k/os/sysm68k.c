/* @(#)sysm68k.c	2.1.1.1	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#include "sys/reg.h"

#ifdef m68k
/*
 * sysm68k - system call
 * 1) Called to grow the user stack on the MC68000
 * 2) Called to perform instruction continuation after signal handlers
 */
sysm68k ()
{
	struct a {
		int cmd;
		int arg1;
		int arg2;
	};

	register i, kind, *regs, *usp;

	switch (((struct a *)u.u_ap)->cmd) {

 	case 1:		/* grow/shrink stack */
		i = ((struct a *)u.u_ap)->arg1;
		i = i<0? -btoc( -i ): btoc( i );

		if( mmuexpand( SEG_STACK, i ) < 0 ) {
			u.u_error = ENOMEM;
			return;
		}
		mmuregs(u.u_procp);
		u.u_ssize += i;
		u.u_rval1 = USRSTACK - ctob(u.u_procp->p_ssize - USIZE);
		return;

	case 2:		
		/* continue from signal handler */

		regs = u.u_ar0;
		usp = (int *) regs[SP];

		/* find the return address for the 'rtr' */
		/* in the user's stack area */
		/* stack format: */
		/*   0 - _sysm68k return addr */
		/*   1 - command */
		/*   2 - old %d0 */
		/*   3 - <kind of resume, previous trap process status reg> */
		/*   4 - previous trap PC */

 		/* get resume kind */
		kind = (fuword((caddr_t) (usp+3)) >> 16) & 0xffff;
		u.u_traptype = kind; /* make it appear that we just got here */
				     /* for the first time for psignal */

		regs[PC] = fuword((caddr_t) (usp+4)); 
		/* make sure we don't allow transfer to supervisor mode */
		regs[PS] = (regs[PS] & 0xff00) |
			   (fuword((caddr_t) (usp+3)) & 0xff);
		u.u_rval1 = fuword((caddr_t) (usp+2));	/* user's D0 */
		usp += 5;
		regs[SP] = (int) usp;

		if (kind == TRAPADDR) /* addr error */
			psignal(u.u_procp, SIGBUS);
		else if (kind == TRAPBUS) /* bus error */
			psignal(u.u_procp, SIGSEGV);
		/* else we can just resume here */
		break;

	default:
		u.u_error = EINVAL;
	}
}
#endif
