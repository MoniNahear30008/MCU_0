/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_hsrefdrv_ifc HSREF
    @ingroup grp_hsrefdrv

    @addtogroup grp_hsrefdrv_ifc
    @{
    @section sec_hsref_overview Overview
    -# HighSpeed Reference-Channel ADC Bare Metal driver interface provides layer 1 APIs to initialize, calibrate and configure
    the HSREF in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each HSREF HW instance needs to be initialized separately using HSREF_DrvInit API.
    -# It is user's responsibility to configure DMA separately to handle data transfers.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_hsrefdrv_fsm Usage
    -# HSREF HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   HSREF_ACQ_0 |

    @section api_list List of HSREF APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref HSREF_DrvInit           | Initialize HSREF                   |
    | @ref HSREF_DrvDeInit         | De-initialize HSREF                |
    | @ref HSREF_DrvInitCalib      | HSREF Initialize calibration       |
    | @ref HSREF_DrvTriggerCalib   | Trigger calibration                |
    | @ref HSREF_DrvCalibStatus    | Get calibration status             |
    | @ref HSREF_DrvCfgSvf         | State variable filter configuration|
    | @ref HSREF_DrvCtrlSvf        | State variable filter control      |
    | @ref HSREF_DrvCfgZcross      | Zero crossing circuit configuration|
    | @ref HSREF_DrvCtrlZcross     | Zero crossing circuit control      |
    | @ref HSREF_DrvFifoCtrlZcross | Zero cross FIFO control            |
    | @ref HSREF_DrvFifoStsZcross  | Zero cross FIFO status             |
    | @ref HSREF_DrvFifoReadZcross | Zero cross FIFO Read               |
    | @ref HSREF_DrvCfgCapBuf      | Capture buffer configuration       |
    | @ref HSREF_DrvCtrlCapBuf     | Capture buffer control             |
    | @ref HSREF_DrvFifoCtrlCapBuf | Capture buffer FIFO control        |
    | @ref HSREF_DrvFifoStsCapBuf  | Capture buffer FIFO status         |
    | @ref HSREF_DrvEnableInterrupt| Enable HSREF interrupts            |
    | @ref HSREF_DrvInterruptStatus| Gets HSREF interrupt status        |
    | @ref HSREF_DrvEventStatus    | Gets HSREF event status            |
    | @ref HSREF_DrvClearInterrupt | Clears HSREF interrupt             |
    @file hsref.h
    @brief Interface for HSREF driver
    @section apis This section defines the interface for HSREF driver
    @version 0.1 Initial Version
*/

#ifndef HSREF_H
#define HSREF_H

#include <stdint.h>

/**
    @name HSREF driver API IDs
    @{
    @brief API IDs for HSREF drier
 */
#define BRCM_SWARCH_HSREF_HW_ID_TYPE                     (0x8580U)    /**< @brief #HSREF_HwIDType                  */
#define BRCM_SWARCH_HSREF_STATE_TYPE                     (0x8581U)    /**< @brief #HSREF_StateType                 */
#define BRCM_SWARCH_HSREF_SVF_SEL_TYPE                   (0x8582U)    /**< @brief #HSREF_SvfSelType                */
#define BRCM_SWARCH_HSREF_CAP_BUF_LANE_TYPE              (0x8583U)    /**< @brief #HSREF_CapBufLaneType            */
#define BRCM_SWARCH_HSREF_CALIB_CONFIG_TYPE              (0x8584U)    /**< @brief #HSREF_CalibConfigType           */
#define BRCM_SWARCH_HSREF_CALIB_STATUS_TYPE              (0x8585U)    /**< @brief #HSREF_CalibStatusType           */
#define BRCM_SWARCH_HSREF_SVF_CONFIG_TYPE                (0x8587U)    /**< @brief #HSREF_SvfConfigType             */
#define BRCM_SWARCH_HSREF_SVF_CONTROL_TYPE               (0x8588U)    /**< @brief #HSREF_SvfControlType            */
#define BRCM_SWARCH_HSREF_ZCROSS_CONFIG_TYPE             (0x8589U)    /**< @brief #HSREF_ZcrossConfigType          */
#define BRCM_SWARCH_HSREF_ZCROSS_CONTROL_TYPE            (0x858AU)    /**< @brief #HSREF_ZcrossControlType         */
#define BRCM_SWARCH_HSREF_ZCROSS_FIFO_CONTROL_TYPE       (0x858BU)    /**< @brief #HSREF_ZcrossFifoControlType     */
#define BRCM_SWARCH_HSREF_ZCROSS_FIFO_STATUS_TYPE        (0x858CU)    /**< @brief #HSREF_ZcrossFifoStatusType      */
#define BRCM_SWARCH_HSREF_ZCROSS_FIFO_STS_TYPE           (0x858DU)    /**< @brief #HSREF_ZcrossFifoStsType         */
#define BRCM_SWARCH_HSREF_ZCROSS_FIFO_READ_TYPE          (0x858EU)    /**< @brief #HSREF_ZcrossFifoReadType        */
#define BRCM_SWARCH_HSREF_CAP_BUF_CONFIG_TYPE            (0x858FU)    /**< @brief #HSREF_CapBufConfigType          */
#define BRCM_SWARCH_HSREF_CAP_BUF_CONTROL_TYPE           (0x8590U)    /**< @brief #HSREF_CapBufControlType         */
#define BRCM_SWARCH_HSREF_CAP_BUF_FIFO_CONTROL_TYPE      (0x8591U)    /**< @brief #HSREF_CapBufFifoControlType     */
#define BRCM_SWARCH_HSREF_CAP_BUF_FIFO_STATUS_TYPE       (0x8592U)    /**< @brief #HSREF_CapBufFifoStatusType      */
#define BRCM_SWARCH_HSREF_SVF_CONFIG_MASK_TYPE           (0x8593U)    /**< @brief #HSREF_SvfConfigMaskType         */
#define BRCM_SWARCH_HSREF_ZCROSS_CONFIG_MASK_TYPE        (0x8594U)    /**< @brief #HSREF_ZcrossConfigMaskType      */
#define BRCM_SWARCH_HSREF_ZCROSS_FIFO_CONTROL_MASK_TYPE  (0x8595U)    /**< @brief #HSREF_ZcrossFifoControlMaskType */
#define BRCM_SWARCH_HSREF_CAP_BUF_CONFIG_MASK_TYPE       (0x8596U)    /**< @brief #HSREF_CapBufConfigMaskType      */
#define BRCM_SWARCH_HSREF_CAP_BUF_FIFO_CONTROL_MASK_TYPE (0x8597U)    /**< @brief #HSREF_CapBufFifoControlMaskType */
#define BRCM_SWARCH_HSREF_INTERRUPT_TYPE                 (0x8598U)    /**< @brief #HSREF_InterruptType             */

#define BRCM_SWARCH_HSREF_DRV_INIT_PROC                  (0x85A0U)    /**< @brief #HSREF_DrvInit                   */
#define BRCM_SWARCH_HSREF_DRV_DEINIT_PROC                (0x85A1U)    /**< @brief #HSREF_DrvDeInit                 */
#define BRCM_SWARCH_HSREF_DRV_INIT_CALIB_PROC            (0x85A2U)    /**< @brief #HSREF_DrvInitCalib              */
#define BRCM_SWARCH_HSREF_DRV_TRIGGER_CALIB_PROC         (0x85A3U)    /**< @brief #HSREF_DrvTriggerCalib           */
#define BRCM_SWARCH_HSREF_DRV_CALIB_STATUS_PROC          (0x85A4U)    /**< @brief #HSREF_DrvCalibStatus            */
#define BRCM_SWARCH_HSREF_DRV_CFG_SVF_PROC               (0x85A5U)    /**< @brief #HSREF_DrvCfgSvf                 */
#define BRCM_SWARCH_HSREF_DRV_CTRL_SVF_PROC              (0x85A6U)    /**< @brief #HSREF_DrvCtrlSvf                */
#define BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC            (0x85A7U)    /**< @brief #HSREF_DrvCfgZcross              */
#define BRCM_SWARCH_HSREF_DRV_CTRL_ZCROSS_PROC           (0x85A8U)    /**< @brief #HSREF_DrvCtrlZcross             */
#define BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_ZCROSS_PROC      (0x85A9U)    /**< @brief #HSREF_DrvFifoCtrlZcross         */
#define BRCM_SWARCH_HSREF_DRV_FIFO_STS_ZCROSS_PROC       (0x85AAU)    /**< @brief #HSREF_DrvFifoStsZcross          */
#define BRCM_SWARCH_HSREF_DRV_FIFO_READ_ZCROSS_PROC      (0x85ABU)    /**< @brief #HSREF_DrvFifoReadZcross         */
#define BRCM_SWARCH_HSREF_DRV_CFG_CAP_BUF_PROC           (0x85ACU)    /**< @brief #HSREF_DrvCfgCapBuf              */
#define BRCM_SWARCH_HSREF_DRV_CTRL_CAP_BUF_PROC          (0x85ADU)    /**< @brief #HSREF_DrvCtrlCapBuf             */
#define BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_CAP_BUF_PROC     (0x85AEU)    /**< @brief #HSREF_DrvFifoCtrlCapBuf         */
#define BRCM_SWARCH_HSREF_DRV_FIFO_STS_CAP_BUF_PROC      (0x85AFU)    /**< @brief #HSREF_DrvFifoStsCapBuf          */

#define BRCM_SWARCH_HSREF_DRV_ENABLE_INTERRUPT_PROC      (0x85B0U)    /**< @brief #HSREF_DrvEnableInterrupt        */
#define BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC      (0x85B1U)    /**< @brief #HSREF_DrvInterruptStatus        */
#define BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC          (0x85B2U)    /**< @brief #HSREF_DrvEventStatus            */
#define BRCM_SWARCH_HSREF_DRV_CLEAR_INTERRUPT_PROC       (0x85B3U)    /**< @brief #HSREF_DrvClearInterrupt         */

/** @} */

/**
    @name HSREF_HwIDType
    @{
    @brief Index of the HSREF module
    @trace #BRCM_SWREQ_HSREF_INIT
 */
typedef uint32_t HSREF_HwIDType;  /**< @brief HSREF HW Id type */
#define HSREF_HW_ID_0      (0UL)  /**< @brief HSREF HW Id 0    */
/** @} */

/**
    @name HSREF_StateType
    @{
    @brief State of HSREF
    @trace #BRCM_SWREQ_HSREF_INIT
 */
typedef uint8_t HSREF_StateType;         /**< @brief HSREF state type */
#define HSREF_STATE_UNINIT         0U    /**< @brief HSREF is uninitialized and value must be zero*/
#define HSREF_STATE_INITIALIZED    1U    /**< @brief HSREF is initialized */
/** @} */

/**
    @name HSREF_SvfConfigMaskType
    @{
    @brief State variable filter configuration mask type

    @trace #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER
 */
typedef uint32_t HSREF_SvfConfigMaskType;             /**< @brief HSREF SVF configuration mask type */
#define HSREF_SVF_CONFIG_MASK_SEL    (0x1UL << 0UL )  /**< @brief HSREF SVF select mask             */
#define HSREF_SVF_CONFIG_MASK_QVAR   (0x1UL << 1UL )  /**< @brief HSREF SVF QVAR configuration mask */
#define HSREF_SVF_CONFIG_MASK_FVAR   (0x1UL << 2UL )  /**< @brief HSREF SVF FVAR configuration mask */
#define HSREF_SVF_CONFIG_MASK_ALL    (0x7UL)          /**< @brief HSREF SVF configuration all mask  */
/** @} */

/**
    @name HSREF_ZcrossConfigMaskType
    @{
    @brief Zero cross configuration mask type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef uint32_t HSREF_ZcrossConfigMaskType;                        /**< @brief HSREF zero cross configuration mask type */
#define HSREF_ZCROSS_CONFIG_MASK_PRE_CAP_DURATION  (0x1UL << 0UL )  /**< @brief HSREF zero cross pre cap duration mask   */
#define HSREF_ZCROSS_CONFIG_MASK_CAP_DURATION      (0x1UL << 1UL )  /**< @brief HSREF zero cross cap duration mask       */
#define HSREF_ZCROSS_CONFIG_MASK_HYS_RANGE         (0x1UL << 2UL )  /**< @brief HSREF zero cross hysteresis range mask   */
#define HSREF_ZCROSS_CONFIG_MASK_HYS_ENABLE        (0x1UL << 3UL )  /**< @brief HSREF zero cross hysteresis enable mask  */
#define HSREF_ZCROSS_CONFIG_MASK_ALL               (0xFUL)          /**< @brief HSREF zero cross configuration all mask  */
/** @} */

/**
    @name HSREF_ZcrossFifoControlMaskType
    @{
    @brief Zero cross FIFO control mask type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef uint32_t HSREF_ZcrossFifoControlMaskType;                 /**< @brief HSREF zero cross FIFO control mask type      */
#define HSREF_ZCROSS_FIFO_CONTROL_MASK_READ       (0x1UL << 0UL ) /**< @brief HSREF zero cross FIFO control read mask      */
#define HSREF_ZCROSS_FIFO_CONTROL_MASK_RESET      (0x1UL << 1UL ) /**< @brief HSREF zero cross FIFO control reset mask     */
#define HSREF_ZCROSS_FIFO_CONTROL_MASK_THRESHOLD  (0x1UL << 2UL ) /**< @brief HSREF zero cross FIFO control threshold mask */
#define HSREF_ZCROSS_FIFO_CONTROL_MASK_ALL        (0x7UL)         /**< @brief HSREF zero cross FIFO control all mask       */
/** @} */

/**
    @name HSREF_ZcrossFifoStsType
    @{
    @brief Zero cross FIFO status type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef uint32_t HSREF_ZcrossFifoStsType;                         /**< @brief HSREF zero cross FIFO status type          */
#define HSREF_ZCROSS_FIFO_STS_EMPTY_MASK    (0x80000000UL)        /**< @brief Mask to extract Zcross FIFO empty bit      */
#define HSREF_ZCROSS_FIFO_STS_EMPTY_SHIFT   (31UL)                /**< @brief Shift to extract Zcross FIFO empty bit     */
#define HSREF_ZCROSS_FIFO_STS_WADDR_MASK    (0x60UL)              /**< @brief Mask to extract Zcross FIFO write address  */
#define HSREF_ZCROSS_FIFO_STS_WADDR_SHIFT   (5UL)                 /**< @brief shift to extract Zcross FIFO write address */
#define HSREF_ZCROSS_FIFO_STS_RADDR_MASK    (0x18UL)              /**< @brief Mask to extract Zcross FIFO read address   */
#define HSREF_ZCROSS_FIFO_STS_RADDR_SHIFT   (3UL)                 /**< @brief shift to extract Zcross FIFO read address  */
#define HSREF_ZCROSS_FIFO_STS_CNT_MASK      (0x7UL)               /**< @brief Mask to extract Zcross FIFO count          */
#define HSREF_ZCROSS_FIFO_STS_CNT_SHIFT     (0UL)                 /**< @brief shift to extract Zcross FIFO count         */
/** @} */

/**
    @name HSREF_CapBufConfigMaskType
    @{
    @brief HSREF capture buffer configuration mask type

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef uint32_t HSREF_CapBufConfigMaskType;                     /**< @brief HSREF capture buffer configuration mask type             */
#define HSREF_CAP_BUF_CONFIG_MASK_SAMPLE_CNT     (0x1UL << 0UL ) /**< @brief HSREF capture buffer SVF sample count configuration mask */
#define HSREF_CAP_BUF_CONFIG_MASK_SVF_POST_DIV   (0x1UL << 1UL ) /**< @brief HSREF capture buffer SVF post divider configuration mask */
#define HSREF_CAP_BUF_CONFIG_MASK_ALL            (0x3UL)         /**< @brief HSREF capture buffer configuration all mask              */
/** @} */

/**
    @name HSREF_CapBufFifoControlMaskType
    @{
    @brief HSREF capture buffer FIFO control mask type

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef uint32_t HSREF_CapBufFifoControlMaskType;                  /**< @brief HSREF capture buffer FIFO control mask type      */
#define HSREF_CAP_BUF_FIFO_CONTROL_MASK_RESET      (0x1UL << 0UL ) /**< @brief HSREF capture buffer FIFO control reset mask     */
#define HSREF_CAP_BUF_FIFO_CONTROL_MASK_THRESHOLD  (0x1UL << 1UL ) /**< @brief HSREF capture buffer FIFO control threshold mask */
#define HSREF_CAP_BUF_FIFO_CONTROL_MASK_ALL        (0x3UL)         /**< @brief HSREF capture buffer FIFO control all mask       */
/** @} */

/**
    @name HSREF_SvfSelType
    @{
    @brief State variable filter select type

    @trace #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER
 */
typedef uint8_t HSREF_SvfSelType;  /**< @brief State variable filter select type */
#define HSREF_SVF_SEL_BP     (0U)  /**< @brief BP Filter */
#define HSREF_SVF_SEL_LP     (1U)  /**< @brief LP Filter */
/** @} */

/**
    @name HSREF_CapBufLaneType
    @{
    @brief State variable filter select type

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef uint8_t HSREF_CapBufLaneType;  /**< @brief Capture buffer lane type */
#define HSREF_CAP_BUF_LANE_0     (0U)  /**< @brief Capture buffer lane 0 */
#define HSREF_CAP_BUF_LANE_1     (1U)  /**< @brief Capture buffer lane 1 */
/** @} */

/**
    @name HSREF_InterruptType
    @{
    @brief HSREF Interrupt Type

    @trace #BRCM_SWREQ_HSREF_INTERRUPT
 */
typedef uint32_t HSREF_InterruptType;                      /**< @brief Select HSREF interrupt type.                             */
#define HSREF_INTERRUPT_ADC                    (0x3UL)     /**< @brief Group of ADC interrupts.                                 */
#define HSREF_INTERRUPT_ADC_CALIB_DONE         (1UL<< 0UL) /**< @brief ADC calibration complete interrupt.                      */
#define HSREF_INTERRUPT_ADC_CALIB_ERROR        (1UL<< 1UL) /**< @brief ADC calibration error encountered during FG calibration. */
#define HSREF_INTERRUPT_CAP_BUF                (0xFCUL)    /**< @brief Group of capture buffer interrupts.                      */
#define HSREF_INTERRUPT_CAP_BUF_MEM_INIT_DONE  (1UL<< 2UL) /**< @brief Capture buffer memory initialization done interrupt.     */
#define HSREF_INTERRUPT_CAP_BUF_MEM_INIT_ERROR (1UL<< 3UL) /**< @brief Capture buffer memory initialization error interrupt.    */
#define HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN0   (1UL<< 4UL) /**< @brief Capture buffer lane0 FIFO overflow interrupt.            */
#define HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN0   (1UL<< 5UL) /**< @brief Capture buffer lane0 FIFO threshold interrupt.           */
#define HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN1   (1UL<< 6UL) /**< @brief Capture buffer lane1 FIFO overflow interrupt.            */
#define HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN1   (1UL<< 7UL) /**< @brief Capture buffer lane1 FIFO threshold interrupt.           */
#define HSREF_INTERRUPT_ZCROSS_FIFO            (0x300UL)   /**< @brief Group of Zero cross interrupts.                          */
#define HSREF_INTERRUPT_ZCROSS_FIFO_OVF        (1UL<< 8UL) /**< @brief Zero cross FIFO overflow interrupt.                      */
#define HSREF_INTERRUPT_ZCROSS_FIFO_TSH        (1UL<< 9UL) /**< @brief Zero cross FIFO threshold interrupt.                     */
#define HSREF_INTERRUPT_ALL                    (0x3FFUL)   /**< @brief HSREF ALL interrupt.                                     */
/** @} */

/**
    @name HSREF_CalibConfigType
    @{
    @brief HSREF calibration configuration

    @trace #BRCM_SWREQ_HSREF_CALIBRATION
 */
typedef struct sHSREF_CalibConfigType {
    uint8_t calStatesEn;            /**< @brief Bit0: Enables FG offset, Bit1: FG daccal Bit2: NA Bit3: bg_offset Bit4: NA */
    uint32_t calBgRdbDuration;      /**< @brief Time for which the BG calibration is run.  Time = 204.8ns*programmed value */
} HSREF_CalibConfigType;
/** @} */

/**
    @name HSREF_CalibStatusType
    @{
    @brief HSREF calibration status

    @trace #BRCM_SWREQ_HSREF_CALIBRATION
 */
typedef struct sHSREF_CalibStatusType {
  uint8_t calStatus;       /**< @brief Calibration status 1 : Calibration in progress 0: Calibration Done */
} HSREF_CalibStatusType;
/** @} */

/**
    @name HSREF_SvfConfigType
    @{
    @brief HSREF state variable filter configuration

    @trace #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER
 */
typedef struct sHSREF_SvfConfigType {
    HSREF_SvfConfigMaskType mask; /**< @brief SVF configuration mask type     */
    uint32_t svfQvar;             /**< @brief Inverse of Q factor             */
    uint32_t svfFvar;             /**< @brief Frequency variable              */
    HSREF_SvfSelType svfSel ;     /**< @brief State variable filter selection */

} HSREF_SvfConfigType;
/** @} */

/**
    @name HSREF_SvfControlType
    @{
    @brief HSREF state variable filter control

    @trace #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER
 */
typedef struct sHSREF_SvfControlType {
    uint8_t svfEnable; /**< @brief 1 will enable the SVF in front of the zero crossing circuit. 0 will disable SVF. */
} HSREF_SvfControlType;
/** @} */

/**
    @name HSREF_ZcrossConfigType
    @{
    @brief HSREF zero cross circuit configuration

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef struct sHSREF_ZcrossConfigType {
    HSREF_ZcrossConfigMaskType mask ; /**< @brief Zero cross configuration mask type */
    uint32_t zCrossPreMskCapDuration; /**< @brief This is the number of samples from the cap start that are ignored for computing the zero crossing when SVF is enabled. */
    uint32_t zCrossCapDuration;       /**< @brief The number of samples for which the zero cross circuit operates. */
    uint32_t zCrossHysRange;          /**< @brief This is the number of samples of hysteresis that is added to the zero crossing circuit.*/
    uint8_t  zCrossHysEnable;         /**< @brief Enable hysteresis for the zero crossing circuit. */
} HSREF_ZcrossConfigType;
/** @} */

/**
    @name HSREF_ZcrossControlType
    @{
    @brief HSREF zero cross control type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef struct sHSREF_ZcrossControlType {
    uint8_t zCrossEnable; /**< @brief 1 will enable the the zero crossing circuit. 0 will disable zero cross circuit. */
} HSREF_ZcrossControlType;
/** @} */

/**
    @name HSREF_ZcrossFifoControlType
    @{
    @brief HSREF zero cross FIFO control type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef struct sHSREF_ZcrossFifoControlType {
    HSREF_ZcrossFifoControlMaskType mask; /**< @brief Zero cross FIFO control mask type */
    uint8_t rdStrobe ;                    /**< @brief Issues the FIFO read strobe and then read the data from the zero cross FIFO registers.     */
    uint8_t reset ;                       /**< @brief Zero cross FIFO software reset control. Its a self clear signal.                           */
    uint32_t threshold ;                   /**< @brief Zero cross circuit has a 4 deep fifo. This sets the threshold for generation of interrupt. */
} HSREF_ZcrossFifoControlType;
/** @} */

/**
    @name HSREF_ZcrossFifoStatusType
    @{
    @brief HSREF zero cross FIFO status type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef struct sHSREF_ZcrossFifoStatusType {
    HSREF_ZcrossFifoStsType status ; /**< @brief Zero cross FIFO status */
} HSREF_ZcrossFifoStatusType;
/** @} */

/**
    @name HSREF_ZcrossFifoReadType
    @{
    @brief HSREF zero cross FIFO read type

    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
 */
typedef struct sHSREF_ZcrossFifoReadType {
    uint32_t data[8U] ; /**< @brief Zero cross FIFO read data */
} HSREF_ZcrossFifoReadType;
/** @} */

/**
    @name HSREF_CapBufConfigType
    @{
    @brief HSREF capture buffer configuration

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef struct sHSREF_CapBufConfigType {
    HSREF_CapBufConfigMaskType mask; /**< @brief HSREF capture buffer configuration mask type. */
    uint32_t capSampleCnt;           /**< @brief This tells how many samples are to be captures on each lane of reference channel. */
    uint32_t  svfPostDiv;             /**< @brief This is the number of samples of hysteresis that is added to the zero crossing circuit.*/
} HSREF_CapBufConfigType;
/** @} */

/**
    @name HSREF_CapBufControlType
    @{
    @brief HSREF capture buffer control

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef struct sHSREF_CapBufControlType {
    uint32_t capBufEnable ; /**< @brief This will enable the capture of the reference channel data into the FIFO's that DMA has to read through NOC.*/
} HSREF_CapBufControlType;
/** @} */

/**
    @name HSREF_CapBufFifoControlType
    @{
    @brief HSREF capture buffer FIFO control type

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef struct sHSREF_CapBufFifoControlType {
    HSREF_CapBufFifoControlMaskType mask;/**< @brief HSREF capture buffer FIFO control mask type. */
    uint8_t reset ;                      /**< @brief This will reset the Capture buffer FIFO.     */
    uint32_t threshold ;                  /**< @brief This defines the threshold upon which the interrupt to the DMA is generated.*/
} HSREF_CapBufFifoControlType;
/** @} */

/**
    @name HSREF_CapBufFifoStatusType
    @{
    @brief HSREF capture buffer FIFO status type

    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
 */
typedef struct sHSREF_CapBufFifoStatusType {
    uint32_t status;      /**< @brief Capture buffer FIFO status */
} HSREF_CapBufFifoStatusType;
/** @} */

/** @brief Initializes HSREF

    This API initializes the given HSREF hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId         Index of the HSREF controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSREF_INIT

    @limitations None
*/

int32_t HSREF_DrvInit(HSREF_HwIDType  hwId);

/** @brief De-initializes HSREF

    This API de-initializes HSREF. Resets the given HSREF controller and puts back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId         Index of the HSREF controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSREF_INIT

    @limitations None
*/

int32_t HSREF_DrvDeInit(HSREF_HwIDType  hwId);

/** @brief HSREF Calibration configuration

    This API configures HSREF hardware background or foreground calibration states.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId           Index of the HSREF controller
    @param[in]  calibConfig    HSREF calibration configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL calibration configuration
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_CALIBRATION

    @limitations None
*/
int32_t HSREF_DrvInitCalib(HSREF_HwIDType  hwId,  HSREF_CalibConfigType *calibConfig);

/** @brief Triggers HSREF Calibration

    This API triggers the start of calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId           Index of the HSREF controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post  None

    @trace  #BRCM_SWREQ_HSREF_CALIBRATION

    @limitations None
*/
int32_t HSREF_DrvTriggerCalib(HSREF_HwIDType  hwId);

/** @brief HSREF Calibration Status

    This API gets HSREF calibration status. Calibration status in not valid if calibration is not enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId           Index of the HSREF controller
    @param[out] calibStatus    Pointer to HSREF calibration status
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL calibration status
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_CALIBRATION

    @limitations None
*/
int32_t HSREF_DrvCalibStatus(HSREF_HwIDType hwId, HSREF_CalibStatusType *calibStatus);

/** @brief HSREF state variable filter configuration

    This API configures HSREF state variable filter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  svfConfig     Pointer to HSREF SVF configuration parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL SVF configuration
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER

    @limitations None
*/
int32_t HSREF_DrvCfgSvf(HSREF_HwIDType  hwId, HSREF_SvfConfigType *svfConfig);

/** @brief HSREF state variable filter control

    This API controls enable/disable of HSREF state variable filter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  svfControl    Pointer to HSREF SVF control parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL SVF control
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER

    @limitations None
*/
int32_t HSREF_DrvCtrlSvf(HSREF_HwIDType  hwId, HSREF_SvfControlType *svfControl);

/** @brief HSREF zero cross configuration

    This API configures HSREF zero cross circuit.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  zCrossConfig  Pointer to HSREF zero cross configuration parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL Zero cross configuration
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_ZERO_CROSS

    @limitations None
*/
int32_t HSREF_DrvCfgZcross(HSREF_HwIDType  hwId, HSREF_ZcrossConfigType *zCrossConfig);

/** @brief HSREF zero cross circuit control

    This API controls enable/disable of HSREF zero cross circuit.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  zCrossControl Pointer to HSREF zero cross control parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL zero cross control
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_ZERO_CROSS

    @limitations None
*/
int32_t HSREF_DrvCtrlZcross(HSREF_HwIDType  hwId, HSREF_ZcrossControlType *zCrossControl);

/** @brief HSREF zero cross circuit FIFO control

    This API controls HSREF zero cross circuit FIFO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  zCrossControl Pointer to HSREF zero cross FIFO control parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL zero cross FIFO control
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized
    @post None

    @trace  #BRCM_SWREQ_HSREF_ZERO_CROSS

    @limitations None
*/
int32_t HSREF_DrvFifoCtrlZcross(HSREF_HwIDType  hwId, HSREF_ZcrossFifoControlType *zCrossFifoControl);

/** @brief HSREF zero cross FIFO status

    This API gets HSREF zero cross FIFO status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId             Index of the HSREF controller
    @param[in]  zCrossFifoStatus Pointer to HSREF zero cross FIFO status
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL zero cross FIFO status
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_ZERO_CROSS

    @limitations None
*/
int32_t HSREF_DrvFifoStsZcross(HSREF_HwIDType  hwId, HSREF_ZcrossFifoStatusType *zCrossFifoStatus);

/** @brief HSREF zero cross FIFO read

    This API reads HSREF zero cross FIFO

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId           Index of the HSREF controller
    @param[in]  zCrossFifoRead Pointer to HSREF zero cross FIFO read content
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL zero cross FIFO read
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_ZERO_CROSS

    @limitations None
*/
int32_t HSREF_DrvFifoReadZcross(HSREF_HwIDType  hwId, HSREF_ZcrossFifoReadType *zCrossFifoRead);

/** @brief HSREF capture buffer configuration

    This API configures HSREF capture buffer configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  zCrossConfig  Pointer to HSREF capture buffer configuration parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL capture buffer configuration
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_CAPTURE_BUFFER

    @limitations None
*/
int32_t HSREF_DrvCfgCapBuf(HSREF_HwIDType  hwId, HSREF_CapBufConfigType *capBufConfig);

/** @brief HSREF capture buffer control

    This API controls enable/disable of HSREF state variable filter.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the HSREF controller
    @param[in]  capBufControl Pointer to HSREF capture buffer control parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL capture buffer control
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_CAPTURE_BUFFER

    @limitations None
*/
int32_t HSREF_DrvCtrlCapBuf(HSREF_HwIDType  hwId, HSREF_CapBufControlType *capBufControl);

/** @brief HSREF capture buffer FIFO control

    This API controls HSREF capture buffer FIFO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId              Index of the HSREF controller
    @param[in]  capBufFifoControl Pointer to HSREF capture buffer FIFO control parameters
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL capture buffer FIFO control
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_CAPTURE_BUFFER

    @limitations None
*/
int32_t HSREF_DrvFifoCtrlCapBuf(HSREF_HwIDType  hwId, HSREF_CapBufFifoControlType *capBufFifoControl);

/** @brief HSREF capture buffer FIFO status

    This API gets HSREF capture buffer FIFO status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId             Index of the HSREF controller
    @param[in]   capBufLane       Capture buffer lane number
    @param[out]  capBufFifoStatus Pointer to HSREF capture buffer FIFO status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL capture buffer FIFO status
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_CAPTURE_BUFFER

    @limitations None
*/
int32_t HSREF_DrvFifoStsCapBuf(HSREF_HwIDType  hwId, HSREF_CapBufLaneType capBufLane, HSREF_CapBufFifoStatusType *capBufFifoStatus);

/** @brief HSREF enable interrupts

    This API enable interrupts based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the HSREF controller
    @param[in]   intrType   Specified interrupt type
    @param[out]  aEnable    Enable/disable control
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_INTERRUPT

    @limitations None
*/
int32_t HSREF_DrvEnableInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType, uint32_t aEnable);

/** @brief HSREF interrupt status

    This API gets interrupt status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the HSREF controller
    @param[in]   intrSts   Pointer to interrupt status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL interrupt status
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_INTERRUPT

    @limitations None
*/
int32_t HSREF_DrvInterruptStatus(HSREF_HwIDType hwId, HSREF_InterruptType *intrSts);

/** @brief HSREF event status

    This API gets event status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the HSREF controller
    @param[in]   intrSts   Pointer to event status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL event status
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_INTERRUPT

    @limitations None
*/
int32_t HSREF_DrvEventStatus(HSREF_HwIDType hwId, HSREF_InterruptType *eventSts);

/** @brief HSREF clear interrupts

    This API clears interrupt based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the HSREF controller
    @param[in]   intrType   Interrupt type to clear the interrupt
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          HSREF instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSREF_INTERRUPT

    @limitations None
*/
int32_t HSREF_DrvClearInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType);

#endif /* HSREF_H */

/** @} */
