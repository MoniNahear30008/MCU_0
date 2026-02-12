/*****************************************************************************
 Copyright 2025 Broadcom Limited.  All rights reserved.

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
    @file imap1_rdb.h
    @brief RDB File for IMAP1

    @version BCM89580_A0_20230125_RDB
*/

#ifndef IMAP1_RDB_H
#define IMAP1_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t IMAP1_RESERVED_TYPE;




typedef uint32_t IMAP1_PCIE_IMAP0_00_TYPE;
#define IMAP1_PCIE_IMAP0_00_ADDR31_12_CACHE_ACCESS_MASK (0xffffffffUL)
#define IMAP1_PCIE_IMAP0_00_ADDR31_12_CACHE_ACCESS_SHIFT (0UL)




typedef uint32_t IMAP1_PCIE_IMAP0_UPPER0_TYPE;
#define IMAP1_PCIE_IMAP0_UPPER0_ADDR33_32_MASK (0xffffffffUL)
#define IMAP1_PCIE_IMAP0_UPPER0_ADDR33_32_SHIFT (0UL)




typedef uint32_t IMAP1_PCIE_IMAP0_UPPER1_TYPE;
#define IMAP1_PCIE_IMAP0_UPPER1_ADDR33_32_MASK (0xffffffffUL)
#define IMAP1_PCIE_IMAP0_UPPER1_ADDR33_32_SHIFT (0UL)




typedef uint32_t IMAP1_PCIE_IMAP0_UPPER2_TYPE;
#define IMAP1_PCIE_IMAP0_UPPER2_ADDR33_32_MASK (0xffffffffUL)
#define IMAP1_PCIE_IMAP0_UPPER2_ADDR33_32_SHIFT (0UL)




typedef uint32_t IMAP1_PCIE_IMAP0_UPPER3_TYPE;
#define IMAP1_PCIE_IMAP0_UPPER3_ADDR33_32_MASK (0xffffffffUL)
#define IMAP1_PCIE_IMAP0_UPPER3_ADDR33_32_SHIFT (0UL)



#define IMAP1_PG0_SIZE  (64UL)


typedef volatile struct sIMAP1_RDBType {
    IMAP1_RESERVED_TYPE rsvd0[3072]; /* OFFSET: 0x0 */
    IMAP1_PCIE_IMAP0_00_TYPE pg0[IMAP1_PG0_SIZE]; /* OFFSET: 0xc00 */
    IMAP1_PCIE_IMAP0_UPPER0_TYPE pg15_00; /* OFFSET: 0xd00 */
    IMAP1_PCIE_IMAP0_UPPER1_TYPE pg31_16; /* OFFSET: 0xd04 */
    IMAP1_PCIE_IMAP0_UPPER2_TYPE pg47_32; /* OFFSET: 0xd08 */
    IMAP1_PCIE_IMAP0_UPPER3_TYPE pg63_48; /* OFFSET: 0xd0c */
} IMAP1_RDBType;


#define IMAP1_BASE                      ((uintptr_t)(0xE02D1000UL))



#define IMAP1_MAX_HW_ID                 (1UL)


#define IMAP1_ADDR_MASK                 (0xFFFFF000UL)


#define IMAP1_PAGE_WRITE_EN             (4UL)


#define IMAP1_PAGE_READ_EN              (2UL)


#define IMAP1_PAGE_RCACHE_EN            (0x10UL)


#define IMAP1_PAGE_WCACHE_EN            (0x100UL)


#define IMAP1_PAGE_VALID                (1UL)

#endif /* IMAP1_RDB_H */
