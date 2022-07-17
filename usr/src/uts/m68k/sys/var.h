/*	@(#)var.h	2.1	 */
struct var {
	int	v_buf;
	int	v_call;
	int	v_inode;
	char *	ve_inode;
	int	v_file;
	char *	ve_file;
	int	v_mount;
	char *	ve_mount;
	int	v_proc;
	char *	ve_proc;
	int	v_text;
	char *	ve_text;
	int	v_clist;
	int	v_sabuf;
	int	v_maxup;
	int	v_smap;
	int	v_hbuf;
	int	v_hmask;
	int	v_pbuf;
#if m68k
	int	v_cmap;		/* coremap size */
#endif
};
extern struct var v;
