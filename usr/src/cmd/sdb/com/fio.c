	/*	@(#)fio.c	2.1		*/

#include "head.h"
char	fbuf[256];	/* current line from file */

/*
 * These procedures manage the source files examined by sdb.
 */

/* Change the current source file to `name'. */
finit(name)
char *name; {
	register char *p, *q;
	
#if DEBUG > 1
	if(mjmdflag)
		fprintf(FPRT2, "finit(name=%s); fwp=%s\n", name, fwp);
#endif
	if (fiobuf.fd) close(fiobuf.fd);
	q = name;
	for (p=fwp; *q; *p++ = *q++) ;
	*p = 0;
	if ((fiobuf.fd = open(filework,0)) == -1) {
		nolines = 1;
		perror(filework);
		return;
	}
	binit(&fiobuf);
	cpstr(curfile, name);
	for (p=fbuf;;p++) {
		if (p >= fbuf + sizeof fbuf)
			p = fbuf + sizeof fbuf - 1;
		if (bread(&fiobuf,p,1) <= 0) {
			nolines = 1;
			fprintf(FPRT1, "%s: No lines in file\n", name);
			return;
		}
		if (*p == '\n') break;
	}
	curstmt.lnno = 1;
	curstmt.stno = 1;
	maxfline = 0;
	nolines = 0;
}

/* Make the next line current. */
fnext() {
	register char *p;
	
	if (nolines){
		return;
	}
	for(p=fbuf;;p++) {
		if (bread(&fiobuf,p,1) <= 0) {
			p--;
			blseek(&fiobuf,0L,0);
			curstmt.lnno = 0;
			continue;
		}
		if (*p == '\n') break;
	}
	curstmt.lnno++;
	curstmt.stno = 1;
}


/* Make the previous line current. */
fprev() {
	char c;
	register int i;
	
	if (nolines){
		return;
	}
	for(i=0; i<3; i++) {
		for (;;) {
			if (bread(&fiobuf, &c+1, -1) <= 0) {
				if (maxfline) blseek(&fiobuf,0L,2);
				else {
					blseek(&fiobuf,0L,0);
					for(;;) {
						if (bread(&fiobuf,&c,1)<=0)
							break;
						if (c == '\n') maxfline++;
					}
				}
			}
			if (c == '\n') break;
		}
	}
	bread(&fiobuf, &c, 1);  /* eat the '\n' */
	
	curstmt.lnno -= 2;
	if (curstmt.lnno < 0) curstmt.lnno = maxfline - 1;
	curstmt.stno = 1;
	
	fnext();
}


/* Print the current line. */
fprint() {
	register char *p;
	
	if (nolines){
		error("No lines in file");
		return;
	}
	printstmt(curstmt);
	printf(": ");
	p = fbuf;
	while(putchar(*p++) != '\n')
		;
}

/* Make line `num' current. */
ffind(num)
register int num; {
	register int i, ofline;
	
	if (nolines){
		return;
	}
	ofline = curstmt.lnno;
	if (num>curstmt.lnno)
		for (i=curstmt.lnno; i<num; i++) {
			fnext();
			if (curstmt.lnno == 1) goto bad;
		}
	if (num<curstmt.lnno)
		for (i=num; i<ofline; i++) {
			fprev();
		}
		
	if (maxfline & num>maxfline) goto bad;
	
	return;

bad:	error("Not that many lines in file");
	ffind(ofline);
}

/* Go back n lines. */
fback(n) {
	int i;
	
	if (nolines){
		return(0);
	}
	for (i=0; i<n; i++) {
		if (curstmt.lnno == 1) return(i);
		fprev();
	}
	return(i);
}

/* Go forwards n lines. */
fforward(n) {
	int i;
	
	if (nolines){
		return(0);
	}
	for (i=0; i<n; i++) {
		fnext();
		if (curstmt.lnno == 1) {
			fprev();
			return(i);
		}
	}
	return(i);
}

/* Print n lines. */
fprintn(n) {
	int i;
	
	if (nolines){
		error("No lines in file");
		return(0);
	}
	for (i=0; i<n; i++) {
		fprint();
		fnext();
		if (curstmt.lnno == 1) break;
	}
	fprev();
	return(i);
}
