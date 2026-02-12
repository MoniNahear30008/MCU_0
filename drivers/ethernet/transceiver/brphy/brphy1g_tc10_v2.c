/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvr_brphy1gdrv_tc10_impl BRPHY1G TC10 implementation
    @ingroup grp_xcvr_brphy1gdrv

    @addtogroup grp_xcvr_brphy1gdrv_tc10_impl
    @{

    @file brphy1g_tc10_v2.c
    @brief This file implements TC10 feature for BRPHY 1G driver

    @version 0.01 draft revision
*/

#include "./../inc/xcvr_functbl.h"
#include "./../rdb/brphy1gv2_test_rdb.h"
#include "./../rdb/brphy1gv2_core_misc_rdb.h"
#include "./../rdb/brphy1gv2_t1core_rdb.h"
#include "brphy1g_platform.h"
#include <bcm_utils.h>
#include "./../rdb/brphy1gv2_pmapmdieee_rdb.h"
#include <system_mcu.h>
#include <bcm_time.h>
#include <bcm_err.h>

/**
    @name TC10 BRPHY 1G driver Design IDs
    @{
    @brief Design IDs for TC10 BRPHY 1G driver
*/
#define BRCM_SWDSGN_BRPHY1GV2_TC10INIT_PROC                 (0xBA01U)  /**< @brief #BRPHY1GV2_TC10Init               */
#define BRCM_SWDSGN_BRPHY1GV2_TC10IRQHANDLER_PROC           (0xBA02U)  /**< @brief #BRPHY1GV2_TC10IRQHandler         */
#define BRCM_SWDSGN_BRPHY1GV2_TC10_TEST_REGS_GLOBAL         (0xBA03U)  /**< @brief #BRPHY1GV2_TC10_TEST_REGS         */
#define BRCM_SWDSGN_BRPHY1GV2_TC10CORE_MISC_REGS_GLOBAL     (0xBA04U)  /**< @brief #BRPHY1GV2_TC10CORE_MISC_REGS     */
#define BRCM_SWDSGN_BRPHY1GV2_TC10SLEEPREQ_PROC             (0xBA0AU)  /**< @brief #BRPHY1GV2_TC10SleepReq           */
#define BRCM_SWDSGN_BRPHY1GV2_TC10WAKEUPREQ_PROC            (0xBA0BU)  /**< @brief #BRPHY1GV2_TC10WakeUpReq          */
#define BRCM_SWDSGN_BRPHY1GV2_TC10SLEEPABORTREQ_PROC        (0xBA0CU)  /**< @brief #BRPHY1GV2_TC10SleepAbortReq      */
#define BRCM_SWDSGN_BRPHY1GV2_TC10FORCESLEEPREQ_PROC        (0xBA0DU)  /**< @brief #BRPHY1GV2_TC10ForceSleepReq      */
#define BRCM_SWDSGN_BRPHY1GV2_TC10FUNCTBL_GLOBAL            (0xBA0EU)  /**< @brief #BRPHY1GV2_TC10FuncTbl            */
#define BRCM_SWDSGN_BRPHY1GV2_TC10DISABLEEVENTS_PROC        (0xBA0FU)  /**< @brief #BRPHY1GV2_TC10DisableEvents      */
#define BRCM_SWDSGN_BRPHY1GV2_TC10ENABLEEVENTS_PROC         (0xBA10U)  /**< @brief #BRPHY1GV2_TC10EnableEvents       */
#define BRCM_SWDSGN_BRPHY1GV2_TC10ISSUPPORTED_PROC          (0xBA11U)  /**< @brief #BRPHY1GV2_TC10IsSupported        */
#define BRCM_SWDSGN_BRPHY1GV2_TC10ALLOWSLEEPREQ_PROC        (0xBA12U)  /**< @brief #BRPHY1GV2_TC10AllowSleepReq      */
#define BRCM_SWDSGN_BRPHY1GV2_TC10RESETSELECT_PROC          (0xBA13U)  /**< @brief #BRPHY1GV2_TC10ResetSelect        */
#define BRCM_SWDSGN_BRPHY1GV2_TC10ENPGMSTATE_PROC           (0xBA14U)  /**< @brief #BRPHY1GV2_TC10EnPgmState         */
#define BRCM_SWDSGN_BRPHY1GV2_TC10EXITHOSTMODE_PROC         (0xBA15U)  /**< @brief #BRPHY1GV2_TC10ExitHostMode       */
#define BRCM_SWDSGN_BRPHY1GV2_TC10ENABLE_PROC               (0xBA16U)  /**< @brief #BRPHY1GV2_TC10Enable             */
#define BRCM_SWDSGN_BRPHY1GV2_TC10VALIDATEPARAMS_PROC       (0xBA17U)  /**< @brief #BRPHY1GV2_TC10ValidateParams     */
#define BRCM_SWDSGN_BRPHY1GV2_TC10_PMAPMDIEEE_REGS_GLOBAL   (0xBA18U)  /**< @brief #BRPHY1GV2_TC10_PMAPMDIEEE_REGS   */
#define BRCM_SWDSGN_BRPHY1GV2_TC10ALLOW_MDI_WAKE_PROC       (0xBA19U)  /**< @brief #BRPHY1GV2_TC10AllowMdiWake       */
/** @} */


/**
    @brief Test block base addresses

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_TEST_RDBType * const BRPHY1GV2_TC10_TEST_REGS[BRPHY1GV2_TEST_MAX_HW_ID] =
{
#if (BRPHY1GV2_TEST_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_TEST_MAX_HW_ID = 0UL"
#endif
    (BRPHY1GV2_TEST_RDBType *const)BRPHY0_1G_TEST_BASE,
#if (BRPHY1GV2_TEST_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_TEST_MAX_HW_ID > 1UL is not supported"
#endif
};

/**
    @brief PMA PMD IEEE block base addresses

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_PMAPMDIEEE_RDBType* const BRPHY1GV2_TC10_PMAPMDIEEE_REGS[BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID] =
{
#if (BRPHY1GV2_TEST_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_TEST_MAX_HW_ID = 0UL"
#endif
    (BRPHY1GV2_PMAPMDIEEE_RDBType *const)BRPHY0_1G_PMA_PMD_IEEE_BASE,
#if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID > 1UL is not supported"
#endif
};

/**
    @brief BRPHY 1G Port Core Misc block base addresses

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_CORE_MISC_RDBType * const BRPHY1GV2_TC10CORE_MISC_REGS[BRPHY1GV2_CORE_MISC_MAX_HW_ID] = {
#if (BRPHY1GV2_CORE_MISC_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_CORE_MISC_MAX_HW_ID = 0UL"
#endif
    (BRPHY1GV2_CORE_MISC_RDBType *const)BRPHY0_1G_CORE_MISC_BASE,
#if (BRPHY1GV2_CORE_MISC_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_CORE_MISC_MAX_HW_ID > 1UL is not supported"
#endif
};

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10ValidateParams(ETHXCVR_IDType aID)
{
#ifdef __BCM8957X__
    MCU_VersionExtType mcuVerExt = {0};
    MCU_GetVersionExt(&mcuVerExt);
#endif
    int32_t retVal;

    if (aID >= BRPHY1GV2_TEST_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
#ifdef __BCM8957X__
    } else if ((MCU_CHIP_FAMILY_8957X == mcuVerExt.family)
            && (MCU_CHIP_REVISION_A0 == mcuVerExt.rev)) {
        retVal = BCM_ERR_NOSUPPORT;
#endif
    } else {
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

/**
    @code{.unparsed}
    On 8957x platform, set DEVAD30_STRAP_REG_1 bit 13 through BRPHY1GV2_PlatformTc10Config
    On other platforms, set POWER_DOWN_RESET_SELECT bit of control_1_register_1 of BRPHY1G TEST block
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static void BRPHY1GV2_TC10ResetSelect(uint8_t aPhyIdx)
{
    /* TC10 reset to be treated as exp.70h soft reset
       Upon wake-up, register config(super-isolate etc.) aren't redone */
#if (ENABLE_BRPHY1G_TC10_RETENTION == 1UL)
    BRPHY1GV2_PlatformTc10Config();
#else
    BRPHY1GV2_TC10_TEST_REGS[aPhyIdx]->control_1_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_1_REG1_TC10_POWER_DOWN_RESET_SELECT_MASK ;
#endif
}

/**
    @code{.unparsed}
    if aID >= BRPHY1GV2_TEST_MAX_HW_ID
        return BCM_ERR_INVAL_PARAMS
    regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1
    regVal &= ~BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_INTR_MASK_MASK
    regVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_REMOTE_WUP_INTR_MASK
    regVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEP_INTR_MASK
    regVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK
    regVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_INTR_EN_MASK
    BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10Init(ETHXCVR_IDType aID,
                                  ETHXCVR_TC10PortContextType *const aCntxt,
                                  const ETHXCVR_TC10PortConfigType *const aConfig)
{
    int32_t retVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        BRPHY1GV2_TC10ResetSelect((uint8_t)aID);
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aID >= BRPHY1GV2_TEST_MAX_HW_ID
        return BCM_ERR_INVAL_PARAMS
    if (ETHXCVR_MODE_ACTIVE_WAKEUP == aMode)
        BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_WAKEUP_REQ_MASK
        aCntxt->stats.tc10LocalWakeUpTrigger++
    else
        if (ETHXCVR_MODE_DOWN == aMode)
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEP_REQ_MASK
            aCntxt->stats.tc10LocalSleepTrigger++
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10SleepReq(ETHXCVR_IDType aID,
                                      ETHXCVR_TC10PortContextType *const aCntxt)
{
    int32_t retVal;
    uint16_t regVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        /* send sleep-request */
        regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1;
        regVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEP_REQ_MASK;
        regVal &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK & 0xFFFFU);
        BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal;
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aID >= BRPHY1GV2_TEST_MAX_HW_ID
        return BCM_ERR_INVAL_PARAMS
    if super isolate is on in shd18_010
        clear SUPER_ISOLATE in shd18_010
        clear TC10_GO2STNDBY_CFG bit in control_2_register_1
    else
        set WAKEUP_REQ bit in tc10_control_0
    BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_WAKEUP_REQ_MASK

    if phy in TC10 sleep
        enable AFE interrupt (AFE_WUP_INTR) in control_register_1
        aCntxt->isLocalWakeUp = ETHXCVR_BOOLEAN_TRUE
    aCntxt->stats.tc10LocalWakeUpTrigger++
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10WakeUpReq(ETHXCVR_IDType aID,
                                       ETHXCVR_TC10PortContextType *const aCntxt)
{
    int32_t retVal;
    uint16_t regVal;
    uint16_t shd18RegVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        /* Read the register since wake-up may be triggered before Init */
        regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1;

        if (0U == (regVal & BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_DISABLE_CFG_MASK)) {
            shd18RegVal = BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010;
            if (0U != (shd18RegVal & BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK)) {
                /* clear go2Stndby */
                BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_TC10_GO2STNDBY_CFG_MASK & 0xFFFFU);

                /* Remove super-isolate */
                BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010 &= (uint16_t)(~BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK & 0xFFFFU);
            } else {
                /* super isolate has already been removed, issue wake-up request */
                BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_WAKEUP_REQ_MASK;
            }

            /* Enabling MDI wakeup */
            if (ETHXCVR_BOOLEAN_TRUE == aCntxt->allowMdiWake) {
                BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_WAKE_BY_WUP_CF_MASK & 0xFFFFU);
            }

            /* Enable AFE interrupt for local wake up (when phy is in sleep) */
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK;

            /*MCUDRV_UpdateTC10Status(aID, MCUDRV_TC10STATUS_LOCAL_WAKE_TRIGGER);*/
        } else {
            /* enable tx */
            BRPHY1GV2_TC10_PMAPMDIEEE_REGS[aID]->m9 &= (uint16_t)(~BRPHY1GV2_PMAPMDIEEE_TDR_GLOBAL_PMD_TRANSMIT_DISABLE_MASK & 0xFFFFU);

            /* Remove super-isolate */
            BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010 &= (uint16_t)(~BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK & 0xFFFFU);
        }
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10SleepAbortReq(ETHXCVR_IDType aID,
                                           ETHXCVR_TC10PortContextType *const aCntxt)
{
    int32_t retVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        /* Sleep Abort */
        BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK;
        BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK & 0xFFFFU);
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }
    return retVal;
}

/**
    @code{.unparsed}
    If aID is invalid
        return BCM_ERR_INVAL_PARAMS
    else
        reset isLocalWakeUp flag
        Set PCS_SLEEP_TRIG field of control_register_1 of BRPHY1GV2_TC10_TEST_REGS block
        Increment LocalSleepTrigger stats
        return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10ForceSleepReq(ETHXCVR_IDType aID,
                                           ETHXCVR_TC10PortContextType *const aCntxt)
{
    int32_t retVal;
    uint16_t regVal;
    uint16_t statusRegVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1;
        if (0U == (regVal & BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_DISABLE_CFG_MASK)) {

            /* Disabling MDI wake */
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_WAKE_BY_WUP_CF_MASK;

            /* Force Sleep */
            regVal &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK & 0xFFFFU);
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal | BRPHY1GV2_TEST_TC10_CONTROL_REG1_PCS_SLEEP_TRIG_MASK;
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal;
        } else {
            /* enable super isolate */
            BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010 |= BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK;

            /* disable tx */
            BRPHY1GV2_TC10_PMAPMDIEEE_REGS[aID]->m9 |= BRPHY1GV2_PMAPMDIEEE_TDR_GLOBAL_PMD_TRANSMIT_DISABLE_MASK;

            /* just to clear the status register */
            statusRegVal = BRPHY1GV2_TC10_TEST_REGS[aID]->status_register_1;

            /* clear psd wake flags */
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal | BRPHY1GV2_TEST_TC10_CONTROL_REG1_PSD_WAKE_FLAG_CLR_MASK;
            BCM_CpuNDelay(500UL);
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal & ((uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_REG1_PSD_WAKE_FLAG_CLR_MASK & 0xFFFFU));

            /* clear status register */
            statusRegVal = BRPHY1GV2_TC10_TEST_REGS[aID]->status_register_1;
            (void)statusRegVal;
        }
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10AllowSleepReq(ETHXCVR_IDType aID,
                                           ETHXCVR_TC10PortContextType *const aCntxt,
                                           ETHXCVR_BooleanType aMode)
{
    int32_t retVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK & 0xFFFFU);
        } else {
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK;
        }
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10AllowMdiWake(ETHXCVR_IDType aID,
                                          ETHXCVR_TC10PortContextType *const aCntxt,
                                          ETHXCVR_BooleanType aMode)
{
    int32_t retVal;
    uint16_t mask = BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_WAKE_BY_WUP_CF_MASK;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        aCntxt->allowMdiWake = aMode;
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
            /* If it gets enabled when in sleep - enable the AFE interrupt */
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK;

            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 &= (uint16_t)~mask;
        } else {
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 |= mask;
        }
    }
    return retVal;
}

/** @brief TC10 Enable Events

    This API enables events specific to TC10

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events enabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @code{.unparsed}
    if aID >= BRPHY1GV2_TEST_MAX_HW_ID
        return BCM_ERR_INVAL_PARAMS
    Enable all interrupts in control_register_1 register
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10EnableEvents(ETHXCVR_IDType aID,
                                          ETHXCVR_TC10PortContextType *const aCntxt)
{
    int32_t retVal;
    uint16_t regVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1;
        /* Enable TC10 interrupts */
        regVal &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_INTR_ENABLES_MASK & 0xFFFFU);
        regVal |= (BRPHY1GV2_TEST_TC10_CONTROL_REG1_REMOTE_WUP_FAIL_INTR_MASK
                  | BRPHY1GV2_TEST_TC10_CONTROL_REG1_REMOTE_WUP_INTR_MASK
                  | BRPHY1GV2_TEST_TC10_CONTROL_REG1_INHIBIT_INTR_MASK
                  | BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEP_FAIL_INTR_MASK
                  | BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEP_INTR_MASK
                  | BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_INTR_EN_MASK);

        regVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK;

        BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal;
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/** @brief TC10 Disable Events

    This API disables events specific to TC10

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events disabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @code{.unparsed}
    if aID >= BRPHY1GV2_TEST_MAX_HW_ID
        return BCM_ERR_INVAL_PARAMS
    Disable all interrupts in control_register_1 register
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10DisableEvents(ETHXCVR_IDType aID,
                                           ETHXCVR_TC10PortContextType *const aCntxt)
{
    int32_t retVal;
    uint16_t regVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1;
        /* Disable TC10 interrupts */
        regVal &= (uint16_t)(~(BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_INTR_ENABLES_MASK |
                               BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_INTR_EN_MASK) & 0xFFFFU);

        BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 = regVal;
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }
    return retVal;
}

/**
    @brief Function type to check if the given PHY is supported by driver.

    This function can be called by integration layer or application to check
    if the connected PHY is supported by this driver or not.
    This function can be called without initializing the PHY (driver implementation
    shall maintain the list of PHYs supported).
    Function shall return #BCM_ERR_OK is given PHY configuration is supported by the driver,
    else it shall return #BCM_ERR_NOSUPPORT.

    @param[in]     aConfig      Pointer to TC10 configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Driver supports the input configuration
    @retval     #BCM_ERR_NOSUPPORT     Driver doesn't support the input configuration

    @post None

    @limitations None

    @code
    If phyType is ETHXCVR_TC10PHY_INT_1000BASET1
        return BCM_ERR_OK
    Else return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10IsSupported(ETHXCVR_IDType aID,
                                         const ETHXCVR_TC10PortConfigType *const aConfig)
{
    int32_t retVal;

    if (ETHXCVR_TC10PHY_INT_1000BASET1 == aConfig->phyType) {
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

    return retVal;
}

/**
    Refer #ETHXCVR_ExtPhyTC10EnableProgramStateType

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10EnableProgramState(ETHXCVR_IDType aID,
                                        ETHXCVR_TC10PortContextType *const aCntxt,
                                        ETHXCVR_BooleanType aMode)
{
    int32_t retVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_EN_PHY_PRGM_CFG_MASK;
        } else {
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_EN_PHY_PRGM_CFG_MASK & 0xFFFFU);
        }
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/**
    Refer #ETHXCVR_ExtPhyTC10ExitHostModeType

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10ExitHostMode(ETHXCVR_IDType aID,
                                          ETHXCVR_TC10PortContextType *const aCntxt, ETHXCVR_WakeupReasonType wakeReason)
{
    int32_t retVal;
    uint16_t regVal;

    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        /* handle go2stndby */
        if (ETHXCVR_WAKEUPREASON_NONE == wakeReason) {
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_TC10_GO2STNDBY_CFG_MASK;
        } else {
            /* although not required, clearing the value just in case newer chipsets have it set by default */
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_2_register_1 &= (uint16_t)(~BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_TC10_GO2STNDBY_CFG_MASK & 0xFFFFU);
        }

        /* remove super-isolate */
        regVal = BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010;
        if (0U != (regVal & BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK)) {
            BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010 = regVal & (uint16_t)(~BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK & 0xFFFFU);
        }

        /* set program done bit to advance the state machine to one of awake or sleep states */
        BRPHY1GV2_TC10_TEST_REGS[aID]->control_1_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_1_REG1_PHY_PRGM_DONE_MASK;
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/**
    Refer #ETHXCVR_ExtPhyTC10EnableType

    @code{.unparsed}
    if aID is more than available brphys
        return BCM_ERR_INVAL_PARAMS
    if aMode is False
        retVal = BCM_ERR_NOSUPPORT
    set TC10_DISABLE_CFG bit in control_register_1 for the given phy index
    clear SUPER_ISOLATE bit in shd18_010
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_TC10Enable(ETHXCVR_IDType aID,
                                    ETHXCVR_TC10PortContextType *const aCntxt,
                                    ETHXCVR_BooleanType aMode)
{
    int32_t retVal;
    uint16_t regVal;

    /* Validate - no host mode check */
    retVal = BRPHY1GV2_TC10ValidateParams(aID);
    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_BOOLEAN_TRUE == aMode) {
            retVal = BCM_ERR_NOSUPPORT;
        } else {
            regVal = BRPHY1GV2_TC10_TEST_REGS[aID]->status_register_1;

            /* disable tc10 configuration */
            BRPHY1GV2_TC10_TEST_REGS[aID]->control_register_1 |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_TC10_DISABLE_CFG_MASK;
            /* remove super-isolate */
            BRPHY1GV2_TC10CORE_MISC_REGS[aID]->shd18_010 &= (uint16_t)(~BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK & 0xFFFFU);

            (void)regVal;
        }
#ifdef __BCM8957X__
    } else if (BCM_ERR_NOSUPPORT == retVal) {
        retVal = BCM_ERR_OK;
#endif
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
const ETHXCVR_ExtPhyTC10FuncTblType BRPHY1GV2_TC10FuncTbl = {
    .isSupported     = BRPHY1GV2_TC10IsSupported,
    .init            = BRPHY1GV2_TC10Init,
    .enPgmState      = BRPHY1GV2_TC10EnableProgramState,
    .exitHostMode    = BRPHY1GV2_TC10ExitHostMode,
    .enable          = BRPHY1GV2_TC10Enable,
    .wakeUpReq       = BRPHY1GV2_TC10WakeUpReq,
    .sleepReq        = BRPHY1GV2_TC10SleepReq,
    .forceSleepReq   = BRPHY1GV2_TC10ForceSleepReq,
    .sleepAbortReq   = BRPHY1GV2_TC10SleepAbortReq,
    .allowSleepReq   = BRPHY1GV2_TC10AllowSleepReq,
    .allowMdiWake    = BRPHY1GV2_TC10AllowMdiWake,
    .getStatus       = NULL,
    .getEventMask    = NULL,
    .getStats        = NULL,
    .getWakeUpReason = NULL,
    .enableEvents    = BRPHY1GV2_TC10EnableEvents,
    .disableEvents   = BRPHY1GV2_TC10DisableEvents,
};

/** @} */
