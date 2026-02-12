/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_hkadcdrv_ifc HKADC
    @ingroup grp_hkadcdrv

    @addtogroup grp_hkadcdrv_ifc
    @{
    @section sec_hkadc_overview Overview
    @note
    -# House Keeping ADC Bare Metal driver interface provides layer 1 APIs to initialize, calibrate and configure
    the HKADC in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each HKADC HW instance needs to be initialized separately using HKADC_DrvInit API.
    -# It is user's responsibility to configure DMA separately to handle data transfers.
    -# User can configure HKADC comparators using HKADC_DrvComparatorConfig API and FIFO threshold using HKADC_DrvConfig.
    -# ADC6 and ADC7 also have a special function of supporting external multiplexer. In this case, each ADC has
       a sequencer which runs through a sequence of external mux selections, done by sending SPI command and then
      capturing the ADC data
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_hkadcdrv_fsm Usage
    -# HKADC HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   HKADC |

    @section api_list List of HKADC APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref HKADC_DrvInit | Initialize HKADC |
    | @ref HKADC_DrvDeInit | De-initialize HKADC |
    | @ref HKADC_DrvCaptureConfig | HKADC capture configuration |
    | @ref HKADC_DrvCaptureControl | HKADC capture control |
    | @ref HKADC_DrvFifoConfig | HKADC FIFO configuration |
    | @ref HKADC_DrvGetFifoData| Retrive HKADC FIFO data|
    | @ref HKADC_DrvComparatorConfig | HKADC comparator configuration |
    | @ref HKADC_DrvComparatorControl| HKADC comparator control|
    | @ref HKADC_DrvConfigCalibration | HKADC Calibration configuration |
    | @ref HKADC_DrvGetCalibStatus | HKADC Calibration Status |
    | @ref HKADC_DrvCalibrationControl | HKADC calibration control|
    | @ref HKADC_DrvConfigChIntr | HKADC Enable Channel interrupt |
    | @ref HKADC_DrvClearChIntr | HKADC clear Channel interrupt |
    | @ref HKADC_DrvGetChIntrStatus | HKADC Get Channel interrupt Status |
    | @ref HKADC_DrvSeqSpiCfg  | HKADC sequencer SPI configuration |
    | @ref HKADC_DrvSeqControl | HKADC sequencer control |
    | @ref HKADC_DrvSeqMemRdWr | HKADC sequencer memory read/write |
    | @ref HKADC_DrvSwSpiRdWr  | HKADC sequencer software SPI read/write |

    @file hkadc.h
    @brief Interface for HKADC driver
    @section apis This section defines the interface for HKADC driver
    @version 0.1 Initial Version
*/

#ifndef HKADC_H
#define HKADC_H

#include <stdint.h>

/**
    @name HKADC driver API IDs
    @{
    @brief API IDs for HKADC drier
 */
#define BRCM_SWARCH_HKADC_HW_ID_TYPE                (0x8601U)    /**< @brief #HKADC_HwIDType           */
#define BRCM_SWARCH_HKADC_CHANNEL_ID_TYPE           (0x8602U)    /**< @brief #HKADC_ChannelIDType      */
#define BRCM_SWARCH_HKADC_CAPTURE_MODE_TYPE         (0x8603U)    /**< @brief #HKADC_CaptureModeType    */
#define BRCM_SWARCH_HKADC_TRIGGER_TYPE              (0x8604U)    /**< @brief #HKADC_TriggerType        */
#define BRCM_SWARCH_HKADC_COMPARATOR_THRESHOLD_TYPE (0x8605U)    /**< @brief #HKADC_ComparatorThresholdType */
#define BRCM_SWARCH_HKADC_CAPTURE_CFG_TYPE          (0x8606U)    /**< @brief #HKADC_CaptureConfigType   */
#define BRCM_SWARCH_HKADC_STATE_TYPE                (0x8607U)    /**< @brief #HKADC_StateType   */
#define BRCM_SWARCH_HKADC_COMPARATOR_CGF_TYPE       (0x8608U)    /**< @brief #HKADC_ComparatorConfigType */
#define BRCM_SWARCH_HKADC_COMPARATOR_TYPE           (0x8609U)    /**< @brief #HKADC_ComparatorType       */
#define BRCM_SWARCH_HKADC_CALIB_CONFIG_TYPE         (0x860AU)    /**< @brief #HKADC_CalibConfigType   */
#define BRCM_SWARCH_HKADC_CALIB_STATUS_TYPE         (0x860BU)    /**< @brief #HKADC_CalibStatusType     */

#define BRCM_SWARCH_HKADC_DRV_INIT_PROC             (0x860CU)    /**< @brief #HKADC_DrvInit         */
#define BRCM_SWARCH_HKADC_DRV_DEINIT_PROC           (0x860DU)    /**< @brief #HKADC_DrvDeInit  */
#define BRCM_SWARCH_HKADC_DRV_CAPTURE_CONFIG_PROC   (0x860EU)    /**< @brief #HKADC_DrvCaptureConfig   */
#define BRCM_SWARCH_HKADC_DRV_FIFO_CONFIG_PROC      (0x860FU)    /**< @brief #HKADC_DrvFifoConfig   */
#define BRCM_SWARCH_HKADC_DRV_COMPARATOR_CFG_PROC   (0x8610U)    /**< @brief #HKADC_DrvComparatorConfig   */
#define BRCM_SWARCH_HKADC_DRV_CFG_CAL_PROC          (0x8611U)    /**< @brief #HKADC_DrvConfigCalibration  */
#define BRCM_SWARCH_HKADC_GET_CAL_STS_PROC          (0x8612U)    /**< @brief #HKADC_DrvGetCalibStatus  */

#define BRCM_SWARCH_HKADC_CHANNEL_INTR_TYPE         (0x8613U)    /**< @brief #HKADC_ChannelIntrType   */
#define BRCM_SWARCH_HKADC_DRV_CONFIG_CH_INTR_PROC   (0x8614U)    /**< @brief #HKADC_DrvConfigChIntr   */
#define BRCM_SWARCH_HKADC_DRV_CLEAR_CH_INTR_PROC    (0x8615U)    /**< @brief #HKADC_DrvClearChIntr     */
#define BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC  (0x8616U)    /**< @brief #HKADC_DrvGetChIntrStatus */

#define BRCM_SWARCH_HKADC_DRV_CAPTURE_CONTROL_PROC    (0x861BU)    /**< @brief #HKADC_DrvCaptureControl   */
#define BRCM_SWARCH_HKADC_DRV_GET_FIFO_DATA_PROC      (0x861CU)    /**< @brief #HKADC_DrvGetFifoData*/
#define BRCM_SWARCH_HKADC_DRV_COMPARATOR_CONTROL_PROC (0x861DU)    /**< @brief #HKADC_DrvComparatorControl*/
#define BRCM_SWARCH_HKADC_DRV_CAL_CONTROL_PROC        (0x861EU)    /**< @brief #HKADC_DrvCalibrationControl*/

#define BRCM_SWARCH_HKADC_SPICONTROL_TYPE             (0x861FU)   /**< @brief #HKADC_SpiControlType*/
#define BRCM_SWARCH_HKADC_SPICONFIG_TYPE              (0x8620U)   /**< @brief #HKADC_SpiConfigType*/
#define BRCM_SWARCH_HKADC_SWSPIRDWR_TYPE              (0x8621U)   /**< @brief #HKADC_SwSpiRdWrType*/
#define BRCM_SWARCH_HKADC_SEQMEMRDWR_TYPE             (0x8622U)   /**< @brief #HKADC_SeqMemRdWrType*/
#define BRCM_SWARCH_HKADC_DRV_SEQSPICFG_PROC          (0x8623U)   /**< @brief #HKADC_DrvSeqSpiCfg*/
#define BRCM_SWARCH_HKADC_DRV_SEQCONTROL_PROC         (0x8624U)   /**< @brief #HKADC_DrvSeqControl*/
#define BRCM_SWARCH_HKADC_DRV_SEQMEMRDWR_PROC         (0x8625U)   /**< @brief #HKADC_DrvSeqMemRdWr*/
#define BRCM_SWARCH_HKADC_DRV_SWSPIRDWR_PROC          (0x8626U)   /**< @brief #HKADC_DrvSwSpiRdWr*/


/** @} */

/**
    @brief Index of the HKADC controller

    @trace #BRCM_SWREQ_HKADC_INIT
 */
typedef uint32_t HKADC_HwIDType;

/**
    @brief Index of the HKADC channel

    @trace #BRCM_SWREQ_HKADC_INIT
 */
typedef uint16_t HKADC_ChannelIDType;

/**
    @name HKADC_StateType
    @{
    @brief State of HKADC

    @trace #BRCM_SWREQ_HKADC_INIT
*/

typedef uint32_t HKADC_StateType;                /**< @brief HKADC state type */
#define HKADC_STATE_UNINIT           (0UL)       /**< @brief HKADC is uninitialized & value must be zero*/
#define HKADC_STATE_INITIALIZED      (1UL)       /**< @brief HKADC is initialized */
/** @} */


/**
    @name HKADC_CaptureModeType
    @{
    @brief House Keeping ADC capture mode

    @trace #BRCM_SWREQ_HKADC_CONFIGURATION
 */
typedef uint8_t HKADC_CaptureModeType;                          /**< @brief HKADC Capture mode Type   */
#define HKADC_CAPTURE_MODE_SINGLE                     (0U)      /**< @brief Single Trigger Conversion Mode  */
#define HKADC_CAPTURE_MODE_CONTINUOUS                 (1U)      /**< @brief Contineous Trigger Conversion Mode  */
/** @} */

/**
    @name HKADC_TriggerType
    @{
    @brief House Keeping ADC Trigger Type

    @trace #BRCM_SWREQ_HKADC_CONFIGURATION
 */
typedef uint8_t HKADC_TriggerType;          /**< @brief HKADC Trigger Type */
#define HKADC_TRIGGER_SOFTWARE       (1U)   /**< @brief Software Trigger */
#define HKADC_TRIGGER_HARDWARE       (0U)   /**< @brief Hardware Trigger */
/** @} */

/**
    @name HKADC_ThresholdModeType
    @{
    @brief House Keeping ADC comparator Threshold mode Type

    @trace #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION
 */
typedef uint8_t HKADC_ComparatorThresholdType;                    /**< @brief HKADC comparator threshold Type */
#define HKADC_COMPARATOR_THRESHOLD_ABOVE       (0U)               /**< @brief output HIGH when ADC value above threshold */
#define HKADC_COMPARATOR_THRESHOLD_BELOW       (1U)               /**< @brief output HIGH when ADC value below threshold */
/** @} */

/**
    @brief House keeping ADC capture control

    @trace #BRCM_SWREQ_HKADC_CONFIGURATION
 */

typedef struct sHKADC_CaptureConfigType {
    HKADC_CaptureModeType capMode;            /**< @brief Capture mode */
    HKADC_TriggerType triggerType;            /**< @brief Trigger type */
    uint8_t seqMode;                          /**< @brief 0: non-sequencer, generate the ADC start from HKADC controller
                                                          1: sequencer, generate the  ADC start from sequencerTrigger type */
    uint16_t programRate;                     /**< @brief The rate at which samples are captured */
} HKADC_CaptureConfigType;


/**
    @brief House keeping ADC Comparator Configuration

    @trace #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION
 */

typedef struct sHKADC_ComparatorConfigType {
    uint16_t                      thresholdValue;              /**< @brief ADC threshold value for interrupt. Min : 0 Max : 0xFFF
                                                                     Comparator will not be configurred if value is 0xFFFF */
    HKADC_ComparatorThresholdType thresholdMode;               /**< @brief HKADC threshold mode
                                                                     0 : output HIGH when ADC value above threshold
                                                                     1 : output HIGH when ADC value below threshold*/
} HKADC_ComparatorConfigType;

/**
    @brief House keeping ADC Comparator

    @trace #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION
 */

typedef struct sHKADC_ComparatorType {
    HKADC_ComparatorConfigType comparator1;                   /**< @brief Comparator1 Configuration*/
    HKADC_ComparatorConfigType comparator2;                   /**< @brief Comparator2 Configuration*/
} HKADC_ComparatorType;

/**
    @brief House keeping ADC Calibration Configuration

    @trace #BRCM_SWREQ_HKADC_CALIBRATION
 */

typedef struct sHKADC_CalibConfigType {
    uint8_t calibrationAverage;              /**< @brief SAR calibration averaging control
                                                  0 - 8x (default), 1 - 16x, 2 - 32x, 3 - 64x */
    uint8_t calibMode;                       /**< @brief SAR calibration mode control
                                                  Bit <2:0> are offset calibration mode.
                                                  Bit <3> is NCDAC offset residual enB.
                                                  Bit<4> is NCDAC bit weight cal enB.
                                                  Bit<5> is remote GND meas. enB.
                                                  Bit<6> is calibration average enB */
} HKADC_CalibConfigType;

/**
    @brief House keeping ADC calibration status

    @trace #BRCM_SWREQ_HKADC_CALIBRATION
 */

typedef struct sHKADC_CalibStatusType {
  uint16_t calStatus;       /**< @brief Calibration status 1 : Calibration in progress 0: Calibration Done */
} HKADC_CalibStatusType;


/** @brief Initialize HKADC

    This API initializes the given HKADC hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKADC controller

    @return     int32_t

    @post None

    @trace  #BRCM_SWREQ_HKADC_INIT

    @limitations None
*/

int32_t HKADC_DrvInit(HKADC_HwIDType  hwId);

/** @brief De-initialize HKADC

    This API de-initializes HKADC. Resets the given HKADC controller and put back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKADC controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_INIT

    @limitations None
*/

int32_t HKADC_DrvDeInit (HKADC_HwIDType  hwId);

/** @brief HKADC capture configuration

    This API configures HKADC with user specified configurations.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      capConfig    House keeping ADC configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION

    @limitations None
*/
int32_t HKADC_DrvCaptureConfig (HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, HKADC_CaptureConfigType *capConfig);

/** @brief HKADC capture enable

    This API enables/disables HKADC capture.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      enDis        Enables/Disables capture configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION

    @limitations None
*/
int32_t HKADC_DrvCaptureControl (HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint8_t enDis);

/** @brief HKADC FIFO configuration

    This API perform FIFO configuration user can also perform FIFO soft reset based on
    the specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      softReset    0 - Don't Care 1 - Perform Softreset on FIFO
    @param[in]      thresholdValue FIFO threshold value, valid values are 0-127

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION

    @limitations None
*/
int32_t HKADC_DrvFifoConfig(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint8_t softReset,  uint8_t thresholdValue);

/** @brief HKADC FIFO Read Data

    This API retreives FIFO data.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[out]     fifoData     ADC FIFO read data

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION

    @limitations None
*/
int32_t HKADC_DrvGetFifoData(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint16_t *fifoData);

/** @brief HKADC comparator configuration

    This API configures comparators of HKADC of a specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      compConfig     Comparator configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HKADC driver state changes to #HKADC_ENABLE

    @trace  #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION

    @limitations None
*/
int32_t HKADC_DrvComparatorConfig(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId , HKADC_ComparatorType *compConfig);

/** @brief Enables/Disables HKADC comparator

    This API enables/disables comparators of HKADC of a specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HKADC controller
    @param[in]      chId                Index (0-7) of the HKADC channel to be configured
    @param[in]      comInstance         0 : Comparator Instance 0 , 1 : Comparator Instance 1
    @param[in]      enDis               0 : Disable Comparator 1 : Enable Comparator

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION

    @limitations None
*/
int32_t HKADC_DrvComparatorControl(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId , uint8_t comInstance, uint8_t enDis);

/** @brief HKADC Calibration configuration

    This API configures and enables HKADC hardware background or foreground calibration.
    This API should be called after configuring HKADC using HKADC_DrvConfig.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      calibConfig    HKADC calibration configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CALIBRATION

    @limitations None
*/
int32_t HKADC_DrvConfigCalibration(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, HKADC_CalibConfigType *calibConfig);

/** @brief HKADC Calibration Enable

    This API enables HKADC hardware background or foreground calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HKADC controller
    @param[in]      chId           Index (0-7) of the HKADC channel to be configured
    @param[in]      enCalib        Enables(1)/Disables(0) HKADC calibration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CALIBRATION

    @limitations None
*/
int32_t HKADC_DrvCalibrationControl(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint8_t enCalib);

/** @brief HKADC Calibration Status

    This API gets HKADC calibration status. Calibration status in not valid if calibration is not enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      calibStatus    HKADC calibration status

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HKADC driver state changes to #HKADC_CALIB_INPROGRESS/HKADC_ENABLE

    @trace  #BRCM_SWREQ_HKADC_CALIBRATION

    @limitations None
*/
int32_t HKADC_DrvGetCalibStatus(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, HKADC_CalibStatusType *calibStatus);

/**
    @name HKADC_ChannelIntrType
    @{
    @brief HouseKeeping ADC channel Interrupts, rising Edge interrupts

    @trace #BRCM_SWREQ_HKADC_INTERRUPT
 */
typedef uint32_t HKADC_ChannelIntrType;                   /**< @brief HKADC channel based interrupt type */

#define HKADC_CHANNEL_INTR_FIFO_EMPTY      (0x1UL)        /**< @brief HKADC empty alarm */
#define HKADC_CHANNEL_INTR_FIFO_OVERFLOW   (0x2UL)        /**< @brief HKADC overflow alarm */
#define HKADC_CHANNEL_INTR_ADC_THRES_0     (0x4UL)        /**< @brief HKADC ADC_thres0 reached alarm*/
#define HKADC_CHANNEL_INTR_ADC_THRES_1     (0x8UL)        /**< @brief HKADC ADC_thres1 reached alarm*/
#define HKADC_CHANNEL_INTR_FIFO_THRES      (0x10UL)       /**< @brief HKADC FIFO_thres reached alarm*/
#define HKADC_CHANNEL_INTR_SEQUENCER       (0x20UL)       /**< @brief HKADC sequencer Interrupt for CH6 and 7 */
#define HKADC_CHANNEL_INTR_ALL             (0x4FUL)       /**< @brief HKADC all interrupts */
#define HKADC_CHANNEL_INTR_EVENT_ALL       (0x100UL)      /**< @brief HKADC all event*/

/** @} */

/** @brief HKADC Enable Channel interrupt

    This API enables/disables HKADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId                 Index of the HKADC Channel
    @param[in]      intrType             HKADC interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT

    @limitations None
*/
int32_t HKADC_DrvConfigChIntr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_ChannelIntrType intrType, uint32_t enaDis);

/** @brief HKADC clear Channel interrupt

    This API clears HKADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId                 Index of the HKADC Channel
    @param[in]      HKADC_IntType        HKADC interrupt type

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT

    @limitations None
*/
int32_t HKADC_DrvClearChIntr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_ChannelIntrType intrType);

/** @brief HKADC Gets Channel interrupt/Event Status

    This API gets HKADC channel based interrupts/Event status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId                 Index of the HKADC Channel
    @param[in]      HKADC_IntType        HKADC interrupt type
    @param[out]     intrStatus           HKADC interrupt/Event Status

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT

    @limitations None
*/

int32_t HKADC_DrvGetChIntrStatus(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_ChannelIntrType intrType, uint32_t *intrStatus);

/**
    @name HKADC_SpiControlType
    @{
    @brief SPI Control for sequencer

    @trace #BRCM_SWREQ_HKADC_CONFIGURATION
*/
typedef uint32_t HKADC_SpiControlType;                         /**< @brief SPI control configuration,
                                                                           user has to use @ref HKADC_SPI_CONTROL_BUILD to
                                                                           build SPI control Type */
#define HKADC_SPICONTROL_SEQ_SPI_WAIT_TIME_SHIFT   (16UL)     /**< @brief SPI wait time(WT) shift */
#define HKADC_SPICONTROL_SPI_CLK_DIVIDER_SHIFT     (8UL)      /**< @brief SPI Clock Divider (CLDIV) shift */
#define HKADC_SPICONTROL_SPI_CMD_SIZE_SHIFT        (6UL)      /**< @brief SPI command size (CSZ) shift */
#define HKADC_SPICONTROL_SPI_CMD_CNT_SHIFT         (4UL)      /**< @brief SPI command count (CCNT) shift*/
#define HKADC_SPICONTROL_SPI_CPOL_SHIFT            (3UL)      /**< @brief SPI polarity(PL) shift*/
#define HKADC_SPICONTROL_SPI_CPHA_SHIFT            (2UL)      /**< @brief SPI phase (PH) shift*/
#define HKADC_SPICONTROL_SPI_SEQ_EN_SHIFT          (0UL)      /**< @brief SPI sequencer enable (SE) shift */

#define HKADC_SPICONTROL_BUILD(SE,PH,PL,CCNT,CSZ,CLDIV,WT)  ((WT << HKADC_SPICONTROL_SEQ_SPI_WAIT_TIME_SHIFT) | (CLDIV << HKADC_SPICONTROL_SPI_CLK_DIVIDER_SHIFT) | (CSZ << HKADC_SPICONTROL_SPI_CMD_SIZE_SHIFT) | (CCNT << HKADC_SPICONTROL_SPI_CMD_CNT_SHIFT) | (PL << HKADC_SPICONTROL_SPI_CPOL_SHIFT) | (PH << HKADC_SPICONTROL_SPI_CPHA_SHIFT) |(SE << HKADC_SPICONTROL_SPI_SEQ_EN_SHIFT))                           /**< @brief SPI control builder*/

/** @} */

/**
    @brief House keeping ADC SPI control

    @trace #BRCM_SWREQ_HKADC_CONFIGURATION
 */

typedef struct sHKADC_SpiConfigType {
    HKADC_SpiControlType spiControl;
    uint32_t spiSeqOut;                   /**< @brief 0: Sequencer data (bits 31:16 of sequencer memory) output
                                                         in serial form over SPI interface
                                                      1: Lower 3 bits of sequencer data output as parallel data. 
                                                      Bits 26:24 of sequencer memory sent out over {csn, mosi, sck} */
} HKADC_SpiConfigType;


/** @brief HKADC sequencer SPI config

    This API configures HKADC channel 6/channel 7 sequencer SPI.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the HKADC controller
    @param[in]      chId             HKADC channels. Valid channels are 6 and 7.
    @param[in]      spiCfg           Spi configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_SEQUENCER

    @limitations None
*/
int32_t HKADC_DrvSeqSpiCfg(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_SpiConfigType* spiCfg);


/** @brief HKADC sequencer control

    This API controls HKADC channel 6/channel 7 sequencer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the HKADC controller
    @param[in]      chId             HKADC channels. Valid channels are 6 and 7.
    @param[in]      enDis            1 : sequencer, generate the  ADC start from sequencer
                                     0 : non-sequencer, generate the ADC start from HKADC controller.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_SEQUENCER

    @limitations None
*/
int32_t HKADC_DrvSeqControl(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, uint8_t enDis);

/**
    @brief House keeping ADC sequencer read/write

    @trace #BRCM_SWREQ_HKADC_SEQUENCER
*/

typedef struct sHKADC_SeqMemRdWrType {
    uint8_t readWrite;                      /**< @brief 0 :Read 1:Write sequencer memory*/
    uint8_t startAddress;                   /**< @brief Start Address for sequencer indirect address */
    uint8_t dataSize;                       /**< @brief Size of sequencer data */
    uint8_t *dataEndFlag;                   /**< @brief End flag. Sequencer will stop after processing this entry for
                                                        single shot mode and will restart processing from
                                                        0th entry for continuous mode. */
    uint8_t *dataRptCnt;                    /**< @brief Repeat count. The entry will be executed SEQ_RPT_CNT+1 times*/
    uint8_t *seqID;                         /**< @brief Indicate the ID of the sequence*/
    uint16_t *spiData;                       /**< @brief SPI data to be send out, after the current entry is processed*/
    uint16_t *postWaitCnt;                   /**< @brief Time for which the sequence runs N clock cycles of HKADC (10ns)*/
} HKADC_SeqMemRdWrType;

/** @brief HKADC sequencer Memory

    This API reads/write HKADC channel 6/channel 7 sequencer memory.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the HKADC controller
    @param[in]      chId             HKADC channels. Valid channels are 6 and 7.
    @param[inout]   seqMem           Sequencer memory

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_SEQUENCER

    @limitations None
*/
int32_t HKADC_DrvSeqMemRdWr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId,  HKADC_SeqMemRdWrType *seqMem);

/**
    @brief House keeping ADC sequencer software SPI

    @trace #BRCM_SWREQ_HKADC_SEQUENCER
 */

typedef struct sHKADC_SwSpiRdWrType {
    uint8_t readWrite;                      /**< @brief 0: Read 1: Write SW SPI Data*/
    uint8_t readDone;                       /**< @brief 0: Read Inprogress 1 : read completed*/
    uint8_t readCmd;                        /**< @brief 0:1 byte 1: 2 bytes, 2: 4 bytes */
    uint8_t readCfg;                        /**< @brief Bytes to read 0: 1byte 1: 2 bytes */
    uint16_t data;                         /**< @brief Software spi command to be sent out/read */
    uint16_t turnaround;                    /**< @brief Number of SCK clock cycles the CSn is asserted between mosi write to read */
} HKADC_SwSpiRdWrType;

/** @brief HKADC sequencer Software SPI control

    This API reads/write HKADC channel 6/channel 7 sequencer software SPI control.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the HKADC controller
    @param[in]      chId             HKADC channels. Valid channels are 6 and 7.
    @param[inout]   swSpi            Sw SPI memory

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_SEQUENCER

    @limitations None
*/
int32_t HKADC_DrvSwSpiRdWr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_SwSpiRdWrType *swSpi);

#endif /* HKADC_H */

/** @} */
