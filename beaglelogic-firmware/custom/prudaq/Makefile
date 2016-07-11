# * Makefile for modified BeagleLogic PRU1 firmware for PRUDAQ board
# * Builds only with CLPRU 2.0.0B* and above
# * Set PRU_C_DIR to "<path-to-clpru>/include;<path-to-clpru>/lib"

# Tools to be used
CC=clpru
LD=lnkpru
AR=arpru
OBJDUMP=dispru
STRIP=strippru

# Compiler Options
# -v3				PRU version 3
# --c99 			C99 support
# --gcc 			Enable GCC extensions
# -O3				Optimization level maximum
# --printf_support=minimal 	Minimal printf
# -ppd				Generate dependencies *.pp
# -ppa				Continue after generating deps
# -DDEBUG			Enable debug
# CFLAGS= -v3 --c99 --gcc -O3 --printf_support=minimal -ppd -ppa -DDEBUG
CFLAGS= -v3 -s -al -O3 --c99 --gcc --printf_support=minimal --symdebug:none

# Linker Options
# -cr 				Link using RAM auto init model (loader assisted)
# -x				Reread libs until no unresolved symbols found
LDFLAGS=-cr --diag_warning=225 -lam335x_pru.cmd -x

.PHONY: all clean

all: prudaq-ch0 prudaq-ch1 prudaq-ch01

%.obj: %.asm
	@echo "  CC	$@"
	@$(CC) $(CFLAGS) -c $<

prudaq-ch0: prudaq-ch0.obj
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@

prudaq-ch1: prudaq-ch1.obj
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@

prudaq-ch01: prudaq-ch01.obj
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@

clean:
	rm -f *.obj *.lst *.s prudaq-ch0 prudaq-ch1 prudaq-ch01
