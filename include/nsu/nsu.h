/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_nsudrv_ifc Network Synchronization Unit (NSU)
    @ingroup grp_nsudrv

    @addtogroup grp_nsudrv_ifc
    @{
    @section sec_nsu_overview Overview
    @note
    -# Network synchronization unit Bare Metal driver interface provides layer 1 APIs to initialize, configure
    the NSU in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each NSU HW instance needs to be initialized separately using NSU_DrvInit API.

    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_nsudrv_fsm Usage
    -# NSU HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   NSU |

    @section api_list List of NSU APIs
    | API Name | Brief Description   |
    |:-----------:|:-------------:|
    | @ref NSU_DrvInit | Initialize NSU |
    | @ref NSU_DrvDeInit | De-Initialize NSU |
    | @ref NSU_DrvResetSrcConfig   | Reset source configuration|
    | @ref NSU_DrvReset            | Reset NSU|
    | @ref NSU_DrvControl          | Enables/Disables NSU|
    | @ref NSU_DrvReadBinCounter   | Read binary counter |
    | @ref NSU_DrvRead1588Counter  | Read 1588 counter|
    | @ref NSU_DrvEventCaptureCtrl | Configure event capture|
    | @ref NSU_DrvReadEventCapTs   | Read event capture timestamp|
    | @ref NSU_DrvEventGenCtrl     | Configure event generator|
    | @ref NSU_DrvWrEventGenCounter| Configure event generator counter|
    | @ref NSU_DrvSystemTickCtrl   | Configure system tick|
    | @ref NSU_DrvWrSysTickCounter | Configure system tick counter|

    @file nsu.h
    @brief Interface for NSU driver
    @section apis This section defines the interface for NSU driver
    @version 0.1 Initial Version
*/

#ifndef NSU_H
#define NSU_H

#include <stdint.h>

/**
    @name NSU driver API IDs
    @{
    @brief API IDs for NSU drier
 */

#define BRCM_SWARCH_NSU_HW_ID_TYPE                             (0x8601U)    /**< @brief #NSU_HwIDType */
#define BRCM_SWARCH_NSU_DRV_INIT_PROC                          (0x8602U)    /**< @brief #NSU_DrvInit  */
#define BRCM_SWARCH_NSU_DRV_DEINIT_PROC                        (0x8603U)    /**< @brief #NSU_DrvDeInit  */
#define BRCM_SWARCH_NSU_DRV_RESET_SRC_CONFIG_PROC              (0x8604U)    /**< @brief #NSU_DrvResetSrcConfig   */
#define BRCM_SWARCH_NSU_DRV_RESET_PROC                         (0x8605U)    /**< @brief #NSU_DrvReset            */
#define BRCM_SWARCH_NSU_DRV_CONTROL_PROC                       (0x8606U)    /**< @brief #NSU_DrvControl          */
#define BRCM_SWARCH_NSU_DRV_READ_BIN_COUNTER_PROC              (0x8607U)    /**< @brief #NSU_DrvReadBinCounter   */
#define BRCM_SWARCH_NSU_DRV_READ_1588_COUNTER_PROC             (0x8608U)    /**< @brief #NSU_DrvRead1588Counter  */
#define BRCM_SWARCH_NSU_DRV_EVENT_CAPTURE_CTRL_PROC            (0x8609U)    /**< @brief #NSU_DrvEventCaptureCtrl */
#define BRCM_SWARCH_NSU_DRV_READ_EVENT_CAP_TS_PROC             (0x860AU)    /**< @brief #NSU_DrvReadEventCapTs   */
#define BRCM_SWARCH_NSU_DRV_EVENT_GENCTRL_PROC                 (0x860BU)    /**< @brief #NSU_DrvEventGenCtrl     */
#define BRCM_SWARCH_NSU_DRV_WREVENT_GEN_COUNTER_PROC           (0x860CU)    /**< @brief #NSU_DrvWrEventGenCounter */
#define BRCM_SWARCH_NSU_DRV_SYSTEM_TICK_CTRL_COUNTER_PROC      (0x860DU)    /**< @brief #NSU_DrvSystemTickCtrl   */
#define BRCM_SWARCH_NSU_DRV_WR_SYSTICK_COUNTER_PROC            (0x860EU)    /**< @brief #NSU_DrvWrSysTickCounter */
#define BRCM_SWARCH_NSU_STATE_TYPE                             (0x860FU)    /**< @brief #NSU_StateType */

/** @} */

/**
    @brief Index of the NSU controller

    @trace #BRCM_SWREQ_NSU_INIT
 */
typedef uint32_t NSU_HwIDType;

/**
    @name NSU_StateType
    @{
    @brief State of NSU

    @trace #BRCM_SWREQ_NSU_INIT
*/

typedef uint32_t NSU_StateType;                /**< @brief NSU state type */
#define NSU_STATE_UNINIT           (0UL)       /**< @brief NSU is uninitialized & value must be zero*/
#define NSU_STATE_INITIALIZED      (1UL)       /**< @brief NSU is initialized */
/** @} */

/** @brief Initialize NSU

    This API initializes the given NSU hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller

    @return     int32_t

    @post None

    @trace  #BRCM_SWREQ_NSU_INIT

    @limitations None
*/

int32_t NSU_DrvInit(NSU_HwIDType  hwId);

/** @brief De-initialize NSU

    This API de-initializes NSU. Resets the given NSU controller and put back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_NSU_INIT

    @limitations None
*/
int32_t NSU_DrvDeInit (NSU_HwIDType  hwId);

/** @brief NSU reset configuration.

    This API configures reset source.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[in]      nsuRstSrc    0 : NSU counters are reset by software only
                                 1 : NSU counters are reset by both software
                                     and hardware input (nsu_hw_rst).

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_RESET

    @limitations None
*/
int32_t NSU_DrvResetSrcConfig(NSU_HwIDType hwId, uint8_t nsuRstSrc);

/** @brief NSU reset.

    This API resets NSU binary, 1588 and systick counters.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_RESET

    @limitations None
*/
int32_t NSU_DrvReset(NSU_HwIDType hwId);

/** @brief NSU enables or disables.

    This API Enables or disables NSU.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[in]      nsuEnDis     0 : Disables Network Synchronization Unit.
                                 1 : Enables Network Synchronization Unit.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_RESET

    @limitations None
*/
int32_t NSU_DrvControl(NSU_HwIDType hwId, uint8_t nsuEnDis);

/** @brief NSU reads binary NS counter.

    This API reads current binary nanosecond counter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[out]     lwNsCount    32-bit lower binary nanosecond counter
    @param[out]     hiNsCount    32-bit upper binary nanosecond counter

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid lwNsCount or hiNsCount
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_COUNTER
    @limitations None
*/
int32_t NSU_DrvReadBinCounter(NSU_HwIDType hwId, uint32_t *lwNsCount, uint32_t *hiNsCount);

/** @brief NSU reads 1588 NS counter.

    This API reads 1588 nanosecond counter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[out]     nsCount      32-bit 1588 nanosecond counter
    @param[out]     secCount     32-bit 1588 second counter

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid nsCount or secCount
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_COUNTER
    @limitations None
*/
int32_t NSU_DrvRead1588Counter(NSU_HwIDType hwId, uint32_t *nsCount, uint32_t *secCount);

/** @brief Event capture Control.

    This API Enables/Disables event capture for the specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the NSU controller
    @param[in]      chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[in]      eventCapEnDis 0 : Disable event capture
                                  1 : Enable event capture

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @limitations None
*/
int32_t NSU_DrvEventCaptureCtrl(NSU_HwIDType hwId, uint8_t chanMask, uint8_t eventCapEnDis);

/** @brief Event capture Timestamp counter.

    This API reads nano second binary time stamp captured on event trigger
    for specified channel mask.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the NSU controller
    @param[in]      chanMask     Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[out]     lwNsCount    Base Address of an array of size eight uint32_t, 32-bit lower binary nanosecond counter
    @param[out]     upNsCount    Base Address of an array of size eight uint32_t, 32-bit upper binary nanosecond counter

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid lwNsCount or hiNsCount
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @limitations None
*/
int32_t NSU_DrvReadEventCapTs(NSU_HwIDType hwId, uint8_t chanMask, uint32_t *lwNsCount, uint32_t *upNsCount);

/** @brief Event Generation Control.

    This API Enables/Disables event generation for the specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the NSU controller
    @param[in]      chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[in]      eventCapEnDis 0 : Disables event generation
                                  1 : Enables event generation

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @limitations None
*/
int32_t NSU_DrvEventGenCtrl(NSU_HwIDType hwId, uint8_t chanMask, uint8_t eventGenEnDis);

/** @brief Event generation nanosecond counter.

    This API writes nanosecond binary counter at which channel x event must be generated.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId          Index of the NSU controller
    @param[in]     chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 7 represents channel 7.
    @param[in]     lwCmptimeNs   Base Address of an array of size eight uint32_t, lower 32-bit nanosecond binary counter
    @param[in]     upCmptimeNs   Base Address of an array of size eight uint32_t, upper 32-bit nanosecond binary counter
                                 [NOTE: Event generation happens when both lower and upper matches with NSU counter]

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid lwCmptimeNs or upCmptimeNs
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_EVENT
    @limitations None
*/
int32_t NSU_DrvWrEventGenCounter(NSU_HwIDType hwId, uint8_t chanMask, uint32_t *lwCmptimeNs, uint32_t *upCmptimeNs);

/** @brief System tick Control.

    This API Enables/Disables system ticks for specified channels.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the NSU controller
    @param[in]      chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 3 represents channel 3.
    @param[in]      tickEnDis     0 : Disables system tick
                                  1 : Enables system tick

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid chanMask
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_SYSTICK
    @limitations None
*/
int32_t NSU_DrvSystemTickCtrl(NSU_HwIDType hwId, uint8_t chanMask, uint8_t tickEnDis);

/** @brief System Tick counter value.

    This API writes system tick counter value for the specified channel(s).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId          Index of the NSU controller
    @param[in]     chanMask      Channel mask, Bit 0 represents channel 0
                                  bit 3 represents channel 3.
    @param[in]     tickCnt       Base Address of an array of size four uint32_t, Systick counter reload value in 10ns multiple. The counter increments every 10ns.
                                 The value programmed should be 1 less than the target value.
                                 For example, for 10us, value programmed should be 990 or 0x3DE

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid  tickCnt
    @return     #BCM_ERR_NODEV          NSU instance not initialized

    @post None

    @trace  #BRCM_SWREQ_NSU_SYSTICK
    @limitations None
*/
int32_t NSU_DrvWrSysTickCounter(NSU_HwIDType hwId, uint8_t chanMask, uint32_t *tickCnt);

#endif /* NSU_H */

/** @} */
