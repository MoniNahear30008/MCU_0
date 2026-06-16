#
# $Id$
# $Copyright: (c) 2025 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# This reference Makefile uses Armgcc 2021 options.

#architecture
architecture = $(arch)

TOOLCHAIN_PATH?=

TMPFLAG = -DARMCC
# Output binary
OUTPUT_DIR = ./bin
BMD_DIR =..
ARM_HEADER_PATH=./arm_prog_common

LIB = build/liblidar.a
LDSCRIPT = firmware.ld

# Create output directory if not available
$(shell mkdir -p $(OUTPUT_DIR))

ifeq ($(architecture),mcu-x86)
    $(info 'Compiling MCU package with GCC compiler...')
    CC=gcc
    CFLAGS = -Os -O3 -Wall -Werror -Wno-error=address -Wno-int-to-pointer-cast -Wno-error=address -Wno-pointer-to-int-cast -fdata-sections -ffunction-sections
    CFLAGS += -D__x86__

all:
	$(CC) test.c -o $(OUTPUT_DIR)/test $(INC_LIBS) $(ADD_FLAGS)

else
    $(info 'Compiling MCU package...')
    # Compiler
    CC =$(TOOLCHAIN_PATH)/bin/arm-none-eabi-gcc
    LD =$(TOOLCHAIN_PATH)/bin/arm-none-eabi-g++ -Wl,--gc-sections -Wl,--print-memory-usage -e
    AR =$(TOOLCHAIN_PATH)/bin/arm-none-eabi-gcc-ar
    ARFLAGS= -rcs
    ELFCON=$(TOOLCHAIN_PATH)/bin/arm-none-eabi-objcopy
    NM=$(TOOLCHAIN_PATH)/bin/arm-none-eabi-nm
    OBJDUMP =$(TOOLCHAIN_PATH)/bin/arm-none-eabi-objdump

    # Update library path
    LIBPATH =
    ADD_FLAGS = -mcpu=cortex-m7
endif
    LDFLAGS = -T $(LDSCRIPT) $(LIBPATH)

    ADD_FLAGS += -Os -std=c99 -g --inline --debug -Wall -Werror -Wno-error=address -Wno-int-to-pointer-cast -Wno-error=address -Wno-pointer-to-int-cast -mthumb -DSRAM_BOOT
    ADD_FLAGS += -D__GNU__ -D__GCC__ -D__CORTEX_MX__ -DGNU  -lc -lm -lrdimon
    ADD_FLAGS += -mcpu=cortex-m7+nofp.dp -Og -ggdb3 -ffreestanding -ffunction-sections -fdata-sections -MMD
    #include directory
    BMD_DIR_INC += $(BMD_DIR)/include
    INCDIR += -I $(BMD_DIR_INC)/i2c
    INCDIR += -I $(BMD_DIR_INC)/uart
#    INCDIR += -I $(BMD_DIR_INC)/dma330
#    INCDIR += -I $(BMD_DIR_INC)/pl08x
    INCDIR += -I $(BMD_DIR_INC)/gpio
#    INCDIR += -I $(BMD_DIR_INC)/ccu
    INCDIR += -I $(BMD_DIR_INC)/wdt
    INCDIR += -I $(BMD_DIR_INC)/hkadc
    INCDIR += -I $(BMD_DIR_INC)/hkdac
    INCDIR += -I $(BMD_DIR_INC)/hsadc
    INCDIR += -I $(BMD_DIR_INC)/hsref
    INCDIR += -I $(BMD_DIR_INC)/hsafe
    INCDIR += -I $(BMD_DIR_INC)/qspi
    INCDIR += -I $(BMD_DIR_INC)/common
    INCDIR += -I $(BMD_DIR_INC)/acqcmn
    INCDIR += -I $(BMD_DIR_INC)/cache
    INCDIR += -I $(BMD_DIR_INC)/clock_reset
    INCDIR += -I $(BMD_DIR_INC)/crypto/macsec
    INCDIR += -I $(BMD_DIR_INC)/ipc
    INCDIR += -I $(ARM_HEADER_PATH)
    INCDIR += -I .

    BLDDIR = $(PWD)
    
    # Libraries used for the application
    INC_LIBS += $(BMD_DIR)/$(LIB) -lc -lm  -lc -lrdimon -lgcc -specs=nano.specs -specs=nosys.specs -mcpu=cortex-m7+nofp.dp
    ELFOBJS = $(OUTPUT_DIR)/startup.o $(OUTPUT_DIR)/exceptions.o $(OUTPUT_DIR)/BCM8915X_BareMetal_helper.o
    INCLUDE_C_HELPER = BCM8915X_BareMetal_helper.c

all:output

output:
	$(info Compiling Common Modules ....)
	$(CC) startup.S $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/startup.o
	$(CC) exceptions.S $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/exceptions.o
	$(CC) $(INCLUDE_C_HELPER) $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/BCM8915X_BareMetal_helper.o

	$(info Compiling HSADC Application  ....)
	$(CC) Mcu_0.c $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/Mcu_0.o
	$(CC) evk_control.c $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/evk_control.o
	$(CC) DrvBrd.c $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/DrvBrd.o
	$(CC) globals.c $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/globals.o
	$(CC) q8_loader.c $(INCDIR) $(ADD_FLAGS) -c -o $(OUTPUT_DIR)/q8_loader.o
	$(LD) BCM_OS_RESET_HANDLER $(LDFLAGS) -o  $(OUTPUT_DIR)/Mcu_0.elf $(OUTPUT_DIR)/Mcu_0.o $(OUTPUT_DIR)/DrvBrd.o $(OUTPUT_DIR)/evk_control.o $(OUTPUT_DIR)/globals.o $(OUTPUT_DIR)/q8_loader.o $(ELFOBJS) $(INC_LIBS)
#	$(LD) BCM_OS_RESET_HANDLER $(LDFLAGS) -o  $(OUTPUT_DIR)/Mcu_0.elf $(OUTPUT_DIR)/Mcu_0.o $(OUTPUT_DIR)/evk_control.o $(ELFOBJS) $(INC_LIBS)

clean:
	rm -f $(OUTPUT_DIR)/*
