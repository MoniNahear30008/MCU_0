#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_safetyislanddrv Safety island (SAFETYISLAND) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_safetyislanddrv
#   @{
#   @file drivers/safetyisland/comp.mk
#   @brief Makefile for SAFETYISLAND Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := safetyislanddrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/safetyisland_req.c
