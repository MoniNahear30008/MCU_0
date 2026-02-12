#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_sdcdrv SDC Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_sdcdrv
#   @{
#   @file drivers/sdc/comp.mk
#   @brief Makefile for sdc Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := sdcdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/sdc_req.c
