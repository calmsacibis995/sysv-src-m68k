/*	@(#)nmf.c	2.1		*/
#include <stdio.h>

#if ( vax || u3b || m68k )
#define SYMSTART 11
#define SYMCLASS 9
#endif

#ifdef pdp11
#define SYMSTART 9
#define SYMCLASS 7
#endif

main(argc, argv)
char	*argv[];
{
	char	name[15], buf[64];
	char	*fname = NULL;
	char	*p;

	strcpy(name, argc > 1? argv[1] : "");
	if (argc > 2)
		fname = argv[2];
	while (gets(buf)) {
		p = &buf[SYMSTART];
		if (*p == '_')
			++p;
		switch (buf[SYMCLASS]) {
		case 'U':
			printf("%s : %s\n", name, p);
			continue;
		case 'T':
			printf("%s = text", p);
			strcpy(name, p);
			break;
		case 'D':
			printf("%s = data", p);
			if (strcmp(name, "") == 0)
				strcpy(name, p);
			break;
		case 'B':
			printf("%s = bss", p);
			break;
		case 'A':
			printf("%s = abs", p);
			break;
		default:
			continue;
		}
		if (fname != NULL)
			printf(", %s", fname);
		printf("\n");
	}
	exit(0);
}
