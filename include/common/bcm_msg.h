/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

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
    @addtogroup grp_abstract_comp
    @{

    @file bcm_msg.h
    @brief BCM Message Identifiers
    This header file contains the message identifiers
    @version 0.1 Initial Version
*/


#ifndef BCM_MSG_H
#define BCM_MSG_H

#include <stdint.h>
#include <stddef.h>

/**
    @name BCM Component API IDs
    @{
    @brief API IDs for BCM Message APIs
*/
#define BRCM_SWARCH_BCM_MSG_ASYNC_MACRO            (0x8D01U)    /**< @brief #BCM_MSG_ASYNC     */
#define BRCM_SWARCH_BCM_MSG_MACRO                  (0x8D02U)    /**< @brief #BCM_MSG           */
#define BRCM_SWARCH_BCM_MSG_GET_GROUP_MACRO        (0x8D03U)    /**< @brief #BCM_MSG_GET_GROUP */
#define BRCM_SWARCH_BCM_MSG_GET_COMP_MACRO         (0x8D04U)    /**< @brief #BCM_MSG_GET_COMP  */
#define BRCM_SWARCH_BCM_MSG_GET_ID_MACRO           (0x8D05U)    /**< @brief #BCM_MSG_GET_ID    */
#define BRCM_SWARCH_BCM_MSG_TYPE                   (0x8D06U)    /**< @brief #BCM_MsgType       */
/** @} */


/**
    @brief System Command ID macros

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
typedef uint32_t BCM_MsgType;
#define BCM_MSG_ID_SHIFT                (0UL)                           /**< @brief Command ID shift value  */
#define BCM_MSG_ID_MASK                 (0xFFUL << BCM_MSG_ID_SHIFT)    /**< @brief Command ID shift mask   */
#define BCM_MSG_COMP_SHIFT              (8UL)                           /**< @brief Component ID shift value*/
#define BCM_MSG_COMP_MASK               (0xFFFFUL << BCM_MSG_COMP_SHIFT)/**< @brief Component ID shift mask */
#define BCM_MSG_GROUP_SHIFT             (24UL)                          /**< @brief Group ID shift value    */
#define BCM_MSG_GROUP_MASK              (0x3FUL << BCM_MSG_GROUP_SHIFT) /**< @brief Group ID shift mask     */
#define BCM_MSG_RESPONSE_SHIFT          (30UL)                          /**< @brief Resp bit Shift value    */
#define BCM_MSG_RESPONSE_MASK           (1UL << BCM_MSG_RESPONSE_SHIFT) /**< @brief Async bit Shift mask    */
#define BCM_MSG_ASYNC_SHIFT             (31UL)                          /**< @brief Async bit Shift value   */
#define BCM_MSG_ASYNC_MASK              (1UL << BCM_MSG_ASYNC_SHIFT)    /**< @brief Async bit Shift mask    */
#define BCM_MSG_MAGIC_CMD               (0xa5a5a5a5UL)                  /**< @brief Command magic           */
#define BCM_MSG_MAGIC_RESP              (0x5a5a5a5aUL)                  /**< @brief Response magic          */

/**
    @brief Macro to compose the command ID for a async notification

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
#define BCM_MSG_ASYNC(aGrp, aComp, aId) ((BCM_MSG_ASYNC_MASK)                                    \
                                          | (((uint32_t)(aGrp) << BCM_MSG_GROUP_SHIFT) & BCM_MSG_GROUP_MASK) \
                                          | (((aComp) << BCM_MSG_COMP_SHIFT) & BCM_MSG_COMP_MASK)  \
                                          | (((aId) << BCM_MSG_ID_SHIFT) & BCM_MSG_ID_MASK))

/**
    @brief Macro to compose the entire command ID

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
#define BCM_MSG(aGrp, aComp, aId) ((((uint32_t)(aGrp) << BCM_MSG_GROUP_SHIFT) & BCM_MSG_GROUP_MASK)  \
                                        |(((uint32_t)(aComp) << BCM_MSG_COMP_SHIFT) & BCM_MSG_COMP_MASK)   \
                                        |(((uint32_t)(aId) << BCM_MSG_ID_SHIFT) & BCM_MSG_ID_MASK))

/**
    @brief Macro to get the group ID for the command

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
#define BCM_MSG_GET_GROUP(aCmd) (((aCmd) & BCM_MSG_GROUP_MASK)  >> BCM_MSG_GROUP_SHIFT)

/**
    @brief Macro to get the component for the command

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
#define BCM_MSG_GET_COMP(aCmd) (((aCmd) & BCM_MSG_COMP_MASK) >> BCM_MSG_COMP_SHIFT)

/**
    @brief Macro to get the command type

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
#define BCM_MSG_GET_ID(aCmd) (((aCmd) & BCM_MSG_ID_MASK)  >> BCM_MSG_ID_SHIFT)

#endif /* BCM_MSG_H */
/** @} */
