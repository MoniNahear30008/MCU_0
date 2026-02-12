#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
###################################################################################

##  @defgroup grp_acqcmndrv Acquisition Common (ACQCMN) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_acqcmndrv
#   @{
#   @file drivers/acqcmn/comp.mk
#   @brief Makefile for ACQCMN Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := acqcmndrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/acqcmn_req.c
