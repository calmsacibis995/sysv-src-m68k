/*	@(#)paths.h	2.1		*/
/*
 * Pathnames for UNIX V/68
 */

#ifndef LIBDIR
#define LIBDIR	"/lib"
#endif

#ifndef LLIBDIR1
#define LLIBDIR1 "/usr/lib"
#endif

#define NDELDIRS 2

/*
 * Directory containing executable ("bin") files
 */
#define BINDIR	"/bin"

/*
 * Directory for "temp"  files
 */
#define TMPDIR	"/tmp"

/*
 * Name of default output object file
 */
#define A_OUT	"a.out"

#define	M4	"/usr/bin/m4"		/* macro processor program */

/*
** Names of various pieces called by the cc command.  The names are
** formed by prepending either BINDIR (B) or LIBDIR (L) and the cc
** command prefix (e.g., "mc68" in "mc68cc").  See cc command for
** details.
*/

#define NAMEcpp		"cpp"		/* C preprocessor:  L		*/
#define NAMEccom	"ccom"		/* C compiler:  L		*/
#define NAMEoptim	"optim"		/* optimizer:  L		*/
#define NAMEas		"as"		/* assembler:  B		*/
#define NAMEld		"ld"		/* loader:  B			*/
#define NAMEcrt0	"crt0.o"	/* C run time startoff:  L	*/
#define NAMEmcrt0	"mcrt0.o"	/* profiling startoff:  L	*/
