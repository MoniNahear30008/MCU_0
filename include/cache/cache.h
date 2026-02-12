/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential

 ******************************************************************************/
/**
    @defgroup grp_cachedrv_ifc Codacache
    @ingroup grp_cachedrv

    @addtogroup grp_cachedrv_ifc
    @{
    @section sec_cache_overview Overview
    -# Cache Bare Metal driver interface provides layer 1 APIs to initialize and configure
    the coda cache in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each cache HW instance needs to be initialized separately using CACHE_DrvInit API.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_cachedrv_fsm Usage
    -# Cache HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   CACHE_0 |

    @section api_list List of CACHE APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref CACHE_DrvInit | Initialize Cache |
    | @ref CACHE_DrvDeInit | De-initialize Cache |
    | @ref CACHE_DrvConfigure | Configure Cache |
    | @ref CACHE_DrvIntrConfig | Configure cache Interrupt    |
    | @ref CACHE_DrvIntrStatus  |  Get Cache interrupt Status  |
    | @ref CACHE_DrvIntrThresholdConfig  |Configure interrupt Threshold |
    | @ref CACHE_DrvIntrClear      | Clear cache interrrupt |
    | @ref CACHE_DrvConfigureWayPartition | Configure Way partition|
    | @ref CACHE_DrvEnableWayPartition | Enable Way Partition|
    | @ref CACHE_DrvMaintenanceControl | Configure and control Maintenance|
    | @ref CACHE_DrvMaintenanceStatus | Get maintenance status|
    | @ref CACHE_DrvFscControl | Fault safety control configuration|
    | @ref CACHE_DrvFscStatus | Fault safety control status|
    | @ref CACHE_DrvOverrideCachePolicy | Cache override policy|
    | @ref CACHE_DrvControl | Enable/Disable cache lookup|

    @file cache.h
    @brief Interface for cache driver
    @section apis This section defines the interface for Cache driver
    @version 0.1 Initial Version
*/

#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

/**
    @name Cache driver API IDs
    @{
    @brief API IDs for Cache drier
 */
#define BRCM_SWARCH_CACHE_HW_ID_TYPE                (0x8501U)    /**< @brief #CACHE_HwIDType     */
#define BRCM_SWARCH_CACHE_INTR_TYPE                 (0x8502U)    /**< @brief #CACHE_IntrType  */
#define BRCM_SWARCH_CACHE_SCRATCH_PAD_CFG_TYPE      (0x8503U)    /**< @brief #CACHE_ScratchPadConfigType  */
#define BRCM_SWARCH_CACHE_MAINTENANCE_OP_TYPE       (0x8504U)    /**< @brief #CACHE_MaintenanceOpType */
#define BRCM_SWARCH_CACHE_WAY_PARTITION_CONFIG_TYPE (0x8505U)    /**< @brief #CACHE_WayPartitionConfigType*/
#define BRCM_SWARCH_CACHE_MAINTENANCE_CONTROL_TYPE  (0x8506U)    /**< @brief #CACHE_MaintenanceControlType*/

#define BRCM_SWARCH_CACHE_CONFIGURE_WAY_PARTITION_PROC (0x8507U) /**< @brief #CACHE_DrvConfigureWayPartition*/
#define BRCM_SWARCH_CACHE_ENABLE_WAY_PARTITION_PROC (0x8508U)    /**< @brief #CACHE_DrvEnableWayPartition*/
#define BRCM_SWARCH_CACHE_MAINTENANCE_CONTROL_PROC  (0x8509U)    /**< @brief #CACHE_DrvMaintenanceControl*/
#define BRCM_SWARCH_CACHE_MAINTENANCE_STATUS_PROC   (0x850AU)    /**< @brief #CACHE_DrvMaintenanceStatus*/

#define BRCM_SWARCH_CACHE_DRV_INIT_PROC             (0x850BU)    /**< @brief #CACHE_DrvInit        */
#define BRCM_SWARCH_CACHE_DRV_DEINIT_PROC           (0x850DU)    /**< @brief #CACHE_DrvDeInit      */
#define BRCM_SWARCH_CACHE_DRV_CONFIGURE_PROC        (0x850EU)    /**< @brief #CACHE_DrvConfigure   */
#define BRCM_SWARCH_CACHE_DRV_INTR_ENABLE_PROC      (0x850FU)    /**< @brief #CACHE_DrvIntrConfig      */
#define BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC         (0x8510U)    /**< @brief #CACHE_DrvIntrStatus      */
#define BRCM_SWARCH_CACHE_DRV_INTR_THOLD_CFG_PROC   (0x8511U)    /**< @brief #CACHE_DrvIntrThresholdConfig  */
#define BRCM_SWARCH_CACHE_DRV_INTR_CLEAR_PROC       (0x8512U)    /**< @brief #CACHE_DrvIntrClear      */
#define BRCM_SWARCH_CACHE_DRV_FSC_CONTROL_PROC      (0x8513U)    /**< @brief #CACHE_DrvFscControl      */
#define BRCM_SWARCH_CACHE_DRV_FSC_STATUS_PROC       (0x8514U)    /**< @brief #CACHE_DrvFscStatus      */
#define BRCM_SWARCH_CACHE_DRV_CONTROL_PROC          (0x8515U)    /**< @brief #CACHE_DrvControl */
#define BRCM_SWARCH_CACHE_STATE_TYPE                (0x8516U)    /**< @brief #CACHE_StateType*/
#define BRCM_SWARCH_CACHE_DRV_OVERRIDE_CACHE_POLICY_PROC    (0x8517U)    /**< @brief #CACHE_DrvOverrideCachePolicy*/
#define BRCM_SWARCH_CACHE_POLICY_TYPE                       (0x8518U)    /**< @brief #CACHE_PolicyType*/


/** @} */

/**
    @brief Index of the Cache controller

    @trace #BRCM_SWREQ_CACHE_INIT
 */
typedef uint32_t CACHE_HwIDType;

/**
    @name CACHE_IntrType
    @{
    @brief Cache Interrupts

    @trace #BRCM_SWREQ_CACHE_INTERRUPT
 */
typedef uint16_t CACHE_IntrType;                                /**< @brief Cache interrupt type */

#define CACHE_INTR_UNCORR_ERR_DET               (0x1UL)        /**< @brief Uncorrectable errors detected, High Level Interrupt Enable*/
#define CACHE_INTR_MAINTENANCE_PERORMANCE_COMP  (0x2UL)        /**< @brief Maintenance/Performance monitor Complete, High Level Interrupt Enable*/
#define CACHE_INTR_CORR_ERR_DET                 (0x4UL)        /**< @brief Correctable error is detected on mem access, High Pulse Interrupt Enable*/
#define CACHE_INTR_CORR_ERR_ABV_THOLD           (0x8UL)        /**< @brief Correctable error is above a threshold High Level Interrupt Enable */
#define CACHE_INTR_ALL                          (0xFUL)        /**< @brief All the above intterrupt */
#define CACHE_INTR_EVENT_ALL                    (0x100UL)      /**< @brief All above as event */

/** @} */

/**
    @name CACHE_StateType
    @{
    @brief State of CACHE

    @trace #BRCM_SWREQ_CACHE_INIT
*/

typedef uint32_t CACHE_StateType;                /**< @brief CACHE state type */
#define CACHE_STATE_UNINIT           (0UL)       /**< @brief CACHE is uninitialized & value must be zero*/
#define CACHE_STATE_INITIALIZED      (1UL)       /**< @brief CACHE is initialized */
/** @} */

/**
    @name CACHE_MaintenanceOpType
    @{
    @brief Cache Maintenance operation

    @trace #BRCM_SWREQ_CACHE_INIT
 */
typedef uint8_t CACHE_MaintenanceOpType;                 /**< @brief Cache maintenance Operation */

#define CACHE_MAINTENANCE_OP_INIT_VALID_ENT             (0x0U) /**< @brief Initialize Valid Entries   */
#define CACHE_MAINTENANCE_OP_FLUSH_VALID_ENT            (0x4U) /**< @brief Flush Valid Entries   */
#define CACHE_MAINTENANCE_OP_FLUSH_ENT_SET_WAY          (0x5U) /**< @brief Flush Entry at Set and way   */
#define CACHE_MAINTENANCE_OP_FLUSH_ENT_ADDR             (0x6U) /**< @brief Flush Entry at address  */
#define CACHE_MAINTENANCE_OP_FLUSH_ADDR_RANGE           (0x7U) /**< @brief Flush Address Range   */
#define CACHE_MAINTENANCE_OP_FLUSH_SET_WAY_RANGE        (0x8U) /**< @brief Flush Set way range*/
/** @} */

/**
    @brief ScratchPad configuration

    @trace #BRCM_SWREQ_CACHE_CONFIGURE
*/
typedef struct sCACHE_ScratchPadConfigType {
    uint8_t enable;                 /**< @brief Enable/Disable ScratchPad  0 - Disable 1 - Enable*/
    uint8_t numPadWay;              /**< @brief Desired number of ways to be used as scratchpad minus 1. */
    uint32_t size;                  /**< @brief Scratchpad size in number of cachelines minus 1  */
    uint32_t baseAddress;           /**< @brief Scratchpad base address. Should be cacheline size aligned */
    uint32_t baseAddressHi;             /**< @brief Bit 0 and 1 of Scratchpad base address hi bits */
} CACHE_ScratchPadConfigType;

/**
    @brief Way partition configuration

    @trace #BRCM_SWREQ_CACHE_CONFIGURE
*/
typedef struct sCACHE_WayPartitionConfigType {
    uint8_t agentId;              /**< @brief Identify Way partition agent Id */
    uint16_t wayVector;           /**< @brief Way enable Vector,This vector specifies ways to which
                                        the source ID identified by the associated Way partition AgentID
                                        can allocate to  */
} CACHE_WayPartitionConfigType;

/**
    @brief Cache maintenance configuration and control

    @trace #BRCM_SWREQ_CACHE_INIT
*/
typedef struct sCACHE_MaintenanceControlType {
    CACHE_MaintenanceOpType maintenanceOperation; /**< @brief Maintenance opertation type*/
    uint16_t word;                                /**< @brief Refer Maintenance opertaion table in TRM */
    uint16_t set;                                 /**< @brief Refer Maintenance opertaion table in TRM */
    uint8_t way;                                  /**< @brief Refer Maintenance opertaion table in TRM */
    uint16_t hiAddr;                              /**< @brief Refer Maintenance opertaion table in TRM */
    uint16_t range;                               /**< @brief Refer Maintenance opertaion table in TRM */
    uint8_t arrayId;                              /**< @brief Initialize based on 0 : Tag Array 1 : Data Array*/
} CACHE_MaintenanceControlType;

/**
    @name CACHE_PolicyType
    @{
    @brief cache policy

    @trace #BRCM_SWREQ_CACHE_INIT
*/
typedef uint32_t CACHE_PolicyType;                /**< @brief CACHE policy type */
#define CACHE_POLICY_NO_OVERRIDE    0x00UL        /**< @brief No override of cache policy*/
#define CACHE_POLICY_BUFFERABLE     0x01UL        /**< @brief Indicates that the data has to buffered in cache*/
#define CACHE_POLICY_MODIFY         0x02UL        /**< @brief Indicates that the transaction can be modifiable transaction*/
#define CACHE_POLICY_CACHEABLE      0x04UL        /**< @brief Indicates that Data is allocated into a cache for future use.*/
#define CACHE_POLICY_ALLOC          0x08UL        /**< @brief Indicates that Data is not allocated into a cache for future use.*/

/** @} */


/** @brief Initialize Cache

    This API initializes the given Cache hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Cache controller

    @return     int32_t

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INIT

    @limitations None
*/

int32_t CACHE_DrvInit(CACHE_HwIDType  hwId);

/** @brief De-Initialize Cache

    This API de-initializes the specified cache

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the cache controller

    @return     #BCM_ERR_OK               De-Initialized cache instance
    @return     #BCM_ERR_INVAL_PARAMS     Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INIT

    @limitations None
*/
int32_t CACHE_DrvDeInit(CACHE_HwIDType  hwId);

/** @brief Configure Way partition

    This API configure way partition and way vector.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the cache
    @param[in]      partitionId      Cache Partition ID (0-3)
    @param[in]      partitionConfig  Way Partition configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Null pointer for partitionConfig
    @return     #BCM_ERR_INVAL_PARAMS   Invalid partition ID

    @post None
    @trace  #BRCM_SWREQ_CACHE_CONFIGURE
    @limitations None
*/
int32_t CACHE_DrvConfigureWayPartition(CACHE_HwIDType hwId, uint8_t partitionId, CACHE_WayPartitionConfigType *partitionConfig);

/** @brief Enable Way Partition

    This API enables/disables way partition.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the cache
    @param[in]      partitionId      Cache Partition ID (0-3)
    @param[in]      enDis            Enables(1) or Disables(0) way partition

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid partition ID
    @return     #BCM_ERR_INVAL_PARAMS   Invalid value for enDis

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE

    @limitations None
*/

int32_t CACHE_DrvEnableWayPartition(CACHE_HwIDType hwId, uint8_t partitionId, uint8_t enDis);

/** @brief Configure specified Cache

    This API configure specified cache with following features based on specified input,
            - ScratchPad
            - Error Detection and
            - Partial Write Allocation

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the cache controller
    @param[in]      errDetection          Enable(1)/Disable(0) Error Detaction
    @param[in]      wrAllocatePartialEn   Enable(1)/Disable(0) Partial write allocation
    @param[in]      scratchPadConfig      ScratchPad Configuration

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_NODEV            Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS     Invalid HW Index
    @return     #BCM_ERR_INVAL_PARAMS     Memory not valid for scratchPadConfig

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE

    @limitations None
*/

int32_t CACHE_DrvConfigure(CACHE_HwIDType hwId, uint8_t errDetection, uint8_t wrAllocatePartial, CACHE_ScratchPadConfigType *scratchPadConfig);

/** @brief Enable/Disable specified Cache

    This API configures cache miss and cache lookup on a specified cache.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the cache controller
    @param[in]      fillEn          If the bit is set to 1, cache allocate on miss is enabled else(0) it is not enabled.
    @param[in]      lookupEn        If the bit is set to 1, the cache is accessed, else(0) it is bypassed.

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware ID
    @return     #BCM_ERR_INVAL_PARAMS   Invalid value for fillEn or lookupEn

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE

    @limitations None
*/

int32_t CACHE_DrvControl(CACHE_HwIDType hwId, uint8_t fillEn, uint8_t lookupEn);

/** @brief Cache Maintenance operation

    This API perform cache maintenance operation based on user specified operation.
    User also need to configure Address, Way, Set and range based on the operation choosed.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the cache
    @param[in]      maintenanceControl Maintenance configuration and control

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for maintenanceControl

    @post None

    @trace  #BRCM_SWREQ_CACHE_MAINTENANCE_OP

    @limitations None
*/
int32_t CACHE_DrvMaintenanceControl(CACHE_HwIDType hwId, CACHE_MaintenanceControlType *maintenanceControl);

/** @brief Cache Maintenance operation status

    This API retrieve cache maintenance operation status, Alternatively user can use @ref CACHE_DrvIntrConfig to enable interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the cache.
    @param[out]     maintenanceSts     Maintenance status, This is set(1) when any cache maintenance operation is in progress and clear(0) otherwise.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for maintenanceSts

    @post None

    @trace  #BRCM_SWREQ_CACHE_MAINTENANCE_OP
    @limitations None
*/
int32_t CACHE_DrvMaintenanceStatus(CACHE_HwIDType hwId, uint8_t *maintenanceSts);

/** @brief Set Correctable Interrupt Threshold

    This API set the correctable Error Threshold value for CACHE_INTR_MEM_CORR_ERR interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      errThreshold           8-bit threshold value

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @limitations None
*/
int32_t CACHE_DrvIntrThresholdConfig(CACHE_HwIDType hwId, uint8_t  errThreshold);

/** @brief FSC control

    This API allows user to enable or disable fault safety controller to monitor and logs the faults.
    It also perform step by step BIST or all 5 step at a time in sequence.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the cache controller
    @param[in]      bistStep              Writing 1 will command the BIST FSM to move to the next BIST step in the sequence.
                                          bistStep and bistAllSeq are mutually exclusive, SW driver internally disable bistAllSeq when bistStep is 1.
    @param[in]      bistAllSeq            Writing a 1 will command the BIST FSM to sequence all 5 steps of the BIST sequence automatically.
                                          bistStep and bistAllSeq are mutually exclusive, SW driver internally disable bistAllSeq when bistStep is 1.

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid BIST all sequence

    @post None

    @trace  #BRCM_SWREQ_CACHE_FSC

    @limitations None
*/
int32_t CACHE_DrvFscControl(CACHE_HwIDType hwId, uint8_t bistStep, uint8_t bistAllSeq);

/** @brief FSC activity monitor

    This API allows user to monitor FSC BIST FSM.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the cache controller
    @param[out]     bistErr               Each bit in this register represent one step of the BIST sequence. When the sequence starts,
                                          it resets. As each BIST step completes, bit is set to 1 if the sequence has completed in error
    @param[out]     bistDone              Each bit in this register represent one step of the BIST sequence.
                                          When the sequence starts, it's reset. As each BIST step completes, its bit is set to 1

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for bistErr
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for bistDone

    @post None

    @trace  #BRCM_SWREQ_CACHE_FSC

    @limitations None
*/
int32_t CACHE_DrvFscStatus(CACHE_HwIDType hwId, uint8_t *bistErr, uint8_t *bistDone);

/** @brief Enable Interrupt Mask

    This API enable/Disable specified cache interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      intrType              Type of interrupt
    @param[in]      enControl             Enable(1)/Disable(0) specified interrupt

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @limitations None
*/
int32_t CACHE_DrvIntrConfig(CACHE_HwIDType hwId, CACHE_IntrType intrType, uint8_t  enControl);

/** @brief Clear Interrupt

    This API clear specified interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      intrType               Type of interrupt to clear

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT

    @limitations None
*/
int32_t CACHE_DrvIntrClear(CACHE_HwIDType hwId, CACHE_IntrType intrType);

/** @brief Get Interrupt Status

    This API get staus of cache interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      intrType              Interrupt type
    @param[out]     intrStatus            Interrupt Status

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT

    @limitations None
*/
int32_t CACHE_DrvIntrStatus(CACHE_HwIDType hwId, CACHE_IntrType intrType, uint32_t *intrStatus);

/** @brief Cache policy

    This API used to override cache read and write policy .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the cache
    @param[in]      rdPolicy         Override cache read policy
    @param[in]      wrPolicy         Override cache write policy

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          Cache instance not initialized

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE

    @limitations None
*/
int32_t CACHE_DrvOverrideCachePolicy(CACHE_HwIDType hwId, CACHE_PolicyType rdPolicy, CACHE_PolicyType wrPolicy);

#endif /* CACHE_H */

/** @} */
