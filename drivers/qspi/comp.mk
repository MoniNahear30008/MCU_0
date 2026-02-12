#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_qspidrv Quad Serial Peripheral Interface (QSPI) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_qspidrv
#   @{
#   @file drivers/qspi/comp.mk
#   @brief Makefile for QSPI Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := qspidrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/qspi_req.c
