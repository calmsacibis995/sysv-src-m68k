#	@(#)libF77.mk	2.1.1.2 
#	3.0 SID #	1.2
# compile, then strip unnecessary symbols

OWN=	bin		#file owner
GRP=	bin		#file group
PROT=	664		#protections

ROOT =
INSDIR = $(ROOT)/usr/lib
TESTDIR = .
FRC =
.c.o :
	rm -f $@
	-cc -O -c $<
	-chmod $(PROT) $@
	-chgrp $(GRP)  $@
	-chown $(OWN)  $@
	-if m68k; \
		then	:; \
		else	ld -r -x $*.o; \
			mv a.out $*.o; \
	fi

MISC =	Version.o main.o s_rnge.o abort_.o getarg_.o iargc_.o getenv_.o signal_.o s_stop.o s_paus.o system_.o
POW =	pow_ci.o pow_dd.o pow_di.o pow_hh.o pow_ii.o  pow_ri.o pow_zi.o pow_zz.o
CX =	c_abs.o c_cos.o c_div.o c_exp.o c_log.o c_sin.o c_sqrt.o
DCX =	z_abs.o z_cos.o z_div.o z_exp.o z_log.o z_sin.o z_sqrt.o
REAL =	r_abs.o r_acos.o r_asin.o r_atan.o r_atn2.o r_cnjg.o r_cos.o\
	r_cosh.o r_dim.o r_exp.o r_imag.o r_int.o\
	r_lg10.o r_log.o r_mod.o r_nint.o r_sign.o\
	r_sin.o r_sinh.o r_sqrt.o r_tan.o r_tanh.o rand_.o
DBL =	d_abs.o d_acos.o d_asin.o d_atan.o d_atn2.o\
	d_cnjg.o d_cos.o d_cosh.o d_dim.o d_exp.o\
	d_imag.o d_int.o d_lg10.o d_log.o d_mod.o\
	d_nint.o d_prod.o d_sign.o d_sin.o d_sinh.o\
	d_sqrt.o d_tan.o d_tanh.o 
INT =	i_abs.o i_dim.o i_dnnt.o i_indx.o i_len.o i_mod.o i_nint.o i_sign.o
HALF =	h_abs.o h_dim.o h_dnnt.o h_indx.o h_len.o h_mod.o  h_nint.o h_sign.o
CMP =	l_ge.o l_gt.o l_le.o l_lt.o hl_ge.o hl_gt.o hl_le.o hl_lt.o
EFL =	ef1asc_.o ef1cmc_.o
CHAR =	s_cat.o s_cmp.o s_copy.o 

all: libF77.a

libF77.a : $(MISC) $(POW) $(CX) $(DCX) $(REAL) $(DBL) $(INT) $(HALF) $(CMP) $(EFL) $(CHAR) cabs.o tanh.o sinh.o
	ar r libF77.a $?

$(MISC) $(POW) $(CX):	complex $(FRC)
$(DCX) $(REAL):		complex $(FRC)
$(DBL) $(INT):		complex $(FRC)
$(HALF) $(CMP) $(CHAR): complex $(FRC)

install: all save
	-rm -f $(INSDIR)/libF77.a
	cp $(TESTDIR)/libF77.a $(INSDIR)/libF77.a 
	chmod $(PROT) $(INSDIR)/libF77.a
	chmod -x $(INSDIR)/libF77.a
	chgrp $(GRP) $(INSDIR)/libF77.a
	chown $(OWN) $(INSDIR)/libF77.a

#--------------------------

uninstall:	$(INSDIR)/OLDlibF77.a
		-rm -f $(INSDIR)/libF77.a
		cp $(INSDIR)/OLDlibF77.a $(INSDIR)/libF77.a
		chmod $(PROT) $(INSDIR)/libF77.a
		chmod -x $(INSDIR)/libF77.a
		chgrp $(GRP) $(INSDIR)/libF77.a
		chown $(OWN) $(INSDIR)/libF77.a

#--------------------------

save:		
		if test -f $(INSDIR)/libF77.a ; \
		then \
			rm -f $(INSDIR)/OLDlibF77.a ; \
			cp $(INSDIR)/libF77.a $(INSDIR)/OLDlibF77.a ; \
		fi

clean:
	 -rm -f *.o

clobber: clean
	 -rm -f $(TESTDIR)/libF77.a

FRC:

