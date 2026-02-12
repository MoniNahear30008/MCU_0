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
    @file s16up_comclk_status_1f_rdb.h
    @brief RDB File for S16UP_COMCLK_STATUS_1F

    @version BCM89580_A0_20230125_RDB
*/

#ifndef S16UP_COMCLK_STATUS_1F_RDB_H
#define S16UP_COMCLK_STATUS_1F_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t S16UP_COMCLK_STATUS_1F_RESERVED_TYPE;




typedef uint16_t S16UP_COMCLK_STATUS_1F_R1TS1_TYPE;
#define S16UP_COMCLK_STATUS_1F_R1TS1_TMUX_DATA_READOUT_MASK (0xffffU)
#define S16UP_COMCLK_STATUS_1F_R1TS1_TMUX_DATA_READOUT_SHIFT (0U)




typedef uint16_t S16UP_COMCLK_STATUS_1F_AEQ_EQ_OC_AFE_STATUS_TYPE;
#define S16UP_COMCLK_STATUS_1F_AEQ_EQ_OC_AFE_STATUS_I_PF_CTRL_AFE_MASK (0x3c0U)
#define S16UP_COMCLK_STATUS_1F_AEQ_EQ_OC_AFE_STATUS_I_PF_CTRL_AFE_SHIFT (6U)
#define S16UP_COMCLK_STATUS_1F_AEQ_EQ_OC_AFE_STATUS_I_LMTOFF_AFE_MASK (0x3fU)
#define S16UP_COMCLK_STATUS_1F_AEQ_EQ_OC_AFE_STATUS_I_LMTOFF_AFE_SHIFT (0U)




typedef uint16_t S16UP_COMCLK_STATUS_1F_ASAS1_TYPE;
#define S16UP_COMCLK_STATUS_1F_ASAS1_I_SLOFF2O_AFE_MASK (0xf00U)
#define S16UP_COMCLK_STATUS_1F_ASAS1_I_SLOFF2O_AFE_SHIFT (8U)
#define S16UP_COMCLK_STATUS_1F_ASAS1_I_SLOFF1O_AFE_MASK (0xf0U)
#define S16UP_COMCLK_STATUS_1F_ASAS1_I_SLOFF1O_AFE_SHIFT (4U)
#define S16UP_COMCLK_STATUS_1F_ASAS1_I_SLOFF0O_AFE_MASK (0xfU)
#define S16UP_COMCLK_STATUS_1F_ASAS1_I_SLOFF0O_AFE_SHIFT (0U)




typedef volatile struct sS16UP_COMCLK_STATUS_1F_RDBType {
    S16UP_COMCLK_STATUS_1F_RESERVED_TYPE rsvd0[8]; /* OFFSET: 0x0 */
    S16UP_COMCLK_STATUS_1F_R1TS1_TYPE rxpmd_1rx_testmux_status1; /* OFFSET: 0x8 */
    S16UP_COMCLK_STATUS_1F_RESERVED_TYPE rsvd1[2]; /* OFFSET: 0xa */
    S16UP_COMCLK_STATUS_1F_AEQ_EQ_OC_AFE_STATUS_TYPE aeq_eq_oc_afe_status; /* OFFSET: 0xc */
    S16UP_COMCLK_STATUS_1F_RESERVED_TYPE rsvd2[2]; /* OFFSET: 0xe */
    S16UP_COMCLK_STATUS_1F_ASAS1_TYPE aeq_slcal_afe_status1; /* OFFSET: 0x10 */
} S16UP_COMCLK_STATUS_1F_RDBType;


#define STATUS_1F_EN_BASE               (0xE04681C0UL)



#define S16UP_COMCLK_STATUS_1F_MAX_HW_ID  (1UL)

#endif /* S16UP_COMCLK_STATUS_1F_RDB_H */
