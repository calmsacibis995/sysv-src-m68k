/*	@(#)callout.c	2.1		*/
#include	"crash.h"
#include	"sys/callo.h"
#include	"sys/map.h"

#define	NMAP	50
#define	NCALL	50

prcallout()
{
	register  int  i;
	register  struct  nlist  *sp;
	struct  callo  callout[NCALL];

	lseek(mem, (long)SYM_VALUE(Callout), 0);
	for(;;) {
		if(read(mem, callout, sizeof callout) != sizeof callout) {
			error("read error on callout structure");
			return;
		}
		for(i = 0; i < NCALL; i++) {
			if(callout[i].c_func == NULL)
				return;
			sp = (struct nlist *)search(callout[i].c_func,
#ifdef pdp11
				(N_TEXT | N_EXT), (N_TEXT | N_EXT));
#else
				N_TEXT, N_TEXT);
#endif
			if(sp == NULL)
				printf("no match\n");
			else {
#ifdef m68k
				printf("%-8.8s  ", &sp->n_name[1]);
#else
				(sp->n_name[0] == '_') ?
					printf("%-7.7s  ", &sp->n_name[1]) :
					printf("%-8.8s  ", &sp->n_name[1]);
#endif

#ifdef	pdp11
				printf("%8.6o", callout[i].c_arg);
#else
				printf("%8.8x", callout[i].c_arg);
#endif
				printf(" %5u\n",callout[i].c_time);
			}
		}
	}
}

prmap(s)
	register  char  *s;
{
	struct	map	mbuf[NMAP];
	register  int  i;
	unsigned  free = 0, seg = 0;
	register  struct  nlist  *sp;

	printf("%s\n", s);
	if((sp = (struct nlist *)nmsrch(s)) == NULL) {
		printf("symbol not found\n");
		return;
	}
	printf("address  size\n");
	lseek(mem, SYM_VALUE(sp), 0);
	for(;;) {
		if(read(mem, mbuf, sizeof mbuf) != sizeof mbuf) {
			error("read error on map structure");
			return;
		}
		for(i = 0; i < NMAP; i++) {
			if(mbuf[i].m_size == 0) {
				printf("%u segments, %u units\n", seg, free);
				return;
			}
			printf(FMT, mbuf[i].m_addr);
			printf(" %5u\n", mbuf[i].m_size);
			free += mbuf[i].m_size;
			seg++;
		}
	}
}
