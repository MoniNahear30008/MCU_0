/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_cortex_mx_cpu ARM Cortex-Mx CPU Primitives
    @ingroup grp_cortex

    @addtogroup grp_cortex_mx_cpu
    @{
    This file implements the CPU Primitive functions for the
    Cortex-Mx architecture

    @file cpu.c
    @brief CPU Primitive functions

    @version 0.1 Initial version
*/
#include <compiler.h>
#include <cortex_mx.h>
#include <cpu.h>
#ifdef CORTEX_ENABLE_EXCEPTTION_HANDLING
#include <exceptions.h>
#endif

/**
    @name CPU Primitives Architecture IDs
    @{
    @brief Architecture IDs for ARM Cortex-Mx CPU Primitives
*/
#define BRCM_SWDSGN_CPU_PRIVILEGE_LEVEL_PROC       (0xBA00U) /**< @brief #CPU_GetPrivilegeLevel          */
#define BRCM_SWDSGN_CPU_VIC_INT_ENABLE_PROC        (0xBA01U) /**< @brief #CPU_VICInterruptEnable         */
#define BRCM_SWDSGN_CPU_VIC_INT_DISABLE_PROC       (0xBA02U) /**< @brief #CPU_VICInterruptDisable        */
#define BRCM_SWDSGN_CPU_SET_EXCEPTION_ACTION_PROC  (0xBA03U) /**< @brief #CPU_SetExceptionAction         */
#define BRCM_SWDSGN_CPU_GET_EXCEPTION_STATUS_PROC  (0xBA04U) /**< @brief #CPU_GetExceptionStatus         */
#define BRCM_SWDSGN_CPU_EXCEPTION_HANDLER_PROC     (0xBA05U) /**< @brief #CPU_ExceptionHandler           */
#define BRCM_SWDSGN_CPU_CMX_DWT_MACRO              (0xBA06U) /**< @brief #CPU_CMX_DWT_BASE               */
#define BRCM_SWDSGN_CPU_CMX_DWT_REGS_TYPE          (0xBA07U) /**< @brief #CPU_CMXDWTRegsType             */
#define BRCM_SWDSGN_CPU_CMX_DWT_REGS_GLOBAL        (0xBA08U) /**< @brief #CPU_CMXDWTRegs                 */
#define BRCM_SWDSGN_CPU_CMX_DEMCR_MACRO            (0xBA09U) /**< @brief #CPU_CMX_DEMCR_ADDR             */
#define BRCM_SWDSGN_CPU_CMX_DEMCR_REG_GLOBAL       (0xBA0AU) /**< @brief #CPU_CMXDEMCRReg                */
#define BRCM_SWDSGN_CPU_ENABLE_CYCLE_CNT_PROC      (0xBA0BU) /**< @brief #CPU_EnableCycleCnt             */
#define BRCM_SWDSGN_CPU_DISABLE_CYCLE_CNT_PROC     (0xBA0CU) /**< @brief #CPU_DisableCycleCnt            */
#define BRCM_SWDSGN_CPU_GET_CYCLE_CNT_PROC         (0xBA0DU) /**< @brief #CPU_GetCycleCnt                */
/** @} */


/**
    @code{.unparsed}
    if IPSR is non zero or
       Thread privilege bit is zero in CONTROL register :
        level = CPU_PRIVILEGE_LEVEL_HANDLER
    else
        level = CPU_PRIVILEGE_LEVEL_USER
    return level
    @endcode

    @trace  #BRCM_SWARCH_CPU_GET_PRIVILEGE_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
*/
CPU_PrivilegeLevelType CPU_GetPrivilegeLevel(void)
{
    CPU_PrivilegeLevelType level = CPU_PRIVILEGE_LEVEL_HANDLER;

#ifdef ENABLE_CPU_MISC_APIS
    uint32_t ctrlVal;
    uint32_t psrVal;

    COMP_ASM("    MRS %0, CONTROL     \n"
             "    MRS %1, IPSR        \n"
             : "=r" (ctrlVal), "=r" (psrVal));

    if ((0U != psrVal) || (0U == (ctrlVal & 1U))) {
        /* Handler level */
    } else {
        level = CPU_PRIVILEGE_LEVEL_USER;
    }
#endif /* ENABLE_CPU_MISC_APIS */

    return level;
}
#ifdef CORTEX_INTERRUPT_ENABLED
/**
    @trace  #BRCM_SWARCH_CPU_VIC_INT_ENABLE_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX_MX
*/
int32_t CPU_VICInterruptEnable(uint32_t aIntNum)
{
    return CORTEX_MX_INTREnableLine(aIntNum);
}

/**
    @trace  #BRCM_SWARCH_CPU_VIC_INT_DISABLE_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX_MX
*/
int32_t CPU_VICInterruptDisable(uint32_t aLineNo)
{
    return CORTEX_MX_INTRDisableLine(aLineNo);
}
#endif

#ifdef CORTEX_ENABLE_EXCEPTTION_HANDLING
/**
    @trace  #BRCM_SWARCH_CPU_SET_EXCEPTION_ACTION_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX_MX
*/
void CPU_SetExceptionAction(CPU_ExceptionType aExceptions, CPU_ExceptionActionType aAction)
{
    CORTEX_MX_SetExceptionAction(aExceptions, aAction);
}

/**
    @trace  #BRCM_SWARCH_CPU_GET_EXCEPTION_STATUS_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX_MX
*/
void CPU_GetExceptionStatus(CPU_ExceptionType aExceptions, CPU_ExceptionStatusType *const aStatus)
{
    CORTEX_MX_GetExceptionStatus(aExceptions, aStatus);
}

/**
    @trace  #BRCM_SWARCH_CPU_EXCEPTION_HANDLER_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX_MX
*/
void CPU_ExceptionHandler(uint32_t *aStack, uint32_t aExceptionNum)
{
    CORTEX_MX_ExceptionHandler(aStack, aExceptionNum);
}
#endif

/** @brief DWT registers related macros

    @trace #BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_GET_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
#define CPU_CMX_DWT_BASE (0xE0001000UL)
#define CPU_CMX_DWT_CTRL_NOCYCCNT_MASK (0x02000000UL)
#define CPU_CMX_DWT_CTRL_CYCCNTENA_ENABLE (0x00000001UL)
/** @} */

/** @brief Cortex MX DWT registers layout

    @trace #BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_GET_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
typedef volatile struct sCPU_CMXDWTRegsType {
    uint32_t ctrl;    /**< @brief DWT_CTRL   */
    uint32_t cyccnt;  /**< @brief DWT_CYCCNT */
} CPU_CMXDWTRegsType;
_Static_assert(sizeof(CPU_CMXDWTRegsType) == 8UL, "Register layout does not match the HW docs!");

/** @brief Global to access the Cortex MX DWT registers

    @trace #BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_GET_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
static CPU_CMXDWTRegsType *const CPU_CMXDWTRegs = (CPU_CMXDWTRegsType *)(CPU_CMX_DWT_BASE);

/** @brief Cortex MX DEMCR register related macros

    @trace #BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_GET_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
#define CPU_CMX_DEMCR_ADDR (0xE000EDFCUL)
#define CPU_CMX_DEMCR_TRCENA_BIT (1UL << 24UL)
/** @} */

/** @brief Global to access the Cortex MX DEMCR registers

    @trace #BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
static volatile uint32_t *const CPU_CMXDEMCRReg = (volatile uint32_t*)(CPU_CMX_DEMCR_ADDR);

/**
    @code{.unparsed}
    if NOCYCCNT bit in DWT_CTRL register is clear:
        Enable the Cortex MX tracing blocks
        Enable HW cycle counting
        if aResetCount in Non-zero
            Reset counter value
        return BCM_ERR_OK
    else:
        return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_CPU_ENABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
int32_t CPU_EnableCycleCnt(uint8_t aResetCount)
{
    int32_t ret;
    uint32_t noCycCntMsk = CPU_CMX_DWT_CTRL_NOCYCCNT_MASK;
    uint32_t dwtCtrlRd = CPU_CMXDWTRegs->ctrl;

    /* BCM_TEST_ADD_PROBE(BCM_ARM_ID, BRCM_SWPTSEQ_CPU_ARM,
                          BRCM_SWPTST_CPU_ABSTRACTION_SET_DWT_CTRL,
                          dwtCtrlRd |= CPU_CMX_DWT_CTRL_NOCYCCNT_MASK) */
    /* BCM_TEST_REG_PROBE(BRCM_SWQTST_BCM8915X_CYCLE_CNT_FAIL_PROBE, dwtCtrlRd |= CPU_CMX_DWT_CTRL_NOCYCCNT_MASK) */
    if (0UL == (dwtCtrlRd & noCycCntMsk)) {
        *CPU_CMXDEMCRReg |= CPU_CMX_DEMCR_TRCENA_BIT;
        CPU_CMXDWTRegs->ctrl |= CPU_CMX_DWT_CTRL_CYCCNTENA_ENABLE;
        if (0U != aResetCount) {
            CPU_CMXDWTRegs->cyccnt = 0UL;
        }
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }

    return ret;
}

/**
    @code{.unparsed}
    Disable HW cycle counting
    Disable the Cortex MX tracing blocks
    @endcode

    @trace #BRCM_SWARCH_CPU_DISABLE_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
void CPU_DisableCycleCnt(void)
{
    CPU_CMXDWTRegs->ctrl &= ~(CPU_CMX_DWT_CTRL_CYCCNTENA_ENABLE);
    *CPU_CMXDEMCRReg &= ~CPU_CMX_DEMCR_TRCENA_BIT;
    return;
}

/**
    @code{.unparsed}
    Sample the cycle count register and return the value
    @endcode

    @trace #BRCM_SWARCH_CPU_GET_CYCLE_CNT_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
uint64_t CPU_GetCycleCnt(void)
{
    return (uint64_t)CPU_CMXDWTRegs->cyccnt;
}

/** @} */
