/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential

 ******************************************************************************/
/**
    @defgroup grp_sdcdrv_ifc SDRAM Controller
    @ingroup grp_sdcdrv

    @addtogroup grp_sdcdrv_ifc
    @{
    @section sec_sdc_overview Overview
    -# SDC Bare Metal driver interface provides layer 1 APIs to configure
    SDC in desired configuration post bring up. It is configured in non-blocking mode and
    APIs provided are non-reenterant.
    -# Each SDC HW instance needs to be initialized separately using SDC_DrvInit API.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_sdcdrv_fsm Usage
    -# SDC HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   SDC_0 |

    @section api_list List of SDC and DDR APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref SDC_DrvInit | Initializes SDRAM Controller|
    | @ref SDC_DrvDeInit | De-initializes SDRAM |
    | @ref SDC_DrvPwrSavingConfig | Power Saving configuration |
    | @ref SDC_DrvSecureMemConfig| Configures secure memory|
    | @ref SDC_DrvSecureMemControl| Enables/Disables secure memory |
    | @ref SDC_DrvStatisticsControl  |  Enables/Disables Counter Collection |
    | @ref SDC_DrvStatisticsGet  |  Get SDRAM counters  |

    @file sdc.h
    @brief Interface for SDC and DDR driver
    @section apis This section defines the interface for DDR driver
    @version 0.1 Initial Version
*/

#ifndef SDC_H
#define SDC_H

#include <stdint.h>

/**
    @name SDC driver API IDs
    @{
    @brief API IDs for SDC drier
 */
#define BRCM_SWARCH_SDC_HW_ID_TYPE                         (0x8501U)    /**< @brief #SDC_HwIDType     */
#define BRCM_SWARCH_SDC_SDC_STATISTICS_COUNTER_TYPE        (0x8502U)    /**< @brief #SDC_StatisticsCounterType */
#define BRCM_SWARCH_SDC_DRV_INIT_PROC                      (0x8503U)    /**< @brief #SDC_DrvInit*/
#define BRCM_SWARCH_SDC_DRV_DEINIT_PROC                    (0x8504U)    /**< @brief #SDC_DrvDeInit*/
#define BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONFIG_PROC         (0x8505U)    /**< @brief #SDC_DrvSecureMemConfig*/
#define BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONTROL_PROC        (0x8506U)    /**< @brief #SDC_DrvSecureMemControl*/
#define BRCM_SWARCH_SDC_DRV_STATISTICS_CONTROL_PROC        (0x8507U)    /**< @brief #SDC_DrvStatisticsControl*/
#define BRCM_SWARCH_SDC_DRV_STATISTICS_GET_PROC            (0x8508U)    /**< @brief #SDC_DrvStatisticsGet*/
#define BRCM_SWARCH_SDC_DRV_PWR_SAVING_CONFIG_PROC         (0x8509U)    /**< @brief #SDC_DrvPwrSavingConfig*/
#define BRCM_SWARCH_SDC_SDC_POWER_SAVING_CONFIG_TYPE       (0x850AU)    /**< @brief #SDC_PowerSavingConfigurationType */
#define BRCM_SWARCH_SDC_STATE_TYPE                         (0x850BU)    /**< @brief #SDC_StateType     */
#define BRCM_SWARCH_SDC_PWR_SAVING_OPMODE_TYPE             (0x850CU)    /**< @brief #SDC_PwrSavingOpModeType*/

/** @} */

/**
    @brief Index of the SDC DDR controller

    @trace #BRCM_SWREQ_SDC_INIT
 */
typedef uint32_t SDC_HwIDType;

/**
    @name SDC_StateType
    @{
    @brief State of SDC

    @trace #BRCM_SWREQ_SDC_INIT
*/

typedef uint32_t SDC_StateType;                  /**< @brief SDC state type */
#define SDC_STATE_UNINIT           (0UL)         /**< @brief SDC is uninitialized & value must be zero*/
#define SDC_STATE_INITIALIZED      (1UL)         /**< @brief SDC is initialized */
/** @} */

/**
    @name SDC_PwrSavingOpModeType
    @{
    @brief State of SDC

    @trace #BRCM_SWREQ_SDC_PWR_SAVING
*/

typedef uint8_t SDC_PwrSavingOpModeType;                       /**< @brief SDC Power Saving operation mode */
#define SDC_PWR_SAVING_OPMODE_SELF_REFRESH        (0U)         /**< @brief SDC Self refresh mode*/
#define SDC_PWR_SAVING_OPMODE_PWR_SAVE            (1U)         /**< @brief SDC power down mode, Power Save feature keeps SDRAM device(s) 
                                                                           into power-down (CKE low) when they have been
                                                                           inactive for the Power Save time-out period.*/
#define SDC_PWR_SAVING_OPMODE_NONE                (2U)         /**< @brief No power saving. Put SDC to default power mode.*/
/** @} */

/**
    @brief SDC Power Saving

    @trace #BRCM_SWREQ_SDC_PWR_SAVING
*/
typedef struct sSDC_PowerSavingConfigurationType {
    SDC_PwrSavingOpModeType opMode;
    uint8_t restartPowerSaving;                                 /**<@brief 1: restart from opMode, 0 : No restart from opMode*/
    uint16_t holdOffPeriod;                                     /**<@brief When OpMode is SDC_PWR_SAVING_OPMODE_SELF_REFRESH
                                                                             12 bit value sets a time for which the SDRAM controller must
                                                                             idle before the memory is put into standby if the STBY bit is set
                                                                             in the SDCS register.  If STBY_T is set to zero, the SDRAM is put into
                                                                             standby as soon as the controller becomes idle when STBY is set.
                                                                             The delay value is set in units of the DFI clock period, which is four
                                                                             times the period of the DRAM data clock.
                                                                           When OpMode is SDC_PWR_SAVING_OPMODE_PWR_DN
                                                                             10-bit sets a time delay from the last memory access at which the SDRAM will
                                                                             be put into power-down, by de-asserting the CKE pin.  A value of zero will
                                                                             de-assert CKE one cycle after completing a burst.  Power-down is automatically
                                                                             exited when necessary to perform a refresh cycle or to close a row with the PGEHLD timer.
                                                                             The delay value is set in units of DFI clock periods*/

}SDC_PowerSavingConfigurationType;


/**
    @brief SDC Statistics

    @trace #BRCM_SWREQ_SDC_DIAG
*/
typedef struct sSDC_StatisticsCounterType {
    uint32_t idleCycleCount;                     /**<@brief  Counts the number of DFI clock cycles during which the
                                                             SDRAM controller is idle. */
    uint32_t readTrafficCount;                   /**<@brief  Counts the number of AXI read data cycles returned from
                                                             the SDRAM controller.*/
    uint32_t writeTrafficCount;                  /**<@brief  Counts the number of AXI write data cycles returned from
                                                             the SDRAM controller.*/
    uint32_t readDataCount;                      /**<@brief  Counts the total number of bytes requested in AXI transactions. */
    uint32_t writeDataCount;                     /**<@brief  Counts the total number of bytes to be written by AXI transactions. */
    uint32_t rowActivationCount;                 /**<@brief  Counts the number of ACTIVATE commands issued to the SDRAM.*/
    uint32_t cycleCount;                         /**<@brief  Counts the number of clock cycles for which the SDRAM controller has been running*/
    uint32_t activeCommandCycleCount;            /**<@brief  Counts the number of clock cycles in which an active command is issued by the
                                                             SDRAM controller to the memory.*/
    uint32_t databusActiveCycleCount;            /**<@brief  Counts the number of clock cycles in which the data bus between the memory and
                                                             the SDRAM controller is in use.*/
    uint32_t writeCommandCount;                  /**<@brief  Counts the number of clock cycles in which a particular command is issued by
                                                             the SDRAM controller to the memory from the access processor.*/
    uint32_t suboptimalWriteCommandCount;        /**<@brief  Number of Write commands issued by the SDRAM access processor where the command writes
                                                             fewer than four 16Byte chunks of data.*/
    uint32_t maskedWriteCommandCount;            /**<@brief  Number of Masked Write commands issued by the SDRAM access processor*/
    uint32_t suboptimalMaskedWriteCommandCount;  /**<@brief  Number of Write commands issued by the SDRAM access processor where the command writes
                                                             fewer than four 16B chunks of data.*/
    uint32_t readCommandCount;                   /**<@brief  Number of Read commands issued by the SDRAM access processor.*/
    uint32_t suboptimalReadCommandCount;         /**<@brief  Number of Read commands issued by the SDRAM access processor where the command returns
                                                             fewer than four 16B chunks of data.*/
    uint32_t maxQueueOccupancyCount;             /**<@brief  SDRAM Queue Maximum Occupancy.*/
    uint32_t requestReorderCount;                /**<@brief  counts the number of times the SDRAM controller chooses a request from the queue other
                                                             than that at the head of the queue, i.e. it re-orders incoming requests*/
    uint32_t lookAheadCommandCount;              /**<@brief  register counts the number of DFI clock cycles in which a look ahead command is issued by
                                                             the SDRAM controller to the memory*/
    uint32_t prechargeCommandCount;              /**<@brief  Number of Precharge commands issued by the SDRAM access processor.*/
    uint32_t requestWriteReorderCount;             /**<@brief  Number of times the SDRAM controller chooses a request from the queue other than that at the head
                                                             of the queue, i.e. it re-orders incoming requests*/
    uint32_t accessProcessorActivationCount;     /**<@brief  Counts the number of row activations issued by the SDRAM controller.*/
    uint32_t refreshInitManagementCycleCount;    /**<@brief  Counts the number of DFI clock cycles in which at least one SDRAM bank had reached the
                                                             refresh Init management threshold.*/
    uint32_t refreshMaxManagementCycleCount;     /**<@brief  Counts the number of DFI clock cycles in which at least one SDRAM bank had reached the
                                                               refresh Max management threshold.*/
    uint32_t perBankRefreshCmdCount;             /**<@brief  Counts the number LPDDR5 refresh management commands sent Per Bank in the SDRAM*/
    uint32_t allBankRefreshCmdCount;             /**<@brief  Counts the number LPDDR5 refresh management commands in the SDRAM*/
} SDC_StatisticsCounterType;


/** @brief Initialize SDC

    This API initializes the specified SDRAM controller

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId        Index of the SDRAM controller

    @return     #BCM_ERR_OK               Initialized SDRAM instance
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_SDC_INIT

    @limitations None
*/

int32_t SDC_DrvInit(SDC_HwIDType  hwId);

/** @brief De-Initialize SDC

    This API de-initializes the SDRAM controller

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the SDRAM controller

    @return     #BCM_ERR_OK               De-Initialized SDRAM instance
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_SDC_INIT

    @limitations None
*/

int32_t SDC_DrvDeInit(SDC_HwIDType  hwId);

/** @brief Power saving configuration

    This API configures power saving mode to reduce power consumption when the
    memory is not being accessed.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId        Index of the SDRAM controller
    @param[in]     pwrConfig   Power saving configurations.

    @return     #BCM_ERR_OK               Initialized SDRAM instance
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_SDC_PWR_SAVING

    @limitations None
*/

int32_t SDC_DrvPwrSavingConfig(SDC_HwIDType  hwId, SDC_PowerSavingConfigurationType *powerConfig);

/** @brief Configure secure memory area.

    This API configures secure memory area based on the memory index. SDRAM Controller has 32 secure memory areas.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the SDRAM Controller.
    @param[in]      sMemIndex        Secure Memory Index (0-31)
    @param[in]      protMask         It is a 8-bit bit mask, Each bit in the field specifies whether or
                                     not an incoming access request to the SDRAM controller with a particular value of
                                     AXI APROT is allowed to access this protected region.
                                     If protMask is set to 0x1, then requests with APROT=0 are allowed.
    @param[in]      startAddr        Secure memory Area start address bits 35-13, The ADDR field specifies the start (byte)
                                     address of the protected secure memory region.  Protected regions must start and end on 8kByte
                                     address boundaries. The Address specified should be a SDC local address.
                                     Ex: Memory protection Start address (0x080000000) should be given as (0x000000000).
    @param[in]      endAddr          Secure memory Area start address bits 35-13, The ADDR field specifies the start (byte)
                                     address of the protected secure memory region.  Protected regions must start and end on 8kByte
                                     address boundaries. The Address specified should be a SDC local address.
                                     Ex: Memory protection end address (0x090000000) should be given as (0x010000000)



    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_SDC_SECURE_AREA

    @limitations None
*/
int32_t SDC_DrvSecureMemConfig(SDC_HwIDType hwId, uint8_t sMemIndex, uint8_t protMask, uint32_t startAddress, uint32_t endAddress);

/** @brief Enables/Disables secure memory

    This API enables/disables secure memory based on the specified index.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the SDC
    @param[in]      sMemIndex        Secure Memory Index (0-31)
    @param[in]      enDis            Enables(1) or Disables(0) secure memory region.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_SDC_SECURE_AREA

    @limitations None
*/
int32_t SDC_DrvSecureMemControl(SDC_HwIDType hwId, uint8_t sMemIndex, uint8_t enDis);

/** @brief Enable SDRAM Statistics

    This API enables/disables counter collection on SDRAM Controller.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the SDRAM controller
    @param[in]      enDis                 Disable(0)/Enable(1) counter collection

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_SDC_DIAG

    @limitations None
*/
int32_t SDC_DrvStatisticsControl(SDC_HwIDType hwId, uint8_t enDis);

/** @brief Retrieve SDRAM Statistics

    This API retrieves various counters of SDRAM Controller.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the SDRAM controller
    @param[out]     counter              Counter of SDRAM Controller

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter

    @post None

    @trace  #BRCM_SWREQ_SDC_DIAG

    @limitations None
*/
int32_t SDC_DrvStatisticsGet(SDC_HwIDType hwId, SDC_StatisticsCounterType *counter);

#endif /* SDC_H */

/** @} */
