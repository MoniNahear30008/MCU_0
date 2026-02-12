/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_ethernet_common_ifc Ethernet Common Interface
    @ingroup grp_transceiverdrv

    @addtogroup grp_ethernet_common_ifc
    @{

    @file ethernet_common.h

    @brief Ethernet Common Interface
    This file contains the common interface for the Ethernet driver.

    @version 1.0 Initial version
*/

#ifndef ETHERNET_COMMON_H
#define ETHERNET_COMMON_H

#include <bcm_err.h>
#include <bcm_utils.h>

/**
    @name Ethernet Common API IDs
    @{
    @brief API IDs for Ethernet Common
*/
#define BRCM_SWARCH_ETHER_COMMON_MAX_PAYLOAD_MACRO   (0x8501U) /**< @brief #ETHER_COMMON_MAX_PAYLOAD_SIZE */
#define BRCM_SWARCH_ETHER_VLANID_TYPE                (0x8502U) /**< @brief #ETHER_VLANIDType              */
#define BRCM_SWARCH_ETHER_RXSTATS_TYPE               (0x8503U) /**< @brief #ETHER_RxStatsType             */
#define BRCM_SWARCH_ETHER_RXSTATSV2_TYPE             (0x8504U) /**< @brief #ETHER_RxStatsV2Type           */
#define BRCM_SWARCH_ETHER_TXSTATS_TYPE               (0x8505U) /**< @brief #ETHER_TxStatsType             */
#define BRCM_SWARCH_ETHER_TXSTATSV2_TYPE             (0x8506U) /**< @brief #ETHER_TxStatsV2Type           */
#define BRCM_SWARCH_ETHER_MDIOHWID_TYPE              (0x8507U) /**< @brief #ETHER_MDIOHwIDType            */
#define BRCM_SWARCH_ETHER_MDIOSLAVEADDR_TYPE         (0x8508U) /**< @brief #ETHER_MDIOSlaveAddrType       */
#define BRCM_SWARCH_ETHER_MDIODEVICE_TYPE            (0x8509U) /**< @brief #ETHER_MDIODeviceType          */
#define BRCM_SWARCH_ETHER_MDIOREGACCESS_TYPE         (0x850AU) /**< @brief #ETHER_MDIORegAccessType       */
#define BRCM_SWARCH_ETHER_MDIOREGADDR_TYPE           (0x850BU) /**< @brief #ETHER_MDIORegAddrType         */
#define BRCM_SWARCH_ETHER_MDIOPKT_TYPE               (0x850CU) /**< @brief #ETHER_MDIOPktType             */
#define BRCM_SWARCH_ETHER_TC_TYPE                    (0x850DU) /**< @brief #ETHER_TCType                  */
#define BRCM_SWARCH_ETHER_PORT_ID_TYPE               (0x850EU) /**< @brief #ETHER_PortIDType              */
#define BRCM_SWARCH_ETHER_IP_ADDRESS_TYPE            (0x850FU) /**< @brief #ETHER_IpAddressType           */
#define BRCM_SWARCH_ETHER_MACADDRESS_SIZE_MACRO      (0x8510U) /**< @brief #ETHER_MACADDRESS_SIZE         */
#define BRCM_SWARCH_ETHER_L4PROTO_TYPE               (0x8511U) /**< @brief #ETHER_L4ProtoType             */
#define BRCM_SWARCH_ETHER_VLANID_MASK_MACRO          (0x8512U) /**< @brief #ETHER_VLANID_MASK             */
#define BRCM_SWARCH_ETHER_IPVERSION_TYPE             (0x8513U) /**< @brief #ETHER_IpVersionType           */
#define BRCM_SWARCH_ETHER_PORTMASK_TYPE              (0x8514U) /**< @brief #ETHER_PortMaskType            */
#define BRCM_SWARCH_ETHER_HWID_TYPE                  (0x8515U) /**< @brief #ETHER_HwIDType              */
#define BRCM_SWARCH_ETHER_COMMON_INIT_PROC           (0x8516U) /**< @brief #ETHER_CommonInit              */
/** @} */

/**
    @brief Ethernet Hw ID

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint8_t ETHER_HwIDType;  /**< @brief Select Ethernet HW index */
#define ETHER_HWID_0      (0U)     /**< @brief ethernet HW index 0 */

/**
    @brief Maximum size of command payload in bytes

    @trace #BRCM_SWREQ_ETHER
*/
#define ETHER_COMMON_MAX_PAYLOAD_SIZE   (392UL)

/**
    @brief Size of MAC address in bytes

    @trace #BRCM_SWREQ_ETHER
*/
#define ETHER_MACADDRESS_SIZE           (6UL)

/**
    @brief Ethernet VLAN ID Mask

    @trace #BRCM_SWREQ_ETHER
*/
#define ETHER_VLANID_MASK          (0xFFFU)    /**< @brief Mask for VLAN ID */

/**
    @name VLAN ID type
    @{
    @brief 802.1Q VLAN ID type

    VLAN range 0x0 to 4095 where 0x000 and 0xFFF are reserved.

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint16_t ETHER_VLANIDType;             /**< @brief 802.1Q VLAN ID type @xsd input:int */
#define ETHER_VLANID_NONE          (0U)        /**< @brief Refers to VLANID = 0 (priority tagged packets) */
#define ETHER_VLANID_MIN           (1U)        /**< @brief Minimum VLAN ID */
#define ETHER_VLANID_MAX           (4094U)     /**< @brief Maximum VLAN ID */
#define ETHER_VLANID_UNTAG         (0x8000U)   /**< @brief Refers to untagged packets */
#define ETHER_VLANID_ALL           (0x8001U)   /**< @brief Any VLAN ID packets */
#define ETHER_VLANID_EXC_UNTAG     (0x8002U)   /**< @brief Except untagged packets */
#define ETHER_VLANID_UNTAG_PRIOTAG (0x8003U)   /**< @brief Untagged & priority tagged packets */
/** @} */

/**
    @name Traffic class type
    @{
    @brief Traffic class type

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint8_t ETHER_TCType;              /**< @brief Traffic class type */
#define ETHER_TC_0       (0U)              /**< @brief Traffic class 0 */
#define ETHER_TC_1       (1U)              /**< @brief Traffic class 1 */
#define ETHER_TC_2       (2U)              /**< @brief Traffic class 2 */
#define ETHER_TC_3       (3U)              /**< @brief Traffic class 3 */
#define ETHER_TC_4       (4U)              /**< @brief Traffic class 4 */
#define ETHER_TC_5       (5U)              /**< @brief Traffic class 5 */
#define ETHER_TC_6       (6U)              /**< @brief Traffic class 6 */
#define ETHER_TC_7       (7U)              /**< @brief Traffic class 7 */
#define ETHER_TC_INVALID (ETHER_TC_7 + 1U) /**< @brief Invalid traffic
                                                       class */
#define ETHER_TC_MAX     (8UL)             /**< @brief Traffic class max */
/** @} */

/**
    @name Port ID type
    @{
    @brief Port index type

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint32_t ETHER_PortIDType;           /**< @brief Port index type  @xsd input:int */
#define ETHER_PORT_ID_INVALID (0xFFFFFFFFUL) /**< @brief Invalid port index */
/** @} */

/**
    @name Port Mask type
    @{
    @brief Switch port mask type

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint64_t ETHER_PortMaskType;
/** @} */

/**
    @name L4 Protocol type
    @{
    @brief L4 Protocol

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint8_t ETHER_L4ProtoType;   /**< @brief Protocol type                              */
#define ETHER_L4PROTO_NONE    (0U)   /**< @brief Do not match protocol field from IP header */
#define ETHER_L4PROTO_TCP     (6U)   /**< @brief TCP (RFC793)                               */
#define ETHER_L4PROTO_UDP     (17U)  /**< @brief UDP (RFC768)                               */
#define ETHER_L4PROTO_SCTP    (132U) /**< @brief SCTP (RFC4960)                             */
/** @} */

/**
    @name Ether IP version type
    @{
    @brief Ethernet IP version ipv4 or ipv6

    @trace #BRCM_SWREQ_ETHER
*/
typedef uint32_t ETHER_IpVersionType;
#define ETHER_IPVERSION_V4              (0UL)          /**< @brief IPV4 */
#define ETHER_IPVERSION_V6              (1UL)          /**< @brief IPV6 */
/** @} */


/**
    @brief IPV4 / IPV6 Address type

    Example IPV4 Address (192.168.10.1):         <BR>
    ipv6 = 0                                     <BR>
    address = {192, 168,  10,  1,  0, 0, 0, 0    <BR>
                 0,   0,   0,  0,  0, 0, 0, 0}   <BR>
                                                 <BR>
    Example IPV6 Address (2001:db8:3333:4444:5555:6666:7777:8888): <BR>
    ipv6 = 1                                                       <BR>
    address = {0x20, 0x01, 0x0D, 0xB8, 0x33, 0x33, 0x44, 0x44,     <BR>
               0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88}     <BR>

    @trace #BRCM_SWREQ_ETHER
*/
typedef struct sETHER_IpAddressType {
    ETHER_IpVersionType ipVer;     /**< @brief @xsd ipver:address                       */
    uint8_t  address[16];          /**< @brief ip address  @xsd type:ip;default:0.0.0.0 */
} ETHER_IpAddressType;

/**
    @brief Ethernet RX packet statistics

    @trace #BRCM_SWREQ_ETHER
 */
typedef struct sETHER_RxStatsType {
    uint32_t gdPkts;          /**< @brief All good packets */
    uint32_t octetsLow;       /**< @brief Octets low packets */
    uint32_t octetsHigh;      /**< @brief Octets high packets */
    uint32_t allPkts;         /**< @brief All good or bad packets */
    uint32_t brdCast;         /**< @brief All good broadcast packets */
    uint32_t mutCast;         /**< @brief All good multicast packets */
    uint32_t uniCast;         /**< @brief All good unicast packets */
    uint32_t pkts64;          /**< @brief All good/bad packets with len < 64 */
    uint32_t pkts65_127;      /**< @brief All good/bad packets with len between
                                    65 to 127 */
    uint32_t pkts128_255;     /**< @brief All good/bad packets with len between
                                    128 to 255 */
    uint32_t pkts256_511;     /**< @brief All good/bad packets with len between
                                    256 to 511 */
    uint32_t pkts512_1023;    /**< @brief All good/bad packets with len between
                                    512 to 1023 */
    uint32_t pkts1024_MAX;    /**< @brief All good/bad packets with len between
                                    1024 and the standard maximum packet size
                                    inclusive */
    uint32_t pkts1024_1522;   /**< @brief All good/bad packets with len between
                                    1024 to 1522 */
    uint32_t pkts1523_2047;   /**< @brief All good/bad packets with len between
                                    1523 to 2047 */
    uint32_t pkts2048_4095;   /**< @brief All good/bad packets with len between
                                    2048 to 4095 */
    uint32_t pkts4096_8191;   /**< @brief All good/bad packets with len between
                                    4095 to 8191 */
    uint32_t pkts8192_MAX;    /**< @brief All good/bad packets with len between
                                    8192 to MAX */
    uint32_t pktsJabber;      /**< @brief All pkts with length>1536 and a bad CRC */
    uint32_t pktsOvrSz;       /**< @brief Rx pkts with length > RX_LENGTH and a
                                    good CRC */
    uint32_t pktsFrag;        /**< @brief All pkts with length < 64 and a bad CRC */
    uint32_t pktsRxDrop;      /**< @brief All pkts that experienced an overrun */
    uint32_t pktsCrcAlignErr; /**< @brief All pkts with a bad CRC or odd number
                                    of nibbles */
    uint32_t pktsUndSz;       /**< @brief All pkts with length < 64 with a good CRC */
    uint32_t pktsCrcErr;      /**< @brief All pkts that did not pass FCS check */
    uint32_t pktsRxDiscard;   /**< @brief All pkts that did not pass FCS check and discarded by the receiver. */
    uint32_t rxPause;         /**< @brief Number of PAUSE events */
} ETHER_RxStatsType;

/**
    @brief Switch MIBS RX packet statistics

    @trace #BRCM_SWREQ_ETHER
 */
typedef struct sETHER_RxStatsV2Type {
    uint64_t octets;          /**< @brief Octet counters                        */
    uint64_t underSizePkts;   /**< @brief All undersize packets with
                                          size between 48 and 63 bytes          */
    uint64_t pkts64;          /**< @brief Packets with len 64                   */
    uint64_t pkts65_127;      /**< @brief Packets with len between 65 to 127    */
    uint64_t pkts128_255;     /**< @brief Packets with len between 128 to 255   */
    uint64_t pkts256_511;     /**< @brief Packets with len between 256 to 511   */
    uint64_t pkts512_1023;    /**< @brief Packets with len between 512 to 1023  */
    uint64_t pkts1024_MAX;    /**< @brief Packets with len between
                                    1024 and the standard maximum packets size
                                    inclusive                                   */
    uint64_t jumboPkt;        /**< @brief Jumbo packets                         */
    uint64_t overSizePkt;     /**< @brief Oversize packets                      */
    uint64_t pausePkt;        /**< @brief Pause packets                         */
    uint64_t pktsFCSerr;      /**< @brief Packets with FCS error                */
    uint64_t gdPkts;          /**< @brief Good octets packets                   */
    uint64_t uniCast;         /**< @brief Unicast packets                       */
    uint64_t multiCast;       /**< @brief Multicast packets                     */
    uint64_t brdCast;         /**< @brief Broadcast packets                     */
    uint64_t frameDiscard;    /**< @brief Frames that got discarded             */
    uint64_t dropPkt;         /**< @brief Packets that got dropped              */
    uint64_t changedSA;       /**< @brief Packets with Source Address changed   */
    uint64_t alignErr;        /**< @brief Packets with alignment error          */
    uint64_t jabber;          /**< @brief Packets jabber error                  */
    uint64_t fragment;        /**< @brief Packets that are fragmented           */
    uint64_t symbolErr;       /**< @brief Packets with symbol error             */
    uint64_t inRangeErr;      /**< @brief In Range Error                        */
    uint64_t outRangeErr;     /**< @brief Out of Range Error                       */
    uint64_t jumboJabber;     /**< @brief Packets with jumbo jabber             */
    uint64_t ip4Checkfail;    /**< @brief Packets with IPV4 Checksum Failure    */
    uint64_t vlanTags;        /**< @brief VLAN Tags in Clear for MACSEC packets */
    uint64_t pauseSkip;       /**< @brief Pause Skip                            */
    uint64_t uniCastFail;     /**< @brief Packets with unicast lookup fail      */
    uint64_t multiCastFail;   /**< @brief Packets with multicast lookup fail    */
    uint64_t rsvd[16ULL];
} ETHER_RxStatsV2Type;

/**
    @brief Ethernet TX packet statistics

    @trace #BRCM_SWREQ_ETHER
 */
typedef struct sETHER_TxStatsType {
    uint32_t octets;               /**< @brief Total number of bytes transmitted */
    uint32_t brdCast;              /**< @brief All good broadcast packets */
    uint32_t mutCast;              /**< @brief All good multicast packets */
    uint32_t uniCast;              /**< @brief All good unicast packets */
    uint32_t txDropped;            /**< @brief All dropped packets */
    uint32_t txDroppedErr;         /**< @brief All packets dropped due to error */
    uint32_t txCollision;          /**< @brief TX collisions */
    uint32_t txCollisionSingle;    /**< @brief Tx single collision */
    uint32_t txCollisionMulti;     /**< @brief Tx Multi collision counter */
    uint32_t txLateCollision;      /**< @brief Tx late collison counter */
    uint32_t txExcessiveCollision; /**< @brief Tx excessive collision counter */
    uint32_t txDeferredTransmit;   /**< @brief Tx deferred transmit counter */
    uint32_t txFrameInDiscard;     /**< @brief Tx deferred transmit counter */
    uint32_t txPause;              /**< @brief Number of PAUSE events */
    uint32_t txQ0;                 /**< @brief Total number of good packets
                                         transmitted on COS0 */
    uint32_t txQ1;                 /**< @brief Total number of good packets
                                         transmitted on COS1 */
    uint32_t txQ2;                 /**< @brief Total number of good packets
                                         transmitted on COS2 */
    uint32_t txQ3;                 /**< @brief Total number of good packets
                                         transmitted on COS3 */
    uint32_t txQ4;                 /**< @brief Total number of good packets
                                         transmitted on COS4 */
    uint32_t txQ5;                 /**< @brief Total number of good packets
                                         transmitted on COS5 */
    uint32_t txQ6;                 /**< @brief Total number of good packets
                                         transmitted on COS6 */
    uint32_t txQ7;                 /**< @brief Total number of good packets
                                         transmitted on COS7 */
    uint32_t pkts64;               /**< @brief All good/bad packets with len < 64 */
    uint32_t pkts65_127;           /**< @brief All good/bad packets with len
                                         between 65 to 127 */
    uint32_t pkts128_255;          /**< @brief All good/bad packets with len
                                         between 128 to 255 */
    uint32_t pkts256_511;          /**< @brief All good/bad packets with len
                                         between 256 to 511 */
    uint32_t pkts512_1023;         /**< @brief All good/bad packets with len
                                         between 512 to 1023 */
    uint32_t pkts1024_MAX;         /**< @brief All good/bad packets with len
                                         between 1024 and the standard maximum
                                         packet size inclusive */
} ETHER_TxStatsType;

/**
    @brief Ethernet TX packet statistics

    @trace #BRCM_SWREQ_ETHER
 */
typedef struct sETHER_TxStatsV2Type {
    uint64_t octets;            /**< @brief Octet counters                      */
    uint64_t pkts64;            /**< @brief Packets with 64 bytes                */
    uint64_t pkts65_127;        /**< @brief Packets with len between 65 to 127   */
    uint64_t pkts128_255;       /**< @brief Packets with len between 128 to 255  */
    uint64_t pkts256_511;       /**< @brief Packets with len between 256 to 511  */
    uint64_t pkts512_1023;      /**< @brief Packets with len between 512 to 1023 */
    uint64_t pkts1024_MAX;      /**< @brief Packets with len between
                                      1024 and the standard maximum packets size
                                      inclusive                                 */
    uint64_t brdCast;           /**< @brief Broadcast packets                    */
    uint64_t multiCast;         /**< @brief Multicast packets                    */
    uint64_t uniCast;           /**< @brief Unicast packets                      */
    uint64_t pausePkt;          /**< @brief Pause packets                        */
    uint64_t frameDiscardUni;   /**< @brief Frame discard unicast               */
    uint64_t Q0;                /**< @brief Total number of good packets
                                      transmitted on COS0                       */
    uint64_t Q1;                /**< @brief Total number of good packets
                                      transmitted on COS1                       */
    uint64_t Q2;                /**< @brief Total number of good packets
                                      transmitted on COS2                       */
    uint64_t Q3;                /**< @brief Total number of good packets
                                      transmitted on COS3                       */
    uint64_t Q4;                /**< @brief Total number of good packets
                                      transmitted on COS4                       */
    uint64_t Q5;                /**< @brief Total number of good packets
                                      transmitted on COS5                       */
    uint64_t Q6;                /**< @brief Total number of good packets
                                      transmitted on COS6                       */
    uint64_t Q7;                /**< @brief Total number of good packets
                                      transmitted on COS7                       */
    uint64_t frameDiscardMulti; /**< @brief Frame discard multicast             */
    uint64_t frameDiscardBroad; /**< @brief Frame discard broadcast             */
    uint64_t unsuccessPkt;      /**< @brief Unsuccessful packets                 */
    uint64_t QBVmaxSDUQ0;       /**< @brief QBV Max SDU Queue 0                 */
    uint64_t QBVmaxSDUQ1;       /**< @brief QBV Max SDU Queue 1                 */
    uint64_t QBVmaxSDUQ2;       /**< @brief QBV Max SDU Queue 2                 */
    uint64_t QBVmaxSDUQ3;       /**< @brief QBV Max SDU Queue 3                 */
    uint64_t QBVmaxSDUQ4;       /**< @brief QBV Max SDU Queue 4                 */
    uint64_t QBVmaxSDUQ5;       /**< @brief QBV Max SDU Queue 5                 */
    uint64_t QBVmaxSDUQ6;       /**< @brief QBV Max SDU Queue 6                 */
    uint64_t QBVmaxSDUQ7;       /**< @brief QBV Max SDU Queue 7                 */
    uint64_t LPIevent;          /**< @brief LPI Event                           */
    uint64_t LPIduration;       /**< @brief LPI Duration                        */
    uint64_t rsvd[16ULL];
} ETHER_TxStatsV2Type;

/**
    @name ETHER_MDIOHwIDType
    @{
    @brief MDIO controller index type

    @trace #BRCM_SWREQ_ETHER
 */
/* Just to support legacy */
typedef uint32_t ETHER_MDIOHwIDType;            /**< @brief MDIO controller index type */
/** @} */

/**
    @name ETHER_MDIOSlaveAddrType
    @{
    @brief MDIO Slave Address Type

    @trace #BRCM_SWREQ_ETHER
*/
/* Just to support legacy */
typedef uint8_t ETHER_MDIOSlaveAddrType;
#define ETHER_MDIOSLAVEADDR_MASK         (0x1FU) /**< @brief MDIO slave address field mask */
#define ETHER_MDIOSLAVEADDR_MAX          (31U)   /**< @brief MDIO slave address max value */
/** @} */

/**
    @name ETHER_MDIODeviceType
    @{
    @brief MDIO Device Type

    @trace #BRCM_SWREQ_ETHER
*/
/* Just to support legacy */
typedef uint8_t ETHER_MDIODeviceType;
#define ETHER_MDIODEVICETYPE_MASK        (0x1FU) /**< @brief MDIO device type field mask */
#define ETHER_MDIODEVICETYPE_MAX         (31U)   /**< @brief MDIO device type max value */
/** @} */

/**
    @name ETHER_MDIORegAccessType
    @{
    @brief MDIO register access type

    @trace #BRCM_SWREQ_ETHER
 */
/* Just to support legacy */
typedef uint8_t ETHER_MDIORegAccessType;
#define ETHER_MDIOREGACCESS_CL22       (1U)   /**< @brief MDIO slave CL22 register access */
#define ETHER_MDIOREGACCESS_CL45       (2U)   /**< @brief MDIO slave CL45 register access */
/** @} */

/**
    @name ETHER_MDIORegAddrType
    @{
    @brief MDIO register addres type

    @trace #BRCM_SWREQ_ETHER
 */
/* Just to support legacy */
typedef uint16_t ETHER_MDIORegAddrType;
#define ETHER_MDIOREGADDR_CL22_MASK     (0x1FUL)    /**< @brief MDIO register address mask for CL22 register access */
#define ETHER_MDIOREGADDR_CL45_MASK     (0xFFFFUL) /**< @brief MDIO register address mask for CL45 register access */
/** @} */

/**
    @brief MDIO transfer packet

    MDIO transfer packet structure.

    @trace #BRCM_SWREQ_ETHER
*/
/* Just to support legacy */
typedef struct sETHER_MDIOPktType {
    ETHER_MDIOSlaveAddrType slaveAddr;   /**< @brief Device address of the MDIO slave.
                                              This slaveAddr is 5bit address */
    ETHER_MDIODeviceType    deviceType;  /**< @brief Device type */
    ETHER_MDIORegAccessType regAccMode;  /**< @brief Slave register access mode type */
    ETHER_MDIORegAddrType   regAddr;     /**< @brief Slave register address */
    uint16_t regData;                    /**< @brief Data read from slave register
                                              or data to write to slave register */
} ETHER_MDIOPktType;

/** @brief Initialize Ethernet common

    This API initializes the Ethernet top level common properties.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      hwId            Index of the Ethernet Hw.
    @param[in]      extPhySel     Select external PHY 0 = BRPHY (CU-PHY 100M/1G), 1 = S16UP (External PHY 2.5G)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHER
*/
extern int32_t ETHER_CommonInit(ETHER_HwIDType hwId, uint8_t extPhySel);

#endif /* ETHERNET_COMMON_H */
/** @} */
