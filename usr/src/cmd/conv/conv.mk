
#	@(#)conv.mk	2.1.1.1	

#	conv.mk conv makefile

install:conv
	if m68k; \
	then \
	cd m68k; \
		make -b SGS="" ROOT="/" OWN="bin" GRP="bin"\
	   PROT="775" AR="ar" CC="cc" YACC="yacc" LINT="lint" LFLAGS="-p"\
	   CFLAGS="-O" LDFLAGS="" BINDIR="/usr/bin" LIBDIR="/usr/lib"\
	   DEFLIST="-DINT32BIT" ARCH="AR32W" ARFORMAT="PORTARW"\
	   FLEX=-DFLEXNAMES USRINC="" LIBLD="../../libld/common/libld.a" install; \
	   fi
	
conv:
	cd m68k;\
	if m68k;\
	then \
		make -b SGS="" ROOT="/port/port" OWN="bin" GRP="bin"\
	   PROT="775" AR="ar" CC="cc" YACC="yacc" LINT="lint" LFLAGS="-p"\
	   CFLAGS="-O" LDFLAGS="" BINDIR="/usr/bin" LIBDIR="/usr/lib"\
	   DEFLIST="-DINT32BIT" ARCH="AR32W" ARFORMAT="PORTARW"\
	   FLEX=-DFLEXNAMES USRINC="" LIBLD="../../libld/common/libld.a" conv; \
	elif vax; \
	then\
		make -b SGS="" ROOT="/" OWN="bin" GRP="bin" PROT="775"\
	   AR="ar" CC="cc" YACC="yacc" LINT="lint" LFLAGS="-p" CFLAGS="-O"\
	   LDFLAGS="" BINDIR="/usr/bin" LIBDIR="/usr/lib" DEFLIST="-DINT32BIT"\
	   ARCH="AR32WR" ARFORMAT="PORTAR" FLEX=-DFLEXNAMES USRINC="" \
	   LIBLD="../../libld/common/libld.a" conv;\
	fi
