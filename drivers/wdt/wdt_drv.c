/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_wdtdrv_drv_impl Driver Implementation
    @ingroup grp_wdtdrv

    @addtogroup grp_wdtdrv_drv_impl
    @{
*/

#include <wdt_rdb.h>
#include <wdt.h>
#include <stdlib.h>
#include <bcm_err.h>
#include <compiler.h>
#include <bcm_utils.h>
#include <cfg_rdb.h>

/**
    @name Watchdog driver implementation IDs
    @{
    @brief Implementation IDs for Watchdog driver
*/
#define BRCM_SWDSGN_WDT_DRV_MS_MACRO                    (0xB201U)    /**< @brief #WDT_DRV_MS_PER_SEC         */
#define BRCM_SWDSGN_WDT_DRV_DEV_TYPE                    (0xB202U)    /**< @brief #WDT_DrvDevType             */
#define BRCM_SWDSGN_WDT_DRV_REGS_GLOBAL                 (0xB203U)    /**< @brief #WDT_DrvRegs                */
#define BRCM_SWDSGN_WDT_DRV_DEV_GLOBAL                  (0xB204U)    /**< @brief #WDT_DrvDev                 */
#define BRCM_SWDSGN_WDT_DRV_INTSETMODE_PROC             (0xB205U)    /**< @brief #WDT_IntDrvSetMode          */
#define BRCM_SWDSGN_WDT_DRV_INIT_PROC                   (0xB206U)    /**< @brief #WDT_DrvInit                */
#define BRCM_SWDSGN_WDT_DRV_SETMODE_PROC                (0xB207U)    /**< @brief #WDT_DrvSetMode             */
#define BRCM_SWDSGN_WDT_DRV_SETTRIGGERCONDITION_PROC    (0xB208U)    /**< @brief #WDT_DrvSetTriggerCondition */
#define BRCM_SWDSGN_WDT_DRV_SERVICE_PROC                (0xB209U)    /**< @brief #WDT_DrvService             */
#define BRCM_SWDSGN_WDT_DRV_MS_TO_TICK_MACRO            (0xB20AU)    /**< @brief #WDT_DRV_MS_TO_TICK         */
#define BRCM_SWDSGN_WDT_DRV_MS_FROM_TICK_MACRO          (0xB20BU)    /**< @brief #WDT_DRV_MS_FROM_TICK       */
#define BRCM_SWDSGN_WDT_DRV_MS_TIMEOUT_MAX_MACRO        (0xB20CU)    /**< @brief #WDT_DRV_MS_TIMEOUT_MAX     */
#define BRCM_SWDSGN_WDT_TOO_EARLY_CFG_PROC              (0xB20DU)    /**< @brief #WDT_DrvTooEarlyConfig      */
#define BRCM_SWDSGN_WDT_CFG_REGS_GLOBAL                 (0xB20EU)    /**< @brief #WDT_CfgRegs                */
#define BRCM_SWDSGN_WDT_DEINIT_PROC                     (0xB20FU)    /**< @brief #WDT_DrvDeInit              */
/** @} */

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
#define WDT_DRV_MS_PER_SEC              (1000UL)
/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_TOO_EARLY_CFG_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
#define WDT_DRV_MS_TO_TICK(time_ms)     ((uint32_t)((time_ms) * ((uint32_t)WDT_CLOCK / (uint32_t)WDT_DRV_MS_PER_SEC)) - 1UL)

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
#define WDT_DRV_MS_FROM_TICK(ticks)     (((uint32_t)((((ticks) + 1ULL) * (uint64_t)WDT_DRV_MS_PER_SEC) / (uint64_t) WDT_CLOCK)))

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
#define WDT_DRV_MS_TIMEOUT_MAX          WDT_DRV_MS_FROM_TICK(0xFFFFFFFFUL)

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef struct sWDT_DrvDevType {
    WDT_ModeType    currentMode;      /**< Current Mode of driver             */
    WDT_StateType   state;            /**< State of the driver                */
    WDT_CfgType     config;           /**< Configuration of the WDT device  */
} WDT_DrvDevType;

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static WDT_RDBType * const WDT_DrvRegs[WDT_NUM_HW_ID] =
{
#if (WDT_NUM_HW_ID == 0)
#error "WDT_NUM_HW_ID = 0"
#endif
    (WDT_RDBType *) WWDT0_BASE,
#if (WDT_NUM_HW_ID > 1)
    (WDT_RDBType *) WWDT1_BASE,
#endif
#if (WDT_NUM_HW_ID > 2)
    (WDT_RDBType *) WWDT2_BASE,
#endif
#if (WDT_NUM_HW_ID > 3)
    (WDT_RDBType *) WWDT3_BASE,
#endif
#if (WDT_NUM_HW_ID > 4)
    (WDT_RDBType *) WWDT4_BASE,
#endif
#if (WDT_NUM_HW_ID > 5)
    (WDT_RDBType *) WWDT5_BASE,
#endif
#if (WDT_NUM_HW_ID > 6)
    (WDT_RDBType *) WWDT6_BASE,
#endif
#if (WDT_NUM_HW_ID > 7)
    (WDT_RDBType *) WWDT7_BASE,
#endif
#if (WDT_NUM_HW_ID > 8)
#error "WDT_NUM_HW_ID > 8"
#endif
};

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static WDT_DrvDevType WDT_DrvDev[WDT_NUM_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_DEINIT_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static CFG_RDBType * const WDT_CfgRegs = (CFG_RDBType *)CFG_BASE;
/**
    @brief Internal function to set the watchdog mode

    Helper function to set the mode of WDT driver.
    Note that this function does not change the state of the
    driver and assumes that the WDT registers are unlocked.

    NOTE: This internal function makes the following assumptions:
          1) hwId, driver state are already validated by the caller.
          2) WDT registers are already unlocked by the caller.
          3) Caller shall lock WDT registers after calling this fucntion.

    @param[in]   hwId     WDT hardware ID
    @param[in]   aMode   mode to be set

    @retval      BCM_ERR_OK              Mode set successfully
    @retval      BCM_ERR_INVAL_PARAMS    Invalid mode

    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
    @code{.unparsed}
    If aMode is WDT_MODE_OFF and runtimeDisable is allowed
        Disable Counter and interrupt
        Clear the interrupt
        Move currentMode to WDT_MODE_OFF state
    If aMode is WDT_MODE_ACTIVE
        Enable Counter and interrupt
        Move currentMode to WDT_MODE_ACTIVE state
    @endcode
    @limitations None
*/
static int32_t WDT_IntDrvSetMode(WDT_HwIDType hwId, WDT_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    switch (aMode) {
    case WDT_MODE_OFF:
        /* Check if runtime disabling is allowed by configuration */
        if (WDT_DISABLE_ALLOWED == WDT_DrvDev[hwId].config.runtimeDisable) {
            /* Disable the counter and interrupt */
            WDT_DrvRegs[hwId]->wdogcontrol &= ~(WDT_WDOGCONTROL_INTEN_MASK |  WDT_WDOGCONTROL_RESEN_MASK);
            WDT_DrvRegs[hwId]->wdogcontrol &= ~(WDT_WDOGCONTROL_EARLY_INTEN_MASK | WDT_WDOGCONTROL_EARLY_RESEN_MASK);

            /* Clear the interrupt */
            WDT_DrvRegs[hwId]->wdogintclr = (WDT_WDOGINTCLR_MASK | WDT_WDOGINTCLR_WWDT_ATTEMPT_TOO_EARLY_CLEAR_MASK);

            WDT_DrvDev[hwId].currentMode = WDT_MODE_OFF;
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
        break;
    case WDT_MODE_ACTIVE:
        /* No need to change Control register as 
         * Legacy Mode for windowed WatchDog is not supported*/
        /*WDT_DrvRegs[hwId]->wdogcontrol = 0UL;*/

        /* Enable back the counter and interrupt */
        WDT_DrvRegs[hwId]->wdogcontrol &= (~WDT_WDOGCONTROL_INTEN_MASK);
        WDT_DrvRegs[hwId]->wdogcontrol |= (WDT_WDOGCONTROL_INTEN_MASK | WDT_WDOGCONTROL_RESEN_MASK);
        if (((WDT_DrvRegs[hwId]->wdogcontrol & WDT_WDOGCONTROL_WDOGMODE_MASK) >> WDT_WDOGCONTROL_WDOGMODE_SHIFT) == 0x1UL) { /* Early detect enabled*/
            WDT_DrvRegs[hwId]->wdogcontrol |= (WDT_WDOGCONTROL_EARLY_INTEN_MASK | WDT_WDOGCONTROL_EARLY_RESEN_MASK);
        }

        WDT_DrvDev[hwId].currentMode =  WDT_MODE_ACTIVE;
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

    return retVal;
}

/** @brief Initialize the given Watchdog Timer.

    This API initializes the given watchdog with the given configurations. If
    the passed hardware ID or configuration is/are invalid, this API returns
    without initializing the Watchdog Timer.

    @behavior Sync, Re-entrant for different hwId

    This API shall be called in #WDT_STATE_UNINIT state.
    
    @pre None

    @param[in]      hwId         Hardware ID
    @param[in]      aConfig     Pointer to watchdog configuration

    @return         int32_t

    @post NONE

    @trace  #BRCM_SWREQ_WDT_INIT
    @trace #BRCM_SWARCH_WDT_INIT_PROC

    @code{.unparsed}
        Validate hwId and aConfig
        Copy aConfig to global config
        Convert defaultTimeout_ms to ticks
        Unlock WDT registers
        Write Ticks
        Configure defaultMode
        Lock WDT register
        Set HW state to idle
    @endcode
    @limitations None
*/
int32_t WDT_DrvInit(WDT_HwIDType hwId, const WDT_CfgType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t ticks = 0UL, temp = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (hwId >= WDT_NUM_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((WDT_DISABLE_ALLOWED != aConfig->runtimeDisable)
                && (WDT_DISABLE_NOT_ALLOWED != aConfig->runtimeDisable)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((WDT_MODE_OFF != aConfig->defaultMode)
                && (WDT_MODE_ACTIVE != aConfig->defaultMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (WDT_DRV_MS_TIMEOUT_MAX < aConfig->defaultTimeout_ms) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (WDT_STATE_UNINIT != WDT_DrvDev[hwId].state) {
        retVal = BCM_ERR_NODEV;
    } else {
#endif
        BCM_MemCpy(&WDT_DrvDev[hwId].config, aConfig, sizeof(WDT_CfgType));
        if (0UL != WDT_DrvDev[hwId].config.defaultTimeout_ms) {
            ticks = WDT_DRV_MS_TO_TICK(WDT_DrvDev[hwId].config.defaultTimeout_ms);
        } else {
            ticks = 1UL;
        }

        WDT_CfgRegs->periph_pwrdn &= ~(1UL << (CFG_PERIPH_PWRDN_PWRDN_WDT_0_SHIFT - hwId));
        temp =  WDT_DrvRegs[hwId]->wdoglock;
       
        /* Unlock the WDT registers */
        WDT_DrvRegs[hwId]->wdoglock = WDT_REG_UNLOCK;

        /* Set the default timeout */
        WDT_DrvRegs[hwId]->wdogload = ticks;
        /* Set default mode */
        retVal = WDT_IntDrvSetMode(hwId,
                WDT_DrvDev[hwId].config.defaultMode);
        (void)temp;
        /* Lock the WDT registers */
        WDT_DrvRegs[hwId]->wdoglock = WDT_REG_LOCK;

        /* Move driver to IDLE state */
        WDT_DrvDev[hwId].state = WDT_STATE_IDLE;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return  retVal;
}

/** @brief De-initialize the given Watchdog Timer.

    This API de-initializes the given watchdog. On successful de-initialization,
    the watchdog shall be in uninitialized state.

    @behavior Sync, Re-entrant

    @pre None 

    @param[in]      hwId         Hardware ID

    @retval     #BCM_ERR_OK             De-init succeeded
    @retval     #BCM_ERR_INVAL_PARAMS   hwId is invalid

    @post None 

    @trace  #BRCM_SWREQ_WDT_INIT
    @trace  #BRCM_SWARCH_WDT_DEINIT_PROC

    @code{.unparsed}
        Validate hwId 
        Set HW State to WDT_STATE_UNINIT
    @endcode
    @limitations None
*/

int32_t WDT_DrvDeInit(WDT_HwIDType hwId)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= WDT_NUM_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* Move driver to uninit state */
        WDT_DrvDev[hwId].state = WDT_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return  retVal;
}

/** @brief Set mode of watchdog

    This API sets the mode of the given watchdog.

    @behavior Sync, Re-entrant for different hwId

    @pre None

    @param[in]      hwId         Hardware ID
    @param[in]      aMode       Mode to be set<BR>
                                If the watchdog configuration (passed during
                                driver initialization), do not permit runtime
                                disabling of watchdog, and the mode passed is
                                #WDT_MODE_OFF, this API returns
                                #BCM_ERR_INVAL_PARAMS.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Mode set successfully
    @retval     #BCM_ERR_INVAL_STATE    Watchdog Timer is not in idle state (#WDT_STATE_IDLE)
    @retval     #BCM_ERR_UNINIT         Driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Runtime disabling is not allowed by watchdog configuration
    @retval     #BCM_ERR_INVAL_PARAMS   hwId is invalid

    @post NONE
    @trace  #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @code{.unparsed}
        Validate hwId and HW state
        if HW state is WDT_STATE_IDLE
            Move HW state to WDT_STATE_BUSY
            Unlock WDT registers
            Configure aMode
            Lock WDT register
            Set HW state to idle
    @endcode

    @limitations None
*/
int32_t WDT_DrvSetMode(WDT_HwIDType hwId, WDT_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= WDT_NUM_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (WDT_STATE_UNINIT == WDT_DrvDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        if (WDT_STATE_IDLE == WDT_DrvDev[hwId].state) {
            /* Move to BUSY state */
            WDT_DrvDev[hwId].state = WDT_STATE_BUSY;

            /* Unlock the WDT registers */
            WDT_DrvRegs[hwId]->wdoglock = WDT_REG_UNLOCK;

            /* Call the internal function to actually set the mode */
            retVal = WDT_IntDrvSetMode(hwId, aMode);
            if (retVal == BCM_ERR_OK) {
                /* Lock the WDT registers */
                WDT_DrvRegs[hwId]->wdoglock = WDT_REG_LOCK;
                /* Move back to IDLE state */
                WDT_DrvDev[hwId].state = WDT_STATE_IDLE;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Set the watchdog trigger condition

    This API sets the trigger condition (i.e. the timeout value of the watchdog).

    @behavior Sync, Re-entrant for different hwId

    @pre None 

    @param[in]      hwId         Hardware ID
    @param[in]      aTimeout_ms New timeout value (in milliseconds) to be set.
                                If timeout value is zero, and the watchdog is
                                already active (#WDT_MODE_ACTIVE), the watchdog
                                timer expires immediately.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Timeout value set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid timeout (more than what the watchdog hardware can support)
    @retval     #BCM_ERR_INVAL_PARAMS   hwId is invalid
    @retval     #BCM_ERR_INVAL_STATE    Watchdog Timer is not in idle state (#WDT_STATE_IDLE)
    @retval     #BCM_ERR_UNINIT         Driver not initialized

    @post None

    @trace  #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @code{.unparsed}
        Validate hwId, aTimeout_ms and HW state
        if HW state is WDT_STATE_IDLE
            Move HW state to WDT_STATE_BUSY
            Unlock WDT registers
            Convert aTimeout_ms to ticks
            Program ticks
            Lock WDT register
            Set HW state to idle
    @endcode

    @limitations None
*/
int32_t WDT_DrvSetTriggerCondition(WDT_HwIDType hwId, uint32_t aTimeout_ms)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t ticks = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= WDT_NUM_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aTimeout_ms >= WDT_DRV_MS_TIMEOUT_MAX) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (WDT_STATE_UNINIT == WDT_DrvDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        if (WDT_STATE_IDLE == WDT_DrvDev[hwId].state) {
            /* Move to BUSY State */
            WDT_DrvDev[hwId].state = WDT_STATE_BUSY;

            /* Unlock the WDT registers */
            WDT_DrvRegs[hwId]->wdoglock = WDT_REG_UNLOCK;

            /*
             * Update the load register only if the current load register value
             * is non-zero.
             */
            if (0UL != WDT_DrvRegs[hwId]->wdogload) {
                if (0UL != aTimeout_ms) {
                    ticks = WDT_DRV_MS_TO_TICK(aTimeout_ms);
                } else {
                    ticks = 1UL;
                }
                WDT_DrvRegs[hwId]->wdogload = ticks;
            }

            /* Lock the WDT registers */
            WDT_DrvRegs[hwId]->wdoglock = WDT_REG_LOCK;

            /* Move back to IDLE State */
            WDT_DrvDev[hwId].state = WDT_STATE_IDLE;

        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @brief Service Watchdog

    This interface services the watchdog and restarts the watchdog timer.

    @behavior Sync, Re-entrant for different hwId

    @pre None

    @param[in]      hwId         Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Watchdog is serviced
    @retval     #BCM_ERR_UNINIT         Driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Watchdog ID is invalid

    @post None 

    @trace  #BRCM_SWREQ_WDT_SERVICE
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @code{.unparsed}
        Validate hwId and HW state
        if HW state is WDT_STATE_IDLE && currentMode is WDT_MODE_ACTIVE
            Move HW state to WDT_STATE_BUSY
            Unlock WDT registers
            Clear WDT interrupt
            Lock WDT register
            Set HW state to idle
    @endcode

    @limitations None
*/

int32_t WDT_DrvService(WDT_HwIDType hwId)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= WDT_NUM_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (WDT_STATE_UNINIT == WDT_DrvDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        if ((WDT_STATE_IDLE == WDT_DrvDev[hwId].state)
                && (WDT_MODE_ACTIVE == WDT_DrvDev[hwId].currentMode)) {
            /* Move to busy state */
            WDT_DrvDev[hwId].state = WDT_STATE_BUSY;

            /* Unlock the WDT registers */
            WDT_DrvRegs[hwId]->wdoglock = WDT_REG_UNLOCK;

            if ((WDT_DrvRegs[hwId]-> wdogris & WDT_WDOGINTCLR_MASK) > 0UL) {
                /* Clear the WDT interrupt */
                WDT_DrvRegs[hwId]->wdogintclr = WDT_WDOGINTCLR_MASK;
            }
            if ((WDT_DrvRegs[hwId]-> wdogris & WDT_WDOGRIS_WWDT_ATTEMPT_TOO_EARLY_MASK) > 0UL) {
                /* Clear early interrupt */
                WDT_DrvRegs[hwId]->wdogintclr = WDT_WDOGINTCLR_WWDT_ATTEMPT_TOO_EARLY_CLEAR_MASK;
            }

            /* Lock the WDT registers */
            WDT_DrvRegs[hwId]->wdoglock = WDT_REG_LOCK;

            /* Move driver back to idle state */
            WDT_DrvDev[hwId].state = WDT_STATE_IDLE;
            retVal = BCM_ERR_OK;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Watch dog Too early configuration 

    This interface allows user to configure threshold for too early and 
    enables/Disables watch dog with early detection.

    @behavior Sync, Re-entrant for different hwId

    @pre None

    @param[in]      hwId               Hardware ID.
    @param[in]      earlyThreshold     Threshold value for "too early"(in milliseconds).
    @param[in]      enDis              1 represents WDT enabled with too early detect.
                                       0 represents WDT enabled without too early detect.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Watchdog is serviced
    @retval     #BCM_ERR_UNINIT         Driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Watchdog ID is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid earlyThreshold

    @post None 

    @trace  #BRCM_SWREQ_WDT_SERVICE
    @trace  #BRCM_SWARCH_WDT_TOO_EARLY_CFG_PROC
    @code{.unparsed}
        Validate hwId HW state and earlyThreshold
        if enDis is set, configure earlyThreshold and enable early threshold interrupt
        else Disable early threshold interrupt
        Configure WDT mode.

    @endcode

    @limitations None
*/

int32_t WDT_DrvTooEarlyConfig(WDT_HwIDType hwId, uint32_t earlyThreshold, uint8_t enDis)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t ctrl = 0UL;
    uint32_t ticks = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= WDT_NUM_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (WDT_STATE_UNINIT == WDT_DrvDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else if (earlyThreshold >= 0x1000000) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (0UL != earlyThreshold) {
            ticks = WDT_DRV_MS_TO_TICK(earlyThreshold);
        } else {
            ticks = 1UL;
        }
        /* Unlock the WDT registers */
        WDT_DrvRegs[hwId]->wdoglock = WDT_REG_UNLOCK;


        ctrl = WDT_DrvRegs[hwId]->wdogcontrol;
        ctrl &= ~(WDT_WDOGCONTROL_WDOGMODE_MASK);

        if (enDis == 1U) {
            WDT_DrvRegs[hwId]->wdogtooearly = ticks;
            ctrl |= (WDT_WDOGCONTROL_EARLY_INTEN_MASK | WDT_WDOGCONTROL_EARLY_RESEN_MASK);
            ctrl |= (0x1UL << WDT_WDOGCONTROL_WDOGMODE_SHIFT);
        } else {
            ctrl &= ~(WDT_WDOGCONTROL_EARLY_INTEN_MASK | WDT_WDOGCONTROL_EARLY_RESEN_MASK);
            ctrl |= (0x3UL << WDT_WDOGCONTROL_WDOGMODE_SHIFT);
        }
        WDT_DrvRegs[hwId]->wdogcontrol = ctrl;
     
        /* Lock the WDT registers */
        WDT_DrvRegs[hwId]->wdoglock = WDT_REG_LOCK;


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @} */
