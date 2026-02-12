#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_dma_v2 Peripheral Direct Memory Access (PL08X) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_dma_v2
#   @{
#   @file drivers/pl08x/comp.mk
#   @brief Makefile for PL08X V2 Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := dma 
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/dma_v2_req.c
BRCM_COMP_REQ_SRC += doc/dma_v2_pl08x_req.c
#BRCM_COMP_REQ_SRC += doc/dma_v2_pl08x_plat_req.c

