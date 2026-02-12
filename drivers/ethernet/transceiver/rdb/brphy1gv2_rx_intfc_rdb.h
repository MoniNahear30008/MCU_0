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
    @file brphy1gv2_rx_intfc_rdb.h
    @brief RDB File for BRPHY1GV2_RX_INTFC

    @version BCM89580_A0_20230125_RDB
*/

#ifndef BRPHY1GV2_RX_INTFC_RDB_H
#define BRPHY1GV2_RX_INTFC_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t BRPHY1GV2_RX_INTFC_CTL_TYPE;
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_1000M_DELAY_SELECT_MASK (0x700U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_1000M_DELAY_SELECT_SHIFT (8U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_100M_DELAY_SELECT_MASK (0xe0U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_100M_DELAY_SELECT_SHIFT (5U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_T0_T1_SYM_INVERT_MASK (0x10U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_T0_T1_SYM_INVERT_SHIFT (4U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_T0_T1_SYM_BIT_REVERSE_ORDER_MASK (0x8U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_T0_T1_SYM_BIT_REVERSE_ORDER_SHIFT (3U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_T0_T1_SYM_SWAP_MASK (0x4U)
#define BRPHY1GV2_RX_INTFC_CTL_AFE_RX_T0_T1_SYM_SWAP_SHIFT (2U)
#define BRPHY1GV2_RX_INTFC_CTL_LINK_SYNC_T0_T1_SYM_SWAP_MASK (0x2U)
#define BRPHY1GV2_RX_INTFC_CTL_LINK_SYNC_T0_T1_SYM_SWAP_SHIFT (1U)
#define BRPHY1GV2_RX_INTFC_CTL_DSP_T0_T1_SYM_SWAP_MASK (0x1U)
#define BRPHY1GV2_RX_INTFC_CTL_DSP_T0_T1_SYM_SWAP_SHIFT (0U)




typedef uint8_t BRPHY1GV2_RX_INTFC_RESERVED_TYPE;




typedef uint16_t BRPHY1GV2_RX_INTFC_SPARE_END_ADDR_TYPE;
#define BRPHY1GV2_RX_INTFC_SPARE_END_ADDR_MASK (0xffffU)
#define BRPHY1GV2_RX_INTFC_SPARE_END_ADDR_SHIFT (0U)




typedef volatile struct sBRPHY1GV2_RX_INTFC_RDBType {
    BRPHY1GV2_RX_INTFC_CTL_TYPE rx_intfc_ctl; /* OFFSET: 0x0 */
    BRPHY1GV2_RX_INTFC_RESERVED_TYPE rsvd0[1018]; /* OFFSET: 0x2 */
    BRPHY1GV2_RX_INTFC_SPARE_END_ADDR_TYPE spare_end_addr; /* OFFSET: 0x3fc */
} BRPHY1GV2_RX_INTFC_RDBType;


#define BRPHY0_1G_RX_INTFC_BASE         (0xE0565000UL)



#define BRPHY1GV2_RX_INTFC_MAX_HW_ID    (1UL)

#endif /* BRPHY1GV2_RX_INTFC_RDB_H */
