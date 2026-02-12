#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_ipcdrv Inter-Processor communication (IPC) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_ipcdrv
#   @{
#   @file drivers/ipc/comp.mk
#   @brief Makefile for the IPC Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME        := ipcdrv
BRCM_COMP_DEPENDS     :=
BRCM_COMP_TYPE        := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += doc/ipc_req.c
