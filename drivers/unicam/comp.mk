####################################################################################
# Copyright 2024 Broadcom.  All rights reserved.
# Broadcom Proprietary and Confidential.
####################################################################################


##  @defgroup grp_unicamdrv MIPI Camera Serial Interface (UNICAM) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_unicamdrv
#   @{
#   @file drivers/unicam/comp.mk
#   @brief Makefile for unicam Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := unicamdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/unicam_req.c
