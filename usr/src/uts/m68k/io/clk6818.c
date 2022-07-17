/*
******************************************************
	@(#)clk6818.c	2.1	
	This file contains the routines required to 
	manipulate the Motorla M146818 hardware clock.
******************************************************
*/

/* This declaration is used to force the object for this module to be extracted
   from the library before other clock handlers (which have same entry points*/

int	MC146818;

#include "sys/types.h"

	/* defines used in conjunction with the clock registers		     */

#define  CLK_ADDR 	0xf1a080	/* clock address 		     */
#define  C_BSET		0x80		/* enable set flag in register B     */
#define  C_INTRATE	0x0a		/* set the periodic interrupt rate   */
#define  C_INIT		0x46		/* initial control values in reg. C;
					   periodic interrupt enable, binary
					   data, 24 hour clock		     */
	/*defines used in conjunction with VME/10 Control Register 0	     */

#define CR0_ADDR	0xf19f05	/*address of Control Register 0	     */
#define CR0_TIMER_ON	0x02		/*used to set bit 1 in CR0	     */

	/* memory map of the VME/10 Clock Registers			     */

struct clk_map
   {
   char fill0,  c_secs;			/* seconds elapsed in current minute */
   char fill2,  c_sec_alarm;		/* when to generate alarm interrupt  */
   char fill4,  c_mins;			/* minutes elapsed in current hour   */
   char fill6,  c_min_alarm;		/* when to generate alarm interrupt  */
   char fill8,  c_hrs;			/* hours elapsed in current day	     */
   char fill10, c_hrs_alarm;		/* when to generate alarm interrupt  */
   char fill12, c_day_wk;		/* day of week (1-7); sunday=1       */
   char fill14, c_day_mon;		/* day of month (1-31);    0xf1a08e  */
   char fill16, c_mon;			/* month of year (1-12)	   0xf1a090  */
   char fill18, c_yr;			/* year of century (0-99)  0xf1a092  */
   char fill20, c_a;			/* register A		   0xf1a094  */
   char fill22, c_b;			/* register B		   0xf1a096  */
   char fill24, c_c;			/* register C		   0xf1a098  */
   char fill26, c_d;			/* register D		   0xf1a09a  */
   };

extern time_t time;			/* time of day in seconds */

   /* initialize the clock; called from main.c when system is booted        */

clkstart ()

   {
   register struct clk_map *clk;
   register char	scratch;
   register char	*cr0_ptr;	/*holds address of CR0		     */

   cr0_ptr = (char *)CR0_ADDR;
   scratch = *cr0_ptr;			/*retrieve contents of CR0	     */
   scratch |= CR0_TIMER_ON;		/*enable timer in CR0 only	     */
   *cr0_ptr = scratch;
   clk = (struct clk_map *) CLK_ADDR;
   clk->c_b |= C_BSET;			/*inhibit update so can set clock    */
   clk->c_a  = C_INTRATE;		/*set periodic interrupt rate	     */
   scratch = clk->c_c;			/*reset interrupt flag		     */
   clk->c_b  = C_INIT;			/*initialize and start timer	     */
   return;
   }

   /* clkset merely replaces the value in the global variable time. In the 
      future it will read the time from the M146818 (which has a battery   
      backup to maintain the correct time when the system is down).	     */

clkset (oldtime)

   time_t	oldtime;

   {
   register short	scratch;

   time = oldtime;			/* reset time of day (sec) 	    */
   return;
   }

   /*for the M146818 timer there is no need to reload the timer after every
      interrupt.  clkreld merely clears the interrupt flags in register C   */

clkreld (on)

   int on;

   {
   register struct clk_map *clk;
   register short	scratch;

   clk = (struct clk_map *) CLK_ADDR;
   scratch = clk->c_c;			/*clear interrupt flag 		     */
   return;
   }

