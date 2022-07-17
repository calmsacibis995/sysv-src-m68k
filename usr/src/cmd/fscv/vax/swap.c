/* @(#)swap.c	2.1		*/
#if m68k || MC68
swapl( lp )
char lp[];	/* long pointer */
{
	/* Subroutine to perform m68k <-> VAX long conversion */
	register char temp;

	temp=lp[0];
	lp[0]=lp[3];
	lp[3]=temp;
	temp=lp[2];
	lp[2]=lp[1];
	lp[1]=temp;
}

swaps( sp )
char sp[];	/* short pointer */
{
	/* Subroutine to perform m68k <-> VAX short conversion */
	register char temp;

	temp=sp[0];
	sp[0]=sp[1];
	sp[1]=temp;
}
#else
swap(longnum)
short longnum[];
{
	/* Subroutine to exchange the high order and low order words of long */
	register short temp;

	temp = longnum[0];
	longnum[0] = longnum[1];
	longnum[1] = temp;
}
#endif
