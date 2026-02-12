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
    .syntax unified
    .arch armv7-m
    .cpu cortex-m7
    .code 16
    .text

/*=====================================================
    Macros
=======================================================*/
#define VECTOR_FN(x)  .word x + 1
/*=====================================================
    Externs
    .extern BCM_ProgressLog
=======================================================*/
/*=====================================================
    exception handlers
=======================================================*/
    .global chip_nmi_handler
    .global CORTEX_MX_VECTOR_TBL

/*=====================================================
    exception handlers
=======================================================*/
    .global BCM_OS_NMI_ISR
    .global BCM_OS_HARD_FAULT_ISR
    .global BCM_OS_MPU_FAULT_ISR
    .global BCM_OS_BUS_FAULT_ISR
    .global BCM_OS_USAGE_FAULT_ISR
    .global BCM_OS_DEBUG_MONITOR_ISR
    .global BCM_OS_SYSTICK_ISR
    .global irq_handler
    .global reserved_handler
    .extern wdt1_handler

BCM_OS_NMI_ISR:
    LDR r0, =0x3
    LDR r1, =0x00020002
BCM_OS_NMI_ISR_LOOP:
    b BCM_OS_NMI_ISR_LOOP

BCM_OS_HARD_FAULT_ISR:
    LDR r0, =0x3
    LDR r1, =0x00020003
BCM_OS_HARD_FAULT_ISR_LOOP:
    b BCM_OS_HARD_FAULT_ISR_LOOP

BCM_OS_MPU_FAULT_ISR:
    LDR r0, =0x3
    LDR r1, =0x00020004
BCM_OS_MPU_FAULT_ISR_LOOP:
    b BCM_OS_MPU_FAULT_ISR_LOOP

BCM_OS_BUS_FAULT_ISR:
    LDR r0, =0x3
    LDR r1, =0x00020005
BCM_OS_BUS_FAULT_ISR_LOOP:
    b BCM_OS_BUS_FAULT_ISR_LOOP

BCM_OS_USAGE_FAULT_ISR:
    LDR r0, =0x3
    LDR r1, =0x00020006
BCM_OS_USAGE_FAULT_ISR_LOOP:
    b BCM_OS_USAGE_FAULT_ISR_LOOP

BCM_OS_DEBUG_MONITOR_ISR:
    LDR r0, =0x3
    LDR r1, =0x0002000C
BCM_OS_DEBUG_MONITOR_ISR_LOOP:
    b BCM_OS_DEBUG_MONITOR_ISR_LOOP

BCM_OS_SYSTICK_ISR:
    b BCM_OS_SYSTICK_ISR

irq_handler:
    b irq_handler

reserved_handler:
    b reserved_handler

chip_nmi_handler:
    b chip_nmi_handler

.section .vector_tbl
CORTEX_MX_VECTOR_TBL:
    VECTOR_FN(__main_stack__)
    VECTOR_FN(BCM_OS_RESET_HANDLER)
    VECTOR_FN(BCM_OS_NMI_ISR_LOOP)
    VECTOR_FN(BCM_OS_HARD_FAULT_ISR)
    VECTOR_FN(BCM_OS_MPU_FAULT_ISR)
    VECTOR_FN(BCM_OS_BUS_FAULT_ISR)
    VECTOR_FN(BCM_OS_USAGE_FAULT_ISR)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(BCM_OS_DEBUG_MONITOR_ISR)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(reserved_handler)
    VECTOR_FN(BCM_OS_SYSTICK_ISR)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
    VECTOR_FN(irq_handler)
