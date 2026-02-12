architecture = $(arch)
PWD := $(CURDIR)
TOOLCHAIN_PATH=
TMPFLAG = -DARMCC
ifeq ($(architecture),x86)
    $(info 'Compiling MCU package with GCC compiler...')
    CC=gcc
    CFLAGS = -Os -O3 -Werror -Wno-error=address -Wno-int-to-pointer-cast -Wno-error=address -Wno-pointer-to-int-cast -fdata-sections -ffunction-sections
    CFLAGS += -D__x86__
else
    $(info 'Compiling MCU package...')
    CC=$(TOOLCHAIN_PATH)/bin/arm-none-eabi-gcc
    AR=$(TOOLCHAIN_PATH)/bin/arm-none-eabi-gcc-ar
    ARFLAGS= -rcs
#   CFLAGS = -O3 -Ospace --inline --gnu --feedback=$(linker_feedback_file) --debug -W --cpu=Cortex-M0plus
    CFLAGS = -Os -std=c11 --inline --debug -Werror -Wno-error=address -Wno-int-to-pointer-cast -Wno-error=address -Wno-pointer-to-int-cast
    CFLAGS += -D__GNU__ -D__GCC__ -D__CORTEX_MX__ -DGNU -lgcc -lc -lm -lrdimon
    CFLAGS += -fdata-sections -ffunction-sections -g
    CFLAGS += -mthumb -mthumb-interwork -Os -ggdb

	ifeq ($(comp_version),2015)
		CFLAGS += -Wall -Werror -I$(TOOLCHAIN_PATH)/arm-none-eabi/include -mcpu=cortex-m7
	else
		CFLAGS += -mcpu=cortex-m7+nofp.dp
	endif
endif

TMPFLAG += -DCAN_RXPKTQ_SIZE=32UL
TMPFLAG += -DCAN_ERRPKTQ_SIZE=32UL
TMPFLAG += -DMACSEC_MAX_PORT_NUM=2UL
TMPFLAG += -DETHXCVR_HW_ID_MAX=2UL
TMPFLAG += -DMACSEC_MAX_SECY_PER_PORT=2
TMPFLAG += -DMACSEC_MAX_RXSC_PER_SECY=2
TMPFLAG += -DMACSEC_TIMESTAMP_PRESCALE=125000UL

INCDIR += include/common
INCDIR += include/ccu
INCDIR += include/uart
INCDIR += include/i2c
INCDIR += include/dma330
INCDIR += include/pl08x
INCDIR += include/can
INCDIR += include/hsadc
INCDIR += include/acqcmn
INCDIR += include/hsref
INCDIR += include/txslzr
INCDIR += include/hkadc
INCDIR += include/awg
INCDIR += include/hkdac
INCDIR += include/qspi
INCDIR += include/gpio
INCDIR += include/wdt
INCDIR += include/ethernet/xgmac
INCDIR += include/ethernet/xgmac/mdio
INCDIR += include/ethernet/xgmac/ptp
INCDIR += include/ethernet/transceiver
INCDIR += drivers/ethernet/transceiver/rdb
INCDIR += drivers/crypto/macsec
INCDIR += include/crypto/macsec
INCDIR += include/clock_reset
INCDIR += include/cpu
INCDIR += include/unicam
INCDIR += include/cache
INCDIR += include/safetyisland
INCDIR += include/pcie
INCDIR += include/sdc
INCDIR += include/intrctrl
INCDIR += include/hsafe
INCDIR += include/nsu
INCDIR += include/ipc

VSRCS += $(wildcard drivers/ccu/*.c)
VSRCS += $(wildcard drivers/uart/*.c)
VSRCS += $(wildcard drivers/i2c/*.c)
VSRCS += $(wildcard drivers/dma330/*.c)
VSRCS += $(wildcard drivers/pl08x/*.c)
VSRCS += $(wildcard drivers/can/*.c)
VSRCS += $(wildcard drivers/awg/*.c)
VSRCS += $(wildcard drivers/hsadc/*.c)
VSRCS += $(wildcard drivers/acqcmn/*.c)
VSRCS += $(wildcard drivers/hsref/*.c)
VSRCS += $(wildcard drivers/txslzr/*.c)
VSRCS += $(wildcard drivers/hkadc/*.c)
VSRCS += $(wildcard drivers/hkdac/*.c)
VSRCS += $(wildcard drivers/qspi/*.c)
VSRCS += $(wildcard drivers/gpio/*.c)
VSRCS += $(wildcard drivers/wdt/*.c)
VSRCS += $(wildcard drivers/ethernet/xgmac/*.c)
VSRCS += $(wildcard drivers/ethernet/xgmac/mdio/*.c)
VSRCS += $(wildcard drivers/ethernet/xgmac/ptp/*.c)
VSRCS += $(wildcard drivers/ethernet/transceiver/*.c)
VSRCS += $(wildcard drivers/ethernet/transceiver/brphy/*.c)
VSRCS += $(wildcard drivers/ethernet/transceiver/s16up/*.c)
VSRCS += $(wildcard drivers/crypto/macsec/*.c)
VSRCS += $(wildcard drivers/clock_reset/*.c)
VSRCS += $(wildcard drivers/unicam/*.c)
VSRCS += $(wildcard drivers/cache/*.c)
VSRCS += $(wildcard drivers/safetyisland/*.c)
VSRCS += $(wildcard common/*.c)
VSRCS += $(wildcard drivers/pcie/*.c)
VSRCS += $(wildcard drivers/sdc/*.c)
VSRCS += $(wildcard drivers/intrctrl/*.c)
VSRCS += $(wildcard drivers/hsafe/*.c)
VSRCS += $(wildcard drivers/nsu/*.c)
VSRCS += $(wildcard drivers/ipc/*.c)

LOBJS = $(addsuffix .o, $(basename $(VSRCS)))
BRCM_ROOT_DIR := $(CURDIR)

# Coverage build related
ifeq ($(coverage),1)

else

# Output directory
BLDDIR = ./build

# Create Output directory if it is not created
$(shell mkdir -p $(BLDDIR))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))
$(info Updated Object : $(BOBJS))
endif

TEMP = $(addprefix -I, $(INCDIR))
INCDIR := $(TEMP)

# Create Output directory if it is not created
BLDDIR = $(PWD)
LOCALDIR = $(PWD)
LIBNAME = liblidar
LIBSUFFIX = a
MV = mv
RM = rm
CPPFLAGS = $(INCDIR)
ECHO = echo

$(BLDDIR)/%.o: %.c
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(CC) $(LOCALDIR)/$< $(CPPFLAGS) $(CFLAGS) $(TMPFLAG) -c -o $@

$(BLDDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) 
	@$(ECHO) 'Building library $(LIBNAME)...'
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJS)
	$(MV) $(BOBJS) $(BLDDIR)/build
	$(MV) $(LIBNAME).$(LIBSUFFIX) $(BLDDIR)/build

BLIBNAME = $(BLDDIR)/$(LIBNAME).$(LIBSUFFIX)

all:: $(BLIBNAME)

clean::
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(RM) -rf build/*.o build/*.a $(BOBJS) $(BLIBNAME) 