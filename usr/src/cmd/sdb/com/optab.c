	/*	@(#)optab.c	2.1		*/

#include "mode.h"
#include "machdep.h"

/*
 *  	Instruction printing data.
 *		MACHINE DEPENDENT
 *
 */

/* The registers in regname must be in the correct order, because the numeric
 * value of the register is used (in dis/bits.c) to index into this table.
 * The null final entry is a sentinel, used in prassym (opset.c).
 *
 * The registers in reglist must be in correct order, because the
 * number of the register is used to index into this table, to get roffs,
 * which gives the offset in the user area from register 0.
 */

#ifdef u3b

STRING	regname[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
			"ap", "fp", "sp", "pc", "psbr", "ssbr", (char *)0};

REGLIST reglist [NUMREGLS] = {
		"r0", R0,
		"r1", R1,
		"r2", R2,
		"r3", R3,
		"r4", R4,
		"r5", R5,
		"r6", R6,
		"r7", R7,
		"r8", R8,
		"fp", FP,
		"ap", AP,
		"sp", SP,
		"pc", PC,
		"psw", PS,
		"psbr", PSBR,
		"ssbr", SSBR,
/*		"u_rval1", VAL1,	not register; removed */
/*		"u_rval2", VAL2,	not register; removed */
};

#endif

#ifdef vax

STRING	regname[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
					"r8", "r9", "r10","r11","ap", "fp", "sp", "pc", (char *)0};

REGLIST reglist [NUMREGLS] = {
		"r0", R0,
		"r1", R1,
		"r2", R2,
		"r3", R3,
		"r4", R4,
		"r5", R5,
		"r6", R6,
		"r7", R7,
		"r8", R8,
		"r9", R9,
		"r10", R10,
		"r11", R11,
		"ap", AP,
		"fp", FP,
		"sp", SP,
		"pc", PC,
		"ps", PS,
};

#endif

#ifdef m68k

STRING	regname[] = { "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
					"a0", "a1", "a2","a3","a4","a5", "fp",
 "sp", "pc", "ps", (char *)0};

REGLIST reglist [NUMREGLS] = {
		"d0", R0,
		"d1", R1,
		"d2", R2,
		"d3", R3,
		"d4", R4,
		"d5", R5,
		"d6", R6,
		"d7", R7,
		"a0", R8,
		"a1", R9,
		"a2", R10,
		"a3", R11,
		"a4", R12,
		"a5", R13,
		"fp", FP,
		"sp", SP,
		"pc", PC,
		"ps", PS,
};

#endif
STRING	fltimm[] = {
"0.5", "0.5625", "0.625", "0.6875", "0.75", "0.8125", "0.875", "0.9375",
"1.0", "1.125", "1.25", "1.375", "1.5", "1.625", "1.75", "1.875",
"2.0", "2.25", "2.5", "2.75", "3.0", "3.25", "3.5", "3.75",
"4.0", "4.5", "5.0", "5.5", "6.0", "6.5", "7.0", "7.5",
"8.0", "9.0", "10.0", "11.0", "12.0", "13.0", "14.0", "15.0",
"16.0", "18.0", "20.0", "22.0", "24.0", "26.0", "28.0", "30.0",
"32.0", "36.0", "40.0", "44.0", "48.0", "52.0", "56.0", "60.0",
"64.0", "72.0", "80.0", "88.0", "96.0", "104.0", "112.0", "120.0"
};

char *fmtr = {"%r"};
char *fmtR = {"%R"};
