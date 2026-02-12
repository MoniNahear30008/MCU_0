/*****************************************************************************
 Copyright 2022-2023 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/** @defgroup grp_dma_v2_req Requirements
    @ingroup grp_dma_v2
    @addtogroup grp_dma_v2_req
    @{

    @file dma_v2_req.c
    @brief DMA V2 Driver Requirements

    @version 0.1 Initial Version
*/

/**
    @brief DMA V2 Driver requirement

    DMA V2 Driver shall support following features
    -# Multiple DMA instance
    -# Multiple DMA channels (shall be managed internally)
    -# DMA request chaining
    -# Auto configure DMA parameters from source destination address
        (flow control, peripheral id, bur tsize, etc.)
    -# 2D DMA (on supported platforms)

    @rationale
    Different platforms support different number of DMA instance and
    different DMA IP.
    Unified DMA interface is needed for components to access the DMA.

    @analysis
    Common APIs shall be provided for Init, Deinit, Start transfer and get
    transfer status.
    DMA Ips specific drivers shall be implemented in separate file and
    included based on platform (in the comp.mk)
    A platform specific

    @verification
    -# Able to transfer data from memory to memory, memory to peripheral
    -# Able to start simultaneous transfers with multiple channels
    -# Able to start simultaneous transfers with multiple DMA instance

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
*/
#define BRCM_SWREQ_DMA_V2_DRIVER

/**
    @brief DMA V2 driver kernel interface

    DMA V2 driver shall implement a function in osil layer
    to do the context switch to the privileged mode and to pass
    on the required arguments to the low level driver

    @rationale
    Needed for executing the DMA V2 driver functions from privileged context.
    Driver APIs are expected to be executed in unprivileged space, and
    driver internally needs to switch to privileged space for access to
    hardware registers and driver data structures. This should act as a
    bridge to feed the data from unprivileged space to privileged space.

    @analysis
    Following commands to be defined
    -# Init
    -# Deinit
    -# Start DMA transfer
    -# Get DAM transfer status

    Following parameters to be defined for the privileged layer call
    -# DMA Instance number
    -# Source address
    -# Destination address
    -# Row Size
    -# Number of Rows
    -# Source skip
    -# Destination skip

    @verification
    -# Review commands and data structures for driver interface completeness.
    -# Able to initialize/deinitialize DMA V2 driver
    -# Able to Start transfer and get status

    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
*/
#define BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE

/**
    @brief DMA V2 driver kernel handler

    DMA V2 diver shall implement a handler function that shall be invoked
    from the privileged mode (kernel space)

    @rationale
    Needed for executing the DMA V2 driver from privileged context.
    This needs to be installed in SVC function table to enable switching to
    privileged space and process client commands.

    @analysis
    Command handler should support handling of init, deinit, start transfer functions.
    This layer (including other requirements in this file) is expected to consume
    less than 1 KB of code and 1 KB of data.

    @verification
    -# Review command handler consistency for integration to SVC function tables.
    -# Able to initialize deinitialize the driver
    -# Able to Start transfer and get status

    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_DMA_V2_DRIVER
    @dependencies #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION

    @vtrace #DMA_EVENT
*/
#define BRCM_SWREQ_DMA_V2_KERNEL_HANDLER


/** @} */
