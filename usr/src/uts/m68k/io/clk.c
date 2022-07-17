/* @(#)clk.c	2.1.1.1	 */
/*
	This file contains the routines required to 
	manipulate the hardware clock on the EXORmacs.
*/

/* This declaration is used to force the object for this module to be extracted
   from the library before other clock handlers (which have same entry points*/

int	EXORclk;


#include "sys/types.h"

/*
	pcmcr1 - control register #1 
	pcmcr3 - control register #3 
	pcmcr2 - control register #2 
	pcmmsb3 - most sign buf register 
	pcmlch3 - latch #3 
	pcmcnt3 - timer counter #3 
	pcmstat - status register 

*/

#define	clk_addr 0xfee040	/* clock address */
#define	WATCHDOG 0xfe0003	/* watchdog timer address */
#define	pcmcr3	pcmcr1		/* sits at same address */
#define	pcmstat	pcmcr2		/* sits at same address */
#define	pcmcnt3	pcmmsb3		/* sits at same address */
#define	pcmcnt	16665		/* counter initial value (1/60 second) */
#define	INIT	0x42		/* initial state: enable clk, enable ints */
#define	hibyte(x)	((x>>8)&0xff)
#define	lobyte(x)	(x&0xff)

struct pcm
{
	char fill0, pcmcr1;
	char fill2, pcmcr2;
	char fill4[9], pcmmsb3;
	char fill14, pcmlch3;
};

extern time_t time;			/* time of day in seconds */


/* initialize the clock */
clkstart ()
{
	register struct pcm *clk;
	clk = (struct pcm *) clk_addr;

	clk->pcmcr2 = 0;		/* point to cr3 */
	clk->pcmcr3 = INIT;		/* enable clock and interrupt */
	clk->pcmmsb3 = hibyte(pcmcnt);	/* most significant byte */
	clk->pcmlch3 = lobyte(pcmcnt);	/* least significant byte */
	clk->pcmcr2 = 1;		/* point to cr1 */
	clk->pcmcr1 = 1;		/* reset timer and clear any interrupts */
	clk->pcmcr1 = 0;		/* start counting */
}

clkset (oldtime)
time_t	oldtime;
{
	time = oldtime;			/* reset time of day (sec) */
}

clkreld (on)
int on;
{
	register struct pcm *clk;
	register char c;

	clk = (struct pcm *) clk_addr;
	if (on)
	{	/* only clear interrupt */
		c = clk->pcmstat;	/* read status register */
		c = clk->pcmcnt3;	/* read counter register */
	}
	else
	{   /* clear interrupt and stop timer */
		clk->pcmcr2 = 1;    /* point to cr1 */
		clk->pcmcr1 = 1;    /* stop timer and clear interrupt */
	}
	/* bump watchdog timer to keep READY/FAIL indicators happy */
	*(char *)WATCHDOG = 1;
}
