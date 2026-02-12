/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvr_common_ifc Ethernet XCVR Common Defines
    @ingroup grp_transceiverdrv

    @addtogroup grp_xcvr_common_ifc
    @{

    @file xcvr_common.h

    @brief Ethernet XCVR common defines
    This file contains the interfaces for the Ethernet XCVR common defines.

    @version 1.0 Initial version
*/

#ifndef XCVR_COMMON_H
#define XCVR_COMMON_H

#include <string.h>
#include <bcm_err.h>
#include <bcm_utils.h>
/**
    @name Ethernet XCVR API IDs
    @{
    @brief API IDs for Ethernet XCVR
*/
#define BRCM_SWARCH_ETHXCVR_MODE_TYPE                  (0x8801U)  /**< @brief #ETHXCVR_ModeType              */
#define BRCM_SWARCH_ETHXCVR_LINKSTATE_TYPE             (0x8802U)  /**< @brief #ETHXCVR_LinkStateType         */
#define BRCM_SWARCH_ETHXCVR_SPEED_TYPE                 (0x8803U)  /**< @brief #ETHXCVR_SpeedType             */
#define BRCM_SWARCH_ETHXCVR_DUPLEXMODE_TYPE            (0x8804U)  /**< @brief #ETHXCVR_DuplexModeType        */
#define BRCM_SWARCH_ETHXCVR_BUSMODE_TYPE               (0x8805U)  /**< @brief #ETHXCVR_BusModeType           */
#define BRCM_SWARCH_ETHXCVR_FLOWCONTROL_TYPE           (0x8806U)  /**< @brief #ETHXCVR_FlowControlType       */
#define BRCM_SWARCH_ETHXCVR_ACCESSMODE_TYPE            (0x8807U)  /**< @brief #ETHXCVR_AccessModeType        */
#define BRCM_SWARCH_ETHXCVR_PHYMEDIA_TYPE              (0x8808U)  /**< @brief #ETHXCVR_PhyMediaType          */
#define BRCM_SWARCH_ETHXCVR_WAKEUPREASON_TYPE          (0x8809U)  /**< @brief #ETHXCVR_WakeupReasonType      */
#define BRCM_SWARCH_ETHXCVR_BOOLEAN_TYPE               (0x880AU)  /**< @brief #ETHXCVR_BooleanType           */
#define BRCM_SWARCH_ETHXCVR_AUTONEGSTATUS_TYPE         (0x880BU)  /**< @brief #ETHXCVR_AutoNegStatusType     */
#define BRCM_SWARCH_ETHXCVR_MAX_PORTS_MACRO            (0x880CU)  /**< @brief #ETHXCVR_MAX_PORTS             */
#define BRCM_SWARCH_ETHXCVR_CONFIG_MAGIC_MACRO         (0x880DU)  /**< @brief #ETHXCVR_CONFIG_MAGIC          */
#define BRCM_SWARCH_ETHXCVR_ID_TYPE                    (0x880EU)  /**< @brief #ETHXCVR_IDType                */
#define BRCM_SWARCH_ETHXCVR_BUSCONFIG_TYPE             (0x8810U)  /**< @brief #ETHXCVR_PhyConfigType         */
#define BRCM_SWARCH_ETHXCVR_PHYCONFIG_TYPE             (0x8811U)  /**< @brief #ETHXCVR_BusConfigType         */
#define BRCM_SWARCH_ETHXCVR_PORTCONFIG_TYPE            (0x8812U)  /**< @brief #ETHXCVR_PortConfigType        */
#define BRCM_SWARCH_ETHXCVR_CONFIG_TYPE                (0x8813U)  /**< @brief #ETHXCVR_ConfigType            */
#define BRCM_SWARCH_ETHXCVR_CONFIGLEGACY_TYPE          (0x8814U)  /**< @brief #ETHXCVR_ConfigLegacyType      */
#define BRCM_SWARCH_ETHXCVR_PORTCONFIGLEGACY_TYPE      (0x8815U)  /**< @brief #ETHXCVR_PortConfigLegacyType  */
#define BRCM_SWARCH_ETHXCVR_MAX_DRIVER_PARAMS_MACRO    (0x8816U)  /**< @brief #ETHXCVR_MAX_DRIVER_PARAMS     */
#define BRCM_SWARCH_ETHXCVR_CFG_VER_LEGACY_MACRO       (0x8817U)  /**< @brief #ETHXCVR_CFG_VER_LEGACY        */
#define BRCM_SWARCH_ETHXCVR_TC10_STATE_TYPE            (0x8818U)  /**< @brief #ETHXCVR_TC10StateType         */
#define BRCM_SWARCH_ETHXCVR_TC10_MODE_TYPE             (0x8819U)  /**< @brief #ETHXCVR_TC10ModeType          */
#define BRCM_SWARCH_ETHXCVR_STATS_TYPE                 (0x881AU)  /**< @brief #ETHXCVR_StatsType             */
#define BRCM_SWARCH_ETHXCVR_STATUS_TYPE                (0x881BU)  /**< @brief #ETHXCVR_StatusType            */
#define BRCM_SWARCH_ETHXCVR_KEY_TYPE                   (0x881CU)  /**< @brief #ETHXCVR_KeyType               */
#define BRCM_SWARCH_ETHXCVR_LOOPBACK_MODE_TYPE         (0x881DU)  /**< @brief #ETHXCVR_LoopbackModeType      */
#define BRCM_SWARCH_ETHXCVR_CABLE_STATUS_TYPE          (0x881EU)  /**< @brief #ETHXCVR_CableStatusType       */
#define BRCM_SWARCH_ETHXCVR_PHYID_TYPE                 (0x881FU)  /**< @brief #ETHXCVR_PhyIDType             */
#define BRCM_SWARCH_ETHXCVR_STATE_TYPE                 (0x8820U)  /**< @brief #ETHXCVR_StateType             */
#define BRCM_SWARCH_ETHXCVR_IRQ_PARAMS_TYPE            (0x8821U)  /**< @brief #ETHXCVR_IrqParamsType         */
#define BRCM_SWARCH_ETHXCVR_CFG_GLOBAL                 (0x8822U)  /**< @brief #ETHXCVR_Cfg                   */
#define BRCM_SWARCH_ETHXCVR_CONFIGUNION_TYPE           (0x8823U)  /**< @brief #ETHXCVR_ConfigUnionType       */
#define BRCM_SWARCH_ETHXCVR_NOTIFYLINKSTATUS_TYPE      (0x8824U)  /**< @brief #ETHXCVR_NotifyLinkStatusType  */
#define BRCM_SWARCH_ETHXCVR_LINKSTATUSMASK_TYPE        (0x8826U)  /**< @brief #ETHXCVR_LinkStatusMaskType    */
#define BRCM_SWARCH_ETHXCVR_WAKEUP_EVENT_TYPE          (0x8827U)  /**< @brief #ETHXCVR_WakeupEventType       */
#define BRCM_SWARCH_ETHXCVR_NOTIFYTC10INFO_TYPE        (0x8828U)  /**< @brief #ETHXCVR_NotifyTC10InfoType    */
#define BRCM_SWARCH_ETHXCVR_TC10PHY_TYPE               (0x882AU)  /**< @brief #ETHXCVR_TC10PhyType           */
#define BRCM_SWARCH_ETHXCVR_TC10PORTCONTEXT_TYPE       (0x882BU)  /**< @brief #ETHXCVR_TC10PortContextType   */
#define BRCM_SWARCH_ETHXCVR_TC10PORTCONFIG_TYPE        (0x882CU)  /**< @brief #ETHXCVR_TC10PortConfigType    */
#define BRCM_SWARCH_ETHXCVR_TC10CONFIG_TYPE            (0x882DU)  /**< @brief #ETHXCVR_TC10ConfigType        */
#define BRCM_SWARCH_ETHXCVR_TC10_CONFIG_MAGIC_MACRO    (0x882EU)  /**< @brief #ETHXCVR_TC10_CONFIG_MAGIC     */
#define BRCM_SWARCH_ETHXCVR_TC10_CONTEXT_MAGIC_MACRO   (0x882FU)  /**< @brief #ETHXCVR_TC10_CONTEXT_MAGIC    */
#define BRCM_SWARCH_ETHXCVR_EEE_MODE_TYPE              (0x8830U)  /**< @brief #ETHXCVR_EEEModeType           */
#define BRCM_SWARCH_ETHXCVR_TC10_CFG_FLAG_TYPE         (0x8831U)  /**< @brief #ETHXCVR_TC10CfgFlagType       */
#define BRCM_SWARCH_ETHXCVR_ACD_BRPHYCONFIG_TMP_TYPE   (0x8832U)  /**< @brief #ETHXCVR_ACDBrPhyConfigTempType */
#define BRCM_SWARCH_ETHXCVR_ACD_CONFIG_TMP_TYPE        (0x8833U)  /**< @brief #ETHXCVR_ACDConfigTmpType      */
#define BRCM_SWARCH_ETHXCVR_ACD_PARAMS_MACRO           (0x8834U)  /**< @brief #ETHXCVR_ACD_PARAMS            */
#define BRCM_SWARCH_ETHXCVR_ACD_PARAMS_TYPE            (0x8835U)  /**< @brief #ETHXCVR_AcdParamsType         */
#define BRCM_SWARCH_ETHXCVR_ACD_BRPHYINITCONFIG_TYPE   (0x8836U)  /**< @brief #ETHXCVR_ACDBrPhyInitConfigTempType */
#define BRCM_SWARCH_ETHXCVR_ACD_BRPHYRUNCONFIG_TYPE    (0x8837U)  /**< @brief #ETHXCVR_ACDBrPhyRunConfigTempType */
#define BRCM_SWARCH_ETHXCVR_EXT_PHY_HEALTH_INFO_TYPE   (0x8838U)  /**< @brief #ETHXCVR_ExtPhyHealthInfoType */
#define BRCM_SWARCH_ETHXCVR_CABLE_DIAG_RESULT_TYPE     (0x8839U)  /**< @brief #ETHXCVR_CableDiagResultType */
#define BRCM_SWARCH_ETHXCVR_PCS_MODE_TYPE              (0x883AU)  /**< @brief #ETHXCVR_PcsModeType */
/** @} */

/**
    @brief Number of ports supported

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_MAX_PORTS          (2U)

/**
    @brief Magic number for XCVR configuration

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_CONFIG_MAGIC   (0x52564358UL)

/**
    @brief Magic number for TC10 configuration

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_TC10_CONFIG_MAGIC   (0x54434647UL)

/**
    @brief Magic number for TC10 context

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_TC10_CONTEXT_MAGIC   (0x54435458UL)

/**
    @brief Number of driver parameters

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_MAX_DRIVER_PARAMS  (8U)

/**
    @brief Configuration legacy version

    Macro to define the legacy version number

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_CFG_VER_LEGACY (0x00000101UL)

/**
    @brief Ethernet transceiver ID

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_IDType;       /**< @brief Select transceiver type. Only one transceiver will be active at a given time */
#define ETHXCVR_ID_CU_PHY      (0U)   /**< @brief Transceiver ID 0 is used to select 100M/1G BASE_T internal PHY */
#define ETHXCVR_ID_SERDES_PHY  (1U)   /**< @brief Transceiver ID 1 is used to select 2500BASE-X/2500BASE-R external PHY */

/**
    @brief Ethernet transceiver link status mask

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint64_t ETHXCVR_LinkStatusMaskType;

/**
    @brief Transceiver Link Notification data

    Transceiver Link notification data type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_NotifyLinkStatusType {
    ETHXCVR_LinkStatusMaskType  linkUpMask;     /**< @brief link up mask */
    ETHXCVR_LinkStatusMaskType  linkDownMask;   /**< @brief link down mask */
    uint32_t                    reserved[6UL];
} ETHXCVR_NotifyLinkStatusType;

/**
    @name ETHXCVR_ModeType
    @{
    @brief Ethernet transceiver mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_ModeType; /**< @brief ETHXCVR_Mode typedef */
#define ETHXCVR_MODE_RESET          (0U) /**< @brief Mode is in Reset */
#define ETHXCVR_MODE_DOWN           (1U) /**< @brief Transceiver down or TC10 Sleep */
#define ETHXCVR_MODE_ACTIVE         (2U) /**< @brief Transceiver active */
#define ETHXCVR_MODE_ISOLATE        (3U) /**< @brief Isolate PHY from MII */
#define ETHXCVR_MODE_ACTIVE_WAKEUP  (4U) /**< @brief Transceiver active along with a wake-up */
#define ETHXCVR_MODE_FORCE_SLEEP    (5U) /**< @brief Transceiver force Sleep */
#define ETHXCVR_MODE_SLEEP_ABORT    (6U) /**< @brief Transceiver sleep abort */
 #define ETHXCVR_MODE_MAX            (ETHXCVR_MODE_SLEEP_ABORT) /**< @brief Max value supported */
/** @} */

/**
    @name ETHXCVR_LinkStateType
    @{
    @brief Transceiver link status

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_LinkStateType;    /**< @brief ETHXCVR_LinkState typedef */
#define ETHXCVR_LINKSTATE_DOWN      (1U)  /**< @brief Link down */
#define ETHXCVR_LINKSTATE_ACTIVE    (2U)  /**< @brief Link up */
/** @} */

/**
    @name ETHXCVR_SpeedType
    @{
    @brief Ethernet transceiver speed

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_SpeedType;         /**< @brief ETHXCVR_Speed typedef */
#define ETHXCVR_SPEED_10MBPS       (0x01U) /**< @brief 10Mbps */
#define ETHXCVR_SPEED_100MBPS      (0x02U) /**< @brief 100Mbps */
#define ETHXCVR_SPEED_1000MBPS     (0x04U) /**< @brief 1000Mbps */
#define ETHXCVR_SPEED_2500MBPS     (0x08U) /**< @brief 2.5G-X */
#define ETHXCVR_SPEED_2500MBPS_X   (0x08U) /**< @brief 2.5G-X */
#define ETHXCVR_SPEED_2500MBPS_R   (0x09U) /**< @brief 2.5G-R */
#define ETHXCVR_SPEED_5000MBPS     (0x10U) /**< @brief 5G-X */
#define ETHXCVR_SPEED_5000MBPS_X   (0x10U) /**< @brief 5G-X */
#define ETHXCVR_SPEED_5000MBPS_R   (0x11U) /**< @brief 5G-R */
#define ETHXCVR_SPEED_5000MBPS_KR  (0x12U) /**< @brief 5G-KR */
#define ETHXCVR_SPEED_10000MBPS    (0x20U) /**< @brief 10G */
#define ETHXCVR_SPEED_10000MBPS_R  (0x21U) /**< @brief 10G-R */
#define ETHXCVR_SPEED_10000MBPS_KR (0x21U) /**< @brief 10G-KR */
#define ETHXCVR_SPEED_VALID_MASK   (0x3FU) /**< @brief Mask indicating valid bits */
/** @} */

/**
    @name ETHXCVR_DuplexModeType
    @{
    @brief Transceiver duplex mode

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_DuplexModeType; /**< @brief ETHXCVR_DuplexMode typedef */
#define ETHXCVR_DUPLEXMODE_HALF (0x1U)  /**< @brief Half duplex */
#define ETHXCVR_DUPLEXMODE_FULL (0x2U)  /**< @brief Full duplex */
/** @} */

/**
    @name ETHXCVR_BusModeType
    @brief Mode of the data bus connecting the MAC and the PHY. It could be
           an MII like bus or an integrated PHY where the bus is not exposed.
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_BusModeType;       /**< @brief ETHXCVR_BusModeType typedef */
#define ETHXCVR_BUSMODE_INTG          (1U) /**< @brief Integrated PHY */
#define ETHXCVR_BUSMODE_RGMII         (2U) /**< @brief RGMII bus */
#define ETHXCVR_BUSMODE_RVMII         (3U) /**< @brief RVMII bus */
#define ETHXCVR_BUSMODE_SGMII         (4U) /**< @brief SGMII bus */
#define ETHXCVR_BUSMODE_RMII          (5U) /**< @brief RMII bus */
#define ETHXCVR_BUSMODE_MII           (6U) /**< @brief MII bus */
#define ETHXCVR_BUSMODE_PCIE          (7U) /**< @brief PCIE bus */
#define ETHXCVR_BUSMODE_XFI           (8U) /**< @brief XFI bus */
#define ETHXCVR_BUSMODE_USXGMII       (9U) /**< @brief USXGMII bus */
/** @} */

/**
    @name ETHXCVR_FlowControlType
    @brief Transceiver Flow control
    This will be the preferred flow control when auto-negotiation is enabled
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_FlowControlType;
#define ETHXCVR_FLOWCONTROL_NONE   (0U) /**< @brief No pause */
#define ETHXCVR_FLOWCONTROL_TXONLY (1U) /**< @brief Transmit pause only */
#define ETHXCVR_FLOWCONTROL_TXRX   (2U) /**< @brief Both transmit and receive pause */
#define ETHXCVR_FLOWCONTROL_RXONLY (3U) /**< @brief Receive pause only */
/** @} */

/**
    @name Transceiver access mode
    @brief Access mode for the PHY. Bus is assumed to be integrated and hence
    memory mapped
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_AccessModeType; /**< @brief ETHXCVR_AccessModeType typedef */
#define ETHXCVR_ACCESSMODE_MMAP       (1U) /**< @brief Memory mapped access */
#define ETHXCVR_ACCESSMODE_MDIO       (2U) /**< @brief MDIO access */
#define ETHXCVR_ACCESSMODE_MDIO_CL45  (2U) /**< @brief MDIO CL45 Reg access */
#define ETHXCVR_ACCESSMODE_MDIO_CL22  (3U) /**< @brief MDIO CL22 Reg access */
/** @} */

/**
    @name ETHXCVR_PhyMediaType
    @brief Physical medium for connecting to external world
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_PhyMediaType;     /**< @brief ETHXCVR_PhyMediaType typedef */
#define ETHXCVR_PHYMEDIA_NONE        (0U) /**< @brief Invalid */
#define ETHXCVR_PHYMEDIA_10BASET1    (1U) /**< @brief 10Base-T1 */
#define ETHXCVR_PHYMEDIA_10BASET     (2U) /**< @brief 10Base-T */
#define ETHXCVR_PHYMEDIA_100BASET1   (3U) /**< @brief 100Base-T1 */
#define ETHXCVR_PHYMEDIA_100BASETX   (4U) /**< @brief 100Base-TX */
#define ETHXCVR_PHYMEDIA_1000BASET1  (5U) /**< @brief 1000Base-T1 */
#define ETHXCVR_PHYMEDIA_1000BASET   (6U) /**< @brief 1000Base-T */
#define ETHXCVR_PHYMEDIA_1000BASEX   (7U) /**< @brief 1000Base-X */
#define ETHXCVR_PHYMEDIA_2500BASET1  (8U) /**< @brief 2.5G-BASET1 */
#define ETHXCVR_PHYMEDIA_5000BASET1  (9U) /**< @brief 5G-BASET1 */
#define ETHXCVR_PHYMEDIA_10GBASET1   (10U) /**< @brief 10G-BASET1 */
/** @} */

/**
    @brief Macro to compose the ACD parameters

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
#define ETHXCVR_ACD_PARAMS(a, b, c) ((((a) << ETHXCVR_ACD_PARAMS_LPTYPE_SHIFT) & ETHXCVR_ACD_PARAMS_LPTYPE_MASK) | \
                                     (((b) << ETHXCVR_ACD_PARAMS_CABLETYPE_SHIFT) & ETHXCVR_ACD_PARAMS_CABLETYPE_MASK) | \
                                     (((c) << ETHXCVR_ACD_PARAMS_STATUS_SHIFT) & ETHXCVR_ACD_PARAMS_STATUS_MASK))

/**
    @name ETHXCVR_AcdParamsType
    @brief ACD parameters map
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_AcdParamsType;    /**< @brief ETHXCVR_AcdParamsType typedef */
#define ETHXCVR_ACD_PARAMS_STATUS_SHIFT     (0U) /**< @brief ACD status shift */
#define ETHXCVR_ACD_PARAMS_STATUS_MASK      (1U << ETHXCVR_ACD_PARAMS_STATUS_SHIFT) /**< @brief ACD status mask */
#define ETHXCVR_ACD_PARAMS_CABLETYPE_SHIFT  (1U) /**< @brief ACD cable type shift */
#define ETHXCVR_ACD_PARAMS_CABLETYPE_MASK   (0x3U << ETHXCVR_ACD_PARAMS_CABLETYPE_SHIFT) /**< @brief ACD cable type mask */
#define ETHXCVR_ACD_PARAMS_LPTYPE_SHIFT     (3U) /**< @brief ACD link partner type shift */
#define ETHXCVR_ACD_PARAMS_LPTYPE_MASK      (0x3U << ETHXCVR_ACD_PARAMS_LPTYPE_SHIFT) /**< @brief ACD link partner type mask */
#define ETHXCVR_ACD_PARAMS_STATUS_DISABLE   (0U) /**< @brief Disable ACD */
#define ETHXCVR_ACD_PARAMS_STATUS_ENABLE    (1U) /**< @brief Enable ACD */
#define ETHXCVR_ACD_PARAMS_CABLETYPE_0      (0U) /**< @brief ACD cable type reserved*/
#define ETHXCVR_ACD_PARAMS_CABLETYPE_1      (1U) /**< @brief ACD cable type for Leoni 647 (Default) */
#define ETHXCVR_ACD_PARAMS_LPTYPE_0         (0U) /**< @brief ACD link partner default */
#define ETHXCVR_ACD_PARAMS_LPTYPE_1         (1U) /**< @brief ACD link partner type 1 for future use */
#define ETHXCVR_ACD_PARAMS_LP0_CB0_ENABLE   ETHXCVR_ACD_PARAMS(ETHXCVR_ACD_PARAMS_LPTYPE_0, ETHXCVR_ACD_PARAMS_CABLETYPE_0, ETHXCVR_ACD_PARAMS_STATUS_ENABLE)
#define ETHXCVR_ACD_PARAMS_LP0_CB1_ENABLE   ETHXCVR_ACD_PARAMS(ETHXCVR_ACD_PARAMS_LPTYPE_0, ETHXCVR_ACD_PARAMS_CABLETYPE_1, ETHXCVR_ACD_PARAMS_STATUS_ENABLE)
#define ETHXCVR_ACD_PARAMS_LP1_CB0_ENABLE   ETHXCVR_ACD_PARAMS(ETHXCVR_ACD_PARAMS_LPTYPE_1, ETHXCVR_ACD_PARAMS_CABLETYPE_0, ETHXCVR_ACD_PARAMS_STATUS_ENABLE)
#define ETHXCVR_ACD_PARAMS_LP1_CB1_ENABLE   ETHXCVR_ACD_PARAMS(ETHXCVR_ACD_PARAMS_LPTYPE_1, ETHXCVR_ACD_PARAMS_CABLETYPE_1, ETHXCVR_ACD_PARAMS_STATUS_ENABLE)
#define ETHXCVR_ACD_PARAMS_LP0_ENABLE       ETHXCVR_ACD_PARAMS_LP0_CB0_ENABLE
#define ETHXCVR_ACD_PARAMS_LP1_ENABLE       ETHXCVR_ACD_PARAMS_LP1_CB0_ENABLE
/** @} */

/**
    @name ETHXCVR_WakeupReasonType
    @brief Defines the transceiver wake up reasons
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_WakeupReasonType;
#define ETHXCVR_WAKEUPREASON_NONE     (0U) /**< @brief No WakeUp Detected */
#define ETHXCVR_WAKEUPREASON_UNKNOWN  (1U) /**< @brief WakeUp Happened But No wake up reason detected */
#define ETHXCVR_WAKEUPREASON_REMOTE   (2U) /**< @brief WakeUp Request Received From the Remote PHY   */
#define ETHXCVR_WAKEUPREASON_LOCAL    (3U) /**< @brief WakeUp Request Asserted By Local PHY   */
#define ETHXCVR_WAKEUPREASON_POWER_ON (4U) /**< @brief Power on wake up detected   */
#define ETHXCVR_WAKEUPREASON_PIN      (5U) /**< @brief Pin wake up detected        */
/** @} */

/**
    @name ETHXCVR_TC10PhyType
    @brief defines various TC10 Phy types
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_TC10PhyType;
#define ETHXCVR_TC10PHY_NONE                     (0U) /**< @brief None                           */
#define ETHXCVR_TC10PHY_INT_100BASET1            (1U) /**< @brief Integrated 100M Phy            */
#define ETHXCVR_TC10PHY_INT_1000BASET1           (2U) /**< @brief Integrated 1G Phy              */
#define ETHXCVR_TC10PHY_EXT_1000BASET1_LEGACY    (3U) /**< @brief Legacy Mode of External 1G Phy */
#define ETHXCVR_TC10PHY_EXT_1000BASET1           (4U) /**< @brief External 1G Phy                */
/** @} */

/**
    @name ETHXCVR_BooleanType
    @brief Enable/disable for a feature
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_BooleanType;
#define ETHXCVR_BOOLEAN_FALSE  (0U) /**< @brief False */
#define ETHXCVR_BOOLEAN_TRUE   (1U) /**< @brief True  */
/** @} */

/**
    @name ETHXCVR_StateType
    @{
    @brief Ethernet transceiver driver state

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_StateType;  /**< @brief ETHXCVR_State typedef */
#define ETHXCVR_STATE_UNINIT (0U)   /**< @brief Transceiver driver uninitialised & value must be zero*/
#define ETHXCVR_STATE_INIT   (1U)   /**< @brief Transceiver driver initialialised */
/** @} */

/**
    @name ETHXCVR_AutoNegStatusType
    @{
    @brief Auto-negotiation status

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_AutoNegStatusType;          /**< @brief ETHXCVR_AutoNegStatusType typedef */
#define ETHXCVR_AUTONEGSTATUS_COMPLETE      (0x1U) /**< @brief Auto-neg complete */
#define ETHXCVR_AUTONEGSTATUS_INCOMPLETE    (0x2U) /**< @brief Auto-neg incomplete */
#define ETHXCVR_AUTONEGSTATUS_NO_ABILITY    (0x3U) /**< @brief Auto-neg ability not present  */
/** @} */

/**
    @name ETHXCVR TC10 States
    @{
    @brief ETHXCVR TC10 State group

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_TC10StateType;           /**< @brief ETHXCVRM TC10 Modes */
#define ETHXCVR_TC10_STATE_START       (0x00U)   /**< @brief Start               */
#define ETHXCVR_TC10_STATE_WAKE_TX     (0x02U)   /**< @brief Wake Transmit       */
#define ETHXCVR_TC10_STATE_WAKETX_DONE (0x03U)   /**< @brief Wake Transmit done  */
#define ETHXCVR_TC10_STATE_AWAKE       (0x04U)   /**< @brief Awake               */
#define ETHXCVR_TC10_STATE_ASSERT_PSD  (0x05U)   /**< @brief Assert PSD signal to trigger psd_out going low */
#define ETHXCVR_TC10_STATE_SLEEP       (0x06U)   /**< @brief Sleep/DeepSleep mode  */
/** @} */

/**
    @name ETHXCVR TC10 Modes
    @{
    @brief ETHXCVR TC10 Mode group

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_TC10ModeType;          /**< @brief ETHXCVRM TC10 Modes  */
#define ETHXCVR_TC10_MODE_NORMAL1    (0x00U)   /**< @brief TC10 is enabled      */
#define ETHXCVR_TC10_MODE_NORMAL2    (0x01U)   /**< @brief TC10 is disabled     */
#define ETHXCVR_TC10_MODE_HOST       (0x02U)   /**< @brief After power-up device will go to Host mode, (all registers are accessible) */
#define ETHXCVR_TC10_MODE_DEEPSLEEP    (0x03U)   /**< @brief After power-up device will go to DEEPSLEEP mode */
/** @} */

/**
    @name ETHXCVR_KeyType
    @{
    @brief Transceiver Key group

    @note Of the 16bits, the upper 4bits represent the bus mode, the next 4bits
          represent the subtype and the lower 8bits represent the value.
          [15:12] = Bus Type
          [11: 8] = Sub Type
          [ 7: 0] = Value

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint16_t ETHXCVR_KeyType;                          /**< @brief Transceiver Key Type                    */
#define ETHXCVR_KEY_RGMII_VOLT_1P5               (0x2000U) /**< @brief RGMII Voltage 1.5V                      */
#define ETHXCVR_KEY_RGMII_VOLT_1P8               (0x2001U) /**< @brief RGMII Voltage 1.8V                      */
#define ETHXCVR_KEY_RGMII_VOLT_2P5               (0x2002U) /**< @brief RGMII Voltage 2.5V                      */
#define ETHXCVR_KEY_RGMII_VOLT_3P3               (0x2003U) /**< @brief RGMII Voltage 3.3V                      */
#define ETHXCVR_KEY_RGMII_RX_DELAY_ENABLE        (0x2101U) /**< @brief RGMII Receive Clock Delay Enable        */
#define ETHXCVR_KEY_RGMII_TX_DELAY_ENABLE        (0x2201U) /**< @brief RGMII Transmit Clock Delay Enable       */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_0P4_NS (0x2300U) /**< @brief RGMII Receive Clock DLL Phase of 0.4ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_0P8_NS (0x2301U) /**< @brief RGMII Receive Clock DLL Phase of 0.8ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_1P2_NS (0x2302U) /**< @brief RGMII Receive Clock DLL Phase of 1.2ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_1P6_NS (0x2303U) /**< @brief RGMII Receive Clock DLL Phase of 1.6ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_2P0_NS (0x2304U) /**< @brief RGMII Receive Clock DLL Phase of 2.0ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_2P4_NS (0x2305U) /**< @brief RGMII Receive Clock DLL Phase of 2.4ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_2P8_NS (0x2306U) /**< @brief RGMII Receive Clock DLL Phase of 2.8ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_3P2_NS (0x2307U) /**< @brief RGMII Receive Clock DLL Phase of 3.2ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_3P6_NS (0x2308U) /**< @brief RGMII Receive Clock DLL Phase of 3.6ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_4P0_NS (0x2309U) /**< @brief RGMII Receive Clock DLL Phase of 4.0ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_4P4_NS (0x230AU) /**< @brief RGMII Receive Clock DLL Phase of 4.4ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_4P8_NS (0x230BU) /**< @brief RGMII Receive Clock DLL Phase of 4.8ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_5P2_NS (0x230CU) /**< @brief RGMII Receive Clock DLL Phase of 5.2ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_5P6_NS (0x230DU) /**< @brief RGMII Receive Clock DLL Phase of 5.6ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_6P0_NS (0x230EU) /**< @brief RGMII Receive Clock DLL Phase of 6.0ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_6P4_NS (0x230FU) /**< @brief RGMII Receive Clock DLL Phase of 6.4ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_6P8_NS (0x2310U) /**< @brief RGMII Receive Clock DLL Phase of 6.8ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_7P2_NS (0x2311U) /**< @brief RGMII Receive Clock DLL Phase of 7.2ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_7P6_NS (0x2312U) /**< @brief RGMII Receive Clock DLL Phase of 7.6ns  */
#define ETHXCVR_KEY_RGMII_RX_CLOCK_DLL_PH_8P0_NS (0x2313U) /**< @brief RGMII Receive Clock DLL Phase of 8.0ns  */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_0P4_NS (0x2400U) /**< @brief RGMII Transmit Clock DLL Phase of 0.4ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_0P8_NS (0x2401U) /**< @brief RGMII Transmit Clock DLL Phase of 0.8ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_1P2_NS (0x2402U) /**< @brief RGMII Transmit Clock DLL Phase of 1.2ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_1P6_NS (0x2403U) /**< @brief RGMII Transmit Clock DLL Phase of 1.6ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_2P0_NS (0x2404U) /**< @brief RGMII Transmit Clock DLL Phase of 2.0ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_2P4_NS (0x2405U) /**< @brief RGMII Transmit Clock DLL Phase of 2.4ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_2P8_NS (0x2406U) /**< @brief RGMII Transmit Clock DLL Phase of 2.8ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_3P2_NS (0x2407U) /**< @brief RGMII Transmit Clock DLL Phase of 3.2ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_3P6_NS (0x2408U) /**< @brief RGMII Transmit Clock DLL Phase of 3.6ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_4P0_NS (0x2409U) /**< @brief RGMII Transmit Clock DLL Phase of 4.0ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_4P4_NS (0x240AU) /**< @brief RGMII Transmit Clock DLL Phase of 4.4ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_4P8_NS (0x240BU) /**< @brief RGMII Transmit Clock DLL Phase of 4.8ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_5P2_NS (0x240CU) /**< @brief RGMII Transmit Clock DLL Phase of 5.2ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_5P6_NS (0x240DU) /**< @brief RGMII Transmit Clock DLL Phase of 5.6ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_6P0_NS (0x240EU) /**< @brief RGMII Transmit Clock DLL Phase of 6.0ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_6P4_NS (0x240FU) /**< @brief RGMII Transmit Clock DLL Phase of 6.4ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_6P8_NS (0x2410U) /**< @brief RGMII Transmit Clock DLL Phase of 6.8ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_7P2_NS (0x2411U) /**< @brief RGMII Transmit Clock DLL Phase of 7.2ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_7P6_NS (0x2412U) /**< @brief RGMII Transmit Clock DLL Phase of 7.6ns */
#define ETHXCVR_KEY_RGMII_TX_CLOCK_DLL_PH_8P0_NS (0x2413U) /**< @brief RGMII Transmit Clock DLL Phase of 8.0ns */
#define ETHXCVR_KEY_RMII_CLK_DIR_IN              (0x5000U) /**< @brief RMII Clock Direction In                 */
#define ETHXCVR_KEY_RMII_CLK_DIR_OUT             (0x5001U) /**< @brief RMII Clock Direction Out                */
/** @} */

/**
    @name ETHXCVR_LoopbackModeType
    @{
    @brief PHY loopback modes

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_LoopbackModeType;
#define ETHXCVR_LOOPBACK_MODE_NONE              (0x0U)    /**< @brief no loopback (normal operation) */
#define ETHXCVR_LOOPBACK_MODE_INTERNAL          (0x1U)    /**< @brief internal loopback              */
#define ETHXCVR_LOOPBACK_MODE_EXTERNAL          (0x2U)    /**< @brief external loopback              */
#define ETHXCVR_LOOPBACK_MODE_REMOTE            (0x3U)    /**< @brief remote loopback                */
/** @} */

/**
    @name ETHXCVR_PcsModeType
    @{
    @brief Select PCS clauses mode to be enabled

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_PcsModeType;
#define ETHXCVR_PCS_MODE_NONE                   (0x0U)    /**< @brief PCS mode selected is HW default */
#define ETHXCVR_PCS_MODE_CL127                  (0x1U)    /**< @brief Select PCS clause CL127 */
#define ETHXCVR_PCS_MODE_CL129                  (0x2U)    /**< @brief Select PCS clause CL129 */
/** @} */


/**
    @name ETHXCVR_CableStatusType
    @{
    @brief PHY cable status types

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_CableStatusType;       /**< @brief cable diagnostics result type   */
#define ETHXCVR_CABLE_STATUS_OK         (0U)   /**< @brief cable diagnostic result OK      */
#define ETHXCVR_CABLE_STATUS_ERROR      (1U)   /**< @brief cable diagnostic result error   */
#define ETHXCVR_CABLE_STATUS_SHORT      (2U)   /**< @brief Short circuit detected          */
#define ETHXCVR_CABLE_STATUS_OPEN       (3U)   /**< @brief Open circuit detected           */
#define ETHXCVR_CABLE_STATUS_WRONG_POL  (4U)   /**< @brief wrong polarity detected         */
/** @} */

/**
    @name ETHXCVR_WakeupEventType
    @{
    @brief Wake-up event types

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint32_t ETHXCVR_WakeupEventType;                 /**< @brief Wake-up event types          */
#define ETHXCVR_WAKEUP_EVENT_REMOTE_WAKEUP     (0x01UL)   /**< @brief Wake-up received from remote
                                                                   (Wake-up Pulse/Wake-up request) */
#define ETHXCVR_WAKEUP_EVENT_LOCAL_WAKEUP      (0x02UL)   /**< @brief Wake-up Pulse generated      */
#define ETHXCVR_WAKEUP_EVENT_REMOTE_WAKE_FAIL  (0x04UL)   /**< @brief Wake-up request failed       */
#define ETHXCVR_WAKEUP_EVENT_SLEEP             (0x08UL)   /**< @brief Sleep trigger                */
#define ETHXCVR_WAKEUP_EVENT_SLEEP_FAIL        (0x10UL)   /**< @brief Sleep request failed         */
#define ETHXCVR_WAKEUP_EVENT_INHIBIT           (0x20UL)   /**< @brief Inhibit                      */
#define ETHXCVR_WAKEUP_EVENT_PIN_WAKEUP        (0x40UL)   /**< @brief Wake-up through pin          */
#define ETHXCVR_WAKEUP_EVENT_WAKEUP_TRIGGER    (0x80UL)   /**< @brief Wake-up triggered by self    */
/** @} */

/**
    @name ETHXCVR_EEEModeType
    @{
    @brief Energy-Efficient Ethernet Mode types

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_EEEModeType;                    /**< @brief Energy-Efficient Ethernet Mode types */
#define ETHXCVR_EEE_MODE_DISABLE               (0x0U)   /**< @brief Energy-Efficient Ethernet disable */
#define ETHXCVR_EEE_MODE_IEEE                  (0x1U)   /**< @brief Energy-Efficient Ethernet IEEE enable  */
#define ETHXCVR_EEE_MODE_FIXED_LATENCY         (0x2U)   /**< @brief Energy-Efficient Ethernet Fixed Latency */
#define ETHXCVR_EEE_MODE_VARIBLE_LATENCY       (0x3U)   /**< @brief Energy-Efficient Ethernet Varible Latency */
/** @} */

/**
    @name ETHXCVR_TC10CfgFlagType
    @{
    @brief TC10 Configuration flags for a phy

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t ETHXCVR_TC10CfgFlagType;              /**< @brief TC10 configuration flags */
#define ETHXCVR_TC10_CFG_FLAG_BLOCK_SLEEP_REQ         (0x1U)   /**< @brief Block TC10 remote sleep request */
#define ETHXCVR_TC10_CFG_FLAG_BLOCK_MDI_WAKE          (0x2U)   /**< @brief Block TC10 wake from MDI */
/** @} */

/**
    @name ETHXCVR_IrqParamsType
    @{
    @brief Transceiver Interrupt parameters
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_IrqParamsType {
    ETHXCVR_WakeupEventType wakeupEventMask; /**< @brief Wake-up Event mask */
    ETHXCVR_BooleanType     isLinkEvent;     /**< @brief Boolean to indicate presence of Link Event */
    ETHXCVR_LinkStateType   linkState;       /**< @brief Link state at the time of irq */
} ETHXCVR_IrqParamsType;
/** @} */

/**
    @name Phy ID type
    @{
    @brief PHY identifier struct

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_PhyIDType {
    uint32_t manuf;     /**< @brief manufacturer ID */
    uint32_t model;     /**< @brief model number    */
    uint32_t rev;       /**< @brief revision number */
    uint32_t reserved;  /**< @brief reserved        */
} ETHXCVR_PhyIDType;
/** @} */

/**
    @name ETHXCVR_BusConfigType
    @{
    @brief Bus configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_BusConfigType {
    uint8_t         cntrlID;           /**< @brief Controller index */
    uint8_t         instID;            /**< @brief Instance index */
    uint8_t         reserved[2U];      /**< @brief Reserved field */
    ETHXCVR_KeyType driverParams[ETHXCVR_MAX_DRIVER_PARAMS]; /**< @brief Driver parameters */
} ETHXCVR_BusConfigType;
/** @} */

/**
    @name ETHXCVR_PhyConfigType
    @{
    @brief Phy configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_PhyConfigType {
    uint8_t                hwID;              /**< @brief Hardware index of the controller */
    uint8_t                slaveID;           /**< @brief Slave address for PHY controled via MDIO */
    ETHXCVR_AccessModeType accessMode;        /**< @brief Access Mode */
    ETHXCVR_PhyMediaType   phyMedia;          /**< @brief Physical media */
    ETHXCVR_BooleanType    masterMode;        /**< @brief Master/Slave Mode */
    ETHXCVR_AcdParamsType  acdParams;         /**< @brief ACD parameters */
    uint8_t                reserved[2U];      /**< @brief Reserved field */
    ETHXCVR_KeyType        driverParams[ETHXCVR_MAX_DRIVER_PARAMS]; /**< @brief Driver parameters */
} ETHXCVR_PhyConfigType;
/** @} */

/**
    @name ETHXCVR_StatsType
    @{
    @brief Trasceiver Statistics Type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_StatsType {
    uint32_t linkStateChangeCount;    /**< @brief Number of Link change events                   */
} ETHXCVR_StatsType;
/** @} */

/**
    @name ETHXCVR_StatusType
    @{
    @brief Trasceiver Status Type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_StatusType {
    uint8_t    reserved[2];

} ETHXCVR_StatusType;
/** @} */

/**
    @brief TC10 Port Context Type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_TC10PortContextType {
    ETHXCVR_BooleanType      allowMdiWake;        /**< @brief caching the mdi wake config */
    uint8_t                  reserved[15];        /**< @brief Reserved for future use     */
} ETHXCVR_TC10PortContextType;

/**
    @brief TC10 Port Configuration type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_TC10PortConfigType {
    uint64_t              wakeForwardMap;    /**< @brief Wake-up forward port bitmask             */
    ETHXCVR_TC10PhyType   phyType;           /**< @brief TC10 Phy Type                            */
    ETHXCVR_BooleanType   tc10Enable;        /**< @brief Boolean to indicate if tc10 is enabled   */
    uint8_t               hwID;              /**< @brief Hardware index of the controller         */
    uint8_t               slaveID;           /**< @brief Slave address for PHY controlled via MDIO */
    ETHXCVR_TC10CfgFlagType tc10Flags;       /**< @brief TC10 Config flags */
    uint8_t               reserved[11];      /**< @brief Reserved for future use                  */
} ETHXCVR_TC10PortConfigType;

/**
    @brief TC10 Configuration Type containing the TC10 configuration of all ports

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_TC10ConfigType {
    uint32_t magic;                                                 /**< @brief Magic - 'TCFG'          */
    uint32_t reserved;
    ETHXCVR_TC10PortConfigType portConfig[ETHXCVR_MAX_PORTS + 1UL]; /**< @brief TC10 port configuration
                                                                         Index 7 (cpu port) carries the Pin wake forward map */
} ETHXCVR_TC10ConfigType;

/**
    @brief Transceiver TC10 Notification Information

    Transceiver TC10 notification information type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_NotifyTC10InfoType {
    ETHXCVR_WakeupEventType wakeupEventMask;                 /**< @brief Wake-up Events being identified */
    ETHXCVR_IDType          xcvrID;                          /**< @brief Transceiver identifier          */
    uint8_t                 reserved1[3];
    uint64_t                timeNs;                          /**< @brief TC10 Event time in nanoseconds */
    uint8_t                 reserved2[24UL];                 /**< @brief Reserved field                  */
} ETHXCVR_NotifyTC10InfoType;

/**
    @brief Port configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_PortConfigType {
    ETHXCVR_IDType          id;          /**< @brief Port index. This is the chip port number */
    ETHXCVR_BooleanType     portEnable;  /**< @brief Flag indicating whether the port is enabled or not */
    ETHXCVR_BooleanType     tc10Enable;  /**< @brief Flag indicating whether the TC10 functionality is enabled or not */
    ETHXCVR_SpeedType       speed;       /**< @brief Speed */
    ETHXCVR_BooleanType     autoNeg;     /**< @brief Autoneg enable/disable */
    ETHXCVR_DuplexModeType  duplex;      /**< @brief Duplex */
    ETHXCVR_FlowControlType flowControl; /**< @brief Flow Control */
    ETHXCVR_PcsModeType     pcsMode;     /**< @brief Select PCS mode type to be enabled */
    ETHXCVR_BooleanType     jumbo;       /**< @brief Jumbo enable/disable */
    ETHXCVR_BusModeType     busMode;     /**< @brief Bus mode */
    ETHXCVR_BooleanType     timeEnable;  /**< @brief Flag indicating whether the 1588 functionality is enabled or not */
    ETHXCVR_EEEModeType     eeeMode;     /**< @brief Energy-Efficient Ethernet mode */
    ETHXCVR_TC10CfgFlagType tc10Flags;   /**< @brief TC10 Config flags */
    ETHXCVR_BusConfigType   bus;         /**< @brief Bus device */
    ETHXCVR_PhyConfigType   phy;         /**< @brief PHY device */
} ETHXCVR_PortConfigType;

/**
    @brief XCVR configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ConfigType {
    uint32_t magic;               /**< @brief Magic number */
    uint32_t version;             /**< @brief BCM transceiver configuration structure version */
    uint32_t numPorts;            /**< @brief Number of valid ports in the configuration. Should not be more
                                              than #ETHXCVR_MAX_PORTS */
    uint32_t reserved;            /**< @brief reserved field to align */
    uint64_t pinWakeForwardMap;   /**< @brief TC10 Wake Forward port bitmap for pin based Wake-up event */
    uint64_t phyWakeForwardMap[ETHXCVR_MAX_PORTS];  /**< @brief TC10 Wake Forward port bitmap for phy based wake-up event */
    ETHXCVR_PortConfigType port[ETHXCVR_MAX_PORTS]; /**< @brief Port configuration */
    uint8_t name[16UL];           /**< @brief Configuration name */
    uint8_t file[128UL];          /**< @brief File name from which this configuration is generated */
} ETHXCVR_ConfigType;

/**
    @brief XCVR ACD configuration structure for BRPHY
    @note This is a temporary change and this is subjected to change without notice

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ACDBrPhyConfigTempType {
    uint32_t          magic;               /**< @brief Magic number */
    uint32_t          version;             /**< @brief Program version */
    uint32_t          instrCount;          /**< @brief @brief Number of 32-bit instructions */
    uint32_t          maxExecInstr;        /**< @brief @brief Maximum number of instructions to execute for the program */
    uint32_t          stackSize;           /**< @brief Max stack size allocated for the program */
    uint32_t          dataSize;            /**< @brief Max data size allocated for the program */
    uint32_t          numData;             /**< @brief Number of 32-bit data in data section */
    uint8_t           reserved[8UL];       /**< @brief reserved field to align */
} ETHXCVR_ACDBrPhyConfigTempType;

/**
    @brief XCVR ACD init configuration structure for BRPHY
    @note This is a temporary change and this is subjected to change without notice

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ACDBrPhyInitConfigTempType {
    ETHXCVR_ACDBrPhyConfigTempType commonCfg; /**< @brief Common config */
    uint32_t          data100MLen;         /**< @brief Init data length for 100M */
    uint32_t          data1GLen;           /**< @brief Init data length for 1G  */
    uint8_t           reserved[8UL];       /**< @brief reserved field for future use */
    uint8_t           program[972UL];      /**< @brief Init program (instructions list + Data)*/
} ETHXCVR_ACDBrPhyInitConfigTempType;

/**
    @brief XCVR ACD run configuration structure for BRPHY
    @note This is a temporary change and this is subjected to change without notice

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ACDBrPhyRunConfigTempType {
    ETHXCVR_ACDBrPhyConfigTempType commonCfg; /**< @brief Common config */
    uint32_t          platMagic;              /**< @brief Platform magic */
    uint32_t          peakAmpThreshold100M;   /**< @brief Peak amplitude threshold value to validate for 100M */
    uint32_t          peakAmpThreshold1G;     /**< @brief Peak amplitude threshold value to validate  for 1G*/
    uint32_t          lenFactorDefault100M;   /**< @brief Default length factor values for BRPHY */
    uint32_t          lenFactorDefault1G;     /**< @brief Default length factor values for BRPHY1G */
    uint32_t          openThreshold100M;      /**< @brief Open threshold defaults for BRPHY */
    uint32_t          openThreshold1GCable0;  /**< @brief Open threshold defaults for BRPHY1G with Cable type:0 */
    uint32_t          openThreshold1GCable1;  /**< @brief Open threshold defaults for BRPHY1G with Cable type:1 */
    uint32_t          data100MLen;            /**< @brief run data length for 100M */
    uint32_t          data1GCable0Len;        /**< @brief Run data cable0 type length for 1G  */
    uint32_t          data1GCable1Len;        /**< @brief Run data cable1 type length for 1G  */
    uint8_t           reserved[24UL];         /**< @brief Reserved for future use */
    uint8_t           program[920UL];         /**< @brief Run program + data */
} ETHXCVR_ACDBrPhyRunConfigTempType;

/**
    @brief XCVR ACD configuration structure
    @note This is a temporary change and this is subjected to change without notice

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ACDConfigTmpType {
    uint8_t                    reserved[1536UL];   /**< @brief reserved for xcvr port config (1512-xcvrcfg + 20 Bytes for future) */
    ETHXCVR_ACDBrPhyInitConfigTempType acdInitCfg;  /**< @brief BRPHY ACD Init config */
    ETHXCVR_ACDBrPhyRunConfigTempType  acdRunCfg;   /**< @brief BRPHY ACD run config */
} ETHXCVR_ACDConfigTmpType;

/**
    @brief Port configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_PortConfigLegacyType {
    ETHXCVR_IDType          id;          /**< @brief Port index. This is the chip port number */
    ETHXCVR_BooleanType     portEnable;  /**< @brief Flag indicating whether the port is enabled or not */
    ETHXCVR_SpeedType       speed;       /**< @brief Speed */
    ETHXCVR_BooleanType     autoNeg;     /**< @brief Autoneg enable/disable */
    ETHXCVR_DuplexModeType  duplex;      /**< @brief Duplex */
    ETHXCVR_FlowControlType flowControl; /**< @brief Flow Control */
    ETHXCVR_BooleanType     jumbo;       /**< @brief Jumbo enable/disable */
    ETHXCVR_BusModeType     busMode;     /**< @brief Bus mode */
    ETHXCVR_BusConfigType   bus;         /**< @brief Bus device */
    ETHXCVR_PhyConfigType   phy;         /**< @brief PHY device */
} ETHXCVR_PortConfigLegacyType;

/**
    @brief XCVR configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef struct sETHXCVR_ConfigLegacyType {
    uint32_t magic;               /**< @brief Magic number */
    uint32_t version;             /**< @brief BCM transceiver configuration structure version */
    uint32_t numPorts;            /**< @brief Number of valid ports in the configuration. Should not be more
                                              than #ETHXCVR_MAX_PORTS */
    ETHXCVR_PortConfigLegacyType port[ETHXCVR_MAX_PORTS]; /**< @brief Port configuration */
    uint8_t name[16UL];           /**< @brief Configuration name */
    uint8_t file[128UL];          /**< @brief File name from which this configuration is generated */
} ETHXCVR_ConfigLegacyType;

/**
    @brief XCVR configuration structure union to support backward compatibility

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
 */
typedef union uETHXCVR_ConfigUnionType {
    ETHXCVR_ConfigType current;
    ETHXCVR_ConfigLegacyType legacy;
    ETHXCVR_ACDConfigTmpType acdCfg;
} ETHXCVR_ConfigUnionType;

/**
    @name ETHXCVR_ExtPhyHealthInfoType
    @{
    @brief PHY health information structure

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_ExtPhyHealthInfoType {
    uint32_t    linkStatusChangeCnt; /**< @brief link status change count */
    uint32_t    sqi;                 /**< @brief link Signal quality index */
    int32_t     currTemp;            /**< @brief current temperature of the PHY */
    uint32_t    currAVolt;           /**< @brief current analog voltage of the PHY */
    uint32_t    currDVolt;           /**< @brief current digital voltage of the PHY */
} ETHXCVR_ExtPhyHealthInfoType;
/** @} */

/**
    @name ETHXCVR_CableDiagResultType
    @{
    @brief cable diagnostic result type

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef struct sETHXCVR_CableDiagResultType {
    ETHXCVR_CableStatusType         cableStatus;   /**< @brief cable staus */
    uint32_t                        cableLen;      /**< @brief Estimated cable length in centimeters */
    uint32_t                        reserved[6UL]; /**< @brief reserved */
} ETHXCVR_CableDiagResultType;
/** @} */

/**
    @brief Global handle for EthXcvr

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
extern const ETHXCVR_ConfigUnionType ETHXCVR_Cfg;

#endif /* XCVR_COMMON_H */
/** @} */
