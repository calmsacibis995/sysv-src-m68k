/*	@(#)process.h	2.1		*/
static char	*prtitle[NUMFLAGS] = {
	"\n\n\t\t\t***ARCHIVE HEADER***\n\tMember Name        Date     Uid  Gid   Mode       Size\n\n",
/*
	12345678901234  0x12345678  123  123  0123456  0x12345678
	Member Name        Date     Uid  Gid   Mode       Size
*/

	"\n\n\t\t\t***FILE HEADER***\n\t Magic   Nscns  Time/Date     Symptr      Nsyms     Opthdr  Flags\n\n",
/*
	0123456  12345  0x12345678  0x12345678  1234567890  0x1234  0x1234
	 Magic   Nscns  Time/Date     Symptr      Nsyms     Opthdr  Flags
*/

	"\n\n\t\t\t***OPTIONAL HEADER in HEX***\n\n",

	"\n\n\t\t\t***SECTION HEADER***\n\tName        Paddr       Vaddr       Scnptr      Relptr     Lnnoptr\n\t            Flags                    Size       Nreloc      Nlnno\n\n",
/*
	12345678  0x12345678  0x12345678  0x12345678  0x12345678  0x12345678
		  0x12345678	 	  0x12345678    12345       12345
	Name        Paddr       Vaddr       Scnptr      Relptr     Lnnoptr
		    Flags		     Size       Nreloc      Nlnno
*/


	"\n\n\t\t\t***SECTION DATA in HEX***\n",

	"\n\n\t\t\t***RELOCATION INFORMATION***\n\t  Vaddr       Symndx     Type\n\n",
/*
	0x12345678  1234567890  123456
	  Vaddr       Symndx    Type
*/

	"\n\n\t\t\t***LINE NUMBER INFORMATION***\n\tSymndx/Paddr  Lnno\n\n",
/*
	 0x12345678   12345
	Symndx/Paddr  Lnno
*/

	"\n\n\t\t\t***LINE NUMBERS FOR FUNCTION***\n\tSymndx/Paddr  Lnno\n\n",
/*
	 0x12345678   12345
	Symndx/Paddr  Lnno
*/
	"\n\n\t\t\t***SYMBOL TABLE INFORMATION***\n     [Index]  Name        Value      Scnum   Type   Sclass  Numaux\naux\t       Tagndx     Misc    Misc     Fcnary       Fcnary    Tvdnx\n\n"
/*

[1234567890]  12345678  0x12345678  12345  0x1234  0x12    123
   [Index]       Name     Value     Scnum   Type   Sclass  Numaux
               1234567890  12345  0x1234  0x12345678  0x12345678  12345
aux              Tagndx    Lnno     Size     Fcnary      Fcnary   Tvndx
*/
};

static  char		*prvtitle[NUMFLAGS] = {
	"\n\n\t\t\t***ARCHIVE HEADER***\n\tMember Name        Date               Uid  Gid   Mode       Size\n\n",
/*
	12345678901234  0x12345678  123  123  0123456  0x12345678
	Member Name        Date     Uid  Gid   Mode       Size
*/

	"\n\n\t\t\t   ***FILE HEADER***\nMagic      Nscns     Time/Date          Symptr      Nsyms Opthdr  Flags\n\n",
/*
	0123456  12345  0x12345678  0x12345678  1234567890  0x1234  0x1234
	 Magic   Nscns  Time/Date     Symptr      Nsyms     Opthdr  Flags
*/

	"\n\n\t\t\t***OPTIONAL HEADER in HEX***\n\n",

	"\n\n\t\t\t***SECTION HEADER***\n\tName        Paddr       Vaddr       Scnptr      Relptr     Lnnoptr\n\t            Flags                    Size       Nreloc      Nlnno\n\n",
/*
	12345678  0x12345678  0x12345678  0x12345678  0x12345678  0x12345678
		  0x12345678	 	  0x12345678    12345       12345
	Name        Paddr       Vaddr       Scnptr      Relptr     Lnnoptr
		    Flags		     Size       Nreloc      Nlnno
*/


	"\n\n\t\t\t***SECTION DATA in HEX***\n",

	"\n\n\t\t\t***RELOCATION INFORMATION***\n\t  Vaddr       Symndx     Type     Name\n\n",
/*
	0x12345678  1234567890  123456
	  Vaddr       Symndx    Type     Name
*/

	"\n\n\t\t\t***LINE NUMBER INFORMATION***\n\tSymndx/Paddr  Lnno     Name\n\n",
/*
	 0x12345678   12345
	Symndx/Paddr  Lnno     Name
*/

	"\n\n\t\t\t***LINE NUMBERS FOR FUNCTION***\n\tSymndx/Paddr  Lnno\n\n",
/*
	 0x12345678   12345
	Symndx/Paddr  Lnno
*/
	"\n\n\t\t\t***SYMBOL TABLE INFORMATION***\n     [Index]  Name        Value      Scnum   Type   Sclass  Numaux\n\n\
aux[1]             Fname\n\
aux[2]         Tvfill     Tvlen    Tvran[0]    Tvran[1]\n\
aux[3]         Scnlen    Nreloc     Nlinno\n\
aux[4]         Tagndx    Fsize         Lnnoptr    Endndx           Tvndx\n\
aux[5]         Tagndx    Fsize  Dim[0]  Dim[1]  Dim[2]  Dim[3]    Tvndx\n\
aux[6]         Tagndx    Lnno    Size    Lnnoptr   Endndx         Tvndx\n\
aux[7]         Tagndx    Lnno    Size  Dim[0]  Dim[1]  Dim[2]  Dim[3]  Tvndx\n\n\n"
};
