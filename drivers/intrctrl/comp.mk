#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_intrctrldrv Interrupt Controller Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_intrctrldrv
#   @{
#   @file drivers/intrctrl/comp.mk
#   @brief Makefile for Interrupt Controller Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := intrctrldrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/intrctrl_req.c
