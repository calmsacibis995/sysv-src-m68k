/*	@(#)t8.c	2.1		*/
 /* t8.c: write out one line of output table */
# include "t..c"
# define realsplit ((ct=='a'||ct=='n') && table[nl][c].rcol)
int watchout;
int once;
putline(i, nl)
	/* i is line number for deciding format */
	/* nl is line number for finding data   usually identical */
{
int c, lf, ct, form, lwid, vspf, ip, cmidx, exvspen, vforml;
int vct, chfont, uphalf;
char *s, *size, *fn;
watchout=vspf=exvspen=0;
if (i==0) once=0;
if (i==0 && ( allflg || boxflg || dboxflg))
	fullwide(0,   dboxflg? '=' : '-');
if (instead[nl]==0 && fullbot[nl] ==0)
for(c=0; c<ncol; c++)
	{
	s = table[nl][c].col;
	if (s==0) continue;
	if (vspen(s))
		{
		for(ip=nl; ip<nlin; ip=next(ip))
			if (!vspen(s=table[ip][c].col)) break;
		if ((int)s>0 && (int)s<128)
		fprintf(tabout, ".ne \\n(%c|u+\\n(.Vu\n",s);
		continue;
		}
	if (point(s)) continue;
	fprintf(tabout, ".ne \\n(%c|u+\\n(.Vu\n",s);
	watchout=1;
	}
if (linestop[nl])
	fprintf(tabout, ".mk #%c\n", linestop[nl]+'a'-1);
lf = prev(nl);
if (instead[nl])
	{
	puts(instead[nl]);
	return;
	}
if (fullbot[nl])
	{
	switch (ct=fullbot[nl])
		{
		case '=':
		case '-':
			fullwide(nl,ct);
		}
	return;
	}
for(c=0; c<ncol; c++)
	{
	if (instead[nl]==0 && fullbot[nl]==0)
	if (vspen(table[nl][c].col)) vspf=1;
	if (lf>=0)
		if (vspen(table[lf][c].col)) vspf=1;
	}
if (vspf)
	{
	fprintf(tabout, ".nr #^ \\n(\\*(#du\n");
	fprintf(tabout, ".nr #- \\n(#^\n"); /* current line position relative to bottom */
	}
vspf=0;
chfont=0;
for(c=0; c<ncol; c++)
	{
	s = table[nl][c].col;
	if (s==0) continue;
	chfont |= (int)(font[c][stynum[nl]]);
	if (point(s) ) continue;
	lf=prev(nl);
	if (lf>=0 && vspen(table[lf][c].col))
		fprintf(tabout, ".if (\\n(%c|+\\n(^%c-1v)>\\n(#- .nr #- +(\\n(%c|+\\n(^%c-\\n(#--1v)\n",s,'a'+c,s,'a'+c);
	else
		fprintf(tabout, ".if (\\n(%c|+\\n(#^-1v)>\\n(#- .nr #- +(\\n(%c|+\\n(#^-\\n(#--1v)\n",s,s);
	}
if (allflg && once>0 )
	fullwide(i,'-');
once=1;
runtabs(i, nl);
if (allh(i) && !pr1403)
	{
	fprintf(tabout, ".nr %d \\n(.v\n", SVS);
	fprintf(tabout, ".vs \\n(.vu-\\n(.sp\n");
	fprintf(tabout, ".nr 35 \\n(.vu\n");
	}
else
	fprintf(tabout, ".nr 35 1m\n");
if (chfont)
	fprintf(tabout, ".nr %2d \\n(.f\n", S1);
fprintf(tabout, "\\&");
vct = 0;
for(c=0; c<ncol; c++)
	{
	uphalf=0;
	if (watchout==0 && i+1<nlin && (lf=left(i,c, &lwid))>=0)
		{
		tohcol(c);
		drawvert(lf, i, c, lwid);
		vct += 2;
		}
	if (rightl && c+1==ncol) continue;
	vforml=i;
	for(lf=prev(nl); lf>=0 && vspen(table[lf][c].col); lf=prev(lf))
		vforml= lf;
	form= ctype(vforml,c);
	if (form != 's')
		{
		ct = reg(c,CLEFT);
		if (form=='a') ct = reg(c,CMID);
		if (form=='n' && table[nl][c].rcol && lused[c]==0) ct= reg(c,CMID);
		fprintf(tabout, "\\h'|\\n(%2su'", ct);
		}
	s= table[nl][c].col;
	fn = font[c][stynum[vforml]];
	size = csize[c][stynum[vforml]];
	if (*size==0)size=0;
	if ((flags[c][stynum[nl]] & HALFUP)!=0)
		uphalf=1;
	switch(ct=ctype(vforml, c))
		{
		case 'n':
		case 'a':
			if (table[nl][c].rcol)
				{
			   if (lused[c]) /*Zero field width*/
				{
				ip = prev(nl);
				if (ip>=0)
				if (vspen(table[ip][c].col))
					{
					if (exvspen==0)
						{
						fprintf(tabout, "\\v'-(\\n(\\*(#du-\\n(^%cu", c+'a');
						if (cmidx)
							fprintf(tabout, "-((\\n(#-u-\\n(^%cu)/2u)", c+'a');
						vct++;
						fprintf(tabout, "'");
						exvspen=1;
						}
					}
				fprintf(tabout, "%c%c",F1,F2);
				if (uphalf) fprintf(tabout, "\\u");
				puttext(s,fn,size);
				if (uphalf) fprintf(tabout, "\\d");
				fprintf(tabout, "%c",F1);
				}
				s= table[nl][c].rcol;
				form=1;
				break;
				}
		case 'c':
			form=3; break;
		case 'r':
			form=2; break;
		case 'l':
			form=1; break;
		case '-':
		case '=':
			if (real(table[nl][c].col))
				fprintf(stderr,"%s: line %d: Data ignored on table line %d\n", ifile, iline-1, i+1);
			makeline(i,c,ct);
			continue;
		default:
			continue;
		}
	if (realsplit ? rused[c]: used[c]) /*Zero field width*/
		{
		/* form: 1 left, 2 right, 3 center adjust */
		if (ifline(s))
			{
			makeline(i,c,ifline(s));
			continue;
			}
		if (filler(s))
			{
			fprintf(tabout, "\\l'|\\n(%2su\\&%s'", reg(c,CRIGHT), s+2);
			continue;
			}
		ip = prev(nl);
		cmidx = (flags[c][stynum[nl]] & (CTOP|CDOWN))==0;
		if (ip>=0)
		if (vspen(table[ip][c].col))
			{
			if (exvspen==0)
				{
				fprintf(tabout, "\\v'-(\\n(\\*(#du-\\n(^%cu", c+'a');
				if (cmidx)
					fprintf(tabout, "-((\\n(#-u-\\n(^%cu)/2u)", c+'a');
				vct++;
				fprintf(tabout, "'");
				}
			}
		fprintf(tabout, "%c", F1);
		if (form!= 1)
			fprintf(tabout, "%c", F2);
		if (vspen(s))
			vspf=1;
		else
		{
		if (uphalf) fprintf(tabout, "\\u");
		puttext(s, fn, size);
		if (uphalf) fprintf(tabout, "\\d");
		}
		if (form !=2)
			fprintf(tabout, "%c", F2);
		fprintf(tabout, "%c", F1);
		}
	ip = prev(nl);
	if (ip>=0)
	if (vspen(table[ip][c].col))
		{
		exvspen = (c+1 < ncol) && vspen(table[ip][c+1].col) &&
			(topat[c] == topat[c+1]) &&
			(cmidx == (flags[c+1] [stynum[nl]]&(CTOP|CDOWN)==0)) && (left(i,c+1,&lwid)<0);
		if (exvspen==0)
			{
			fprintf(tabout, "\\v'(\\n(\\*(#du-\\n(^%cu", c+'a');
			if (cmidx)
				fprintf(tabout, "-((\\n(#-u-\\n(^%cu)/2u)", c+'a');
			vct++;
			fprintf(tabout, "'");
			}
		}
	else
		exvspen=0;
	/* if lines need to be split for gcos here is the place for a backslash */
	if (vct > 7 && c < ncol)
		{
		fprintf(tabout, "\n.sp-1\n\\&");
		vct=0;
		}
	}
fprintf(tabout, "\n");
if (allh(i) && !pr1403) fprintf(tabout, ".vs \\n(%du\n", SVS);
if (watchout)
	funnies(i,nl);
if (vspf)
	{
	for(c=0; c<ncol; c++)
		if (vspen(table[nl][c].col) && (nl==0 || (lf=prev(nl))<0 || !vspen(table[lf][c].col)))
			{
			fprintf(tabout, ".nr ^%c \\n(#^u\n", 'a'+c);
			topat[c]=nl;
			}
	}
}
puttext(s,fn, size)
	char *s, *size, *fn;
{
if (point(s))
	{
	putfont(fn);
	putsize(size);
	fprintf(tabout, "%s",s);
	if (*fn>0) fprintf(tabout, "\\f\\n(%2d", S1);
	if (size!=0) putsize("0");
	}
}
funnies( stl, lin)
{
/* write out funny diverted things */
int c, s, pl, lwid, dv, lf, ct;
char *fn;
fprintf(tabout, ".mk ##\n"); /* rmember current vertical position */
fprintf(tabout, ".nr %d \\n(##\n", S1); /* bottom position */
for(c=0; c<ncol; c++)
	{
	s = (int)table[lin][c].col;
	if (point(s)) continue;
	if (s==0) continue;
	fprintf(tabout, ".sp |\\n(##u-1v\n");
	fprintf(tabout, ".nr %d ", SIND);
	for(pl=stl; pl>=0 && !isalpha(ct=ctype(pl,c)); pl=prev(pl))
		;
	switch (ct)
		{
		case 'n':
		case 'c':
			fprintf(tabout, "(\\n(%2su+\\n(%2su-\\n(%c-u)/2u\n",reg(c,CLEFT),reg(c-1+ctspan(lin,c),CRIGHT), s);
			break;
		case 'l':
			fprintf(tabout, "\\n(%2su\n",reg(c,CLEFT));
			break;
		case 'a':
			fprintf(tabout, "\\n(%2su\n",reg(c,CMID));
			break;
		case 'r':
			fprintf(tabout, "\\n(%2su-\\n(%c-u\n", reg(c,CRIGHT), s);
			break;
		}
	fprintf(tabout, ".in +\\n(%du\n", SIND);
	fn=font[c][stynum[stl]];
	putfont(fn);
	pl = prev(stl);
	if (stl>0 && pl>=0 && vspen(table[pl][c].col))
		{
		fprintf(tabout, ".sp |\\n(^%cu\n", 'a'+c);
		if ((flags[c][stynum[stl]]&(CTOP|CDOWN))==0)
			{
			fprintf(tabout, ".nr %d \\n(#-u-\\n(^%c-\\n(%c|+1v\n",TMP, 'a'+c, s);
			fprintf(tabout, ".if \\n(%d>0 .sp \\n(%du/2u\n", TMP, TMP);
			}
		}
	fprintf(tabout, ".%c+\n",s);
	fprintf(tabout, ".in -\\n(%du\n", SIND);
	if (*fn>0) putfont("P");
	fprintf(tabout, ".mk %d\n", S2);
	fprintf(tabout, ".if \\n(%d>\\n(%d .nr %d \\n(%d\n", S2, S1, S1, S2);
	}
fprintf(tabout, ".sp |\\n(%du\n", S1);
for(c=dv=0; c<ncol; c++)
	{
	if (stl+1< nlin && (lf=left(stl,c,&lwid))>=0)
		{
		if (dv++ == 0)
			fprintf(tabout, ".sp -1\n");
		tohcol(c);
		dv++;
		drawvert(lf, stl, c, lwid);
		}
	}
if (dv)
	fprintf(tabout,"\n");
}
putfont(fn)
	char *fn;
{
if (fn && *fn)
	fprintf(tabout,  fn[1] ? "\\f(%.2s" : "\\f%.2s",  fn);
}
putsize(s)
	char *s;
{
if (s && *s)
	fprintf(tabout, "\\s%s",s);
}
