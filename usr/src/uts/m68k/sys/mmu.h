/*
	@(#)mmu.h	2.26 
	This is the header file for the UNIX V/68 generic
	MMU interface. This provides the information that
	is used by the various routines that call:

	mmufork ()
	mmuexec ()
	mmuexit ()
	mmuread ()
	mmuwrite ()
	mmuattach ()
	mmudetach ()
	mmuregs ()
	mmualloc ()
	mmuinit ()
	mmuint ()

	The above routines and secondary routines called
	by them are contained in io/mmu1.c and io/mmu2.c.
*/


/*
	This is the description of the mmu registers
*/

#ifdef MEXOR
struct	mmu_regs
	{
	ushort	mr_begin;	/* Virtual address start/256	*/
	ushort	mr_end;		/* Virtual address end/256	*/
	ushort	mr_offset;	/* Physical address/256		*/
	ushort	mr_ctrl;	/* control register		*/
	};

#define	MMUREGS		((struct mmu_regs *)0xfe2000)
#define	ENDREG(x)	((ushort)((((unsigned)(x))-1)>>8))
#define	LDREG(x)	((ushort)(((unsigned)(x))>>8))

#endif

#ifdef M68451
struct mmu_regs
	{
	ushort	mr_lba,		/* Logical Base Address		*/
		mr_lam,		/* Logical Address Mask		*/
		mr_pba;		/* Physical Base Address	*/
	char	mr_asn,		/* Address Space Number		*/
		mr_ssr,		/* Segment Status Register	*/
		mr_asm,		/* Address Space Mask		*/
		mr_ptr;		/* Descriptor number		*/
	};


#define addrspace	((char *)0xf1a800)  /* Address Space Table       */
#define descwrite	((char *)0xf1a83f)  /* Write to here loads desc  */
#define ssrclr		((char *)0xf1a831)  /* SSR: write to here resets */
#define descread	((char *)0xf1a831)  /* SSR: read from here dumps */
#define descmap		((char *)0xf1a83d)  /* Read from here translates */

#define SYSTEM_DATA	10
#define SYSTEM_PROG	12
#define USER_DATA	2
#define USER_PROG	4
#define	MMUREGS		((struct mmu_regs *)0xf1a820)
#define MMUTOTSEGS	32
#endif

/*
These defines are used by the MMU status register
*/

#ifdef MEXOR
#define MMU_ENBL	1
#define MMU_PTCT	2
#define MMUSEGS		4	/* Maximum physical segments */
#endif
#ifdef M68451
#define MMU_ENBL	1
#define MMU_PTCT	2
#define MMUSEGS		4	/* Maximum physical segments */
#endif

		/* This should be changed for multiple shared memory */



/*
	This is the description of the internal MMU table
	for the EXORmacs MMU. A different one is expected
	for the 68451
	(for the present this one will do)
*/


struct mmu_entry	/* Per process entry */
	{
#ifdef MEXOR
	ushort	mmu_status;	/* Flags for this physical segment */
	ushort	mmu_end;	/* MMU mr_end register precomputed */
#endif
#ifdef M68451
	int	mmu_status;	/* Flags for this physical segment */
#endif
	caddr_t mmu_paddr;	/* Physical start address of segment */
	caddr_t mmu_vaddr;	/* Virtual start address of segment */
	long	mmu_size;	/* Segment size in bytes */
	};


struct	mmu_table	/* Internal MMU table */
	{
	struct proc *mmu_proc;	/* Pointer to related proc table entry */
	struct mmu_entry mmu_entry[MMUSEGS];	/* physical segments */
	};


extern struct mmu_table mmu_table[];	/* The internal table itself */



/*
The following define the status bits in mmu_status
*/

#define SEG_VALID	MMU_ENBL	/* Segment entry allocated */
#define SEG_PROTECT	MMU_PTCT	/* Segment is write protected */
#define SEG_SWAPPED	4		/* Segment is swapped */




/*
	Other MMU related defines
*/


#define MD_NALLOC	0	/* do not allocate memory for swapin */
#define MD_ALLOC	1	/* allocate memory before swapin */


/*
	The following causes a type cast
*/
#define	ptom(pm) ((struct mmu_table *)((struct proc *)pm)->p_mmuent)
