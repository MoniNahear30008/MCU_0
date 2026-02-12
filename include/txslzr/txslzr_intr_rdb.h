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
    @file txslzr_intr_rdb.h
    @brief RDB File for TXSLZR_INTR

    @version BCM89580_A0_20230125_RDB
*/

#ifndef TXSLZR_INTR_RDB_H
#define TXSLZR_INTR_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t TXSLZR_INTR_ATIE_TYPE;
#define TXSLZR_INTR_ATIE_P_SEQ_SINGLE_SHOT_DONE_MASK (0x1UL)
#define TXSLZR_INTR_ATIE_P_SEQ_SINGLE_SHOT_DONE_SHIFT (0UL)




typedef uint32_t TXSLZR_INTR_ATIE1_TYPE;
#define TXSLZR_INTR_ATIE1_SEQ_SINGLE_SHOT_DONE_W2CLR_MASK (0x1UL)
#define TXSLZR_INTR_ATIE1_SEQ_SINGLE_SHOT_DONE_W2CLR_SHIFT (0UL)




typedef uint32_t TXSLZR_INTR_ATIE2_TYPE;
#define TXSLZR_INTR_ATIE2_ENABLE_SEQ_SINGLE_SHOT_DONE_MASK (0x1UL)
#define TXSLZR_INTR_ATIE2_ENABLE_SEQ_SINGLE_SHOT_DONE_SHIFT (0UL)




typedef uint8_t TXSLZR_INTR_RESERVED_TYPE;




typedef uint32_t TXSLZR_INTR_SPARE_END_REG_TYPE;
#define TXSLZR_INTR_SPARE_END_REG_MASK (0xffffffffUL)
#define TXSLZR_INTR_SPARE_END_REG_SHIFT (0UL)




typedef volatile struct sTXSLZR_INTR_RDBType {
    TXSLZR_INTR_ATIE_TYPE acq_txslzr_intr_eipr; /* OFFSET: 0x0 */
    TXSLZR_INTR_ATIE1_TYPE acq_txslzr_intr_eisr; /* OFFSET: 0x4 */
    TXSLZR_INTR_ATIE2_TYPE acq_txslzr_intr_eier; /* OFFSET: 0x8 */
    TXSLZR_INTR_RESERVED_TYPE rsvd0[240]; /* OFFSET: 0xc */
    TXSLZR_INTR_SPARE_END_REG_TYPE spare_end_reg; /* OFFSET: 0xfc */
} TXSLZR_INTR_RDBType;


#define TXSLZR_INTR_BASE                (0xE0607F00UL)



#define TXSLZR_INTR_MAX_HW_ID           (1UL)

#endif /* TXSLZR_INTR_RDB_H */
