/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_acqcmndrv_ifc ACQCMN
    @ingroup grp_acqcmndrv

    @addtogroup grp_acqcmndrv_ifc
    @{
    @section sec_acqcmn_overview Overview
    -# Acquisition Common Bare Metal driver interface provides layer 1 APIs to initialize and configure
    the ACQCMN in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each ACQCMN HW instance needs to be initialized separately using ACQCMN_DrvInit API.
    -# It is user's responsibility to configure DMA separately to handle data transfers.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_acqcmndrv_fsm Usage
    -# ACQCMN HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |  ACQ_COMMON_0 |

    @section api_list List of ACQCMN APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref ACQCMN_DrvInit              | ACQCMN Initialize                              |
    | @ref ACQCMN_DrvDeInit            | ACQCMN De-initialize                           |
    | @ref ACQCMN_DrvReset             | ACQCMN Reset                                   |
    | @ref ACQCMN_DrvClkCtrl           | ACQCMN Clock control                           |
    | @ref ACQCMN_DrvAxisResSet        | ACQCMN set the resolution from nsu_bin_cntr    |
    | @ref ACQCMN_DrvAxisCfgSet        | ACQCMN Set Axis configuration                  |
    | @ref ACQCMN_DrvAxisCfgGet        | ACQCMN Get Axis configuration                  |
    | @ref ACQCMN_DrvSrcControl        | ACQCMN source control configuration            |
    | @ref ACQCMN_DrvEnableInterrupt   | ACQCMN Enable interrupts                       |
    | @ref ACQCMN_DrvInterruptStatus   | ACQCMN Get interrupt status                    |
    | @ref ACQCMN_DrvEventStatus       | ACQCMN Get event status                        |
    | @ref ACQCMN_DrvClearInterrupt    | ACQCMN Clear interrupt                         |
    @file acqcmn.h
    @brief Interface for ACQCMN driver
    @section apis This section defines the interface for ACQCMN driver
    @version 0.1 Initial Version
*/

#ifndef ACQCMN_H
#define ACQCMN_H

#include <stdint.h>

/**
    @name ACQCMN driver API IDs
    @{
    @brief API IDs for ACQCMN driver
 */
#define BRCM_SWARCH_ACQCMN_HW_ID_TYPE                     (0x8640U)    /**< @brief #ACQCMN_HwIDType               */
#define BRCM_SWARCH_ACQCMN_STATE_TYPE                     (0x8641U)    /**< @brief #ACQCMN_StateType              */
#define BRCM_SWARCH_ACQCMN_RESET_TYPE                     (0x8642U)    /**< @brief #ACQCMN_ResetType              */
#define BRCM_SWARCH_ACQCMN_CLK_TYPE                       (0x8643U)    /**< @brief #ACQCMN_ClkType                */
#define BRCM_SWARCH_ACQCMN_AXIS_RESOLUTION_TYPE           (0x8644U)    /**< @brief #ACQCMN_AxisResolutionType     */
#define BRCM_SWARCH_ACQCMN_AXIS_ID_TYPE                   (0x8645U)    /**< @brief #ACQCMN_AxisIdType             */
#define BRCM_SWARCH_ACQCMN_AWG_MUX_TYPE                   (0x8646U)    /**< @brief #ACQCMN_AwgMuxType             */
#define BRCM_SWARCH_ACQCMN_EDGE_SEL_TYPE                  (0x8647U)    /**< @brief #ACQCMN_EdgeSelType            */
#define BRCM_SWARCH_ACQCMN_SENSOR_SEL_TYPE                (0x8648U)    /**< @brief #ACQCMN_SensorSelType          */
#define BRCM_SWARCH_ACQCMN_INTERRUPT_TYPE                 (0x864BU)    /**< @brief #ACQCMN_InterruptType          */
#define BRCM_SWARCH_ACQCMN_AXIS_CFG_TYPE                  (0x864CU)    /**< @brief #ACQCMN_AxisCfgType            */

#define BRCM_SWARCH_ACQCMN_DRV_INIT_PROC                  (0x8650U)    /**< @brief #ACQCMN_DrvInit                */
#define BRCM_SWARCH_ACQCMN_DRV_DEINIT_PROC                (0x8651U)    /**< @brief #ACQCMN_DrvDeInit              */
#define BRCM_SWARCH_ACQCMN_DRV_RESET_PROC                 (0x8652U)    /**< @brief #ACQCMN_DrvReset               */
#define BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC              (0x8653U)    /**< @brief #ACQCMN_DrvClkCtrl             */
#define BRCM_SWARCH_ACQCMN_DRV_AXIS_RES_SET_PROC          (0x8654U)    /**< @brief #ACQCMN_DrvAxisResSet          */
#define BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_SET_PROC          (0x8655U)    /**< @brief #ACQCMN_DrvAxisCfgSet          */
#define BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_GET_PROC          (0x8656U)    /**< @brief #ACQCMN_DrvAxisCfgGet          */
#define BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC      (0x8657U)    /**< @brief #ACQCMN_DrvEnableInterrupt     */
#define BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC      (0x8658U)    /**< @brief #ACQCMN_DrvInterruptStatus     */
#define BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC          (0x8659U)    /**< @brief #ACQCMN_DrvEventStatus         */
#define BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC       (0x865AU)    /**< @brief #ACQCMN_DrvClearInterrupt      */
#define BRCM_SWARCH_ACQCMN_DRV_SRC_CONTROL_PROC           (0x865BU)    /**< @brief #ACQCMN_DrvSrcControl          */

#define BRCM_SWARCH_ACQCMN_SRC_SEL_BLK_TYPE               (0x865CU)    /**< @brief #ACQCMN_SrcSelBlkType          */
#define BRCM_SWARCH_ACQCMN_SRC_SEL_EVENT_TYPE             (0x865DU)    /**< @brief #ACQCMN_SrcSelEventType        */
#define BRCM_SWARCH_ACQCMN_SRC_SEL_TYPE                   (0x865EU)    /**< @brief #ACQCMN_SrcSelType             */
#define BRCM_SWARCH_ACQCMN_SRC_CTRL_TYPE                  (0x865FU)    /**< @brief #ACQCMN_SrcCtrlType            */

/** @} */

/**
    @name ACQCMN_HwIDType
    @{
    @brief Index of the ACQCMN module
    @trace #BRCM_SWREQ_ACQCMN_INIT
 */
typedef uint32_t ACQCMN_HwIDType;
/** @} */

/**
    @name ACQCMN_StateType
    @{
    @brief State of ACQCMN
    @trace #BRCM_SWREQ_ACQCMN_INIT
 */
typedef uint8_t ACQCMN_StateType;         /**< @brief ACQCMN state type */
#define ACQCMN_STATE_UNINIT         0U    /**< @brief ACQCMN is uninitialized and value must be zero*/
#define ACQCMN_STATE_INITIALIZED    1U    /**< @brief ACQCMN is initialized */
/** @} */

/**
    @name ACQCMN_ResetType
    @{
    @brief ACQCMN Reset Type

    @trace #BRCM_SWREQ_ACQCMN_RESET_CONTROL
 */
typedef uint32_t ACQCMN_ResetType;             /**< @brief Select ACQCMN reset type */
#define ACQCMN_RESET_HSAFE_RDB     (1UL<< 0UL) /**< @brief ACQCMN hsafe rdb         */
#define ACQCMN_RESET_HSADC_TOP     (1UL<< 1UL) /**< @brief ACQCMN hsadc top         */
#define ACQCMN_RESET_HSADC_RDB     (1UL<< 2UL) /**< @brief ACQCMN hsadc rdb         */
#define ACQCMN_RESET_CMN_TRIG      (1UL<< 3UL) /**< @brief ACQCMN cmn_trigger       */
#define ACQCMN_RESET_HSADC_FFT     (1UL<< 4UL) /**< @brief ACQCMN hsadc fft         */
#define ACQCMN_RESET_HSADC_OCP     (1UL<< 5UL) /**< @brief ACQCMN hsadc ocp         */
#define ACQCMN_RESET_HSREF_TOP     (1UL<< 6UL) /**< @brief ACQCMN hsref top         */
#define ACQCMN_RESET_HSREF_RDB     (1UL<< 7UL) /**< @brief ACQCMN hsref rdb         */
#define ACQCMN_RESET_HSREF_OCP     (1UL<< 8UL) /**< @brief ACQCMN hsref ocp         */
#define ACQCMN_RESET_HKDAC_TOP     (1UL<< 9UL) /**< @brief ACQCMN hkdac top         */
#define ACQCMN_RESET_HKDAC_RDB     (1UL<<10UL) /**< @brief ACQCMN hkdac rdb         */
#define ACQCMN_RESET_HKADC_TOP     (1UL<<11UL) /**< @brief ACQCMN hkadc top         */
#define ACQCMN_RESET_HKADC_RDB     (1UL<<12UL) /**< @brief ACQCMN hkadc rdb         */
#define ACQCMN_RESET_TXSLZR_TOP    (1UL<<13UL) /**< @brief ACQCMN txslzr top        */
#define ACQCMN_RESET_TXSLZR_RDB    (1UL<<14UL) /**< @brief ACQCMN txslzr rdb        */
#define ACQCMN_RESET_AWG_TOP       (1UL<<15UL) /**< @brief ACQCMN awg top           */
#define ACQCMN_RESET_AWG_RDB       (1UL<<16UL) /**< @brief ACQCMN awg rdb           */
#define ACQCMN_RESET_CAPCOM_TOP    (1UL<<17UL) /**< @brief ACQCMN capcom top        */
#define ACQCMN_RESET_CAPCOM_RDB    (1UL<<18UL) /**< @brief ACQCMN capcom rdb        */
#define ACQCMN_RESET_AXIS_CTRL     (1UL<<19UL) /**< @brief ACQCMN axis_ctrl         */
#define ACQCMN_RESET_NSU_TOP       (1UL<<20UL) /**< @brief ACQCMN nsu top           */
#define ACQCMN_RESET_NSU_RDB       (1UL<<21UL) /**< @brief ACQCMN nsu rdb           */

/** @} */

/**
    @name ACQCMN_ClkType
    @{
    @brief ACQCMN clock Type

    @trace #BRCM_SWREQ_ACQCMN_CLOCK_CONTROL
 */
typedef uint32_t ACQCMN_ClkType;             /**< @brief ACQCMN clock type        */
#define ACQCMN_CLK_ADC_2X_HSADC  (1UL<< 0UL) /**< @brief ACQCMN clk_adc_2x_hsadc  */
#define ACQCMN_CLK_ADC_HSADC     (1UL<< 2UL) /**< @brief ACQCMN clk_adc_hsadc     */
#define ACQCMN_CLK_WND_HSADC     (1UL<< 3UL) /**< @brief ACQCMN clk_wnd_hsadc     */
#define ACQCMN_CLK_FFT_HSADC     (1UL<< 4UL) /**< @brief ACQCMN clk_fft_hsadc     */
#define ACQCMN_CLK_OCP_HSADC     (1UL<< 5UL) /**< @brief ACQCMN clk_ocp_hsadc     */
#define ACQCMN_CLK_ADC_HSREF     (1UL<< 6UL) /**< @brief ACQCMN clk_adc_hsref     */
#define ACQCMN_CLK_OCP_HSREF     (1UL<< 7UL) /**< @brief ACQCMN clk_ocp_hsref     */
#define ACQCMN_CLK_100_HKDAC     (1UL<< 8UL) /**< @brief ACQCMN clk_100_hkdac     */
#define ACQCMN_CLK_100_HKADC     (1UL<< 9UL) /**< @brief ACQCMN clk_100_hkadc     */
#define ACQCMN_CLK_ADC_TXSLZR    (1UL<<10UL) /**< @brief ACQCMN clk_adc_txslzr    */
#define ACQCMN_CLK_100_AWG       (1UL<<11UL) /**< @brief ACQCMN clk_100_awg       */
#define ACQCMN_CLK_100_CAPCOM    (1UL<<12UL) /**< @brief ACQCMN clk_100_capcom    */
#define ACQCMN_CLK_ALL           (0x1FFDUL)  /**< @brief ACQCMN all clock type    */
/** @} */

/**
    @name ACQCMN_AxisResolutionType
    @{
    @brief ACQCMN Axis resolution Type

    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
 */
typedef uint32_t ACQCMN_AxisResolutionType;  /**< @brief ACQCMN Axis Resolution Type  */
#define ACQCMN_AXIS_RESOLUTION_640NS  0UL    /**< @brief ACQCMN Axis Resolution 640ns */
#define ACQCMN_AXIS_RESOLUTION_40NS   1UL    /**< @brief ACQCMN Axis Resolution 40ns  */
/** @} */

/**
    @name ACQCMN_AxisIdType
    @{
    @brief ACQCMN Axis Id Type

    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
 */
typedef uint32_t ACQCMN_AxisIdType;  /**< @brief Select ACQCMN Axis Id Type */
#define ACQCMN_AXIS_ID_0  0UL        /**< @brief ACQCMN Axis ID 0           */
#define ACQCMN_AXIS_ID_1  1UL        /**< @brief ACQCMN Axis ID 1           */
/** @} */

/**
    @name ACQCMN_AwgMuxType
    @{
    @brief ACQCMN AWG Mux Type

    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
 */
typedef uint32_t ACQCMN_AwgMuxType;  /**< @brief Select the AWG_DIN/LOAD for the axis_ctrl */
#define ACQCMN_AWG_MUX_0  0UL        /**< @brief AWG0 */
#define ACQCMN_AWG_MUX_1  1UL        /**< @brief AWG1 */
#define ACQCMN_AWG_MUX_2  2UL        /**< @brief AWG2 */
#define ACQCMN_AWG_MUX_3  3UL        /**< @brief AWG3 */
/** @} */

/**
    @name ACQCMN_EdgeSelType
    @{
    @brief ACQCMN Edge selection Type

    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
 */
typedef uint32_t ACQCMN_EdgeSelType; /**< @brief Edge selection to increment the counter for the Pulse Angle position detection */
#define ACQCMN_EDGE_SEL_RISING   0UL /**< @brief Rising  edge */
#define ACQCMN_EDGE_SEL_FALLING  1UL /**< @brief falling edge */
#define ACQCMN_EDGE_SEL_BOTH     2UL /**< @brief Both edge    */
#define ACQCMN_EDGE_SEL_DISABLED 3UL /**< @brief disabled     */
/** @} */

/**
    @name ACQCMN_SensorSelType
    @{
    @brief ACQCMN sensor selection Type

    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
 */
typedef uint32_t ACQCMN_SensorSelType;     /**< @brief Position information detector selection */
#define ACQCMN_SENSOR_SEL_TIME_BASED   0UL /**< @brief Time based         */
#define ACQCMN_SENSOR_SEL_QUAD_ENC     1UL /**< @brief Quadrature encoder */
#define ACQCMN_SENSOR_SEL_PULSE_ANGLE  2UL /**< @brief Pulse angle        */
#define ACQCMN_SENSOR_SEL_HKADC        3UL /**< @brief HKADC              */
#define ACQCMN_SENSOR_SEL_AWG          4UL /**< @brief AWG                */
#define ACQCMN_SENSOR_SEL_CAPCOM       5UL /**< @brief CAPCOM             */
#define ACQCMN_SENSOR_SEL_DISABLED     6UL /**< @brief Disabled           */
/** @} */

/**
    @name ACQCMN_InterruptType
    @{
    @brief ACQCMN Interrupt Type

    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
 */
typedef uint32_t ACQCMN_InterruptType;                      /**< @brief Select ACQCMN interrupt type.                */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_GRP0    (0xFUL )     /**< @brief ACQCMN NSU generate event group 0            */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH0     (1UL<< 0UL ) /**< @brief ACQCMN NSU generate event channel 0          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH1     (1UL<< 1UL ) /**< @brief ACQCMN NSU generate event channel 1          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH2     (1UL<< 2UL ) /**< @brief ACQCMN NSU generate event channel 2          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH3     (1UL<< 3UL ) /**< @brief ACQCMN NSU generate event channel 3          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_GRP1    (0xF0UL )    /**< @brief ACQCMN NSU generate event group 1            */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH4     (1UL<< 4UL ) /**< @brief ACQCMN NSU generate event channel 4          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH5     (1UL<< 5UL ) /**< @brief ACQCMN NSU generate event channel 5          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH6     (1UL<< 6UL ) /**< @brief ACQCMN NSU generate event channel 6          */
#define ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH7     (1UL<< 7UL ) /**< @brief ACQCMN NSU generate event channel 7          */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_GRP0 (0xF00UL )   /**< @brief ACQCMN NSU timestamp capture event group 0   */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH0  (1UL<< 8UL ) /**< @brief ACQCMN NSU timestamp capture event channel 0 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH1  (1UL<< 9UL ) /**< @brief ACQCMN NSU timestamp capture event channel 1 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH2  (1UL<< 10UL) /**< @brief ACQCMN NSU timestamp capture event channel 2 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH3  (1UL<< 11UL) /**< @brief ACQCMN NSU timestamp capture event channel 3 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_GRP1 (0xF000UL )  /**< @brief ACQCMN NSU timestamp capture event group 1   */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH4  (1UL<< 12UL) /**< @brief ACQCMN NSU timestamp capture event channel 4 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH5  (1UL<< 13UL) /**< @brief ACQCMN NSU timestamp capture event channel 5 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH6  (1UL<< 14UL) /**< @brief ACQCMN NSU timestamp capture event channel 6 */
#define ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH7  (1UL<< 15UL) /**< @brief ACQCMN NSU timestamp capture event channel 7 */
#define ACQCMN_INTERRUPT_NSU_SYSTICK_GRPP0     (0xF0000UL)  /**< @brief ACQCMN NSU systick interrupt group 0         */
#define ACQCMN_INTERRUPT_NSU_SYSTICK0          (1UL<< 16UL) /**< @brief ACQCMN NSU systick interrupt0                */
#define ACQCMN_INTERRUPT_NSU_SYSTICK1          (1UL<< 17UL) /**< @brief ACQCMN NSU systick interrupt1                */
#define ACQCMN_INTERRUPT_NSU_SYSTICK2          (1UL<< 18UL) /**< @brief ACQCMN NSU systick interrupt2                */
#define ACQCMN_INTERRUPT_NSU_SYSTICK3          (1UL<< 19UL) /**< @brief ACQCMN NSU systick interrupt3                */
#define ACQCMN_INTERRUPT_ALL                   (0xFFFFFUL)  /**< @brief ACQCMN All interrupts                        */
/** @} */

/**
    @name ACQCMN_AxisCfgType
    @{
    @brief ACQCMN Axis configuration Type

    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
 */
typedef struct sACQCMN_AxisCfgType {       /**< @brief ACQCMN Axis configuration Type                                                 */
    ACQCMN_AwgMuxType      awgMux        ; /**< @brief To select the AWG_DIN/LOAD for the axis_ctrl                                   */
    uint32_t               deglitchEnB   ; /**< @brief Glitch is filtered based on the DEGLITCH_WIDTH setting for for axis_gpio B     */
    uint32_t               deglitchEnA   ; /**< @brief Glitch is filtered based on the DEGLITCH_WIDTH setting for for axis_gpio A     */
    uint32_t               deglitchEnZ   ; /**< @brief Glitch is filtered based on the DEGLITCH_WIDTH setting for for axis_gpio Z     */
    uint32_t               deglitchWidth ; /**< @brief Signals need to be stable for N cycles (100MHz domain)                         */
    ACQCMN_EdgeSelType     edgeSel       ; /**< @brief Edge selection to increment the counter for the Pulse Angle position detection */
    ACQCMN_SensorSelType   sensorSel     ; /**< @brief Position information detector selection                                        */
    uint32_t               prescalerEn   ; /**< @brief To enable the clock prescaler for "Time Based" position detection              */
    uint32_t               prescalerVal  ; /**< @brief Clock prescaler value for "Time Based" position detection                      */
} ACQCMN_AxisCfgType;
/** @} */

/**
    @name ACQCMN_SrcSelBlkType
    @{
    @brief ACQCMN Source Select Block Type

    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
 */
typedef uint32_t ACQCMN_SrcSelBlkType;                      /**< @brief ACQCMN Select source block type.        */
#define ACQCMN_SRC_SEL_BLK_NSU_CNTR_HW_RST     (1UL<< 0UL ) /**< @brief ACQCMN NSU Counter hardware reset block */
#define ACQCMN_SRC_SEL_BLK_CCU_TMR0            (1UL<< 1UL ) /**< @brief ACQCMN CAPCOM TMR0 block */
#define ACQCMN_SRC_SEL_BLK_CCU_TMR1            (1UL<< 2UL ) /**< @brief ACQCMN CAPCOM TMR1 block */

/** @} */

/**
    @name ACQCMN_SrcSelEventType
    @{
    @brief ACQCMN Source Select Event Type

    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
 */
typedef uint32_t ACQCMN_SrcSelEventType;           /**< @brief ACQCMN select source event type.  */
#define ACQCMN_SRC_SEL_EVENT0_NSU                 (1UL<< 0UL ) /**< @brief ACQCMN select NSU event0          */
#define ACQCMN_SRC_SEL_EVENT1_NSU                 (1UL<< 1UL ) /**< @brief ACQCMN select NSU event1          */
#define ACQCMN_SRC_SEL_EVENT2_NSU                 (1UL<< 2UL ) /**< @brief ACQCMN select NSU event2          */
#define ACQCMN_SRC_SEL_EVENT3_NSU                 (1UL<< 3UL ) /**< @brief ACQCMN select NSU event3          */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_ETR          (1UL<< 4UL ) /**< @brief ACQCMN select TMR ETR source */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN0          (1UL<< 5UL ) /**< @brief ACQCMN select TMR IN0 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN1          (1UL<< 6UL ) /**< @brief ACQCMN select TMR IN1 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN2          (1UL<< 7UL ) /**< @brief ACQCMN select TMR IN2 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN3          (1UL<< 8UL ) /**< @brief ACQCMN select TMR IN3 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN0    (1UL<< 9UL ) /**< @brief ACQCMN select TMR event IN0 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN1    (1UL<< 10UL ) /**< @brief ACQCMN select TMR event IN1 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN2    (1UL<< 11UL ) /**< @brief ACQCMN select TMR event IN2 source  */
#define ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN3    (1UL<< 12UL ) /**< @brief ACQCMN select TMR event IN3 source  */

/** @} */

/**
    @name ACQCMN_SrcSelType
    @{
    @brief ACQCMN Source Select Type

    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
 */
typedef struct sACQCMN_SrcSelType {   /**< @brief Select ACQCMN source type */
    ACQCMN_SrcSelBlkType   blkType ;  /**< @brief Select ACQCMN block type  */
    ACQCMN_SrcSelEventType event   ;  /**< @brief Select ACQCMN event type  */
} ACQCMN_SrcSelType ;
/** @} */

/**
    @name ACQCMN_SrcCtrlType
    @{
    @brief ACQCMN source control type

    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
 */
typedef struct sACQCMN_SrcCtrlType {
    uint8_t srcSignal   ;   /**< @brief To enable source signal (1 : Enable, 0: Disable)  */
    uint8_t srcCtrlData ;   /**< @brief Source control data     (Bit [0:0] : Polarity selection [0 : NON_INVERT, 1 : INVERT],
                                                                 Bit [1:1] : Edge detection     [0 : SYNC,  1 : TRUE],
                                                                 Bit [3:2] : Edge selection     [0 : BUFFER,1 : RISING, 2: FALLING, 3: BOTH]) */
} ACQCMN_SrcCtrlType;
/** @} */

/** @brief Initializes ACQCMN

    This API initializes the given acquisition common hardware block

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId         Index of the ACQCMN controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_INIT

    @limitations None
*/
int32_t ACQCMN_DrvInit(ACQCMN_HwIDType hwId);

/** @brief De-initializes ACQCMN

    This API de-initializes ACQCMN. Resets the given ACQCMN controller and puts back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId         Index of the ACQCMN controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_INIT

    @limitations None
*/
int32_t ACQCMN_DrvDeInit(ACQCMN_HwIDType hwId);

/** @brief ACQCMN reset control

    This API enables/disables specified reset type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the ACQCMN controller
    @param[in]  resetType  Specified reset type
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_RESET_CONTROL

    @limitations None
*/
int32_t ACQCMN_DrvReset(ACQCMN_HwIDType hwId, ACQCMN_ResetType resetType);

/** @brief ACQCMN clock control

    This API enables/disables specified clock type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the ACQCMN controller
    @param[in]  clkType    Specified clock type
    @param[in]  aEnable    Enable/disable control
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_CLOCK_CONTROL

    @limitations None
*/
int32_t ACQCMN_DrvClkCtrl(ACQCMN_HwIDType hwId, ACQCMN_ClkType clkType, uint32_t aEnable);

/** @brief Sets the resolution and range from nsu_bin_cntr

    This API sets the resolution and range from nsu_bin_cntr

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the ACQCMN controller
    @param[in]  res        Resolution of the nsu_bin_cntr
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_AXIS_CONFIG

    @limitations None
*/
int32_t ACQCMN_DrvAxisResSet(ACQCMN_HwIDType hwId, ACQCMN_AxisResolutionType res);

/** @brief Sets Axis configuration

    This API sets Axis configuration based on axis instance

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the ACQCMN controller
    @param[in]  axId       Axis ID
    @param[in]  axisCfg    Pointer to axis configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL axis configuration
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_AXIS_CONFIG

    @limitations None
*/
int32_t ACQCMN_DrvAxisCfgSet(ACQCMN_HwIDType hwId, ACQCMN_AxisIdType axId, ACQCMN_AxisCfgType *axisCfg);

/** @brief Gets Axis configuration

    This API gets Axis configuration

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the ACQCMN controller
    @param[in]  axId       Axis ID
    @param[out] axisCfg    Pointer to axis configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_AXIS_CONFIG

    @limitations None
*/
int32_t ACQCMN_DrvAxisCfgGet(ACQCMN_HwIDType hwId, ACQCMN_AxisIdType axId, ACQCMN_AxisCfgType *axisCfg);

/** @brief ACQCMN Source control configuration

    This API configures source control information

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the ACQCMN controller
    @param[in]  srcSel     Pointer to source select information
    @param[out] srcCtrl    Pointer to source control information
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL srcSel
    @retval     #BCM_ERR_INVAL_PARAMS   NULL srcCtrl
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG

    @limitations None
*/
int32_t ACQCMN_DrvSrcControl(ACQCMN_HwIDType hwId, ACQCMN_SrcSelType *srcSel, ACQCMN_SrcCtrlType *srcCtrl);


/** @brief ACQCMN enable interrupts

    This API enables interrupts based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the ACQCMN controller
    @param[in]   intrType   Specified interrupt type
    @param[in]   aEnable    Enable/disable control
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_INTERRUPT

    @limitations None
*/
int32_t ACQCMN_DrvEnableInterrupt(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable);

/** @brief ACQCMN interrupt status

    This API gets interrupt status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the ACQCMN controller
    @param[out]  intrSts   Pointer to interrupt status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL interrupt status
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_INTERRUPT

    @limitations None
*/
int32_t ACQCMN_DrvInterruptStatus(ACQCMN_HwIDType hwId, ACQCMN_InterruptType *intrSts);

/** @brief ACQCMN event status

    This API gets event status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the ACQCMN controller
    @param[out]  eventSts  Pointer to event status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL event status
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_INTERRUPT

    @limitations None
*/
int32_t ACQCMN_DrvEventStatus(ACQCMN_HwIDType hwId, ACQCMN_InterruptType *eventSts);

/** @brief ACQCMN clear interrupts

    This API clears interrupt based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the ACQCMN controller
    @param[in]   intrType   Interrupt type to clear the interrupt
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          ACQCMN instance not initialized

    @post None

    @trace  #BRCM_SWREQ_ACQCMN_INTERRUPT

    @limitations None
*/
int32_t ACQCMN_DrvClearInterrupt(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType);

#endif /* ACQCMN_H */

/** @} */
