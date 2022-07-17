# @(#)profiler.mk	2.1	

OL =
TESTDIR = .
CC = cc
CFLAGS = -O

compile all:
	-if vax || m68k; then cd vax; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)"; \
		else cd pdp11; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)"; fi
		

install: 
	-if vax || m68k; then cd vax; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)" \
			install;\
		else cd pdp11; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)" \
			install;fi

clean:
	-if vax || m68k; then cd vax; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)" \
			clean;\
		else cd pdp11; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)" \
			clean;fi

clobber:
	-if vax || m68k; then cd vax; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)" \
			clobber;\
		else cd pdp11; \
		make -f profiler.mk \
			OL="$(OL)" TESTDIR="$(TESTDIR)" CFLAGS="$(CFLAGS)" \
			clobber;fi
