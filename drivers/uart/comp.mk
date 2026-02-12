#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_uartdrv Universal Asynchronous Receiver/Transmitter (UART) Driver
#   @ingroup grp_driver
#
#   @addtogroup grp_uartdrv
#   @{
#   @file drivers/uart/comp.mk
#   @brief Makefile for UART Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := uartdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/uart_req.c
