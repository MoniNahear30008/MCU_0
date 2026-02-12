#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
###################################################################################


##  @defgroup grp_transceiverdrv Transceiver Driver
#   @ingroup grp_ethernet
#
#   @addtogroup grp_transceiverdrv
#   @{
#   @file drivers/ethernet/transceiver/comp.mk
#   @brief Makefile for Transceiver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := transceiverdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/xcvr_req.c
