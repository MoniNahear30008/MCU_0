#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_awgdrv Arbitrary Waveform Generator (AWG) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_awgdrv
#   @{
#   @file drivers/awg/comp.mk
#   @brief Makefile for AWG Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := awgdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/awg_req.c
