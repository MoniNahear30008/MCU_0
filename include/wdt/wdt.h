/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/


/**
  @defgroup grp_wdtdrv_ifc Watchdog Timer
  @ingroup grp_wdtdrv

  @addtogroup grp_wdtdrv_ifc
  @{
  @section sec_wdt_overview Overview
    Watchdog timers are implemented as 32-bit down counters. The timers can be
    initialized and set by the APIs #WDT_DrvInit. When a counter is
    loaded with the required timeout value (by #WDT_DrvSetTriggerCondition API)
    and enabled (by #WDT_DrvSetMode API), the watchdog timer starts counting
    down.
    If the watchdog is not serviced (using #WDT_DrvService API) within the
    timeout period, chip reset will be triggered.

   @section sec_wtd_program Programmable Parameters
    The following key parameters are programmable through the data structure
    #WDT_CfgType:
    - Watchdog runtimeDisable: Indicates whether runtime
                               disabling of Watchdog Timer is allowed.
    - Watchdog defaultMode:    Default mode of the watchdog driver.
                               If default mode is set to #WDT_MODE_ACTIVE,
                               watchdog timer is loaded with the
                               'defaultTimeout_ms'.
    - defaultTimeout_ms:       Default timeout value of the
                               watchdog timer (in milliseconds).
    -# WDT HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   WWDT0 |
       | 1           |   WWDT1 |
       | 2           |   WWDT2 |
       | 3           |   WWDT3 |
       | 4           |   WWDT4 |
       | 5           |   WWDT5 |
       | 6           |   WWDT6 |
       | 7           |   WWDT7 |


  @section api_list List of WDT APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref WDT_DrvInit | Initialize the given Watchdog Timer |
    | @ref WDT_DrvDeInit | De-initialize the given Watchdog Timer |
    | @ref WDT_DrvSetMode | Set mode of watchdog |
    | @ref WDT_DrvSetTriggerCondition | Set the watchdog trigger condition |
    | @ref WDT_DrvService | Service watchdog |
    | @ref WDT_DrvTooEarlyConfig | Configure and enable | 

  @file wdt.h
  @brief Watchdog Timer APIs
  @section apis This section defines the interface for Watchdog Timer driver
  @version 0.1 Imported from docx
*/

#ifndef WDT_H
#define WDT_H

#include <stdint.h>
#include <bcm_err.h>

/**
    @name Watchdog Timer API IDs
    @{
    @brief API IDs for Watchdog Timer
*/
#define BRCM_SWARCH_WDT_HWID_TYPE                     (0x8501U)    /**< @brief #WDT_HwIDType            */
#define BRCM_SWARCH_WDT_MODE_TYPE                     (0x8502U)    /**< @brief #WDT_ModeType            */
#define BRCM_SWARCH_WDT_STATE_TYPE                    (0x8503U)    /**< @brief #WDT_StateType           */
#define BRCM_SWARCH_WDT_DISABLE_TYPE                  (0x8504U)    /**< @brief #WDT_Disable_Type        */
#define BRCM_SWARCH_WDT_CFG_TYPE                      (0x8505U)    /**< @brief #WDT_CfgType             */
#define BRCM_SWARCH_WDT_INIT_PROC                     (0x8506U)    /**< @brief #WDT_DrvInit                */
#define BRCM_SWARCH_WDT_DEINIT_PROC                   (0x8507U)    /**< @brief #WDT_DrvDeInit              */
#define BRCM_SWARCH_WDT_SET_MODE_PROC                 (0x8508U)    /**< @brief #WDT_DrvSetMode             */
#define BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC    (0x8509U)    /**< @brief #WDT_DrvSetTriggerCondition */
#define BRCM_SWARCH_WDT_SERVICE_PROC                  (0x850AU)    /**< @brief #WDT_DrvService             */
#define BRCM_SWARCH_WDT_TOO_EARLY_CFG_PROC            (0x850BU)    /**< @brief #WDT_DrvTooEarlyConfig */
/** @} */

/**
    @brief Watchdog hardware ID type

    Watchdog hardware ID (Shall be less than #WDT_MAX_HW_ID)

    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef uint8_t WDT_HwIDType;

/**
    @name WDT_ModeType
    @{
    @brief Mode of the watchdog hardware

    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef uint8_t WDT_ModeType;          /**< @brief Watchdog hardware mode type */
#define WDT_MODE_OFF        (0U)       /**< @brief Watchdog hardware is off & value must be zero*/
#define WDT_MODE_ACTIVE     (1U)       /**< @brief Watchdog hardware is active */
/** @} */

/**
    @name WDT_StateType
    @{
    @brief State of watchdog Timer


    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef uint16_t WDT_StateType;         /**< @brief Watchdog state type */
#define WDT_STATE_UNINIT    (0U)       /**< @brief Watchdog Timer is uninitialized & value must be zero*/
#define WDT_STATE_IDLE      (1U)       /**< @brief Watchdog Timer is idle */
#define WDT_STATE_BUSY      (2U)       /**< @brief Watchdog Timer is busy */
/** @} */

/**
    @name Watchdog disabling
    @{
    @brief Runtime disabling of Watchdog Timer

    Indicates whether runtime disabling of Watchdog Timer is allowed or not.

    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef uint8_t WDT_Disable_Type;
#define WDT_DISABLE_ALLOWED     (0U)   /**< @brief Runtime disabling of
                                             Watchdog Timer is allowed */
#define WDT_DISABLE_NOT_ALLOWED (1U)   /**< @brief Runtime disabling of
                                             Watchdog Timer is not allowed */
/** @} */

/**
    @brief Watchdog Timer configuration

    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef struct sWDT_CfgType {
    uint32_t runtimeDisable;            /**< @brief Indicates whether runtime
                                             disabling of Watchdog Timer is allowed:
                                             - #WDT_DISABLE_ALLOWED or
                                             - #WDT_DISABLE_NOT_ALLOWED */
    WDT_ModeType defaultMode;           /**< @brief Default mode of the watchdog
                                             driver. If default mode is set to
                                             #WDT_MODE_ACTIVE, watchdog timer is
                                             loaded with the defaultTimeout_ms */
    uint32_t defaultTimeout_ms;         /**< @brief Default timeout value of the
                                             watchdog timer (in milliseconds) */
} WDT_CfgType;

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

    @limitations None
*/
extern int32_t WDT_DrvInit(WDT_HwIDType hwId, const WDT_CfgType *const aConfig);

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

    @limitations None
*/
extern int32_t WDT_DrvDeInit(WDT_HwIDType hwId);

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

    @limitations None
*/
extern int32_t WDT_DrvSetMode(WDT_HwIDType hwId, WDT_ModeType aMode);

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

    @limitations None
*/
extern int32_t WDT_DrvSetTriggerCondition(WDT_HwIDType hwId, uint32_t aTimeout_ms);

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

    @limitations None
*/
extern int32_t WDT_DrvService(WDT_HwIDType hwId);

/** @brief Watch dog Too early configuration 

    This interface allows user to configure threshold for too early and 
    enables/disables watch dog with early detection.

    @behavior Sync, Re-entrant for different hwId

    @pre None

    @param[in]      hwId               Hardware ID.
    @param[in]      earlyThreshold     Threshold value for "too early" (in milliseconds).
    @param[in]      enDis              1 represents WDT enabled with too early detect.
                                       0 represents WDT enabled without too early detect.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Watchdog is serviced
    @retval     #BCM_ERR_UNINIT         Driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Watchdog ID is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid earlyThreshold

    @post None 

    @trace  #BRCM_SWREQ_WDT_SERVICE

    @limitations None
*/

int32_t WDT_DrvTooEarlyConfig(WDT_HwIDType hwId, uint32_t earlyThreshold, uint8_t enDis);

#endif  /* WDT_H */
/** @} */
