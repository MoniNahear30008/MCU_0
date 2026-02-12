#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_i2cdrv Inter-Integrated Circuit (I2C) Bus Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_i2cdrv
#   @{
#   @file drivers/i2c/comp.mk
#   @brief Makefile for I2C Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := i2cdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/i2c_req.c
