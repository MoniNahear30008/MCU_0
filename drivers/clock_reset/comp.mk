####################################################################################
# Copyright 2024 Broadcom.  All rights reserved.
# Broadcom Proprietary and Confidential.
####################################################################################

##  @defgroup grp_clkrstdrv Clock and reset Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_clkrstdrv
#   @{
#   @file drivers/clock_reset/comp.mk
#   @brief Makefile for clock and reset Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := clockresetdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/clock_reset_req.c
