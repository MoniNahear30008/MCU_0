/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_cpu_abstract CPU Abstraction interface
    @ingroup grp_abstract

    @addtogroup grp_cpu_abstract
    @{

    @file cpu.h
    @brief This file contains CPU Abstraction API interfaces

    @version 0.1 Initial Version
*/

#ifndef CPU_H
#define CPU_H

#include <inttypes.h>
#include <stdint.h>

/**
    @name CPU abstraction Architecture IDs
    @{
    @brief Architecture IDs for CPU abstraction
*/
#define BRCM_SWARCH_CPU_PRIVILEGE_LEVEL_TYPE         (0x8A00U)    /**< @brief #CPU_PrivilegeLevelType  */
#define BRCM_SWARCH_CPU_GET_PRIVILEGE_PROC           (0x8A01U)    /**< @brief #CPU_GetPrivilegeLevel   */
#define BRCM_SWARCH_CPU_CLZ_PROC                     (0x8A05U)    /**< @brief #CPU_Clz                 */
#define BRCM_SWARCH_CPU_BITREVERSE_PROC              (0x8A06U)    /**< @brief #CPU_BitReverse          */
#define BRCM_SWARCH_CPU_BREAKPOINT_PROC              (0x8A07U)    /**< @brief #CPU_Breakpoint          */
#define BRCM_SWARCH_CPU_SVC_REQUEST_PROC             (0x8A0AU)    /**< @brief #CPU_SvcRequest          */
#define BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC        (0x8A0BU)    /**< @brief #CPU_EnableCycleCnt      */
#define BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC       (0x8A0CU)    /**< @brief #CPU_DisableCycleCnt     */
#define BRCM_SWARCH_CPU_GET_CYCLE_CNT_PROC           (0x8A0DU)    /**< @brief #CPU_GetCycleCnt         */
/** @} */

/**
   @name CPU Privilege levels
   @{
   @brief CPU Privilege levels

   @trace  #BRCM_SWREQ_CPU_ABSTRACTION
*/
typedef uint32_t CPU_PrivilegeLevelType;
#define CPU_PRIVILEGE_LEVEL_USER      (0U)   /**< @brief User mode      */
#define CPU_PRIVILEGE_LEVEL_HANDLER   (1U)   /**< @brief Handler mode   */
/** @} */

/** @brief Get current CPU privilege level

    Get the current CPU privilege level

    @behavior Sync, Non-reentrant

    @pre None

    @retval CPU_PRIVILEGE_LEVEL_USER - Currently in user mode
    @retval CPU_PRIVILEGE_LEVEL_HANDLER - Currently in handler mode

    @post None

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @limitations None
*/
/*  @event         misra_c_2012_rule_5_8_violation, MISRA C-2012 Identifiers
    @issue         Identifier CPU_GetPrivilegeLevel is already used to represent a function\
                   with external linkage.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_5_8_violation] */
CPU_PrivilegeLevelType CPU_GetPrivilegeLevel(void);

#ifdef ENABLE_CPU_MISC_APIS
/** @brief Trigger a SW breakpoint

    @behavior Sync, Non-reentrant

    @pre None

    @return void

    @post Breakpoint is triggered. The behaviour after
        the breakpoint is triggered is system specific

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @limitations None
*/
/*  @event         misra_c_2012_rule_5_8_violation, MISRA C-2012 Identifiers
    @issue         Identifier CPU_Breakpoint is already used to represent a function\
                   with external linkage.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_5_8_violation] */
void CPU_Breakpoint(void);
#endif /* ENABLE_CPU_MISC_APIS */

/** @brief Count of leading zeroes

    returns the count of leading zeroes

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aValue      Input value which needs
                            to be used to determine
                            count of leading zeroes

    @retval    Count of leading zeroes

    @post None

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @limitations None
*/
/*  @event         misra_c_2012_rule_5_8_violation, MISRA C-2012 Identifiers
    @issue         Identifier CPU_Clz is already used to represent a function\
                   with external linkage.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_5_8_violation] */
uint32_t CPU_Clz(uint32_t aValue);

/** @brief Reverse the bits

    Reverses the order of bits in the passed vale

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aValue      Input value whose bits
                            need to be reversed

    @retVal    uint32_t    Value with bits reversed

    @post None

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @limitations None
*/
/*  @event         misra_c_2012_rule_5_8_violation, MISRA C-2012 Identifiers
    @issue         Identifier CPU_BitReverse is already used to represent a function\
                   with external linkage.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_5_8_violation] */
uint32_t CPU_BitReverse(uint32_t aValue);

#ifdef ENABLE_CPU_MISC_APIS
/** @brief SVC request

    This API executes the CPU specific SW trap instruction and requests
    a kernel service.

    @behavior Sync, Non-reentrant

    @pre CPU should be executing at #CPU_PRIVILEGE_LEVEL_USER privilege level

    @param[in]      aSVCReq                 SVC Request type
    @param[in]      aSysReqID               SVC Request ID

    @return void

    @post Executes the SVC call

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @limitations
    + aSysReqID should be valid and appropriate.
    + aSVCReq shall be a valid pointer
*/
/*  @event         misra_c_2012_rule_5_8_violation, MISRA C-2012 Identifiers
    @issue         Identifier CPU_SvcRequest is already used to represent a function\
                   with external linkage.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_5_8_violation] */
void CPU_SvcRequest(uint32_t aSysReqID, void * const aSVCReq);
#endif /* ENABLE_CPU_MISC_APIS */

/** @brief Enable the CPU cycle counting

    @behavior Sync, Non-reentrant

    @pre CPU should be executing at #CPU_PRIVILEGE_LEVEL_USER privilege level

    @param[in]      aResetCount             1: Reset counter and enable counter
                                            0: Just enable counter

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK             CPU cycle counting enabled
    @retval         #BCM_ERR_NOSUPPORT      CPU cycle count fetch is not supported

    @post CPU cycle counting enabled

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION
*/
int32_t CPU_EnableCycleCnt(uint8_t aResetCount);

/** @brief Disable the CPU cycle counting

    @behavior Sync, Non-reentrant

    @pre CPU should be executing at #CPU_PRIVILEGE_LEVEL_USER privilege level

    @return void

    @post CPU cycle counting disabled

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION
*/
void CPU_DisableCycleCnt(void);

/** @brief Get the number of CPU cycles elapsed since the last call to #CPU_EnableCycleCnt

    @behavior Sync, Non-reentrant

    @pre CPU should be executing at #CPU_PRIVILEGE_LEVEL_USER privilege level

    @return Number of CPU ticks elapsed since the last call to #CPU_EnableCycleCnt

    @post None

    @trace  #BRCM_SWREQ_CPU_ABSTRACTION
*/
uint64_t CPU_GetCycleCnt(void);

#endif /* CPU_H */

/** @} */
