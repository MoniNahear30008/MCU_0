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
    @file s16up_u_tx_x1_status0_rdb.h
    @brief RDB File for S16UP_U_TX_X1_STATUS0

    @version BCM89580_A0_20230125_RDB
*/

#ifndef S16UP_U_TX_X1_STATUS0_RDB_H
#define S16UP_U_TX_X1_STATUS0_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t S16UP_U_TX_X1_STATUS0_UTPS_TYPE;
#define S16UP_U_TX_X1_STATUS0_UTPS_TX_GBOX_AFRST_STKY_MASK (0x800U)
#define S16UP_U_TX_X1_STATUS0_UTPS_TX_GBOX_AFRST_STKY_SHIFT (11U)
#define S16UP_U_TX_X1_STATUS0_UTPS_TX_GBOX_OVERFLOW_ERR_DET_LATCH_MASK (0x400U)
#define S16UP_U_TX_X1_STATUS0_UTPS_TX_GBOX_OVERFLOW_ERR_DET_LATCH_SHIFT (10U)
#define S16UP_U_TX_X1_STATUS0_UTPS_TGUEDL_MASK (0x200U)
#define S16UP_U_TX_X1_STATUS0_UTPS_TGUEDL_SHIFT (9U)




typedef uint8_t S16UP_U_TX_X1_STATUS0_RESERVED_TYPE;




typedef uint16_t S16UP_U_TX_X1_STATUS0_UTLA_STAT_LN_TYPE;
#define S16UP_U_TX_X1_STATUS0_UTLA_STAT_LN_TPSFSL_MASK (0x1fU)
#define S16UP_U_TX_X1_STATUS0_UTLA_STAT_LN_TPSFSL_SHIFT (0U)




typedef volatile struct sS16UP_U_TX_X1_STATUS0_RDBType {
    S16UP_U_TX_X1_STATUS0_UTPS_TYPE pcs; /* OFFSET: 0x0 */
    S16UP_U_TX_X1_STATUS0_RESERVED_TYPE rsvd0[2]; /* OFFSET: 0x2 */
    S16UP_U_TX_X1_STATUS0_UTLA_STAT_LN_TYPE tla; /* OFFSET: 0x4 */
} S16UP_U_TX_X1_STATUS0_RDBType;


#define U_TX_X1_STATUS0_BASE            (0xE04E5300UL)



#define S16UP_U_TX_X1_STATUS0_MAX_HW_ID  (1UL)

#endif /* S16UP_U_TX_X1_STATUS0_RDB_H */
