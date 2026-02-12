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
    @defgroup grp_abstract_err Peripheral Drivers Return Codes
    @ingroup grp_appendix

    @addtogroup grp_abstract_err
    @{
    All Components use these errors as error return types.
    <BR>All Components use BCM_ReportError function for reporting errors.

    @file bcm_err.h
    @brief BCM Error Types
    This header file contains the interface functions for error types
    and error reporting function.
    @version 0.1 Initial Version
*/


#ifndef BCM_ERR_H
#define BCM_ERR_H

#include <stdint.h>
#include <stddef.h>
#include <bcm_comp.h>
/**
    @name BCM Error API IDs
    @{
    @brief API IDs for BCM Error APIs
*/
#define BRCM_SWARCH_BCM_ERR_TYPE                (0x8001U)    /**< @brief #BCM_ErrorType                 */
/** @} */

/**
    @name BCM Error Codes
    @{
    @brief Error return values

    @trace #BRCM_SWREQ_BCM_COMPONENT
*/
typedef int32_t BCM_ErrorType;
#define BCM_ERR_OK              (0x0)  /**< @brief No Error                          */
#define BCM_ERR_BUSY            (0x1)  /**< @brief Device or resource busy           */
#define BCM_ERR_NODEV           (0x2)  /**< @brief No device found                   */
#define BCM_ERR_NOT_FOUND       (0x3)  /**< @brief Not Found                         */
#define BCM_ERR_NOMEM           (0x4)  /**< @brief Out of memory                     */
#define BCM_ERR_NOSUPPORT       (0x5)  /**< @brief Not supported                     */
#define BCM_ERR_INVAL_PARAMS    (0x6)  /**< @brief Invalid argument                  */
#define BCM_ERR_INVAL_MAGIC     (0x7)  /**< @brief Invalid magic number              */
#define BCM_ERR_INVAL_STATE     (0x8)  /**< @brief Invalid state                     */
#define BCM_ERR_INVAL_BUF_STATE (0x9)  /**< @brief Invalid buffer state              */
#define BCM_ERR_EAGAIN          (0xA)  /**< @brief Try again                         */
#define BCM_ERR_TIME_OUT        (0xB)  /**< @brief Timeout                           */
#define BCM_ERR_UNINIT          (0xC)  /**< @brief Device or resource not initialized*/
#define BCM_ERR_CANCELLED       (0xD)  /**< @brief Cancel request success            */
#define BCM_ERR_DATA_INTEG      (0xE)  /**< @brief Data integrity error              */
#define BCM_ERR_AUTH_FAILED     (0xF)  /**< @brief Authentication error              */
#define BCM_ERR_VERSION         (0x10) /**< @brief Wrong version of hw/sw            */
#define BCM_ERR_BUS_FAILURE     (0x11) /**< @brief Bus Failure                       */
#define BCM_ERR_NACK            (0x12) /**< @brief NACK error                        */
#define BCM_ERR_MAX_ATTEMPS     (0x13) /**< @brief Maximum num of attempts           */
#define BCM_ERR_UNKNOWN         (0x14) /**< @brief Unknown error                     */
#define BCM_ERR_CUSTOM          (0x15) /**< @brief Module specific error             */
#define BCM_ERR_NOPERM          (0x16) /**< @brief Permission denied                 */
/** @} */
#endif /* BCM_ERR_H */
/** @} */
