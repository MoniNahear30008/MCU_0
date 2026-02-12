#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_ccudrv Capture and Compare Unit (CCU) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_ccudrv
#   @{
#   @file drivers/ccu/comp.mk
#   @brief Makefile for CCU Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := ccudrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/ccu_req.c
