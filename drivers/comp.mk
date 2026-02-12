#
# Copyright 2024 Broadcom.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defchapter grp_system Introduction
#
#   @addtogroup grp_system
#   @{
#   <b><u> About this Document </u></b>
#
#   This document is a software user guide for BCM8915X Bare Metal SW drivers. The main purpose of this document
#   is to help user to understand the Bare Metal SW Driver and its usage. The Bare Metal Driver is platform agnostic
#   so that it can be easily ported to customer platform.
#
#   <b><u> Prerequisites </u></b><br>
#    The user must have some prerequisite knowledge to use Bare Metal Drivers effectively. The following
#    documents can be referred to get more information on BCM8915X peripherals.
#        -# BCM8915X DataSheet [TBD]
#        -# Technical Reference Manual [TBD]
#
#
#   <b><u> Intended Audience </u></b>
#
#   This document is intended to provide guidelines to platform software developer (SDK Developers) to integrate
#   and use Bare Metal driver interface.
#   This document describes BCM8915X Bare Metal driver programming sequence involved in peripheral initialization
#   and configuration sequences. This document does not fully describe BCM8915X hardware features.
#   For specific details please refer to BCM8915X Data Sheet [TBD].
#
#
#   <b><u> Bare Metal Driver Software </u></b>
#
#   Bare Metal software, delivered as RIGL_X_Y_Z.tgz, is a collection of peripheral APIs.
#   A Bare Metal Driver provides an interface to customer SDK for accessing BCM8915X chip.
#   The APIs are delivered as C source code and can be integrated to support user proprietary software.
#
#   Bare Metal driver provides APIs for,
#       -# Peripheral Initialization and DeInitialization.
#       -# Peripheral Configuration APIs.
#       -# Peripheral data transfer (Send/receive) APIs.
#       
#   @note
#
#       Multiple instance of same peripheral can be accessed based on hwID. Refer to corresponding driver section for details.
#
#   Bare Metal Driver user specification will not cover complete BCM8915X SoC. Please see the list of supported peripherals in section 3.
#
#   <b><u>Guidelines and Recommendations for Integration</u></b>
#    The following guideline needs to be followed to integrate a Bare Metal Driver in customer platforms.
#   -# Bare Metal driver APIs are non-reentrant. SDK integration is expected to handle multithreading requirements in SDK.
#   -# SDK is expected to protect or call APIs as per HW specified access mechanisms.
#   -# Wherever applicable, the driver provides API support for reads and writes in interrupt based Peripheral IO (PIO) mode or DMA transfer mode depending on
#      peripheral support.
#   -# Wherever applicable, if DMA transfer is needed, user is expected to preconfigure associated DMA channel and enable DMA request on the 
#      peripheral at the end.
#   -# Interrupt handler and callback function implementation are the resposibility of the integration layer. It can be modified or optimized by the
#      platform SW/SDK depending on the use case.
#   -# SDK integration can extend driver APIs for multiple HW instantiations with HwID. Detailed information is mentioned as part of the specific driver
#      description.  
#      Peripheral controller base address is accessed based on index HwID passed as 0 to <PERIPH>_MAX_HW_ID - 1,
#      depending on HW instance.
#
#       | HwID        | HW Instance   |
#       |:-----------:|:-------------:|
#       | 0           |   <PERIPH>_0  |
#       | 1           |   <PERIPH>_1  |
#
#   @}

#
#   @defchapter grp_system1 Architecture
#   @{
#   @addtogroup grp_system1
#   @{
#   A Bare Metal Driver provides a hardware abstraction layer as Layer 1 APIs to access hardware registers for
#   initialization and configuration of the peripherals of the SoC.
#
#   @image html bmd_architecture_diagram.png "Bare Metal Driver SW hierarchy" width="200%"
#
#   @section sec_bmd_integration Integration Guide
#   -# API documentation for each peripheral and guideline for its usage as applicable.
#   @}
#   @}
#   @defchapter grp_driver Peripheral Drivers
#
#   @addtogroup grp_driver
#   @{
#   Peripheral Driver interface to user SDK
#   <table>
#   <tr> <th> Peripheral Name </th><th>Class</th> <th>Mode of Transfer</th><th>Driver Name</th><th>Driver API Description</th></tr>
#   
#   </table>
#   @file system/abstract/comp.mk
#   @brief Makefile for System Abstractions
#   @version 0.1 Initial version
#
#   @}
#   @defchapter grp_ethernet Ethernet 
#   @{
#   @}
#   @defchapter grp_appendix Appendix
#   @{
#   @}

BRCM_COMP_NAME := system
BRCM_COMP_DEPENDS := module crypto ptu socket rpc nvm
BRCM_COMP_TYPE := lib
