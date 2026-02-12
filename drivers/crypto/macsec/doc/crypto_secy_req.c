/*****************************************************************************
 Copyright 2022 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_crypto_secy_req Requirements
    @ingroup grp_macsecdrv
    @addtogroup grp_crypto_secy_req
    @{

    @file crypto_secy_req.c
    @brief Requirements
    This file contains the requirements for Crypto Abstractions
    @version 0.1 Changelist0
*/

/**
    @brief Provide abstraction for Crypto related services

    Provide abstractions for Crypto related services including
    -# Algorithm selection
    -# Signature generation
    -# Signature verification
    -# Encryption
    -# Decryption
    -# Adding/Deleting keys

    @rationale
    This layer assists in retaining the rest of the system independent of where
    the crypto service is running (Software, hardware, internal/external IP etc)

    @analysis
    EVITA provides synchronous interface for most of the functionality needed for
    this requirement. However, Asyncronous design is needed to reduce the number
    of wait points and ease debugging based on state machine.
    Most the architecture follows EVITA with little extension of functions and
    arguments to meet Asyncronous design.
    This being a thin layer of abstraction, it is not expected to exceed 4 KB of
    code and is not expected to have any data memory usage. CPU usage is also
    very small, in the order of 500 cycles per function call (depends on how many
    calls are made).
    <br>To enable parallel processing of multiple verifications, signature generations etc,
    Message Queues with multiple contexts are used.

    @alternate_arch
    Use a synchronous design with explicit wait points in each function. But, this
    is not taken to keep the design consistent with the rest of the components/modules
    in the system

    @verification
        -# Able to create and destroy multiple contexts for crypto related functionality
        -# Configurability for number of contexts at build time

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER

*/
#define BRCM_SWREQ_SECY

/** @} */
