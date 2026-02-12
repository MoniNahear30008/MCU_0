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
    @file cache_fsc_rdb.h
    @brief RDB File for CACHE_FSC

    @version BCM89580_A0_20230125_RDB
*/

#ifndef CACHE_FSC_RDB_H
#define CACHE_FSC_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t CACHE_FSC_FSCBISTCR_TYPE;
#define CACHE_FSC_FSCBISTCR_BIST_START_MASK (0x1UL)
#define CACHE_FSC_FSCBISTCR_BIST_START_SHIFT (0UL)
#define CACHE_FSC_FSCBISTCR_BIST_STEP_MASK (0x2UL)
#define CACHE_FSC_FSCBISTCR_BIST_STEP_SHIFT (1UL)
#define CACHE_FSC_FSCBISTCR_RSVD1_MASK (0xfffffffcUL)
#define CACHE_FSC_FSCBISTCR_RSVD1_SHIFT (2UL)




typedef uint32_t CACHE_FSC_FSCBISTAR_TYPE;
#define CACHE_FSC_FSCBISTAR_BIST_DONE_MASK (0x1fUL)
#define CACHE_FSC_FSCBISTAR_BIST_DONE_SHIFT (0UL)
#define CACHE_FSC_FSCBISTAR_BIST_ERR_MASK (0x3e0UL)
#define CACHE_FSC_FSCBISTAR_BIST_ERR_SHIFT (5UL)
#define CACHE_FSC_FSCBISTAR_RSVD1_MASK (0xfffffc00UL)
#define CACHE_FSC_FSCBISTAR_RSVD1_SHIFT (10UL)




typedef uint8_t CACHE_FSC_RESERVED_TYPE;




typedef uint32_t CACHE_FSC_FSCLF0_TYPE;
#define CACHE_FSC_FSCLF0_LATENT_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCLF0_LATENT_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCLF1_TYPE;
#define CACHE_FSC_FSCLF1_LATENT_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCLF1_LATENT_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCLF2_TYPE;
#define CACHE_FSC_FSCLF2_LATENT_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCLF2_LATENT_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCLF3_TYPE;
#define CACHE_FSC_FSCLF3_LATENT_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCLF3_LATENT_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCMF0_TYPE;
#define CACHE_FSC_FSCMF0_MISSION_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCMF0_MISSION_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCMF1_TYPE;
#define CACHE_FSC_FSCMF1_MISSION_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCMF1_MISSION_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCMF2_TYPE;
#define CACHE_FSC_FSCMF2_MISSION_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCMF2_MISSION_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCMF3_TYPE;
#define CACHE_FSC_FSCMF3_MISSION_FAULT_MASK (0xffffffffUL)
#define CACHE_FSC_FSCMF3_MISSION_FAULT_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCCETHF0_TYPE;
#define CACHE_FSC_FSCCETHF0_CERR_OVER_THRESH_MASK (0xffffffffUL)
#define CACHE_FSC_FSCCETHF0_CERR_OVER_THRESH_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCCETHF1_TYPE;
#define CACHE_FSC_FSCCETHF1_CERR_OVER_THRESH_MASK (0xffffffffUL)
#define CACHE_FSC_FSCCETHF1_CERR_OVER_THRESH_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCCETHF2_TYPE;
#define CACHE_FSC_FSCCETHF2_CERR_OVER_THRESH_MASK (0xffffffffUL)
#define CACHE_FSC_FSCCETHF2_CERR_OVER_THRESH_SHIFT (0UL)




typedef uint32_t CACHE_FSC_FSCCETHF3_TYPE;
#define CACHE_FSC_FSCCETHF3_CERR_OVER_THRESH_MASK (0xffffffffUL)
#define CACHE_FSC_FSCCETHF3_CERR_OVER_THRESH_SHIFT (0UL)




typedef volatile struct sCACHE_FSC_RDBType {
    CACHE_FSC_FSCBISTCR_TYPE fscbistcr; /* OFFSET: 0x0 */
    CACHE_FSC_FSCBISTAR_TYPE fscbistar; /* OFFSET: 0x4 */
    CACHE_FSC_RESERVED_TYPE rsvd0[8]; /* OFFSET: 0x8 */
    CACHE_FSC_FSCLF0_TYPE fsclf0; /* OFFSET: 0x10 */
    CACHE_FSC_FSCLF1_TYPE fsclf1; /* OFFSET: 0x14 */
    CACHE_FSC_FSCLF2_TYPE fsclf2; /* OFFSET: 0x18 */
    CACHE_FSC_FSCLF3_TYPE fsclf3; /* OFFSET: 0x1c */
    CACHE_FSC_FSCMF0_TYPE fscmf0; /* OFFSET: 0x20 */
    CACHE_FSC_FSCMF1_TYPE fscmf1; /* OFFSET: 0x24 */
    CACHE_FSC_FSCMF2_TYPE fscmf2; /* OFFSET: 0x28 */
    CACHE_FSC_FSCMF3_TYPE fscmf3; /* OFFSET: 0x2c */
    CACHE_FSC_FSCCETHF0_TYPE fsccethf0; /* OFFSET: 0x30 */
    CACHE_FSC_FSCCETHF1_TYPE fsccethf1; /* OFFSET: 0x34 */
    CACHE_FSC_FSCCETHF2_TYPE fsccethf2; /* OFFSET: 0x38 */
    CACHE_FSC_FSCCETHF3_TYPE fsccethf3; /* OFFSET: 0x3c */
} CACHE_FSC_RDBType;


#define RIG_LLC_FSC_CSR_BASE            (0xE029A000UL)



#define CACHE_FSC_MAX_HW_ID             (1UL)

#endif /* CACHE_FSC_RDB_H */
