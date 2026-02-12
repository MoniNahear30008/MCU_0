#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
###################################################################################

##  @defchapter grp_ethernet Ethernet
#   @addtogroup grp_ethernet
#   @{
#
#   @file drivers/ethernet/comp.mk
#   @brief Define architecture
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := ethernet
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += ethernet_req.c