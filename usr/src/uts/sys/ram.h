/* @(#)ram.h	2.1	 */

/* ram configuration structure */

struct ram {
	unsigned short	ram_lo;	/* low ram range */
	unsigned short	ram_hi;	/* high ram range ( + 1 ) */
	unsigned short	ram_sz;	/* ram size */
	unsigned short	ram_flg;
};

/* ram_flg values */

#define RAM_INIT 1	/* ram must be written to be reset */
#define RAM_HUNT 2	/* ram boards may or may not be located within range */

/* initial probe structure */

struct probe {
	char	*probe_addr;	/* address to be probed */
	short	probe_byte;	/* byte to write into probe_addr */
};

/* (probe_addr is only read from if read if probe_byte < 0) */
