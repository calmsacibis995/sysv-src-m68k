/*	@(#)text.c	2.1		*/
#include	"crash.h"
#include	"sys/text.h"
#include	"sys/inode.h"
#include	"sys/proc.h"

prtext(c, all)
	int	c;
	int	all;
{
	struct	text	tbuf;
	char	x_flag;

	if(c == -1)
		return;
	if(c >= v.v_text) {
		printf("%4d  out of range\n", c);
		return;
	}
	if(readmem(&tbuf, (long)SYM_VALUE(Text) + c * sizeof tbuf,
		sizeof tbuf) != sizeof tbuf) {
			printf("%4d  read error on text table\n", c);
			return;
	}
	if(!all && tbuf.x_iptr == NULL)
		return;
	printf("%4u  %5u  %3u  %5u",
		c, tbuf.x_iptr ?
		((unsigned)tbuf.x_iptr-Inode->n_value)/sizeof(struct inode):0,
		tbuf.x_count, tbuf.x_ccount);
#if (defined(vax) || defined(m68k))
	printf("  %4u",
		tbuf.x_caddr ?
		((unsigned)tbuf.x_caddr-Proc->n_value)/sizeof(struct proc):0);
#endif
#ifdef	pdp11
	printf("  %7o", tbuf.x_caddr);
#endif
	printf("  %7u   %4u  ",
		tbuf.x_daddr, tbuf.x_size);
	x_flag = tbuf.x_flag;
	printf("%s%s%s%s%s\n",
		x_flag & XTRC ? "exclu-write" : "",
		x_flag & XWRIT ? "write" : "",
		x_flag & XLOAD ? "load" : "",
		x_flag & XLOCK ? "lock" : "",
		x_flag & XWANT ? "want" : "");
}
