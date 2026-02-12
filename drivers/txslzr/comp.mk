#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
###################################################################################

##  @defgroup grp_txslzrdrv TX Serializer (TXSLZR) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_txslzrdrv
#   @{
#   @file drivers/txslzr/comp.mk
#   @brief Makefile for TXSLZR Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := txslzrdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/txslzr_req.c
