	/*	@(#)runpcs.c	2.1		*/

/*
 *	UNIX debugger
 */

#include "head.h"

#ifdef u3b

/* breakpoint instructions */
#define BPT1	0xDA000000	/* for 3B */
#define BPT2	0x0000DA00	/*	instruction may begin on half word */

#define MASK1	0xFFFF0000	/*	but ptrace works on word bounds */
#define MASK2	0x0000FFFF

#define exect	execve		/*  no trace bit on 3B ** !! */

#endif 
#ifdef vax

/* breakpoint instruction */
#define BPT	03

#endif

#ifdef m68k

/* breakpoint instruction */
#define BPT	0x4e410000	/* TRAP 1 instruction */
#define MASK	0x0000FFFF      /* trap instruction must be in hiword of long */

#define exect	execve		/* exect non-standard system call */

#ifdef VARU
extern ADDR	ubase;		/* ubase calculated when no memory mgt */
#endif

#endif

extern MSG		NOFORK;
extern MSG		ENDPCS;
extern MSG		BADWAIT;

extern ADDR		sigint;
extern ADDR		sigqit;

/* breakpoints */
extern BKPTR		bkpthead;

extern CHAR		lastc;
extern int		errno;
extern INT		pid;
extern L_INT		loopcnt;

extern ADDR	userpc;



getsig(sig)
{	return(sig);
}

/* runpcs - control the user process, running it in single step or
 *	continuous mode, depending on runmode, and with signal execsig.
 *
 *	It has the side effect of deleting all breakpoints (calls delbp()).
 */
runpcs(runmode,execsig)
{
	REG BKPTR	bkpt;
	int cmdval;

	/* for 'g' (goto) command, set userpc to address to which to jump */
	if (adrflg) userpc = dot;

	/* execute breakpoints (or single step) loopcnt number of times */
	while (loopcnt-- > 0) {
#if DEBUG > 1
		if(mjmdflag)
			fprintf(FPRT2, "runpcs(runmode=%d, execsig=%d);\n",
				runmode, execsig);
#endif
#if DEBUG
		if (debug) fprintf(FPRT2, "\tcontinue %#x %d\n",userpc,execsig);
#endif
		errno = 0;
		if (runmode == SINGLE) {
#ifndef u3b
			/* hardware handles single-stepping */
			delbp();
			ptrace(SINGLE, pid, userpc, execsig);
			bpwait(); chkerr(); execsig=0; readregs();
#else
			/*  no single-step on 3B */
			execbkpt((BKPTR)0,execsig);	/* fake tracing */
#endif

			continue;
		}

		else {		/* continuing from a breakpoint is hard */
#if DEBUG > 1
			if(mjmdflag) fprintf(FPRT2,
			     " : userpc=%#x, dot=%#x; adrflg=%d; excuting=%d\n",
					userpc, dot, adrflg, executing);
#endif

			/* if truly continuing from bkpt, execute instruction
			 * that should be where breakpoint was set	    */
			if   ((bkpt=scanbkpt(userpc))
			  && !(adrflg && args[0]=='\0') /*  'g' cmd */
/* ?? 0 bkpt */		  && !(userpc==0 && (cmd=='r'||cmd=='R')))  
			{
				execbkpt(bkpt, execsig);
				execsig = 0;
			}
			setbp();
			ptrace(runmode, pid, userpc, execsig);
			bpwait(); chkerr(); execsig=0; delbp(); readregs();
		}

		/* stopped by BPT instruction */
		if ((signo==0) && (bkpt=scanbkpt(userpc))) {
	loop1:

#if DEBUG
			if (debug) fprintf(FPRT2, "\n BPT code; '%s'%#x'%#x'%d",
				bkpt->comm,bkpt->comm[0],'\n',bkpt->flag);
#endif
			dot=bkpt->loc;
			if (bkpt->comm[0] != '\n') {
				cmdval = acommand(bkpt->comm);
				/* if not "really" breakpoint, i.e.
				 * user did not give k command, then
				 * cmdval == 1, and should execute
				 * breakpoint instruction, not count
				 * as a breakpoint in the loop, and
				 * continue to the next breakpoint
				 */
				if (cmdval) {
					execbkpt(bkpt,execsig);
					execsig=0;
					/* if at another breakpoint,
					 * handle as before (check for
					 * user command, by going to
					 * loop1).
					 */
					if ((signo == 0) &&
					    (bkpt=scanbkpt(userpc)))
						goto loop1;
					loopcnt++;  /* Don't count iteration */
					continue;
				}
			}
		}

		/* else not stopped by a BPT instruction */
		else {
			execsig=signo;
			if (execsig) break;
		}
	}	/* end of while loop */
#if DEBUG
 		if (debug) fprintf(FPRT2, "Returning from runpcs\n");
#endif
}

#define BPOUT 0
#define BPIN 1
INT bpstate = BPOUT;

/* endpcs() - kill child (user) process */
endpcs()
{
	REG BKPTR	bkptr;
#if DEBUG
 		if (debug) fprintf(FPRT2, "Entering endpcs with pid=%d\n", pid);
#endif
	if (pid) {
		errno = 0;
   		ptrace(EXIT,pid,0,0); pid=0;
		chkerr();

		/*  cleanup of breakpoint states not necessary.
		*	Only two modes being used are BKPTSET and 0.
		*/
/*		for (bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt) {
			if (bkptr->flag) bkptr->flag = BKPTSET;
		}
*/
	}
	bpstate=BPOUT;
}

/* setup() -initializes ptrace, forks the child (user) process, and
 *		waits for child.
 */
setup()
{
	close(fsym); fsym = -1;
	if ((pid = fork()) == 0) {
		ptrace(SETTRC,0,0,0);

		/* restore user interrupt handling mode */
		signal(SIGINT,sigint); signal(SIGQUIT,sigqit);
#if DEBUG
 		if (debug) fprintf(FPRT2, "About to doexec  pid=%d\n",pid);
#endif
		doexec(); exit(0);
	}
	else if (pid == -1) {
		error(NOFORK);
		longjmp(env);
	}
	else {
		bpwait();

		/* read uu->u_ar0 */
		if (-1 == (int) (((struct user *)uu)->u_ar0 =
			(int *) ptrace(RUREGS, pid,
					(char *)&(((struct user *)uu)->u_ar0) -
					  (char *)uu,
					0)))
		{
			perror("Cannot ptrace child process; try again.");
			endpcs();
			longjmp(env);
		}

		/* convert absolute address in user area to sdb internal
		 * address by subtracting the address of the user area,
		 * and adding the address of sdb's copy of that area.
		 */
		(((struct user *)uu)->u_ar0 =
			(int *)((char *)(((struct user *)uu)->u_ar0)
				 - ADDR_U + (int)uu));
		readregs();
#if DEBUG > 1
		if(mjmdflag)
			fprintf(FPRT2, "setup(): userpc=%#x;\n", userpc);
#endif
#if DEBUG
		if (debug) fprintf(FPRT2, "About to open symfil = %s\n",symfil);
#endif
		fsym=open(symfil,0);
		if (errflg) {
			fprintf(FPRT1, "%s: cannot execute\n",symfil);
#if DEBUG
			 if (debug) fprintf(FPRT2, "%d %s\n", errflg, errflg);
#endif
			endpcs();
		}
	}
	bpstate=BPOUT;
}

/* execbkpt(bkptr, execsig) - execute the one instruction that was replaced
 *		by a preakpoint instruction.  It has the side effect of
 *		removing all breakpoints, by calling delbp().
 *
 *		In the case of the 3B, a NULL bkptr means that single
 *		stepping is requested, and that a breakpoint instruction
 *		has not just been encountered.
 */

#ifndef u3b
static
execbkpt(bkptr, execsig)
BKPTR bkptr;
{
#if DEBUG
	if (debug) fprintf(FPRT2,"exbkpt: count=%d, loc=%#x; pid=%d\n",
			bkptr->count, bkptr->loc, pid);
#endif
#if DEBUG > 1
	if (mjmdflag)
		fprintf(FPRT2,"execbkpt: execsig=%d; userpc=%#x;\n",
			execsig,userpc);
#endif
	ptrace(SINGLE, pid, bkptr->loc, execsig);
	bkptr->flag=BKPTSET;
	bpwait(); chkerr(); readregs();
}

#else
/* modified execbkpt() since no tracing on the 3B-20 !
 *	Added option for bkptr = 0 to fake single stepping.
 *	Because do not have single stepping via tracing
 *	set breakpoint at next physical instruction and/or
 *	the next logical instr (loc "jump'ed" to) if not too difficult.
 *	Note: need single stepping to implement just simple breakpoints !
 */
static
execbkpt(bkptr,execsig)
BKPTR	bkptr;
{
	register int ptv;
	long dis_dot();
	BKPT	bkpt1, bkpt2;
	unsigned key1, key2;
	union word word;
#define opcode word.c[0]
	extern int errlev;		/*  in dis/bits.c */
	extern long dis_adr[];		/*  in dis/bits.c */
	long nextloc;
	long upc = userpc;

#if DEBUG
	if (debug && bkptr)
		fprintf(FPRT2, "exbkpt: count=%d, loc=%#x; pid=%d;\n",
			bkptr->count, bkptr->loc, pid);
#endif
#if DEBUG > 1
	if(mjmdflag)
		fprintf(FPRT2, "execbkpt: execsig=%d; upc=%#x;\n",execsig, upc);
#endif
	errno = 0;
	delbp();
	/* key1 and key2 are the first and second nibbles of the op code */
	word.w = chkget(upc, ISP);
#if DEBUG > 1
	if(mjmdflag)
	   fprintf(FPRT2, "  : word.w=%#x; word.s[0]=%#x;\n", word.w,word.s[0]);
#endif
	if(word.s[0] == (short)0xA100)	/* special 'nop' to align RET instr */
		word.w = chkget(upc+2, ISP);
	key1 = (word.c[0]>>4) & 0xf;
	key2 = word.c[0] & 0xf;
	bkpt1.loc = dis_dot(upc,ISP,'\0');	/* next physical instr */
	bkpt2.flag = 0;
	nextloc = dis_adr[1];
#if DEBUG > 1
	if(mjmdflag)
		fprintf(FPRT2,
			"  : key1=%#x, key2=%#x, nadr=%d, nextloc=%#x(,%#x);\n",
				key1, key2, dis_adr[0], nextloc,dis_adr[2]);
#endif
	if(errlev || key1 == 0x8 || key1 == 0x9 ||
			(key1 == 0x7 && key2 != 0xa && key2 != 0xe)) {
		if(opcode == 0x7B) {	/* RET instruction */
			frame = SDBREG(FP);
			nextloc = get(NEXTCALLPC, DSP);
#if DEBUG > 1
			if(mjmdflag)
				fprintf(FPRT2, "RET: nextloc=%#lx;\n",nextloc);
#endif
		}
		if(errlev || nextloc == -1) {
			if(bkptr)
				fprintf(FPRT1, "Cannot reset breakpoint: ");
			else
				fprintf(FPRT1, "Cannot single-step: ");
			printline();
			fprintf(FPRT1, "\n");
			longjmp(env, 0);
		}
		else if (nextloc != bkpt1.loc) {
			bkpt2.loc = nextloc;	/* next logical instr */
			bkpt2.flag = 1;
			setbp1(&bkpt2);		/* in case branch is taken */
		}
	}
	setbp1(&bkpt1);			/* in case no branch, or not taken */
	ptv = ptrace(CONTIN,pid,userpc,execsig);	/* continue */
	if(errno && ptv == -1)
		fprintf(FPRT1,
			"Bad ptrace(CONTIN): ptv=%d; errno=%d; loc=%#x;\n",
					ptv, errno, bkptr->loc);
	if(bkptr)
		bkptr->flag=BKPTSET;
	bpwait(); chkerr(); readregs();
	if(bkpt2.flag)			/* in case "jump" instruction */
		delbp1(&bkpt2);		/* restore */
	delbp1(&bkpt1);			/* restore */
}
#endif

/* doexec() - performs exec call, after parsing arguments on sdb "run" command
 *		line.
 */
extern STRING environ;

doexec()
{
	char *argl[MAXARG], args[LINSIZ];
	register char c, redchar, *argsp, **arglp, *filnam;

	arglp = argl;
	argsp = args;
	*arglp++ = symfil;
	c = ' ';

	do {
		while (eqany(c, " \t")) {
			c = rdc();	/* get char from extern args[] */
		} 
		if (eqany(c, "<>")) {	/* redirecting I/O argument */
			redchar = c;
			do {
				c = rdc();
			} while (eqany(c, " \t"));
			filnam = argsp;
			do {
				*argsp++ = c;
				c = rdc();
			} while (!eqany(c, " <>\t\n"));
			*argsp++ = '\0';
			if (redchar == '<') {
				close(0);
				if (open(filnam,0) < 0) {
					fprintf(FPRT1,
						"%s: cannot open\n",filnam);
					 exit(0);
				}
			} else {
				close(1);
				if (creat(filnam,0666) < 0) {
					fprintf(FPRT1,
						"%s: cannot create\n",filnam);
					 exit(0);
				}
			}
		} else if (c == '"' || c == '\'') {  /*  quoted argument */
			char sc = c;
			*arglp++ = argsp;
			for (c = rdc(); c != sc; c = rdc()) {
				if (c == '\\')
					c = bsconv(rdc());
				*argsp++ = c;
			}
			c = rdc();
			*argsp++ = '\0';
		} else if (c != '\n') {		/* a regular argument */
			*arglp++ = argsp;
			do {
				*argsp++ = c;
				c = rdc();
			} while(!eqany(c, " <>\t\n"));
			*argsp++ = '\0';
		}
	} while (c != '\n');
	*arglp = (char *) 0;
#if DEBUG
	if (debug) {
		char **dap;
#ifdef vax
		fprintf(FPRT2,
			"About to exect(%s, %d, %d)\n",symfil,argl,environ);
#else
		fprintf(FPRT2,
			"About to execve(%s, %d, %d)\n",symfil,argl,environ);
#endif
		for (dap = argl; *dap; dap++) {
			fprintf(FPRT2, "%s, ", *dap);
		}
	}
#endif
	exect(symfil, argl, environ);

#ifdef vax
	perror("Returned from exect");
#else
	perror("Returned from execve");
#endif
}

/* scanpkpt() - returns a pointer to the entry in the breakpoint list
 *		corresponding to the address given, if that is a
 *		breakpoint address.  Otherwise returns NULL pointer.
 */
BKPTR
scanbkpt(adr)
ADDR adr;
{
	REG BKPTR	bkptr;
	for(bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt) {
		if (bkptr->flag && bkptr->loc==adr) break;
	}
	return(bkptr);
}

/* delbp(). Remove breakpoints [delbp1() puts original instruction back] */

delbp()
{
	REG BKPTR	bkptr;

#if DEBUG > 1
	if(mjmdflag)
		fprintf(FPRT2, "delbp():\n");
#endif
	if (bpstate != BPOUT) {
		for (bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt) {
			if (bkptr->flag) {

				delbp1(bkptr);	/*  separate subr */
			}
		}
		bpstate=BPOUT;
	}
}

/* delbp1(bkptr) - deletes breakpoint given by bkptr */

#ifdef vax
static
delbp1(bkptr)
BKPTR	bkptr;
{
	register long adr;
	register int ptv;
	struct proct *procp;

	adr = bkptr->loc;
	ptv = ptrace(RIUSER, pid, adr, 0);
	if (!(errno && ptv == -1)) {
#if DEBUG > 1
		if (mjmdflag)
			fprintf(FPRT2, "delbp1(): adr=%#x; ptv=%#x;\n",
				adr,ptv);
#endif
		ptv = ptrace(WIUSER, pid, adr,
				(bkptr->ins & 0xff) | (ptv & ~0xff));
		if (!(errno && ptv == -1)) return;
	}
	procp = adrtoprocp(adr);
	if (procp->pname[0] == '_')
		fprintf(FPRT1, "Cannot delete breakpoint:  %.7s:%d @ %d\n",
			(procp->pname)+1, adrtolineno(adr, procp), dot);
	else
		fprintf(FPRT1, "Cannot delete breakpoint:  %.8s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
}
#endif

#ifdef m68k
static
delbp1(bkptr)
BKPTR	bkptr;
{
	register long adr;
	register int ptv;
	struct proct *procp;

	adr = bkptr->loc;
	ptv = ptrace(RIUSER, pid, adr, 0);
	if (!(errno && ptv == -1)) {
#if DEBUG > 1
		if (mjmdflag)
			fprintf(FPRT2, "delbp1(): adr=%#x; ptv=%#x;\n",
				adr,ptv);
#endif
		ptv = ptrace(WIUSER, pid, adr,
				(bkptr->ins & ~MASK) | (ptv & MASK));
		if (!(errno && ptv == -1)) return;
	}
	procp = adrtoprocp(adr);
#ifdef FLEXNAMES
	fprintf(FPRT1, "Cannot delete breakpoint:  %s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
#else
	fprintf(FPRT1, "Cannot delete breakpoint:  %.8s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
#endif
}

#endif
#ifdef u3b
/* separated delbp1() from delbp() so execbkpt() can call  (3B) */
static
delbp1(bkptr)
BKPTR	bkptr;
{
	register long adr, wa, val;
	register int  ptv;
	struct proct *procp;

	adr = bkptr->loc;
	wa = adr & ~(WORDSIZE-1);	/*  word boundary */
	errno = 0;
	ptv = ptrace(RIUSER, pid, wa, 0);
	if( !(ptv == -1 && errno) ) {
		val = ptv;
#if DEBUG > 1
		if(mjmdflag)
			fprintf(FPRT2, "delbp1(): adr=%#x; wa=%#x; val=%#x;\n",
					adr, wa, val);
#endif
		if(adr == wa)
			val = (bkptr->ins & MASK1) | (val&(~MASK1));
		else		/* instr on half word boundary */
			val = (bkptr->ins & MASK2) | (val&(~MASK2));
#if DEBUG > 1
		if(mjmdflag)
			fprintf(FPRT2, "	new val = %#x;\n", val);
#endif
		ptv = ptrace(WIUSER,pid,wa,val);
	}
	if (errno && ptv == -1) {
	     procp = adrtoprocp(adr);
	     fprintf(FPRT1, "Cannot delete breakpoint:  %.8s:%d @ %d\n",
		procp->pname, adrtolineno(adr, procp), dot);
	}
}
#endif


/* setbp() -
 * Insert breakpoints [setbp1() overwrites instruction space with bpt instr] *
 */
setbp()
{
	REG BKPTR	bkptr;

#if DEBUG > 1
	if(mjmdflag)
		fprintf(FPRT2, "setbp():\n");
#endif
	if (bpstate != BPIN) {
		for (bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt) {
			if (bkptr->flag) {
				setbp1(bkptr);	/*  separate subr */
			}
		}
		bpstate=BPIN;
	}
}

/* setpb1(bkptr) - sets breakpoint given by bkptr */

#ifdef vax
static
setbp1(bkptr)
BKPTR	bkptr;
{
	register long adr;
	register int ptv;
	struct proct *procp;
	adr = bkptr->loc;
	errno = 0;
	ptv = ptrace(RIUSER, pid, adr, 0);

	if (!(errno && ptv == -1)) {
		bkptr->ins = ptv;
#if DEBUG > 1
		if (mjmdflag)
			fprintf(FPRT2, "setbp1(): adr=%#x; ins=%#x\n", 
				adr, ptv);
#endif
		ptv = ptrace(WIUSER, pid, adr, BPT | ptv &~0xff);
		if (!(errno && ptv == -1)) return;
	}

	procp = adrtoprocp(adr);
	if (procp->pname[0] == '_')
		fprintf(FPRT1, "Cannot set breakpoint:  %.7s:%d @ %d\n",
			(procp->pname)+1, adrtolineno(adr, procp), dot);
	else
		fprintf(FPRT1, "Cannot set breakpoint:  %.8s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
}

#endif

#ifdef m68k
static
setbp1(bkptr)
BKPTR	bkptr;
{
	register long adr;
	register int ptv;
	struct proct *procp;
	adr = bkptr->loc;
	errno = 0;
	ptv = ptrace(RIUSER, pid, adr, 0);

	if (!(errno && ptv == -1)) {
		bkptr->ins = ptv;
#if DEBUG > 1
		if (mjmdflag)
			fprintf(FPRT2, "setbp1(): adr=%#x; ins=%#x\n", 
				adr, ptv);
#endif
		ptv = ptrace(WIUSER, pid, adr, BPT | ptv & MASK);
		if (!(errno && ptv == -1)) return;
	}

	procp = adrtoprocp(adr);
#ifdef FLEXNAMES
	fprintf(FPRT1, "Cannot set breakpoint:  %s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
#else
	fprintf(FPRT1, "Cannot set breakpoint:  %.8s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
#endif
}

#endif
#ifdef u3b
/* separated setbp1() from setbp() so execbkpt() can call */
static
setbp1(bkptr)
BKPTR	bkptr;
{
	register long adr, wa, val;
	register int  ptv;
	struct proct *procp;

	adr = bkptr->loc;
	wa = adr & ~(WORDSIZE-1);	/*  word boundary */
	errno = 0;
	ptv = ptrace(RIUSER, pid, wa, 0);
	if( !(ptv == -1 && errno) ) {
		bkptr->ins = ptv;
#if DEBUG > 1
		if(mjmdflag)
			fprintf(FPRT2, "setbp1(): adr=%#x; wa=%#x; ins=%#x;\n",
					adr, wa, bkptr->ins);
#endif
		if(adr == wa)
			val = BPT1 | (bkptr->ins&(~MASK1));
		else		/* instr on half word boundary */
			val = BPT2 | (bkptr->ins&(~MASK2));
		ptv = ptrace(WIUSER, pid, wa, val);
#if DEBUG > 1
		if(mjmdflag)
			fprintf(FPRT2, "	new val=%#x(%#x); wa=%#x;\n",
						ptv, val, wa);
#endif
	}
	if (errno && ptv==-1) {
		procp = adrtoprocp(adr);
		fprintf(FPRT1, "Cannot set breakpoint:  %.8s:%d @ %d\n",
			procp->pname, adrtolineno(adr, procp), dot);
	}
}
#endif

bpwait()
{
	REG ADDR w;
	INT stat;

	signal(SIGINT, 1);
#if DEBUG
	if (debug) fprintf(FPRT2, "Waiting for pid %d\n", pid);
#endif
	while ((w = wait(&stat)) != pid  &&  w!=-1) ;
#if DEBUG
	if (debug) fprintf(FPRT2, "Ending wait\n");
	if (debug) fprintf(FPRT2, "w = %d; pid = %d; stat = %#x;\n",w,pid,stat);
#endif
/*	dot = userpc = SDBREG(PC);	*/
	signal(SIGINT,sigint);
	if (w == -1) {
		pid = 0;
		errflg=BADWAIT;
	}
	else if ((stat & 0177) != 0177) {
		if (signo = stat&0177) sigprint();
		if (stat&0200) {
			error(" - core dumped");
			close(fcor);
			setcor();
		}
		pid=0;
		errflg=ENDPCS;
	}
	else {
		signo = stat>>8;
#if DEBUG
    		if (debug)
		    fprintf(FPRT2, "stat = %#x, signo = %d\n", stat, signo);
#endif
		if (signo != SIGTRAP &&
		    ptrace(RUREGS, pid, SYSREG(PC), 0) != adsubn)
		{
			sigprint();
		}
		else	signo = 0;
	}
}

readregs()
{
	/*get REG values from pcs*/
	REG i;
	for (i=NUMREGLS - 1; --i>=0;) {
        	SDBREG(reglist[i].roffs) =
                    ptrace(RUREGS, pid, SYSREG(reglist[i].roffs), 0);
	}
 	dot = userpc = SDBREG(PC);
}

char 
readchar() {
	lastc = *argsp++;
	if (lastc == '\0') lastc = '\n';
	return(lastc);
}

char
rdc()
{
	register char c;

	c = *argsp++;
	return(c == '\0' ? '\n' : c);
}
