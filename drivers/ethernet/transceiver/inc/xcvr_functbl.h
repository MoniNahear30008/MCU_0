/*****************************************************************************
 Copyright 2022-2023 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_xcvr_functbl_ifc XCVR function table Interface
    @ingroup grp_transceiverdrv

    @addtogroup grp_xcvr_functbl_ifc
    @{

    @file ethxcvr_functbl.h
    @brief Ethernet Transceiver Driver Integration Interfaces
    This header file contains the integration interface for Ethernet Transceiver Driver

    @version 0.01 draft
*/

#ifndef XCVR_FUNCTBL_H
#define XCVR_FUNCTBL_H

#include <xcvr_common.h>

/**
    @name Ethernet Transceiver Driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for Ethernet Transceiver Driver
*/
#define BRCM_SWARCH_ETHXCVR_PHYFUNCTBL_TYPE                   (0x8230U) /**< @brief #ETHXCVR_FuncTblType                      */
#define BRCM_SWARCH_ETHXCVR_INIT_TYPE                         (0x8231U) /**< @brief #ETHXCVR_InitType                         */
#define BRCM_SWARCH_ETHXCVR_RESET_TYPE                        (0x8232U) /**< @brief #ETHXCVR_ResetType                        */
#define BRCM_SWARCH_ETHXCVR_SETMODE_TYPE                      (0x8233U) /**< @brief #ETHXCVR_SetModeType                      */
#define BRCM_SWARCH_ETHXCVR_GETMODE_TYPE                      (0x8234U) /**< @brief #ETHXCVR_GetModeType                      */
#define BRCM_SWARCH_ETHXCVR_SETMASTERMODE_TYPE                (0x8235U) /**< @brief #ETHXCVR_SetMasterModeType                */
#define BRCM_SWARCH_ETHXCVR_GETMASTERMODE_TYPE                (0x8236U) /**< @brief #ETHXCVR_GetMasterModeType                */
#define BRCM_SWARCH_ETHXCVR_GETSPEED_TYPE                     (0x8237U) /**< @brief #ETHXCVR_GetSpeedType                     */
#define BRCM_SWARCH_ETHXCVR_SETSPEED_TYPE                     (0x8238U) /**< @brief #ETHXCVR_SetSpeedType                     */
#define BRCM_SWARCH_ETHXCVR_GETDUPLEXMODE_TYPE                (0x8239U) /**< @brief #ETHXCVR_GetDuplexModeType                */
#define BRCM_SWARCH_ETHXCVR_SETDUPLEXMODE_TYPE                (0x823AU) /**< @brief #ETHXCVR_SetDuplexModeType                */
#define BRCM_SWARCH_ETHXCVR_GETFLOWCONTROL_TYPE               (0x823BU) /**< @brief #ETHXCVR_GetFlowControlType               */
#define BRCM_SWARCH_ETHXCVR_SETFLOWCONTROL_TYPE               (0x823CU) /**< @brief #ETHXCVR_SetFlowControlType               */
#define BRCM_SWARCH_ETHXCVR_GETLINKSTATE_TYPE                 (0x823DU) /**< @brief #ETHXCVR_GetLinkStateType                 */
#define BRCM_SWARCH_ETHXCVR_GETSQIVALUE_TYPE                  (0x823EU) /**< @brief #ETHXCVR_GetSQIValueType                  */
#define BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_TYPE               (0x823FU) /**< @brief #ETHXCVR_IRQHandlerType                   */
#define BRCM_SWARCH_ETHXCVR_LINKCHANGEINDHANDLER_TYPE         (0x8240U) /**< @brief #ETHXCVR_LinkChangeIndHandlerType         */
#define BRCM_SWARCH_ETHXCVR_SETLOOPBACKMODE_TYPE              (0x8241U) /**< @brief #ETHXCVR_SetLoopbackModeType              */
#define BRCM_SWARCH_ETHXCVR_GETLOOPBACKMODE_TYPE              (0x8242U) /**< @brief #ETHXCVR_GetLoopbackModeType              */
#define BRCM_SWARCH_ETHXCVR_SETJUMBOMODE_TYPE                 (0x8243U) /**< @brief #ETHXCVR_SetJumboModeType                 */
#define BRCM_SWARCH_ETHXCVR_GETJUMBOMODE_TYPE                 (0x8244U) /**< @brief #ETHXCVR_GetJumboModeType                 */
#define BRCM_SWARCH_ETHXCVR_SETAUTONEGMODE_TYPE               (0x8246U) /**< @brief #ETHXCVR_SetAutoNegModeType               */
#define BRCM_SWARCH_ETHXCVR_GETAUTONEGSTATUS_TYPE             (0x8247U) /**< @brief #ETHXCVR_GetAutoNegStatusType             */
#define BRCM_SWARCH_ETHXCVR_RESTARTAUTONEG_TYPE               (0x8248U) /**< @brief #ETHXCVR_RestartAutoNegType               */
#define BRCM_SWARCH_ETHXCVR_GETSTATS_TYPE                     (0x8249U) /**< @brief #ETHXCVR_GetStatsType                     */
#define BRCM_SWARCH_ETHXCVR_STATEHANDLER_TYPE                 (0x824AU) /**< @brief #ETHXCVR_StateHandlerType                 */
#define BRCM_SWARCH_ETHXCVR_UPDATEHWSTATUS_TYPE               (0x824BU) /**< @brief #ETHXCVR_UpdateHWStatusType               */
#define BRCM_SWARCH_ETHXCVR_DEINIT_TYPE                       (0x824CU) /**< @brief #ETHXCVR_DeInitType                       */
#define BRCM_SWARCH_ETHXCVR_PHYCACHEDINFO_TYPE                (0x824DU) /**< @brief #ETHXCVR_PhyCachedInfoType                */
#define BRCM_SWARCH_ETHXCVR_PHYCNTXT_TYPE                     (0x824EU) /**< @brief #ETHXCVR_CntxtType                        */
#define BRCM_SWARCH_ETHXCVR_GETBASEADDR_TYPE                  (0x824FU) /**< @brief #ETHXCVR_GetBaseAddrType                  */
#define BRCM_SWARCH_ETHXCVR_RUNCABLEDIAG_TYPE                 (0x8250U) /**< @brief #ETHXCVR_RunCableDiagType                 */
#define BRCM_SWARCH_S16UP_FUNCTBLE_TYPE                       (0x8251U) /**< @brief #S16UP_FuncTble                           */
#define BRCM_SWARCH_BRPHY1GV2_FUNCTBL_TYPE                    (0x8252U) /**< @brief #BRPHY1GV2_FuncTbl                        */
#define BRCM_SWARCH_BRPHY1GV2_TC10FUNCTBL_TYPE                (0x8253U) /**< @brief #BRPHY1GV2_TC10FuncTbl                    */

#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10GETSTATUS_TYPE          (0x8254U) /**< @brief #ETHXCVR_ExtPhyTC10GetStatusType          */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10WAKEUPREQ_TYPE          (0x8255U) /**< @brief #ETHXCVR_ExtPhyTC10WakeUpReqType          */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10SLEEPREQ_TYPE           (0x8256U) /**< @brief #ETHXCVR_ExtPhyTC10SleepReqType           */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10FORCESLEEPREQ_TYPE      (0x8257U) /**< @brief #ETHXCVR_ExtPhyTC10ForceSleepReqType      */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10SLEEPABORTREQ_TYPE      (0x8258U) /**< @brief #ETHXCVR_ExtPhyTC10SleepAbortReqType      */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10INIT_TYPE               (0x8259U) /**< @brief #ETHXCVR_ExtPhyTC10InitType               */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10FUNCTBL_TYPE            (0x825AU) /**< @brief #ETHXCVR_ExtPhyTC10FuncTblType            */

#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10GETSTATS_TYPE           (0x825CU) /**< @brief #ETHXCVR_ExtPhyTC10GetStatsType           */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10GETWAKEUPREASON_TYPE    (0x825DU) /**< @brief #ETHXCVR_ExtPhyTC10GetWakeUpReasonType    */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10ISSUPPORTED_TYPE        (0x825EU) /**< @brief #ETHXCVR_ExtPhyTC10IsSupportedType        */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10DISABLEEVENTS_TYPE      (0x825FU) /**< @brief #ETHXCVR_ExtPhyTC10DisableEventsType      */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10ENABLEEVENTS_TYPE       (0x8260U) /**< @brief #ETHXCVR_ExtPhyTC10EnableEventsType       */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10GETEVENTMASK_TYPE       (0x8261U) /**< @brief #ETHXCVR_ExtPhyTC10GetEventMaskType   */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10ALLOWSLEEPREQ_TYPE      (0x8262U) /**< @brief #ETHXCVR_ExtPhyTC10AllowSleepReqType      */
#define BRCM_SWARCH_ETHXCVR_EXT_PHY_TC10ALLOW_MDI_WAKE_TYPE   (0x8263U) /**< @brief #ETHXCVR_ExtPhyTC10AllowMdiWakeType       */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10ENABLEPROGRAMSTATE_TYPE (0x8264U) /**< @brief #ETHXCVR_ExtPhyTC10EnableProgramStateType */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10EXITHOSTMODE_TYPE       (0x8265U) /**< @brief #ETHXCVR_ExtPhyTC10ExitHostModeType       */
#define BRCM_SWARCH_ETHXCVR_EXTPHYTC10ENABLE_TYPE             (0x8266U) /**< @brief #ETHXCVR_ExtPhyTC10EnableType             */
#define BRCM_SWARCH_ETHXCVR_PLATFORMGETFNTBLSCNTXT_PROC       (0x8267U) /**< @brief #ETHXCVR_PlatformGetFnTblsCntxt           */
#define BRCM_SWARCH_ETHXCVR_PLATFORMGETTC10FNTBLSCNTXT_PROC   (0x8268U) /**< @brief #ETHXCVR_PlatformGetTc10FnTblsCntxt       */
#define BRCM_SWARCH_ETHXCVR_PHY_BIT_REVERSE_PROC              (0x8269U) /**< @brief #ETHXCVR_PhybitReverse                    */
#define BRCM_SWARCH_ETHXCVR_PHY_GET_ID_TYPE                   (0x826AU) /**< @brief #ETHXCVR_PhyGetIDType                     */
#define BRCM_SWARCH_ETHXCVR_PHY_OUI_MACRO                     (0x826BU) /**< @brief #ETHXCVR_PHY_OUI                          */
#define BRCM_SWARCH_ETHXCVR_PHY_MODEL_MACRO                   (0x826CU) /**< @brief #ETHXCVR_PHY_MODEL                        */
#define BRCM_SWARCH_ETHXCVR_PHY_REV_MACRO                     (0x826DU) /**< @brief #ETHXCVR_PHY_REV                          */

/** @} */

/**
    @brief PHY cached info structure type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_PhyCachedInfoType {
    ETHXCVR_LinkStateType       linkState;      /**< @brief PHY Link state */
    ETHXCVR_ModeType            mode;           /**< @brief PHY Mode */
    ETHXCVR_BooleanType         connMode;       /**< @brief PHY connection mode */
    ETHXCVR_BooleanType         loopbackMode;   /**< @brief PHY loopback mode */
    ETHXCVR_BooleanType         jumboMode;      /**< @brief PHY jumbo mode */
    ETHXCVR_BooleanType         autonegMode;    /**< @brief PHY AutoNeg mode */
    ETHXCVR_SpeedType           speed;          /**< @brief PHY speed */
    ETHXCVR_DuplexModeType      duplexMode;     /**< @brief PHY duplex mode */
    ETHXCVR_FlowControlType     flowControl;    /**< @brief PHY flow control */
    ETHXCVR_ModeType            localModeState; /**< @brief PHY Local Mode State */
    uint16_t                    tc10Status;     /**< @brief PHY Captured TC10 Status*/
    uint32_t                    sqiValue;       /**< @brief PHY SQI value */
    ETHXCVR_StatsType           stats;          /**< @brief PHY stats */
    ETHXCVR_AutoNegStatusType   autonegStatus;  /**< @brief PHY Autoneg status */
} ETHXCVR_PhyCachedInfoType;

/**
    @brief PHY context structure type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_CntxtType {
    ETHXCVR_IDType              id;         /**< @brief PHY identifier */
    ETHXCVR_ModeType            mode;           /**< @brief PHY Mode */
    uint32_t                    data[16UL]; /**< @brief PHY context data */
    ETHXCVR_PhyCachedInfoType   cacheInfo;  /**< @brief PHY cached information */
    BCM_HandleType              connHdl;
} ETHXCVR_CntxtType;

/**
    @brief Function type to initialize

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_InitType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief function type to de-initialize

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_DeInitType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to reset

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ResetType)(uint8_t aId,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to set mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetModeType)(uint8_t aId,
                                   ETHXCVR_CntxtType *const aPhyCntxt,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_ModeType aMode);

/**
    @brief Function type to get mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetModeType)(uint8_t aId,
                                   ETHXCVR_CntxtType *const aPhyCntxt,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_ModeType *const aMode);

/**
    @brief Function type to set master/slave mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetMasterModeType)(uint8_t aId,
                                     ETHXCVR_CntxtType *const aPhyCntxt,
                                     const ETHXCVR_PortConfigType *const aConfig,
                                     ETHXCVR_BooleanType aMode);

/**
    @brief Function type to get master/slave mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetMasterModeType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_BooleanType *const aMode);

/**
    @brief Function type to get speed

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetSpeedType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_SpeedType *const aSpeed);

/**
    @brief Function type to set speed

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetSpeedType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_SpeedType aSpeed);

/**
    @brief Function type to get duplex mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetDuplexModeType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_DuplexModeType *const aDuplexMode);

/**
    @brief Function type to set duplex mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetDuplexModeType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_DuplexModeType aDuplexMode);

/**
    @brief Function type to get flow control

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetFlowControlType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType *const aFlowControl);

/**
    @brief Function type to set flow control

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetFlowControlType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType aFlowControl);

/**
    @brief Function type to get link state

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetLinkStateType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_LinkStateType *const aLinkState);

/**
    @brief Function type to get SQI value

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetSQIValueType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    uint32_t *const aSQIValue);

/**
    @brief Function type to handle link IRQ

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_IRQHandlerType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_IrqParamsType *const aParams);

/**
    @brief Function type to handle link change indication

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_LinkChangeIndHandlerType)(uint8_t aId,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_LinkStateType *const aLinkState,
                                    uint32_t *const aIsLinkStateChanged);

/**
    @brief Function type to set loopback mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetLoopbackModeType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_LoopbackModeType aMode
                                    );

/**
    @brief Function type to get loopback mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetLoopbackModeType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_LoopbackModeType *const aMode);

/**
    @brief Function type to set jumbo mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetJumboModeType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_BooleanType aJumboMode);

/**
    @brief Function type to get jumbo mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetJumboModeType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_BooleanType *const aJumboMode);

/**
    @brief Function type to set auto-negotiation mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_SetAutoNegModeType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_BooleanType aAutoNegMode);

/**
    @brief Function type to get auto-negotiation status

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetAutoNegStatusType)(ETHXCVR_IDType aID,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_AutoNegStatusType *const aAutoNegStatus);

/**
    @brief Function type to restart auto-negotiation

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_RestartAutoNegType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to retrieve statistics

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetStatsType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_StatsType *const aStats);

/**
    @brief Function type for state handler

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_StateHandlerType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    uint32_t *const aIsModeChanged,
                                    ETHXCVR_ModeType *const aMode);

/**
    @brief Function type to update HW status parameters

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_UpdateHWStatusType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to fetch the phy/Bus base address

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_GetBaseAddrType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    uint32_t *const aAddr);
/**
    @brief Function type to execute the cable diagnostics on phy

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_RunCableDiagType)(ETHXCVR_IDType aID,
                                    ETHXCVR_CntxtType *const aPhyCntxt,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    const ETHXCVR_AcdParamsType aParams,
                                    ETHXCVR_CableDiagResultType *const aResult);

/**
    @brief Function type to Get PHY (hardware) identifier

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_PhyGetIDType)(ETHXCVR_IDType aID,
                                           ETHXCVR_PhyIDType *const aPhyID);

/** @brief XCVR TC10 Event Handler

    This API handles the events arising out of TC10 block. This API is
    responsible for reading the status registers and interpreting them.

    This API acts as the interrupt handler handling the tc10 interrupts
    or as a polled API to check wake-up.

    @behavior Sync, Re-entrant

    @pre None
    @param[in]     aID            Index of the transceiver
    @param[inOut]  aCntxt         Pointer to context structure
    @param[in]     aIsTC10Enabled Boolean to indicate if tc10 is enabled on that phy or not
    @param[out]    aEvents        Pointer to retrieve active events

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events handled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10GetEventMaskType)(ETHXCVR_IDType aID,
                                                      ETHXCVR_TC10PortContextType *const aCntxt,
                                                      ETHXCVR_BooleanType aIsTC10Enabled,
                                                      ETHXCVR_WakeupEventType *const aEvents);

/** @brief TC10 Get Status

    This API retrieves the status of TC10 driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[out]    aStatus      Pointer to retrieve status information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Status retrieved successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10GetStatusType)(ETHXCVR_IDType aID,
                                                   ETHXCVR_TC10PortContextType *const aCntxt,
                                                   ETHXCVR_StatusType *const aStatus);

/** @brief TC10 Get Statistics

    This API retrieves the statistics from TC10 driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[out]    aStatus      Pointer to retrieve statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Statistics retrieved successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10GetStatsType)(ETHXCVR_IDType aID,
                                                  ETHXCVR_TC10PortContextType *const aCntxt,
                                                  ETHXCVR_StatsType *const aStats);

/** @brief TC10 Get Wake-up Reason

    This API retrieves the wake-up reason

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[out]    aReason      Pointer to retrieve wake-up reason

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Wake-up reason retrieved successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10GetWakeUpReasonType)(ETHXCVR_IDType aID,
                                                         ETHXCVR_TC10PortContextType *const aCntxt,
                                                         ETHXCVR_WakeupReasonType *const aReason);

/** @brief XCVR TC10 trigger Wake-up

    This API triggers a TC10 Wake-up request. Based on the current state of the
    phy, a Wake-Up-Pulse or a Wake-Up-Request will be sent appropriately.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Wake-up request successful
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10WakeUpReqType)(ETHXCVR_IDType aID,
                                                   ETHXCVR_TC10PortContextType *const aCntxt);

/** @brief XCVR TC10 trigger Sleep

    This API requests a TC10 sleep and implicitly enables accepting further
    sleep requests from link partner

    @behavior Sync, Re-entrant

    @pre Traffic should have been disabled

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Sleep request successful
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10SleepReqType)(ETHXCVR_IDType aID,
                                                  ETHXCVR_TC10PortContextType *const aCntxt);

/** @brief XCVR TC10 initiate forceful Sleep

    This API forces a TC10 sleep and implicitly enables accepting further
    sleep requests from link partner

    @behavior Sync, Re-entrant

    @pre Traffic should have been disabled

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Force Sleep successful
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10ForceSleepReqType)(ETHXCVR_IDType aID,
                                                       ETHXCVR_TC10PortContextType *const aCntxt);

/** @brief XCVR TC10 trigger Abort Sleep request

    This API aborts an ongoing request for sleep issued by link partner

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Sleep abort request successful
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10SleepAbortReqType)(ETHXCVR_IDType aID,
                                                       ETHXCVR_TC10PortContextType *const aCntxt);

/** @brief Allow Sleep request

    Sleep requests from remote partner will be aborted by default during boot-up.
    This API enables/disables accepting sleep requests from remote.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[in]     aMode        Boolean to indicate if sleep requests have to be allowed or not

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10AllowSleepReqType)(ETHXCVR_IDType aID,
                                                       ETHXCVR_TC10PortContextType *const aCntxt,
                                                       ETHXCVR_BooleanType aMode);

/** @brief Allow wake up through MDI

    This API enables/disables wakeup of phy based on wake up from remote.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inout]  aCntxt       Pointer to context structure
    @param[in]     aMode        TRUE = Enable wakeup on WUP, FALSE = Disable wakeup on WUP

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Completed successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
                                       OR TC10 state machine is not in awaken state

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10AllowMdiWakeType)(ETHXCVR_IDType aID,
                                                      ETHXCVR_TC10PortContextType *const aCntxt,
                                                      ETHXCVR_BooleanType aMode);

/** @brief TC10 Init

    This API initialises TC10 hardware logic

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[in]     aConfig      Port configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Initialised successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10InitType)(ETHXCVR_IDType aID,
                                              ETHXCVR_TC10PortContextType *const aCntxt,
                                              const ETHXCVR_TC10PortConfigType *const aConfig);

/** @brief TC10 Enable Program State

    When the TC10 straps are set-up in host mode, TC10 state machine will be
    held in initial state. Upon removing super-isolate, it advances to one of
    program, awake or sleep states based on the flags being set. This API
    enables/disables entering programming state before removing super-isolate
    as part of #ETHXCVR_ExtPhyTC10GetEventMaskType

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[in]     aMode        Boolean to Enable/Disable entering Program State.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Success
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10EnableProgramStateType)(ETHXCVR_IDType aID,
                                                            ETHXCVR_TC10PortContextType *const aCntxt,
                                                            ETHXCVR_BooleanType aMode);

/** @brief TC10 Exit Host Mode

    This API is meant to signal an exit out of Programming state. While the
    TC10 state machine is in Programming state, calling this API will signal
    the state machine to exit out of programming state and enter "Awake" state.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[in]     wakeReason   Wake reason

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Success
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10ExitHostModeType)(ETHXCVR_IDType aID,
                                                      ETHXCVR_TC10PortContextType *const aCntxt, ETHXCVR_WakeupReasonType wakeReason);

/** @brief TC10 Enable

    This API is meant to override the tc10 mode straps to disable TC10
    functionality on a PHY which supports TC10 but the link partner either
    doesn't support or isn't configured to enable TC10.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure
    @param[in]     aMode        Boolean to Enable/Disable TC10 functionality.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Success
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps
    @retval     #BCM_ERR_NOSUPPORT     Requested aMode is unsupported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10EnableType)(ETHXCVR_IDType aID,
                                                ETHXCVR_TC10PortContextType *const aCntxt,
                                                ETHXCVR_BooleanType aMode);

/**
    @brief Function type to check if the given PHY is supported by driver.

    This function can be called by integration layer or application to check
    if the connected PHY is supported by this driver or not.
    This function can be called without initializing the PHY (driver implementation
    shall maintain the list of PHYs supported).
    Function shall return #BCM_ERR_OK is given PHY configuration is supported by the driver,
    else it shall return #BCM_ERR_NOSUPPORT.

    @param[in]     aID          Index of the transceiver
    @param[in]     aConfig      Pointer to TC10 configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Driver supports the input configuration
    @retval     #BCM_ERR_NOSUPPORT     Driver doesn't support the input configuration

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10IsSupportedType)(ETHXCVR_IDType aID,
                                                     const ETHXCVR_TC10PortConfigType *const aConfig);

/** @brief TC10 Enable Events

    This API enables events specific to TC10

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events enabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10EnableEventsType)(ETHXCVR_IDType aID,
                                                      ETHXCVR_TC10PortContextType *const aCntxt);

/** @brief TC10 Disable Events

    This API disables events specific to TC10

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aID          Index of the transceiver
    @param[inOut]  aCntxt       Pointer to context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Events disabled successfully
    @retval     #BCM_ERR_INVAL_STATE   TC10 control not set to HOST mode through straps

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef int32_t (*ETHXCVR_ExtPhyTC10DisableEventsType)(ETHXCVR_IDType aID,
                                                       ETHXCVR_TC10PortContextType *const aCntxt);


/**
    @brief Ethernet PHY function table type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_FuncTblType {
    ETHXCVR_InitType                 init;                 /**< @brief Function type to initialize transceiver */
    ETHXCVR_DeInitType               deinit;               /**< @brief Function type to de-initialize transceiver */
    ETHXCVR_ResetType                reset;                /**< @brief Function type to reset */
    ETHXCVR_SetModeType              setMode;              /**< @brief Function type to set mode */
    ETHXCVR_GetModeType              getMode;              /**< @brief Function type to get mode */
    ETHXCVR_SetMasterModeType        setMasterMode;        /**< @brief Function type to set master/slave mode */
    ETHXCVR_GetMasterModeType        getMasterMode;        /**< @brief Function type to get master/slave mode */
    ETHXCVR_GetSpeedType             getSpeed;             /**< @brief Function type to get speed */
    ETHXCVR_SetSpeedType             setSpeed;             /**< @brief Function type to set speed */
    ETHXCVR_SetDuplexModeType        setDuplexMode;        /**< @brief Function type to set duplex mode */
    ETHXCVR_GetDuplexModeType        getDuplexMode;        /**< @brief Function type to get duplex mode */
    ETHXCVR_SetFlowControlType       setFlowControl;       /**< @brief Function type to set flow control */
    ETHXCVR_GetFlowControlType       getFlowControl;       /**< @brief Function type to get flow control */
    ETHXCVR_GetLinkStateType         getLinkState;         /**< @brief Function type to get link state */
    ETHXCVR_GetSQIValueType          getSQIValue;          /**< @brief Function type to get SQI value */
    ETHXCVR_SetLoopbackModeType      setLoopbackMode;      /**< @brief Function type to set loopback mode */
    ETHXCVR_GetLoopbackModeType      getLoopbackMode;      /**< @brief Function type to get loopback mode */
    ETHXCVR_SetJumboModeType         setJumboMode;         /**< @brief Function type to set jumbo mode */
    ETHXCVR_GetJumboModeType         getJumboMode;         /**< @brief Function type to get jumbo mode */
    ETHXCVR_SetAutoNegModeType       setAutoNegMode;       /**< @brief Function type to set Auto-negotiation mode */
    ETHXCVR_GetAutoNegStatusType     getAutoNegStatus;     /**< @brief Function type to get Auto-negotiation status */
    ETHXCVR_RestartAutoNegType       restartAutoNeg;       /**< @brief Function type to restart Auto-negotiation */
    ETHXCVR_GetStatsType             getStats;             /**< @brief Function type to get statistics */
    ETHXCVR_StateHandlerType         stateHandler;         /**< @brief Function type for state handler */
    ETHXCVR_UpdateHWStatusType       updateHWStatus;       /**< @brief Function type to update HW status parameters */
    ETHXCVR_IRQHandlerType           irqHandler;           /**< @brief Function type to handle IRQ */
    ETHXCVR_LinkChangeIndHandlerType linkChangeIndHandler; /**< @brief Function type to handle link change indication */
    ETHXCVR_GetBaseAddrType          getBaseAddr;          /**< @brief Function type to get the phy/bus base address */
    ETHXCVR_RunCableDiagType         runCableDiag;         /**< @brief Function type to execute the Cable diagnostics on given phy */
    ETHXCVR_PhyGetIDType             getID;                /**< @brief Function type to get ID */
} ETHXCVR_FuncTblType;

/**
    @brief Ethernet PHY TC10 function table type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ExtPhyTC10FuncTblType {
    ETHXCVR_ExtPhyTC10IsSupportedType   isSupported;   /**< @brief Function to check if PHY is supported     */
    ETHXCVR_ExtPhyTC10InitType          init;          /**< @brief Function pointer to initialise            */
    ETHXCVR_ExtPhyTC10EnableProgramStateType enPgmState; /**< @brief Function pointer to enable entering programming state */
    ETHXCVR_ExtPhyTC10ExitHostModeType  exitHostMode;  /**< @brief Function pointer to exit Host Mode        */
    ETHXCVR_ExtPhyTC10EnableType        enable;        /**< @brief Function pointer to enable/disable TC10 configuration   */
    ETHXCVR_ExtPhyTC10WakeUpReqType     wakeUpReq;     /**< @brief Function pointer to request a wake-up     */
    ETHXCVR_ExtPhyTC10SleepReqType      sleepReq;      /**< @brief Function pointer to request a sleep       */
    ETHXCVR_ExtPhyTC10ForceSleepReqType forceSleepReq; /**< @brief Function pointer to force sleep           */
    ETHXCVR_ExtPhyTC10SleepAbortReqType sleepAbortReq; /**< @brief Function pointer to abort a sleep request */
    ETHXCVR_ExtPhyTC10AllowSleepReqType allowSleepReq; /**< @brief Function pointer to allow sleep request   */
    ETHXCVR_ExtPhyTC10AllowMdiWakeType  allowMdiWake;  /**< @brief Function pointer to allow wake up through MDI */
    ETHXCVR_ExtPhyTC10GetStatusType     getStatus;     /**< @brief Function pointer to get the status        */
    ETHXCVR_ExtPhyTC10GetEventMaskType  getEventMask;  /**< @brief Function pointer to get the events        */
    ETHXCVR_ExtPhyTC10GetStatsType      getStats;      /**< @brief Function pointer to get statistics        */
    ETHXCVR_ExtPhyTC10GetWakeUpReasonType getWakeUpReason; /**< @brief Function pointer to get wake-up reason*/
    ETHXCVR_ExtPhyTC10EnableEventsType  enableEvents;  /**< @brief Function pointer to enable events         */
    ETHXCVR_ExtPhyTC10DisableEventsType disableEvents; /**< @brief Function pointer to disable events        */
} ETHXCVR_ExtPhyTC10FuncTblType;

/**
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_PlatformGetFnTblsCntxt(ETHXCVR_IDType aID,
                                       const ETHXCVR_FuncTblType **const aPhyFnTbl);
/**
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
int32_t ETHXCVR_PlatformGetTc10FnTblsCntxt(ETHXCVR_IDType aID,
                                               const ETHXCVR_ExtPhyTC10FuncTblType **const aPhyTc10FnTbl);


/**
    @brief external function table declaration

    @trace #BRCM_SWREQ_S16UP
 */

extern const ETHXCVR_FuncTblType S16UP_FuncTble;
/**
    @brief external function table declaration

    @trace #BRCM_SWREQ_BRPHY1GV2
 */

extern const ETHXCVR_FuncTblType BRPHY1GV2_FuncTbl;

/**
    @brief external function table declaration

    @trace #BRCM_SWREQ_BRPHY1GV2
 */

extern const ETHXCVR_ExtPhyTC10FuncTblType BRPHY1GV2_TC10FuncTbl;
/**
    @brief This is helper function to calculate OUI, revision id and model id from id0 and id1 registers.
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
uint32_t ETHXCVR_PhybitReverse(uint32_t n);
/**
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_PHY_OUI(id0, id1)                               ETHXCVR_PhybitReverse((id0) << 6U | ((id1) >> 10U & 0x3FU))
/**
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_PHY_MODEL(id0, id1)                             ((id1) >> 4U & 0x3FU)
/**
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/

#define ETHXCVR_PHY_REV(id0, id1)                               ((id1) & 0x3U)


#endif /* XCVR_FUNCTBL_H */

/** @} */
