#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_hsafedrv High speed analog front end (HSAFE) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_hsafedrv
#   @{
#   @file drivers/hsafe/comp.mk
#   @brief Makefile for HSAFE Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := hsafedrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/hsafe_req.c
