#
# Copyright 2024 Broadcom Limited.  All rights reserved.
# 
# Broadcom Proprietary and Confidential
# 
###################################################################################

##  @defgroup grp_hsrefdrv High Speed Reference-Channel ADC (HSREF) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_hsrefdrv
#   @{
#   @file drivers/hsref/comp.mk
#   @brief Makefile for HSREF Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := hsrefdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/hsref_req.c
