/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

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
    @file s16up_tx_pcs_rdb.h
    @brief RDB File for S16UP_TX_PCS

    @version BCM89580_A0_20230125_RDB
*/

#ifndef S16UP_TX_PCS_RDB_H
#define S16UP_TX_PCS_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t S16UP_TX_PCS_RESERVED_TYPE;




typedef uint16_t S16UP_TX_PCS_TX_ACONTROL0_TYPE;
#define S16UP_TX_PCS_TX_ACONTROL0_POL_FLIP_MASK (0x20U)
#define S16UP_TX_PCS_TX_ACONTROL0_POL_FLIP_SHIFT (5U)
#define S16UP_TX_PCS_TX_ACONTROL0_MDATA_EN_MASK (0x1U)
#define S16UP_TX_PCS_TX_ACONTROL0_MDATA_EN_SHIFT (0U)




typedef uint16_t S16UP_TX_PCS_TX_PCS_STATUS_TYPE;
#define S16UP_TX_PCS_TX_PCS_STATUS_SEQ_DET_1_MASK (0x80U)
#define S16UP_TX_PCS_TX_PCS_STATUS_SEQ_DET_1_SHIFT (7U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LF_DET_1_MASK (0x40U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LF_DET_1_SHIFT (6U)
#define S16UP_TX_PCS_TX_PCS_STATUS_RF_DET_1_MASK (0x20U)
#define S16UP_TX_PCS_TX_PCS_STATUS_RF_DET_1_SHIFT (5U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LI_DET_1_MASK (0x10U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LI_DET_1_SHIFT (4U)
#define S16UP_TX_PCS_TX_PCS_STATUS_SEQ_DET_0_MASK (0x8U)
#define S16UP_TX_PCS_TX_PCS_STATUS_SEQ_DET_0_SHIFT (3U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LF_DET_0_MASK (0x4U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LF_DET_0_SHIFT (2U)
#define S16UP_TX_PCS_TX_PCS_STATUS_RF_DET_0_MASK (0x2U)
#define S16UP_TX_PCS_TX_PCS_STATUS_RF_DET_0_SHIFT (1U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LI_DET_0_MASK (0x1U)
#define S16UP_TX_PCS_TX_PCS_STATUS_LI_DET_0_SHIFT (0U)




typedef volatile struct sS16UP_TX_PCS_RDBType {
    S16UP_TX_PCS_RESERVED_TYPE rsvd0[4]; /* OFFSET: 0x0 */
    S16UP_TX_PCS_TX_ACONTROL0_TYPE anatxacontrol0; /* OFFSET: 0x4 */
    S16UP_TX_PCS_RESERVED_TYPE rsvd1[10]; /* OFFSET: 0x6 */
    S16UP_TX_PCS_TX_PCS_STATUS_TYPE tx_pcs_status; /* OFFSET: 0x10 */
} S16UP_TX_PCS_RDBType;


#define TX_PCS_REG0_BASE                (0xE04E01C0UL)



#define S16UP_TX_PCS_MAX_HW_ID          (1UL)

#endif /* S16UP_TX_PCS_RDB_H */
