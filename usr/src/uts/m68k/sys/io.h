/*	@(#)io.h	2.1.1.1	 */
#ifdef m68k

struct	size
{
	daddr_t	nblocks;	/* size of slice in blocks */
	daddr_t	blkoff;		/* first physical block in slice */
};

/*
 *	The slicing is defined for:
 *
 *		Where Minor Number[0:2] defines one of 8 slices and
 *		      Minor Number[3:5] defines the following devices:
 *
 *			0: hard device 0 (fixed)
 *			1: hard device 1 (removable)
 *			2: hard device 2 (fixed)
 *			3: hard device 3 (removable)
 *			4: floppy device 4
 *			5: floppy device 5
 *			6: floppy device 6
 *			7: floppy device 7
 *
 *		      Minor Number[6:7] defines one of 4 controllers.
 *			(Only on MEXOR at this time.)
 */

/*
 * 32mb fixed/removable CMD disk
 * 16/16mb nominal 13.5/13.5mb formatted
 * Used to define drives 0 and 1 or drives 2 and 3
 */
#define UDFRC1\
	26336,	0,\
	23044,	3292,\
	19752,	6584,\
	16460,	9876,\
	13168,	13168,\
	9876,	16460,\
	6584,	19752,\
	3292,	23044,\
	26336,	0,\
	23044,	3292,\
	19752,	6584,\
	16460,	9876,\
	13168,	13168,\
	9876,	16460,\
	6584,	19752,\
	3292,	23044,

/*
 * 96mb fixed/removable CMD disk
 * 80/16mb nominal 67.4/13.5mb formatted
 * Used to define drives 0 and 1 or drives 2 and 3
 */
#define UDFRC3\
	26336,	0,\
	105344,	26336,\
	79008,	52672,\
	52672,	79008,\
	39504,	92176,\
	26336,	105344,\
	13168,	118512,\
	131680,	0,\
	26336,	0,\
	23044,	3292,\
	19752,	6584,\
	16460,	9876,\
	13168,	13168,\
	9876,	16460,\
	6584,	19752,\
	3292,	23044,

/*
 * Alternate 96mb fixed/removable CMD disk
 * 80/16mb nominal 67.4/13.5mb formatted
 * Used to define drives 0 and 1 or drives 2 and 3
 */
#define UDFRC4\
	26336,	0,\
	13168,	13168,\
	39504,	26336,\
	39504,	65840,\
	13168,	105344,\
	13168,	118512,\
	26336,	52672,\
	131680,	0,\
	26336,	0,\
	23044,	3292,\
	19752,	6584,\
	16460,	9876,\
	13168,	13168,\
	9876,	16460,\
	6584,	19752,\
	3292,	23044,

/*
 * 50mb fixed/removable LARK disk
 * 25/25mb nominal 20.4/20.4mb formatted
 * Used to define drives 0 and 1 or drives 2 and 3
 */
#define UDFRL1\
	39936,	0,\
	34944,	4992,\
	29952,	9984,\
	24960,	14976,\
	19968,	19968,\
	14976,	24960,\
	9984,	29952,\
	4992,	34944,\
	39936,	0,\
	34944,	4992,\
	29952,	9984,\
	24960,	14976,\
	19968,	19968,\
	14976,	24960,\
	9984,	29952,\
	4992,	34944,

/* space holder in table for unused minor device */
#define NODISK\
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

/* defines partition 0 for single sided and partition 1 for double sided */
#define FLOPPY\
	500,	0,\
	1000,	0,\
	0,0,0,0,0,0,0,0,0,0,0,0,

#ifndef DISK01
#define DISK01 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifndef DISK23
#define DISK23 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifdef	MEXOR
#ifndef DISK89
#define DISK89 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifndef DISK1011
#define DISK1011 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifndef DISK1617
#define DISK1617 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifndef DISK1819
#define DISK1819 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifndef DISK2425
#define DISK2425 UDFRC1		/* Fixed/Removable CMD */
#endif
#ifndef DISK2627
#define DISK2627 UDFRC1		/* Fixed/Removable CMD */
#endif

struct	size ud_sizes[CTLS][8][8] =
#else
struct	size ud_sizes[8][8] =
#endif
{
	DISK01			/* Fixed/Removable CMD */
	DISK23			/* Fixed/Removable CMD */
	FLOPPY
	FLOPPY
	FLOPPY
	FLOPPY
#ifdef	MEXOR
#if CTLS>1			/* more than one controller */
	DISK89			/* Fixed/Removable CMD */
	DISK1011		/* Fixed/Removable CMD */
	FLOPPY
	FLOPPY
	FLOPPY
	FLOPPY
#if CTLS>2			/* more than two controllers */
	DISK1617		/* Fixed/Removable CMD */
	DISK1819		/* Fixed/Removable CMD */
	FLOPPY
	FLOPPY
	FLOPPY
	FLOPPY
#if CTLS>3			/* more than three controllers */
	DISK2425		/* Fixed/Removable CMD */
	DISK2627		/* Fixed/Removable CMD */
	FLOPPY
	FLOPPY
	FLOPPY
	FLOPPY
#endif
#endif
#endif
#endif
};

#else

struct size {
	daddr_t nblocks;
	int	cyloff;
};

#ifdef RP03_0
struct size rp_sizes[8] = {
	10000,	0,		/* cyl 0 thru 49 */
	71200,	50,		/* cyl 50 thru 405 */
	40600,	203,		/* cyl 203 thru 405 */
	0,	0,
	0,	0,
	0,	0,
	0,	0,
	81200,	0,		/* cyl 0 thru 405 */
};
#endif
#ifdef RP05_0
#define RP04_0
#endif
#ifdef RP04_0
struct size hp_sizes[8] = {
	18392,	0,		/* cyl 0 thru 43 */
	153406,	44,		/* cyl 44 thru 410 */
	87780,	201,		/* cyl 201 thru 410 */
	22154,	358,		/* cyl 358 thru 410 */
	0,	0,
	0,	0,
	0,	0,
	171798,	0,		/* cyl 0 thru 410 */
};
#endif
#ifdef RP06_0
struct size hp_sizes[8] = {
	18392,	0,		/* cyl 0 thru 43 */
	322278,	44,		/* cyl 44 thru 814 */
	256652,	201,		/* cyl 201 thru 814 */
	191026,	358,		/* cyl 358 thru 814 */
	125400,	515,		/* cyl 515 thru 814 */
	59774,	672,		/* cyl 672 thru 814 */
	0,	0,
	340670,	0,		/* cyl 0 thru 814 */
};
#endif
#ifdef RM80_0
struct size he_sizes[8] = {
	18228,	0,		/* cyl 0 thru 41 */
	224378,	42,		/* cyl 42 thru 558 */
	158410,	194,		/* cyl 194 thru 558 */
	92442,	346,		/* cyl 346 thru 558 */
	26474,	498,		/* cyl 498 thru 558 */
	0,	0,
	0,	0,
	242606,	0,		/* cyl 0 thru 558 */
};
#endif
#ifdef RM05_0
struct size hm_sizes[8] = {
	24320,	0,		/* cyl 0 thru 39 */
	476064,	40,		/* cyl 40 thru 822 */
	403104,	160,		/* cyl 160 thru 822 */
	330144,	280,		/* cyl 280 thru 822 */
	257184,	400,		/* cyl 400 thru 822 */
	184224,	520,		/* cyl 520 thru 822 */
	111264,	640,		/* cyl 640 thru 822 */
	500384,	0,		/* cyl 0 thru 822 */
};
#endif
#ifdef RP07_0
struct size hP_sizes[8] = {
	64000,	0,		/* cyl 0 thru 39 */
	944000,	40,		/* cyl 40 thru 630 */
	840000,	105,		/* cyl 105 thru 630 */
	672000,	210,		/* cyl 210 thru 630 */
	504000,	315,		/* cyl 315 thru 630 */
	336000,	420,		/* cyl 420 thru 630 */
	168000,	525,		/* cyl 525 thru 630 */
	1008000,	0,		/* cyl 0 thru 630 */
};
#endif
#endif
