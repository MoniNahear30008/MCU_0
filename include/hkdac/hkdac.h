/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_hkdacdrv_ifc HKDAC
    @ingroup grp_hkdacdrv

    @addtogroup grp_hkdacdrv_ifc
    @{
    @section sec_hkdac_overview Overview
    @note
    -# House Keeping DAC Bare Metal driver interface provides layer 1 APIs to initialize and configure
    the HKDAC in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each HKDAC HW instance needs to be initialized separately using HKDAC_DrvInit API.
    -# User can use HKDAC_DrvConfigChIntr, HKDAC_DrvClearChIntr and HKDAC_DrvGetChIntrStatus API to configure DAC interrupts.
    -# User can use HKDAC_DrvConfig to enable mux selection for the AWG input.

    @note Please refer to respective technical reference manual for more details about the HW features.

    -# HKDAC HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   HKDAC       |

    @section api_list List of HKDAC APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref HKDAC_DrvInit            | Initialize HKDAC               |
    | @ref HKDAC_DrvDeInit          | De-initialize HKDAC            |
    | @ref HKDAC_DrvConfig          | HKDAC configuration            |
    | @ref HKDAC_DrvDacControl      | HKDAC channel control          |
    | @ref HKDAC_DrvConfigChIntr    | HKDAC Enable Channel interrupt |
    | @ref HKDAC_DrvClearChIntr     | HKDAC clear Channel interrupt  |
    | @ref HKDAC_DrvGetChIntrStatus | HKDAC Get Channel interrupt Status |
    | @ref HKDAC_DrvPowerConfig     | HKDAC power configuration      |
    | @ref HKDAC_DrvGetData         | HKDAC DAC data                 |
    | @ref HKDAC_DrvConfigData      | Write DAC data          |
    
    @file hkdac.h
    @brief Interface for HKDAC driver
    @section apis This section defines the interface for HKDAC driver
    @version 0.1 Initial Version
*/

#ifndef HKDAC_H
#define HKDAC_H

#include <stdint.h>

/**
    @name HKDAC driver API IDs
    @{
    @brief API IDs for HKDAC drier
 */
#define BRCM_SWARCH_HKDAC_HW_ID_TYPE                (0x8801U)    /**< @brief #HKDAC_HwIDType           */
#define BRCM_SWARCH_HKDAC_CHANNEL_ID_TYPE           (0x8802U)    /**< @brief #HKDAC_ChannelIDType      */
#define BRCM_SWARCH_HKDAC_SYNCGROUP_TYPE            (0x8803U)    /**< @brief #HKDAC_SyncGroupType      */
#define BRCM_SWARCH_HKDAC_SYNCMODE_TYPE             (0x8804U)    /**< @brief #HKDAC_SyncModeType       */
#define BRCM_SWARCH_HKDAC_CHANNELCONFIG_TYPE        (0x8807U)    /**< @brief #HKDAC_ChannelConfigType  */
#define BRCM_SWARCH_HKDAC_STATE_TYPE                (0x8808U)    /**< @brief #HKDAC_StateType          */
#define BRCM_SWARCH_HKDAC_CHANNEL_DATA_TYPE         (0x8809U)    /**< @brief #HKDAC_ChannelDataType    */

#define BRCM_SWARCH_HKDAC_DRV_INIT_PROC             (0x880BU)    /**< @brief #HKDAC_DrvInit            */
#define BRCM_SWARCH_HKDAC_DRV_DEINIT_PROC           (0x880CU)    /**< @brief #HKDAC_DrvDeInit          */
#define BRCM_SWARCH_HKDAC_DRV_CFG_PROC              (0x880DU)    /**< @brief #HKDAC_DrvConfig          */
#define BRCM_SWARCH_HKDAC_DRV_DAC_CONTROL_PROC      (0x880EU)    /**< @brief #HKDAC_DrvDacControl      */
#define BRCM_SWARCH_HKDAC_DRV_POWER_CONFIG_PROC     (0x880FU)    /**< @brief #HKDAC_DrvPowerConfig     */
#define BRCM_SWARCH_HKDAC_DRV_GET_DATA_PROC         (0x8811U)    /**< @brief #HKDAC_DrvGetData         */
#define BRCM_SWARCH_HKDAC_CHANNEL_INTR_TYPE         (0x8812U)    /**< @brief #HKDAC_ChannelIntrType    */
#define BRCM_SWARCH_HKDAC_DRV_CONFIG_CH_INTR_PROC   (0x8813U)    /**< @brief #HKDAC_DrvConfigChIntr    */
#define BRCM_SWARCH_HKDAC_DRV_CLEAR_CH_INTR_PROC    (0x8814U)    /**< @brief #HKDAC_DrvClearChIntr     */
#define BRCM_SWARCH_HKDAC_DRV_GET_CH_INTR_STS_PROC  (0x8815U)    /**< @brief #HKDAC_DrvGetChIntrStatus */
#define BRCM_SWARCH_HKDAC_DRV_CONFIG_DATA_PROC      (0x8816U)    /**< @brief #HKDAC_DrvConfigData      */

/** @} */

/**
    @brief Index of the HKDAC controller

    @trace #BRCM_SWREQ_HKDAC_INIT
 */
typedef uint32_t HKDAC_HwIDType;
/**
    @name HKDAC_StateType
    @{
    @brief State of HKDAC

    @trace #BRCM_SWREQ_HKDAC_INIT
*/

typedef uint32_t HKDAC_StateType;          /**< @brief HKDAC state type */
#define HKDAC_STATE_UNINIT           (0UL) /**< @brief HKDAC is uninitialized & value must be zero*/
#define HKDAC_STATE_INITIALIZED      (1UL) /**< @brief HKDAC is initialized */
/** @} */

/**
    @brief Index of the HKDAC channel

    @trace #BRCM_SWREQ_HKDAC_INIT
 */
typedef uint32_t HKDAC_ChannelIDType;


/**
    @name HKDAC_SyncGroupType
    @{
    @brief House Keeping DAC synchronous group
    To sync {DAC 6-7} -> GRP2, {DAC 4-5} -> GRP1, {DAC 0-3} -> GRP0
        DAC0_SYNC_GRP 8'b0000_1111
        DAC1_SYNC_GRP 8'b0000_1111
        DAC2_SYNC_GRP 8'b0000_1111
        DAC3_SYNC_GRP 8'b0000_1111
        DAC4_SYNC_GRP 8'b0011_0000
        DAC5_SYNC_GRP 8'b0011_0000
        DAC6_SYNC_GRP 8'b1100_0000
        DAC7_SYNC_GRP 8'b1100_0000

        will be waiting for ALL grouped members
        sync enter: non-empty and the EN_DAC is asserted
        sync exit : empty and the EN_DAC is de-asserted

    @trace #BRCM_SWREQ_HKDAC_CONFIGURATION
 */
typedef uint16_t HKDAC_SyncGroupType;                               /**< @brief HKDAC Synchronous FIFO readout for DAC group  */
#define HKDAC_SYNCGROUP_DAC0                             (1UL)      /**< @brief Sync for DAC 0  */
#define HKDAC_SYNCGROUP_DAC1                             (2UL)      /**< @brief Sync for DAC 1  */
#define HKDAC_SYNCGROUP_DAC2                             (4UL)      /**< @brief Sync for DAC 2  */
#define HKDAC_SYNCGROUP_DAC3                             (8UL)      /**< @brief Sync for DAC 3  */
#define HKDAC_SYNCGROUP_DAC4                          (0x10UL)      /**< @brief Sync for DAC 4  */
#define HKDAC_SYNCGROUP_DAC5                          (0x20UL)      /**< @brief Sync for DAC 5  */
#define HKDAC_SYNCGROUP_DAC6                          (0x40UL)      /**< @brief Sync for DAC 6  */
#define HKDAC_SYNCGROUP_DAC7                          (0x80UL)      /**< @brief Sync for DAC 7  */
/** @} */

/**
    @name HKDAC_SyncModeType
    @{
    @brief House Keeping DAC Sync Mode

    @trace #BRCM_SWREQ_HKDAC_CONFIGURATION
 */
typedef uint8_t HKDAC_SyncModeType;                        /**< @brief HKDAC sync mode Type                    */
#define HKDAC_SYNCMODE_FIFO_RD_ENDAC               (0UL)   /**< @brief FIFO read when non-empty and dacControl */
#define HKDAC_SYNCMODE_FIFO_RD_ENDAC_EXT_TRG       (1UL)   /**< @brief FIFO read when non-empty, dacControl and external trigger comes */
#define HKDAC_SYNCMODE_FIFO_RD_GRP_MEM             (2UL)   /**< @brief Sync FIFO read for group members        */
#define HKDAC_SYNCMODE_FIFO_RD_GRP_MEM_EXT_TRG     (3UL)   /**< @brief Sync FIFO read for group members and external trigger comes     */
/** @} */

/**
    @brief House keeping DAC Config

    @trace #BRCM_SWREQ_HKDAC_CONFIGURATION
 */

typedef struct sHKDAC_ChannelConfigType {
    HKDAC_SyncGroupType   syncGroup;     /**< @brief Synchronous FIFO readout for DAC group */
    HKDAC_SyncModeType     syncMode;     /**< @brief Sync mode for enabling DAC*/
    uint16_t       programRateCount;     /**< @brief control the DAC program rate via this register,
                                                     set the controller busy flag HIGH for N cycles prog rate = REF_CLK/(1+N)*/
    uint16_t                  enAwg;     /**< @brief Enable mux selection for the AWG input and strobe
                                                     Only applicable for DAC 4 - 7 */
    uint8_t            lowPowerMode;     /**< @brief Enables(1)/Disables(0) low power mode*/
} HKDAC_ChannelConfigType;

/**
    @brief House keeping DAC data 

    @trace #BRCM_SWREQ_HKDAC_CONFIGURATION
 */

typedef struct sHKDAC_ChannelDataType {
    uint16_t      loData;     /**< @brief DAC lower data (sample0)                        */
    uint16_t      hiData;     /**< @brief DAC upper data applicable only in 2-sample mode */
    uint16_t      dataSize;   /**< @brief DAC data size Max 16*/
    uint8_t       chId;       /**< @brief Channed Id */  
} HKDAC_ChannelDataType;

/** @brief Initialize HKDAC

    This API initializes the given HKDAC hardware.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKDAC controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKDAC_INIT

    @limitations None
*/
int32_t HKDAC_DrvInit(HKDAC_HwIDType  hwId);

/** @brief De-initialize HKDAC

    This API de-initializes HKDAC. Resets the given HKDAC controller and puts back
    to its original state (same as Power on reset).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKDAC controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HKDAC driver state changes to #HKDAC_STATE_UNINIT

    @trace  #BRCM_SWREQ_HKDAC_INIT

    @limitations None
*/
int32_t HKDAC_DrvDeInit (HKDAC_HwIDType  hwId);

/** @brief HKDAC configuration

    This API configures HKDAC with user specified configuration but does not enable channels.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKDAC controller
    @param[in]      chId         Index (0-7) of the HKDAC channel to be configured
    @param[in]      hkdacConfig  House keeping DAC configuration

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NODEV          HW not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hkdacConfig
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid SyncGroup
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid SyncMode

    @post None

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION

    @limitations None
*/
int32_t HKDAC_DrvConfig (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, HKDAC_ChannelConfigType *hkdacConfig);

/** @brief HKDAC AFE power configuration

    This API configures HKDAC AFE power. HW defaults to power down. It is user's responsibility to
    power up DAC AFE by calling this API.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKDAC controller
    @param[in]      chId         Index (0-7) of the HKDAC channel to be configured
    @param[in]      pwrConfig    0 - Power Down 1 - Power Up

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NODEV          HW not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel index

    @post None

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION

    @limitations None
*/
int32_t HKDAC_DrvPowerConfig (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, uint8_t pwrConfig);

/** @brief HKDAC controller configuration

    This API enables or disables HKDAC controller.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKDAC controller
    @param[in]      chId         Index (0-7) of the HKDAC channel to be configured
    @param[in]      dacControl   House keeping DAC controller enable(1)/Disable(0)

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NODEV          HW not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel index

    @post None

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION

    @limitations None
*/
int32_t HKDAC_DrvDacControl (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, uint8_t dacControl);

/** @brief HKDAC get DAC data

    This API gets DAC data.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKDAC controller
    @param[in]      chId         Index (0-7) of the HKDAC channel to be configured
    @param[out]     loData       DAC lower data sample0
    @param[out]     hiData       DAC upper data applicable only in 2-sample mode

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NODEV          HW not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid memory

    @post None

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION

    @limitations None
*/

int32_t HKDAC_DrvGetData (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, uint32_t *loData,  uint32_t *hiData);

/** @brief Configure DAC data

    This API configures DAC lower data (sample 0) and upper data (applicable for 2-sample mode).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKDAC controller
    @param[in]      chData       HKDAC channel data

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NODEV          HW not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid memory
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid data size

    @post None

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION

    @limitations None
*/

int32_t HKDAC_DrvConfigData (HKDAC_HwIDType  hwId, HKDAC_ChannelDataType *chData);

/**
    @name HKDAC_ChannelIntrType
    @{
    @brief HouseKeeping DAC channel Interrupts/Events

    @trace #BRCM_SWREQ_HKDAC_INTERRUPT
 */
typedef uint32_t HKDAC_ChannelIntrType;                   /**< @brief HKDAC channel based interrupt type */

#define HKDAC_CHANNEL_INTR_FIFO_EMPTY      (0x1UL)        /**< @brief HKDAC empty alarm */
#define HKDAC_CHANNEL_INTR_FIFO_OVERFLOW   (0x2UL)        /**< @brief HKDAC overflow alarm */
#define HKDAC_CHANNEL_INTR_ALL             (0x3UL)        /**< @brief All the above interrupts*/
#define HKDAC_CHANNEL_INTR_EVENT_ALL       (0x100UL)      /**< @brief All the above interrupts*/
/** @} */

/** @brief HKDAC Enable Channel interrupt

    This API enables/disables HKDAC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKDAC controller
    @param[in]      chId                 Index of the HKDAC Channel
    @param[in]      intrType             HKDAC interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Interrupt Type
    @retval     #BCM_ERR_NODEV          HW not initialized

    @post   None

    @trace  #BRCM_SWREQ_HKDAC_INTERRUPT

    @limitations None
*/
int32_t HKDAC_DrvConfigChIntr(HKDAC_HwIDType  hwId,  HKDAC_ChannelIDType chId, HKDAC_ChannelIntrType intrType, uint32_t enaDis);

/** @brief HKDAC clear Channel interrupt

    This API clears HKDAC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKDAC controller
    @param[in]      chId                 Index of the HKDAC Channel
    @param[in]      intrType        HKDAC interrupt type

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Channel
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Interrupt Type
    @retval     #BCM_ERR_NODEV          HW not initialized

    @post   None

    @trace  #BRCM_SWREQ_HKDAC_INTERRUPT

    @limitations None
*/
int32_t HKDAC_DrvClearChIntr(HKDAC_HwIDType  hwId,  HKDAC_ChannelIDType chId, HKDAC_ChannelIntrType intrType);

/** @brief HKDAC Get Channel interrupt Status

    This API gets HKDAC channel based interrupt status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKDAC controller
    @param[in]      chId                 Index of the HKDAC Channel
    @param[in]      intrType             HKDAC interrupt type
    @param[out]     intrStatus           HKDAC interrupt Status

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   None

    @trace  #BRCM_SWREQ_HKDAC_INTERRUPT

    @limitations None
*/
int32_t HKDAC_DrvGetChIntrStatus(HKDAC_HwIDType  hwId,  HKDAC_ChannelIDType chId, HKDAC_ChannelIntrType intrType, uint32_t *intrStatus);

#endif /* HKDAC_H */

/** @} */
