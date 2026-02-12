#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#     
# Broadcom Proprietary and Confidential
#         
####################################################################################
##  @defgroup grp_wdtdrv Watchdog Timer (WDT) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_wdtdrv
#   @{
#   @file drivers/wdt/comp.mk
#   @brief Makefile for WDT Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := wdtdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/wdt_req.c
