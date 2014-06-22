# * Makefile for BeagleLogic PRU firmware
# * Builds only with CLPRU 2.0.0B* and above
# * Set PRU_C_DIR to "<path-to-clpru>/include;<path-to-clpru>/lib"

# BBB hostname
BBHOST = root@192.168.7.2

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

all: beaglelogic-pru0 beaglelogic-pru1 beaglelogic-pru0.elf.lst beaglelogic-pru1.elf.lst

%.obj: %.asm
	@echo "  CC	$@"
	@$(CC) $(CFLAGS) -c $<

%.obj: %.c
	@echo "  CC	$@"
	@$(CC) $(CFLAGS) -c $< -ea=.s

beaglelogic-pru0: beaglelogic-pru0.obj beaglelogic-pru0-core.obj beaglelogic-pru0-syscalls.obj
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@
	
beaglelogic-pru1: beaglelogic-pru1-core.obj
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@

beaglelogic-pru0.elf.lst: beaglelogic-pru0
	@echo "  DUMP	$@"
	@$(OBJDUMP) -1 $< > $@

beaglelogic-pru1.elf.lst: beaglelogic-pru1
	@echo "  DUMP	$@"
	@$(OBJDUMP) -1 $< > $@

install-frombb: beaglelogic-pru0 beaglelogic-pru1
	cp -t /lib/firmware beaglelogic-pru0 beaglelogic-pru1

install-tobb: beaglelogic-pru0 beaglelogic-pru1
	scp -q beaglelogic-pru0 $(BBHOST):/lib/firmware
	scp -q beaglelogic-pru1 $(BBHOST):/lib/firmware

clean:
	rm -f *.obj *.lst *.s beaglelogic-pru0 beaglelogic-pru1

