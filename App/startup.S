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
#define CORTEX_SHCSR                0xE000ED24
#define CORTEX_CCR                  0xE000ED14

/*=====================================================
    Externs
=======================================================*/
    .extern main
    .extern __main_stack__
    .extern main_stack_size

/*=====================================================
    Reset handler
=======================================================*/
    .global  BCM_OS_RESET_HANDLER
    .global  main

.section .reset_func
BCM_OS_RESET_HANDLER:
/*=====================================================
 *    Setup vector_reg = CORTEX_MX_VECTOR_TBL
 *=======================================================*/
    LDR r0, =CORTEX_MX_VECTOR_TBL
    LDR r1, =0xE000ED08
    STR r0, [r1]

/*=====================================================
 *    Setup stack MSP = __main_stack__
 *=======================================================*/
    LDR r0, =__main_stack__
    MSR MSP, r0
    b STACK_PAINT_EXIT
/*=====================================================
 *    Paint the main stack.
 *=======================================================*/
    LDR r2, =0xA5A5A5A5
    MOV r3, r2
    MOV r4, r2
    MOV r5, r2
    MOV r6, r2
    MOV r7, r2
    MOV r8, r2
    MOV r9, r2

    LDR r0, =main_stack_size

    MOV r1, r0, asr #8
    SUB r0, r0, r1, lsl #8

STACK_PAINT_CHUNK_256_LOOP:
    CBZ r1, STACK_PAINT_CHUNK_128
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    SUB r1, r1, #1
    B STACK_PAINT_CHUNK_256_LOOP

STACK_PAINT_CHUNK_128:
    MOV r1, r0, asr #7
    SUB r0, r0, r1, lsl #7

    CBZ r1, STACK_PAINT_CHUNK_64
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}
    PUSH {r2-r9}

STACK_PAINT_CHUNK_64:
    MOV r1, r0, asr #6
    SUB r0, r0, r1, lsl #6

    CBZ r1, STACK_PAINT_CHUNK_32
    PUSH {r2-r9}
    PUSH {r2-r9}

STACK_PAINT_CHUNK_32:
    MOV r1, r0, asr #5
    SUB r0, r0, r1, lsl #5

    CBZ r1, STACK_PAINT_CHUNK_LAST_LOOP
    PUSH {r2-r9}

STACK_PAINT_CHUNK_LAST_LOOP:
    CBZ r0, STACK_PAINT_EXIT
    RSB r1, r0, #0
    STRB r2, [sp, r1]
    SUB r0, r0, #1
    B STACK_PAINT_CHUNK_LAST_LOOP

STACK_PAINT_EXIT:

/*=====================================================
 *    Reset the stack pointer
 *=======================================================*/
    LDR r0, =__main_stack__
    MSR MSP, r0

/* Enable Div by zero trap (bit4) */
    LDR r0, =CORTEX_CCR
    LDR r1, =0x10
    STR r1, [r0]

/* Enable Faults (18-USAGE, 17-BUS, 16-MEM) */
    LDR r0, =CORTEX_SHCSR
    LDR r1, =0x70000
    STR r1, [r0]

/*=====================================================
 *    Jump to main
 *=======================================================*/
    BLX main

LOOP:
    b       LOOP
