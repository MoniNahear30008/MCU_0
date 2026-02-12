#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_pciedrv PCIe Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_pciedrv
#   @{
#   PCIe Driver provides APIs to initialize and configure
#   PCIe endpoint or root complex device.
#   It supports configuration of the device enumaration parameters
#   BAR spaces and RC Spaces,
#   enabling and disabling of the PCIe interface.
#
#
#   @file drivers/pcie/comp.mk
#   @brief Makefile for PCIe Driver component
#
#   @version 0.1 Initial version
#   @}
#

BRCM_COMP_NAME    := pciedrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE    := lib

BRCM_COMP_REQ_SRC    += doc/pciedrv_req.c
