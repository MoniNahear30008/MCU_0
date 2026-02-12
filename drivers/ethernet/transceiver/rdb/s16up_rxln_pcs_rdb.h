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
    @file s16up_rxln_pcs_rdb.h
    @brief RDB File for S16UP_RXLN_PCS

    @version BCM89580_A0_20230125_RDB
*/

#ifndef S16UP_RXLN_PCS_RDB_H
#define S16UP_RXLN_PCS_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t S16UP_RXLN_PCS_RX_PRBS_TYPE;
#define S16UP_RXLN_PCS_RX_PRBS_PRBSMON_EN_MASK (0x1000U)
#define S16UP_RXLN_PCS_RX_PRBS_PRBSMON_EN_SHIFT (12U)
#define S16UP_RXLN_PCS_RX_PRBS_PRBSMON_INV_MASK (0x800U)
#define S16UP_RXLN_PCS_RX_PRBS_PRBSMON_INV_SHIFT (11U)
#define S16UP_RXLN_PCS_RX_PRBS_PRBSMON_ORDER_MASK (0x700U)
#define S16UP_RXLN_PCS_RX_PRBS_PRBSMON_ORDER_SHIFT (8U)




typedef uint8_t S16UP_RXLN_PCS_RESERVED_TYPE;




typedef uint16_t S16UP_RXLN_PCS_RX_CONT10001_TYPE;
#define S16UP_RXLN_PCS_RX_CONT10001_LPI_EN_RX_MASK (0x8000U)
#define S16UP_RXLN_PCS_RX_CONT10001_LPI_EN_RX_SHIFT (15U)
#define S16UP_RXLN_PCS_RX_CONT10001_ECCFED_MASK (0x40U)
#define S16UP_RXLN_PCS_RX_CONT10001_ECCFED_SHIFT (6U)
#define S16UP_RXLN_PCS_RX_CONT10001_CRC_CHECKER_DISABLE_MASK (0x20U)
#define S16UP_RXLN_PCS_RX_CONT10001_CRC_CHECKER_DISABLE_SHIFT (5U)
#define S16UP_RXLN_PCS_RX_CONT10001_AUTODET_EN_MASK (0x10U)
#define S16UP_RXLN_PCS_RX_CONT10001_AUTODET_EN_SHIFT (4U)
#define S16UP_RXLN_PCS_RX_CONT10001_INVERT_SIGNAL_DETECT_MASK (0x8U)
#define S16UP_RXLN_PCS_RX_CONT10001_INVERT_SIGNAL_DETECT_SHIFT (3U)
#define S16UP_RXLN_PCS_RX_CONT10001_SIGNAL_DETECT_EN_MASK (0x4U)
#define S16UP_RXLN_PCS_RX_CONT10001_SIGNAL_DETECT_EN_SHIFT (2U)




typedef volatile struct sS16UP_RXLN_PCS_RDBType {
    S16UP_RXLN_PCS_RX_PRBS_TYPE prbs; /* OFFSET: 0x0 */
    S16UP_RXLN_PCS_RESERVED_TYPE rsvd0[2]; /* OFFSET: 0x2 */
    S16UP_RXLN_PCS_RX_CONT10001_TYPE control1000x1; /* OFFSET: 0x4 */
} S16UP_RXLN_PCS_RDBType;


#define RX_PCS_REG1_BASE                (0xE04E0340UL)



#define S16UP_RXLN_PCS_MAX_HW_ID        (1UL)

#endif /* S16UP_RXLN_PCS_RDB_H */
