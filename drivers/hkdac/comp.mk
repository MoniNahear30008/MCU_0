#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
###################################################################################

##  @defgroup grp_hkdacdrv House Keeping Digital to Analog Converter (HKDAC) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_hkdacdrv
#   @{
#   @file drivers/hkdac/comp.mk
#   @brief Makefile for HKDAC Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := hkdacdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/hkdac_req.c
