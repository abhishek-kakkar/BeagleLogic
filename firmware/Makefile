# Makefile for BeagleLogic PRU firmware
# Based on Makefile provided by TI examples
#
# PRU_CGT environment variable must point to the TI PRU code gen tools directory. E.g.:
#(Desktop Linux) export PRU_CGT=/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
#(Windows) set PRU_CGT=C:/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
#(ARM Linux*) export PRU_CGT=/usr/share/ti/cgt-pru
#
# *ARM Linux also needs to create a symbolic link to the /usr/bin/ directory in
# order to use the same Makefile
#(ARM Linux) ln -s /usr/bin/ /usr/share/ti/cgt-pru/bin

ifndef PRU_CGT
define ERROR_BODY

*******************************************************************************
PRU_CGT environment variable is not set. Examples given:
(Desktop Linux) export PRU_CGT=/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
(Windows) set PRU_CGT=C:/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
(ARM Linux*) export PRU_CGT=/usr/share/ti/cgt-pru

*ARM Linux also needs to create a symbolic link to the /usr/bin/ directory in
order to use the same Makefile
(ARM Linux) ln -s /usr/bin/ /usr/share/ti/cgt-pru/bin
*******************************************************************************

endef
$(error $(ERROR_BODY))
endif

# Point to PRU SW Support Package installation
# On a Debian BeagleBoard image this is at the following location
PRU_SP_PATH ?= /usr/lib/ti/pru-software-support-package

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)

LINKER_COMMAND_FILE=./AM335x_PRU.cmd

LIBS=--library=$(PRU_SP_PATH)/lib/rpmsg_lib.lib

INCLUDE=--include_path=$(PRU_SP_PATH)/include --include_path=$(PRU_SP_PATH)/include/am335x

STACK_SIZE=0x100
HEAP_SIZE=0x100

GEN_DIR=release

#Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --display_error_number --endian=little --hardware_mac=on --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa

#Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE)

TARGET_PRU0=$(GEN_DIR)/beaglelogic-pru0.out
TARGET_PRU1=$(GEN_DIR)/beaglelogic-pru1.out

TARGETS=$(TARGET_PRU0) $(TARGET_PRU1)

MAP=$(GEN_DIR)/$(PROJ_NAME).map

SOURCES=main.c

OBJECTS_PRU0=$(GEN_DIR)/beaglelogic-pru0.object $(GEN_DIR)/beaglelogic-pru0-core.object

OBJECTS_PRU1=$(GEN_DIR)/beaglelogic-pru1.object $(GEN_DIR)/beaglelogic-pru1-core.object

all: printStart $(TARGETS) printEnd

printStart:
	@echo ''
	@echo '************************************************************'
	@echo 'Building project: $(PROJ_NAME)'

printEnd:
	@echo ''
	@echo 'Output files can be found in the "$(GEN_DIR)" directory'
	@echo ''
	@echo 'Finished building project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

# Invokes the linker (-z flag) to make the .out file
$(TARGET_PRU0): $(OBJECTS_PRU0) $(LINKER_COMMAND_FILE)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: PRU Linker'
	$(PRU_CGT)/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $(TARGET_PRU0) $(OBJECTS_PRU0) -m$(MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)
	@echo 'Finished building target: $@'

$(TARGET_PRU1): $(OBJECTS_PRU1) $(LINKER_COMMAND_FILE)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: PRU Linker'
	$(PRU_CGT)/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $(TARGET_PRU1) $(OBJECTS_PRU1) -m$(MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)
	@echo 'Finished building target: $@'

# Invokes the compiler on all c files in the directory to create the object files
$(GEN_DIR)/%.object: %.c
	@mkdir -p $(GEN_DIR)
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	$(PRU_CGT)/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

$(GEN_DIR)/%.object: %.asm
	@mkdir -p $(GEN_DIR)
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	$(PRU_CGT)/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

.PHONY: all clean

# Remove the $(GEN_DIR) directory
clean:
	@echo ''
	@echo '************************************************************'
	@echo 'Cleaning project: $(PROJ_NAME)'
	@echo ''
	@echo 'Removing files in the "$(GEN_DIR)" directory'
	@rm -rf $(GEN_DIR)
	@echo ''
	@echo 'Finished cleaning project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(OBJECTS_PRU0:%.object=%.pp)
-include $(OBJECTS_PRU1:%.object=%.pp)

# Deployment commands
install: deploy
deploy: deploy-pru0 deploy-pru1

deploy-pru0: $(TARGET_PRU0)
	@echo ''
	@echo 'Installing PRU0 firmware to /lib/firmware'
	@cp -v $(TARGET_PRU0) /lib/firmware/beaglelogic-pru0-fw
	@echo ''

deploy-pru1: $(TARGET_PRU1)
	@echo ''
	@echo 'Installing PRU1 firmware to /lib/firmware'
	@cp -v $(TARGET_PRU1) /lib/firmware/beaglelogic-pru1-logic
	@echo 'Symlinking beaglelogic-pru1-logic to beaglelogic-pru1-fw'
	@ln -sfv /lib/firmware/beaglelogic-pru1-logic /lib/firmware/beaglelogic-pru1-fw
	@echo ''
