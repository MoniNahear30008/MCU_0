/*****************************************************************************
 Copyright 2017-2023 Broadcom Limited.  All rights reserved.

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

/**
    @defgroup grp_utils_crc CRC
    @ingroup grp_abstract

    @addtogroup grp_utils_crc
    @{

    @file arch_crc.h
    @brief CRC functions

    @version 0.2.0 Modifications as per new convention
    @version 0.1.0 Initial version
*/

#ifndef ARCH_CRC_H
#define ARCH_CRC_H

#include <stddef.h>
#include <inttypes.h>

/**
    @name CRC Architecture IDs
    @{
    @brief Architecture IDs for CRC

*/
#define BRCM_SWARCH_BCM_CRC32_PROC           (0x8801U)    /**< @brief #BCM_CRC32       */
#define BRCM_SWARCH_BCM_CRC32_CUSTOM_PROC    (0x8802U)    /**< @brief #BCM_CRC32Custom */
#define BRCM_SWARCH_BCM_CHKSUM_PROC          (0x8803U)    /**< @brief #BCM_Chksum      */
#define BRCM_SWARCH_BCM_CRC16_PROC           (0x8804U)    /**< @brief #BCM_CRC16       */
#define BRCM_SWARCH_BCM_CRC32_POLY_MACR1     (0x8805U)    /**< @brief #BCM_CRC32_POLY  */
/** @} */

/**
    @brief Polynomial for CRC generation

    @trace #BRCM_SWREQ_BCM_UTILS
*/
#define BCM_CRC32_POLY          (0x04C11DB7UL)

/** @brief CRC 32 calculation

    This API calculates 32-bit CRC for provided input data

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Starting address from which CRC has to be computed
    @param[in]      aLen        Size in bytes for which CRC has to be computed
    @param[in]      aPoly       Polynomial to be used in calculation

    @return
        + CRC value for provided data
        + 0xFFFFFFFFUL: If invalid (NULL) pointer to starting address, @c aAddr
        + 0xFFFFFFFFUL: If size in bytes is 0UL, @c aLen

    @post None

    @limitations None

    @trace #BRCM_SWREQ_BCM_UTILS
*/
uint32_t BCM_CRC32(const uint8_t *const aAddr, uint32_t aLen, uint32_t aPoly);

/** @brief Custom CRC 32 calculation

    This API calculates 32-bit CRC with some input initialization vector. Initial 32-bit CRC calculation start with @c aInitVal.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Starting address from which CRC has to be computed
    @param[in]      aLen        Size in bytes for which CRC has to be computed
    @param[in]      aPoly       Polynomial to be used in calculation
    @param[in]      aInitVal    Initialization vector

    @return
        + CRC value for provided data
        + aInitVal: If invalid (NULL) pointer to starting address, @c aAddr
        + aInitVal: If size in bytes is 0UL, @c aLen

    @post None

    @limitations None

    @trace #BRCM_SWREQ_BCM_UTILS
*/
uint32_t BCM_CRC32Custom(const uint8_t *const aAddr, uint32_t aLen, uint32_t aPoly, uint32_t aInitVal);

/** @brief CRC 16 calculation

    This API calculates 32-bit CRC with some input initialization vector. Initial 16-bit CRC calculation start with @c aInitVal.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Starting address from which CRC has to be computed
    @param[in]      aLen        Size in bytes for which CRC has to be computed
    @param[in]      aPoly       Polynomial to be used in calculation
    @param[in]      aInitVal    Initialization vector

    @return
        + CRC value for provided data
        + 0U: If size in bytes is 0UL, @c aLen

    @post None

    @limitations The pointer @c aAddr shall point to a readable memory.

    @trace #BRCM_SWREQ_BCM_UTILS
*/
extern uint16_t BCM_CRC16(const uint8_t *const aAddr, uint32_t aLen, uint16_t aPoly, uint16_t aInitVal);

/** @brief Checksum calculation

    This API calculates 16-bit checksum for provided input data

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Starting address from which check sum has to be computed
    @param[in]      aLen        Size in bytes for which check sum has to be computed

    @return
        + Check sum value for provided data
        + 0xFFFFU: If size in bytes is 0UL, @c aLen

    @post None

    @limitations The pointer @c aAddr shall point to a readable memory.

    @trace #BRCM_SWREQ_BCM_UTILS
*/
uint16_t BCM_Chksum(const uint8_t *const aAddr, uint32_t aLen);

#endif /* ARCH_CRC_H */
/** @} */
