/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_ptp_drv_impl PTP Driver Design
    @ingroup grp_xgmacdrv

    @addtogroup grp_ptp_drv_impl
    @{

    @file drivers/ethernet/xgmac/ptp/ptp_drv.c
    @brief PTP Driver Design

    @version 0.1 Initial version
*/

#include "./../rdb/xgmac_core_rdb.h"
#include <ptp.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>
#include <bcm_time.h>

/**
    @name PTP Driver Design IDs
    @{
    @brief Driver Design IDs for PTP
*/
#define BRCM_SWDSGN_PTP_DRV_DEV_TYPE                             (0xB610U) /**< @brief #PTP_DrvDevType                   */
#define BRCM_SWDSGN_PTP_DRV_DEV_GLOBAL                           (0xB611U) /**< @brief #PTP_DrvDev                       */
#define BRCM_SWDSGN_PTP_REGS_GLOBAL                              (0xB612U) /**< @brief #PTP_Regs                         */

#define BRCM_SWDSGN_PTP_DRV_INIT_PROC                            (0xB620U) /**< @brief #PTP_DrvInit                      */
#define BRCM_SWDSGN_PTP_DRV_DEINIT_PROC                          (0xB621U) /**< @brief #PTP_DrvDeInit                    */
#define BRCM_SWDSGN_PTP_DRV_OP_MODE_SET_PROC                     (0xB622U) /**< @brief #PTP_DrvOpModeSet                 */
#define BRCM_SWDSGN_PTP_DRV_OP_MODE_GET_PROC                     (0xB623U) /**< @brief #PTP_DrvOpModeGet                 */
#define BRCM_SWDSGN_PTP_DRV_CONFIG_SET_PROC                      (0xB624U) /**< @brief #PTP_DrvConfigSet                 */
#define BRCM_SWDSGN_PTP_DRV_CONFIG_GET_PROC                      (0xB625U) /**< @brief #PTP_DrvConfigGet                 */
#define BRCM_SWDSGN_PTP_DRV_TIMESTAMP_SET_PROC                   (0xB626U) /**< @brief #PTP_DrvTimestampSet              */
#define BRCM_SWDSGN_PTP_DRV_TIMESTAMP_GET_PROC                   (0xB627U) /**< @brief #PTP_DrvTimestampGet              */
#define BRCM_SWDSGN_PTP_DRV_SYSTEM_TIME_SET_PROC                 (0xB628U) /**< @brief #PTP_DrvSystemTimeSet             */
#define BRCM_SWDSGN_PTP_DRV_SYSTEM_TIME_GET_PROC                 (0xB629U) /**< @brief #PTP_DrvSystemTimeGet             */
#define BRCM_SWDSGN_PTP_DRV_PULSE_SET_PROC                       (0xB62CU) /**< @brief #PTP_DrvPulseSet                  */
#define BRCM_SWDSGN_PTP_DRV_PULSE_GET_PROC                       (0xB62DU) /**< @brief #PTP_DrvPulseGet                  */
#define BRCM_SWDSGN_PTP_DRV_PACKET_GEN_SET_PROC                  (0xB62EU) /**< @brief #PTP_DrvPacketGenSet              */
#define BRCM_SWDSGN_PTP_DRV_PACKET_GEN_GET_PROC                  (0xB62FU) /**< @brief #PTP_DrvPacketGenGet              */

#define BRCM_SWDSGN_PTP_INT_SNAPSHOT_CONFIG_SET_PROC             (0xB630U) /**< @brief #PTP_IntSnapshotConfigSet         */
#define BRCM_SWDSGN_PTP_INT_SNAPSHOT_CONFIG_GET_PROC             (0xB631U) /**< @brief #PTP_IntSnapshotConfigGet         */
#define BRCM_SWDSGN_PTP_INT_PROTOCOL_CONFIG_SET_PROC             (0xB632U) /**< @brief #PTP_IntProtocolConfigSet         */
#define BRCM_SWDSGN_PTP_INT_PROTOCOL_CONFIG_GET_PROC             (0xB633U) /**< @brief #PTP_IntProtocolConfigGet         */
#define BRCM_SWDSGN_PTP_INT_CONFIG_SET_GRP0_PROC                 (0xB634U) /**< @brief #PTP_IntConfigSetGrp0             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_SET_GRP1_PROC                 (0xB635U) /**< @brief #PTP_IntConfigSetGrp1             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_SET_GRP2_PROC                 (0xB636U) /**< @brief #PTP_IntConfigSetGrp2             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_SET_GRP3_PROC                 (0xB637U) /**< @brief #PTP_IntConfigSetGrp3             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_GET_GRP0_PROC                 (0xB638U) /**< @brief #PTP_IntConfigGetGrp0             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_GET_GRP1_PROC                 (0xB639U) /**< @brief #PTP_IntConfigGetGrp1             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_GET_GRP2_PROC                 (0xB63AU) /**< @brief #PTP_IntConfigGetGrp2             */
#define BRCM_SWDSGN_PTP_INT_CONFIG_GET_GRP3_PROC                 (0xB63BU) /**< @brief #PTP_IntConfigGetGrp3             */

#define BRCM_SWDSGN_PTP_INT_MSG_GEN_MODE_SET_PROC                (0xB641U) /**< @brief #PTP_IntMsgGenModeSet             */
#define BRCM_SWDSGN_PTP_INT_PKT_GEN_ENABLE_SET_PROC              (0xB642U) /**< @brief #PTP_IntPktGenEnableSet           */
#define BRCM_SWDSGN_PTP_INT_PKT_GEN_AUTO_SET_PROC                (0xB643U) /**< @brief #PTP_IntPktGenAutoSet             */
#define BRCM_SWDSGN_PTP_INT_PKT_GEN_TRIGGER_SET_PROC             (0xB644U) /**< @brief #PTP_IntPktGenTriggerSet          */
#define BRCM_SWDSGN_PTP_INT_INTERVAL_RATIO_SET_PROC              (0xB645U) /**< @brief #PTP_IntIntervalRatioSet          */
#define BRCM_SWDSGN_PTP_INT_DOMAIN_NUM_SET_PROC                  (0xB646U) /**< @brief #PTP_IntDomainNumSet              */
#define BRCM_SWDSGN_PTP_INT_SRC_PORT_IDENTITY_SET_PROC           (0xB647U) /**< @brief #PTP_IntSrcPortIdentitySet        */
#define BRCM_SWDSGN_PTP_INT_MSG_GEN_MODE_GET_PROC                (0xB648U) /**< @brief #PTP_IntMsgGenModeGet             */
#define BRCM_SWDSGN_PTP_INT_PKT_GEN_ENABLE_GET_PROC              (0xB649U) /**< @brief #PTP_IntPktGenEnableGet           */
#define BRCM_SWDSGN_PTP_INT_PKT_GEN_AUTO_GET_PROC                (0xB64AU) /**< @brief #PTP_IntPktGenAutoGet             */
#define BRCM_SWDSGN_PTP_INT_PKT_GEN_TRIGGER_GET_PROC             (0xB64BU) /**< @brief #PTP_IntPktGenTriggerGet          */
#define BRCM_SWDSGN_PTP_INT_INTERVAL_RATIO_GET_PROC              (0xB64CU) /**< @brief #PTP_IntIntervalRatioGet          */
#define BRCM_SWDSGN_PTP_INT_DOMAIN_NUM_GET_PROC                  (0xB64DU) /**< @brief #PTP_IntDomainNumGet              */
#define BRCM_SWDSGN_PTP_INT_SRC_PORT_IDENTITY_GET_PROC           (0xB64EU) /**< @brief #PTP_IntSrcPortIdentityGet        */

#define BRCM_SWDSGN_PTP_INT_TS_SNAPSHOT_SET_PROC                 (0xB650U) /**< @brief #PTP_IntTsSnapshotSet             */
#define BRCM_SWDSGN_PTP_INT_TS_CORR_SET_PROC                     (0xB651U) /**< @brief #PTP_IntTsCorrSet                 */
#define BRCM_SWDSGN_PTP_INT_TS_DOMAIN_INCR_SET_PROC              (0xB652U) /**< @brief #PTP_IntTsDomainIncrSet           */
#define BRCM_SWDSGN_PTP_INT_TS_AUX_SNAPSHOT_SET_PROC             (0xB653U) /**< @brief #PTP_IntTsAuxSnapshotSet          */
#define BRCM_SWDSGN_PTP_INT_TS_SNAPSHOT_GET_PROC                 (0xB654U) /**< @brief #PTP_IntTsSnapshotGet             */
#define BRCM_SWDSGN_PTP_INT_TS_CORR_GET_PROC                     (0xB655U) /**< @brief #PTP_IntTsCorrGet                 */
#define BRCM_SWDSGN_PTP_INT_TS_DOMAIN_INCR_GET_PROC              (0xB656U) /**< @brief #PTP_IntTsDomainIncrGet           */
#define BRCM_SWDSGN_PTP_INT_TS_AUX_SNAPSHOT_GET_PROC             (0xB657U) /**< @brief #PTP_IntTsAuxSnapshotGet          */

/** @} */

/**
    @trace #BRCM_SWARCH_PTP_DRV_INIT_PROC
    @trace #BRCM_SWARCH_PTP_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_PTP_DRV_OP_MODE_SET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_OP_MODE_GET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_INIT
*/
static XGMAC_CORE_RDBType * const PTP_Regs[XGMAC_CORE_MAX_HW_ID] =
{

    (XGMAC_CORE_RDBType *)XG_CORE_BASE,

#if (XGMAC_CORE_MAX_HW_ID > 1UL)
#error "XGMAC_CORE_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @name PTP Driver device type
    @{
    @brief PTP Driver device type struct

    @trace #BRCM_SWARCH_PTP_DRV_INIT_PROC
    @trace #BRCM_SWARCH_PTP_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_PTP_DRV_OP_MODE_SET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_OP_MODE_GET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_INIT
 */
typedef struct sPTP_DrvDevType {
    PTP_StateType   state; /**< @brief State of the driver */
} PTP_DrvDevType;
/** @} */

/**
    @brief PTP hardware context instance

    @trace #BRCM_SWARCH_PTP_DRV_INIT_PROC
    @trace #BRCM_SWARCH_PTP_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_PTP_DRV_OP_MODE_SET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_OP_MODE_GET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_INIT
*/
static PTP_DrvDevType PTP_DrvDev[XGMAC_CORE_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @brief Snapshot related configuration

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntSnapshotConfigSet(PTP_HwIDType hwId, PTP_ConfigType *aConfig)
{
    if (0U != (aConfig->snapMsgType & PTP_SNAPSHOT_MSG_EVENT)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK ;
    }

    if (0U != (aConfig->snapMsgType & PTP_SNAPSHOT_MSG_ALL)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENALL_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENALL_MASK ;
    }

    if (0U != (aConfig->snapMsgType & PTP_SNAPSHOT_MSG_MASTER)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK ;
    }

    PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK ;
    if (0U != (aConfig->snapMsgType & PTP_SNAPSHOT_MSG_E2E_TC)) {
        PTP_Regs[hwId]->mac_timestamp_control |= (0x2UL << XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT) ;
    } else if (0U != (aConfig->snapMsgType & PTP_SNAPSHOT_MSG_P2P_TC)) {
        PTP_Regs[hwId]->mac_timestamp_control |= (0x3UL << XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT) ;
    } else if (0U != (aConfig->snapMsgType & PTP_SNAPSHOT_MSG_PDELAY)) {
        PTP_Regs[hwId]->mac_timestamp_control |= (0x1UL << XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT) ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK ;
    }
    return BCM_ERR_OK;
}

/**
    @brief To get snapshot related configuration

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntSnapshotConfigGet(PTP_HwIDType hwId, XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control, PTP_ConfigType *aConfig)
{
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK)) {
        aConfig->snapMsgType |= PTP_SNAPSHOT_MSG_EVENT ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENALL_MASK)) {
        aConfig->snapMsgType |= PTP_SNAPSHOT_MSG_ALL ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK)) {
        aConfig->snapMsgType |= PTP_SNAPSHOT_MSG_MASTER ;
    }

    if (0x2UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT)) {
        aConfig->snapMsgType |= PTP_SNAPSHOT_MSG_E2E_TC ;
    }
    if (0x3UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT)) {
        aConfig->snapMsgType |= PTP_SNAPSHOT_MSG_P2P_TC ;
    }
    if (0x1UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT)) {
        aConfig->snapMsgType |= PTP_SNAPSHOT_MSG_PDELAY ;
    }

    return BCM_ERR_OK ;
}

/**
    @brief Set the protocol related configurations
           Like PTP over Ethernet, IPV6, IPV4, 1588V2, 802P1AS

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntProtocolConfigSet(PTP_HwIDType hwId, PTP_ConfigType *aConfig)
{
    if (0U != (aConfig->protocolEn & PTP_PROTOCOL_ETH)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPENA_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPENA_MASK ;
    }

    if (0U != (aConfig->protocolEn & PTP_PROTOCOL_IPV6)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPV6ENA_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPV6ENA_MASK ;
    }

    if (0U != (aConfig->protocolEn & PTP_PROTOCOL_IPV4)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPV4ENA_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPV4ENA_MASK ;
    }

    if (0U != (aConfig->protocolEn & PTP_PROTOCOL_1588V2)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSVER2ENA_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSVER2ENA_MASK ;
    }

    if (0U != (aConfig->protocolEn & PTP_PROTOCOL_802P1AS)) {
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_AV8021ASMEN_MASK ;
    } else {
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_AV8021ASMEN_MASK ;
    }
    return BCM_ERR_OK;
}

/**
    @brief Get protocol related configuration
           Like PTP over Ethernet, IPV6, IPV4, 1588V2, 802P1AS

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntProtocolConfigGet(PTP_HwIDType hwId, XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control, PTP_ConfigType *aConfig)
{
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPENA_MASK)) {
        aConfig->protocolEn |= PTP_PROTOCOL_ETH ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPV6ENA_MASK)) {
        aConfig->protocolEn |= PTP_PROTOCOL_IPV6 ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSIPV4ENA_MASK)) {
        aConfig->protocolEn |= PTP_PROTOCOL_IPV4 ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSVER2ENA_MASK)) {
        aConfig->protocolEn |= PTP_PROTOCOL_1588V2 ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_AV8021ASMEN_MASK)) {
        aConfig->protocolEn |= PTP_PROTOCOL_802P1AS ;
    }
    return BCM_ERR_OK;
}

/**
    @brief To set PTP Group0 configuration like
           Enable/Disable time stamp
           System time update method
           Initializing time stamp
           Updating system time

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigSetGrp0(PTP_HwIDType hwId, PTP_ConfigOptType aConfOpt, PTP_ConfigType *aConfig)
{
    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_ENABLE)) {
        if( PTP_TS_DISABLE == aConfig->tsEnable) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENA_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENA_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_UPDT_METHOD)) {
        if(PTP_SYS_TIME_UPD_METHOD_COARSE == aConfig->sysTimeUpdateMethod) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSCFUPDT_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSCFUPDT_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_INIT)) {
        if(PTP_SYS_TIME_INIT == aConfig->sysTimeInit) {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSINIT_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSINIT_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_UPDT)) {
        if(PTP_SYS_TIME_UPDATE_DONE == aConfig->sysTimeUpdate) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSUPDT_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSUPDT_MASK ;
        }
    }
    return BCM_ERR_OK;
}

/**
    @brief To get PTP Group0 configuration like
           Enable/Disable time stamp
           System time update method
           Initializing time stamp
           Updating system time

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigGetGrp0(PTP_HwIDType hwId, XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control, PTP_ConfigType *aConfig)
{
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENA_MASK)) {
        aConfig->tsEnable = PTP_TS_ENABLE ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSCFUPDT_MASK)) {
        aConfig->sysTimeUpdateMethod = PTP_SYS_TIME_UPD_METHOD_FINE ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSINIT_MASK)) {
        aConfig->sysTimeInit = PTP_SYS_TIME_INIT ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSUPDT_MASK)) {
        aConfig->sysTimeUpdate = PTP_SYS_TIME_UPDATE ;
    }
    return BCM_ERR_OK;
}

/**
    @brief To set PTP Group1 configuration like
           Enable/Disable Time stamp Interrupt Trigger
           Update Addednd
           Set Timestamp Digital or Binary Rollover Control
           Enable/Disable MAC Address for PTP Packet Filtering

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigSetGrp1(PTP_HwIDType hwId, PTP_ConfigOptType aConfOpt, PTP_ConfigType *aConfig)
{
    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_INTR_TRIGGER)) {
        if( PTP_TS_INTR_TRIGGER_DIS == aConfig->tsIntrTrig) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSTRIG_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSTRIG_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_UPDT_ADDEND)) {
        if( PTP_SYS_TIME_UPDATE_ADDEND_DONE == aConfig->sysTimeUpdateAddend) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSADDREG_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSADDREG_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_ROLLOVR_CTRL)) {
        if( PTP_TS_ROLLOVER_BINARY == aConfig->tsRollover) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSCTRLSSR_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSCTRLSSR_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_EN_MACADDR)) {
        if( PTP_ADDR_FILTER_NONE == aConfig->addrFilter) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENMACADDR_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENMACADDR_MASK ;
        }
    }
    return BCM_ERR_OK;
}

/**
    @brief To get PTP Group1 configuration like
           Get time stamp Interrupt Trigger Enable/Disable status
           Get status of Addednd Update
           Get Digital/Binary Rollover Control configuration
           Get Enable/Disable status PTP Packet Filtering

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigGetGrp1(PTP_HwIDType hwId, XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control, PTP_ConfigType *aConfig)
{
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSTRIG_MASK)) {
        aConfig->tsIntrTrig = PTP_TS_INTR_TRIGGER_EN ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSADDREG_MASK)) {
        aConfig->sysTimeUpdateAddend = PTP_SYS_TIME_UPDATE_ADDEND ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSCTRLSSR_MASK)) {
        aConfig->tsRollover = PTP_TS_ROLLOVER_DIGITAL ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENMACADDR_MASK)) {
        aConfig->addrFilter = PTP_ADDR_FILTER_DST_MAC ;
    }
    return BCM_ERR_OK;
}

/**
    @brief To set PTP Group2 configuration like
           Set system time input configuration
           Set Transmit time stamp Status Mode
           Set protocol related configuration
           Set snapshot related configuration

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigSetGrp2(PTP_HwIDType hwId, PTP_ConfigOptType aConfOpt, PTP_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_SYS_TIME_INPUT)) {
        if( PTP_SYS_TIME_INPUT_INTERNAL == aConfig->sysTimeInput) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_ESTI_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_ESTI_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_TX_STATUS_MODE)) {
        if( PTP_TX_TS_STATUS_IGNORE == aConfig->txTsStatusMode) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TXTSSTSM_MASK ;
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TXTSSTSM_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_PROTOCOL)) {
        ret = PTP_IntProtocolConfigSet(hwId, aConfig);
    }

    if ((BCM_ERR_OK == ret) && (0U != (aConfOpt & PTP_CONFIG_OPT_TS_SNAPSHOT))) {
        ret = PTP_IntSnapshotConfigSet(hwId, aConfig);
    }
    return ret ;
}

/**
    @brief To get PTP Group2 configuration like
           Get system time input configuration
           Get Transmit time stamp Status Mode
           Get protocol related configuration
           Get snapshot related configuration

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigGetGrp2(PTP_HwIDType hwId, XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control, PTP_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_ESTI_MASK)) {
        aConfig->sysTimeInput = PTP_SYS_TIME_INPUT_EXTERNAL ;
    }
    if(0UL != (mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TXTSSTSM_MASK)) {
        aConfig->txTsStatusMode = PTP_TX_TS_STATUS_OVERWRITE ;
    }

    ret = PTP_IntProtocolConfigGet(hwId, mac_timestamp_control, aConfig);
    if (BCM_ERR_OK == ret) {
        ret = PTP_IntSnapshotConfigGet(hwId, mac_timestamp_control, aConfig);
    }
    return ret;
}

/**
    @brief To set PTP Group3 configuration like
           Set Auxiliary Snapshot 0 Enable
           Set Auxiliary Snapshot FIFO Clear

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigSetGrp3(PTP_HwIDType hwId, PTP_ConfigOptType aConfOpt, PTP_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_AUX_SNAP0_ENABLE)) {
        if( 0U == aConfig->auxSnap0En) {
            PTP_Regs[hwId]->mac_auxiliary_control &= ~XGMAC_CORE_MAC_AUXILIARY_CONTROL_ATSEN0_MASK ;
        } else {
            PTP_Regs[hwId]->mac_auxiliary_control |=  XGMAC_CORE_MAC_AUXILIARY_CONTROL_ATSEN0_MASK ;
        }
    }

    if(0UL != (aConfOpt & PTP_CONFIG_OPT_TS_AUX_SNAP0_FIFO_CLR)) {
        if( 0U == aConfig->auxSnap0FifoClr) {
            PTP_Regs[hwId]->mac_auxiliary_control &= ~XGMAC_CORE_MAC_AUXILIARY_CONTROL_ATSFC_MASK ;
        } else {
            PTP_Regs[hwId]->mac_auxiliary_control |=  XGMAC_CORE_MAC_AUXILIARY_CONTROL_ATSFC_MASK ;
        }
    }
    return ret ;
}

/**
    @brief To get PTP Group3 configuration like
           Get Auxiliary Snapshot 0 Enable
           Get Auxiliary Snapshot FIFO Clear

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntConfigGetGrp3(PTP_HwIDType hwId, PTP_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
    XGMAC_CORE_MAC_AUXILIARY_CONTROL_TYPE mac_auxiliary_control = 0;
    mac_auxiliary_control = PTP_Regs[hwId]->mac_auxiliary_control ;

    if(0UL != (mac_auxiliary_control & XGMAC_CORE_MAC_AUXILIARY_CONTROL_ATSEN0_MASK)) {
        aConfig->auxSnap0En = 1U ;
    }
    if(0UL != (mac_auxiliary_control & XGMAC_CORE_MAC_AUXILIARY_CONTROL_ATSFC_MASK)) {
        aConfig->auxSnap0FifoClr = 1U ;
    }
    return ret;
}

/**
    @brief To enable automatic packet generation

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntPktGenEnableSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_ENABLE)) {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_PTOEN_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_PTOEN_MASK ;
    }
    return ret ;
}

/**
    @brief To get enable status of automatic packet generation

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntPktGenEnableGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if (0U != (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_PTOEN_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_ENABLE ;
    }
    return ret ;
}

/**
    @brief To set automatic packet generation

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntPktGenAutoSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_AUTO_SYNC)) {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_ASYNCEN_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_ASYNCEN_MASK ;
    }

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_AUTO_DLY_REQ)) {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_DRRDIS_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_DRRDIS_MASK ;
    }

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_AUTO_DLY_RESP)) {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_DRRDIS_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_DRRDIS_MASK ;
    }

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_AUTO_PDLY_REQ)) {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_APDREQEN_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_APDREQEN_MASK ;
    }

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_AUTO_PDLY_RESP)) {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_PDRDIS_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_PDRDIS_MASK ;
    }
    return ret ;
}

/**
    @brief To get automatic packet generation

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntPktGenAutoGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if (0U != (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_ASYNCEN_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_AUTO_SYNC ;
    }
    if (0U == (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_DRRDIS_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_AUTO_DLY_REQ ;
    }
    if (0U == (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_DRRDIS_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_AUTO_DLY_RESP ;
    }
    if (0U != (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_APDREQEN_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_AUTO_PDLY_REQ ;
    }
    if (0U == (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_PDRDIS_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_AUTO_PDLY_RESP ;
    }
    return ret ;
}

/**
    @brief To set packet generation trigger

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntPktGenTriggerSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_TRIGGER_SYNC)) {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_ASYNCTRIG_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_ASYNCTRIG_MASK ;
    }

    if (0U != (aPktgenConfig->mode & PTP_PKTGEN_TRIGGER_PDLY_REQ)) {
        PTP_Regs[hwId]->mac_pto_control |=  XGMAC_CORE_MAC_PTO_CONTROL_APDREQTRIG_MASK ;
    } else {
        PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_APDREQTRIG_MASK ;
    }
    return ret ;
}

/**
    @brief To get packet generation trigger

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntPktGenTriggerGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if (0U != (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_ASYNCTRIG_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_TRIGGER_SYNC ;
    }
    if (0U != (PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_APDREQTRIG_MASK)) {
        aPktgenConfig->mode |= PTP_PKTGEN_TRIGGER_PDLY_REQ ;
    }
    return ret ;
}

/**
    @brief To set message generation mode

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntMsgGenModeSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;

    ret = PTP_IntPktGenEnableSet(hwId, aPktgenConfig);

    if(BCM_ERR_OK == ret) {
        ret = PTP_IntPktGenAutoSet(hwId, aPktgenConfig);
    }

    if(BCM_ERR_OK == ret) {
        ret = PTP_IntPktGenTriggerSet(hwId, aPktgenConfig);
    }
    return ret ;
}

/**
    @brief To get message generation mode

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntMsgGenModeGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;

    ret = PTP_IntPktGenEnableGet(hwId, aPktgenConfig);

    if(BCM_ERR_OK == ret) {
        ret = PTP_IntPktGenAutoGet(hwId, aPktgenConfig);
    }

    if(BCM_ERR_OK == ret) {
        ret = PTP_IntPktGenTriggerGet(hwId, aPktgenConfig);
    }
    return ret ;
}

/**
    @brief To set interval ratio

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntIntervalRatioSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;

    PTP_Regs[hwId]->mac_log_message_interval &=  ~XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_DRSYNCR_MASK ;
    PTP_Regs[hwId]->mac_log_message_interval |=  ((aPktgenConfig->interval.ratioDlyReqToSync & 0x7UL) << XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_DRSYNCR_SHIFT);

    PTP_Regs[hwId]->mac_log_message_interval &=  ~XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LSI_MASK ;
    PTP_Regs[hwId]->mac_log_message_interval |=  ((aPktgenConfig->interval.sync & 0xFFUL) << XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LSI_SHIFT);

    PTP_Regs[hwId]->mac_log_message_interval &=  ~XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LMPDRI_MASK ;
    PTP_Regs[hwId]->mac_log_message_interval |=  ((aPktgenConfig->interval.pDelayReq & 0xFFUL) << XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LMPDRI_SHIFT);

    return ret ;
}

/**
    @brief To get interval ratio

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntIntervalRatioGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    aPktgenConfig->interval.ratioDlyReqToSync = (uint8_t)((PTP_Regs[hwId]->mac_log_message_interval & XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_DRSYNCR_MASK) >> XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_DRSYNCR_SHIFT);
    aPktgenConfig->interval.sync              = (uint8_t)((PTP_Regs[hwId]->mac_log_message_interval & XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LSI_MASK)     >> XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LSI_SHIFT);
    aPktgenConfig->interval.pDelayReq         = (uint8_t)((PTP_Regs[hwId]->mac_log_message_interval & XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LMPDRI_MASK)  >> XGMAC_CORE_MAC_LOG_MESSAGE_INTERVAL_LMPDRI_SHIFT);
    return ret ;
}

/**
    @brief To set domain number

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntDomainNumSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    PTP_Regs[hwId]->mac_pto_control &= ~XGMAC_CORE_MAC_PTO_CONTROL_DN_MASK ;
    PTP_Regs[hwId]->mac_pto_control |= ((aPktgenConfig->domainNum & 0xFFUL) << XGMAC_CORE_MAC_PTO_CONTROL_DN_SHIFT);
    return ret ;
}

/**
    @brief To get domain number

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntDomainNumGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    aPktgenConfig->domainNum = (uint8_t)((PTP_Regs[hwId]->mac_pto_control & XGMAC_CORE_MAC_PTO_CONTROL_DN_MASK) >> XGMAC_CORE_MAC_PTO_CONTROL_DN_SHIFT);
    return ret ;
}

/**
    @brief To set source port identity

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntSrcPortIdentitySet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    PTP_Regs[hwId]->mac_source_port_identity0 = (((aPktgenConfig->srcPortIdentity[1U] & 0xFFFFUL) << 16UL) | (aPktgenConfig->srcPortIdentity[0U] & 0xFFFFUL)) ;
    PTP_Regs[hwId]->mac_source_port_identity1 = (((aPktgenConfig->srcPortIdentity[3U] & 0xFFFFUL) << 16UL) | (aPktgenConfig->srcPortIdentity[2U] & 0xFFFFUL)) ;
    PTP_Regs[hwId]->mac_source_port_identity2 = (aPktgenConfig->srcPortIdentity[4U] & 0xFFFFUL);
    return ret ;
}

/**
    @brief To get source port identity

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
static int PTP_IntSrcPortIdentityGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
    aPktgenConfig->srcPortIdentity[0U] = (uint16_t)( PTP_Regs[hwId]->mac_source_port_identity0 & 0xFFFFUL);
    aPktgenConfig->srcPortIdentity[1U] = (uint16_t)((PTP_Regs[hwId]->mac_source_port_identity0 & 0xFFFF0000UL) >> 16UL);
    aPktgenConfig->srcPortIdentity[2U] = (uint16_t)( PTP_Regs[hwId]->mac_source_port_identity1 & 0xFFFFUL);
    aPktgenConfig->srcPortIdentity[3U] = (uint16_t)((PTP_Regs[hwId]->mac_source_port_identity1 & 0xFFFF0000UL) >> 16UL);
    aPktgenConfig->srcPortIdentity[4U] = (uint16_t)( PTP_Regs[hwId]->mac_source_port_identity2 & 0xFFFFUL);
    return ret ;
}

/**
    @brief To set Tx time stamp snapshot

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsSnapshotSet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeSpecType *aTsValue)
{
    int32_t ret = BCM_ERR_OK ;

    if(PTP_TIMESTAMP_SNAPSHOT_TX == aTsClass) {
        PTP_Regs[hwId]->mac_tx_timestamp_status_seconds     = (uint32_t)(aTsValue->second & XGMAC_CORE_MAC_TX_TIMESTAMP_STATUS_SECONDS_TXTSSTSHI_MASK);
        PTP_Regs[hwId]->mac_tx_timestamp_status_nanoseconds = (aTsValue->nanosecond & XGMAC_CORE_MAC_TX_TIMESTAMP_STATUS_NANOSECONDS_TXTSSLO_MASK);
    }
    return ret ;
}

/**
    @brief To get Tx time stamp snapshot

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsSnapshotGet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeStatusType *aTsStatus)
{
    int32_t ret = BCM_ERR_OK ;
    if(PTP_TIMESTAMP_SNAPSHOT_TX == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.second     = (uint64_t)PTP_Regs[hwId]->mac_tx_timestamp_status_seconds;
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_tx_timestamp_status_nanoseconds;

        aTsStatus->txSnapshotStatus.overflow = (uint8_t)(PTP_Regs[hwId]->mac_timestamp_status  & XGMAC_CORE_MAC_TIMESTAMP_STATUS_TSSOVF_MASK);
        aTsStatus->txSnapshotStatus.ready    = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_TXTSC_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_TXTSC_SHIFT);
        aTsStatus->txSnapshotStatus.numSnap  = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_TTSNS_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_TTSNS_SHIFT);
        aTsStatus->txSnapshotStatus.pktId    = (uint16_t)(PTP_Regs[hwId]->mac_tx_timestamp_status_pktid & XGMAC_CORE_MAC_TX_TIMESTAMP_STATUS_PKTID_MASK);
    }
    return ret ;
}

/**
    @brief To set MAC timestamp ingress/egress correction, Asymmetry ingress/egress correction

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsCorrSet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeSpecType *aTsValue)
{
    int32_t ret = BCM_ERR_OK ;

    if(PTP_TIMESTAMP_CORR_IGR == aTsClass) {
        PTP_Regs[hwId]->mac_timestamp_ingress_corr_nanosecond = aTsValue->nanosecond;
    }
    if(PTP_TIMESTAMP_CORR_EGR == aTsClass) {
        PTP_Regs[hwId]->mac_timestamp_egress_corr_nanosecond  = aTsValue->nanosecond;
    }

    if(PTP_TIMESTAMP_CORR_ASYM_IGR == aTsClass) {
        PTP_Regs[hwId]->mac_timestamp_ingress_asym_corr = aTsValue->nanosecond;
    }
    if(PTP_TIMESTAMP_CORR_ASYM_EGR == aTsClass) {
        PTP_Regs[hwId]->mac_timestamp_egress_asym_corr  = aTsValue->nanosecond;
    }
    return ret;
}

/**
    @brief To get MAC timestamp ingress/egress correction, Asymmetry ingress/egress correction

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsCorrGet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeStatusType *aTsStatus)
{
    int32_t ret = BCM_ERR_OK ;
    if(PTP_TIMESTAMP_CORR_IGR == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_timestamp_ingress_corr_nanosecond ;
    }
    if(PTP_TIMESTAMP_CORR_EGR == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_timestamp_egress_corr_nanosecond ;
    }
    if(PTP_TIMESTAMP_CORR_ASYM_IGR == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_timestamp_ingress_asym_corr ;
    }
    if(PTP_TIMESTAMP_CORR_ASYM_EGR == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_timestamp_egress_asym_corr ;
    }
    return ret ;
}

/**
    @brief To set Rx/Tx domain time increment value

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsDomainIncrSet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeSpecType *aTsValue)
{
    int32_t ret = BCM_ERR_OK ;

    if(PTP_TIMESTAMP_DOMAIN_INCR_RX == aTsClass) {
        PTP_Regs[hwId]->mac_rx_domain_time_incr = aTsValue->nanosecond;
    }

    if(PTP_TIMESTAMP_DOMAIN_INCR_TX == aTsClass) {
        PTP_Regs[hwId]->mac_tx_domain_time_incr = aTsValue->nanosecond;
    }
    return ret;
}

/**
    @brief To get Rx/Tx domain time increment value

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsDomainIncrGet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeStatusType *aTsStatus)
{
    int32_t ret = BCM_ERR_OK ;
    if(PTP_TIMESTAMP_DOMAIN_INCR_RX == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_rx_domain_time_incr;
    }
    if(PTP_TIMESTAMP_DOMAIN_INCR_TX == aTsClass) {
        aTsStatus->txSnapshotStatus.timestamp.nanosecond = PTP_Regs[hwId]->mac_tx_domain_time_incr;
    }
    return ret ;
}

/**
    @brief To set auxiliary time stamp snapshot

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsAuxSnapshotSet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeSpecType *aTsValue)
{
    int32_t ret = BCM_ERR_OK ;
    if(PTP_TIMESTAMP_AUX_SNAP_0 == aTsClass) {
        PTP_Regs[hwId]->mac_auxiliary_timestamp_seconds     = (uint32_t)(aTsValue->second & XGMAC_CORE_MAC_AUXILIARY_TIMESTAMP_SECONDS_AYXTSHI_MASK);
        PTP_Regs[hwId]->mac_auxiliary_timestamp_nanoseconds = (aTsValue->nanosecond & XGMAC_CORE_MAC_AUXILIARY_TIMESTAMP_NANOSECONDS_AUXTSLO_MASK);
    }
    return ret;
}

/**
    @brief To get auxiliary time stamp snapshot

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
static int PTP_IntTsAuxSnapshotGet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeStatusType *aTsStatus)
{
    int32_t ret = BCM_ERR_OK ;
    if (0UL != (PTP_TIMESTAMP_AUX_SNAP & aTsClass)) {
        aTsStatus->auxSnapStatus.ready                = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_AUXTSTRIG_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_AUXTSTRIG_SHIFT);
        aTsStatus->auxSnapStatus.numAuxsnap           = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_ATSNS_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_ATSNS_SHIFT);
        aTsStatus->auxSnapStatus.triggerMiss          = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_ATSSTM_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_ATSSTM_SHIFT);
        aTsStatus->auxSnapStatus.triggerId            = ((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_ATSSTN_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_ATSSTN_SHIFT);
        aTsStatus->auxSnapStatus.timestamp.second     = (uint64_t)PTP_Regs[hwId]->mac_auxiliary_timestamp_seconds ;
        aTsStatus->auxSnapStatus.timestamp.nanosecond = (PTP_Regs[hwId]->mac_auxiliary_timestamp_nanoseconds & XGMAC_CORE_MAC_AUXILIARY_TIMESTAMP_NANOSECONDS_AUXTSLO_MASK);
    }
    return ret ;
}

/** @brief PTP Initialization

    @code{.unparsed}
    Check hardware instance with max supported device
    Mask the Timestamp Trigger interrupt by clearing the bit 12 of MAC_Interrupt_Enable register.
    Initialize Timestamp.
    @endcode

    @trace  #BRCM_SWARCH_PTP_DRV_INIT_PROC
    @trace  #BRCM_SWREQ_PTP_INIT
*/
int PTP_DrvInit(PTP_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        PTP_Regs[hwId]->mac_interrupt_enable  &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK ;
        PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSENA_MASK ;

        PTP_DrvDev[hwId].state = PTP_STATE_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}


/** @brief PTP De-Initialization

    @code{.unparsed}
    Check hardware instance with max supported device
    State to PTP_STATE_UNINIT;
    @endcode

    @trace  #BRCM_SWARCH_PTP_DRV_DEINIT_PROC
    @trace  #BRCM_SWREQ_PTP_INIT
*/
int PTP_DrvDeInit(PTP_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        PTP_DrvDev[hwId].state = PTP_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Set PTP Operating mode

    @code{.unparsed}
    Check hardware instance with max supported device.

    Set the respective PTP operating modes PTP_OP_MODE_CLOCK_MASTER,
    PTP_OP_MODE_CLOCK_SLAVE, PTP_OP_MODE_E2E_TRANSPARENT,
    PTP_OP_MODE_P2P_TRANSPARENT, PTP_OP_MODE_PDELAY_SUPPORT,
    PTP_OP_MODE_PDELAY_CLOCK_MASTER, PTP_OP_MODE_PDELAY_CLOCK_SLAVE,
    PTP_OP_MODE_PDELAY_E2E_TRANSPARENT in mac_timestamp_control registers.
    @endcode

    @trace  #BRCM_SWARCH_PTP_DRV_OP_MODE_SET_PROC
    @trace  #BRCM_SWREQ_PTP_OPERATION_MODE
*/
int PTP_DrvOpModeSet(PTP_HwIDType hwId, PTP_OpModeType aOpMode)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK ;
        if (PTP_OP_MODE_E2E_TRANSPARENT == aOpMode) {
            PTP_Regs[hwId]->mac_timestamp_control |= (0x2UL << XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT) ;
        }
        if (PTP_OP_MODE_P2P_TRANSPARENT == aOpMode) {
            PTP_Regs[hwId]->mac_timestamp_control |= (0x3UL << XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT) ;
        }
        if(0U != (aOpMode & PTP_OP_MODE_PDELAY_SUPPORT)) {
            PTP_Regs[hwId]->mac_timestamp_control |= (0x1UL << XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT) ;
            if(0U != (aOpMode & PTP_OP_MODE_E2E_TRANSPARENT)) {
                PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK ;
            } else {
                PTP_Regs[hwId]->mac_timestamp_control |= XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK ;
            }
        } else {
            PTP_Regs[hwId]->mac_timestamp_control |= XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK ;
        }

        if(0U != (aOpMode & PTP_OP_MODE_CLOCK_MASTER)) {
            PTP_Regs[hwId]->mac_timestamp_control |= XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK ;
        }
        if(0U != (aOpMode & PTP_OP_MODE_CLOCK_SLAVE)) {
            PTP_Regs[hwId]->mac_timestamp_control &= ~XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get PTP Operating mode

    @code{.unparsed}
    Check hardware instance and pointer of the operating modes.
    Get PTP operating modes from the mac_timestamp_control registers
    @endcode

    @trace  #BRCM_SWARCH_PTP_DRV_OP_MODE_GET_PROC
    @trace  #BRCM_SWREQ_PTP_OPERATION_MODE
*/
int PTP_DrvOpModeGet(PTP_HwIDType hwId, PTP_OpModeType *aOpMode)
{
    int32_t ret = BCM_ERR_OK ;
    XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if(NULL == aOpMode) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        mac_timestamp_control = PTP_Regs[hwId]->mac_timestamp_control ;
        *aOpMode = 0U ;
        if (0x0UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_SHIFT)) {
            *aOpMode = PTP_OP_MODE_CLOCK_SLAVE ;
        } else {
            *aOpMode = PTP_OP_MODE_CLOCK_MASTER ;
        }
        if (0x2UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT)) {
            *aOpMode = PTP_OP_MODE_E2E_TRANSPARENT ;
        }
        if (0x3UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT)) {
            *aOpMode = PTP_OP_MODE_P2P_TRANSPARENT ;
        }
        if (0x1UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_SNAPTYPSEL_SHIFT)) {
            *aOpMode = PTP_OP_MODE_PDELAY_SUPPORT ;
            if (0x0UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSEVNTENA_SHIFT)) {
                *aOpMode |= PTP_OP_MODE_E2E_TRANSPARENT ;
            } else if (0x0UL == ((mac_timestamp_control & XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSMSTRENA_SHIFT)) {
                *aOpMode |= PTP_OP_MODE_CLOCK_SLAVE ;
            } else {
                *aOpMode |= PTP_OP_MODE_CLOCK_MASTER ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Set PTP Configuration

    @code{.unparsed}
    Check hardware instance and pointer to PTP configuration parameters.
    Set the protocol related configurations Like PTP over Ethernet, IPV6, IPV4, 1588V2, 802P1AS
    Enable MAC Address for PTP Packet Filtering.
    Initialize and Enable time stamp.
    Perform System time, Time stamp and snapshot related configuration.
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvConfigSet(PTP_HwIDType hwId, PTP_ConfigOptType aConfOpt, PTP_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (0U != (aConfOpt & PTP_CONFIG_OPT_TS_GRP0)) {
            ret = PTP_IntConfigSetGrp0(hwId, aConfOpt, aConfig);
        }

        if ((BCM_ERR_OK == ret) && (0U != (aConfOpt & PTP_CONFIG_OPT_TS_GRP1))) {
            ret = PTP_IntConfigSetGrp1(hwId, aConfOpt, aConfig);
        }

        if ((BCM_ERR_OK == ret) && (0U != (aConfOpt & PTP_CONFIG_OPT_TS_GRP2))) {
            ret = PTP_IntConfigSetGrp2(hwId, aConfOpt, aConfig);
        }

        if ((BCM_ERR_OK == ret) && (0U != (aConfOpt & PTP_CONFIG_OPT_TS_GRP3))) {
            ret = PTP_IntConfigSetGrp3(hwId, aConfOpt, aConfig);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get PTP Configuration

    @code{.unparsed}
    Check hardware instance and pointer to PTP configuration parameters.

    Get the protocol related configurations, snapshot related configurations,
    time stamp related configurations, system time related configurations,
    Address filtering configuration and time stamp enable configuration from
    mac_timestamp_control register.

    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvConfigGet(PTP_HwIDType hwId, PTP_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
    XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TYPE mac_timestamp_control = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        mac_timestamp_control = PTP_Regs[hwId]->mac_timestamp_control ;

        BCM_MemSet(aConfig, 0U, sizeof(PTP_ConfigType));

        ret = PTP_IntConfigGetGrp0(hwId, mac_timestamp_control, aConfig);
        if (BCM_ERR_OK == ret) {
            ret = PTP_IntConfigGetGrp1(hwId, mac_timestamp_control, aConfig);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntConfigGetGrp2(hwId, mac_timestamp_control, aConfig);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntConfigGetGrp3(hwId, aConfig);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Set PTP Timestamp

    @code{.unparsed}
    Check hardware instance and pointer to PTP time stamp configuration parameters.
    Based on time stamp class :
    Set Tx time stamp snapshot
    Set MAC timestamp ingress/egress correction, Asymmetry ingress/egress correction
    Set Rx/Tx domain time increment value
    Set auxiliary time stamp snapshot
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvTimestampSet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeSpecType *aTsValue)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == aTsValue) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = PTP_IntTsSnapshotSet(hwId, aTsClass, aTsValue);
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntTsCorrSet(hwId, aTsClass, aTsValue);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntTsDomainIncrSet(hwId, aTsClass, aTsValue);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntTsAuxSnapshotSet(hwId, aTsClass, aTsValue);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/** @brief Get PTP Timestamp

    @code{.unparsed}
    Check hardware instance and pointer to PTP time stamp configuration parameters.
    Based on time stamp class :
    Get Tx time stamp snapshot
    Get MAC timestamp ingress/egress correction, Asymmetry ingress/egress correction
    Get Rx/Tx domain time increment value
    Get auxiliary time stamp snapshot
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_TIMESTAMP_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvTimestampGet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeStatusType *aTsStatus)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == aTsStatus) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = PTP_IntTsSnapshotGet(hwId, aTsClass, aTsStatus);
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntTsCorrGet(hwId, aTsClass, aTsStatus);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntTsDomainIncrGet(hwId, aTsClass, aTsStatus);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntTsAuxSnapshotGet(hwId, aTsClass, aTsStatus);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/** @brief Set PTP System Time

    @code{.unparsed}
    Check hardware instance and pointer to PTP system time parameters.
    Based on PTP_SYS_TIME_OP_VALUE option, update mac_system_time_higher_word_seconds, mac_system_time_seconds and mac_system_time_nanoseconds registers
    Based on PTP_SYS_TIME_OP_INCREMENT option, update mac_sub_second_increment register.
    Based on PTP_SYS_TIME_OP_ADDEND option, update mac_timestamp_addend register.
    Based on PTP_SYS_TIME_OP_UPDATE option, update mac_system_time_seconds_update and mac_system_time_nanoseconds_update register.
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_SYSTEM_TIME_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvSystemTimeSet(PTP_HwIDType hwId, PTP_SysTimeOpType aSysTimeOp, PTP_SysTimeType *aSysTime)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == aSysTime) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(PTP_SYS_TIME_OP_VALUE == aSysTimeOp) {
            PTP_Regs[hwId]->mac_system_time_higher_word_seconds = (uint32_t)((aSysTime->systimeValue.second >> 32UL) & XGMAC_CORE_MAC_SYSTEM_TIME_HIGHER_WORD_SECONDS_TSHWR_MASK);
            PTP_Regs[hwId]->mac_system_time_seconds             = (uint32_t)(aSysTime->systimeValue.second & XGMAC_CORE_MAC_SYSTEM_TIME_SECONDS_TSS_MASK);
            PTP_Regs[hwId]->mac_system_time_nanoseconds         = aSysTime->systimeValue.nanosecond;
        }
        if(PTP_SYS_TIME_OP_INCREMENT == aSysTimeOp) {
            PTP_Regs[hwId]->mac_sub_second_increment &= ~XGMAC_CORE_MAC_SUB_SECOND_INCREMENT_SSINC_MASK ;
            PTP_Regs[hwId]->mac_sub_second_increment |= ((aSysTime->systimeValue.nanosecond << XGMAC_CORE_MAC_SUB_SECOND_INCREMENT_SSINC_SHIFT) & XGMAC_CORE_MAC_SUB_SECOND_INCREMENT_SSINC_MASK);
        }

        if(PTP_SYS_TIME_OP_ADDEND == aSysTimeOp) {
            PTP_Regs[hwId]->mac_timestamp_addend = aSysTime->systimeValue.subNanosecond;
            if(PTP_SYS_TIME_UPDATE_ADDEND == aSysTime->sysTimeConfig.sysTimeUpdateAddend) {
                PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSADDREG_MASK ;
            }
        }

        if(PTP_SYS_TIME_OP_UPDATE == aSysTimeOp) {
            PTP_Regs[hwId]->mac_system_time_seconds_update      = (uint32_t)(aSysTime->systimeValue.second & XGMAC_CORE_MAC_SYSTEM_TIME_SECONDS_UPDATE_TSS_MASK);
            PTP_Regs[hwId]->mac_system_time_nanoseconds_update &= ~XGMAC_CORE_MAC_SYSTEM_TIME_NANOSECONDS_UPDATE_TSSS_MASK ;
            PTP_Regs[hwId]->mac_system_time_nanoseconds_update |= (uint32_t)(aSysTime->systimeValue.nanosecond & XGMAC_CORE_MAC_SYSTEM_TIME_NANOSECONDS_UPDATE_TSSS_MASK);
            if(PTP_SYS_TIME_SIGN_POS == aSysTime->sysTimeConfig.sysTimeSign) {
                PTP_Regs[hwId]->mac_system_time_nanoseconds_update &= ~XGMAC_CORE_MAC_SYSTEM_TIME_NANOSECONDS_UPDATE_ADDSUB_MASK;
            } else {
                PTP_Regs[hwId]->mac_system_time_nanoseconds_update |= XGMAC_CORE_MAC_SYSTEM_TIME_NANOSECONDS_UPDATE_ADDSUB_MASK;
            }
            if(PTP_SYS_TIME_UPDATE == aSysTime->sysTimeConfig.sysTimeUpdate) {
                PTP_Regs[hwId]->mac_timestamp_control |=  XGMAC_CORE_MAC_TIMESTAMP_CONTROL_TSUPDT_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/** @brief Get PTP System Time

    @code{.unparsed}
    Check hardware instance and pointer to PTP system time parameters.
    Based on PTP_SYS_TIME_OP_VALUE option, get system time seconds and nanoseconds information from mac_system_time_higher_word_seconds, mac_system_time_seconds and mac_system_time_nanoseconds registers.
    Based on PTP_SYS_TIME_OP_INCREMENT option, get system time nanoseconds information from mac_sub_second_increment register.
    Based on PTP_SYS_TIME_OP_ADDEND option, get system time sub nanoseconds information from mac_timestamp_addend register.
    Based on PTP_SYS_TIME_OP_UPDATE option, get system time seconds and nanoseconds information from mac_system_time_seconds_update and mac_system_time_nanoseconds_update register.
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_SYSTEM_TIME_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvSystemTimeGet(PTP_HwIDType hwId, PTP_SysTimeOpType aSysTimeOp, PTP_SysTimeType *aSysTime)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == aSysTime) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(PTP_SYS_TIME_OP_VALUE == aSysTimeOp) {
            aSysTime->systimeValue.second     = (((uint64_t)PTP_Regs[hwId]->mac_system_time_higher_word_seconds << 32UL) | (uint64_t)PTP_Regs[hwId]->mac_system_time_seconds) ;
            aSysTime->systimeValue.nanosecond = PTP_Regs[hwId]->mac_system_time_nanoseconds;
        }

        if(PTP_SYS_TIME_OP_INCREMENT == aSysTimeOp) {
            aSysTime->systimeValue.nanosecond = ((PTP_Regs[hwId]->mac_sub_second_increment & XGMAC_CORE_MAC_SUB_SECOND_INCREMENT_SSINC_MASK) >> XGMAC_CORE_MAC_SUB_SECOND_INCREMENT_SSINC_SHIFT);
        }

        if(PTP_SYS_TIME_OP_ADDEND == aSysTimeOp) {
            aSysTime->systimeValue.subNanosecond = PTP_Regs[hwId]->mac_timestamp_addend ;
        }

        if(PTP_SYS_TIME_OP_UPDATE == aSysTimeOp) {
            aSysTime->systimeValue.nanosecond = (PTP_Regs[hwId]->mac_system_time_nanoseconds_update & XGMAC_CORE_MAC_SYSTEM_TIME_NANOSECONDS_UPDATE_TSSS_MASK);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/** @brief Set pulse configuration

    @code{.unparsed}
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_PULSE_SET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvPulseSet(PTP_HwIDType hwId, PTP_PulseConfigType *aPulseConfig)
{
    int32_t ret = BCM_ERR_OK ;
    if(0UL != (aPulseConfig->aCfgMask & PTP_PULSE_CONFIG_MASK_FLEX_CMD)) {
        PTP_Regs[hwId]->mac_pps_control &= ~(XGMAC_CORE_MAC_PPS_CONTROL_PPSCTRL0_PPSCMD0_MASK) ;
        PTP_Regs[hwId]->mac_pps_control |=  aPulseConfig->ppsFlexCmd;
    }
    if(0UL != (aPulseConfig->aCfgMask & PTP_PULSE_CONFIG_MASK_TGT_TIME)) {
        PTP_Regs[hwId]->mac_pps0_target_time_seconds      = (uint32_t)(aPulseConfig->targetTime.second & XGMAC_CORE_MAC_PPS0_TARGET_TIME_SECONDS_TSTRH0_MASK);
        PTP_Regs[hwId]->mac_pps0_target_time_nanoseconds &= ~XGMAC_CORE_MAC_PPS0_TARGET_TIME_NANOSECONDS_TTSL0_MASK ;
        PTP_Regs[hwId]->mac_pps0_target_time_nanoseconds |= (uint32_t)(aPulseConfig->targetTime.nanosecond & XGMAC_CORE_MAC_PPS0_TARGET_TIME_NANOSECONDS_TTSL0_MASK);
    }
    return ret ;
}

/** @brief Get pulse configuration

    @code{.unparsed}
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_PULSE_GET_PROC
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
int PTP_DrvPulseGet(PTP_HwIDType hwId, PTP_PulseConfigType *aPulseConfig, PTP_PulseStatusType *aPulseStatus)
{
    int32_t ret = BCM_ERR_OK ;
    aPulseConfig->ppsFlexCmd            = (uint8_t)(PTP_Regs[hwId]->mac_pps_control & XGMAC_CORE_MAC_PPS_CONTROL_PPSCTRL0_PPSCMD0_MASK);
    aPulseConfig->targetTime.second     = PTP_Regs[hwId]->mac_pps0_target_time_seconds ;
    aPulseConfig->targetTime.nanosecond = (PTP_Regs[hwId]->mac_pps0_target_time_nanoseconds & XGMAC_CORE_MAC_PPS0_TARGET_TIME_NANOSECONDS_TTSL0_MASK) ;
    aPulseStatus->busy    = (uint8_t)((PTP_Regs[hwId]->mac_pps0_target_time_nanoseconds & XGMAC_CORE_MAC_PPS0_TARGET_TIME_NANOSECONDS_TRGTBUSY0_MASK) >> XGMAC_CORE_MAC_PPS0_TARGET_TIME_NANOSECONDS_TRGTBUSY0_SHIFT);
    aPulseStatus->reached = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_TSTARGT0_MASK)   >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_TSTARGT0_SHIFT);
    aPulseStatus->error   = (uint8_t)((PTP_Regs[hwId]->mac_timestamp_status & XGMAC_CORE_MAC_TIMESTAMP_STATUS_TSTRGTERR0_MASK) >> XGMAC_CORE_MAC_TIMESTAMP_STATUS_TSTRGTERR0_SHIFT);
    return ret ;
}

/** @brief Set automatic PTP Message configuration

    @code{.unparsed}
    Check hardware instance and pointer to packet generation configurations.
    Set the packet generation configuration parameters (mode, interval, domainNum, srcPortIdentity)
    in the following registers mac_pto_control, mac_log_message_interval, mac_source_port_identity0
    mac_source_port_identity1, mac_source_port_identity2 registers.
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
int PTP_DrvPacketGenSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == aPktgenConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = PTP_IntMsgGenModeSet(hwId, aPktgenConfig);
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntIntervalRatioSet(hwId, aPktgenConfig);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntDomainNumSet(hwId, aPktgenConfig);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntSrcPortIdentitySet(hwId, aPktgenConfig);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get automatic PTP Message configuration

    @code{.unparsed}
    Check hardware instance and pointer to packet generation configurations.
    Get the packet generation configuration parameters (mode, interval, domainNum, srcPortIdentity)
    from the following registers mac_pto_control, mac_log_message_interval, mac_source_port_identity0
    mac_source_port_identity1, mac_source_port_identity2 registers.
    @endcode

    @trace #BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
int PTP_DrvPacketGenGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= XGMAC_CORE_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (PTP_DrvDev[hwId].state == PTP_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == aPktgenConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = PTP_IntMsgGenModeGet(hwId, aPktgenConfig);
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntIntervalRatioGet(hwId, aPktgenConfig);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntDomainNumGet(hwId, aPktgenConfig);
        }
        if(BCM_ERR_OK == ret) {
            ret = PTP_IntSrcPortIdentityGet(hwId, aPktgenConfig);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @} */
