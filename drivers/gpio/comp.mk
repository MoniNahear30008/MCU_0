#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_gpiodrv General Purpose Input/Output (GPIO) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_gpiodrv
#   @{
#   @file drivers/gpio/comp.mk
#   @brief Makefile for GPIO Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := gpiodrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/gpio_req.c