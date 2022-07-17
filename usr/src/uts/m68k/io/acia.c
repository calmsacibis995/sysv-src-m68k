/* @(#)acia.c	2.1.1.3	 */

/* This file contains all the associated routines for the EXORmacs acia driver.
 * In addition, it contains the putchar() routine for the console.
 */

char EXORcon;	/* provide a unique identifier so this putchar can be linked */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/tty.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/termio.h>
#include <sys/conf.h>
#include <sys/sysinfo.h>

#ifdef DEBUG
char DebugACIA = 0;			/* 0 means no debug printouts */
#endif
#define console		0		/* console index */
#define XDONE		0x02		/* xmit data reg empty */
#define LF		0x0a		/* ascii line feed */
#define CR		0x0d		/* ascii carriage return */
#define FALSE		0

/* Control Masks */
#define RESET		0x03		/* master reset */
#define AINIT		0x17		/* init mask (8b1sNo|RESET */
#define DIV16		0x01		/* divide clock by 16 */
#define DEFAULT		0x15		/* DIV16 | 8b1sno 
					   with xmit & rec ints disabled  */
#define XID		0x9f		/* xmit interrupt disable */
#define XIDHUP		0x40		/* xmit interrupt disable & hangup */
#define RIE		0x80		/* receive interrupt enable */
#define XIE		0x20		/* transmit interrupt enable */
#define INTOFF		0x5f		/* interrupt disable mask */
#define XBRK		0x60		/* send break with XID */
#define OPTNS		0x1c		/* options field */

/* Status Mask */
#define RCVDONE		0x01		/* charcter received */
#define XDONE		0x02		/* transmit ready */
#define NOCARR		0x04		/* no carrier when set */
#define FE		0x10		/* framing error */
#define OVRN		0x20		/* overrun error */
#define PE		0x40		/* parity error */

/* Option Modes */
#define _7b1sEv		0x08		/* 7 bits, 1 stop, even parity */
#define _7b1sOd		0x0c		/* 7 bits, 1 stop, odd parity */
#define _7b2sEv		0x00		/* 7 bits, 2 stop, even parity */
#define _7b2sOd		0x04		/* 7 bits, 2 stop, odd parity */
#define _8b1sNo		0x14		/* 8 bits, 1 stop, no parity */
#define _8b1sEv		0x18		/* 8 bits, 1 stop, even parity */
#define _8b1sOd		0x1c		/* 8 bits, 1 stop, odd parity */
#define _8b2sNo		0x10		/* 8 bits, 2 stop, no parity */

#define ACIAMAX		22		/* 5 boards * 4 devices/board
					 + 2 devices on debug board */

#define XMODE(X,Y)	((X) = (((X) & XID) | (Y)))
					/* disables xmit interrupt and
					   expects Y to be a value affecting
					   only bits (5:6), xmit control */

/* Acia device structure assumes an odd starting address */

struct device
{
	char csreg;			/* control and status regs */
	char fill0;			/* fill char */
	char dreg;			/* xmit and receive data regs */
	char fill1;			/* fill char */
};

#define acia_breg ((char *) 0xfee001)	/* debug board baud rate reg */

char	acia_speeds[] 		/* values to place in baud rate reg */
		= { 0 /*  00 */,  0 /*  50 */,  1 /*  75 */,  2 /* 110 */,
		    3 /* 134 */,  4 /* 150 */,  4 /* 200 */,  5 /* 300 */,
		    6 /* 600 */,  7 /* 1200*/,  8 /* 1800*/, 10 /* 2400*/,
		   12 /*4800 */, 14 /* 9600*/};

extern int acia_cnt;			/* number of acia's */
extern struct device *acia_addr[];	/* addresses of acia boards */
extern struct tty acia_tty[];		/* acia tty structure */
extern ttrstrt();
char a_state[ACIAMAX] = {DEFAULT};	/* acia control reg contents and
					   initial a_state[console] */

struct device *acia_dev[ACIAMAX];	/* acia device pointers */

char a_mode[] = {			/* options modes */
		_8b1sNo, _8b1sNo, _7b1sEv, _7b1sOd,
		_8b1sNo, _8b1sNo, _7b2sEv, _7b2sOd,
		_8b1sNo, _8b1sNo, _8b1sEv, _8b1sOd,
		_8b2sNo, _8b2sNo, _8b1sNo, _8b1sNo,
		};

#define MISSING	((struct device *)0)	/* writes to device cause bus errors */

#define ACIA_ACT	02	/* character in buffer flag */
char acia_work;			/* work flag */


aciaopen(dev, flag)
unsigned int dev, flag;
{
	register struct tty *tp;
	register struct device *addr;
	register char *state;
	register int opt = {0};
	extern aciaproc();

	if (dev >= acia_cnt)			/* check for valid dev */
	{
		u.u_error = ENXIO;
		return;
	}
	addr = acia_dev[dev];			/* get dev address */
	if( addr == MISSING )
	{
		u.u_error = ENXIO;		/* device is missing */
		return;
	}
	tp = &acia_tty[dev];			/* get tty struct */
	state = &a_state[dev];

	if ((tp->t_state & (ISOPEN|WOPEN)) == FALSE)
	{
		ttinit(tp);			/* init tty structure */
		tp->t_proc = aciaproc;
		addr->csreg = RESET;
		aciaparam(dev);
	}
	if (!(addr->csreg & NOCARR))		/* update carrier flag */
		tp->t_state |= CARR_ON;
	if (!(flag & FNDELAY))
		while ((tp->t_state & CARR_ON) == FALSE)
		{
			tp->t_state |= WOPEN;
			sleep((caddr_t)&tp->t_canq, TTIPRI);
		}
	(*linesw[tp->t_line].l_open)(tp);
#ifdef DEBUG
	if (DebugACIA)
		printf("aciaopen: opened dev %d\n", dev);
#endif
}


aciaclose(dev)
unsigned int dev;
{
	register struct tty *tp;
	register char *state;
	register struct device *addr;

	addr = acia_dev[dev];
	tp = &acia_tty[dev];
	state = &a_state[dev];

	(*linesw[tp->t_line].l_close)(tp);
	if (tp->t_cflag & HUPCL && !(tp->t_state & ISOPEN))
	{
		XMODE(*state, XIDHUP);
		addr->csreg = *state;
#ifdef DEBUG
		if (DebugACIA)
		{
			printf("aciaclose: closed dev %d state %x\n",
				dev, tp->t_state);
			if(tp->t_state & BUSY)
				printf(" Closed while busy \n");
		}
#endif
		tp->t_state &= ~BUSY;	/* guarantee not busy since XID */
	}
}


aciaread(dev)
unsigned int dev;
{
	register struct tty *tp;

	tp = &acia_tty[dev];
	(*linesw[tp->t_line].l_read)(tp);
#ifdef DEBUG
	if (DebugACIA)
		printf("aciaread: read dev %d\n", dev);
#endif
}


aciawrite(dev)
unsigned int dev;
{
	register struct tty *tp;

	tp = &acia_tty[dev];
	(*linesw[tp->t_line].l_write)(tp);
#ifdef DEBUG
	if (DebugACIA)
		printf("aciawrite: wrote to dev %d\n", dev);
#endif
}


aciaioctl(dev, cmd, arg, mode)
unsigned int dev, cmd, arg, mode;
{
	register struct tty *tp;
	register char *state;

	tp = &acia_tty[dev];

	if (ttiocom(tp, cmd, arg, mode))
		aciaparam(dev);
}


aciaparam(dev)
unsigned int dev;
	{
	register int		opt;		/* control reg. word select */
	register struct tty 	*tp;
	register char		*state;		/* current state of acia */
	register struct device	*addr;		/* phyisical address of acia */
	register int		flags;		/* input mode flags */

	addr = acia_dev[dev];
	if( addr == MISSING )
	{
		u.u_error = ENXIO;		/* device is missing */
		return;
	}
	tp = &acia_tty[dev];
	state = &a_state[dev];

	/* if a time driven scanner is implemented, a call to the scanner
	 * (aciascan) should be inserted here.
	 */

	flags = tp->t_cflag;
#ifdef DEBUG
	if (DebugACIA)
		{
		if(tp->t_state & BUSY)
		printf("aciaparam: changed dev %d while in state %x\n", 
		dev, tp->t_state);
		}
#endif

	*state |= RIE;			/* enable rec interrupt */
	/* xmit interrupt is only enabled while there is output */

	if ((flags & CBAUD) == B0)	/* hang up on zero baud rate */
	{
		tp->t_state &= ~BUSY;	/* guarantee not busy since XID */
		XMODE(*state, XIDHUP);  /* is forced when line is off */
		addr->csreg = *state;
		return;
	}

	opt = 0;
	if ((flags & CS8) == CS8)	/* if CS8 set 8 bits else 7 */
		opt |= 8;
	if (flags & CSTOPB)		/* if CSTOPB, 2 stop bits */
		opt |= 4;		/* else 1 stop bit */
	if (flags & PARENB)		/* Parity enable */
		opt |= 2;
	if (flags & PARODD)		/* odd parity else even */
		opt |= 1;

	spl5();				/* guarantee no interrupt */
	*state &= ~OPTNS;		/* clear old options */
	*state |= a_mode[opt];		/* set new ones */
	addr->csreg = *state;
	spl0();				/* allow ints again */

	if (dev <= 1)		/* 0 and 1 are on the debug board */
	{
		register char baud_rate;
		register char state_breg;

		baud_rate = acia_speeds[flags & CBAUD];
		if (baud_rate)		/* skip if just shutting off line */
		{
			state_breg = *acia_breg;	/* read baud rate reg */
			if (dev == console)
			{
				/* baud rate for console is in high nibble */
				state_breg &= 0x0f;
				state_breg |= (baud_rate << 4);
			}
			else
			{
				/* baud rate for 2nd acia is in low nibble */
				state_breg &= 0xf0;
				state_breg |= baud_rate;
			}
			*acia_breg = state_breg;
		}
	}
}


aciaproc(tp, cmd)
register struct tty *tp;
unsigned int cmd;
{
	register struct device *addr;
	unsigned int dev;
	register char *state;

	dev = tp - acia_tty;
	state = &a_state[dev];
	addr = acia_dev[dev];

	switch(cmd)
	{
	case T_TIME:		/* resuming output after delay */
		tp->t_state &= ~(TIMEOUT | BUSY);  /* insure ~BUSY since */
		addr->csreg = (*state &= ~XBRK);   /* break leaves intrs off */
		goto start;

	case T_WFLUSH:		/* flush output queue */
		tp->t_tbuf.c_size -= tp->t_tbuf.c_count;
		tp->t_tbuf.c_count = 0;
		/* fall thru */

	case T_RESUME:		/* resume output, XON has been received */
		tp->t_state &= ~TTSTOP;
		/* fall thru */

	case T_OUTPUT:		/* handle first or next character */
	start:
	{
		register struct ccblock *tbuf;

		if (tp->t_state & (BUSY|TTSTOP))
			break;
		tbuf = &tp->t_tbuf;
		if (tbuf->c_ptr == NULL || tbuf->c_count == 0)
		{
			if (tbuf->c_ptr)
				tbuf->c_ptr -= tbuf->c_size - tbuf->c_count;
			if (!(CPRES & (*linesw[tp->t_line].l_output)(tp)))
				break;
		}
		tp->t_state |= BUSY;
		XMODE( *state, XIE );
		addr->csreg = *state;
		break;
	}

	case T_SUSPEND:		/* stop output, XOFF has been received */
		tp->t_state |= TTSTOP;
		break;

	case T_BLOCK:		/* send XOFF */
		tp->t_state &= ~TTXON;
		tp->t_state |= TBLOCK|TTXOFF;
		XMODE( *state, XIE );
		addr->csreg = *state;
		break;

	case T_RFLUSH:	/* flush input queue.  if input is blocked, send XON */
		if (!(tp->t_state & TBLOCK))
			break;
		/* fall thru */

	case T_UNBLOCK:		/* send XON */
		tp->t_state &= ~(TTXOFF | TBLOCK);
		tp->t_state |= TTXON;
		XMODE( *state, XIE );
		addr->csreg = *state;
		break;

	case T_BREAK:		/* send break condition for 1/4 second */
		addr->csreg = (*state |= XBRK);
		tp->t_state |= TIMEOUT;
		timeout(ttrstrt, tp, HZ/4);
		break;
	}
}


aciainit()
{
	register struct device *addr, **adev;
	register char *state;
	register int dev;
	register unsigned short i;

	i = 0;
	do			/* delay loop of 100ms (1 bit time) */
		;
	while (--i);

	adev = &acia_dev[0];
	state = &a_state[0];
	for( dev = 0 ; dev < acia_cnt ; dev++ )
	{
		if( dev <= 1)
			addr = acia_addr[console] + dev;
		else
			addr = acia_addr[(dev + 2) >> 2] + ((dev + 2) & 3);
		if(  bprobe(&addr->csreg, AINIT ) ) {
			/* AINIT failed: flag this device MISSING */
			*adev++ = MISSING;
#ifdef DEBUG
			if (DebugACIA)
				printf("aciainit: missing dev %d\n", dev);
#endif
			continue;
		}
		*adev++ = addr;
		*state++ = DEFAULT;
		addr->csreg = DEFAULT;
#ifdef DEBUG
		if (DebugACIA)
			printf("aciainit: initialized dev %d\n", dev);
#endif
	}
}


aciaintr(dev)
unsigned int dev;
{
	register char *state;
	register struct tty *tp;
	register struct device *addr;
	register int num;

	if (dev <= 1)
	{			/* Two ports on debug board */
		num = 1;
		addr = acia_dev[console];
		tp = &acia_tty[console];
		state = &a_state[console];
	}
	else
	{			/* Four ports on board need servicing */
		num = (((dev + 2) >> 2) << 2) - 2;
		addr = acia_dev[num];
		tp = &acia_tty[num];
		state = &a_state[num];
		num = 3;
	}

	for ( ; num >= 0; --num, tp++, state++, addr++)
	{
		register unsigned char c, s;

		s = addr->csreg;
		if (s & NOCARR)
		{	/* no-carrier processing */
			if (tp->t_state & CARR_ON)
			{
				if (tp->t_state & ISOPEN)
				{
					signal(tp->t_pgrp, SIGHUP);
					ttyflush(tp, (FREAD|FWRITE));
				}
				tp->t_state &= ~CARR_ON;
			}
			c = addr->dreg;		/* read the data register to clear the IRQ	*/
								/* the character may be thrown away.		*/
			continue;
		}
		if ((tp->t_state & CARR_ON) == 0)
		{
			wakeup(&tp->t_canq);
			tp->t_state |= CARR_ON;
		}
		if (s & RCVDONE)
		{	/* receive processing */
			c = addr->dreg;

			if (tp->t_iflag&IXON)
			{	/* XON/XOFF processing */
				register unsigned char ctmp;

				ctmp = c & 0177;
				if (tp->t_state&TTSTOP) /*output stopped by  */
				{			/*cntrl-s	     */
					if (ctmp == CSTART || tp->t_iflag&IXANY)
						(*tp->t_proc)(tp, T_RESUME);
				}
				else
				{
					if (ctmp == CSTOP)
						(*tp->t_proc)(tp, T_SUSPEND);
				}
				if (ctmp == CSTART || ctmp == CSTOP)
					goto transmit;
			}
						/* Check for errors */
		    {
			register int flg;
			char lbuf[3];		/* local character buffer */
			register int lcnt;	/* count of chars in lbuf */

			lcnt = 1;
			flg = tp->t_iflag;
			if (s&PE && !(flg&INPCK))/*parity err & no input parity
						   check ?		     */
				s &= ~PE;
			if (s&(FE|PE|OVRN)) 	/*frame error | parity error |
						  overrun error?	     */
			{	/* error processing */
				if (c == 0)
				{	/* break processing */
					if (flg&IGNBRK)
						goto transmit;
					if (flg&BRKINT)/*signal break on input*/
					{
						signal(tp->t_pgrp, SIGINT);
						ttyflush(tp, (FREAD|FWRITE));
						goto transmit;
					}
				}
				else
				{
					if (flg&IGNPAR)
						goto transmit;
				}
				if (flg&PARMRK) 	/*mark parity errors?*/
				{
					lbuf[2] = 0377;
					lbuf[1] = 0;
					lcnt = 3;
					sysinfo.rawch += 2;
				}
				else
					c = 0;
			}
			else
			{	/* normal char processing */
				if (flg&ISTRIP)	       /*strip input to 7-bits*/
					c &= 0177;
				else
				{
					if (c == 0377 && flg&PARMRK)
					{		/*mark parity errors ?*/
						lbuf[1] = 0377;
						lcnt = 2;
					}
				}
			}
			/* stash character in r_buf */
			lbuf[0] = c;
			if (tp->t_rbuf.c_ptr == NULL)
				goto transmit;
			while (lcnt)
			{
				*tp->t_rbuf.c_ptr++ = lbuf[--lcnt];
				if (--tp->t_rbuf.c_count == 0)
				{			/*move characters to */
					aciascan(tp);	/*the raw queue	     */
				}
				else
					acia_work = ACIA_ACT;
			}
			if (acia_work)
			{			/*move characters to */
				aciascan(tp);	/*the raw queue	     */
			}
		    }
		}
	transmit:
		if (*state&XIE && s&XDONE)	/*interrupt enabled & transmit*/
						/*data register is empty ?    */
		{	/* transmit processing */
			if (tp->t_state & TTXON) 	/*start transmission?*/
			{
				addr->dreg = CSTART;
				tp->t_state &= ~TTXON;
			}
			else if (tp->t_state & TTXOFF) 	/*stop transmission ?*/
			{
				addr->dreg = CSTOP;
				tp->t_state &= ~TTXOFF;
			}
			else if (tp->t_state & BUSY)
			{
				if (tp->t_tbuf.c_count)
				{
					addr->dreg = *tp->t_tbuf.c_ptr++;
					tp->t_tbuf.c_count--;
				}
				tp->t_state &= ~BUSY;
				aciaproc(tp, T_OUTPUT);
			}
			else
				addr->csreg = *state &= XID;
		}
	}
}


/* causes input characters to be moved from raw buffer to the raw input queue*/

/* This scanner is invoked on every interrupt for this device.
 * It may behoove us to inplement a timer driven scanner, or some
 * other event driven scanner, in the future.
 */

aciascan(tp)

register struct tty 	*tp;
{
	tp->t_rbuf.c_ptr -= tp->t_rbuf.c_size - tp->t_rbuf.c_count;
	(*linesw[tp->t_line].l_input)(tp);
	acia_work = 0;
}


/*
 * putchar() is used by the system to send messages to the console.
 * console must be first device on debug board or communications module.
 */

putchar(c)
register char c;
{
	register struct device *addr;
	register char *state;

	addr = acia_addr[console];		/* console address */
	state = &a_state[console];
	addr->csreg = *state & XID;		/* disable interrupts */

	for (;;)
	{
		do				/* wait for xmit ready */
			;
		while (!(addr->csreg & XDONE));
		addr->dreg = c;			/* xmit input char */
		if (c != LF)
			break;
		c = CR;				/* send CR if privious c==LF */
	}

	do					/* wait for xmit ready */
		;
	while (!(addr->csreg & XDONE));

	addr->csreg = *state;			/* restore previous state */
}
