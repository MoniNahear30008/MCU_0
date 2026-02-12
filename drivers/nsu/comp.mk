#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_nsudrv Network Synchronization Unit (NSU) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_nsudrv
#   @{
#   @file drivers/nsu/comp.mk
#   @brief Makefile for NSU Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := nsudrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/nsu_req.c
