/*	@(#)stat.c	2.1		*/
#include	"crash.h"
#include	"sys/inode.h"
#include	"sys/proc.h"
#include	"sys/file.h"
#include	"sys/mount.h"
#include	"sys/text.h"
#include	"sys/utsname.h"

struct	glop	dumpglop;
struct	dstk	dmpstack;
long	dumploc;

prstat()
{
	long	toc, lbolt;
	unsigned  panicstr;
	struct	utsname	utsname;
	char	panic[30];

	if(readmem(&utsname, (long)SYM_VALUE(Sys), sizeof utsname) != sizeof utsname)
		error("read error on uname structure");
	else {
		printf("\tsysname: %.9s\n", utsname.sysname);
		printf("\tnodename: %.9s\n", utsname.nodename);
		printf("\trelease: %.9s\n", utsname.release);
		printf("\tversion: %.9s\n", utsname.version);
		printf("\tmachine: %.9s\n", utsname.machine);
	}
	if(readmem(&toc, (long)SYM_VALUE(Time), sizeof toc) != sizeof toc)
		toc = 0;
	printf("\ttime of crash: %s", ctime(&toc));
	if(readmem(&lbolt, (long)SYM_VALUE(Lbolt),sizeof lbolt) != sizeof lbolt)
		lbolt = 0;
	printf("\tage of system: ");
	lbolt = (lbolt + (long) 3599) / (long) 3600;
	if(lbolt / (long)(60 * 24))
		printf("%lu day, ", lbolt / (long)(60 * 24));
	lbolt %= (long)(60 * 24);
	if(lbolt / (long)60)
		printf("%lu hr., ", lbolt / (long)60);
	lbolt %= (long) 60;
	if(lbolt)
		printf("%lu min.", lbolt);
	printf("\n");

	if(readmem(&panicstr, (long)SYM_VALUE(Panic), sizeof panicstr) ==
		sizeof panicstr && panicstr != 0) {
			if(readmem(panic, (long)panicstr,
				sizeof(panic)) == sizeof(panic))
					printf("\tpanic: %.30s\n\n", panic);
	}
#ifdef	pdp11
	printf("\tlow memory:\n");
	printf("\t00: %06o  02: %06o  r0: %06o  r1: %06o  r2:  %06o\n",
		dumpglop.g_x0, dumpglop.g_x1, dumpglop.g_r0,
		dumpglop.g_r1, dumpglop.g_r2);
	printf("\tr3: %06o  r4: %06o  r5: %06o  sp: %06o  ka6: %06o\n",
		dumpglop.g_r3, dumpglop.g_r4, dumpglop.g_r5,
		dumpglop.g_sp, dumpglop.g_ka6);
#endif

#ifdef	vax
	dumploc=(SYM_VALUE(Dmpstk)-sizeof(struct dstk) + 4);
	if(readmem(&dmpstack,(long)dumploc,
		sizeof(dmpstack)) != sizeof(dmpstack)) {
			error("read error on dumpstack");
			return(-1);
	}

	printf("\nDUMPSTACK:\tstkptr: %x  pcbb: %x  mapen: %x  ipl: %x",
		dmpstack.stkptr,dmpstack.pcbb,dmpstack.mapen,dmpstack.ipl);
	printf("\n    r0:  %08x  r1:  %08x  r2:  %08x  r3:  %08x  r4:  %08x",
		dmpstack.r0, dmpstack.r1, dmpstack.r2, dmpstack.r3, dmpstack.r4);
	printf("\n    r5:  %08x  r6:  %08x  r7:  %08x  r8:  %08x  r9:  %08x",
		dmpstack.r5,dmpstack.r6,dmpstack.r7,dmpstack.r8,dmpstack.r9);
	printf("\n    r10: %08x  r11: %08x  r12: %08x  r13: %08x\n",
		dmpstack.r10,dmpstack.r11,dmpstack.r12,dmpstack.r13);
#endif
#ifdef	m68k
	dumploc=SYM_VALUE(Dmpstk) ;  /* registers loaded  low to high */
	if(readmem(&dmpstack,(long)dumploc,
		sizeof(dmpstack)) != sizeof(dmpstack)) {
			error("read error on dumpstack");
			return(-1);
	}

#if MC68010
	printf("\nDUMPSTACK:\tvbr: %x  usp: %x  sr: %x ",
		dmpstack.vbr,dmpstack.usp,dmpstack.sr);
#else
	printf("\nDUMPSTACK:\tusp: %x  sr: %x ",
		dmpstack.usp,dmpstack.sr);
#endif
	printf("\n    d0:  %08x  d1:  %08x  d2:  %08x  d3:  %08x",
		dmpstack.d0, dmpstack.d1, dmpstack.d2, dmpstack.d3);
	printf("\n    d4:  %08x  d5:  %08x  d6:  %08x  d7:  %08x",
		dmpstack.d4, dmpstack.d5, dmpstack.d6, dmpstack.d7);
	printf("\n    a0:  %08x  a1:  %08x  a2:  %08x  a3:  %08x",
		dmpstack.a0, dmpstack.a1, dmpstack.a2, dmpstack.a3);
	printf("\n    a4:  %08x  a5:  %08x  a6:  %08x  ksp: %08x \n",
		dmpstack.a4, dmpstack.a5 ,dmpstack.a6, dmpstack.ksp);
#endif
	return(0);
}

prvar()
{
	extern	struct	var	v;

	if(readmem(&v, (long)SYM_VALUE(V), sizeof v) != sizeof v) {
		error("read error on v structure");
		return;
	}
	v.ve_inode = (char *)(((unsigned)v.ve_inode - Inode->n_value) /
		sizeof (struct inode));
	v.ve_file = (char *)(((unsigned)v.ve_file - File->n_value) /
		sizeof (struct file));
	v.ve_mount = (char *)(((unsigned)v.ve_mount - Mount->n_value) /
		sizeof (struct mount));
	v.ve_proc = (char *)(((unsigned)v.ve_proc - Proc->n_value) /
		sizeof (struct proc));
	v.ve_text = (char *)(((unsigned)v.ve_text - Text->n_value) /
		sizeof (struct text));
	printf("buffers	  %3d\n",v.v_buf);
	printf("calls	  %3d\n",v.v_call);
	printf("inodes	  %3d\n",v.v_inode);
	printf("e_inodes  %3d\n",v.ve_inode);
	printf("files	  %3d\n",v.v_file);
	printf("e_files	  %3d\n",v.ve_file);
	printf("mounts	  %3d\n",v.v_mount);
	printf("e_mounts  %3d\n",v.ve_mount);
	printf("procs	  %3d\n",v.v_proc);
	printf("e_procs	  %3d\n",v.ve_proc);
	printf("texts	  %3d\n",v.v_text);
	printf("e_texts	  %3d\n",v.ve_text);
	printf("clists	  %3d\n",v.v_clist);
	printf("sabufs	  %3d\n",v.v_sabuf);
	printf("maxproc	  %3d\n",v.v_maxup);
#ifdef	pdp11
	printf("coremap	  %3d\n",v.v_cmap);
#endif
	printf("swapmap	  %3d\n",v.v_smap);
	printf("hashbuf	  %3d\n",v.v_hbuf);
	printf("hashmask  %3d\n",v.v_hmask);
#if (defined(vax) || defined(m68k))
	printf("physbuf	  %3d\n",v.v_pbuf);
#endif
	return(0);
}
