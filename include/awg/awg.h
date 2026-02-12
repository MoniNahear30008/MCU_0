/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential

 ******************************************************************************/

/**
    @defgroup grp_awgdrv_ifc AWG
    @ingroup grp_awgdrv

    @addtogroup grp_awgdrv_ifc
    @{
    @section sec_awg_overview Overview
    @note
    -# The Arbitrary Waveform Generator (AWG) generates waveforms based on software configuration. AWG Bare Metal driver interface
       provides layer 1 APIs to initialize and configure the AWG in desired configuration. It is configured
       in non-blocking mode and APIs provided are non-reenterant.
    -# Each AWG HW instance needs to be initialized separately using HKDAC_DrvInit API.

    @note Please refer to respective technical reference manual for more details about the HW features.

    -# AWG HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   AWG |

    @section api_list List of AWG APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref AWG_DrvInit | Initialize AWG |
    | @ref AWG_DrvDeInit | De-initialize AWG |
    | @ref AWG_DrvChannelConfig | AWG channel configuration |
    | @ref AWG_DrvChannelEnableControl | AWG channel control |
    | @ref AWG_DrvChannelRunControl | AWG channel run control |
    | @ref AWG_DrvWaveformMemAccess | AWG waveform memory configuration |
    | @ref AWG_DrvGetCurrActivePage | Get current Active page|
    | @ref AWG_DrvSetActivePage     | Set channel active page|
    | @ref AWG_DrvGetCurrentPhase | AWG get current phase information |
    | @ref AWG_DrvChannelConfigFrequency | Configure frequency|
    | @ref AWG_DrvChannelRampConfig | Config ramp|
    | @ref AWG_DrvChannelRampControl | Ramp control|
    | @ref AWG_DrvConfigChIntr | AWG Enable Channel interrupt |
    | @ref AWG_DrvClearChIntr | AWG clear Channel interrupt |
    | @ref AWG_DrvGetChIntrStatus | AWG Get Channel interrupt Status |

    @file awg.h
    @brief Interface for AWG driver
    @section apis This section defines the interface for AWG driver

    @version 0.1 Initial Version
*/

#ifndef AWG_H
#define AWG_H

#include <stdint.h>

/**
    @name AWG driver API IDs
    @{
    @brief API IDs for AWG drier
 */
#define BRCM_SWARCH_AWG_HW_ID_TYPE                          (0x8901U)    /**< @brief #AWG_HwIDType     */
#define BRCM_SWARCH_AWG_CH_ID_TYPE                          (0x8902U)    /**< @brief #AWG_ChIDType  */
#define BRCM_SWARCH_AWG_CURRENT_PHASE_TYPE                  (0x8903U)    /**< @brief #AWG_CurrentPhaseType     */
#define BRCM_SWARCH_AWG_INCREMENT_MODE_TYPE                 (0x8904U)    /**< @brief #AWG_IncrementModeType */
#define BRCM_SWARCH_AWG_CHANNEL_NUM_PAGES_TYPE              (0x8905U)    /**< @brief #AWG_ChannelNumPagesType   */
#define BRCM_SWARCH_AWG_CHANNEL_ACTIVE_PAGE_TYPE            (0x8906U)    /**< @brief #AWG_ChannelActivePageType   */
#define BRCM_SWARCH_AWG_WAVEFORM_MEM_OP_TYPE                (0x8908U)    /**< @brief #AWG_WaveformMemOpType    */
#define BRCM_SWARCH_AWG_CHANNEL_CONFIG_TYPE                 (0x8909U)    /**< @brief #AWG_ChannelConfigType    */
#define BRCM_SWARCH_AWG_CHANNEL_CONTROL_TYPE                (0x890AU)    /**< @brief #AWG_ChannelControlType     */
#define BRCM_SWARCH_AWG_CHANNEL_WAVEFORM_MEM_TYPE           (0x890BU)    /**< @brief #AWG_ChannelWaveformMemType     */

#define BRCM_SWARCH_AWG_DRV_INIT_PROC                       (0x890CU)    /**< @brief #AWG_DrvInit    */
#define BRCM_SWARCH_AWG_DRV_DEINIT_PROC                     (0x890DU)    /**< @brief #AWG_DrvDeInit   */
#define BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_PROC             (0x890EU)    /**< @brief #AWG_DrvChannelConfig     */
#define BRCM_SWARCH_AWG_DRV_CHANNEL_ENABLE_CONTROL_PROC     (0x8910U)    /**< @brief #AWG_DrvChannelEnableControl  */
#define BRCM_SWARCH_AWG_DRV_WAVEFORM_MEM_ACCESS_PROC        (0x8911U)    /**< @brief #AWG_DrvWaveformMemAccess   */
#define BRCM_SWARCH_AWG_DRV_GET_CURRENT_PHASE_PROC          (0x8912U)    /**< @brief #AWG_DrvGetCurrentPhase   */
#define BRCM_SWARCH_AWG_CHANNEL_INTR_TYPE                   (0x8913U)    /**< @brief #AWG_ChannelIntrType   */
#define BRCM_SWARCH_AWG_DRV_CONFIG_CH_INTR_PROC             (0x8914U)    /**< @brief #AWG_DrvConfigChIntr   */
#define BRCM_SWARCH_AWG_DRV_CLEAR_CH_INTR_PROC              (0x8915U)    /**< @brief #AWG_DrvClearChIntr     */
#define BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC            (0x8916U)    /**< @brief #AWG_DrvGetChIntrStatus */
#define BRCM_SWARCH_AWG_DRV_GET_CURR_ACTIVE_PAGE_PROC       (0x8917U)    /**< @brief #AWG_DrvGetCurrActivePage   */
#define BRCM_SWARCH_AWG_DRV_SET_ACTIVE_PAGE_PROC            (0x8918U)    /**< @brief #AWG_DrvSetActivePage   */
#define BRCM_SWARCH_AWG_CHANNEL_ACTIVE_PAGE_LATCH_CTRL_TYPE (0x8919U)    /**< @brief #AWG_ChannelActivePageLatchCtrlType*/
#define BRCM_SWARCH_AWG_DRV_CHANNEL_RUN_CONTROL_PROC        (0x891AU)    /**< @brief #AWG_DrvChannelRunControl  */
#define BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_FREQUENCY_PROC   (0x891BU)    /**< @brief #AWG_DrvChannelConfigFrequency*/
#define BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONTROL_PROC       (0x891CU)    /**< @brief #AWG_DrvChannelRampControl*/
#define BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONFIG_PROC        (0x891DU)    /**< @brief #AWG_DrvChannelRampConfig*/
#define BRCM_SWARCH_AWG_STATE_TYPE                          (0x891EU)    /**< @brief #AWG_StateType     */

/** @} */

/**
    @brief Index of the AWG controller

    @trace #BRCM_SWREQ_AWG_INIT
 */
typedef uint32_t AWG_HwIDType;

/**
    @name AWG_StateType
    @{
    @brief State of AWG

    @trace #BRCM_SWREQ_AWG_INIT
*/

typedef uint32_t AWG_StateType;         /**< @brief AWG state type */
#define AWG_STATE_UNINIT           (0UL)       /**< @brief AWG is uninitialized & value must be zero*/
#define AWG_STATE_INITIALIZED      (1UL)       /**< @brief AWG is initialized */
/** @} */

/**
    @brief Index of the AWG channel

    @trace #BRCM_SWREQ_AWG_INIT
 */
typedef uint32_t AWG_ChIDType;

/**
    @brief Current phase of AWG channel

    @trace #BRCM_SWREQ_AWG_INIT
 */
typedef uint32_t AWG_CurrentPhaseType;

/**
    @name AWG_IncrementModeType
    @{
    @brief AWG increment mode

    @trace #BRCM_SWREQ_AWG_CH_CONFIGURATION
 */
typedef uint8_t AWG_IncrementModeType;                      /**< @brief AWG Increment mode type   */
#define AWG_INCREMENT_MODE_PHASE_ACCUMULATOR          (0U)   /**< @brief Phase Accumulator mode (Continuous phase increment Mode ) */
#define AWG_INCREMENT_MODE_PHASE_INCREMENT            (1U)   /**< @brief Phase increment on hardware increment signal  */
/** @} */

/**
    @name AWG_ChannelNumPagesType
    @{
    @brief AWG number of page

    @trace #BRCM_SWREQ_AWG_CH_CONFIGURATION
 */
typedef uint8_t AWG_ChannelNumPagesType;                    /**< @brief AWG channel number of page type */
#define AWG_CHANNEL_NUM_PAGES_ONE                    (0U)   /**< @brief AWG channel one page */
#define AWG_CHANNEL_NUM_PAGES_TWO                    (1U)   /**< @brief AWG channel two page */

/** @} */

/**
    @name AWG_ChannelActivePageType
    @{
    @brief AWG active page

    @trace #BRCM_SWREQ_AWG_CH_CONFIGURATION
 */
typedef uint8_t AWG_ChannelActivePageType;                    /**< @brief AWG channel number active page type */
#define AWG_CHANNEL_ACTIVE_PAGE_ZERO                    (0U)   /**< @brief AWG channel page 0 is active*/
#define AWG_CHANNEL_ACTIVE_PAGE_ONE                     (1U)   /**< @brief AWG channel page 1 is active */
/** @} */

/**
    @name AWG_ChannelActivePageLatchCtrlType
    @{
    @brief AWG active page

    @trace #BRCM_SWREQ_AWG_CH_CONFIGURATION
 */
typedef uint8_t AWG_ChannelActivePageLatchCtrlType;                              /**< @brief AWG channel active page latch control */
#define AWG_CHANNEL_ACTIVE_PAGE_LATCH_CTRL_LOOP_RESET                 (0U)   /**< @brief Configured active page takes effect on loop reset or HW trigger*/
#define AWG_CHANNEL_ACTIVE_PAGE_LATCH_CTRL_HW_TRIG                    (1U)   /**< @brief Configured active page takes effect only on HW trigger */
/** @} */


/**
    @name AWG_WaveformMemOpType
    @{
    @brief AWG waveform memory operation

    @trace #BRCM_SWREQ_AWG_WAVEFORM_MEM_CONFIG
 */
typedef uint8_t AWG_WaveformMemOpType;
#define AWG_WAVEFORM_MEM_OP_READ              (0U)   /**< @brief Read operation on Waveform memory */
#define AWG_WAVEFORM_MEM_OP_WRITE             (1U)   /**< @brief Write operation on Waveform memory */
/** @} */

/**
    @brief AWG channel configuration

    @trace #BRCM_SWREQ_AWG_CH_CONFIGURATION
 */

typedef struct sAWG_ChannelConfigType {
    AWG_IncrementModeType incrementMode;            /**< @brief  Channel Increment Mode*/
    uint8_t incrementCtrl;                          /**< @brief applicable only when incrementMode is in phase increment mode
                                                         0 - Phase increment on rising edge if HW increment input
                                                         1 - Phase increment when HW increment input signal is high */
    uint8_t clockDivider;                           /**< @brief  Clock divider for AWG Channel.*/
    AWG_ChannelNumPagesType numberPages;            /**< @brief  Number of page for  AWG channel*/
    AWG_ChannelActivePageLatchCtrlType activePageLatch; /**< @brief  Latch Control for Active page . */
    uint8_t rampResetCtrl;                          /**< @brief  Ramp reset control
                                                         0 : On loop reset, previous RAMP value holds for next loop
                                                         1 : On loop reset, RAMP value resets to defaultRampVal for next loop */
    uint8_t syncCtrl;                               /**< @brief Controls usage of bits 2:1 of AWG waveform memory
                                                      0 : Bit 2 of waveform memory used for ramp increment control (if set, ramp value is incremented
                                                          by rampIncrementVal), Bit 1 of waveform memory used as Sync1 signal.
                                                      1: Bits 2:1 of waveform memory used for ramp control.
                                                         00: Ramp value unchanged,
                                                         11: Ramp value reset to  defaultRampVal
                                                         10/01: Ramp value incremented/decremented by rampIncrementVal */
} AWG_ChannelConfigType;

/**
    @brief AWG channel control

    @trace #BRCM_SWREQ_AWG_CH_CONFIGURATION
 */

typedef struct sAWG_ChannelControlType {
    uint8_t enable;                             /**< @brief  AWG channel Enable 0 - Disable channel 1 - enable channel*/
} AWG_ChannelControlType;

/**
    @brief AWG channed Waveform Memory control

    @trace #BRCM_SWREQ_AWG_WAVEFORM_MEM_CONFIG
 */
typedef struct sAWG_ChannelWaveformMemType {
    AWG_WaveformMemOpType opMode;              /**< @brief Read/Write Operation to be performed*/
    uint32_t *data;                            /**< @brief Write/read data to/from waveform memory for AWG Channel.
                                                       31:16 - unused
                                                       15:04 - Digital data output [11:0]
                                                       03:03 - Loop reset/end flag. Memory replay will restart from the start of page when an entry with this flag is set.
                                                       02:02 - Ramp[1], ramp increment control. When set to 1, ramp value will increment by rampIncrementVal and will get applied for the next entry.
                                                       01:01 - Ramp[0]/Sync[1]. Functionality controlled by CH_SYNC_CTRL - please check CH_SYNC_CTRL description for more details.
                                                       00:00 - Sync[0] signal.*/
    uint16_t numberOfLocation;                 /**< @brief Number of location to read/write*/
    uint8_t pageNumber;                        /**< @brief page number */
} AWG_ChannelWaveformMemType;

/** @brief Initialize AWG

    This API initializes the given AWG hardware.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the AWG controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_UNINIT  Device or resource not initialized
    @retval     #BCM_ERR_OK      Initialized

    @post AWG driver state changes to #AWG_STATE_INITIALIZED

    @trace  #BRCM_SWREQ_AWG_INIT

    @limitations None
*/
int32_t AWG_DrvInit(AWG_HwIDType  hwId);

/** @brief De-initialize AWG

    This API de-initializes AWG. Resets the given AWG controller and puts back
    to its original state (same as Power on reset).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the AWG controller

    @return int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_OK             Success

    @post AWG driver state changes to #AWG_STATE_UNINIT

    @trace  #BRCM_SWREQ_AWG_INIT

    @limitations None
*/
int32_t AWG_DrvDeInit (AWG_HwIDType  hwId);


/** @brief AWG channel configuration

    This API configures AWG channel with user specified configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the AWG controller
    @param[in]      chId         Index of the AWG channel
    @param[in]      chConfig   channel configuration

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/

int32_t AWG_DrvChannelConfig (AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_ChannelConfigType *chConfig);

/** @brief AWG Ramp Configration

    This API configures ramp on a AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      rampIncrementVal     Ramp increment value for AWG Channel
    @param[in]      defaultRampVal Default ramp value for AWG channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/
int32_t AWG_DrvChannelRampConfig(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint16_t rampIncrementVal, uint16_t defaultRampVal);

/** @brief AWG Ramp control

    This API enables/disables ramp on a AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      rampCtrl      Enable(1)/Disable(0) ramp on channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/

int32_t AWG_DrvChannelRampControl(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t rampCtrl);

/** @brief AWG channel frequency

    This API configures channel frequency. This is valid for Phase Accumulator mode only.
    It specifies the step through which the accumulator increments on every clock cycle.

    @behavior Sync, Non-reentrant

    @pre None

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/

int32_t AWG_DrvChannelConfigFrequency(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint32_t frequency);

/** @brief AWG channel control

    This API enables/disables AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      chControl Enables/Disables AWG channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success


    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/
int32_t AWG_DrvChannelEnableControl(AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_ChannelControlType chControl);

/** @brief AWG channel run control

    This API pauses/resumes AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      runControl    Pauses/Resumes AWG channel. 0 - Pause 1 - Resume

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/
int32_t AWG_DrvChannelRunControl(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t chRunControl);

/** @brief AWG waveform memory configuration
    This API reads/writes AWG waveform memory based on channel.
    Write always uses auto address increment mode. Writes all the data on a page
    based on number of pages selected on @ref AWG_DrvChannelConfig.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                Index of the AWG controller
    @param[in]      chId                Index of the AWG channel
    @param[in]      chWaveFormMem       Read/write data

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_WAVEFORM_MEM_CONFIG

    @limitations None
*/

int32_t AWG_DrvWaveformMemAccess(AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_ChannelWaveformMemType *chWaveFormMem);

/** @brief AWG get current active page

    This API enables user to get current active page.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the AWG controller
    @param[in]      chId              Index of the AWG channel
    @param[out]     currActivePage    Current Active page. 0 - Page0 1 - Page1

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/
int32_t AWG_DrvGetCurrActivePage(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t *currActivePage);

/** @brief AWG Set channel current active page

    This API configures AWG channel active page, This API can be used only if
    numberPages is AWG_CHANNEL_NUM_PAGES_TWO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the AWG controller
    @param[in]      chId              Index of the AWG channel
    @param[in]      activePage        Set channel Active page. 0 - Page0 1 - Page1

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None
    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION

    @limitations None
*/

int32_t AWG_DrvSetActivePage(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t activePage);

/** @brief AWG gets current phase information

    This API gets AWG channel current phase information.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG channel
    @param[out]     AWG_CurPhase         Current phase of the specified channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post NONE

    @trace  #BRCM_SWREQ_AWG_GET_PHASE

    @limitations None
*/

int32_t AWG_DrvGetCurrentPhase(AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_CurrentPhaseType *curPhase);

/**
    @name AWG_ChannelIntrType
    @{
    @brief AWG channel Interrupts and event, rising Edge interrupts

    @trace #BRCM_SWREQ_AWG_INTERRUPT
 */
typedef uint32_t AWG_ChannelIntrType;                     /**< @brief AWG channel based interrupt type */

#define AWG_CHANNEL_INTR_MEM_SEC         (0x1UL)          /**< @brief AWG Memory Single error Correction */
#define AWG_CHANNEL_INTR_MEM_INIT_DONE   (0x2UL)          /**< @brief AWG Memory initialization Done  */
#define AWG_CHANNEL_INTR_MEM_INIT_ERR    (0x4UL)          /**< @brief AWG Memory initialization error  */
#define AWG_CHANNEL_INTR_MEM_DED         (0x8UL)          /**< @brief AWG Memory Double Error detection  */
#define AWG_CHANNEL_INTR_PG_SWITCH_DONE  (0x10UL)         /**< @brief AWG Memory page switch Done  */
#define AWG_CHANNEL_INTR_RESET           (0x20UL)         /**< @brief AWG Reset */
#define AWG_CHANNEL_INTR_ALL             (0x3FUL)         /**< @brief AWG Select all intrrupt */
#define AWG_CHANNEL_INTR_EVENT_ALL       (0x10000UL)      /**< @brief AWG Get event for all*/

/** @} */

/** @brief AWG Enable Channel interrupt

    This API enables/disables AWG channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG Channel
    @param[in]      intrType             AWG interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post   None

    @trace  #BRCM_SWREQ_AWG_INTERRUPT

    @limitations None
*/
int32_t AWG_DrvConfigChIntr(AWG_HwIDType  hwId,  AWG_ChIDType chId, AWG_ChannelIntrType intrType, uint8_t enaDis);

/** @brief AWG clears Channel interrupt

    This API clears AWG channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG Channel
    @param[in]      AWG_IntType          AWG interrupt type

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post   NA

    @trace  #BRCM_SWREQ_AWG_INTERRUPT

    @limitations None
*/
int32_t AWG_DrvClearChIntr(AWG_HwIDType  hwId,  AWG_ChIDType chId, AWG_ChannelIntrType intrType);

/** @brief AWG Get Channel interrupt Status

    This API gets AWG channel based interrupts status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the AWG controller
    @param[in]      chId                 Index of the AWG Channel
    @param[in]      AWG_IntType          AWG interrupt/Event type
                                         User can use AWG_CHANNEL_INTR_EVENT_ALL to get HW event for all the events @ref AWG_ChannelIntrType
    @param[out]     intrStatus           AWG interrupt Status

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post   None

    @trace  #BRCM_SWREQ_AWG_INTERRUPT

    @limitations None
*/

int32_t AWG_DrvGetChIntrStatus(AWG_HwIDType  hwId,  AWG_ChIDType chId, AWG_ChannelIntrType intrType, uint32_t *intrStatus);



#endif /* AWG_H */

/** @} */
