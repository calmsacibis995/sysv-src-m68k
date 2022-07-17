/*	@(#)filedefs.h	2.1		*/
struct filelist {
	char		*file;
	LDFILE		*ldptr;
	struct filelist	*nextitem;
};

#define FILELIST	struct filelist
#define LISTSZ	sizeof(FILELIST)
