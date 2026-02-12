#
# Copyright 2024 Broadcom Limited.  All rights reserved.
# 
# Broadcom Proprietary and Confidential
# 
###################################################################################

##  @defgroup grp_dma330drv Direct Memory Access 330 (DMA-330) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_dma330drv
#   @{
#   @file drivers/dma330/comp.mk
#   @brief Makefile for DMA330 Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := dma330drv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib


BRCM_COMP_REQ_SRC += doc/dma330_req.c
