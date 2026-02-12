/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_ccudrv_ifc Interface
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_ifc
    @{
    @section sec_ccu_overview Overview
    CCU driver manages compare output and capture input for CCT block. Each
    channel of CCU can be configured in capture or compare mode.

    In compare mode, CCU channel is configured for a compare threshold value. If
    pin action chosen is HIGH level on compare match, in this case compare
    output is set to LOW level from start of CCU counter value till compare
    threshold. On compare match, compare output is set to HIGH level till CCU
    counter increments to maximum configured count. In this way, different duty
    cycle signals can be generated on compare output pin. #CCU_DrvSetCompPinProperty
    need to be called before calling #CCU_DrvSetCompAbsThreshold to set compare threshold
    value.

    In capture mode, CCU channel captures different signal information of input
    signal on capture pin. CCU channel can be configure for edge count, edge
    detect, timestamp capture and signal measurement capture modes. In edge
    count mode, CCU driver can be queried for number of expected edge counted of
    the input signal. In edge detect mode, CCU driver invoke callback function
    on detection of expected edge of the input signal. In timestamp mode, CCU
    driver captures timestamp on reception of expected edge of the input signal
    and invokes callback based on the notify interval configured. In signal
    measurement mode, CCU driver can be used to retrieve HIGH time, LOW time,
    period or duty cycle of the input signal.

    @note There shall be only one client for each CCU channel.

    @limitations None
    @section api_list List of CCU APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref CCU_DrvInit | Initialize CCU driver |
    | @ref CCU_DrvDeInit | De-initialize CCU driver |
    | @ref CCU_DrvGetChanMode | Retrieve mode of the channel |
    | @ref CCU_DrvStartCompChannel | Start compare channel |
    | @ref CCU_DrvStopCompChannel | Stop compare channel |
    | @ref CCU_DrvSetCompPinProperty | Set compare out pin properties |
    | @ref CCU_DrvSetCompAbsThreshold | Set absolute threshold for compare match |
    | @ref CCU_DrvEnableCompCallback | Enable compare callbac |
    | @ref CCU_DrvDisableCompCallback | Disable compare callback |
    | @ref CCU_DrvSetCapSignalProp | Set capture signal properties |
    | @ref CCU_DrvEnableCapCallback | Enable capture callback |
    | @ref CCU_DrvDisableCapCallback | Disable capture callback |
    | @ref CCU_DrvGetCapInputState | Retrieve input state of the capture channel |
    | @ref CCU_DrvStartCapTimestamp | Start capture channel time stamping |
    | @ref CCU_DrvStopCapTimestamp | Stop capture channel time stamping |
    | @ref CCU_DrvGetCapTimestampIndex | Retrieve capture channel timestamp index |
    | @ref CCU_DrvResetCapEdgeCount | Reset the capture edge counter |
    | @ref CCU_DrvEnableCapEdgeCount | Enable capture edge counter |
    | @ref CCU_DrvDisableCapEdgeCount | Disable capture edge counter |
    | @ref CCU_DrvEnableCapEdgeDetection | Enable capture edge detection |
    | @ref CCU_DrvDisableCapEdgeDetection | Disable capture edge detection |
    | @ref CCU_DrvGetCapEdgeNumbers | Retrieve capture edge number |
    | @ref CCU_DrvStartCapSignalMeasurement | Start capture signal measurement |
    | @ref CCU_DrvStopCapSignalMeasurement | Stop capture signal measurement |
    | @ref CCU_DrvGetCapTimeElapsed | Retrieve capture time elapsed |
    | @ref CCU_DrvGetCapDutyCycleValues | Retrieve capture channel duty cycle values |
    | @ref CCU_DrvIRQHandler | CCU irq handler |
    | @ref CCU_DrvCombOutputSel | CCU combined output select |
    | @ref CCU_DrvInputFilterCtrlSet | CCU input filter control select |
    | @ref CCU_DrvEventTriggerSel | CCU event trigger select control |
    | @ref CCU_EnableInterrupt | CCU enable interrupt |
    | @ref CCU_ClearInterrupt | CCU clear interrupt |
    | @ref CCU_GetInterruptStatus | CCU get interrupt status |

    @file ccu.h
    @brief CCU driver application interface
    @section apis This section defines the interface for CCU driver
    @version 0.1 Imported from docx
*/

#ifndef CCU_H
#define CCU_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>

/**
    @name CCU API IDs
    @{
    @brief API IDs for CCU driver
*/
#define BRCM_SWARCH_CCU_HWID_TYPE                         (0x8100U)    /**< @brief #CCU_HwIDType                  */
#define BRCM_SWARCH_CCU_CHAN_TYPE                         (0x8101U)    /**< @brief #CCU_ChanType                  */
#define BRCM_SWARCH_CCU_CHANMODE_TYPE                     (0x8102U)    /**< @brief #CCU_ChanModeType              */
#define BRCM_SWARCH_CCU_OUTPINSEL_TYPE                    (0x8103U)    /**< @brief #CCU_OutPinSelType             */
#define BRCM_SWARCH_CCU_PINACTION_TYPE                    (0x8104U)    /**< @brief #CCU_PinActionType             */
#define BRCM_SWARCH_CCU_INPINSEL_TYPE                     (0x8105U)    /**< @brief #CCU_InPinSelType              */
#define BRCM_SWARCH_CCU_INPUTSTATE_TYPE                   (0x8106U)    /**< @brief #CCU_InputStateType            */
#define BRCM_SWARCH_CCU_EDGE_TYPE                         (0x8107U)    /**< @brief #CCU_EdgeType                  */
#define BRCM_SWARCH_CCU_VALUE_TYPE                        (0x8108U)    /**< @brief #CCU_ValueType                 */
#define BRCM_SWARCH_CCU_TSINDEX_TYPE                      (0x8109U)    /**< @brief #CCU_TSIndexType               */
#define BRCM_SWARCH_CCU_EDGENUMBER_TYPE                   (0x810AU)    /**< @brief #CCU_EdgeNumberType            */
#define BRCM_SWARCH_CCU_CAPMODE_TYPE                      (0x810BU)    /**< @brief #CCU_CapModeType               */
#define BRCM_SWARCH_CCU_SIGNALPROPERTY_TYPE               (0x810CU)    /**< @brief #CCU_SignalPropertyType        */
#define BRCM_SWARCH_CCU_TIMESTAMPBUF_TYPE                 (0x810DU)    /**< @brief #CCU_TimeStampBufType          */
#define BRCM_SWARCH_CCU_CALLBACK_TYPE                     (0x810EU)    /**< @brief #CCU_CallbackType              */
#define BRCM_SWARCH_CCU_CHANCAPCFG_TYPE                   (0x810FU)    /**< @brief #CCU_ChanCapCfgType            */
#define BRCM_SWARCH_CCU_CHANCOMPCFG_TYPE                  (0x8110U)    /**< @brief #CCU_ChanCompCfgType           */
#define BRCM_SWARCH_CCU_CHANCFGTBL_TYPE                   (0x8111U)    /**< @brief #CCU_ChanCfgTblType            */
#define BRCM_SWARCH_CCU_CONFIG_TYPE                       (0x8112U)    /**< @brief #CCU_ConfigType                */
#define BRCM_SWARCH_CCU_DUTYCYCLE_TYPE                    (0x8113U)    /**< @brief #CCU_DutyCycleType           */
#define BRCM_SWARCH_CCU_COMB_OUT_SEL_TYPE                 (0x8114U)    /**< @brief #CCU_CombOutSelType        */
#define BRCM_SWARCH_CCU_INPUT_FILTER_TYPE                 (0x8115U)    /**< @brief #CCU_InputFilterType            */
#define BRCM_SWARCH_CCU_EVENT_TRIG_SEL_MASK_TYPE          (0x8116U)    /**< @brief #CCU_EventTrigSelMaskType */
#define BRCM_SWARCH_CCU_INTERRUPT_TYPE                    (0x8117U)    /**< @brief #CCU_InterruptType          */

#define BRCM_SWARCH_CCU_INIT_PROC                         (0x8118U)    /**< @brief #CCU_DrvInit                      */
#define BRCM_SWARCH_CCU_DEINIT_PROC                       (0x8119U)    /**< @brief #CCU_DrvDeInit                    */
#define BRCM_SWARCH_CCU_GETCHANMODE_PROC                  (0x811AU)    /**< @brief #CCU_DrvGetChanMode               */
#define BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC             (0x811BU)    /**< @brief #CCU_DrvStartCompChannel          */
#define BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC              (0x811CU)    /**< @brief #CCU_DrvStopCompChannel           */
#define BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC           (0x811DU)    /**< @brief #CCU_DrvSetCompPinProperty        */
#define BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC          (0x811EU)    /**< @brief #CCU_DrvSetCompAbsThreshold       */
#define BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC           (0x811FU)    /**< @brief #CCU_DrvEnableCompCallback        */
#define BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC          (0x8120U)    /**< @brief #CCU_DrvDisableCompCallback       */
#define BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC             (0x8121U)    /**< @brief #CCU_DrvSetCapSignalProp          */
#define BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC            (0x8122U)    /**< @brief #CCU_DrvEnableCapCallback         */
#define BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC           (0x8123U)    /**< @brief #CCU_DrvDisableCapCallback        */
#define BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC             (0x8124U)    /**< @brief #CCU_DrvGetCapInputState          */
#define BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC            (0x8125U)    /**< @brief #CCU_DrvStartCapTimestamp         */
#define BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC             (0x8126U)    /**< @brief #CCU_DrvStopCapTimestamp          */
#define BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC         (0x8127U)    /**< @brief #CCU_DrvGetCapTimestampIndex      */
#define BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC            (0x8128U)    /**< @brief #CCU_DrvResetCapEdgeCount         */
#define BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC           (0x8129U)    /**< @brief #CCU_DrvEnableCapEdgeCount        */
#define BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC          (0x812AU)    /**< @brief #CCU_DrvDisableCapEdgeCount       */
#define BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC       (0x812BU)    /**< @brief #CCU_DrvEnableCapEdgeDetection    */
#define BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC      (0x812CU)    /**< @brief #CCU_DrvDisableCapEdgeDetection   */
#define BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC            (0x812DU)    /**< @brief #CCU_DrvGetCapEdgeNumbers         */
#define BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC    (0x812EU)    /**< @brief #CCU_DrvStartCapSignalMeasurement */
#define BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC     (0x812FU)    /**< @brief #CCU_DrvStopCapSignalMeasurement  */
#define BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC            (0x8130U)    /**< @brief #CCU_DrvGetCapTimeElapsed         */
#define BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC        (0x8131U)    /**< @brief #CCU_DrvGetCapDutyCycleValues     */
#define BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC              (0x8132U)    /**< @brief #CCU_DrvIRQHandler */
#define BRCM_SWARCH_CCU_DRV_COMB_OUTPUT_SEL_PROC          (0x8133U)    /**< @brief #CCU_DrvCombOutputSel */
#define BRCM_SWARCH_CCU_DRV_INPUT_FILTER_CTRL_SET_PROC    (0x8134U)    /**< @brief #CCU_DrvInputFilterCtrlSet */
#define BRCM_SWARCH_CCU_DRV_EVENT_TRIGGER_SEL_PROC        (0x8135U)    /**< @brief #CCU_DrvEventTriggerSel */
#define BRCM_SWARCH_CCU_ENABLE_INTERRUPT_PROC             (0x8136U)    /**< @brief #CCU_EnableInterrupt */
#define BRCM_SWARCH_CCU_CLEAR_INTERRUPT_PROC              (0x8137U)    /**< @brief #CCU_ClearInterrupt */
#define BRCM_SWARCH_CCU_GET_INTERRUPT_STATUS_PROC         (0x8138U)    /**< @brief #CCU_GetInterruptStatus */


/** @} */

/**
    @brief CCU controller HW index
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_HwIDType;
#define CCU_HWID_0  (0UL)  /**< CCU controller index 0 */
#define CCU_HWID_1  (1UL)  /**< CCU controller index 1 */

/**
    @name CCU_ChanType
    @{
    @brief CCU channel type

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_ChanType;           /**< @brief CCU channel type */
#define CCU_CHAN_0       (0UL)           /**< @brief CCU channel 0 ID */
#define CCU_CHAN_1       (1UL)           /**< @brief CCU channel 1 ID */
#define CCU_CHAN_2       (2UL)           /**< @brief CCU channel 2 ID */
#define CCU_CHAN_3       (3UL)           /**< @brief CCU channel 3 ID */
#define CCU_CHAN_MAX     (CCU_CHAN_3)    /**< @brief CCU channel ID max */
/** @} */

/**
    @name CCU_ChanModeType
    @{
    @brief CCU channel mode type

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_ChanModeType;      /**< @brief CCU channel mode type */
#define CCU_CHANMODE_CAPTURE   (1UL)    /**< @brief CCU channel mode: capture */
#define CCU_CHANMODE_COMPARE   (2UL)    /**< @brief CCU channel mode: compare */
#define CCU_CHANMODE_NONE      (3UL)    /**< @brief CCU channel mode: none */
/** @} */

/**
    @name CCU_OutPinSelType
    @{
    @brief CCU output pin select

    This type is used to select the compare out pin.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_OutPinSelType;   /**< @brief CCU output pin select type */
#define CCU_OUTPINSEL_OUT     (1UL)   /**< @brief CCU out pin select OUTn */
#define CCU_OUTPINSEL_OUT_N   (2UL)   /**< @brief CCU out pin select OUTn_n */
/** @} */

/**
    @name CCU_PinActionType
    @{
    @brief CCU pin action

    This type is used to define the action to be taken on compare out pin on
    compare match.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_PinActionType;    /**< @brief CCU pin action type */
#define CCU_PINACTION_SET_LOW  (1UL)   /**< @brief Set out pin to LOW level */
#define CCU_PINACTION_SET_HIGH (2UL)   /**< @brief Set out pin to HIGH level */
#define CCU_PINACTION_TOGGLE   (3UL)   /**< @brief Toggle out pin level. <b>
                                             This pin action is not supported.
                                             </b> */
#define CCU_PINACTION_DISABLE  (4UL)   /**< @brief Disable change on out pin
                                             level */
/** @} */

/**
    @name CCU_InPinSelType
    @{
    @brief CCU input pin select type

    This type is used to select the capture input pin.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_InPinSelType;    /**< @brief CCU input pin select type */
#define CCU_INPINSEL_IN0      (0UL)   /**< @brief CCU input pin select IN0 */
#define CCU_INPINSEL_IN1      (1UL)   /**< @brief CCU input pin select IN1 */
#define CCU_INPINSEL_IN2      (2UL)   /**< @brief CCU input pin select IN2 */
#define CCU_INPINSEL_IN3      (3UL)   /**< @brief CCU input pin select IN3 */
#define CCU_INPINSEL_ETR      (4UL)   /**< @brief CCU input pin select ETR (Used for external event capture) */
#define CCU_INPINSEL_MAX      (4UL)   /**< @brief CCU input pin select MAX */
/** @} */

/**
    @name CCU_InputStateType
    @{
    @brief CCU input state type

    This type is used to describe capture channel input state.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_InputStateType;   /**< @brief CCU input state type */
#define CCU_INPUTSTATE_ACTIVE  (1UL)   /**< @brief CCU input state ACTIVE: an
                                            activation edge is detected */
#define CCU_INPUTSTATE_IDLE    (2UL)   /**< @brief CCU input state IDLE: no
                                            activation edge detected since last
                                            call of #CCU_DrvGetCapInputState or
                                            #CCU_Init */
/** @} */

/**
    @name CCU_EdgeType
    @{
    @brief CCU edge type

    This type is used to describe signal edges.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_EdgeType;      /**< @brief CCU edge type */
#define CCU_EDGE_RISING     (1UL)   /**< @brief CCU signal rising edge */
#define CCU_EDGE_FALLING    (2UL)   /**< @brief CCU signal falling edge */
#define CCU_EDGE_BOTH       (3UL)   /**< @brief CCU signal both edge */
/** @} */

/**
    @name CCU_CombOutSelType
    @{
    @brief CCU compare combined output selection type

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_CombOutSelType; /**< @brief CCU compare combined output selection type for the selected channel. */
#define CCU_COMB_OUT_SEL_0 (0UL)   /**< @brief CCU compare combined output 0 */
#define CCU_COMB_OUT_SEL_1 (1UL)   /**< @brief CCU compare combined output 1 */
#define CCU_COMB_OUT_SEL_2 (2UL)   /**< @brief CCU compare combined output 2 */
#define CCU_COMB_OUT_SEL_3 (3UL)   /**< @brief CCU compare combined output 3 */
/** @} */

/**
    @name CCU_InputFilterType
    @{
    @brief CCU input filter control type
    Enable input filter or synchronizer on input pins.
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_InputFilterType; /**< @brief CCU input filter type selection. */
#define CCU_INPUT_FILTER_NORMAL (0UL)   /**< @brief Enable input filter on input pins. */
#define CCU_INPUT_FILTER_SYNC   (1UL)   /**< @brief Enable synchronizer on input pins.  */
/** @} */

/**
    @name CCU_EventTrigSelType
    @{
    @brief CCU event trigger select type

    This type is used to select event trigger mask. By default events are masked.
    To enable event trigger, event need to be unmasked.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_EventTrigSelMaskType;    /**< @brief CCU event trigger select type */
#define CCU_EVENT_TRIG_SEL_MASK_EVENT0        (1UL << 0UL)     /**< @brief CCU Up-Down Counter time event0 trigger mask.  */
#define CCU_EVENT_TRIG_SEL_MASK_EVENT1        (1UL << 1UL)     /**< @brief CCU Up-Down Counter time event1 trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_EVENT2        (1UL << 2UL)     /**< @brief CCU Up-Down Counter time event2 trigger mask.  */
#define CCU_EVENT_TRIG_SEL_MASK_EVENT3        (1UL << 3UL)     /**< @brief CCU Up-Down Counter time event3 trigger mask.  */
#define CCU_EVENT_TRIG_SEL_MASK_ETR_RE        (1UL << 4UL)     /**< @brief CCU external event rising edge trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_ETR_FE        (1UL << 5UL)     /**< @brief CCU external event falling edge trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN0_RE    (1UL << 6UL)   /**< @brief Prescalar capture input pin 0 event on rising edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN0_FE    (1UL << 7UL)   /**< @brief Prescalar capture input pin 0 event on falling edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN1_RE    (1UL << 8UL)   /**< @brief Prescalar capture input pin 1 event on rising edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN1_FE    (1UL << 9UL)   /**< @brief Prescalar capture input pin 1 event on falling edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN2_RE    (1UL << 10UL)  /**< @brief Prescalar capture input pin 2 event on rising edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN2_FE    (1UL << 11UL)  /**< @brief Prescalar capture input pin 2 event on falling edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN3_RE    (1UL << 12UL)  /**< @brief Prescalar capture input pin 3 event on rising edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_PSC_IN3_FE    (1UL << 13UL)  /**< @brief Prescalar capture input pin 3 event on falling edge trigger on the prescalar. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT0          (1UL << 14UL)     /**< @brief CCT_OUT0 event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT0_N         (1UL << 15UL)     /**< @brief CCT_OUT0n event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT1          (1UL << 16UL)     /**< @brief CCT_OUT1 event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT1_N         (1UL << 17UL)     /**< @brief CCT_OUT1n event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT2          (1UL << 18UL)     /**< @brief CCT_OUT2 event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT2_N         (1UL << 19UL)     /**< @brief CCT_OUT2n event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT3          (1UL << 20UL)     /**< @brief CCT_OUT3 event trigger mask. */
#define CCU_EVENT_TRIG_SEL_MASK_OUT3_N         (1UL << 21UL)     /**< @brief CCT_OUT3n event trigger mask. */
/** @} */

/**
    @name CCU_InterruptType
    @{
    @brief CCU Interrupt Type

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
 */
typedef uint32_t CCU_InterruptType; /**< @brief Select CCU interrupt type. */
#define CCU_INTERRUPT_CCR_A_IRQ_EN              (1UL<<0UL) /**< @brief Capture/Compare Register sub-channel A IRQ Enable for the selected channel (0-3). */
#define CCU_INTERRUPT_CCR_B_IRQ_EN              (1UL<<1UL) /**< @brief Capture/Compare Register sub-channel B IRQ Enable for the selected channel (0-3). */
#define CCU_INTERRUPT_UDC_IRQ_EN                (1UL<<2UL) /**< @brief Up-Down Counter IRQ Enable. */
#define CCU_INTERRUPT_ETR_RE_IRQ_EN             (1UL<<3UL) /**< @brief External Event Rising Edge IRQ Enable. */
#define CCU_INTERRUPT_ETR_FE_IRQ_EN             (1UL<<4UL) /**< @brief External Event Falling Edge IRQ Enable. */
#define CCU_INTERRUPT_ALL                       (0x7FFUL)  /**< @brief Select all interrupt */
/** @} */


/**
    @name CCU_ValueType
    @{
    @brief CCU value type

    This type is used to describe capture timestamp ticks, absolute threshold
    for compare etc.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint16_t CCU_ValueType;     /**< @brief CCU value type */
/** @} */

/**
    @name CCU_TSIndexType
    @{
    @brief CCU timestamp index type

    This type is used to describe timestamp buffer index.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_TSIndexType;   /**< @brief CCU timestamp index type */
/** @} */

/**
    @name CCU_EdgeNumberType
    @{
    @brief CCU edge number type

    This type is used to describe number of capture signal edge detected in case
    of capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_EdgeNumberType;    /**< @brief CCU edge number type */
/** @} */

/**
    @name CCU_CapModeType
    @{
    @brief CCU capture mode

    This type is used to describe mode of capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_CapModeType;          /**< @brief CCU capture mode
                                                 type */
#define CCU_CAPMODE_NONE           (0UL)   /**< @brief No capture mode */
#define CCU_CAPMODE_TIMESTAMP      (1UL)   /**< @brief Timestamp capture
                                                 mode */
#define CCU_CAPMODE_EDGE_COUNT     (2UL)   /**< @brief Edge count capture
                                                 mode */
#define CCU_CAPMODE_EDGE_DETECT    (3UL)   /**< @brief Edge detect capture
                                                 mode */
#define CCU_CAPMODE_SIGNAL_MEASURE (4UL)   /**< @brief Signal measure capture
                                                 mode */
/** @} */

/**
    @name CCU_SignalPropertyType
    @{
    @brief CCU signal property type

    This type is used to describe signal property in case of
    #CCU_CAPMODE_SIGNAL_MEASURE mode of capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_SignalPropertyType;       /**< @brief CCU signal property
                                                    type */
#define CCU_SIGNALPROPERTY_LOW_TIME    (1UL)   /**< @brief Signal property LOW
                                                    time */
#define CCU_SIGNALPROPERTY_HIGH_TIME   (2UL)   /**< @brief Signal property HIGH
                                                    time */
#define CCU_SIGNALPROPERTY_PERIOD_TIME (3UL)   /**< @brief Signal property period
                                                    time */
#define CCU_SIGNALPROPERTY_DUTY_CYCLE  (4UL)   /**< @brief Signal property duty
                                                    cycle */
/** @} */

/**
    @name CCU_TimeStampBufType
    @{
    @brief CCU timestamp buffer type

    This type is used to describe type of timestamp buffer i.e. linear,
    circular.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_TimeStampBufType;        /**< @brief CCU timestamp buffer type */
#define CCU_TIMESTAMPBUF_LINEAR       (1UL)   /**< @brief Timestamp buffer linear,
                                                   shall be filled once */
#define CCU_TIMESTAMPBUF_CIRCULAR     (2UL)   /**< @brief Timestamp buffer circular,
                                                   shall be filled in circular
                                                   order */
/** @} */

/** @brief CCU callback type

    This type is used to describe function pointers for callback functions.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU_CONFIGURATION

    @limitations Callback functions are invoked from interrupt context. Callback
    functions shall be implemented to have minimum execution time.
*/
typedef void (*CCU_CallbackType) (void);

/**
    @brief Configuration for capture channel

    Type to describe configuration for capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_ChanCapCfgType {
    CCU_CallbackType timestampCB; /**< @brief Function pointer for
                                             timestamp callback. This callback
                                             function is invoked by driver in
                                             #CCU_CAPMODE_TIMESTAMP mode based
                                             on the notify interval mentioned
                                             during #CCU_DrvStartCapTimestamp and
                                             when callback is enabled by calling
                                             #CCU_DrvEnableCapCallback. */
    CCU_CallbackType edgeDetectCB;/**< @brief Function pointer for
                                             edge-detect callback. This callback
                                             function is invoked by driver on
                                             detection of expected edge when
                                             edge-detect mode and capture
                                             callback are enabled by calling
                                             #CCU_DrvEnableCapEdgeDetection and
                                             #CCU_DrvEnableCapCallback
                                             respectively. */
    CCU_InPinSelType pin;               /**< @brief Default input pin for the
                                             channel */
    CCU_EdgeType edge;                  /**< @brief Default capture edge for the
                                             channel */
    CCU_CapModeType mode;               /**< @brief Capture mode for the
                                             channel */
    CCU_TimeStampBufType TSBufProp;     /**< @brief Type of timestamp buffer for
                                             the channel. This is used in case
                                             of #CCU_CAPMODE_TIMESTAMP capture
                                             mode. */
    CCU_SignalPropertyType sigMeasProp; /**< @brief Signal measurement property
                                             for the channel. This parameter is
                                             to be used in case of
                                             #CCU_CAPMODE_SIGNAL_MEASURE
                                             capture mode. */
    uint8_t swTriggerEn;  /**< @brief Software event trigger for capture event, if hw event trigger is needed sw trigger need not to be enabled. */
} CCU_ChanCapCfgType;

/**
    @brief Configuration for compare channel

    Type to describe configuration for compare channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_ChanCompCfgType {
    CCU_CallbackType compCB;  /**< @brief Function pointer for compare
                                         callback. This function is invoked by
                                         driver on compare match, if callback is
                                         enabled by calling
                                         #CCU_DrvEnableCompCallback */
    CCU_ValueType compThreshold;    /**< @brief Default compare threshold for
                                         the channel */
    uint8_t swTriggerEn;   /**< @brief Hardware enable control is overridden and updown counter is always enabled. This control is needed when sw trigger
                                                         is required to start UDC counters. */
    uint8_t xorOutputEn;  /**< @brief Enable XOR output of channel for sub channel A and B, send to selected output line. */
} CCU_ChanCompCfgType;

/**
    @brief Channel configuration table

    Type to describe channel configuration table.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_ChanCfgTblType {
    CCU_ChanType chanNum;       /**< @brief Channel number */
    CCU_ChanModeType chanMode;  /**< @brief Mode of the channel */
    CCU_ChanCapCfgType capCfg;  /**< @brief Capture channel configuration. This
                                     field shall be updated appropriately in
                                     case of chanMode is
                                     #CCU_CHANMODE_CAPTURE. */
    CCU_ChanCompCfgType compCfg;/**< @brief Compare channel configuration. This
                                     field shall be updated appropriately in
                                     case of chanMode is
                                     #CCU_CHANMODE_COMPARE. */
} CCU_ChanCfgTblType;

/**
    @brief CCU driver configuration

    Type to describe CCU driver configuration.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_ConfigType {
    uint16_t cntrMaxValue;              /**< @brief CCU counter’s maximum value.
                                             CCU counter starts from 0 and
                                             increment till maximum value before
                                             wrapping back to 0. Functionality
                                             of all the capture and compare
                                             channel is based on single CCU
                                             counter. */
    uint16_t preScaleRatio;             /**< @brief Pre scale ratio for clock
                                            used to increment CCU counter. This
                                            ratio is used to decrease frequency
                                            of clock feed to CCU counter. i.e.
                                            - 0: Clock frequency division by 1
                                            - 1: Clock frequency division by 2
                                            - 2: Clock frequency division by 3
                                            - And so on... */
    CCU_ChanCfgTblType * const chanCfgTbl;  /**< @brief Pointer to channel
                                                 configuration table */
    uint32_t chanCfgTblSz;                  /**< @brief Size of the channel
                                                 configuration table */
} CCU_ConfigType;

/**
    @brief Signal duty cycle

    Type to describe capture signal duty-cycle for capture channel in signal
    measurement mode.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_DutyCycleType {
    CCU_ValueType activeTime;   /**< @brief Active time of the signal in terms
                                     of CCU counter ticks. Active time means
                                     time duration in ticks for which input
                                     signal is at level HIGH within a period. */
    CCU_ValueType periodTime;   /**< @brief Period of the signal in terms of
                                     CCU counter ticks. Period is the time
                                     duration in ticks between two consecutive
                                     rising edges of input signal. */
} CCU_DutyCycleType;

/** @brief Initialize CCU driver

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aConfig     Pointer to CCU driver configuration

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK             success
    @retval         #BCM_ERR_INVAL_PARAMS   aConfig in NULL
    @retval         #BCM_ERR_INVAL_PARAMS   Some configuration in aConfig is not correct


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvInit(CCU_HwIDType aID, const CCU_ConfigType *const aConfig);

/** @brief De-initialize CCU driver

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         CCU controller index

    @return     int32_t

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvDeInit(CCU_HwIDType aID);

/** @brief Retrieve mode of the channel

    API to retrieve mode of the channel.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number for which mode is queried
    @param[out]     aChanMode   Mode of the channel

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK             success
    @retval         #BCM_ERR_INVAL_PARAMS   Invalid input parameter

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvGetChanMode(CCU_HwIDType aID, CCU_ChanType aChanNum,
                            CCU_ChanModeType *const aChanMode);

/** @brief Start compare channel

    API to start compare channel. This API changes channel state from stopped to
    running. This API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvStartCompChannel(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Stop compare channel

    API to stop compare channel. This API changes channel state from running to
    stopped. This API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvStopCompChannel(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Set compare out pin properties

    API to set compare out pin properties. This API shall be called when channel
    is configured in #CCU_CHANMODE_COMPARE mode and channel is not in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number
    @param[in]      aPinAction  Action on compare output pin on compare match
    @param[in]      aOutPin     Compare output pin selected

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvSetCompPinProperty(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                   CCU_PinActionType aPinAction,
                                   CCU_OutPinSelType aOutPin);

/** @brief Set absolute threshold for compare match

    API to set absolute threshold for compare match. This API shall be called
    when channel is configured in #CCU_CHANMODE_COMPARE mode and channel is not
    in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number
    @param[in]      aAbsValue   Absolute threshold value for compare match

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvSetCompAbsThreshold(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                    CCU_ValueType aAbsValue);

/** @brief Enable compare callback

    API to enable compare callback. If compare callback is enabled,
    CCU_ChanCompCfgType::compCB is non-null in compare channel config and
    compare channel is started by #CCU_DrvStartCompChannel, then
    CCU_ChanCompCfgType::compCB shall be invoked for each compare match. This
    API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode and channel is not in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvEnableCompCallback(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Disable compare callback

    API to disable compare callback. If compare callback is disabled then
    CCU_ChanCompCfgType::compCB shall not be invoked for each compare match on
    the channel. This API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode and channel is not in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvDisableCompCallback(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Set capture signal properties

    API to set capture signal properties. This API shall be called when channel
    is configured in #CCU_CHANMODE_CAPTURE mode and channel is not in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number
    @param[in]      aInPin      Input pin selected for the capture channel
    @param[in]      aEdge       Capture edge for the channel

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvSetCapSignalProp(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                 CCU_InPinSelType aInPin,
                                 CCU_EdgeType aEdge);

/** @brief Enable capture callback

    API to enable capture callback. If the capture mode selected is
    #CCU_CAPMODE_EDGE_DETECT, callback is enabled and
    CCU_ChanCapCfgType::edgeDetectCB is non-null, then
    CCU_ChanCapCfgType::edgeDetectCB shall be invoked on each edge.

    If the capture mode selected is #CCU_CAPMODE_TIMESTAMP, callback is enabled
    and CCU_ChanCapCfgType::timestampCB is non-null, then
    CCU_ChanCapCfgType::timestampCB shall be invoked after each notify interval.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode and channel is not in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvEnableCapCallback(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Disable capture callback

    API to disable capture callback. This API shall be called when channel is
    configured in #CCU_CHANMODE_CAPTURE mode and channel is not in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvDisableCapCallback(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Retrieve input state of the capture channel

    API to retrieve input state of the capture channel. This API shall return
    input state as #CCU_INPUTSTATE_IDLE, if capture edge not detected after
    starting of capture or after last call to #CCU_DrvGetCapInputState. This API
    shall return input state as #CCU_INPUTSTATE_ACTIVE, if capture edge is
    detected after starting of capture or after last call to
    #CCU_DrvGetCapInputState.

    In case of signal measurement, this API shall return input state as
    #CCU_INPUTSTATE_IDLE if signal measure is not complete and this API shall
    return input state as #CCU_INPUTSTATE_ACTIVE if signal measurement is
    complete.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number
    @param[out]     aInputState Retrieve state of the capture channel

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvGetCapInputState(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                       CCU_InputStateType *const aInputState);

/** @brief Start capture channel time stamping

    API to start capture channel time stamping.

    On starting of capture channel for time stamping, channel shall be moved
    from stopped to running state and time stamp shall be captured on detection
    of each configured edges.

    Time stamping can be stopped by explicitly calling #CCU_StopCapTimestamp or
    when @a aTSBuf is of linear type and it is completely filled.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode and channel capture mode is
    #CCU_CAPMODE_TIMESTAMP.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number
    @param[out]     aTSBuf          Pointer to timestamp buffer
    @param[in]      aBufSize        Size of timestamp buffer in terms of
                                    #CCU_ValueType
    @param[in]      aNotifyInterval Interval after which
                                    CCU_ChanCapCfgType::timestampCB shall be
                                    invoked (if callback is enabled and @a
                                    aNotifyInterval is greater than 0). @a
                                    aNotifyInterval is in terms of number of
                                    timestamp captured.

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvStartCapTimestamp(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                  CCU_ValueType *const aTSBuf,
                                  uint32_t aBufSize,
                                  uint32_t aNotifyInterval);

/** @brief Stop capture channel time stamping

    API to stop capture channel time stamping.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode and channel
    capture mode is #CCU_CAPMODE_TIMESTAMP.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

     Return values are documented in reverse-chronological order
     @retval         #BCM_ERR_OK                    Success
     @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
     @retval         #BCM_ERR_INVAL_STATE    Invalid status.
     @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvStopCapTimestamp(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Retrieve capture channel timestamp index

    API to retrieve capture channel timestamp index.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is #CCU_CAPMODE_TIMESTAMP
    and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number
    @param[out]     aTSIndex        Retrieve capture channel timestamp index

    @retval     #CCU_IndexType          Index of next timestamp to be written in
                                        the timestamp buffer.

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvGetCapTimestampIndex(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                           CCU_TSIndexType *const aTSIndex);

/** @brief Reset the capture edge counter

    API to reset the capture edge counter.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_EDGE_COUNT and channel is in stopped state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvResetCapEdgeCount(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Enable capture edge counter

    API to enable capture edge counter.

    This API changes channel from stopped to running state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_COUNT and channel is in stopped state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvEnableCapEdgeCount(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Disable capture edge counter

    API to disable capture edge counter.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_COUNT and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvDisableCapEdgeCount(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Enable capture edge detection

    API to enable capture edge detection.

    This API changes channel from stopped to running state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_DETECT and channel is in stopped state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvEnableCapEdgeDetection(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Disable capture edge detection

    API to disable capture edge detection.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_DETECT and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNINIT              Not initialized

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvDisableCapEdgeDetection(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Retrieve capture edge number

    API to retrieve capture edge number.
    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_EDGE_COUNT and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number
    @param[out]     aEdgeNum        Retrieve capture edge number

       Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvGetCapEdgeNumbers(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                         CCU_EdgeNumberType *const aEdgeNum);

/** @brief Start capture signal measurement

    API to start capture signal measurement.
    This API changes channel from stopped to running state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_SIGNAL_MEASURE and channel is in stopped
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

       Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvStartCapSignalMeasurement(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Stop capture signal measurement

    API to stop capture signal measurement.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_SIGNAL_MEASURE and channel is in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

     Return values are documented in reverse-chronological order
     @retval        #BCM_ERR_OK                    Success
     @retval        #BCM_ERR_INVAL_PARAMS  Invalid parameter

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvStopCapSignalMeasurement(CCU_HwIDType aID, CCU_ChanType aChanNum);

/** @brief Retrieve capture time elapsed

    API to retrieve capture time elapsed.

    Elapsed time shall be 0 in case signal measurement is in-progress or channel
    input state is #CCU_INPUTSTATE_IDLE. This API shall be called when channel
    is configured in #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_SIGNAL_MEASURE, signal measure property is
    #CCU_SIGNALPROPERTY_LOW_TIME / #CCU_SIGNALPROPERTY_HIGH_TIME /
    #CCU_SIGNALPROPERTY_PERIOD_TIME and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number
    @param[out]     aTimeElapsed    Retrieve capture time elapsed

    Return values are documented in reverse-chronological order
    @retval     #CCU_ValueType          Elapsed time for channel after
                                        #CCU_DrvStartCapSignalMeasurementor after
                                        last #CCU_DrvGetCapTimeElapsed. Elapsed
                                        time refers to:
                                        - Low time, if signal property is
                                          #CCU_SIGNALPROPERTY_LOW_TIME
                                        - High time, if signal property is
                                          #CCU_SIGNALPROPERTY_HIGH_TIME
                                        - Period time, if signal property is
                                          #CCU_SIGNALPROPERTY_PERIOD_TIME

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvGetCapTimeElapsed(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                         CCU_ValueType *const aTimeElapsed);

/** @brief Retrieve capture channel duty cycle values

    API to retrieve capture channel duty cycle values.

    This API provides CCU_DutyCycleType::activeTime and
    CCU_DutyCycleType::periodTime for concurrent interval for single duty
    cycle value. Duty cycle, CCU_DutyCycleType::activeTime and
    CCU_DutyCycleType::periodTime are returned as 0 in case these are not
    captured for concurrent interval.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_SIGNAL_MEASURE, signal measure property is
    #CCU_SIGNALPROPERTY_DUTY_CYCLE and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD
    @param[in]      aID             CCU controller index
    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval        #BCM_ERR_OK                    Success
    @retval        #BCM_ERR_INVAL_PARAMS  Invalid parameter


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvGetCapDutyCycleValues(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                      CCU_DutyCycleType *const aDutyCycle);

/** @brief Set Compare Output Combine selection.

    API to select combined output for the selected channel. This API shall be called
    when channel is configured in #CCU_CHANMODE_COMPARE mode and channel is not
    in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID         CCU controller index
    @param[in]      aChanNum    Channel number
    @param[in]      outSel   Output selection

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvCombOutputSel(CCU_HwIDType aID, CCU_ChanType aChanNum, CCU_CombOutSelType outSel);

/** @brief Input Filter Control.

    API to select input filter control for the selected input pins.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID            CCU controller index
    @param[in]      aInputPin    Input pin selection
    @param[in]      aFilterType  Select filter type
    @param[in]      aEnable       Enable/disable selected filter on the selected input pin.

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvInputFilterCtrlSet(CCU_HwIDType aID, CCU_InPinSelType aInputPin, CCU_InputFilterType aFilterType, uint32_t aEnable);

/** @brief CCU event trigger selection.

    API to select event trigger for generating trigger. If event is enabled, it triggers interrupt otherwise
    events are masked by default.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aID               CCU controller index.
    @param[in]      eventTrigSel  Event trigger selection.
    @param[in]      aEnable         Enable/disable selected event trigger.

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter
    @retval         #BCM_ERR_INVAL_STATE    Invalid status.
    @retval         #BCM_ERR_UNKNOWN         Integration error


    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern int32_t CCU_DrvEventTriggerSel(CCU_HwIDType aID, CCU_EventTrigSelMaskType eventTrigSel, uint32_t aEnable);

/** @brief Enable the CCU interrupt

    This API enables the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId            CCU controller index.
    @param[in]      intrType      Interrupt type
    @param[in]      aChanNum  Channel number
    @param[in]      aEnable       Enable or disable the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter

    @post None

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
int CCU_EnableInterrupt(CCU_HwIDType aId, CCU_InterruptType intrType, CCU_ChanType aChanNum, uint32_t aEnable);


/** @brief Clear the interrupt

    This API clears the interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId             CCU controller index.

    Return values are documented in reverse-chronological order

    @retval         #BCM_ERR_OK                    Success
    @retval         #BCM_ERR_INVAL_PARAMS  Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_CCU

    @limitations None
*/
int CCU_ClearInterrupt(CCU_HwIDType aId);

/** @brief Get the interrupt status

    This API gets the interrupt status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId             CCU controller index.
    @param[out]    intrStatus    Get the interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_CCU

    @limitations None
*/
int CCU_GetInterruptStatus(CCU_HwIDType aId, uint32_t *intrStatus);

/** @brief  CCU IRQ Handler

  This API is called when capture/compare interrupt is asserted

  @param[in]    aId       Index of the CCU controller

  @behavior Async, Non-reentrant

  @pre None

  @return       void
  @post None

    @trace #BRCM_SWREQ_CCU_KERNEL_HANDLER

  @limitations None
 */
void CCU_DrvIRQHandler(CCU_HwIDType aID);

#endif /* CCU_H */
/** @} */

