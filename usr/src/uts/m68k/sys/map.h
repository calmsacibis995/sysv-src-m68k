/* @(#)map.h	2.3	 */
struct map
{
	short	m_size;
	unsigned short m_addr;
};

extern struct map swapmap[];
extern struct map coremap[];
#ifdef m68k
extern struct map initmap[];
#endif

#define	mapstart(X)	&X[1]
#define	mapwant(X)	X[0].m_addr
#define	mapsize(X)	X[0].m_size
#define	mapdata(X)	{(X)-2, 0} , {0, 0}
#define	mapinit(X, Y)	X[0].m_size = (Y)-2
