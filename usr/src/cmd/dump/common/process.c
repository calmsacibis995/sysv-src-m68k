/*	@(#)process.c	2.1		*/
#include	<stdio.h>
#include	<ar.h>
#include	"filehdr.h"
#include	"ldfcn.h"
#include	"filedefs.h"
#include	"flagdefs.h"
#include	"process.h"
#include	"sgs.h"


process(listhead)

FILELIST	*listhead;

{
    extern int		fprintf( ),
			sprintf( ),
			cfree( ),
			ldaclose( ),
			ldclose( ),
			ldahread( );

    extern LDFILE	*ldaopen( );

    extern FLAG		flag[ ];
    extern int		vflag;

    int			i,
			status;
    FILELIST		*listptr,
			*saveptr,
			*lastptr;
    LDFILE		*newptr;
    ARCHDR		arhead;
    char		name[513];


    for (i = 0; i < NUMFLAGS; ++i) {
	if (flag[i].setting == ON) {
	    printf(vflag?prvtitle[i]:prtitle[i]);
	    lastptr = NULL;
	    for (listptr = listhead; listptr != NULL; listptr = saveptr) {
		saveptr = listptr->nextitem;

#ifdef PORTAR
		if (TYPE(listptr->ldptr) == ARTYPE) {
#else
		if (TYPE(listptr->ldptr) == ARMAG) {
#endif
		    if ((newptr=ldaopen(listptr->file, listptr->ldptr))==NULL) {
			fprintf(stderr, 
			        "%sobject:  cannot ldaopen %s\n", SGS, listptr->file);
			status = FAILURE;
		    } else {
			do {
			    if ((status=ldahread(newptr, &arhead)) == SUCCESS) {
#ifdef PORTAR
				sprintf(name, "%s[%.16s]", listptr->file,
					arhead.ar_name);
#else
				sprintf(name, "%s[%.14s]", listptr->file,
					arhead.ar_name);
#endif
				if ((status = (*(flag[i].ldfcn))(newptr, name))
				    != SUCCESS) {
				    ldaclose(newptr);
				}
			    } else {
				ldaclose(newptr);
			    }
			} while(ldclose(newptr) == FAILURE);
		    }
		} else {
		    status = (*(flag[i].ldfcn))(listptr->ldptr, listptr->file);
		}

		if (status != SUCCESS) {
		    ldaclose(listptr->ldptr);
		    if (lastptr = NULL) {
			listhead = saveptr;
		    } else {
			lastptr->nextitem = saveptr;
		    }
		    cfree(listptr);
		} else {
		    lastptr = listptr;
		}
	    }
	}
    }
    return;
}
