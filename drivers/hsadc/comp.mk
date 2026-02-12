#
# Copyright 2024 Broadcom Limited.  All rights reserved.
# 
# Broadcom Proprietary and Confidential
# 
###################################################################################

##  @defgroup grp_hsadcdrv High Speed Analog to Digital Converter (HSADC) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_hsadcdrv
#   @{
#   @file drivers/hsadc/comp.mk
#   @brief Makefile for HSADC Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := hsadcdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/hsadc_req.c
