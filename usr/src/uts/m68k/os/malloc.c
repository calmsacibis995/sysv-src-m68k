/* @(#)malloc.c	2.2	 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/map.h"
#ifndef m68k
#include "sys/page.h"
#include "sys/mem.h"
#endif

/*
 * Allocate 'size' units from the given map.
 * Return the base of the allocated space.
 * In a map, the addresses are increasing and the
 * list is terminated by a 0 size.
 * The swap map unit is 512 bytes.
 * Algorithm is first-fit.
 */
malloc(mp, size)
struct map *mp;
{
	register unsigned int a;
	register struct map *bp;

	for (bp = mapstart(mp); bp->m_size; bp++) {
		if (bp->m_size >= size) {
			a = bp->m_addr;
			bp->m_addr += size;
			if ((bp->m_size -= size) == 0) {
				do {
					bp++;
					(bp-1)->m_addr = bp->m_addr;
				} while ((bp-1)->m_size = bp->m_size);
				mapsize(mp)++;
			}
			return(a);
		}
	}
	return(0);
}

/*
 * Free the previously allocated space aa
 * of size units into the specified map.
 * Sort aa into map and combine on
 * one or both ends if possible.
 */
mfree(mp, size, a)
struct map *mp;
register unsigned int a;
{
	register struct map *bp;
	register unsigned int t;

	bp = mapstart(mp);
	for (; bp->m_addr<=a && bp->m_size!=0; bp++);
	if (bp>mapstart(mp) && (bp-1)->m_addr+(bp-1)->m_size == a) {
		(bp-1)->m_size += size;
		if (a+size == bp->m_addr) {
			(bp-1)->m_size += bp->m_size;
			while (bp->m_size) {
				bp++;
				(bp-1)->m_addr = bp->m_addr;
				(bp-1)->m_size = bp->m_size;
			}
			mapsize(mp)++;
		}
	} else {
		if (a+size == bp->m_addr && bp->m_size) {
			bp->m_addr -= size;
			bp->m_size += size;
		} else if (size) {
			if (mapsize(mp) == 0) {
				printf("\nDANGER: mfree map overflow %x\n", mp);
				printf("  lost %d items at %d\n", size, a);
				return;
			}
			do {
				t = bp->m_addr;
				bp->m_addr = a;
				a = t;
				t = bp->m_size;
				bp->m_size = size;
				bp++;
			} while (size = t);
			mapsize(mp)--;
		}
	}
	if (mapwant(mp)) {
		mapwant(mp) = 0;
		wakeup((caddr_t)mp);
	}
}
/*
******************************************************
	The following code is for the VAX only
******************************************************
*/

#ifndef m68k
/*
 * allocate memory
 * Note: the memory map is stored in the array
 * `memvad', one bit per page frame.  If the
 * i'th bit of memvad is 0, then page frame i
 * is free (available), if 1, then it is allocated.
 * Up to NICMEM free page frame numbers are
 * stored in the array mem.m_pnum.
 */
extern int	*memvad;

memall(base, size)
register int *base;
{
	register int i, pos, ndx, mask;

	if (size <= 0 || size > mem.m_avail)
		return(0);
	if (mem.m_free < 0)
		panic("bad mem free-list");
	for(i=size; --i>=0; ) {
		if (mem.m_free == 0) {
			pos = mem.m_ptr + 1;
			if (pos >= mem.m_hi)
				pos = mem.m_lo;
			while(mem.m_free<NICMEM) {
				if ((memvad[pos>>5]&masktab[pos&0x1f])==0)
					mem.m_pnum[mem.m_free++] = pos;
				if (pos == mem.m_ptr)
					break;
				if (++pos >= mem.m_hi)
					pos = mem.m_lo;
			}
			if (mem.m_free <= 0)
				panic("lost mem");
			mem.m_ptr = pos;
		}
		pos = mem.m_pnum[--mem.m_free];
		mask = masktab[pos&0x1f];
		ndx = pos >> 5;
		if (memvad[ndx]&mask)
			panic("dup alloc");
		memvad[ndx] |= mask;
		*base++ = pos;
		mem.m_avail--;
	}
	return(size);
}


/*
 * Free memory
 */
memfree(base, size)
register int *base, size;
{
	register int pos, ndx, mask;

	while(--size>=0) {
		if (*base == 0) {
			printf("memfree %x\n",base);
			base++;
			continue;
		}
		pos = *base++ &= PG_PFNUM;
		if (pos<mem.m_lo || pos>=mem.m_hi) 
			panic("bad mem free");
		ndx = pos >> 5;
		mask = masktab[pos & 0x1f];
		if ((memvad[ndx]&mask)==0)
			panic("dup free");
		memvad[ndx] &= ~mask;
		if (mem.m_free < NICMEM && mem.m_free >= 0)
			mem.m_pnum[mem.m_free++] = pos;
		mem.m_avail++;
	}
}

/*
 * Initialize memory map
 */
meminit(first, last)
{
	mem.m_lo = first;
	mem.m_hi = mem.m_ptr = last;
	mem.m_avail = last - first;
	mem.m_free = 0;
	maxmem = mem.m_avail;
}
#endif
