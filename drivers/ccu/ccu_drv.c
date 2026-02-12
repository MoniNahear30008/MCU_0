/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_ccudrv_drv_impl CCU Driver Design
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_drv_impl
    @{
*/

#include <string.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include "cct_rdb.h"
#include <ccu.h>
#include <compiler.h>
#include <bcm_utils.h>

/*static const BCM_CompIDType BCM_LogCompID = BCM_CCT_ID;
static const uint16_t BCM_LogFileID = 0xB1U;*/

/**
    @name CCU Design IDs
    @{
    @brief Design IDs for CCU driver
*/
#define BRCM_SWDSGN_CCU_DRV_CHANSTATE_TYPE                    (0xB101U)    /**< @brief #CCU_DrvChanStateType                      */
#define BRCM_SWDSGN_CCU_DRV_SUBCHANSTATE_TYPE                 (0xB102U)    /**< @brief #CCU_DrvSubChanStateType                   */
#define BRCM_SWDSGN_CCU_DRV_CHANCBEN_TYPE                     (0xB103U)    /**< @brief #CCU_DrvChanCBEnType                       */
#define BRCM_SWDSGN_CCU_DRV_SUBCHAN_IDX_TYE                   (0xB104U)    /**< @brief #CCU_DrvSubChanIdxType                       */
#define BRCM_SWDSGN_CCU_DEV_CHANDEV_TYPE                      (0xB105U)    /**< @brief #CCU_DrvChanDevType                        */
#define BRCM_SWDSGN_CCU_RWDEV_TYPE                            (0xB106U)    /**< @brief #CCU_DrvRWDevType                          */
#define BRCM_SWDSGN_CCU_RWDEV_GLOBAL                          (0xB107U)    /**< @brief #CCU_DrvRWDev                              */
#define BRCM_SWDSGN_CCU_SETCAPDEFSIGNALPROP_PROC              (0xB108U)    /**< @brief #CCU_DrvSetCapDefSignalProp                */
#define BRCM_SWDSGN_CCU_INITDEV_PROC                          (0xB109U)    /**< @brief #CCU_DrvInitDev                            */
#define BRCM_SWDSGN_CCU_INTSETCAPSIGNALPROP_PROC              (0xB10AU)    /**< @brief #CCU_DrvIntSetCapSignalProp                */
#define BRCM_SWDSGN_CCU_INIT_PROC                             (0xB10BU)    /**< @brief #CCU_DrvInit                               */
#define BRCM_SWDSGN_CCU_DEINIT_PROC                           (0xB10CU)    /**< @brief #CCU_DrvDeInit                             */
#define BRCM_SWDSGN_CCU_GETCHANMODE_PROC                      (0xB10DU)    /**< @brief #CCU_DrvGetChanMode                        */
#define BRCM_SWDSGN_CCU_STARTCOMPCHANNEL_PROC                 (0xB10EU)    /**< @brief #CCU_DrvStartCompChannel                   */
#define BRCM_SWDSGN_CCU_STOPCOMPCHANNEL_PROC                  (0xB10FU)    /**< @brief #CCU_DrvStopCompChannel                    */
#define BRCM_SWDSGN_CCU_SETCOMPPINPROPERTY_PROC               (0xB110U)    /**< @brief #CCU_DrvSetCompPinProperty                 */
#define BRCM_SWDSGN_CCU_SETCOMPABSTHRESHOLD_PROC              (0xB111U)    /**< @brief #CCU_DrvSetCompAbsThreshold                */
#define BRCM_SWDSGN_CCU_ENABLECOMPCALLBACK_PROC               (0xB112U)    /**< @brief #CCU_DrvEnableCompCallback                 */
#define BRCM_SWDSGN_CCU_DISABLECOMPCALLBACK_PROC              (0xB113U)    /**< @brief #CCU_DrvDisableCompCallback                */
#define BRCM_SWDSGN_CCU_SETCAPSIGNALPROP_PROC                 (0xB114U)    /**< @brief #CCU_DrvSetCapSignalProp                   */
#define BRCM_SWDSGN_CCU_ENABLECAPCALLBACK_PROC                (0xB115U)    /**< @brief #CCU_DrvEnableCapCallback                  */
#define BRCM_SWDSGN_CCU_DISABLECAPCALLBACK_PROC               (0xB116U)    /**< @brief #CCU_DrvDisableCapCallback                 */
#define BRCM_SWDSGN_CCU_GETCAPINPUTSTATE_PROC                 (0xB117U)    /**< @brief #CCU_DrvGetCapInputState                   */
#define BRCM_SWDSGN_CCU_STARTCAPTIMESTAMP_PROC                (0xB118U)    /**< @brief #CCU_DrvStartCapTimestamp                  */
#define BRCM_SWDSGN_CCU_STOPCAPTIMESTAMP_PROC                 (0xB119U)    /**< @brief #CCU_DrvStopCapTimestamp                   */
#define BRCM_SWDSGN_CCU_GETCAPTIMESTAMPINDEX_PROC             (0xB11AU)    /**< @brief #CCU_DrvGetCapTimestampIndex               */
#define BRCM_SWDSGN_CCU_RESETCAPEDGECOUNT_PROC                (0xB11BU)    /**< @brief #CCU_DrvResetCapEdgeCount                  */
#define BRCM_SWDSGN_CCU_ENABLECAPEDGECOUNT_PROC               (0xB11CU)    /**< @brief #CCU_DrvEnableCapEdgeCount                 */
#define BRCM_SWDSGN_CCU_DISABLECAPEDGECOUNT_PROC              (0xB11DU)    /**< @brief #CCU_DrvDisableCapEdgeCount                */
#define BRCM_SWDSGN_CCU_ENABLECAPEDGEDETECTION_PROC           (0xB11EU)    /**< @brief #CCU_DrvEnableCapEdgeDetection             */
#define BRCM_SWDSGN_CCU_DISABLECAPEDGEDETECTION_PROC          (0xB11FU)    /**< @brief #CCU_DrvDisableCapEdgeDetection            */
#define BRCM_SWDSGN_CCU_GETCAPEDGENUMBERS_PROC                (0xB120U)    /**< @brief #CCU_DrvGetCapEdgeNumbers                  */
#define BRCM_SWDSGN_CCU_STARTCAPSIGNALMEASUREMENT_PROC        (0xB121U)    /**< @brief #CCU_DrvStartCapSignalMeasurement          */
#define BRCM_SWDSGN_CCU_STOPCAPSIGNALMEASUREMENT_PROC         (0xB122U)    /**< @brief #CCU_DrvStopCapSignalMeasurement           */
#define BRCM_SWDSGN_CCU_GETCAPTIMEELAPSED_PROC                (0xB123U)    /**< @brief #CCU_DrvGetCapTimeElapsed                  */
#define BRCM_SWDSGN_CCU_GETCAPDUTYCYCLEVALUES_PROC            (0xB124U)    /**< @brief #CCU_DrvGetCapDutyCycleValues              */
#define BRCM_SWDSGN_CCU_CAPTIMESTAMP_HANDLER_PROC             (0xB125U)    /**< @brief #CCU_DrvCapTimestampHandler                */
#define BRCM_SWDSGN_CCU_CAPSIGMEAS_HANDLER_PROC               (0xB126U)    /**< @brief #CCU_DrvCapSigMeasHandler                  */
#define BRCM_SWDSGN_CCU_CHAN_HANDLER_PROC                     (0xB127U)    /**< @brief #CCU_DrvChanHandler                        */
#define BRCM_SWDSGN_CCU_UDC_HANDLER_PROC                      (0xB128U)    /**< @brief #CCU_DrvUDCHandler                         */
#define BRCM_SWDSGN_CCU_DRV_IRQ_HANDLER_PROC                  (0xB129U)    /**< @brief #CCU_DrvIRQHandler                         */
#define BRCM_SWDSGN_CCU_CCTCHAN_TYPE                          (0xB12DU)    /**< @brief #CCU_CCTChanType                           */
#define BRCM_SWDSGN_CCU_CCTSUBCHAN_TYPE                       (0xB12EU)    /**< @brief #CCU_CCTSubChanType                        */
#define BRCM_SWDSGN_CCU_CCTMODE_TYPE                          (0xB12FU)    /**< @brief #CCU_CCTModeType                           */
#define BRCM_SWDSGN_CCU_CCTCOMPOUTPOL_TYPE                    (0xB130U)    /**< @brief #CCU_CCTCompOutPolType                     */
#define BRCM_SWDSGN_CCU_CCTINPINNUM_TYPE                      (0xB131U)    /**< @brief #CCU_CCTInPinNumType                       */
#define BRCM_SWDSGN_CCU_CCTEDGE_TYPE                          (0xB132U)    /**< @brief #CCU_CCTEdgeType                           */
#define BRCM_SWDSGN_CCU_CCTCONFIG_TYPE                        (0xB133U)    /**< @brief #CCU_CCTConfigType                         */
#define BRCM_SWDSGN_CCU_UDC_CTRL_ALL_EVENT_MACRO              (0xB134U)    /**< @brief #CCU_UDC_CTRL_CCT_MASK_ALL_EVENT_MASK      */
#define BRCM_SWDSGN_CCU_PSC_CTRL_ALL_EVENTS_MACRO             (0xB135U)    /**< @brief #CCU_PSC_CTRL_CCT_PSC_MASK_ALL_EVENTS_MASK */
#define BRCM_SWDSGN_CCU_CCTREGS_GLOBAL                        (0xB136U)    /**< @brief #CCU_CCTRegs                               */
#define BRCM_SWDSGN_CCU_CCTINIT_PROC                          (0xB137U)    /**< @brief #CCU_CCTInit                               */
#define BRCM_SWDSGN_CCU_CCTDEINIT_PROC                        (0xB138U)    /**< @brief #CCU_CCTDeInit                             */
#define BRCM_SWDSGN_CCU_CCTSETCOMPAREVALUE_PROC               (0xB139U)    /**< @brief #CCU_CCTSetCompareValue                    */
#define BRCM_SWDSGN_CCU_CCTSELECTMODE_PROC                    (0xB13AU)    /**< @brief #CCU_CCTSelectMode                         */
#define BRCM_SWDSGN_CCU_CCTSETCOMPAREOUTPOLARITY_PROC         (0xB13BU)    /**< @brief #CCU_CCTSetCompareOutPolarity              */
#define BRCM_SWDSGN_CCU_CCTENABLECOMPAREOUT_PROC              (0xB13CU)    /**< @brief #CCU_CCTEnableCompareOut                   */
#define BRCM_SWDSGN_CCU_CCTTRIGGERCOMPARE_PROC                (0xB13DU)    /**< @brief #CCU_CCTTriggerCompare                     */
#define BRCM_SWDSGN_CCU_CCTGETCAPTUREVALUE_PROC               (0xB13EU)    /**< @brief #CCU_CCTGetCaptureValue                    */
#define BRCM_SWDSGN_CCU_CCTSELCAPTUREINPUT_PROC               (0xB13FU)    /**< @brief #CCU_CCTSelCaptureInput                    */
#define BRCM_SWDSGN_CCU_CCTTRIGGERCAPTURE_PROC                (0xB140U)    /**< @brief #CCU_CCTTriggerCapture                     */
#define BRCM_SWDSGN_CCU_CCTENABLECAPCOMPINTERRUPT_PROC        (0xB141U)    /**< @brief #CCU_CCTEnableCapCompInterrupt             */
#define BRCM_SWDSGN_CCU_CCTDISABLECAPCOMPINTERRUPT_PROC       (0xB142U)    /**< @brief #CCU_CCTDisableCapCompInterrupt            */
#define BRCM_SWDSGN_CCU_CCTENABLEUDCINTERRUPT_PROC            (0xB143U)    /**< @brief #CCU_CCTEnableUDCInterrupt                 */
#define BRCM_SWDSGN_CCU_CCTDISABLEUDCINTERRUPT_PROC           (0xB144U)    /**< @brief #CCU_CCTDisableUDCInterrupt                */
#define BRCM_SWDSGN_CCU_CCTDISABLEETRINTERRUPT_PROC           (0xB145U)    /**< @brief #CCU_CCTDisableETRInterrupt                */
#define BRCM_SWDSGN_CCU_DRV_COMB_OUTPUT_SEL_PROC              (0xB146U)    /**< @brief #CCU_DrvCombOutputSel */
#define BRCM_SWDSGN_CCU_DRV_INPUT_FILTER_CTRL_SET_PROC        (0xB147U)    /**< @brief #CCU_DrvInputFilterCtrlSet */
#define BRCM_SWDSGN_CCU_DRV_EVENT_TRIGGER_SEL_PROC            (0xB148U)    /**< @brief #CCU_DrvEventTriggerSel */
#define BRCM_SWDSGN_CCU_ENABLE_INTERRUPT_PROC                 (0xB149U)    /**< @brief #CCU_EnableInterrupt */
#define BRCM_SWDSGN_CCU_CLEAR_INTERRUPT_PROC                  (0xB14AU)    /**< @brief #CCU_ClearInterrupt */
#define BRCM_SWDSGN_CCU_GET_INTERRUPT_STATUS_PROC             (0xB14BU)    /**< @brief #CCU_GetInterruptStatus */
#define BRCM_SWDSGN_CCU_CCT_ENABLE_COMPARE_XOROUT_PROC        (0xB14CU)    /**< @brief #CCU_CCTEnableCompareXorOut */
#define BRCM_SWDSGN_CCU_INT_IN_PSCEVENT_TRIGGERSEL_PROC       (0xB14DU)    /**< @brief #CCU_IntInPscEventTriggerSel */
#define BRCM_SWDSGN_CCU_INT_OUT_PSCEVENT_TRIGGERSEL_PROC      (0xB14EU)    /**< @brief #CCU_IntOutPscEventTriggerSel */
#define BRCM_SWDSGN_CCU_INT_IN_PSCEVENT_TRIGGERSEL_TEMP_PROC  (0xB14FU)    /**< @brief #CCU_IntInPscEventTriggerSelTemp */
#define BRCM_SWDSGN_CCU_INT_OUT_PSCEVENT_TRIGGERSEL_TEMP_PROC (0xB150U)    /**< @brief #CCU_IntOutPscEventTriggerSelTemp */
/** @} */

/**
    @name CCU_DrvChanStateType
    @{
    @brief CCU_DrvChanStateType

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvChanStateType;        /**< @brief CCU_DrvChanStateType */
#define CCU_DRV_CHANSTATE_STOPPED      (0x1UL) /**< @brief Stopped */
#define CCU_DRV_CHANSTATE_RUNNING      (0x2UL) /**< @brief Running */
/** @} */

/**
    @name CCU_DrvSubChanStateType
    @{
    @brief CCU_DrvSubChanStateType

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvSubChanStateType;    /**< @brief CCU_DrvSubChanStateType */
#define CCU_DRV_SUBCHANSTATE_IN_USE   (0x1UL) /**< @brief In use */
#define CCU_DRV_SUBCHANSTATE_NOT_USE  (0x2UL) /**< @brief Not use */
/** @} */

/**
    @name CCU_DrvChanCBEnType
    @{
    @brief CCU_DrvChanCBEnType

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvChanCBEnType; /**< @brief CCU_DrvChanCBEnType */
/** @} */

/**
    @name CCU Driver Sub Channel Index
    @{
    @brief CCU Sub channel Index

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvSubChanIdxType;    /**< @brief CCU_DrvSubChanIdxType */
#define CCU_DRV_SUBCHAN_IDX_MAX        (2)     /**< @brief Maximum Index */
#define CCU_DRV_SUBCHAN_IDX_A          (0x0UL) /**< @brief Index 0 */
#define CCU_DRV_SUBCHAN_IDX_B          (0x1UL) /**< @brief Index 1 */
/** @} */

/**
    @brief CCT channel type

    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_CCTChanType;

/**
    @{
    @brief CCT  type

    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTSubChanType;
#define CCU_CCTSUBCHAN_A          (1UL) /**< @brief Subchannel A */
#define CCU_CCTSUBCHAN_B          (2UL) /**< @brief Subchannel B */
#define CCU_CCTSUBCHAN_BOTH       (3UL) /**< @brief Subchannel A and B */
/** @} */

/**
    @{
    @brief CCT mode type
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTModeType;
#define CCU_CCTMODE_CAPTURE        (1UL) /**< @brief Capture */
#define CCU_CCTMODE_COMPARE        (2UL) /**< @brief Compare */
/** @} */

/**
    @{
    @brief CCT polarity type
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTCompOutPolType;
#define CCU_CCTCOMPOUTPOL_HIGH   (1UL) /**< @brief High */
#define CCU_CCTCOMPOUTPOL_LOW    (2UL) /**< @brief Low */
/** @} */

/**
    @{
    @brief CCT input pin type
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTInPinNumType;
#define CCU_CCTINPINNUM_0            (0UL) /**< @brief 0 */
#define CCU_CCTINPINNUM_1            (1UL) /**< @brief 1 */
#define CCU_CCTINPINNUM_2            (2UL) /**< @brief 2 */
#define CCU_CCTINPINNUM_3            (3UL) /**< @brief 3 */
/** @} */

/**
    @{
    @brief CCT edge type
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTEdgeType;
#define CCU_CCTEDGE_RISING         (0UL) /**< @brief Rising Edge */
#define CCU_CCTEDGE_FALLING        (1UL) /**< @brief Falling Edge */
/** @} */

/**
    @{
    @brief CCT config values
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef struct sCCU_CCTConfigType {
    uint16_t    cntrMaxValue;   /**< Up-down counter max value */
    uint16_t    preScaleRatio;  /**< Pre-scale ratio */
} CCU_CCTConfigType;
/** @} */

/**
    @name CCU_DrvChanDevType
    @{
    @brief CCU Channel Device Type

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_DrvChanDevType{                                /**< @brief CCU_DrvChanDevType */
    CCU_ChanModeType        mode;                                   /**< @brief mode                */
    CCU_DrvChanStateType    state;                                 /**< @brief state               */
    CCU_DrvSubChanStateType subChanState[CCU_DRV_SUBCHAN_IDX_MAX]; /**< @brief subChanState        */
    uint32_t                isCapSigPropSet;                        /**< @brief isCapSigPropSet     */
    /* Capture related parameters */
    CCU_CapModeType         capDefMode;                             /**< @brief capDefMode          */
    CCU_InPinSelType        capDefPin;                              /**< @brief capDefPin           */
    CCU_EdgeType            capDefEdge;                             /**< @brief capDefEdge          */
    CCU_CapModeType         capMode;                                /**< @brief capMode             */
    CCU_InPinSelType        capPin;                                 /**< @brief capPin              */
    CCU_EdgeType            capEdge;                                /**< @brief capEdge             */
    CCU_InputStateType      inputState;                             /**< @brief inputState          */
    CCU_DrvChanCBEnType     isCbEn;                                /**< @brief isCbEn              */
    CCU_ValueType *         TSBuf;                                  /**< @brief TSBuf               */
    uint32_t                bufSize;                                /**< @brief bufSize             */
    uint32_t                notifyInterval;                         /**< @brief notifyInterval      */
    CCU_TSIndexType         tsWrIndex;                              /**< @brief tsWrIndex           */
    CCU_EdgeNumberType      edgeCnt;                                /**< @brief edgeCnt             */
    CCU_TimeStampBufType    TSBufProp;                              /**< @brief TSBufProp           */
    CCU_SignalPropertyType  sigMeasProp;                            /**< @brief sigMeasProp         */
    CCU_DutyCycleType       dutyCycle;                              /**< @brief dutyCycle           */
    CCU_CallbackType        timestampCB;                            /**< @brief timestampCB         */
    CCU_CallbackType        edgeDetectCB;                           /**< @brief edgeDetectCB        */
    uint32_t                fallingEdgeIdx;                         /**< @brief fallingEdgeIdx      */
    uint32_t                risingEdgeIdx;                          /**< @brief risingEdgeIdx       */
    uint64_t                fallingEdgeTS[2UL];                     /**< @brief fallingEdgeTS       */
    uint64_t                risingEdgeTS[2UL];                      /**< @brief risingEdgeTS        */
    /* Compare related parametes */
    CCU_CallbackType        compCB;                                 /**< @brief compCB              */
    CCU_OutPinSelType       compOutPin;                             /**< @brief compOutPin          */
    CCU_PinActionType       pinAction;                              /**< @brief pinAction           */
    CCU_ValueType           compThreshold;                          /**< @brief compThreshold       */
    CCU_ValueType           compDefThreshold;                       /**< @brief compDefThreshold    */
    CCU_ValueType           elapsedTime;                            /**< @brief elapsedTime         */
    uint8_t                 swTriggerEn;                            /**< @brief SW trigger enable */
    uint8_t                 xorOutputEn;                            /**< @brief Enable XOR output of channel for sub channel A and B, send to selected output line. */
} CCU_DrvChanDevType;
/** @} */

/**
    @name CCU_DrvRWDevType
    @{
    @brief CCU read-write device structure

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct sCCU_DrvRWDevType {                         /**< @brief CCU_DrvRWDevType */
    uint32_t isInit;                                        /**< @brief isInit */
    uint32_t cntrWrapCnt;                                   /**< @brief cntrWrapCnt */
    CCU_DrvChanDevType         chan[CCT_MAX_CHAN_CNT];     /**< @brief chan */
} CCU_DrvRWDevType;
/** @} */

/**
    @brief CCT UDC CTRL All Event Mask

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
#define CCU_UDC_CTRL_CCT_MASK_ALL_EVENT_MASK        (0x0000003FUL)

/**
    @brief CCT PSC CTRL All Event Mask

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
#define CCU_PSC_CTRL_CCT_PSC_MASK_ALL_EVENTS_MASK   (0x003FFFFFUL)

/**
    @brief CCT Register base address

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static CCT_RDBType* const CCU_CCTRegs[CCT_MAX_HW_ID] =
{
    #if CCT_MAX_HW_ID >= 1UL
        (CCT_RDBType *)CAPCOM_0_BASE,
    #endif
    #if CCT_MAX_HW_ID >= 2UL
        (CCT_RDBType *)CAPCOM_1_BASE,
    #endif
    #if CCT_MAX_HW_ID > 2UL
      #error "CCT_MAX_HW_ID > 2"
    #endif
};
/**
    @brief CCT HW initialization.
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTInit(CCU_HwIDType aID, CCU_CCTConfigType *const aConfig)
{
    /* Reset and Enable CCT
     * NOTE: Disable-Enable CCT doesn't reset preconfigured register
     * values */
    CCU_CCTRegs[aID]->ctrla &= ~CCT_CTRLA_EN_MASK;
    CCU_CCTRegs[aID]->ctrla |= CCT_CTRLA_EN_MASK;

    /* Disable UDC trigger events, Set UDC value, unidirectional
     * counter and trigger it */
    CCU_CCTRegs[aID]->udc_ctrl |= CCU_UDC_CTRL_CCT_MASK_ALL_EVENT_MASK;
    CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_ARR_MASK;
    CCU_CCTRegs[aID]->udc_ctrl |= ((((uint32_t)aConfig->cntrMaxValue)
                << CCT_UDC_CTRL_ARR_SHIFT) &  CCT_UDC_CTRL_ARR_MASK);
    CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDEN_MASK;
    CCU_CCTRegs[aID]->udc_trig |= CCT_UDC_TRIG_MASK;

    /* Disable PSC trigger events */
    CCU_CCTRegs[aID]->psc_ctrl = CCU_PSC_CTRL_CCT_PSC_MASK_ALL_EVENTS_MASK;

    /* Set PSC ratio */
    CCU_CCTRegs[aID]->psc_ratio &= ~CCT_PSC_RATIO_MASK;
    CCU_CCTRegs[aID]->psc_ratio |= ((((uint32_t)aConfig->preScaleRatio)
                << CCT_PSC_RATIO_SHIFT) & CCT_PSC_RATIO_MASK);

    /* Trigger to load new PSC ratio */
    CCU_CCTRegs[aID]->psc_trig |= CCT_PSC_TRIG_MASK;
}

/**
    @brief Disable CCT
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDeInit(CCU_HwIDType aID)
{
    /* Disable CCT */
    CCU_CCTRegs[aID]->ctrla &= ~CCT_CTRLA_EN_MASK;

}

/**
    @brief Set CCT compare value
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSetCompareValue(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                    CCU_CCTSubChanType aSubChan,
                                    uint16_t aValue)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_val &= ~CCT_CCR_VAL_A_VAL_MASK;
        CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_val |=
            ((((uint32_t)aValue) << CCT_CCR_VAL_A_VAL_SHIFT) & CCT_CCR_VAL_A_VAL_MASK);
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_val &= ~CCT_CCR_VAL_B_VAL_MASK;
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_val |=
                    ((((uint32_t)aValue) << CCT_CCR_VAL_B_VAL_SHIFT)
                     & CCT_CCR_VAL_B_VAL_MASK);
        }
    }
}

/**
    @brief Select CCT mode as capture mode or compare mode.
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSelectMode(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                               CCU_CCTSubChanType aSubChan,
                               CCU_CCTModeType aMode)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_CCSEL_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_CCSEL_MASK;
        }
    }

    if (0UL != mask) {
        switch (aMode) {
        case CCU_CCTMODE_CAPTURE:
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
            break;
        case CCU_CCTMODE_COMPARE:
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
            break;
        default:
            /* Default compare mode is selected. */
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
            break;
        }
    }
}

/**
    @brief Set compare polarity
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSetCompareOutPolarity(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                          CCU_CCTSubChanType aSubChan,
                                          CCU_CCTCompOutPolType aPolarity)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_POL_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_POL_MASK;
        }
    }

    if (0UL != mask) {
        switch (aPolarity){
        case CCU_CCTCOMPOUTPOL_HIGH:
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
            break;
        case CCU_CCTCOMPOUTPOL_LOW:
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
            break;
        default:
            /* Default low compare out polarity selected. */
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
            break;
        }
    }
}

/**
    @brief Enable CCT compare out
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTEnableCompareOut(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                     CCU_CCTSubChanType aSubChan,
                                     uint32_t aEnable)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_OUT_OEB_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_OUT_OEB_MASK;
        }
    }

    if (0UL != mask) {
        if (0UL != aEnable) {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
        } else {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
        }
    }
}

/**
    @brief Enable CCT compare XOR of channel A and channel B out
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTEnableCompareXorOut(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                     CCU_CCTSubChanType aSubChan,
                                     uint32_t aEnable)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_EN_XOR_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_EN_XOR_MASK;
        }
    }

    if (0UL != mask) {
        if (0UL != aEnable) {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
        } else {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
        }
    }
}


/**
    @brief CCT trigger compare value
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTTriggerCompare(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                   CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_A_TRIG_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_B_TRIG_MASK;
        }
    }
}

/**
    @brief Gets Capture value
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTGetCaptureValue(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                    CCU_CCTSubChanType aSubChan,
                                    uint16_t *const aValue)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        *aValue = (uint16_t)(((CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_val) & CCT_CCR_VAL_A_VAL_MASK)
            >> CCT_CCR_VAL_A_VAL_SHIFT);
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            *aValue =
                (uint16_t)(((CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_val) & CCT_CCR_VAL_B_VAL_MASK)
                >> CCT_CCR_VAL_B_VAL_SHIFT);
        } else {
            *aValue = 0U;
        }
    }
}

/**
    @brief Select capture input settings
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSelCaptureInput(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                    CCU_CCTSubChanType aSubChan,
                                    CCU_CCTInPinNumType aInPinNum,
                                    CCU_CCTEdgeType aEdge,
                                    uint32_t aEnable)
{
    uint32_t shift = 32UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        shift = CCT_CCR_CTRL_A_MASK_IN0_RE_SHIFT;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            shift = CCT_CCR_CTRL_B_MASK_IN0_RE_SHIFT;
        }
    }

     /* @event  CERT INT30-C
          @issue left shift (2UL * aInPinNum) + aEdge) may wrap
          @risk   No risk
          @justification The shift value is less than 31.
     */
    if (32UL > shift) {
        if (0UL != aEnable) {
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~(0x1UL << (shift + (2UL * aInPinNum) + aEdge));
        } else {
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_ctrl |= (0x1UL << (shift + (2UL * aInPinNum) + aEdge));
        }
    }
}

/**
    @brief Trigger capture event
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTTriggerCapture(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                   CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_BOTH == aSubChan) {
        CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_trig |=
            (CCT_CCR_TRIG_A_TRIG_MASK | CCT_CCR_TRIG_B_TRIG_MASK);
    } else {
        if (CCU_CCTSUBCHAN_A == aSubChan) {
            CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_A_TRIG_MASK;
        } else {
            if (CCU_CCTSUBCHAN_B == aSubChan) {
                CCU_CCTRegs[aID]->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_B_TRIG_MASK;
            }
        }
    }

}

/**
    @brief Enable capture compare interrupt
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTEnableCapCompInterrupt(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                           CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
       /* @event   CERT INT30-C
             @issue 2UL * aChanNum may wrap
             @risk   No risk
             @justification Channel number limited to 4.
             */
        /* coverity[cert_int30_c_violation] */
       /* coverity[cert_int34_c_violation] */
        CCU_CCTRegs[aID]->irq_ctrl |= (CCT_IRQ_CTRL_CCR0_A_IRQ_EN_MASK << (2UL * aChanNum));
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            CCU_CCTRegs[aID]->irq_ctrl |= (CCT_IRQ_CTRL_CCR0_B_IRQ_EN_MASK << (2UL * aChanNum));
        }
    }
}

/**
    @brief Disable capture compare interrupt
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/

static void CCU_CCTDisableCapCompInterrupt(CCU_HwIDType aID, CCU_CCTChanType aChanNum,
                                            CCU_CCTSubChanType aSubChan)
{
    /* @event   CERT INT30-C
             @issue 2UL * aChanNum may wrap
             @risk   No risk
             @justification Channel number limited to 4.
             */
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        /* coverity[cert_int30_c_violation] */
        /* coverity[cert_int34_c_violation] */
        CCU_CCTRegs[aID]->irq_ctrl &= ~(CCT_IRQ_CTRL_CCR0_A_IRQ_EN_MASK << (2UL * aChanNum));
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            CCU_CCTRegs[aID]->irq_ctrl &= ~(CCT_IRQ_CTRL_CCR0_B_IRQ_EN_MASK << (2UL * aChanNum));
        }
    }
}

/**
    @brief Enable UDC interrupt
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static  void CCU_CCTEnableUDCInterrupt(CCU_HwIDType aID)
{
    CCU_CCTRegs[aID]->irq_ctrl |= CCT_IRQ_CTRL_UDC_IRQ_EN_MASK;
}

/**
    @brief Disable UDC interrupt
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDisableUDCInterrupt(CCU_HwIDType aID)
{
    CCU_CCTRegs[aID]->irq_ctrl &= ~CCT_IRQ_CTRL_UDC_IRQ_EN_MASK;
}

/**
    @brief Disable ETR interrupt
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDisableETRInterrupt(CCU_HwIDType aID, CCU_CCTEdgeType aEdge)
{
    switch (aEdge) {
    case CCU_CCTEDGE_RISING:
        CCU_CCTRegs[aID]->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_RE_IRQ_EN_MASK;
        break;
    case CCU_CCTEDGE_FALLING:
        CCU_CCTRegs[aID]->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_FE_IRQ_EN_MASK;
        break;
    default:
        /* Default falling edge ETR interrupt */
        CCU_CCTRegs[aID]->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_FE_IRQ_EN_MASK;
        break;
    }
}

/**
    @name CCU_DrvRWDev

    @brief CCU read-write device

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
static CCU_DrvRWDevType CCU_DrvRWDev[CCT_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @brief Set capture signal property for the given channel.
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvSetCapDefSignalProp(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    CCU_InPinSelType inPin = CCU_DrvRWDev[aID].chan[aChanNum].capDefPin;
    CCU_EdgeType edge = CCU_DrvRWDev[aID].chan[aChanNum].capDefEdge;

    /* Set capture signal and edge */
    switch (edge) {
    case CCU_EDGE_RISING:
        CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, 1UL);
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev[aID].chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        }
        break;
    case CCU_EDGE_FALLING:
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev[aID].chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_RISING, 1UL);
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        } else {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        }
        break;
    case CCU_EDGE_BOTH:
        CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, 1UL);
        if ((CCU_CAPMODE_EDGE_COUNT == CCU_DrvRWDev[aID].chan[aChanNum].capMode)
           || (CCU_CAPMODE_EDGE_DETECT == CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        } else {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        }
        break;
    default:
        BCM_LOG_ERROR(0U, BCM_STATE_INIT, aChanNum, "Failed to Set capture signal property for the given channel \n");
        break;
    }
}

/**
    @brief Configure initialization data
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvInitDev(CCU_HwIDType aID, const CCU_ConfigType *const aConfig)
{
    CCU_DrvRWDev[aID].cntrWrapCnt = 0UL;
    CCU_DrvRWDev[aID].isInit = 0UL;
#if (CCT_MAX_CHAN_CNT == 0)
#error "CCT_MAX_CHAN_CNT == 0"
#endif
#if (CCT_MAX_CHAN_CNT > 0)
    CCU_DrvRWDev[aID].chan[0UL].isCapSigPropSet = 0UL;
    CCU_DrvRWDev[aID].chan[0UL].edgeCnt = 0UL;
    CCU_DrvRWDev[aID].chan[0UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[0UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev[aID].chan[0UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev[aID].chan[0UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[0UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[0UL].fallingEdgeTS[1UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[0UL].mode = CCU_CHANMODE_NONE;
    CCU_DrvRWDev[aID].chan[0UL].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev[aID].chan[0UL].inputState = CCU_INPUTSTATE_IDLE;
#endif
#if (CCT_MAX_CHAN_CNT > 1)
    CCU_DrvRWDev[aID].chan[1UL].isCapSigPropSet = 0UL;
    CCU_DrvRWDev[aID].chan[1UL].edgeCnt = 0UL;
    CCU_DrvRWDev[aID].chan[1UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[1UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev[aID].chan[1UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev[aID].chan[1UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[1UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[1UL].fallingEdgeTS[1UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[1UL].mode = CCU_CHANMODE_NONE;
    CCU_DrvRWDev[aID].chan[1UL].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev[aID].chan[1UL].inputState = CCU_INPUTSTATE_IDLE;
#endif
#if (CCT_MAX_CHAN_CNT > 2)
    CCU_DrvRWDev[aID].chan[2UL].isCapSigPropSet = 0UL;
    CCU_DrvRWDev[aID].chan[2UL].edgeCnt = 0UL;
    CCU_DrvRWDev[aID].chan[2UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[2UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev[aID].chan[2UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev[aID].chan[2UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[2UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[2UL].fallingEdgeTS[1UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[2UL].mode = CCU_CHANMODE_NONE;
    CCU_DrvRWDev[aID].chan[2UL].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev[aID].chan[2UL].inputState = CCU_INPUTSTATE_IDLE;
#endif
#if (CCT_MAX_CHAN_CNT > 3)
    CCU_DrvRWDev[aID].chan[3UL].isCapSigPropSet = 0UL;
    CCU_DrvRWDev[aID].chan[3UL].edgeCnt = 0UL;
    CCU_DrvRWDev[aID].chan[3UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[3UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev[aID].chan[3UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev[aID].chan[3UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev[aID].chan[3UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[3UL].fallingEdgeTS[1UL] = 0ULL;
    CCU_DrvRWDev[aID].chan[3UL].mode = CCU_CHANMODE_NONE;
    CCU_DrvRWDev[aID].chan[3UL].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev[aID].chan[3UL].inputState = CCU_INPUTSTATE_IDLE;
#endif
#if (CCT_MAX_CHAN_CNT > 4)
#error "CCT_MAX_CHAN_CNT > 4"
#endif
}

/**
    @brief Set capture signal property based on channel number
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvIntSetCapSignalProp(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    CCU_EdgeType edge;
    CCU_InPinSelType inPin;

    inPin = CCU_DrvRWDev[aID].chan[aChanNum].capPin;
    edge = CCU_DrvRWDev[aID].chan[aChanNum].capEdge;

    /* Set capture signal and edge */
    switch (edge) {
    case CCU_EDGE_RISING:
        CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, 1UL);
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev[aID].chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        }
        break;
    case CCU_EDGE_FALLING:
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev[aID].chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_RISING, 1UL);
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        } else {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        }
        break;
    default:
        CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, 1UL);
        if ((CCU_CAPMODE_EDGE_COUNT == CCU_DrvRWDev[aID].chan[aChanNum].capMode)
           || (CCU_CAPMODE_EDGE_DETECT == CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        } else {
            CCU_CCTSelCaptureInput(aID, aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, 1UL);
        }
        break;
    }
}

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
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvInit(CCU_HwIDType aID, const CCU_ConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;
    CCU_CCTConfigType cctCfg;
    CCU_ChanCfgTblType * chanCfgTbl;
    uint32_t chanNum = 0UL;

    if ((NULL == aConfig) || (aID >= CCT_MAX_HW_ID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        (void) memset(&CCU_DrvRWDev[aID], 0L, sizeof(CCU_DrvRWDevType));
    }

    if ((BCM_ERR_OK == retVal) && (0UL != CCU_DrvRWDev[aID].isInit)) {

        /* Check the controller status */

        retVal = BCM_ERR_UNINIT;

    } else {
        if(BCM_ERR_OK == retVal) {
            CCU_DrvInitDev(aID, aConfig);

            cctCfg.cntrMaxValue = aConfig->cntrMaxValue;
            cctCfg.preScaleRatio = aConfig->preScaleRatio;
            CCU_CCTInit(aID, &cctCfg);
            CCU_CCTEnableUDCInterrupt(aID);

            chanCfgTbl = aConfig->chanCfgTbl;

            if (NULL != chanCfgTbl) {
                for (i = 0UL; i < aConfig->chanCfgTblSz; i++) {
                    if (chanCfgTbl[i].chanNum >= CCT_MAX_CHAN_CNT) {
                        continue;
                    }
                    chanNum = chanCfgTbl[i].chanNum;
                    switch (chanCfgTbl[i].chanMode) {
                    case CCU_CHANMODE_CAPTURE:
                        CCU_DrvRWDev[aID].chan[chanNum].mode = chanCfgTbl[i].chanMode;
                        CCU_DrvRWDev[aID].chan[chanNum].capDefMode = chanCfgTbl[i].capCfg.mode;
                        CCU_DrvRWDev[aID].chan[chanNum].capDefPin = chanCfgTbl[i].capCfg.pin;
                        CCU_DrvRWDev[aID].chan[chanNum].capDefEdge = chanCfgTbl[i].capCfg.edge;
                        CCU_DrvRWDev[aID].chan[chanNum].TSBufProp = chanCfgTbl[i].capCfg.TSBufProp;
                        CCU_DrvRWDev[aID].chan[chanNum].sigMeasProp = chanCfgTbl[i].capCfg.sigMeasProp;
                        CCU_DrvRWDev[aID].chan[chanNum].timestampCB = chanCfgTbl[i].capCfg.timestampCB;
                        CCU_DrvRWDev[aID].chan[chanNum].edgeDetectCB = chanCfgTbl[i].capCfg.edgeDetectCB;
                        CCU_DrvRWDev[aID].chan[chanNum].swTriggerEn = chanCfgTbl[i].capCfg.swTriggerEn;
                        break;
                    case CCU_CHANMODE_COMPARE:
                        CCU_DrvRWDev[aID].chan[chanNum].mode = chanCfgTbl[i].chanMode;
                        CCU_DrvRWDev[aID].chan[chanNum].compCB = chanCfgTbl[i].compCfg.compCB;
                        CCU_DrvRWDev[aID].chan[chanNum].compDefThreshold =
                                            chanCfgTbl[i].compCfg.compThreshold;
                        CCU_DrvRWDev[aID].chan[chanNum].swTriggerEn =
                                            chanCfgTbl[i].compCfg.swTriggerEn;

                        CCU_DrvRWDev[aID].chan[chanNum].xorOutputEn =
                                                            chanCfgTbl[i].compCfg.xorOutputEn;

                        break;
                    default:
                        BCM_LOG_ERROR(0U, BCM_STATE_INIT, retVal, "Failed to initialized CCU driver \n");
                        break;
                    }
                }
            }

            CCU_DrvRWDev[aID].isInit = 1UL;
        }
    }
    return retVal;
}

/** @brief De-initialize CCU driver

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         CCU controller index

    @return     int32_t

    @post TBD

    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvDeInit(CCU_HwIDType aID)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;
    /* Check the controller status */
    if (aID >= CCT_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else {
        /* Check state of all the channels */
        for (i = 0UL; i < CCT_MAX_CHAN_CNT; i++) {
            if (CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev[aID].chan[i].state) {
            /* Disable all the interrupts */
            CCU_CCTDisableCapCompInterrupt(aID, i, CCU_CCTSUBCHAN_A);
            CCU_CCTDisableCapCompInterrupt(aID, i, CCU_CCTSUBCHAN_B);
            } else {
              retVal = BCM_ERR_BUSY;
              break;
            }
        }
        if(BCM_ERR_OK == retVal) {
            CCU_CCTDisableUDCInterrupt(aID);
            CCU_CCTDisableETRInterrupt(aID, CCU_CCTEDGE_RISING);
            CCU_CCTDisableETRInterrupt(aID, CCU_CCTEDGE_FALLING);

            /* De-initialize CCT */
            CCU_CCTDeInit(aID);

            CCU_DrvRWDev[aID].isInit = 0UL;
        }
    }

    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetChanMode(CCU_HwIDType aID, CCU_ChanType aChanNum,
                            CCU_ChanModeType *const aChanMode)
{
    int32_t retVal = BCM_ERR_OK;
    if (aID >= CCT_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_CHAN_MAX < aChanNum)
            || (NULL == aChanMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aChanMode = CCU_DrvRWDev[aID].chan[aChanNum].mode;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvStartCompChannel(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;
    CCU_CCTSubChanType subChan = 0UL;
    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        if (CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A]
                == CCU_DRV_SUBCHANSTATE_IN_USE) {
            subChan = CCU_CCTSUBCHAN_A;
        } else {
            if (CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B]
                    == CCU_DRV_SUBCHANSTATE_IN_USE) {
                subChan = CCU_CCTSUBCHAN_B;
            } else {
                subChan = CCU_CCTSUBCHAN_A;
                CCU_CCTSetCompareValue(aID, aChanNum, subChan,
                        CCU_DrvRWDev[aID].chan[aChanNum].compDefThreshold);
            }
        }

        /* Enable interupt for the channel */
        CCU_CCTEnableCapCompInterrupt(aID, aChanNum, subChan);

        /* Set Compare mode */
        CCU_CCTSelectMode(aID, aChanNum, subChan, CCU_CCTMODE_COMPARE);

        /* Set Compare trigger bit */
        CCU_CCTTriggerCompare(aID, aChanNum, subChan);
        /* Test SW trigger */
        if(CCU_DrvRWDev[aID].chan[aChanNum].swTriggerEn > 0U) {
            CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_HW_EN_OVR_MASK;
        }
        /* mark the channel state to running */
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvStopCompChannel(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;
    CCU_CCTSubChanType subChan = 0UL;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        if (CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A]
                == CCU_DRV_SUBCHANSTATE_IN_USE) {
            subChan = CCU_CCTSUBCHAN_A;
        } else {
            if (CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B]
                    == CCU_DRV_SUBCHANSTATE_IN_USE) {
                subChan = CCU_CCTSUBCHAN_B;
            }
        }

        if (0UL != subChan) {
            /* Disable interrupt for the channel */
            CCU_CCTDisableCapCompInterrupt(aID, aChanNum, subChan);

            /* Halt the compare operation by changing mode */
            CCU_CCTSelectMode(aID, aChanNum, subChan, CCU_CCTMODE_CAPTURE);
        }

        /* Mark sub-channel as not in use */
        CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A] =
                                                    CCU_DRV_SUBCHANSTATE_NOT_USE;
        CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B] =
                                                    CCU_DRV_SUBCHANSTATE_NOT_USE;
        /* Disable the SW trigger that overrides hardware enable control. */
        CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_HW_EN_OVR_MASK;
        /* mark the channel state to stop */
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvSetCompPinProperty(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                       CCU_PinActionType aPinAction,
                                       CCU_OutPinSelType aOutPin)
{
    int32_t retVal = BCM_ERR_OK;
    CCU_CCTSubChanType subChan;
    uint32_t xorOutputEn = 0UL;
    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_PINACTION_SET_LOW > aPinAction)
            || (CCU_PINACTION_DISABLE < aPinAction)
            || (CCU_OUTPINSEL_OUT > aOutPin)
            || (CCU_OUTPINSEL_OUT_N < aOutPin)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
   } else {

        if (CCU_OUTPINSEL_OUT == aOutPin) {
            subChan = CCU_CCTSUBCHAN_A;
            CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A] =
                                                    CCU_DRV_SUBCHANSTATE_IN_USE;
        } else {
            subChan = CCU_CCTSUBCHAN_B;
            CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B] =
                                                    CCU_DRV_SUBCHANSTATE_IN_USE;
        }
        xorOutputEn = CCU_DrvRWDev[aID].chan[aChanNum].xorOutputEn;
        switch (aPinAction) {
        case CCU_PINACTION_SET_LOW:
            CCU_CCTEnableCompareXorOut(aID, aChanNum, subChan, xorOutputEn);
            CCU_CCTSetCompareOutPolarity(aID, aChanNum, subChan, CCU_CCTCOMPOUTPOL_LOW);
            CCU_CCTEnableCompareOut(aID, aChanNum, subChan, 1UL);
            break;
        case CCU_PINACTION_SET_HIGH:
            CCU_CCTEnableCompareXorOut(aID, aChanNum, subChan, xorOutputEn);
            CCU_CCTSetCompareOutPolarity(aID, aChanNum, subChan, CCU_CCTCOMPOUTPOL_HIGH);
            CCU_CCTEnableCompareOut(aID, aChanNum, subChan, 1UL);
            break;
        case CCU_PINACTION_TOGGLE:
            /* TODO: What to do for this state? */
            break;
        default:
            /* Disable out enable, Pin will be HIGH */
            CCU_CCTEnableCompareOut(aID, aChanNum, subChan, 0UL);
            break;
        }
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvSetCompAbsThreshold(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                        CCU_ValueType aAbsValue)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        /* Setting absolute threshold value */
        if (CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A] ==
                                                CCU_DRV_SUBCHANSTATE_IN_USE) {
            CCU_CCTSetCompareValue(aID, aChanNum, CCU_CCTSUBCHAN_A, aAbsValue);
        } else {
            if (CCU_DrvRWDev[aID].chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B] ==
                                                CCU_DRV_SUBCHANSTATE_IN_USE) {
                CCU_CCTSetCompareValue(aID, aChanNum, CCU_CCTSUBCHAN_B, aAbsValue);
            }
        }
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvEnableCompCallback(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].isCbEn = 1UL;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvDisableCompCallback(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        CCU_DrvRWDev[aID].chan[aChanNum].isCbEn = 0UL;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvSetCapSignalProp(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                     CCU_InPinSelType aInPin,
                                     CCU_EdgeType aEdge)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)
        || (CCU_INPINSEL_IN3 < aInPin)
        || (CCU_EDGE_RISING > aEdge)
        || (CCU_EDGE_BOTH < aEdge)) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;

    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
   } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
   } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capPin = aInPin;
        CCU_DrvRWDev[aID].chan[aChanNum].capEdge = aEdge;
        CCU_DrvRWDev[aID].chan[aChanNum].isCapSigPropSet = 1UL;
    }

    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvEnableCapCallback(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].isCbEn = 1UL;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvDisableCapCallback(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        CCU_DrvRWDev[aID].chan[aChanNum].isCbEn = 0UL;
    }

    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetCapInputState(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                        CCU_InputStateType *const aInputState)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        *aInputState = CCU_DrvRWDev[aID].chan[aChanNum].inputState;
        CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
    }

    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvStartCapTimestamp(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                      CCU_ValueType * const aTSBuf,
                                      uint32_t aBufSize,
                                      uint32_t aNotifyInterval)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum) || (NULL == aTSBuf)
        || (0UL == aBufSize)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_TIMESTAMP;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
        CCU_DrvRWDev[aID].chan[aChanNum].tsWrIndex = 0UL;
        CCU_DrvRWDev[aID].chan[aChanNum].TSBuf = aTSBuf;
        CCU_DrvRWDev[aID].chan[aChanNum].bufSize = aBufSize;
        CCU_DrvRWDev[aID].chan[aChanNum].notifyInterval = aNotifyInterval;

        if (0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCapSigPropSet) {
            CCU_DrvIntSetCapSignalProp(aID, aChanNum);
        } else {
            CCU_DrvSetCapDefSignalProp(aID, aChanNum);
        }

        switch (CCU_DrvRWDev[aID].chan[aChanNum].capEdge) {
        case CCU_EDGE_RISING:
            CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
            CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
            CCU_CCTTriggerCapture(aID, aChanNum, CCU_CCTSUBCHAN_A);
            break;
        case CCU_EDGE_FALLING:
            CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
            CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
            CCU_CCTTriggerCapture(aID, aChanNum, CCU_CCTSUBCHAN_A);
            break;
        case CCU_EDGE_BOTH:
            CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
            CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_B);
            CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
            CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_CAPTURE);
            CCU_CCTTriggerCapture(aID, aChanNum, CCU_CCTSUBCHAN_BOTH);
            break;
        default:
            BCM_LOG_ERROR(0U, BCM_STATE_INIT, retVal, "Capture Edge is not selected for timestamp\n");
            break;
        }
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvStopCapTimestamp(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_TIMESTAMP != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_NONE;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
        CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
        CCU_DrvRWDev[aID].chan[aChanNum].TSBuf = NULL;
        CCU_DrvRWDev[aID].chan[aChanNum].bufSize = 0UL;

        CCU_CCTDisableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTDisableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_B);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_COMPARE);
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetCapTimestampIndex(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                            CCU_TSIndexType *const aTSIndex)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_TIMESTAMP != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        *aTSIndex = CCU_DrvRWDev[aID].chan[aChanNum].tsWrIndex;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvResetCapEdgeCount(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].edgeCnt = 0UL;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvEnableCapEdgeCount(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_EDGE_COUNT;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
        if (0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCapSigPropSet) {
            CCU_DrvIntSetCapSignalProp(aID, aChanNum);
        } else {
            CCU_DrvSetCapDefSignalProp(aID, aChanNum);
        }
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
        CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        if(CCU_DrvRWDev[aID].chan[aChanNum].swTriggerEn > 0U) {
            CCU_CCTTriggerCapture(aID, aChanNum, CCU_CCTSUBCHAN_A);
        }
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvDisableCapEdgeCount(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_EDGE_COUNT != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_NONE;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
        CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
        CCU_CCTDisableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvEnableCapEdgeDetection(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_EDGE_DETECT;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
        if (0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCapSigPropSet) {
            CCU_DrvIntSetCapSignalProp(aID, aChanNum);
        } else {
            CCU_DrvSetCapDefSignalProp(aID, aChanNum);
        }
        CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);

        if(CCU_DrvRWDev[aID].chan[aChanNum].swTriggerEn > 0U) {
            CCU_CCTTriggerCapture(aID, aChanNum, CCU_CCTSUBCHAN_A);
        }
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvDisableCapEdgeDetection(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_EDGE_DETECT != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_NONE;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
        CCU_CCTDisableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetCapEdgeNumbers(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                         CCU_EdgeNumberType *const aEdgeNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_CAPMODE_EDGE_COUNT != CCU_DrvRWDev[aID].chan[aChanNum].capMode) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        *aEdgeNum = CCU_DrvRWDev[aID].chan[aChanNum].edgeCnt;
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvStartCapSignalMeasurement(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_SIGNAL_MEASURE;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
        CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeIdx = 0UL;
        CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeIdx = 0UL;
        if (0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCapSigPropSet) {
            CCU_DrvIntSetCapSignalProp(aID, aChanNum);
        } else {
            CCU_DrvSetCapDefSignalProp(aID, aChanNum);
        }
        CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTEnableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_B);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_CAPTURE);
        CCU_CCTTriggerCapture(aID, aChanNum, CCU_CCTSUBCHAN_BOTH);
    }
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvStopCapSignalMeasurement(CCU_HwIDType aID, CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_SIGNAL_MEASURE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        CCU_DrvRWDev[aID].chan[aChanNum].capMode = CCU_CAPMODE_NONE;
        CCU_DrvRWDev[aID].chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
        CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
        CCU_CCTDisableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);
        CCU_CCTDisableCapCompInterrupt(aID, aChanNum, CCU_CCTSUBCHAN_B);
        CCU_CCTSelectMode(aID, aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_COMPARE);
    }
    return retVal;
}

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
                                        #CCU_DrvStartCapSignalMeasurement or after
                                        last #CCU_DrvGetCapTimeElapsed. Elapsed
                                        time refers to:
                                        - Low time, if signal property is
                                          #CCU_SIGNALPROPERTY_LOW_TIME
                                        - High time, if signal property is
                                          #CCU_SIGNALPROPERTY_HIGH_TIME
                                        - Period time, if signal property is
                                          #CCU_SIGNALPROPERTY_PERIOD_TIME

    @post TBD
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetCapTimeElapsed(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                         CCU_ValueType *const aTimeElapsed)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_SIGNAL_MEASURE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        *aTimeElapsed = CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime;
        if (CCU_DrvRWDev[aID].chan[aChanNum].sigMeasProp != CCU_SIGNALPROPERTY_PERIOD_TIME) {
            CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime = 0U;
        }
    }
    return retVal;
}

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

    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetCapDutyCycleValues(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                          CCU_DutyCycleType *const aDutyCycle)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX < aChanNum) || (NULL == aDutyCycle)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_UNINIT;
    } else if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev[aID].chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev[aID].chan[aChanNum].state)
        || (CCU_CAPMODE_SIGNAL_MEASURE != CCU_DrvRWDev[aID].chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        aDutyCycle->activeTime = CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime;
        aDutyCycle->periodTime = CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime;
        CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime = 0U;
        CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime = 0U;
    }
    return retVal;
}

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

    @post TBD

    @trace #BRCM_SWREQ_CCU
    @trace #BRCM_SWARCH_CCU_DRV_COMB_OUTPUT_SEL_PROC

    @code{.unparsed}
    Set the output combine control register for the selected channel number
    @endcode
*/
int32_t CCU_DrvCombOutputSel(CCU_HwIDType aID, CCU_ChanType aChanNum, CCU_CombOutSelType outSel)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((CCU_CHAN_MAX < aChanNum) || (CCT_MAX_HW_ID <= aID) || (CCU_COMB_OUT_SEL_3 < outSel)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((BCM_ERR_OK == retVal) && (1UL != CCU_DrvRWDev[aID].isInit)) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
       /* @event   CERT INT30-C
             @issue         Coverity infers that the 8UL * outSel may wrap
             @risk      No risk
             @justification 8 * outSel will not cross unsigned integer maximum limit.
             */
       /* coverity[cert_int30_c_violation] */
       /* coverity[cert_int34_c_violation] */
       CCU_CCTRegs[aID]->out_ctrl |= ((0x3UL << (2UL * aChanNum)) << (8UL * outSel));
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

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

    @post TBD

    @trace #BRCM_SWREQ_CCU
    @trace #BRCM_SWARCH_CCU_DRV_INPUT_FILTER_CTRL_SET_PROC

    @code{.unparsed}
    Set the input control register for the selected filter type
    @endcode

*/
int32_t CCU_DrvInputFilterCtrlSet(CCU_HwIDType aID, CCU_InPinSelType aInputPin, CCU_InputFilterType aFilterType, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t filterShift = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((CCT_MAX_HW_ID <= aID) || (CCU_INPINSEL_MAX < aInputPin)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((1UL != CCU_DrvRWDev[aID].isInit)) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if(aFilterType == CCU_INPUT_FILTER_SYNC) {
            filterShift = 0UL;
        } else {
            filterShift = 8UL;
        }
        if(CCU_INPINSEL_IN0  == aInputPin) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->inp_ctrl |= (CCT_INP_CTRL_SYNC_EN_IN0_MASK << filterShift);
            } else {
                CCU_CCTRegs[aID]->inp_ctrl &= ~(CCT_INP_CTRL_SYNC_EN_IN0_MASK << filterShift);
            }
        } else if (CCU_INPINSEL_IN1  == aInputPin) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->inp_ctrl |= (CCT_INP_CTRL_SYNC_EN_IN1_MASK << filterShift);
            } else {
                CCU_CCTRegs[aID]->inp_ctrl &= ~(CCT_INP_CTRL_SYNC_EN_IN1_MASK << filterShift);
            }
        } else if (CCU_INPINSEL_IN2  == aInputPin) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->inp_ctrl |= (CCT_INP_CTRL_SYNC_EN_IN2_MASK << filterShift);
            } else {
                CCU_CCTRegs[aID]->inp_ctrl &= ~(CCT_INP_CTRL_SYNC_EN_IN2_MASK << filterShift);
            }
        } else if (CCU_INPINSEL_IN3  == aInputPin) {
            if(aEnable > 0UL) {
               CCU_CCTRegs[aID]->inp_ctrl |= (CCT_INP_CTRL_SYNC_EN_IN3_MASK << filterShift);
            } else {
               CCU_CCTRegs[aID]->inp_ctrl &= ~(CCT_INP_CTRL_SYNC_EN_IN3_MASK << filterShift);
            }
        } else if (CCU_INPINSEL_ETR  == aInputPin) {
            if(aEnable > 0UL) {
               CCU_CCTRegs[aID]->inp_ctrl |= (CCT_INP_CTRL_SYNC_EN_ETR_MASK << filterShift);
            } else {
               CCU_CCTRegs[aID]->inp_ctrl &= ~(CCT_INP_CTRL_SYNC_EN_ETR_MASK << filterShift);
            }
        } else {
          retVal = BCM_ERR_NOSUPPORT;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/**
    @brief CCU input prescalar event trigger select.
    @trace #BRCM_SWARCH_CCU_DRV_EVENT_TRIGGER_SEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_IntInPscEventTriggerSelTemp(CCU_HwIDType aID, CCU_EventTrigSelMaskType eventTrigSel, uint32_t aEnable)
{
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN3_RE) > 0UL) {
         if(aEnable > 0UL) {
             CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN3_RE_MASK;
         } else {
             CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN3_RE_MASK;
         }
     }

     if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN3_FE) > 0UL) {
         if(aEnable > 0UL) {
             CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN3_FE_MASK;
         } else {
             CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN3_FE_MASK;
         }
     }

}

/**
    @brief CCU input prescalar event trigger select.
    @trace #BRCM_SWARCH_CCU_DRV_EVENT_TRIGGER_SEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_IntInPscEventTriggerSel(CCU_HwIDType aID, CCU_EventTrigSelMaskType eventTrigSel, uint32_t aEnable)
{
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN0_RE) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN0_RE_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN0_RE_MASK;
        }
    }

    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN0_FE) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN0_FE_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN0_FE_MASK;
        }
    }
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN1_RE) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN1_RE_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN1_RE_MASK;
        }
    }

    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN1_FE) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN1_FE_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN1_FE_MASK;
        }
    }
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN2_RE) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN2_RE_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN2_RE_MASK;
        }
    }

    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_PSC_IN2_FE) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_IN2_FE_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_IN2_FE_MASK;
        }
    }
    CCU_IntInPscEventTriggerSelTemp(aID, eventTrigSel, aEnable);
 }

/**
    @brief CCU compare output prescalar event trigger select.
            This is helper function to avoid CCM metric.
    @trace #BRCM_SWARCH_CCU_DRV_EVENT_TRIGGER_SEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_IntOutPscEventTriggerSelTemp(CCU_HwIDType aID, CCU_EventTrigSelMaskType eventTrigSel, uint32_t aEnable)
{
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT3) > 0UL) {
          if(aEnable > 0UL) {
              CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT3_MASK;
          } else {
              CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT3_MASK;
          }
      }

      if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT3_N) > 0UL) {
          if(aEnable > 0UL) {
              CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT3N_MASK;
          } else {
              CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT3N_MASK;
          }
      }

}

/**
    @brief CCU compare output prescalar event trigger select.
    @trace #BRCM_SWARCH_CCU_DRV_EVENT_TRIGGER_SEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_IntOutPscEventTriggerSel(CCU_HwIDType aID, CCU_EventTrigSelMaskType eventTrigSel, uint32_t aEnable)
{
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT0) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT0_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT0_MASK;
        }
    }

    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT0_N) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT0N_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT0N_MASK;
        }
    }
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT1) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT1_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT1_MASK;
        }
    }

    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT1_N) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT1N_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT1N_MASK;
        }
    }
    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT2) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT2_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT2_MASK;
        }
    }

    if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_OUT2_N) > 0UL) {
        if(aEnable > 0UL) {
            CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_OUT2N_MASK;
        } else {
            CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_OUT2N_MASK;
        }
    }
    CCU_IntOutPscEventTriggerSelTemp(aID, eventTrigSel, aEnable);
}

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

    @post TBD

    @trace #BRCM_SWREQ_CCU
    @trace #BRCM_SWARCH_CCU_DRV_EVENT_TRIGGER_SEL_PROC

    @code{.unparsed}
    Set up-down control register for the selected event trigger type
    set prescaler trigger register based on the selected event trigger type
    @endcode

*/
int32_t CCU_DrvEventTriggerSel(CCU_HwIDType aID, CCU_EventTrigSelMaskType eventTrigSel, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((CCT_MAX_HW_ID <= aID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((1UL != CCU_DrvRWDev[aID].isInit)) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_EVENT0) > 0UL) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDC_MASK_EVT0_MASK;
                CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_EVT0_MASK;
            } else {
                CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_UDC_MASK_EVT0_MASK;
                CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_EVT0_MASK;
            }
        }
        if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_EVENT1) > 0UL) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDC_MASK_EVT1_MASK;
                CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_EVT1_MASK;
            } else {
                CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_UDC_MASK_EVT1_MASK;
                CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_EVT1_MASK;
            }
        }
        if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_EVENT2) > 0UL) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDC_MASK_EVT2_MASK;
                CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_EVT2_MASK;
            } else {
                CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_UDC_MASK_EVT2_MASK;
                CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_EVT2_MASK;
            }
        }

        if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_EVENT3) > 0UL) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDC_MASK_EVT3_MASK;
                CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_EVT3_MASK;
            } else {
                CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_UDC_MASK_EVT3_MASK;
                CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_EVT3_MASK;
            }
        }

        if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_ETR_RE) > 0UL) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDC_MASK_ETR_RE_MASK;
                CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_ETR_RE_MASK;
            } else {
                CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_UDC_MASK_ETR_RE_MASK;
                CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_ETR_RE_MASK;
            }
        }
        if((eventTrigSel & CCU_EVENT_TRIG_SEL_MASK_ETR_FE) > 0UL) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aID]->udc_ctrl &= ~CCT_UDC_CTRL_UDC_MASK_ETR_FE_MASK;
                CCU_CCTRegs[aID]->psc_ctrl &= ~CCT_PSC_CTRL_MASK_ETR_FE_MASK;
            } else {
                CCU_CCTRegs[aID]->udc_ctrl |= CCT_UDC_CTRL_UDC_MASK_ETR_FE_MASK;
                CCU_CCTRegs[aID]->psc_ctrl |= CCT_PSC_CTRL_MASK_ETR_FE_MASK;
            }
        }

        CCU_IntInPscEventTriggerSel(aID, eventTrigSel, aEnable);
        CCU_IntOutPscEventTriggerSel(aID, eventTrigSel, aEnable);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_ENABLE_INTERRUPT_PROC

    @code{.unparsed}
    Set interrupt control register for the selected interrupt type and channel number
    @endcode

*/
int CCU_EnableInterrupt(CCU_HwIDType aId, CCU_InterruptType intrType, CCU_ChanType aChanNum, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((CCT_MAX_HW_ID <= aId)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((1UL != CCU_DrvRWDev[aId].isInit)) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        /* @event   CERT INT30-C
          @issue left shift (2UL * aChanNum) may wrap
          @risk   No risk
          @justification The shift value is less than 31.
         */
        if(CCU_INTERRUPT_ALL == intrType) {
            if(aEnable > 0UL) {
                CCU_CCTRegs[aId]->irq_ctrl = CCU_INTERRUPT_ALL;
            } else {
                CCU_CCTRegs[aId]->irq_ctrl &= ~CCU_INTERRUPT_ALL;
            }
        } else {
            if((intrType & CCU_INTERRUPT_CCR_A_IRQ_EN) > 0UL) {
                if(aEnable > 0UL) {
                    /* coverity[cert_int30_c_violation] */
                    /* coverity[cert_int34_c_violation] */
                    CCU_CCTRegs[aId]->irq_ctrl |= (CCT_IRQ_CTRL_CCR0_A_IRQ_EN_MASK << (2UL * aChanNum));
                } else {
                    /* coverity[cert_int30_c_violation] */
                    /* coverity[cert_int34_c_violation] */
                    CCU_CCTRegs[aId]->irq_ctrl &= (~(CCT_IRQ_CTRL_CCR0_A_IRQ_EN_MASK << (2UL * aChanNum)));
                }
            }
            if((intrType & CCU_INTERRUPT_CCR_B_IRQ_EN) > 0UL) {
                if(aEnable > 0UL) {
                    /* coverity[cert_int30_c_violation] */
                    /* coverity[cert_int34_c_violation] */
                    CCU_CCTRegs[aId]->irq_ctrl |= (CCT_IRQ_CTRL_CCR0_B_IRQ_EN_MASK << (2UL * aChanNum));
                } else {
                    /* coverity[cert_int30_c_violation] */
                    /* coverity[cert_int34_c_violation] */
                    CCU_CCTRegs[aId]->irq_ctrl &= (~(CCT_IRQ_CTRL_CCR0_B_IRQ_EN_MASK << (2UL * aChanNum)));
                }
            }
            if((intrType & CCU_INTERRUPT_UDC_IRQ_EN) > 0UL) {
                if(aEnable > 0UL) {
                    CCU_CCTEnableUDCInterrupt(aId);
                } else {
                    CCU_CCTDisableUDCInterrupt(aId);
                }
            }
            if((intrType & CCU_INTERRUPT_ETR_RE_IRQ_EN) > 0UL) {
                if(aEnable > 0UL) {
                    CCU_CCTRegs[aId]->irq_ctrl |= CCT_IRQ_CTRL_ETR_RE_IRQ_EN_MASK;
                } else {
                    CCU_CCTRegs[aId]->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_RE_IRQ_EN_MASK;
                }
            }
            if((intrType & CCU_INTERRUPT_ETR_FE_IRQ_EN) > 0UL) {
                if(aEnable > 0UL) {
                    CCU_CCTRegs[aId]->irq_ctrl |= CCT_IRQ_CTRL_ETR_FE_IRQ_EN_MASK;
                } else {
                    CCU_CCTRegs[aId]->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_FE_IRQ_EN_MASK;
                }
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_CLEAR_INTERRUPT_PROC

    @code{.unparsed}
    Clear the interrupt by writing interrupt status register with same interrupt status value
    @endcode

*/
int CCU_ClearInterrupt(CCU_HwIDType aId)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t status;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((CCT_MAX_HW_ID <= aId)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((1UL != CCU_DrvRWDev[aId].isInit)) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        status  = CCU_CCTRegs[aId]->irq_stat;
        /* Clear interrupt status */
        CCU_CCTRegs[aId]->irq_stat = status;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

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
    @trace #BRCM_SWARCH_CCU_GET_INTERRUPT_STATUS_PROC

    @code{.unparsed}
    Get the interrupt status by reading interrupt status register
    @endcode

*/
int CCU_GetInterruptStatus(CCU_HwIDType aId, uint32_t *intrStatus)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((CCT_MAX_HW_ID <= aId)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((1UL != CCU_DrvRWDev[aId].isInit)) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        *intrStatus  = CCU_CCTRegs[aId]->irq_stat;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/**
    @brief CCU timestamp handler
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvCapTimestampHandler(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                        CCU_CCTSubChanType aSubChan)
{
    uint32_t tsWrIndex;
    /* @event   CERT INT30-C
        @issue      CCU_DrvRWDev[aID].chan[aChanNum].edgeCnt++ may wrap
        @risk    No risk
        @justification Edge count is number of edge count, it has no side effect if it wraps.
      */
    /* coverity[cert_int30_c_violation] */
    CCU_DrvRWDev[aID].chan[aChanNum].edgeCnt++;
    tsWrIndex = CCU_DrvRWDev[aID].chan[aChanNum].tsWrIndex;
    if ((CCU_TIMESTAMPBUF_LINEAR == CCU_DrvRWDev[aID].chan[aChanNum].TSBufProp)
            && (tsWrIndex < CCU_DrvRWDev[aID].chan[aChanNum].bufSize)) {
        CCU_CCTGetCaptureValue(aID, aChanNum, aSubChan,
                &(CCU_DrvRWDev[aID].chan[aChanNum].TSBuf[tsWrIndex]));
        CCU_DrvRWDev[aID].chan[aChanNum].tsWrIndex++;
    }

    if (CCU_TIMESTAMPBUF_CIRCULAR == CCU_DrvRWDev[aID].chan[aChanNum].TSBufProp) {
        if (tsWrIndex == CCU_DrvRWDev[aID].chan[aChanNum].bufSize) {
            tsWrIndex = 0UL;
            CCU_DrvRWDev[aID].chan[aChanNum].tsWrIndex = 0UL;
        }
        CCU_CCTGetCaptureValue(aID, aChanNum, aSubChan,
                &(CCU_DrvRWDev[aID].chan[aChanNum].TSBuf[tsWrIndex]));
        CCU_DrvRWDev[aID].chan[aChanNum].tsWrIndex++;
    }

    if ((0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCbEn)
            && (NULL != CCU_DrvRWDev[aID].chan[aChanNum].timestampCB)
            && (0UL != CCU_DrvRWDev[aID].chan[aChanNum].notifyInterval)
            && ((CCU_DrvRWDev[aID].chan[aChanNum].edgeCnt
                    % CCU_DrvRWDev[aID].chan[aChanNum].notifyInterval) == 0UL)) {
        CCU_DrvRWDev[aID].chan[aChanNum].timestampCB();
    }
}

/**
    @brief Capture signal measurement handler.
    @trace #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvCapSigMeasHandler(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                     CCU_CCTSubChanType aSubChan)
{
    uint32_t idx;
    CCU_ValueType TS;
    uint64_t TS1, TS2, TS3, TS4;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        idx = (CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeIdx) % 2UL;
        CCU_CCTGetCaptureValue(aID, aChanNum, aSubChan, &TS);
        CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[idx] =
            (((uint64_t)CCU_DrvRWDev[aID].cntrWrapCnt << 16UL) + (uint64_t)TS);

        /* @event   CERT INT30-C
             @issue CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeIdx++ may wrap
             @risk   No risk
             @justification Edge count is number of edge count, it has no side effect if it wraps.
             */
        /* coverity[cert_int30_c_violation] */
        CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeIdx++;
    } else {
        idx = (CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeIdx) % 2UL;
        CCU_CCTGetCaptureValue(aID, aChanNum, aSubChan, &TS);
        CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[idx] =
            (((uint64_t)CCU_DrvRWDev[aID].cntrWrapCnt << 16UL) + (uint64_t)TS);
        /* @event   CERT INT30-C
         @issue CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeIdx++ may wrap
         @risk   No risk
         @justification Edge count is number of edge count, it has no side effect if it wraps.
         */
        /* coverity[cert_int30_c_violation] */
        CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeIdx++;
    }

    switch (CCU_DrvRWDev[aID].chan[aChanNum].sigMeasProp) {
        case CCU_SIGNALPROPERTY_LOW_TIME:
        TS1 = CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[0UL];
        if (TS2 > TS1) {
            CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS2 - TS1);
            CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
        } else {
            TS1 = CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[0UL];
            TS2 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[1UL];
            if (TS2 > TS1) {
                CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime =
                    (CCU_ValueType)(TS2 - TS1);
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime = 0U;
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        }
        break;
    case CCU_SIGNALPROPERTY_HIGH_TIME:
        TS1 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[0UL];
        if (TS2 > TS1) {
            CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS2 - TS1);
            CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
        } else {
            TS1 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[0UL];
            TS2 = CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[1UL];
            if (TS2 > TS1) {
                CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime =
                    (CCU_ValueType)(TS2 - TS1);
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime = 0U;
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        }
        break;
    case CCU_SIGNALPROPERTY_PERIOD_TIME:
        CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
        TS1 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[1UL];
        if (TS2 > TS1) {
            CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS2 - TS1);
        } else {
            CCU_DrvRWDev[aID].chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS1 - TS2);
        }
        break;
    case CCU_SIGNALPROPERTY_DUTY_CYCLE:
        TS1 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[0UL];
        TS3 = CCU_DrvRWDev[aID].chan[aChanNum].risingEdgeTS[1UL];
        TS4 = CCU_DrvRWDev[aID].chan[aChanNum].fallingEdgeTS[1UL];
        if (TS3 > TS1) {
            CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime =
                (CCU_ValueType)(TS3 - TS1);
            if (TS2 > TS1) {
                CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime =
                    (CCU_ValueType)(TS2 - TS1);
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime = 0U;
                CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime = 0U;
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        } else if (TS4 > TS2) {
            CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime =
                (CCU_ValueType)(TS4 - TS2);
            if (TS4 > TS3) {
                CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime =
                    (CCU_ValueType)(TS4 - TS3);
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime = 0U;
                CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime = 0U;
                CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        } else {
            CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.periodTime = 0U;
            CCU_DrvRWDev[aID].chan[aChanNum].dutyCycle.activeTime = 0U;
            CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
        }
        break;
    default:
        BCM_LOG_ERROR(0U, BCM_STATE_INIT, aChanNum, "Signal measured handler failed for channel number \n");
        break;
    }
}

/**
    @brief CCU channel irq handler
    @trace  #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
static void CCU_DrvChanHandler(CCU_HwIDType aID, CCU_ChanType aChanNum,
                                CCU_CCTSubChanType aSubChan)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aID >= CCT_MAX_HW_ID) || (CCU_CHAN_MAX <  aChanNum)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (1UL != CCU_DrvRWDev[aID].isInit) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev[aID].chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
            if (CCU_CHANMODE_CAPTURE == CCU_DrvRWDev[aID].chan[aChanNum].mode) {
                switch(CCU_DrvRWDev[aID].chan[aChanNum].capMode) {
                case CCU_CAPMODE_EDGE_COUNT:
                    CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
                    CCU_DrvRWDev[aID].chan[aChanNum].edgeCnt++;
                    break;
                case CCU_CAPMODE_EDGE_DETECT:
                    CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
                    if ((0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCbEn) &&
                            (NULL != CCU_DrvRWDev[aID].chan[aChanNum].edgeDetectCB)) {
                        CCU_DrvRWDev[aID].chan[aChanNum].edgeDetectCB();
                    }
                    break;
                case CCU_CAPMODE_SIGNAL_MEASURE:
                    CCU_DrvCapSigMeasHandler(aID, aChanNum, aSubChan);
                    break;
                case CCU_CAPMODE_TIMESTAMP:
                    CCU_DrvRWDev[aID].chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
                    CCU_DrvCapTimestampHandler(aID, aChanNum, aSubChan);
                    break;
                default:
                    BCM_LOG_ERROR(0U, BCM_STATE_INIT, aChanNum, "Channel handler failed for channel number \n");
                    break;
                }
            }

            if (CCU_CHANMODE_COMPARE == CCU_DrvRWDev[aID].chan[aChanNum].mode) {
                if ((0UL != CCU_DrvRWDev[aID].chan[aChanNum].isCbEn)
                        && (NULL != CCU_DrvRWDev[aID].chan[aChanNum].compCB)) {
                    CCU_DrvRWDev[aID].chan[aChanNum].compCB();
                }
            }

    }
    if(BCM_ERR_OK != retVal) {
        BCM_LOG_ERROR(0U, BCM_STATE_INIT, aChanNum, "Channel handler failed for channel number %d \n",retVal);
    }
    return;
}

/**
    @brief CCU UDC irq handler
    @trace  #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER

    @limitations None
*/
static void CCU_DrvUDCHandler(CCU_HwIDType aID)
{
    if (0UL != CCU_DrvRWDev[aID].isInit) {
        CCU_DrvRWDev[aID].cntrWrapCnt++;
    }
}

/** @} */

/**
    @addtogroup grp_ccudrv_ifc
    @{
*/

/**
    @brief CCU irq handler
    @trace  #BRCM_SWARCH_CCU_DRV_IRQ_HANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
void CCU_DrvIRQHandler(CCU_HwIDType aID)
{
    uint32_t regVal = CCU_CCTRegs[aID]->irq_stat;

    /* Clear interrupt status */
    CCU_CCTRegs[aID]->irq_stat = regVal;

    /* Process interrupts */
    if ((regVal & CCT_IRQ_STAT_CCR0_A_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_0, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR0_B_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_0, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_CCR1_A_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_1, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR1_B_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_1, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_CCR2_A_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_2, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR2_B_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_2, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_CCR3_A_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_3, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR3_B_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(aID, CCU_CHAN_3, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_UDC_IRQ_STAT_MASK) != 0UL) {
        CCU_DrvUDCHandler(aID);
    }

}

/** @} */
