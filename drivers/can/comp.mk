#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################


##  @defgroup grp_candrv Controller Area Network (CAN) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_candrv
#   @{
#   @file drivers/can/comp.mk
#   @brief Makefile for CAN Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := candrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/can_req.c
