/* @(#)reg.h	2.6	 */
/*
 * Location of the users' stored registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
#define	R8	8
#define	R9	9
#define	R10	10
#define	R11	11

#ifdef	m68k

#define	R12	12
#define	R13	13
#define	R14	14
#if	M68010 || M68020
#define	R15	(-1)
#else
#define	R15	(-2)
#endif

#define	FP	R14
#define	SP	R15

#if	M68010 || M68020
#define	PS	15
#define	PC	16
#else
#define	PS	16
#define	PC	17
#endif

#else

#define	R12	(-3)
#define	R13	(-2)

#define	AP	R12
#define	FP	R13
#define	SP	13
#define	PS	17
#define	PC	16

#endif
