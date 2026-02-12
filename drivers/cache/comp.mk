#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_cachedrv Coda Cache Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_cachedrv
#   @{
#   @file drivers/cache/comp.mk
#   @brief Makefile for cache Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := cachedrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/cache_req.c
