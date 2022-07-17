/* @(#)ram.c	2.1	 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/map.h"
#include "sys/ram.h"

extern struct probe probe_tbl[];	/* probe initialization table */
extern struct ram ram_tbl[];	/* ram configuration table */
extern unsigned ram_nbr;	/* number of entries in ram_tbl[] */
extern caddr_t locore;		/* location of proc[0]'s u area */
extern long physmem;		/* top of memory in clicks */
extern caddr_t bsmear();	/* smears a block of ram */
extern int bprobe();		/* probes a ram location*/

/* raminit() -	initializes memory according to ram_tbl[],
 *		sets physmem,
 *		saves initial coremap in initmap,
 *		initializes troublesome memory by probing it.
 *
 * locate each ram board comprising memory and call mfree() to add it to the
 * coremap.  care must be taken so that an area of memory isn't freed twice,
 * and that the area below locore + ctob(USIZE) isn't touched.
 *
 * run down the probe_table and initialize any specified memory address
 * that needs probing.
 *
 * ram_tbl[] and probe_tbl are built by config(1M).
 */

raminit()
{
    {	/* locate and initialize ram */

	register unsigned short top = btoct(locore) + USIZE;
	register unsigned short low, nxt, siz;
	register struct ram *ram = ram_tbl;
	register unsigned nbr = ram_nbr;

	for( ; nbr ; nbr--, ram++ )
	{
		nxt = ram->ram_lo;
		while( low = nxt, nxt += ram->ram_sz, nxt <= ram->ram_hi )
		{
			if( nxt < top )
				continue;
			if( low < top )
				low = top;
			if( ram->ram_flg&RAM_INIT )
			{
				siz = btoct( bsmear(ctob( low ),
						    ctob( ram->ram_hi - low )));
				if( ( siz -= low ) == 0 )
					continue;
			}
			else 
			{
				siz = nxt - low;
				if( ram->ram_flg&RAM_HUNT )
				{
					if( bprobe( ctob( low ), -1 ) )
						continue;
				}
			}
			mfree( coremap, siz, low );
			top = low + siz;
		}
	}
        physmem = top;	/* set physmem */
    }


    {	/* save coremap in initmap */

	register struct map *cp = &coremap[1];
	register struct map *ip = &initmap[1];

	while( cp->m_size )
		*ip++ = *cp++;
    }
    {	/* probe any required bytes */

	register struct probe *probe = probe_tbl;

	for( ; probe->probe_addr ; probe++ )
		(void) bprobe( probe->probe_addr, probe->probe_byte );
    }
}
