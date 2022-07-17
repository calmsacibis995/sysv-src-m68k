/* @(#)lp.c	2.1.1.1	 */
/*
 * EXORmacs line printer driver
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/tty.h"
#include "sys/lprio.h"

extern struct device *lp_addr[];
extern	lp_cnt;

#define	PRIN	0	/* minor device 0 ==> Printronix */
#define	CENT	1	/* minor device 1 ==> Centronics */
#define	START	2	/* minor device 2 ==> start only (not h/w interrupt) */
#define	unit	0	/* only one unit supported */
#define	LPMAX	1	/* only one unit supported */

#define	LPPRI	(PZERO+8)
#define	LPLOWAT	40
#define	LPHIWAT	100

struct device
{
	unsigned char dreg;			/* output data register */
	char fill0;				/* fill char */
	unsigned char cra;			/* control register A */
	char fill1;				/* fill char */
	unsigned char sreg;			/* status register */
	char fill2;				/* fill char */
	unsigned char crb;			/* control register B */
	char fill3;				/* fill char */
};

struct lp {
	struct	clist l_outq;
	char	flag, ind;
	int	ccc, mcc, mlc;
	int	line, col;
} lp_dt[LPMAX];

#define	OPEN	010
#define	CAP	020
#define	NOCR	040
#define	ASLP	0100


#define SEL	1		/* printer is selected */	
#define OUTPP	2		/* printer is out of paper */
#define PBUSY	4		/* printer busy */
#define FOLLOW	0x30		/* follow mode for strobe line */
#define STROBE	0x8		/* toggle bit to send data or reset */
#define IE	1		/* interrupt enable */
#define IRQ	0x80		/* interrupt request */
#define	IOSEL   4		/* I/O register select */
#define LOtoHI	2		/* Interrupt edge select */
#define	OUT     0xff		/* all 8 bits selected for output */
#define	INP     0		/* all 8 bits selected for input */

lpopen(dev, mode)
{
	register struct lp *lp;
	register struct device *lpdev;

	if (dev > LPMAX || (lp = &lp_dt[unit])->flag)
	{
		u.u_error = EIO;
		return;
	}
	lpdev = lp_addr[unit];
	if (lp->col == 0) {		/* first time only */
		lp->ind = 4;
		lp->col = 132;
		lp->line = 66;
		lpdev->cra = 0;		/* A-side pia set for data output */
		lpdev->dreg = OUT;
		lpdev->cra = FOLLOW|STROBE|IOSEL|LOtoHI;
		lpdev->crb = 0;		/* B-side pia set for status input */
		lpdev->sreg = INP;
		lpdev->crb = FOLLOW|IOSEL|LOtoHI;	/* reset printer */
		lpdev->crb = FOLLOW|STROBE|IOSEL|LOtoHI;
		/* may need a delay here to give time to reset */
		/* may also require select code to be sent */
	}
	if (((SEL|OUTPP)&lpdev->sreg) != SEL)
	{
		u.u_error = EIO;
		return;
	}
	lp->flag = OPEN;
	lpdev->cra |= IE;
	lpdev->crb |= IE;
	lpoutput(dev, '\f');
}

lpclose(dev)
{
	lpoutput(dev, '\f');
	lp_dt[unit].flag = 0;
}

lpwrite(dev)
{
	register c;
	register struct lp *lp;

	lp = &lp_dt[unit];
	while (u.u_count) {
		spl5();
		while(lp->l_outq.c_cc > LPHIWAT) {
			lpintr(START);
			lp->flag |= ASLP;
			sleep(lp, LPPRI);
		}
		spl0();
		c = fubyte(u.u_base++);
		if (c < 0) {
			u.u_error = EFAULT;
			break;
		}
		u.u_count--;
		lpoutput(dev, c);
	}
	spl5();
	lpintr(START);
	spl0();
}

lpoutput(dev, c)
register dev, c;
{
	register struct lp *lp;

	lp = &lp_dt[unit];
	if(lp->flag&CAP) {
		if(c>='a' && c<='z')
			c += 'A'-'a'; else
		switch(c) {
		case '{':
			c = '(';
			goto esc;
		case '}':
			c = ')';
			goto esc;
		case '`':
			c = '\'';
			goto esc;
		case '|':
			c = '!';
			goto esc;
		case '~':
			c = '^';
		esc:
			lpoutput(dev, c);
			lp->ccc--;
			c = '-';
		}
	}
	switch(c) {
	case '\t':
		lp->ccc = ((lp->ccc+8-lp->ind) & ~7) + lp->ind;
		return;
	case '\n':
		lp->mlc++;
		if(lp->mlc >= lp->line )
			c = '\f';
	case '\f':
		if (dev&CENT)
			putc('\r', &lp->l_outq);
		lp->mcc = 0;
		if (lp->mlc) {
			putc(c, &lp->l_outq);
			if(c == '\f')
				lp->mlc = 0;
		}
		if (dev&CENT)
			goto strtlp;
	case '\r':
		putc('\r', &lp->l_outq);
	   strtlp:
		lp->ccc = lp->ind;
		spl5();
		lpintr(START);
		spl0();
		return;
	case '\b':
		if(lp->ccc > lp->ind)
			lp->ccc--;
		return;
	case ' ':
		lp->ccc++;
		return;
	default:
		if(lp->ccc < lp->mcc) {
			if (lp->flag&NOCR) {
				lp->ccc++;
				return;
			}
			putc('\r', &lp->l_outq);
			lp->mcc = 0;
		}
		if(lp->ccc < lp->col) {
			while(lp->ccc > lp->mcc) {
				putc(' ', &lp->l_outq);
				lp->mcc++;
			}
			putc(c, &lp->l_outq);
			lp->mcc++;
		}
		lp->ccc++;
	}
}

lpintr(dev)
{
	register struct lp *lp;
	register struct device *lpdev;
	register char c;

	lp = &lp_dt[unit];
	lpdev = lp_addr[unit];

	if (!(dev & START))
	{	/* hardware interrupt caused this call to lpintr */
		if (!(lpdev->cra & IRQ || lpdev->crb & IRQ))
			return;		/* not lp interrupt */

		/* valid lp interrupt being serviced */
		c = lpdev->dreg;	/* clear interrupts */
		c = lpdev->sreg;

	}

	while (((lpdev->sreg&(PBUSY|OUTPP|SEL))==SEL)
		&& (c = getc(&lp->l_outq)) >= 0)
		{
		/* strobe character out and disable interrupt */
		lpdev->dreg = c;
		lpdev->cra = FOLLOW|IOSEL|LOtoHI|IE;
		lpdev->cra = FOLLOW|STROBE|IOSEL|LOtoHI|IE;
		}
	if (lp->l_outq.c_cc <= LPLOWAT && lp->flag&ASLP) {
		lp->flag &= ~ASLP;
		wakeup(lp);
	}
}

lpioctl(dev, cmd, arg, mode)
{
	register struct lp *lp;
	struct lprio lpr;

	lp = &lp_dt[unit];
	switch (cmd)
	{
	default:
		u.u_error = EINVAL;
		return;

	case LPRGET:
		lpr.ind = lp->ind;
		lpr.col = lp->col;
		lpr.line = lp->line;
		if (copyout(&lpr, arg, sizeof lpr))
			u.u_error = EFAULT;
		return;

	case LPRSET:
		if (copyin(arg, &lpr, sizeof lpr)) {
			u.u_error = EFAULT;
			return;
		}
		lp->ind = lpr.ind;
		lp->col = lpr.col;
		lp->line = lpr.line;
	}
}
