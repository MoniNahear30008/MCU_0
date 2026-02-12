/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_hsadcdrv_ifc HSADC
    @ingroup grp_hsadcdrv

    @addtogroup grp_hsadcdrv_ifc
    @{
    @section sec_hsadc_overview Overview
    -# HighSpeed ADC Bare Metal driver interface provides layer 1 APIs to initialize, calibrate and configure
    the HSADC in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each HSADC HW instance needs to be initialized separately using HSADC_DrvInit API.
    -# It is user's responsibility to configure DMA separately to handle data transfers.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_hsadcdrv_fsm Usage
    -# HSADC HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   HSADC_ACQ_0 |
       | 1           |   HSADC_ACQ_1 |
       | 2           |   HSADC_ACQ_2 |
       | 3           |   HSADC_ACQ_3 |

    @section api_list List of HSADC APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref HSADC_DrvInit | Initialize HSADC |
    | @ref HSADC_DrvDeInit | De-initialize HSADC |
    | @ref HSADC_DrvConfigSamplingMode | Configure HSADC sampling mode |
    | @ref HSADC_DrvInitCalibration | HSADC Initialize calibration|
    | @ref HSADC_DrvTriggerCalib    | Trigger calibration|
    | @ref HSADC_DrvConfigCaptureMode | HSADC configure capture mode|
    | @ref HSADC_DrvConfigAcqController | HSADC configure acquisition controller|
    | @ref HSADC_DrvCaptureStatus | HSADC get capture status |
    | @ref HSADC_DrvFftConfig | HSADC configure FFT |
    | @ref HSADC_DrvConfigReadDoneAddress | HSADC configure read done address |
    | @ref HSADC_DrvConfigReadCapSize | HSADC configure read capture size|
    | @ref HSADC_DrvWindowMemConfig | HSADC window memory configuration |
    | @ref HSADC_DrvWindowControl | Enable/Disable window |
    | @ref HSADC_DrvClampConfig | Configure Clamp |
    | @ref HSADC_DrvTofOffsetConfig | Configure TOF offset  |
    | @ref HSADC_DrvConfigFirCoeff | Configure FIR coefficient   |
    | @ref HSADC_DrvConfigFilterGain | Configure filter gain   |
    | @ref HSADC_DrvFilterControl | Enable/Disable filter    |
    | @ref HSADC_DrvFftConfigAxisCapture | Configure Axis capture    |
    | @ref HSADC_DrvFftGetAxisInformation | Get Axis data    |
    | @ref HSADC_DrvFftGetCalibStatus | Get calibration status    |
    | @ref HSADC_DrvConfigChIntr | Configure channel Interrupt    |
    | @ref HSADC_DrvClearChIntr | clear channel Interrupt    |
    | @ref HSADC_DrvGetChIntrStatus | Get channel Interrupt status  |
    | @ref HSADC_DrvConfigIntr | Configure non-channel based Interrupt  |
    | @ref HSADC_DrvClearIntr | HSADC clear non Channel based interrupt |
    | @ref HSADC_DrvGetIntrStatus | HSADC Get non Channel interrupt Status |

    @file hsadc.h
    @brief Interface for HSADC driver
    @section apis This section defines the interface for HSADC driver
    @version 0.1 Initial Version
*/

#ifndef HSADC_H
#define HSADC_H

#include <stdint.h>

/**
    @name HSADC driver API IDs
    @{
    @brief API IDs for HSADC drier
 */
#define BRCM_SWARCH_HSADC_HW_ID_TYPE                          (0x8501U)    /**< @brief #HSADC_HwIDType     */
#define BRCM_SWARCH_HSADC_SAMPLING_MODE_TYPE                  (0x8502U)    /**< @brief #HSADC_SamplingModeType */
#define BRCM_SWARCH_HSADC_CAPTURE_MODE_TYPE                   (0x8503U)    /**< @brief #HSADC_CaptureModeType   */
#define BRCM_SWARCH_HSADC_CLAMP_CTRL_CFG_TYPE                 (0x8504U)    /**< @brief #HSADC_ClampControlConfigType   */
#define BRCM_SWARCH_HSADC_FFT_CFG_TYPE                        (0x8505U)    /**< @brief #HSADC_FftConfigType     */
#define BRCM_SWARCH_HSADC_FFT_AXIS_INFO_TYPE                  (0x8506U)    /**< @brief #HSADC_FftGetAxisInformationType    */
#define BRCM_SWARCH_HSADC_CALIB_CONFIG_TYPE                   (0x8507U)    /**< @brief #HSADC_CalibConfigType   */

#define BRCM_SWARCH_HSADC_DRV_INIT_PROC                       (0x8509U)    /**< @brief #HSADC_DrvInit                  */
#define BRCM_SWARCH_HSADC_DRV_DEINIT_PROC                     (0x850AU)    /**< @brief #HSADC_DrvDeInit                */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_SAMPLING_MODE_PROC       (0x850BU)    /**< @brief #HSADC_DrvConfigSamplingMode    */
#define BRCM_SWARCH_HSADC_DRV_INIT_CALIBRATION_PROC           (0x850CU)    /**< @brief #HSADC_DrvInitCalibration       */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_CAPTURE_MODE_PROC        (0x850DU)    /**< @brief #HSADC_DrvConfigCaptureMode     */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_ACQCONTROLLER_PROC       (0x850EU)    /**< @brief #HSADC_DrvConfigAcqController   */
#define BRCM_SWARCH_HSADC_DRV_CAPTURE_STATUS_PROC             (0x850FU)    /**< @brief #HSADC_DrvCaptureStatus         */
#define BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_PROC                 (0x8510U)    /**< @brief #HSADC_DrvFftConfig             */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_READ_DONEADDRESS_PROC    (0x8511U)    /**< @brief #HSADC_DrvConfigReadDoneAddress */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_READ_CAPSIZE_PROC        (0x8512U)    /**< @brief #HSADC_DrvConfigReadCapSize     */
#define BRCM_SWARCH_HSADC_WINDOW_OP_TYPE                      (0x8513U)    /**< @brief #HSADC_WindowOpType             */
#define BRCM_SWARCH_HSADC_DRV_WINDOWMEM_CONFIG_PROC           (0x8514U)    /**< @brief #HSADC_DrvWindowMemConfig       */
#define BRCM_SWARCH_HSADC_DRV_WINDOW_CONTROL_PROC             (0x8515U)    /**< @brief #HSADC_DrvWindowControl         */
#define BRCM_SWARCH_HSADC_DRV_CLAMP_CONFIG_PROC               (0x8516U)    /**< @brief #HSADC_DrvClampConfig           */
#define BRCM_SWARCH_HSADC_DRV_TOFOFFSET_CONFIG_PROC           (0x8517U)    /**< @brief #HSADC_DrvTofOffsetConfig       */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_FIRCOEFF_PROC            (0x8518U)    /**< @brief #HSADC_DrvConfigFirCoeff        */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_FILTERGAIN_PROC          (0x8519U)    /**< @brief #HSADC_DrvConfigFilterGain      */
#define BRCM_SWARCH_HSADC_DRV_FILTER_CONTROL_PROC             (0x851AU)    /**< @brief #HSADC_DrvFilterControl         */
#define BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_AXIS_CAPTURE_PROC    (0x851BU)    /**< @brief #HSADC_DrvFftConfigAxisCapture  */
#define BRCM_SWARCH_HSADC_DRV_FFT_GET_AXIS_INFORMATION_PROC   (0x851CU)    /**< @brief #HSADC_DrvFftGetAxisInformation */
#define BRCM_SWARCH_HSADC_DRV_FFT_GET_CALIB_STATUS_PROC       (0x851DU)    /**< @brief #HSADC_DrvFftGetCalibStatus     */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_CH_INTR_PROC             (0x851EU)    /**< @brief #HSADC_DrvConfigChIntr          */
#define BRCM_SWARCH_HSADC_DRV_CLEAR_CH_INTR_PROC              (0x851FU)    /**< @brief #HSADC_DrvClearChIntr           */
#define BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC         (0x8520U)    /**< @brief #HSADC_DrvGetChIntrStatus       */
#define BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC                (0x8521U)    /**< @brief #HSADC_DrvConfigIntr            */
#define BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC                 (0x8522U)    /**< @brief #HSADC_DrvClearIntr             */
#define BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC            (0x8523U)    /**< @brief #HSADC_DrvGetIntrStatus         */
#define BRCM_SWARCH_HSADC_CALIB_STATUS_TYPE                   (0x8524U)    /**< @brief #HSADC_CalibStatusType          */
#define BRCM_SWARCH_HSADC_WINDOW_MEM_TYPE                     (0x8525U)    /**< @brief #HSADC_WindowMemType            */
#define BRCM_SWARCH_HSADC_CHANNEL_ID_TYPE                     (0x8526U)    /**< @brief #HSADC_ChannelIDType            */
#define BRCM_SWARCH_HSADC_CHANNEL_INTR_TYPE                   (0x8527U)    /**< @brief #HSADC_ChannelIntrType          */
#define BRCM_SWARCH_HSADC_NONCHANNEL_INTR_TYPE                (0x8528U)    /**< @brief #HSADC_NonChannelIntrType       */
#define BRCM_SWARCH_HSADC_FFT_AXIS_CAP_CONFIG_TYPE            (0x8529U)    /**< @brief #HSADC_FftAxisCapConfigType     */
#define BRCM_SWARCH_HSADC_DRV_TRIGGR_CALIB_PROC               (0x8530U)    /**< @brief #HSADC_DrvTriggerCalib          */
#define BRCM_SWARCH_HSADC_STATE_TYPE                          (0x8531U)    /**< @brief #HSADC_StateType                */
#define BRCM_SWARCH_HSADC_DRV_GET_BUF_STS_PROC                (0x8532U)    /**< @brief #HSADC_DrvGetBufStatus          */

/** @} */

/**
    @brief Index of the HSADC controller

    @trace #BRCM_SWREQ_HSADC_INIT
 */
typedef uint32_t HSADC_HwIDType;


/**
    @name HSADC_SamplingMode
    @{
    @brief HighSpeed ADC channel Sampling mode

    @trace #BRCM_SWREQ_HSADC_CONFIGURATION
 */
typedef uint32_t HSADC_SamplingModeType;        /**< @brief HSADC channel Sampling Mode */
#define HSADC_SAMPLING_MODE_1P25G       (0UL)   /**< @brief 1.25G channel Sampling Mode*/
#define HSADC_SAMPLING_MODE_5G          (1UL)   /**< @brief 5G    channel Sampling Mode*/
#define HSADC_SAMPLING_MODE_MAX         (2UL)   /**< @brief Maximum Sampling Mode*/
/** @} */

/**
    @name HSADC_CaptureMode
    @{
    @brief HighSpeed ADC channel Capture mode

    @trace #BRCM_SWREQ_HSADC_CONFIGURATION
 */
typedef uint32_t HSADC_CaptureModeType;         /**< @brief HSADC Capture mode type */
#define HSADC_CAPTURE_MODE_NONE         (0UL)   /**< @brief HSADC No capture*/
#define HSADC_CAPTURE_MODE_SINGLE       (1UL)   /**< @brief HSADC single capture mode*/
#define HSADC_CAPTURE_MODE_AGGREGATE    (2UL)   /**< @brief HSADC aggregate capture mode*/
#define HSADC_CAPTURE_MODE_CONTINUOUS   (3UL)   /**< @brief HSADC continuous capture mode*/

/** @} */

/**
    @name HSADC_FftAxisCapConfig
    @{
    @brief HighSpeed ADC FFT Axis capture configuration

    @trace #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
 */
typedef uint32_t HSADC_FftAxisCapConfigType;          /**< @brief HSADC FFT axis Capture mode type */
#define HSADC_FFT_AXIS_CAP_CONFIG_ADCBUF      (0UL)   /**< @brief HSADC current axis information is captured into the ADC buffer in the last location of the capture */
#define HSADC_FFT_AXIS_CAP_CONFIG_FFT         (1UL)   /**< @brief HSADC the current axis inforamtion is stored in the last two bins of the FFT*/

/** @} */


/**
    @brief High speed ADC clamp configuration

    @trace #BRCM_SWREQ_HSADC_CONFIGURATION
 */

typedef struct sHSADC_ClampControlConfigType {
  uint16_t adcClampDelay;                 /**< @brief  Delay from the trigger signal to when the clamp is going to be generated. */
  uint16_t adcClampDuration;               /**< @brief  The time for which the clamp stays high. Each tick is 3.2ns. */
  uint8_t adcClampEnable;                /**< @brief  Enable/Disable AFE Clamp */
} HSADC_ClampControlConfigType;




/**
    @brief High speed ADC FFT Configuration

    @trace #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
 */
typedef struct sHSADC_FftConfigType {
  uint16_t fftEnable;              /**< @brief Enable/Disable FFT  */
  uint16_t fftSize;                /**< @brief Sample Size 7: 8k  6: 4k  5: 2k  4: 1k  3: 512  2-0: NA */
  uint16_t fftReadPeriod;          /**< @brief Time for which FFT buffer is enabled to perform read*/
  uint16_t captureStartAddress;    /**< @brief Starting address of the ADC buffer from which the data is stored
                                               In 1.25G, the address is defined in increments of 8 samples.i.e 7 would mean
                                               the first capture sample would be stored at 7*8+1 = 57th sample space in memory.
                                               In 1.25 mode this should only be programmed in increments of 2, odd values only.
                                               In 5G, the address is defined in increments of 32 samples, i.e 7 would mean
                                               the first capture sample would be stored at 7*32+1=225th sample space in memory.*/
} HSADC_FftConfigType;

/**
    @brief High speed ADC FFT Get Axis information

    @trace #BRCM_SWREQ_HSADC_CONFIGURATION
 */
typedef struct sHSADC_FftGetAxisInformationType {
  uint32_t fftAxisInfoReadLow;     /**< @brief Read information of Axis information low*/
  uint32_t fftAxisInfoReadHigh;    /**< @brief Read information of Axis information high*/
} HSADC_FftGetAxisInformationType;

/**
    @brief High speed ADC calibration configuration

    @trace #BRCM_SWREQ_HSADC_CALIBRATION
 */
typedef struct sHSADC_CalibConfigType {
  uint16_t calStatesEn;           /**< @brief Bit 0: Enables FG offset, Bit 1: FG daccal Bit 2: BG offset and gain Bit 3: bg_offset Bit 4: bg_gain */
  uint16_t calBgRdbDuration;      /**< @brief Time for which the BG calibration is run.  Time = 204.8ns*programmed value */
} HSADC_CalibConfigType;

/**
    @brief High speed ADC calibration status

    @trace #BRCM_SWREQ_HSADC_CALIBRATION
 */

typedef struct sHSADC_CalibStatusType {
  uint16_t calStatus;       /**< @brief Calibration status 1 : Calibration in progress 0: Calibration Done */
} HSADC_CalibStatusType;


/**
    @name HSADC_WindowOpType
    @brief HSADC window memory operation

    @trace #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
 */
typedef uint32_t HSADC_WindowOpType;
#define HSADC_WINDOW_OP_READ              (0UL)   /**< @brief Read operation on window memory */
#define HSADC_WINDOW_OP_WRITE             (1UL)   /**< @brief Write operation on window memory */

/**
    @brief HSADC window Memory configuration

    @trace #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
 */
typedef struct sHSADC_WindowMemType {
    HSADC_WindowOpType opMode;              /**< @brief Read/Write Operation to be performed*/
    uint32_t *data;                         /**< @brief Write/read data to/from window memory HSADC.
                                                        This array should be one fourth of fft size. */
    uint16_t fftSize;                       /**< @brief fftSize @ref HSADC_FftConfigType*/
} HSADC_WindowMemType;

/** @brief Initialize HSADC

    This API initializes the given HSADC hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_INIT

    @trace  #BRCM_SWREQ_HSADC_INIT

    @limitations None
*/

int32_t HSADC_DrvInit(HSADC_HwIDType  hwId);

/** @brief De-initialize HSADC

    This API de-initializes HSADC. Resets the given HSADC controller and puts back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_UNINIT

    @trace  #BRCM_SWREQ_HSADC_INIT

    @limitations None
*/

int32_t HSADC_DrvDeInit (HSADC_HwIDType  hwId);

/** @brief HSADC configure sampling mode

    This API configures HSADC with specified sampling mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[in]      samplingMode High speed ADC sampling mode
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigSamplingMode(HSADC_HwIDType  hwId, HSADC_SamplingModeType samplingMode);

/** @brief HSADC Calibration configuration

    This API configures and enables HSADC hardware background or foreground calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      calibConfig    HSADC calibration configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_CALIB_INPROGRESS

    @trace  #BRCM_SWREQ_HSADC_CALIBRATION

    @limitations None
*/
int32_t HSADC_DrvInitCalibration(HSADC_HwIDType  hwId,  HSADC_CalibConfigType *calibConfig);

/** @brief HSADC Triggers Calibration

    This API triggers the start of calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post  None

    @trace  #BRCM_SWREQ_HSADC_CALIBRATION

    @limitations None
*/
int32_t HSADC_DrvTriggerCalib(HSADC_HwIDType  hwId);

/** @brief HSADC configure capture mode

    This API configures HSADC with specified capture mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[in]      captureMode  High speed ADC sampling mode
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigCaptureMode(HSADC_HwIDType  hwId, HSADC_CaptureModeType captureMode);

/** @brief HSADC configure Acquisition

    This API configures HSADC Acquisition controller .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the HSADC controller
    @param[in]      channelEnable   Channel to be enabled,
                                    In 1.25G sampling mode each bit indicated the respective channel. Only 1,3,7 and 15 are valid values.
                                    In 5G sampling mode all 4 bits have to be set.
    @param[in]      decimationRate  Decimation Rate 0: No decimation  1: decimate by 2    2: decimate by 4
    @param[in]      aggregateCount  Number of captures to aggregate. Range from 1 to 32
    @param[in]      captureSize     Capture size in multiple of 16 samples.max of 32k samples. Each sample is 2 bytes of data. Example: To capture 4K samples program 4096/16 -1 = 255
    @return     int32_t


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigAcqController(HSADC_HwIDType  hwId, uint8_t channelEnable, uint8_t decimationRate, uint8_t aggregateCount, uint16_t captureSize);


/** @brief HSADC capture status

    This API retrives HSADC capture status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[out]     captureBusy  1 - Indicated that the capture is ongoing.
                                     In single mode it will be high during any given capture.
                                     In continous mode it will be high till the last capture in continous mode.
                                     In aggregation mode it will be high till the completion of a given aggregation.
                                 0 - Idel
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvCaptureStatus(HSADC_HwIDType  hwId, uint8_t *captureBusy);

/** @brief HSADC FFT configuration

    This API configures HSADC FFT pre-divider, sample size, axis information and enables the FFT.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      fftConfig     HSADC FFT configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvFftConfig(HSADC_HwIDType  hwId, HSADC_FftConfigType *fftConfig);

/** @brief Configures HSADC read done address
 *
    This API configures the read done address.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[out]     rdDoneAddr          Read Address to be configured.
                                        In 1.25G only the bottom 9 address bits have a meaning.
                                        In 5G all 11 bits have a meaning.
                                        Note: Reading from this address location will
                                        indicate that the OCP has read the captured data during the read phase.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigReadDoneAddress(HSADC_HwIDType  hwId, uint16_t rdDoneAddr);

/** @brief HSADC configure Read capture size

    This API defines how long time is allocated for either the FFT logic or
    OCP to read the ADC buffer data.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      readCapSize         This is in multiples of 16 samples.
                                        In non-FFT or FFT mode, this should be programmed
                                        to be few clock cycles (~5) less than the spacing between two triggers.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigReadCapSize(HSADC_HwIDType  hwId, uint16_t readCapSize);

/** @brief HSADC Window memory read/Write

    This API reads/writes data to the window memory based on fftSize. This will be used only when FFT is
    enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      windowConfig  HSADC window data
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvWindowMemConfig(HSADC_HwIDType  hwId, HSADC_WindowMemType *windowConfig);

/** @brief HSADC enables/disables window function

    This API enables/disables HSADC window function. This should be set only after
    configuring window @ref HSADC_DrvWindowMemConfig.


    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[in]      enWindow     0 - Disable window function 1 - enables window function
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvWindowControl(HSADC_HwIDType  hwId, uint8_t enWindow);


/** @brief HSADC clamp config

    This API configures clamp controls.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller
    @param[in]      clampConfig  HSADC clamp configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvClampConfig(HSADC_HwIDType  hwId, HSADC_ClampControlConfigType *clampConfig);

/** @brief HSADC TOF configuration

    This API configures time of flight offset.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the HSADC controller
    @param[in]      tofOffset     HSADC time of flight offset.
                                  This is a signed value that will be added to the incoming ADC signal to push
                                  the signal to positive direction.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvTofOffsetConfig(HSADC_HwIDType  hwId, uint16_t tofOffset);

/** @brief HSADC FIR coeffecient configuration

    This API configures filter coefficients.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HSADC controller
    @param[in]      firCoefficient HSADC FIR Coefficient array. Totally support 32 co-efficients.
                                   This is a signed 10-bit value.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigFirCoeff(HSADC_HwIDType  hwId, uint16_t *firCoefficient);

/** @brief HSADC post filter gain configuration

    This API configures post filter gain. This can be applied even when filter is
    in disabled state.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HSADC controller
    @param[in]      dGain          This set the post filter gain.
                                   The gain is applied even when the filter is disabled.
                                   0 -  1x   1 - 2x   2 - 4x   3 - 8x
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvConfigFilterGain(HSADC_HwIDType  hwId, uint8_t dGain);

/** @brief HSADC enables/disables Filter

    This API enables/disables filter path, This should be set after enabling the
    clock and after writing the coeffecient using @ref HSADC_DrvConfigFirCoeff.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HSADC controller
    @param[in]      enControl      1- enable filter path 0 - Disable
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvFilterControl(HSADC_HwIDType  hwId, uint8_t enControl);

/** @brief HSADC configure FFT axis capture

    This API configures FFT axis capture information.
    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId                    Index of the HSADC controller
    @param[in]     HSADC_FftAxisCapConfig HSADC FFT axis capture configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvFftConfigAxisCapture(HSADC_HwIDType  hwId,  HSADC_FftAxisCapConfigType fftAxisCapConfig);


/** @brief HSADC gets FFT axis information

    This API gets HSADC FFT axis information. Capturing of axis information depends on
    HSADC_FftAxisCapConfig value in @ref HSADC_DrvFftConfigAxisCapture.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[out]     fftAxisInfo HSADC FFT axis information
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvFftGetAxisInformation(HSADC_HwIDType  hwId,  HSADC_FftGetAxisInformationType *fftAxisInfo);

/** @brief HSADC Calibration Status

    This API gets HSADC calibration status. Calibration status in not valid if calibration is not enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[out]     calibStatus           HSADC calibration status
                                          1 : Calibration in progress
                                          0 : Calibration completed
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_CALIB_INPROGRESS/HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CALIBRATION

    @limitations None
*/
int32_t HSADC_DrvFftGetCalibStatus(HSADC_HwIDType  hwId,  HSADC_CalibStatusType *calibStatus);

/**
    @brief Index of the HSADC channel

    @trace #BRCM_SWREQ_HSADC_INIT
 */
typedef uint32_t HSADC_ChannelIDType;

#define HSADC_CHANNEL_ID_MAX             (3UL)  /**< @brief 4 channels (0-3)*/

/**
    @name HSADC_ChannelIntrType
    @{
    @brief HighSpeed ADC channel Interrupts or Events.

    @trace #BRCM_SWREQ_HSADC_INTERRUPT
 */
typedef uint32_t HSADC_ChannelIntrType;             /**< @brief HSADC channel based interrupt type */
#define HSADC_CHANNEL_INTR_FFT_DONE        (0x1UL)    /**< @brief FFT complete interrupt */
#define HSADC_CHANNEL_INTR_CAP_DONE        (0x2UL)    /**< @brief ADC Buffer capture is complete */
#define HSADC_CHANNEL_INTR_DMA_ERR         (0x4UL)    /**< @brief Previous DMA request for Buf ( adc/fft depending on mode)
                                                                hasnt been cleared before new data arrived.*/
#define HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR  (0x8UL)    /**< @brief OCP attempting to read buf0 ( adc/fft depending on mode)
                                                                when it doesnt have access.*/
#define HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR  (0x10UL)   /**< @brief When FFT enabled, FFT buffer write window clashes with read window. */
#define HSADC_CHANNEL_INTR_ALL             (0x1FUL)   /**< @brief Select all the interrupts mentioned above */
#define HSADC_CHANNEL_INTR_GET_EVENT_ALL        (0x10000UL)   /**< @brief Select all the interrupts mentioned above */
/** @} */

/**
    @name HSADC_StateType
    @{
    @brief State of HSADC

    @trace #BRCM_SWREQ_HSADC_INIT
*/

typedef uint8_t HSADC_StateType;                     /**< @brief HSADC state type */
#define HSADC_STATE_INITIALIZED    1U                 /**< @brief HSADC is uninitialized & value must be zero*/
#define HSADC_STATE_UNINIT         0U                 /**< @brief HSADC is initialized */

/** @} */

/** @brief HSADC Enable Channel interrupt

    This API enables/disables HSADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      chId                 Index of the HSADC Channel
    @param[in]      intrType             HSADC interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT

    @limitations None
*/
int32_t HSADC_DrvConfigChIntr(HSADC_HwIDType  hwId,  HSADC_ChannelIDType chId, HSADC_ChannelIntrType intrType, uint32_t enaDis);

/** @brief HSADC clears Channel interrupt

    This API clears HSADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      chId                 Index of the HSADC Channel
    @param[in]      HSADC_IntType        HSADC interrupt type
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT

    @limitations None
*/
int32_t HSADC_DrvClearChIntr(HSADC_HwIDType  hwId,  HSADC_ChannelIDType chId, HSADC_ChannelIntrType intrType);

/** @brief HSADC Gets Channel interrupt Status

    This API gets HSADC channel based interrupts or event status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      chId                 Index of the HSADC Channel
    @param[in]      HSADC_IntType        HSADC interrupt/Event type
    @param[out]     intrStatus           HSADC interrupt Status
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT

    @limitations None
*/

int32_t HSADC_DrvGetChIntrStatus(HSADC_HwIDType  hwId,  HSADC_ChannelIDType chId, HSADC_ChannelIntrType intrType, uint32_t *intrStatus);

/**
    @name HSADC_NonChannelIntrType
    @{
    @brief HighSpeed ADC Interrupts or Event

    @trace #BRCM_SWREQ_HSADC_INTERRUPT
 */
typedef uint32_t HSADC_NonChannelIntrType;                          /**< @brief HSADC Non channel based interrupt type */
#define HSADC_NONCHANNEL_INTR_CALIB_ERR                   (0x1UL)   /**< @brief ADC calibration error encountered during foreground calibration*/
#define HSADC_NONCHANNEL_INTR_CALIB_DONE                  (0x2UL)   /**< @brief ADC calibration complete */
#define HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE           (0x4UL)   /**< @brief This is useful for aggregate and continous mode, indicates the mode of
                                                                                    operation is complete. In continous mode this will be set when capture is done after exiting the mode.
                                                                                    In aggregate mode it will be set after an aggregation cycle is comple. Rising Edge Interrupt Enable*/
#define HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR            (0x8UL)   /**< @brief Data overflow in FP17 computation inside FFT occured. Rising Edge Interrupt Enable*/
#define HSADC_NONCHANNEL_INTR_FFT_IN_ERR                 (0x10UL)   /**< @brief Error indicating that the incoming data is arriving faster than what FFT can handle
                                                                                    Rising Edge Interrupt Enable */
#define HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR        (0x20UL)   /**< @brief Window Buffer memory initialization error, High Level Interrupt Enable*/
#define HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE       (0x40UL)   /**< @brief Window Buffer memory initialization done High Level Interrupt Enable */
#define HSADC_NONCHANNEL_INTR_WNDBUF_SEC                 (0x80UL)   /**< @brief Window Buffer single error correction error High Level Interrupt Enable   */
#define HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR       (0x100UL)   /**< @brief FFT buffer memory initialization error High Level Interrupt Enable  */
#define HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE      (0x200UL)   /**< @brief FFT buffer memory initialization done High Level Interrupt Enable */
#define HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR          (0x400UL)   /**< @brief FFT memory initialization error High Level Interrupt Enable  */
#define HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE         (0x800UL)   /**< @brief FFT memory initialization done High Level Interrupt Enable */
#define HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR      (0x1000UL)   /**< @brief ADC buffer memory initialization error High Level Interrupt Enable  */
#define HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE     (0x2000UL)   /**< @brief ADC buffer memory initialization done High Level Interrupt Enable */
#define HSADC_NONCHANNEL_INTR_WNDBUF_DED               (0x4000UL)   /**< @brief Window Buffer double error detection Rising Edge Interrupt Enable */
#define HSADC_NONCHANNEL_INTR_ALL                      (0x7FFFUL)   /**< @brief Select all the interrupts mentioned above */
#define HSADC_NONCHANNEL_INTR_GET_EVENT_ALL              (0x1000000UL)   /**< @brief Select all the event mentioned above */


/** @} */
/** @brief HSADC enables non channel interrupt

    This API enables/disables HSADC non channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      intrType             HSADC interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT

    @limitations None
*/
int32_t HSADC_DrvConfigIntr(HSADC_HwIDType  hwId,  HSADC_NonChannelIntrType intrType, uint32_t enaDis);

/** @brief HSADC clears non Channel based interrupt

    This API clears HSADC non channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      intrType             HSADC interrupt type
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT

    @limitations None
*/
int32_t HSADC_DrvClearIntr(HSADC_HwIDType  hwId,  HSADC_NonChannelIntrType intrType);

/** @brief HSADC Gets non Channel interrupt Status

    This API gets HSADC non channel based interrupts or event status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      intrType             HSADC interrupt/Event type
    @param[out]     intrStatus           HSADC interrupt Status
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT

    @limitations None
*/

int32_t HSADC_DrvGetIntrStatus(HSADC_HwIDType  hwId,  HSADC_NonChannelIntrType intrType, uint32_t *intrStatus);

/** @brief HSADC get buffer Status

    This API gets HSADC buffer status based on the hwId.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller
    @param[in]      flag         Channel index and buffer clear. MSB 4 bits represents channel number.
                                 LSB 4 bits as follow,
                                 X1 : clear lost frame, X represents the channel number.
                                 X2 : clear buffer status, X represents the channel number.
    @param[out]     bufStatus    HSADC buffer A and B status.
                                 bit 0 and 1 represents Buffer A status
                                 bit 3 and 4 represents Buffer B status
                                 0 : Idle 1 : write phase
                                 2 : Read Done(RD_Done not observed)
                                 3 : Read Done(RD_Done observed)
    @param[out]     lostFrame    HSADC lost frame counter
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid bufStatus/calibStatus
    @retval     #BCM_ERR_NODEV          No valid device

    @post NONE 

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION

    @limitations None
*/
int32_t HSADC_DrvGetBufStatus(HSADC_HwIDType  hwId,  uint8_t flag, uint8_t *bufStatus, uint16_t *lostFrame);


#endif /* HSADC_H */

/** @} */
