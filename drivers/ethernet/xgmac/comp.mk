#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
###################################################################################


##  @defgroup grp_xgmacdrv Ethernet MAC (XGMAC) Driver
#   @ingroup grp_ethernet
#
#   @addtogroup grp_xgmacdrv
#   @{
#   @file drivers/ethernet/xgmac/comp.mk
#   @brief Makefile for XGMAC Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := xgmacdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/xgmac_req.c
BRCM_COMP_REQ_SRC += ptp/doc/ptp_req.c
BRCM_COMP_REQ_SRC += mdio/doc/mdio_req.c
