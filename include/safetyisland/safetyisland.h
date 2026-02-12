/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_safetyislanddrv_ifc Safety Island
    @ingroup grp_safetyislanddrv

    @addtogroup grp_safetyislanddrv_ifc
    @{
    @section sec_safetyisland_overview Overview
    -# Safety Island Bare Metal driver interface provides layer 1 APIs to configure
    the safety Island fault in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_safetyislanddrv_fsm Usage
    -# Safety Island HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |Safety_island_0|

    @section api_list List of Safety Island APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref SAFETYISLAND_DrvInit                            | Initialize SAFETYISLAND module       |
    | @ref SAFETYISLAND_DrvDeInit                          | De-initialize SAFETYISLAND module    |
    | @ref SAFETYISLAND_DrvFaultControlConfig              | Fault control configuration          |
    | @ref SAFETYISLAND_DrvFaultMaskConfig                 | Fault Mask configuration             |
    | @ref SAFETYISLAND_DrvFaultStatus                     | Fault Status                         |
    | @ref SAFETYISLAND_DrvFaultClear                      | Fault Clear                          |
    | @ref SAFETYISLAND_DrvFaultSafetyStatus               | Fault safety status                  |
    | @ref SAFETYISLAND_DrvFaultSafetyClear                | Fault safety clear                   |
    | @ref SAFETYISLAND_DrvStatusFaultSafetyResetSequencer | Fault safety reset sequencer status  |
    | @ref SAFETYISLAND_DrvClearFaultSafetyResetSequencer  | Fault safety reset sequencer clear   |
    | @ref SAFETYISLAND_DrvFaultSafetyBISRStatus           | Fault BISR status                    |
    | @ref SAFETYISLAND_DrvFaultSafetyGenReqCpuReset       | Generate CPU reset Request           |
    | @ref SAFETYISLAND_DrvFaultSafetyResetGenCpu          | Generate CPU reset                   |
    | @ref SAFETYISLAND_DrvFaultSafetyFaultGenCpu          | Generate CPU fault                   |
    | @ref SAFETYISLAND_DrvWriteFaultSafetyDebug           | Write fault safety debug information |
    | @ref SAFETYISLAND_DrvReadFaultSafetyDebug            | Read fault safety debug information  |

    @file safetyisland.h
    @brief Interface for safety island driver
    @section apis This section defines the interface for safety island driver
    @version 0.1 Initial Version
*/

#ifndef SAFETYISLAND_H
#define SAFETYISLAND_H

#include <stdint.h>

/**
    @name SafetyIsland driver API IDs
    @{
    @brief API IDs for SafetyIsland driver
 */
#define BRCM_SWARCH_SAFETYISLAND_HW_ID_TYPE                                   (0x8B01U)    /**< @brief #SAFETYISLAND_HwIDType                            */
#define BRCM_SWARCH_SAFETYISLAND_STATE_TYPE                                   (0x8B02U)    /**< @brief #SAFETYISLAND_StateType                           */
#define BRCM_SWARCH_SAFETYISLAND_SECURE_BOOT_TYPE                             (0x8B03U)    /**< @brief #SAFETYISLAND_SecureBootType                      */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_CAUSE_TYPE                             (0x8B04U)    /**< @brief #SAFETYISLAND_FaultCauseType                      */
#define BRCM_SWARCH_SAFETYISLAND_MCU0_NMI_TIME_OUT_RST_TYPE                   (0x8B05U)    /**< @brief #SAFETYISLAND_Mcu0NmiTimeOutRstType               */
#define BRCM_SWARCH_SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_TYPE                (0x8B06U)    /**< @brief #SAFETYISLAND_Mcu0NmiTimeOutScalerType            */
#define BRCM_SWARCH_SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_TYPE            (0x8B07U)    /**< @brief #SAFETYISLAND_ResetFsmRstOutCntScalerType         */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_MASK_TYPE                              (0x8B08U)    /**< @brief #SAFETYISLAND_FaultMaskType                       */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_STS_TYPE                        (0x8B09U)    /**< @brief #SAFETYISLAND_FaultSafetyStsType                  */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_BISR_STS_TYPE                   (0x8B0AU)    /**< @brief #SAFETYISLAND_FaultSafetyBISRStsType              */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_RSTFSM_TYPE                     (0x8B0BU)    /**< @brief #SAFETYISLAND_FaultSafetyRstfsmType               */
#define BRCM_SWARCH_SAFETYISLAND_CPU_FAULT_RESET_VAL_TYPE                     (0x8B0CU)    /**< @brief #SAFETYISLAND_CpuFaultResetValType                */

#define BRCM_SWARCH_SAFETYISLAND_FAULT_CONTROL_TYPE                           (0x8B10U)    /**< @brief #SAFETYISLAND_FaultControlType                    */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_MASK_CONFIG_TYPE                       (0x8B11U)    /**< @brief #SAFETYISLAND_FaultMaskConfigType                 */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_STATUS_TYPE                            (0x8B12U)    /**< @brief #SAFETYISLAND_FaultStatusType                     */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_CLEAR_TYPE                             (0x8B13U)    /**< @brief #SAFETYISLAND_FaultClearType                      */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_STATUS_TYPE                     (0x8B14U)    /**< @brief #SAFETYISLAND_FaultSafetyStatusType               */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_CLEAR_TYPE                      (0x8B15U)    /**< @brief #SAFETYISLAND_FaultSafetyClearType                */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_RESET_SEQUENCER_STATUS_TYPE     (0x8B16U)    /**< @brief #SAFETYISLAND_FaultSafetyResetSequencerStatusType */
#define BRCM_SWARCH_SAFETYISLAND_FAULT_SAFETY_RESET_SEQUENCER_CLEAR_TYPE      (0x8B17U)    /**< @brief #SAFETYISLAND_FaultSafetyResetSequencerClearType  */
#define BRCM_SWARCH_SAFETYISLAND_CPU_FAULT_RESET_GEN_TYPE                     (0x8B18U)    /**< @brief #SAFETYISLAND_CpuFaultResetGenType                */

#define BRCM_SWARCH_SAFETYISLAND_DRV_INIT_PROC                                (0x8B20U)    /**< @brief #SAFETYISLAND_DrvInit                             */
#define BRCM_SWARCH_SAFETYISLAND_DRV_DEINIT_PROC                              (0x8B21U)    /**< @brief #SAFETYISLAND_DrvDeInit                           */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CONTROL_CONFIG_PROC                (0x8B22U)    /**< @brief #SAFETYISLAND_DrvFaultControlConfig               */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC                   (0x8B23U)    /**< @brief #SAFETYISLAND_DrvFaultMaskConfig                  */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_STATUS_PROC                        (0x8B24U)    /**< @brief #SAFETYISLAND_DrvFaultStatus                      */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC                         (0x8B25U)    /**< @brief #SAFETYISLAND_DrvFaultClear                       */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_STATUS_PROC                 (0x8B26U)    /**< @brief #SAFETYISLAND_DrvFaultSafetyStatus                */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_CLEAR_PROC                  (0x8B27U)    /**< @brief #SAFETYISLAND_DrvFaultSafetyClear                 */
#define BRCM_SWARCH_SAFETYISLAND_DRV_STATUS_FAULT_SAFETY_RESET_SEQUENCER_PROC (0x8B28U)    /**< @brief #SAFETYISLAND_DrvStatusFaultSafetyResetSequencer  */
#define BRCM_SWARCH_SAFETYISLAND_DRV_CLEAR_FAULT_SAFETY_RESET_SEQUENCER_PROC  (0x8B29U)    /**< @brief #SAFETYISLAND_DrvClearFaultSafetyResetSequencer   */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_BISR_STATUS_PROC            (0x8B2AU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyBISRStatus            */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_GEN_REQ_CPU_RESET_PROC      (0x8B2BU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyGenReqCpuReset        */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC          (0x8B2CU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyResetGenCpu           */
#define BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_FAULT_GEN_CPU_PROC          (0x8B2DU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyFaultGenCpu           */
#define BRCM_SWARCH_SAFETYISLAND_DRV_WRITE_FAULT_SAFETY_DEBUG_PROC            (0x8B2EU)    /**< @brief #SAFETYISLAND_DrvWriteFaultSafetyDebug            */
#define BRCM_SWARCH_SAFETYISLAND_DRV_READ_FAULT_SAFETY_DEBUG_PROC             (0x8B2FU)    /**< @brief #SAFETYISLAND_DrvReadFaultSafetyDebug             */
/** @} */

/**
    @name SAFETYISLAND_HwIDType
    @{
    @brief Index of the SAFETYISLAND module

    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
 */
typedef uint32_t SAFETYISLAND_HwIDType;  /**< @brief SAFETYISLAND HW Id type */
#define SAFETYISLAND_HW_ID_0      (0UL)  /**< @brief SAFETYISLAND HW Id 0    */
/** @} */

/**
    @name SAFETYISLAND_StateType
    @{
    @brief State of SAFETYISLAND driver

    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
 */
typedef uint8_t SAFETYISLAND_StateType;           /**< @brief SAFETYISLAND state type */
#define SAFETYISLAND_STATE_UNINIT           (0U)  /**< @brief SAFETYISLAND is uninitialized and value must be zero*/
#define SAFETYISLAND_STATE_INITIALIZED      (1U)  /**< @brief SAFETYISLAND is initialized */
/** @} */

/**
    @name SAFETYISLAND_SecureBootType
    @{
    @brief Safety Island Software enabled secure boot configuration

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
 */
typedef uint8_t SAFETYISLAND_SecureBootType;       /**< @brief Software enabled secure boot configuration type */
#define SAFETYISLAND_SECURE_BOOT_DISABLE     (0U)  /**< @brief Disable secure boot configuration               */
#define SAFETYISLAND_SECURE_BOOT_ENABLE      (1U)  /**< @brief Enable secure boot configuration                */
/** @} */

/**
    @name SAFETYISLAND_FaultCauseType
    @{
    @brief Safety Island fault cause configuration

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
 */
typedef uint8_t SAFETYISLAND_FaultCauseType;       /**< @brief safetyisland fault cause Type               */
#define SAFETYISLAND_FAULT_CAUSE_DISABLE     (0U)  /**< @brief Disable fault cause information to serially
                                                               shiftout from GPIO pin                      */
#define SAFETYISLAND_FAULT_CAUSE_ENABLE      (1U)  /**< @brief Enable GPIO pin to serially
                                                               shiftout fault_cause information            */
/** @} */

/**
    @name SAFETYISLAND_Mcu0NmiTimeOutRstType
    @{
    @brief Reset the NMI state timeout counter in reset sequencer

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
 */
typedef uint8_t SAFETYISLAND_Mcu0NmiTimeOutRstType;      /**< @brief Reset type of the NMI state timeout counter in reset sequencer */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_RST_OUT     (0U)  /**< @brief Deafult state of the timeout counter     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_RST_IN      (1U)  /**< @brief Reset the timeout counter                */
/** @} */

/**
    @name SAFETYISLAND_Mcu0NmiTimeOutRstType
    @{
    @brief Scaling factor for mcu_0 NMI interrupt timeout counter

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
 */
typedef uint8_t SAFETYISLAND_Mcu0NmiTimeOutScalerType;   /**< @brief Type of scaling factor for mcu_0 NMI interrupt timeout counter */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_0     (0U) /**< @brief MCU0 NMI time out scaler value 0     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_1     (1U) /**< @brief MCU0 NMI time out scaler value 1     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_2     (2U) /**< @brief MCU0 NMI time out scaler value 2     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_3     (3U) /**< @brief MCU0 NMI time out scaler value 3     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_4     (4U) /**< @brief MCU0 NMI time out scaler value 4     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_5     (5U) /**< @brief MCU0 NMI time out scaler value 5     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_6     (6U) /**< @brief MCU0 NMI time out scaler value 6     */
#define SAFETYISLAND_MCU0_NMI_TIME_OUT_SCALER_7     (7U) /**< @brief MCU0 NMI time out scaler value 7     */
/** @} */

/**
    @name SAFETYISLAND_ResetFsmRstOutCntScalerType
    @{
    @brief Scaling factor for resetfsm_reset_out counter

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
 */
typedef uint8_t SAFETYISLAND_ResetFsmRstOutCntScalerType;  /**< @brief Scaling factor for resetfsm_reset_out counter */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_0   (0U) /**< @brief Reset FSM reset out counter scaler value 0    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_1   (1U) /**< @brief Reset FSM reset out counter scaler value 1    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_2   (2U) /**< @brief Reset FSM reset out counter scaler value 2    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_3   (3U) /**< @brief Reset FSM reset out counter scaler value 3    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_4   (4U) /**< @brief Reset FSM reset out counter scaler value 4    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_5   (5U) /**< @brief Reset FSM reset out counter scaler value 5    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_6   (6U) /**< @brief Reset FSM reset out counter scaler value 6    */
#define SAFETYISLAND_RESET_FSM_RST_OUT_CNT_SCALER_7   (7U) /**< @brief Reset FSM reset out counter scaler value 7    */
/** @} */

/**
    @name SAFETYISLAND_FaultMaskType
    @{
    @brief Safety Island fault mask type

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
 */
typedef uint32_t SAFETYISLAND_FaultMaskType;                               /**< @brief SAFETYISLAND fault mask Type */

#define SAFETYISLAND_FAULT_MASK_OUT_LIVE                  (0x1UL << 31UL)  /**< @brief Mask for Rigel Fault out live status (@note This bit is not present in Hardware) */
#define SAFETYISLAND_FAULT_MASK_CPU_FAULT_OUT             (0x1UL << 24UL)  /**< @brief Mask for CPU fault out, SW triggered fault mask   */
#define SAFETYISLAND_FAULT_MASK_QSPI_FIFO_OVERUNDER_FLOW  (0x1UL << 23UL)  /**< @brief Mask for Qspi Fifo Over Or Under Flow             */
#define SAFETYISLAND_FAULT_MASK_AHB_APB_BUS               (0x1UL << 22UL)  /**< @brief Mask for AHB APB Bus fault                        */
#define SAFETYISLAND_FAULT_MASK_SYSFABRIC_NOC             (0x1UL << 21UL)  /**< @brief Mask for System Fabric NOC fault                  */
#define SAFETYISLAND_FAULT_MASK_OVERHEAT_DETECTOR         (0x1UL << 20UL)  /**< @brief Mask for Internal Overheat Detector fault         */
#define SAFETYISLAND_FAULT_MASK_BROWNOUT_DETECTOR         (0x1UL << 19UL)  /**< @brief Mask for Internal Power fault                     */
#define SAFETYISLAND_FAULT_MASK_POWER_GOOD_PIN            (0x1UL << 18UL)  /**< @brief Mask for External Power_Good Input fault          */
#define SAFETYISLAND_FAULT_MASK_RESETB_IN_PIN             (0x1UL << 17UL)  /**< @brief Mask for RESETB_in Pin is Active (@note This bit is not present in Hardware) */
#define SAFETYISLAND_FAULT_MASK_RIG_DED                   (0x1UL << 16UL)  /**< @brief Mask for (Non Mcu_0) Memory DED fault             */
#define SAFETYISLAND_FAULT_MASK_MCU1_LOCKUP_Q8_FATAL      (0x1UL << 15UL)  /**< @brief Mask for MCU1 lockup and Q8 fatal fault           */
#define SAFETYISLAND_FAULT_MASK_SM_GP_SRAM_MEM_PROTECT    (0x1UL << 14UL)  /**< @brief Mask for General purpose SRAM memory access protection fault */
#define SAFETYISLAND_FAULT_MASK_SM_ACQ_HSADC_FUNC         (0x1UL << 13UL)  /**< @brief Mask for acquisition system ADC functional fault mask        */
#define SAFETYISLAND_FAULT_MASK_SM_PMIC_INT               (0x1UL << 12UL)  /**< @brief Mask for external power monitor IC input interrupt fault     */
#define SAFETYISLAND_FAULT_MASK_WWDT_1_7_TIMEOUT          (0x1UL << 11UL)  /**< @brief Mask for WWDT 1 to 7 timeout fault                */
#define SAFETYISLAND_FAULT_MASK_MCU0_WWDT0_TIMEOUT        (0x1UL << 10UL)  /**< @brief Mask for MCU_0 WWDT timeout fault                 */
#define SAFETYISLAND_FAULT_MASK_MCU0_NMI_TIMEOUT          (0x1UL <<  9UL)  /**< @brief Mask for MCU_0 NMI timeout fault (@note This bit is not present in Hardware) */
#define SAFETYISLAND_FAULT_MASK_MCU0_SYSTEM_RESET_REQ     (0x1UL <<  8UL)  /**< @brief Mask for MCU_0 system reset request fault (@note This bit is not present in Hardware)*/
#define SAFETYISLAND_FAULT_MASK_MCU0_FUNCTIONAL           (0x1UL <<  7UL)  /**< @brief Mask for MCU_0 fault sources DCLS, LOCKUP, NOC    */
#define SAFETYISLAND_FAULT_MASK_MCU0_MEM_DED              (0x1UL <<  6UL)  /**< @brief Mask for MCU_0 related memory (DTCM, GPRAM, HSM-Lite, LSP) fault      */
#define SAFETYISLAND_FAULT_MASK_MCU0_MEM_INIT_TIMEOUT     (0x1UL <<  5UL)  /**< @brief Mask for MCU_0 related memory (DTCM, GPRAM, HSM-Lite, LSP) init error */
#define SAFETYISLAND_FAULT_MASK_MCU0_MBIST_FAIL           (0x1UL <<  4UL)  /**< @brief Mask for MCU0 MBIST test failed fault (@note This bit is not present in Hardware)*/
#define SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_TIMEOUT       (0x1UL <<  3UL)  /**< @brief Mask for system PLL lock timeout fault */
#define SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_LOSS          (0x1UL <<  2UL)  /**< @brief Mask for system PLL loss of lock fault */
#define SAFETYISLAND_FAULT_MASK_BISR_FAIL                 (0x1UL <<  1UL)  /**< @brief Mask for Memory built in Self repair (BISR) fail fault */
#define SAFETYISLAND_FAULT_MASK_OTP0_FDONE_TIMEOUT        (0x1UL <<  0UL)  /**< @brief Mask for OTP0 fdone timeout fault */
#define SAFETYISLAND_FAULT_MASK_CONFIG_DEFAULT            (0xF1FCC0UL   )  /**< @brief Mask configuration with default faults */
#define SAFETYISLAND_FAULT_MASK_CONFIG_ALL                (0x1FFFCEFUL  )  /**< @brief Mask configuration for all faults */
#define SAFETYISLAND_FAULT_MASK_STATUS_ALL                (0x81FDFFFFUL )  /**< @brief Mask for all fault status         */
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyStsType
    @{
    @brief Safety Island fault safety status mask

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
 */
typedef uint32_t SAFETYISLAND_FaultSafetyStsType;                            /**< @brief SAFETYISLAND fault safety status type */
#define SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_STARTED (0x1UL << 5UL)  /**< @brief Mask for MCU0 mem init started */
#define SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_GOOD    (0x1UL << 4UL)  /**< @brief Mask for MCU0 mem init good    */
#define SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_ERROR   (0x1UL << 3UL)  /**< @brief Mask for MCU0 mem init error   */
#define SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_DONE    (0x1UL << 2UL)  /**< @brief Mask for MCU0 mem init done    */
#define SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_GOOD       (0x1UL << 1UL)  /**< @brief Mask for MCU0 MBIST good       */
#define SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_DONE       (0x1UL << 0UL)  /**< @brief Mask for MCU0 MBIST done       */
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyBISRStsType
    @{
    @brief Safety Island fault safety BISR status

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_BISR_STATUS
 */
typedef uint32_t SAFETYISLAND_FaultSafetyBISRStsType;           /**< @brief SAFETYISLAND fault Safety BISR status Type */
#define SAFETYISLAND_FAULT_SAFETY_BISR_STS_GOOD  (0x1UL << 1UL) /**< @brief BISR status good */
#define SAFETYISLAND_FAULT_SAFETY_BISR_STS_DONE  (0x1UL << 0UL) /**< @brief BISR status done */
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyResetSequencerStateType
    @{
    @brief Safety Island fault safety reset sequencer state

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
 */
typedef uint32_t SAFETYISLAND_FaultSafetyRstfsmType;                               /**< @brief Reset sequencer state Type   */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_DEFAULT                  (0x0UL)          /**< @brief RSTFSM_DEFAULT               */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_START                    (0x1UL << 0UL)   /**< @brief RSTFSM_START                 */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_STRAPEN                  (0x1UL << 1UL)   /**< @brief RSTFSM_STRAPEN               */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_XTAL                     (0x1UL << 2UL)   /**< @brief RSTFSM_XTAL                  */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_PLLCHK                   (0x1UL << 3UL)   /**< @brief RSTFSM_PLLCHK                */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_PLLDIVEN                 (0x1UL << 4UL)   /**< @brief RSTFSM_PLLDIVEN              */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_BISR_CHECK               (0x1UL << 5UL)   /**< @brief RSTFSM_BISR_CHECK            */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_BISR_INIT                (0x1UL << 6UL)   /**< @brief RSTFSM_BISR_INIT             */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_BISR_DONE                (0x1UL << 7UL)   /**< @brief RSTFSM_BISR_DONE             */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_FLASH_NOC_MBIST_CHECK    (0x1UL << 8UL)   /**< @brief RSTFSM_FLASH_NOC_MBIST_CHECK */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_MBIST_INIT               (0x1UL << 9UL)   /**< @brief RSTFSM_MBIST_INIT            */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_MBIST_DONE               (0x1UL << 10UL)  /**< @brief RSTFSM_MBIST_DONE            */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_CPUMEM_CHECK             (0x1UL << 11UL)  /**< @brief RSTFSM_CPUMEM_CHECK          */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_CPUMEM_INIT              (0x1UL << 12UL)  /**< @brief RSTFSM_CPUMEM_INIT           */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_CPUMEM_INITDONE          (0x1UL << 13UL)  /**< @brief RSTFSM_CPUMEM_INITDONE       */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_CORE_RST                 (0x1UL << 14UL)  /**< @brief RSTFSM_CORE_RST              */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_MCU0_CPUWAIT             (0x1UL << 15UL)  /**< @brief RSTFSM_MCU0_CPUWAIT          */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_RSTFSM_GOOD              (0x1UL << 16UL)  /**< @brief RSTFSM_RSTFSM_GOOD           */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_RSTFSM_NMI               (0x1UL << 17UL)  /**< @brief RSTFSM_RSTFSM_NMI            */
#define SAFETYISLAND_FAULT_SAFETY_RSTFSM_RSTFSM_RST               (0x1UL << 18UL)  /**< @brief RSTFSM_RSTFSM_RST            */
/** @} */

/**
    @name SAFETYISLAND_CpuFaultResetValType
    @{
    @brief Safety Island CPU fault reset values

    @trace #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION
 */
typedef uint8_t SAFETYISLAND_CpuFaultResetValType;     /**< @brief SAFETYISLAND CPU fault reset value type */
#define SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW        0U /**< @brief Fault reset value low       */
#define SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH       1U /**< @brief Fault reset value high      */
#define SAFETYISLAND_CPU_FAULT_RESET_VAL_NO_CHANGE  2U /**< @brief Fault reset value no change */
/** @} */

/**
    @name SAFETYISLAND_FaultControlType
    @{
    @brief Fault control configuration

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
 */
typedef struct sSAFETYISLAND_FaultControlType {
    SAFETYISLAND_SecureBootType softSecureBootEn;                     /**< @brief Software enabled secure boot enable bit                */
    SAFETYISLAND_FaultCauseType faultCause;                           /**< @brief Fault Cause configuration                              */
    SAFETYISLAND_Mcu0NmiTimeOutRstType mcu0NmiTimeoutRst;             /**< @brief Reset the NMI state timeout counter in reset sequencer */
    SAFETYISLAND_Mcu0NmiTimeOutScalerType mcu0NmiTimeoutScaler;       /**< @brief Scaling factor for mcu_0 NMI interrupt timeout counter */
    SAFETYISLAND_ResetFsmRstOutCntScalerType resetFsmRstOutCntScaler; /**< @brief scaling factor for resetfsm_reset_out counter          */
} SAFETYISLAND_FaultControlType;
/** @} */

/**
    @name SAFETYISLAND_FaultMaskConfigType
    @{
    @brief Fault Mask configuration

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultMaskConfigType {
   SAFETYISLAND_FaultMaskType faultMask; /**< @brief  Fault Mask configuration  */
} SAFETYISLAND_FaultMaskConfigType;
/** @} */

/**
    @name SAFETYISLAND_FaultStatusType
    @{
    @brief Fault Status

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultStatusType {
   SAFETYISLAND_FaultMaskType status; /**< @brief  Fault Status */
} SAFETYISLAND_FaultStatusType;
/** @} */

/**
    @name SAFETYISLAND_FaultClearType
    @{
    @brief Fault Clear

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultClearType {
   SAFETYISLAND_FaultMaskType faultMask; /**< @brief  Fault Mask configuration to clear faults */
} SAFETYISLAND_FaultClearType;
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyStatusType
    @{
    @brief Fault safety status

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultSafetyStatusType {
   SAFETYISLAND_FaultSafetyStsType status; /**< @brief  Fault Safety Status  */
} SAFETYISLAND_FaultSafetyStatusType;
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyClearType
    @{
    @brief Fault Safety clear type

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultSafetyClearType {
   SAFETYISLAND_FaultSafetyStsType faultSafetyMask; /**< @brief  Fault safety mask configuration to clear faults */
} SAFETYISLAND_FaultSafetyClearType;
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyResetSequencerStatusType
    @{
    @brief Fault safety reset sequencer status type

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultSafetyResetSequencerStatusType {
    SAFETYISLAND_FaultSafetyRstfsmType state; /**< @brief  Fault safety reset sequencer state */
} SAFETYISLAND_FaultSafetyResetSequencerStatusType;
/** @} */

/**
    @name SAFETYISLAND_FaultSafetyResetSequencerClearType
    @{
    @brief Fault safety reset sequencer clear type

    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
 */
typedef struct sSAFETYISLAND_FaultSafetyResetSequencerClearType {
    SAFETYISLAND_FaultSafetyRstfsmType rstFsmMask; /**< @brief  Fault safety reset sequencer clear mask */
} SAFETYISLAND_FaultSafetyResetSequencerClearType;
/** @} */

/**
    @name SAFETYISLAND_CpuFaultResetGenType
    @{
    @brief CPU reset and fault generation type

    @trace #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION
 */
typedef struct sSAFETYISLAND_CpuFaultResetGenType {
    SAFETYISLAND_CpuFaultResetValType outValue;     /**< @brief  On fault 0/1 will be
                                                         generated from cpuReset and cpuFault Pin */
} SAFETYISLAND_CpuFaultResetGenType;
/** @} */

/** @brief Initialize SAFETYISLAND driver

    This API Initializes SAFETYISLAND driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId              Index of the SAFETYISLAND controller

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_OK             Success

    @post SAFETYISLAND driver state changed to #SAFETYISLAND_STATE_INITIALIZED

    @trace  #BRCM_SWREQ_SAFETYISLAND_INIT

    @limitations None
*/
int32_t SAFETYISLAND_DrvInit(SAFETYISLAND_HwIDType hwId);

/** @brief De-initialize SAFETYISLAND driver

    This API De-initializes SAFETYISLAND driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId              Index of the SAFETYISLAND controller

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_OK             Success

    @post SAFETYISLAND driver state changed to #SAFETYISLAND_STATE_UNINIT

    @trace  #BRCM_SWREQ_SAFETYISLAND_INIT

    @limitations None
*/
int32_t SAFETYISLAND_DrvDeInit(SAFETYISLAND_HwIDType hwId) ;

/** @brief Fault Control Configuration

    This API performs fault control configuration

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId                Index of the SAFETYISLAND module
    @param[in]     faultControlConfig  Fault control configuration

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultControlConfig(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultControlType faultControlConfig);

/** @brief Fault Mask Configuration

    This API configures fault mask. Each fault signal may be individually masked for
    FAULT_OUT generation.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId     Index of the SAFETYISLAND controller
    @param[in]      config   Fault Mask configuration.
                             config.faultMask[BIT_POS] = False implies BIT_POS fault is enabled.
                             config.faultMask[BIT_POS] = True  implies BIT_POS fault is disabled/ignored.
                             For example,config.faultMask.BIT23 = False implies QSPI FIFO overflow and underflow fault is enabled
                             Similarly,  config.faultMask.BIT23 = True  implies QSPI FIFO overflow and underflow fault is disabled/ignored
    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultMaskConfig(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config);

/** @brief Get Fault status
    This API gets status of all the faults.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId   Index of the SAFETYISLAND module
    @param[out]    fault  Pointer to fault status type

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultStatus(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultStatusType *fault);

/** @brief Fault clear
    This API clears the faults

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId   Index of the SAFETYISLAND module
    @param[in]     clear  Fault clear type

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultClear(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear);

/** @brief Get Fault safety Status
    This API gets fault safety status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId               Index of the SAFETYISLAND module
    @param[out]    faultSafetyStatus  Pointer to fault safety Status

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultSafetyStatus(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyStatusType *faultSafety);

/** @brief Fault safety clear
    This API clears the safety faults

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId   Index of the SAFETYISLAND module
    @param[in]     clear  Safety fault clear type

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultSafetyClear(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyClearType clear);

/** @brief Gets the reset sequencer state.
    This API gets the reset sequencer state.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId                  Index of the SAFETYISLAND module
    @param[out]    resetSequencerStatus  Pointer to fault safety Status

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvStatusFaultSafetyResetSequencer(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyResetSequencerStatusType *resetSequencerStatus);

/** @brief Fault safety clear
    This API clears the safety faults

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId   Index of the SAFETYISLAND module
    @param[in]     clear  Safety fault clear type

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR

    @limitations None
*/
int32_t SAFETYISLAND_DrvClearFaultSafetyResetSequencer(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyResetSequencerClearType clear);

/** @brief Get fault safety BISR status
    This API gets fault safety BISR status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId                Index of the SAFETYISLAND module
    @param[out]    faultSafetyBISRSts  Pointer to fault safety BISR status

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_BISR_STATUS

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultSafetyBISRStatus(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyBISRStsType *faultSafetyBISRSts);

/** @brief Generate CPU reset request
    This API generates CPU reset request and send out to the pin.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId               Index of the SAFETYISLAND module
    @param[in]     cpuResetReqConfig  CPU reset configuration

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultSafetyGenReqCpuReset(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_CpuFaultResetGenType cpuResetReqConfig);

/** @brief This API sets value of cpu_reset_out_n.
    This value is output to resetb_out output pin of the chip.
    Asserting this pin does not reset the chip internal logic.
    It only drive the set value to the resetb_out pin

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId               Index of the SAFETYISLAND module
    @param[in]     cpuResetConfig     CPU reset configuration

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultSafetyResetGenCpu(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_CpuFaultResetGenType cpuResetConfig);

/** @brief Generate CPU fault
    This API generates CPU fault and send out to the pin.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId               Index of the SAFETYISLAND module
    @param[in]     cpuFaultConfig     CPU fault configuration

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION

    @limitations None
*/
int32_t SAFETYISLAND_DrvFaultSafetyFaultGenCpu(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_CpuFaultResetGenType cpuFaultConfig);

/** @brief Writes safety debug register
    This API writes to specified safety debug register

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                Index of the SAFETYISLAND module
    @param[in]      index               Index of the debug register
    @param[in]      data                Write data

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_DEBUG

    @limitations None
*/
int32_t SAFETYISLAND_DrvWriteFaultSafetyDebug(SAFETYISLAND_HwIDType hwId, uint32_t index, uint32_t data);

/** @brief Reads safety debug register
    This API reads specified safety debug register

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                Index of the SAFETYISLAND module
    @param[in]      index               Index of the debug register
    @param[out]     data                Pointer to the Read data

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_SAFETYISLAND_DEBUG

    @limitations None
*/
int32_t SAFETYISLAND_DrvReadFaultSafetyDebug(SAFETYISLAND_HwIDType hwId, uint32_t index, uint32_t *data);

#endif /* SAFETYISLAND_H */

/** @} */
