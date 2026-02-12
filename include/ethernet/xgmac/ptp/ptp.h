/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_ptpdrv_ifc  Precision Time Protocol (PTP)
    @ingroup grp_xgmacdrv

    @addtogroup grp_ptpdrv_ifc
    @{
    @section sec_ptp_overview Overview

    The XGMAC PTP supports the IEEE 1588-2002 (version 1) and IEEE 1588-2008 (version 2). The IEEE 1588-2002
    supports PTP transported over UDP/IP. The IEEE 1588-2008 supports PTP transported over Ethernet. The
    XGMAC PTP provides programmable support for both standards. It supports the following features:
        -# Supports both timestamp formats
        -# Provides an option to take snapshot of all packets or only PTP type packets
        -# Provides an option to take snapshot of only event messages
        -# Provides an option to take the snapshot based on the clock type: ordinary, boundary, end-to-end transparent, and peer-to-peer transparent
        -# Provides an option to select the node to be a master or slave for ordinary and boundary clock
        -# Identifies the PTP message type, version, and PTP payload in packets sent directly over Ethernet and sends the status
        -# Provides an option to measure sub-second time in digital or binary format

       PTP controller base address is accessed based on index HwID passed as 0 to PTP_MAX_HW_ID - 1,
       depending on HW instance.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   PTP_0       |

    @section api_list List of PTP APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref PTP_DrvInit          | Initialize PTP driver |
    | @ref PTP_DrvDeInit        | De-initialize PTP driver |
    | @ref PTP_DrvOpModeSet     | Set PTP Operating mode |
    | @ref PTP_DrvOpModeGet     | Get PTP Operating mode status |
    | @ref PTP_DrvConfigSet     | Set PTP Configuration |
    | @ref PTP_DrvConfigGet     | Get PTP Configuration status |
    | @ref PTP_DrvTimestampSet  | Set PTP Timestamp |
    | @ref PTP_DrvTimestampGet  | Get PTP Timestamp status |
    | @ref PTP_DrvSystemTimeSet | Set PTP System Time |
    | @ref PTP_DrvSystemTimeGet | Get PTP System Time status |
    | @ref PTP_DrvPulseSet      | Set PTP Pulse Configuration |
    | @ref PTP_DrvPulseGet      | Get PTP Pulse Configuration status |
    | @ref PTP_DrvPacketGenSet  | Set Automatic PTP Message Generation |
    | @ref PTP_DrvPacketGenGet  | Get Automatic PTP Message Generation status |
    @file ptp.h
    @brief Interface for PTP driver
    This header file defines interface for PTP driver.

    @version 0.1 Initial Version
*/

#ifndef PTP_H
#define PTP_H

#include <stdint.h>

/**
    @name PTP Driver API IDs
    @{
    @brief API IDs for PTP Driver
*/
#define BRCM_SWARCH_PTP_HW_ID_TYPE                  (0x8E00U) /**< @brief #PTP_HwIDType             */
#define BRCM_SWARCH_PTP_SNAPSHOT_OUT_INTF_TYPE      (0x8E01U) /**< @brief #PTP_SnapshotOutIntfType  */
#define BRCM_SWARCH_PTP_PROTOCOL_TYPE               (0x8E02U) /**< @brief #PTP_ProtocolType         */
#define BRCM_SWARCH_PTP_TS_CAPTURE_ACCUR_TYPE       (0x8E03U) /**< @brief #PTP_TsCaptureAccurType   */
#define BRCM_SWARCH_PTP_TX_TS_STATUS_TYPE           (0x8E04U) /**< @brief #PTP_TxTsStatusType       */
#define BRCM_SWARCH_PTP_SYS_TIME_INPUT_TYPE         (0x8E05U) /**< @brief #PTP_SysTimeInputType     */
#define BRCM_SWARCH_PTP_CHKSUM_TYPE                 (0x8E06U) /**< @brief #PTP_ChksumType           */
#define BRCM_SWARCH_PTP_ADDR_FILTER_TYPE            (0x8E07U) /**< @brief #PTP_AddrFilterType       */
#define BRCM_SWARCH_PTP_TS_ROLLOVER_TYPE            (0x8E09U) /**< @brief #PTP_TsRolloverType       */
#define BRCM_SWARCH_PTP_SNAPSHOT_MSG_TYPE           (0x8E10U) /**< @brief #PTP_SnapshotMsgType      */
#define BRCM_SWARCH_PTP_PRESENT_TIME_GEN_TYPE       (0x8E11U) /**< @brief #PTP_PresentTimeGenType   */
#define BRCM_SWARCH_PTP_SYS_TIME_UPD_METHOD_TYPE    (0x8E12U) /**< @brief #PTP_SysTimeUpdMethodType */
#define BRCM_SWARCH_PTP_SYS_TIME_UPDATE_TYPE        (0x8E13U) /**< @brief #PTP_SysTimeUpdateType    */
#define BRCM_SWARCH_PTP_TS_ENABLE                   (0x8E14U) /**< @brief #PTP_TsEnableType         */
#define BRCM_SWARCH_PTP_SYS_TIME_OP_TYPE            (0x8E15U) /**< @brief #PTP_SysTimeOpType        */
#define BRCM_SWARCH_PTP_SYS_TIME_CONFIG_TYPE        (0x8E16U) /**< @brief #PTP_SysTimeConfigType    */
#define BRCM_SWARCH_PTP_SYS_TIME_TYPE               (0x8E17U) /**< @brief #PTP_SysTimeType          */
#define BRCM_SWARCH_PTP_TIMESTAMP_TYPE              (0x8E18U) /**< @brief #PTP_TimestampType        */
#define BRCM_SWARCH_PTP_OP_MODE_TYPE                (0x8E19U) /**< @brief #PTP_OpModeType           */
#define BRCM_SWARCH_PTP_CONFIG_OPT_TYPE             (0x8E20U) /**< @brief #PTP_ConfigOptType        */
#define BRCM_SWARCH_PTP_CONFIG_TYPE                 (0x8E21U) /**< @brief #PTP_ConfigType           */
#define BRCM_SWARCH_PTP_TIME_SPEC_TYPE              (0x8E22U) /**< @brief #PTP_TimeSpecType         */
#define BRCM_SWARCH_PTP_SNAPSHOT_STATUS_TYPE        (0x8E23U) /**< @brief #PTP_SnapshotStatusType   */
#define BRCM_SWARCH_PTP_AUX_SNAP_STATUS_TYPE        (0x8E24U) /**< @brief #PTP_AuxSnapStatusType    */
#define BRCM_SWARCH_PTP_TIME_STATUS_TYPE            (0x8E25U) /**< @brief #PTP_TimeStatusType       */
#define BRCM_SWARCH_PTP_STATE_TYPE                  (0x8E26U) /**< @brief #PTP_StateType            */
#define BRCM_SWARCH_PTP_SYS_TIME_UPDATE_ADDEND_TYPE (0x8E27U) /**< @brief #PTP_SysTimeUpdateAddendType*/
#define BRCM_SWARCH_PTP_SYS_TIME_SIGN_TYPE          (0x8E28U) /**< @brief #PTP_SysTimeSignType      */
#define BRCM_SWARCH_PTP_TS_INTR_TRIG_TYPE           (0x8E29U) /**< @brief #PTP_TsIntrTrigType       */
#define BRCM_SWARCH_PTP_SYS_TIME_INIT_TYPE          (0x8E2AU) /**< @brief #PTP_SysTimeInitType      */
#define BRCM_SWARCH_PTP_PULSE_CONFIG_MASK_TYPE      (0x8E2BU) /**< @brief #PTP_PulseConfigMaskType  */

#define BRCM_SWARCH_PTP_PULSE_STATUS_TYPE           (0x8E31U) /**< @brief #PTP_PulseStatusType      */
#define BRCM_SWARCH_PTP_PULSE_FLEX_CMD_TYPE         (0x8E36U) /**< @brief #PTP_PulseFlexCmdType     */
#define BRCM_SWARCH_PTP_PULSE_CONFIG_TYPE           (0x8E39U) /**< @brief #PTP_PulseConfigType      */
#define BRCM_SWARCH_PTP_PKTGEN_TYPE                 (0x8E40U) /**< @brief #PTP_PktgenType           */
#define BRCM_SWARCH_PTP_PKTGEN_RATIO_TYPE           (0x8E41U) /**< @brief #PTP_PktgenRatioType      */
#define BRCM_SWARCH_PTP_PKTGEN_INTERVAL_TYPE        (0x8E42U) /**< @brief #PTP_PktgenIntervalType   */
#define BRCM_SWARCH_PTP_PKTGEN_CONFIG_TYPE          (0x8E43U) /**< @brief #PTP_PktgenConfigType     */

#define BRCM_SWARCH_PTP_DRV_INIT_PROC               (0x8E60U) /**< @brief #PTP_DrvInit              */
#define BRCM_SWARCH_PTP_DRV_DEINIT_PROC             (0x8E61U) /**< @brief #PTP_DrvDeInit            */
#define BRCM_SWARCH_PTP_DRV_OP_MODE_SET_PROC        (0x8E62U) /**< @brief #PTP_DrvOpModeSet         */
#define BRCM_SWARCH_PTP_DRV_OP_MODE_GET_PROC        (0x8E63U) /**< @brief #PTP_DrvOpModeGet         */
#define BRCM_SWARCH_PTP_DRV_CONFIG_SET_PROC         (0x8E64U) /**< @brief #PTP_DrvConfigSet         */
#define BRCM_SWARCH_PTP_DRV_CONFIG_GET_PROC         (0x8E65U) /**< @brief #PTP_DrvConfigGet         */
#define BRCM_SWARCH_PTP_DRV_TIMESTAMP_SET_PROC      (0x8E66U) /**< @brief #PTP_DrvTimestampSet      */
#define BRCM_SWARCH_PTP_DRV_TIMESTAMP_GET_PROC      (0x8E67U) /**< @brief #PTP_DrvTimestampGet      */
#define BRCM_SWARCH_PTP_DRV_SYSTEM_TIME_SET_PROC    (0x8E68U) /**< @brief #PTP_DrvSystemTimeSet     */
#define BRCM_SWARCH_PTP_DRV_SYSTEM_TIME_GET_PROC    (0x8E69U) /**< @brief #PTP_DrvSystemTimeGet     */
#define BRCM_SWARCH_PTP_DRV_PULSE_SET_PROC          (0x8E6AU) /**< @brief #PTP_DrvPulseSet          */
#define BRCM_SWARCH_PTP_DRV_PULSE_GET_PROC          (0x8E6BU) /**< @brief #PTP_DrvPulseGet          */
#define BRCM_SWARCH_PTP_DRV_PACKET_GEN_SET_PROC     (0x8E6CU) /**< @brief #PTP_DrvPacketGenSet      */
#define BRCM_SWARCH_PTP_DRV_PACKET_GEN_GET_PROC     (0x8E6DU) /**< @brief #PTP_DrvPacketGenGet      */

/** @} */

/**
    @name PTP_HwIDType
    @{
    @brief Index of the PTP module

    @trace #BRCM_SWREQ_PTP_INIT
 */
typedef uint32_t PTP_HwIDType;
/** @} */

/**
    @name PTP_StateType
    @{
    @brief State of PTP driver

    @trace #BRCM_SWREQ_PTP_INIT
*/
typedef uint8_t PTP_StateType;           /**< @brief PTP state type */
#define PTP_STATE_UNINIT           (0U)  /**< @brief PTP is uninitialized and value must be zero*/
#define PTP_STATE_INITIALIZED      (1U)  /**< @brief PTP is initialized */
/** @} */

/**
    @name PTP_TimeSpecType
    @{
    @brief PTP timespec structure to accommodate timestamp/System_time values
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_TimeSpecType {
    uint64_t         second;    /**< @brief 64-bit timespec second value                */
    uint32_t     nanosecond;    /**< @brief 32-bit timespec nanosecond/sub-second value */
    uint32_t  subNanosecond;    /**< @brief 32-bit timespec sub-nanosecond value        */
} PTP_TimeSpecType;
/** @} */

/**
    @name PTP_SnapshotOutIntfType
    @{
    @brief Tx Timestamp Snapshot FIFO output interface type
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SnapshotOutIntfType;          /**< @brief PTP_SnapshotOutIntfType (@note This parameter is reserved for future use) */
#define PTP_SNAPSHOT_OUT_INTF_STREAM      (0x1U)  /**< @brief Tx Timestamp Status FIFO output read from Streaming Interface */
#define PTP_SNAPSHOT_OUT_INTF_CSR_SLAVE   (0x0U)  /**< @brief Tx Timestamp Status FIFO output read from CSR Slave Interface */
/** @} */

/**
    @name PTP_ProtocolType
    @{
    @brief Enable applicable protocols:  AV IEEE 802.1AS / IEEE1588 ver.2 / IPv4 / IPv6 / Ethernet
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_ProtocolType;
#define PTP_PROTOCOL_802P1AS    (0x1U<<4U)    /**< @brief Enable AV IEEE 802.1AS Mode */
#define PTP_PROTOCOL_1588V2     (0x1U<<3U)    /**< @brief Enable PTP Packet Processing for IEEE1588 ver.2 Format */
#define PTP_PROTOCOL_IPV4       (0x1U<<2U)    /**< @brief Enable Processing of PTP Packets Sent over IPv4-UDP */
#define PTP_PROTOCOL_IPV6       (0x1U<<1U)    /**< @brief Enable Processing of PTP Packets Sent over IPv6-UDP */
#define PTP_PROTOCOL_ETH        (0x1U<<0U)    /**< @brief Enable Processing of PTP over Ethernet Packets      */
/** @} */

/**
    @name PTP_TsCaptureAccurType
    @{
    @brief Timestamp Capturing Accuracy selection
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_TsCaptureAccurType;         /**< @brief PTP_TsCaptureAccurType (@note This parameter is reserved for future use) */
#define PTP_TS_CAPTURE_ACCUR_CLK_DOMAIN (0x3U)  /**< @brief Timestamp captured in respective MAC Receiver/Transmitter Clock Domain */
#define PTP_TS_CAPTURE_ACCUR_XPCS       (0x1U)  /**< @brief Timestamp captured includes the latencies provided by XPCS layer */
#define PTP_TS_CAPTURE_ACCUR_NONE       (0x0U)  /**< @brief XPCS latencies are not considered in the captured Timestamp      */
/** @} */

/**
    @name PTP_TxTsStatusType
    @{
    @brief Transmit Timestamp Status Mode select
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_TxTsStatusType;
#define PTP_TX_TS_STATUS_OVERWRITE   (0x1U)  /**< @brief MAC overwrites the earlier transmit timestamp status */
#define PTP_TX_TS_STATUS_IGNORE      (0x0U)  /**< @brief MAC ignores the timestamp status of current packet   */
/** @} */

/**
    @name PTP_SysTimeInputType
    @{
    @brief System Time Input
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeInputType;
#define PTP_SYS_TIME_INPUT_EXTERNAL      (0x1U)  /**< @brief Uses the external 64-bit reference System Time input */
#define PTP_SYS_TIME_INPUT_INTERNAL      (0x0U)  /**< @brief Uses the internal reference system Time              */
/** @} */

/**
    @name PTP_ChksumType
    @{
    @brief Enable checksum correction during OST for PTP over UDP/IPv4 packets
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_ChksumType;        /**< @brief PTP_ChksumType (@note This parameter is reserved for future use) */
#define PTP_CHKSUM_UPDATE      (0x1U)  /**< @brief updated the last two bytes of PTP message to keep the UDP checksum correct   */
#define PTP_CHKSUM_RETAIN      (0x0U)  /**< @brief Application must form the packet to keep UDP checksum correct                */
/** @} */

/**
    @name PTP_AddrFilterType
    @{
    @brief MAC Address for PTP Packet Filtering
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_AddrFilterType;
#define PTP_ADDR_FILTER_DST_MAC (0x1U)  /**< @brief  Enable MAC Dst Address for PTP Packet Filtering */
#define PTP_ADDR_FILTER_NONE    (0x0U)  /**< @brief Disable MAC Dst Address for PTP Packet Filtering */
/** @} */

/**
    @name PTP_SnapshotMsgType
    @{
    @brief Select PTP packets for Taking Timestamp Snapshot
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/

typedef uint8_t PTP_SnapshotMsgType;
#define PTP_SNAPSHOT_MSG_ALL     (0x1U<<5U)  /**< @brief Tmestamp Snapshot enabled for all packets received by MAC */
#define PTP_SNAPSHOT_MSG_P2P_TC  (0x1U<<4U)  /**< @brief Snapshot taken for Peer-to-Peer Transparent mode */
#define PTP_SNAPSHOT_MSG_E2E_TC  (0x1U<<3U)  /**< @brief Snapshot taken for  End-to-End  Transparent mode */
#define PTP_SNAPSHOT_MSG_PDELAY  (0x1U<<2U)  /**< @brief Snapshot taken for Master/Slave with peer delay mechanism support */
#define PTP_SNAPSHOT_MSG_MASTER  (0x1U<<1U)  /**< @brief Snapshot taken only for the messages relevant to the Master node  */
#define PTP_SNAPSHOT_MSG_EVENT   (0x1U<<0U)  /**< @brief Timestamp Snapshot only for event messages (Sync/Delay_Req/Pdelay_Req/Pdelay_Resp) */
/** @} */

/**
    @name PTP_TsRolloverType
    @{
    @brief Select Timestamp Digital or Binary Rollover Control
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_TsRolloverType;
#define PTP_TS_ROLLOVER_DIGITAL    (0x1U)    /**< @brief Timestamp Low register rolls over after 0x3B9A_C9FF(1e9) */
#define PTP_TS_ROLLOVER_BINARY     (0x0U)    /**< @brief Rollover value of sub-second register is 0x7FFF_FFFF     */
/** @} */

/**
    @name PTP_PresentTimeGenType
    @{
    @brief Presentation Time Generation Enabling
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_PresentTimeGenType;      /**< @brief PTP_PresentTimeGenType (@note This parameter is reserved for future use) */
#define PTP_PRESENT_TIME_GEN_EN      (0x1U)  /**< @brief Presentation Time Generation enable  */
#define PTP_PRESENT_TIME_GEN_DIS     (0x0U)  /**< @brief Presentation Time Generation disable */
/** @} */

/**
    @name PTP_SysTimeUpdateAddendType
    @{
    @brief Update System Time
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeUpdateAddendType;
#define PTP_SYS_TIME_UPDATE_ADDEND      (0x1U)  /**< @brief Update the System Time         */
#define PTP_SYS_TIME_UPDATE_ADDEND_DONE (0x0U)  /**< @brief System Time addend update done */
/** @} */

/**
    @name PTP_TsIntrTrigType
    @{
    @brief Timestamp Interrupt Trigger
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_TsIntrTrigType;
#define PTP_TS_INTR_TRIGGER_EN      (0x1U)  /**< @brief Enable Timestamp Interrupt Trigger  */
#define PTP_TS_INTR_TRIGGER_DIS     (0x0U)  /**< @brief Disable Timestamp Interrupt Trigger */
/** @} */

/**
    @name PTP_SysTimeUpdateType
    @{
    @brief Update System Time
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeUpdateType;
#define PTP_SYS_TIME_UPDATE         (0x1U)  /**< @brief Update the System Time   */
#define PTP_SYS_TIME_UPDATE_DONE    (0x0U)  /**< @brief System Time update done  */
/** @} */

/**
    @name PTP_SysTimeInitType
    @{
    @brief Initialize PTP Timestamp
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeInitType;
#define PTP_SYS_TIME_INIT        (1U)  /**< @brief Initialize Timestamp.            */
#define PTP_SYS_TIME_INIT_DONE   (0U)  /**< @brief Time stamp initialization done. */
/** @} */

/**
    @name PTP_SysTimeUpdMethodType
    @{
    @brief Select Fine/Coarse method to Update System Time
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeUpdMethodType;
#define PTP_SYS_TIME_UPD_METHOD_FINE    (0x1U)  /**< @brief Use   Fine method to update system time */
#define PTP_SYS_TIME_UPD_METHOD_COARSE  (0x0U)  /**< @brief Use Coarse method to update system time */
/** @} */

/**
    @name PTP_TsEnableType
    @{
    @brief Enable PTP Timestamp
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_TsEnableType;
#define PTP_TS_DISABLE      (0U)  /**< @brief Disable Timestamp  */
#define PTP_TS_ENABLE       (1U)  /**< @brief Enable Timestamp */
/** @} */

/**
    @name PTP_SysTimeSignType
    @{
    @brief Update System Time
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeSignType;
#define PTP_SYS_TIME_SIGN_NEG      (0x1U)  /**< @brief Negative Time */
#define PTP_SYS_TIME_SIGN_POS      (0x0U)  /**< @brief Positive Time */
/** @} */

/**
    @name PTP_SysTimeOpType
    @{
    @brief System Time Operation type
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_SysTimeOpType;
#define PTP_SYS_TIME_OP_VALUE      (0x1U)  /**< @brief Get the value of System Time */
#define PTP_SYS_TIME_OP_INCREMENT  (0x2U)  /**< @brief System Time Increment value  */
#define PTP_SYS_TIME_OP_ADDEND     (0x3U)  /**< @brief System Time Addend value     */
#define PTP_SYS_TIME_OP_CONTROL    (0x4U)  /**< @brief System Time Control          */
#define PTP_SYS_TIME_OP_UPDATE     (0x5U)  /**< @brief Update System Time           */
/** @} */

/**
   @name PTP_SysTimeConfigType
    @{
    @brief PTP System Time configuration structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_SysTimeConfigType {
    PTP_SysTimeInputType        sysTimeInput;        /**< @brief External/Internal System Time Input      */
    PTP_SysTimeUpdMethodType    sysTimeUpdateMethod; /**< @brief Fine/Coarse method to Update System Time */
    PTP_SysTimeUpdateAddendType sysTimeUpdateAddend; /**< @brief Update Addend                            */
    PTP_SysTimeUpdateType       sysTimeUpdate;       /**< @brief Update Timestamp                         */
    PTP_SysTimeSignType         sysTimeSign;         /**< @brief System time sign(Positive/Negative)      */
} PTP_SysTimeConfigType;
/** @} */

/**
   @name PTP_SysTimeType
    @{
    @brief PTP configuration structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef union uPTP_SysTimeType {
    PTP_SysTimeConfigType  sysTimeConfig;   /**< @brief PTP System Time configuration */
    PTP_TimeSpecType       systimeValue;    /**< @brief PTP System Time value         */
} PTP_SysTimeType;
/** @} */

/**
    @name PTP_ConfigOptType
    @{
    @brief PTP configuration options
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint32_t PTP_ConfigOptType;
#define PTP_CONFIG_OPT_TS_GRP0                 (0xFUL)
#define PTP_CONFIG_OPT_TS_ENABLE               (0x1UL<<0UL)  /**< @brief Bitmask to enable Time stamp */
#define PTP_CONFIG_OPT_TS_UPDT_METHOD          (0x1UL<<1UL)  /**< @brief Bitmask to select Fine or Coarse Timestamp Update method */
#define PTP_CONFIG_OPT_TS_INIT                 (0x1UL<<2UL)  /**< @brief Bitmask to initialize time stamp                         */
#define PTP_CONFIG_OPT_TS_UPDT                 (0x1UL<<3UL)  /**< @brief Bitmask to update time stamp                             */
#define PTP_CONFIG_OPT_TS_GRP1                 (0xF0UL)
#define PTP_CONFIG_OPT_TS_INTR_TRIGGER         (0x1UL<<4UL)  /**< @brief Bitmask to enable time stamp interrupt trigger.          */
#define PTP_CONFIG_OPT_TS_UPDT_ADDEND          (0x1UL<<5UL)  /**< @brief Bitmask to update Addend Register                        */
#define PTP_CONFIG_OPT_TS_ROLLOVR_CTRL         (0x1UL<<6UL)  /**< @brief Bitmask to timestamp Digital or Binary Rollover Control  */
#define PTP_CONFIG_OPT_TS_EN_MACADDR           (0x1UL<<7UL)  /**< @brief Bitmask to Enable MAC Address for PTP Packet Filtering   */
#define PTP_CONFIG_OPT_TS_GRP2                 (0xF00UL)
#define PTP_CONFIG_OPT_TS_SYS_TIME_INPUT       (0x1UL<<8UL)  /**< @brief Bitmask to select System Time Input(External/Internal)   */
#define PTP_CONFIG_OPT_TS_TX_STATUS_MODE       (0x1UL<<9UL)  /**< @brief Bitmask to select transmit time stamp Status Mode.       */
#define PTP_CONFIG_OPT_TS_PROTOCOL             (0x1UL<<10UL) /**< @brief Bitmask for Protocol related configuration               */
#define PTP_CONFIG_OPT_TS_SNAPSHOT             (0x1UL<<11UL) /**< @brief Bitmask for Snapshot related configuration               */
#define PTP_CONFIG_OPT_TS_GRP3                 (0x3000UL)
#define PTP_CONFIG_OPT_TS_AUX_SNAP0_ENABLE     (0x1UL<<12UL) /**< @brief Bitmask for Auxiliary Snapshot0 Enable                   */
#define PTP_CONFIG_OPT_TS_AUX_SNAP0_FIFO_CLR   (0x1UL<<13UL) /**< @brief Bitmask for Auxiliary Snapshot0 FIFO Clear.              */
#define PTP_CONFIG_OPT_TS_ALL                  (0x3FFFUL)    /**< @brief Bitmask for all configuration                            */
/** @} */

/**
   @name PTP_ConfigType
    @{
    @brief PTP configuration structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_ConfigType {
    uint8_t                     auxSnap0En ;          /**< @brief Auxiliary Snapshot 0 Enable.  */
    uint8_t                     auxSnap0FifoClr ;     /**< @brief Auxiliary Snapshot FIFO Clear */
    PTP_SnapshotOutIntfType     snapOutIntf;          /**< @brief Tx Timestamp Status FIFO output read from Streaming or CSR Slave Interface  */
    PTP_ProtocolType            protocolEn;           /**< @brief Enable applicable protocols: 802.1AS/1588v2/IPv4/IPv6/Eth                   */
    PTP_TsCaptureAccurType      tsCapAccur;           /**< @brief Timestamp capturing accuracy: PTP Clock Domain or include XPCS latencies    */
    PTP_TxTsStatusType          txTsStatusMode;       /**< @brief Transmit Timestamp Status Mode                                              */
    PTP_SysTimeInputType        sysTimeInput;         /**< @brief External/Internal System Time Input                                         */
    PTP_ChksumType              chksumUpdate;         /**< @brief Enable checksum correction during OST for PTP over UDP/IPv4 packets         */
    PTP_AddrFilterType          addrFilter;           /**< @brief MAC Address for PTP Packet Filtering                                        */
    PTP_SnapshotMsgType         snapMsgType;          /**< @brief Select PTP packets for Taking Snapshots                                     */
    PTP_TsRolloverType          tsRollover;           /**< @brief Timestamp Digital or Binary Rollover Control                                */
    PTP_PresentTimeGenType      presentTimeGen;       /**< @brief Presentation Time Generation Enabling (MCGR mode)                           */
    PTP_SysTimeUpdateAddendType sysTimeUpdateAddend;  /**< @brief Update Addednd                                                              */
    PTP_TsIntrTrigType          tsIntrTrig;           /**< @brief Timestamp Interrupt Trigger                                                 */
    PTP_SysTimeUpdateType       sysTimeUpdate;        /**< @brief Update Timestamp                                                            */
    PTP_SysTimeInitType         sysTimeInit;          /**< @brief Initialize Time stamp                                                       */
    PTP_SysTimeUpdMethodType    sysTimeUpdateMethod;  /**< @brief Fine/Coarse method to Update System Time                                    */
    PTP_TsEnableType            tsEnable;             /**< @brief Enable PTP Timestamp                                                        */
} PTP_ConfigType;
/** @} */

/**
    @name PTP_TimestampType
    @{
    @brief Timestamp type
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint32_t PTP_TimestampType;
#define PTP_TIMESTAMP_STATUS            (0x0U)     /**< @brief MAC Timestamp  Status                       */
#define PTP_TIMESTAMP_SNAPSHOT_TX       (0x1U)     /**< @brief Timestamp captured for the PTP packet transmitted by the MAC */
#define PTP_TIMESTAMP_CORR_IGR          (0x2U)     /**< @brief MAC Timestamp Ingress Correction            */
#define PTP_TIMESTAMP_CORR_EGR          (0x3U)     /**< @brief MAC Timestamp  Egress Correction            */
#define PTP_TIMESTAMP_CORR_ASYM_IGR     (0x4U)     /**< @brief MAC Timestamp Ingress Asymmetry Correction  */
#define PTP_TIMESTAMP_CORR_ASYM_EGR     (0x5U)     /**< @brief MAC Timestamp  Egress Asymmetry Correction  */
#define PTP_TIMESTAMP_DOMAIN_INCR_RX    (0x6U)     /**< @brief Rx Domain Time Increment Value              */
#define PTP_TIMESTAMP_DOMAIN_INCR_TX    (0x7U)     /**< @brief Tx Domain Time Increment Value              */
#define PTP_TIMESTAMP_AUX_SNAP          (0x1000U)  /**< @brief Auxiliary Snapshot operations */
#define PTP_TIMESTAMP_AUX_SNAP_0        (0x1001U)  /**< @brief Auxiliary Snapshot #0         */
#define PTP_TIMESTAMP_AUX_SNAP_1        (0x1002U)  /**< @brief Auxiliary Snapshot #1         */
#define PTP_TIMESTAMP_AUX_SNAP_2        (0x1004U)  /**< @brief Auxiliary Snapshot #2         */
#define PTP_TIMESTAMP_AUX_SNAP_3        (0x1008U)  /**< @brief Auxiliary Snapshot #3         */
#define PTP_TIMESTAMP_AUX_SNAP_CLR      (0x1010U)  /**< @brief Auxiliary Snapshot FIFO Clear */
/** @} */

/**
    @name PTP_SnapshotStatusType
    @{
    @brief PTP Timestamp Snapshot Status structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_SnapshotStatusType {
    uint8_t  overflow;          /**< @brief Timestamp Seconds Overflow                            */
    uint8_t  ready;             /**< @brief A timestamp of the current packet being transmitted   */
    uint8_t  numSnap;           /**< @brief Number of Snapshot Captured                           */
    uint16_t pktId;             /**< @brief The packet ID corresponding to the captured Timestamp */
    PTP_TimeSpecType timestamp; /**< @brief Captured Timestamp value                              */
} PTP_SnapshotStatusType;
/** @} */

/**
    @name PTP_AuxSnapStatusType
    @{
    @brief Auxiliary Timestamp Snapshot Status structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_AuxSnapStatusType {
    uint8_t  ready;              /**< @brief An auxiliary snapshot is written to the FIFO          */
    uint8_t  numAuxsnap;         /**< @brief Number of Auxiliary Snapshot Captured                 */
    uint8_t  triggerMiss;        /**< @brief Auxiliary timestamp snapshot FIFO is full and external trigger was set  */
    PTP_TimestampType triggerId; /**< @brief Auxiliary timestamp snapshot Trigger Identifier */
    PTP_TimeSpecType  timestamp; /**< @brief Captured Auxiliary Timestamp value              */
} PTP_AuxSnapStatusType;
/** @} */

/**
    @name PTP_TimeStatusType
    @{
    @brief PTP Time Status structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef union uPTP_TimeStatusType {
    PTP_SnapshotStatusType txSnapshotStatus;    /**< @brief   PTP Tx  Timestamp Snapshot Status  */
    PTP_AuxSnapStatusType     auxSnapStatus;    /**< @brief Auxiliary Timestamp Snapshot Status  */
} PTP_TimeStatusType;
/** @} */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/**
    @name PTP_PulseStatusType
    @{
    @brief PPS Pulse Status structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_PulseStatusType {
    uint8_t busy ;      /**< @brief PPS0 Target Time Register Busy */
    uint8_t error;      /**< @brief Timestamp Target Time error    */
    uint8_t reached;    /**< @brief Timestamp Target Time Reached  */
} PTP_PulseStatusType;
/** @} */

/**
    @name PTP_PulseFlexCmdType
    @{
    @brief PPS Pulse Flexible Mode command
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_PulseFlexCmdType;
#define PTP_PULSE_FLEX_CMD_NONE              (0x0U) /**< @brief No Command */
#define PTP_PULSE_FLEX_CMD_SINGLE_START      (0x1U) /**< @brief Generate a single pulse rising at the start point defined in Target Time registers */
#define PTP_PULSE_FLEX_CMD_TRAIN_START       (0x2U) /**< @brief Generate the train of pulses rising at the start point defined in Target Time registers and of a duration defined in PPS0 Width register and repeated at interval defined in PPS Interval register */
#define PTP_PULSE_FLEX_CMD_CANCEL_START      (0x3U) /**< @brief Cancel PTP_PULSE_FLEX_CMD_SINGLE_START/_TRAIN_START commands if the system time has not crossed the programmed start time */
#define PTP_PULSE_FLEX_CMD_TRAIN_STOP_AT     (0x4U) /**< @brief Stop the train of pulses initiated by PTP_PULSE_FLEX_CMD_TRAIN_START after the time programmed in the Target Time registers elapses */
#define PTP_PULSE_FLEX_CMD_TRAIN_STOP_NOW    (0x5U) /**< @brief Stop the train of pulses initiated by PTP_PULSE_FLEX_CMD_TRAIN_START immediately */
#define PTP_PULSE_FLEX_CMD_TRAIN_STOP_CANCEL (0x6U) /**< @brief Cancel the PTP_PULSE_FLEX_CMD_TRAIN_STOP_AT command if the programmed stop time has not elapsed. The PPS pulse train becomes free-running on the successful execution of this command */
/** @} */

/**
    @name PTP_PulseConfigMaskType
    @{
    @brief PPS Pulse Flexible Mode command
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef uint8_t PTP_PulseConfigMaskType;
#define PTP_PULSE_CONFIG_MASK_FLEX_CMD       (0x1UL << 0UL) /**< @brief Mask to set PPS Pulse Flexible Mode command */
#define PTP_PULSE_CONFIG_MASK_TGT_TIME       (0x1UL << 1UL) /**< @brief Mask to set PPS Target time to generate interrupt */
/** @} */

/**
   @name PTP_PulseConfigType
    @{
    @brief PTP Pulse-Per-Second (PPS) Configuration structure
    @trace #BRCM_SWREQ_PTP_CONFIGURATION
*/
typedef struct sPTP_PulseConfigType {
    PTP_PulseFlexCmdType     ppsFlexCmd; /**< @brief PPS Pulse Flexible Mode command       */
    PTP_TimeSpecType         targetTime; /**< @brief PPS Target time to generate interrupt */
    PTP_PulseConfigMaskType  aCfgMask ;
} PTP_PulseConfigType;
/** @} */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/**
    @name PTP_PktgenType
    @{
    @brief Automatic PTP Message Generation mode
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
typedef uint8_t PTP_PktgenType;
#define PTP_PKTGEN_ENABLE           (0x1U<<0U)  /**< @brief Enable PTP Message generation                                      */
#define PTP_PKTGEN_AUTO_SYNC        (0x1U<<1U)  /**< @brief SYNC message is generated periodically based on interval programmed or trigger from application */
#define PTP_PKTGEN_AUTO_DLY_REQ     (0x1U<<6U)  /**< @brief Automatic generate Delay_Request on receiving SYNC message         */
#define PTP_PKTGEN_AUTO_DLY_RESP    (0x1U<<6U)  /**< @brief Automatic generate Delay_Resp on receiving Delay_Request message   */
#define PTP_PKTGEN_AUTO_PDLY_REQ    (0x1U<<2U)  /**< @brief PDelay_Req message is generated periodically based on interval programmed or trigger from application */
#define PTP_PKTGEN_AUTO_PDLY_RESP   (0x1U<<7U)  /**< @brief Automatic generate PDelay_Resp on receiving PDelay_Request message */
#define PTP_PKTGEN_TRIGGER_SYNC     (0x1U<<4U)  /**< @brief Automatic PTP SYNC message Trigger                                 */
#define PTP_PKTGEN_TRIGGER_PDLY_REQ (0x1U<<5U)  /**< @brief Automatic PTP Pdelay_Req message Trigger                           */
 /** @} */

/**
    @name PTP_PktgenRatioType
    @{
    @brief PTP Message Ratio for packet generation
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
typedef uint8_t PTP_PktgenRatioType;
#define PTP_PKTGEN_RATIO_DLYREQ_SYNC_1  (0x0U)  /**< @brief Delay_Req generated for every received SYNC messages */
#define PTP_PKTGEN_RATIO_DLYREQ_SYNC_2  (0x1U)  /**< @brief Delay_Req generated for every  2 SYNC messages */
#define PTP_PKTGEN_RATIO_DLYREQ_SYNC_4  (0x2U)  /**< @brief Delay_Req generated for every  4 SYNC messages */
#define PTP_PKTGEN_RATIO_DLYREQ_SYNC_8  (0x3U)  /**< @brief Delay_Req generated for every  8 SYNC messages */
#define PTP_PKTGEN_RATIO_DLYREQ_SYNC_16 (0x4U)  /**< @brief Delay_Req generated for every 16 SYNC messages */
#define PTP_PKTGEN_RATIO_DLYREQ_SYNC_32 (0x5U)  /**< @brief Delay_Req generated for every 32 SYNC messages */
/** @} */

/**
   @name PTP_PktgenIntervalType
    @{
    @brief Interval and Ratio settings for Generating PTP Messages
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
typedef struct sPTP_PktgenIntervalType {
    PTP_PktgenRatioType  ratioDlyReqToSync;  /**< @brief PTP Message Ratio for packet generation     */
    uint8_t  pDelayReq; /**< @brief Schedule the periodic PDelay_Request transmission, range:[-15,15] */
    uint8_t  sync;      /**< @brief Indicates the periodicity of the automatically generated SYNC message, range:[-15,15] */
} PTP_PktgenIntervalType;
/** @} */

/**
   @name PTP_PktgenConfigType
    @{
    @brief Automatic PTP Message Generation Configuration
    @trace #BRCM_SWREQ_PTP_PKT_GENERATION
*/
typedef struct sPTP_PktgenConfigType {
    PTP_PktgenType          mode;     /**< @brief Automatic PTP Message Generation mode */
    PTP_PktgenIntervalType  interval; /**< @brief Interval and Ratio settings for Generating PTP Messages */
    uint8_t                 domainNum;               /**< @brief Domain Number to be filled in auto-generated PTP Message header */
    uint16_t                srcPortIdentity[5U];     /**< @brief 80-bit Source Port Identity to be filled in auto-generated PTP Message header */
} PTP_PktgenConfigType;
/** @} */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/**
    @name PTP_OpModeType
    @{
    @brief PTP Operating Mode
    @trace #BRCM_SWREQ_PTP_OPERATION_MODE
*/
typedef uint8_t PTP_OpModeType;
#define PTP_OP_MODE_CLOCK_MASTER    (0x1U)  /**< @brief Ordinary/Boundary Master Clock mode */
#define PTP_OP_MODE_CLOCK_SLAVE     (0x2U)  /**< @brief Ordinary/Boundary  Slave Clock mode */
#define PTP_OP_MODE_E2E_TRANSPARENT (0x4U)  /**< @brief  End-to-end  Transparent mode */
#define PTP_OP_MODE_P2P_TRANSPARENT (0x8U)  /**< @brief Peer-to-Peer Transparent mode */
#define PTP_OP_MODE_PDELAY_SUPPORT  (0x10U) /**< @brief bitmask flag for Peer-Delay Mechanism */

#define PTP_OP_MODE_PDELAY_CLOCK_MASTER    (PTP_OP_MODE_PDELAY_SUPPORT | PTP_OP_MODE_CLOCK_MASTER)    /**< @brief Ordinary/Boundary Master Clock mode with Peer-Delay Mechanism support */
#define PTP_OP_MODE_PDELAY_CLOCK_SLAVE     (PTP_OP_MODE_PDELAY_SUPPORT | PTP_OP_MODE_CLOCK_SLAVE)     /**< @brief Ordinary/Boundary  Slave Clock mode with Peer-Delay Mechanism support */
#define PTP_OP_MODE_PDELAY_E2E_TRANSPARENT (PTP_OP_MODE_PDELAY_SUPPORT | PTP_OP_MODE_E2E_TRANSPARENT) /**< @brief End-to-end Transparent mode with Peer-Delay Mechanism support */
/** @} */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** @brief PTP Initialization

    API to Initialize PTP.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwID         Index of the PTP

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_PTP_INIT

    @limitations None
*/
int PTP_DrvInit(PTP_HwIDType hwId);

/** @brief PTP De-Initialization

    API to De-Initialize PTP.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwID         Index of the PTP

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_PTP_INIT

    @limitations None
*/
int PTP_DrvDeInit(PTP_HwIDType hwId);

/** @brief Set PTP Operating mode

    API to configure PTP Operating mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID        Index of the PTP
    @param[in]      aOpMode     Operating mode Selection

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_OPERATION_MODE

    @limitations None
*/
int PTP_DrvOpModeSet(PTP_HwIDType hwId, PTP_OpModeType  aOpMode);

/** @brief Get PTP Operating mode

    API to get PTP Operating mode status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID        Index of the PTP
    @param[out]     aOpMode     Operating mode setting status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL PTP operating mode value
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_OPERATION_MODE

    @limitations None
*/
int PTP_DrvOpModeGet(PTP_HwIDType hwId, PTP_OpModeType *aOpMode);

/** @brief Set PTP Configuration

    API to set PTP Configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID        Index of the PTP
    @param[in]      aConfOpt    PTP Configuration options select
    @param[in]      aConfig     PTP Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL PTP configuration value
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvConfigSet(PTP_HwIDType hwId, PTP_ConfigOptType aConfOpt, PTP_ConfigType *aConfig);

/** @brief Get PTP Configuration

    API to get PTP Configuration status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID        Index of the PTP
    @param[out]     aConfig     PTP Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL PTP configuration value
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvConfigGet(PTP_HwIDType hwId, PTP_ConfigType *aConfig);

/** @brief Set PTP Timestamp

    API to set PTP Timestamp.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID        Index of the PTP
    @param[out]     aTsClass    Select PTP Timestamp class
    @param[out]     aTsValue    PTP Timestamp value

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL time stamp configuration value
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvTimestampSet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeSpecType *aTsValue);

/** @brief Get PTP Timestamp

    API to get PTP Timestamp status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID         Index of the PTP
    @param[out]     aTsClass    Select PTP Timestamp class
    @param[out]     aTsStatus   PTP Timestamp status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL time stamp status
    @retval     #BCM_ERR_NODEV         PTP instance not initialized
    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvTimestampGet(PTP_HwIDType hwId, PTP_TimestampType aTsClass, PTP_TimeStatusType *aTsStatus);

/** @brief Set PTP System Time

    API to set PTP System Time.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID         Index of the PTP
    @param[in]      aSysTimeOp  PTP System Time Operation structure
    @param[out]     aSysTime    PTP System Time structure (config or value)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL system time configuration
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvSystemTimeSet(PTP_HwIDType hwId, PTP_SysTimeOpType aSysTimeOp, PTP_SysTimeType *aSysTime);

/** @brief Get PTP System Time

    API to get PTP System Time status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID         Index of the PTP
    @param[in]      aSysTimeOp  PTP System Time Operation structure
    @param[out]     aSysTime    PTP System Time structure (config or value)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL system time configuration
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvSystemTimeGet(PTP_HwIDType hwId, PTP_SysTimeOpType aSysTimeOp, PTP_SysTimeType *aSysTime);

/** @brief Set PTP PPS Configuration

    API to set PTP Pulse-Per-Second (PPS) Configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID          Index of the PTP
    @param[in]      aPulseConfig  PTP PPS Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL pulse configuration and pulse status
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvPulseSet(PTP_HwIDType hwId, PTP_PulseConfigType *aPulseConfig);

/** @brief Get PTP PPS Configuration

    API to get PTP Pulse-Per-Second (PPS) Configuration status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID          Index of the PTP
    @param[out]     aPulseConfig  PTP PPS Configuration structure
    @param[out]     aPulseStatus  Pointer to pulse status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL pulse configuration and pulse status
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_CONFIGURATION

    @limitations None
*/
int PTP_DrvPulseGet(PTP_HwIDType hwId, PTP_PulseConfigType *aPulseConfig, PTP_PulseStatusType *aPulseStatus);

/** @brief Set Automatic PTP Message Generation

    API to set Automatic PTP Message Generation.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID            Index of the PTP
    @param[in]      aPktgenConfig  Automatic PTP Message Generation Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL packet generation configuration
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_PKT_GENERATION

    @limitations None
*/
int PTP_DrvPacketGenSet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig);

/** @brief Get Automatic PTP Message Generation

    API to Get Automatic PTP Message Generation status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwID            Index of the PTP
    @param[in]      aPktgenConfig  Automatic PTP Message Generation Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            PTP driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS  Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS  NULL packet generation configuration
    @retval     #BCM_ERR_NODEV         PTP instance not initialized

    @post None

    @trace  #BRCM_SWREQ_PTP_PKT_GENERATION

    @limitations None
*/
int PTP_DrvPacketGenGet(PTP_HwIDType hwId, PTP_PktgenConfigType *aPktgenConfig);

 #endif /* PTP_H */
/** @} */
