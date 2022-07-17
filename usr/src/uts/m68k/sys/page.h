/*	@(#)page.h	2.1	 */
/*
 * VAX page table entry
 */

struct pt_entry {
	unsigned pg_pfnum:21,	/* Page frame number */
		pg_dec2:2,	/* Reserved for DEC */
		pg_type:2,	/* Type of page (user definable) */
		pg_dec1:1,	/* Reserved for DEC */
		pg_m:1,		/* Modified bit */
		pg_prot:4,	/* Protection */
		pg_v:1;		/* Valid */
};

#define	PG_PFNUM	0x1fffff
#define	PG_M	0x04000000
#define	PG_PROT	0x78000000
#define	PG_V	0x80000000
#define	PG_TYPE	0x01800000

#define	PG_NOACC	0
#define	PG_KR	0x18000000
#define	PG_KW	0x10000000
#define	PG_UW	0x20000000
#define	PG_URKW	0x60000000
#define	PG_URKR	0x78000000

/*	Definition of a virtual address.	*/
struct vaddress {
	unsigned v_byte:9,	/* Byte within the page */
		v_vpn:21,	/* Virtual page number */
		v_region:2;	/* Region of memory(system,user,data,stack) */
};

/*	Definition of a physical address.	*/
struct paddress {
	unsigned p_byte:9,	/* Byte within the page */
		p_pfn:21,	/* Page frame number */
		p_mbz:2;	/* Must be zero */
};
#define	SYSVA	0x80000000
#define	svtoc(X)	(((int)(X) >> 9) & PG_PFNUM)
#define	ctosv(X)	(((int)(X) << 9) + SYSVA)
#define	ctopv(X)	((int)(X) << 9)
#define	svtopte(X)	(sbrpte + svtoc(X))
extern int * sbrpte;
