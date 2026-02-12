/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xgmacdrv_ifc XGMAC
    @ingroup grp_xgmacdrv

    @addtogroup grp_xgmacdrv_ifc
    @{
    @section sec_xgmac_overview Overview

    XGMAC IP block is used as MAC controller. It is connected as the master to main fabric port on system side.
    It controls MAC behaviour when sending and receiving packets. The buffer descriptors are created by the
    software network stack running on one of the available processors.
    The MAC is controlled by the host processor by accessing the configuration and control registers on APB interface.
        -# Interface internal automotive PHY on internal GMII/MII port.
        -# Communicate with External multi-gigabit Automotive Ethernet PHY on the SGMII (SERDES) interface.
        -#  XGMAC HW instance is accessed based on HwID passed as the first parameter in each API.
       XGMAC controller base address is accessed based on index HwID passed as 0 to XGMAC_MAX_HW_ID - 1,
       depending on HW instance.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   XGMAC_0     |

    @section api_list List of XGMAC APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref XGMAC_DrvInit                   | Initialize XGMAC driver                   |
    | @ref XGMAC_DrvDeInit                 | De-initialize XGMAC                       |
    | @ref XGMAC_SpeedModeSet              | Configure Speed Mode                      |
    | @ref XGMAC_SetMacAddress             | Configure MAC address                     |
    | @ref XGMAC_MacPacketFilterEnable     | XGMAC packet filter enable                |
    | @ref XGMAC_SetAddressFilter          | Set address filter                        |
    | @ref XGMAC_SetFlowCtrl               | Set flow control                          |
    | @ref XGMAC_MacEnable                 | MAC enable to transmit and receive packets|
    | @ref XGMAC_SetMacLoopback            | Set loopback mode                         |
    | @ref XGMAC_SetMTLTxQConfiguration    | Set MTL Transmit Queue configuration      |
    | @ref XGMAC_SetMTLRxQConfiguration    | Set MTL Receive Queue configuration       |
    | @ref XGMAC_FlushTxFifo               | Flush the TX FIFO                         |
    | @ref XGMAC_DMAModeSwReset            | DMA mode software reset                   |
    | @ref XGMAC_DMAInit                   | DMA initialize                            |
    | @ref XGMAC_SetDMAQMode               | Enable Queue in selected mode             |
    | @ref XGMAC_DMASetTxRingLen           | Configure DMA transmit ring length        |
    | @ref XGMAC_DMASetRxRingLen           | Configure DMA receive ring length         |
    | @ref XGMAC_DMASetRxTailPtr           | Configure DMA receive tail pointer        |
    | @ref XGMAC_DMASetTxTailPtr           | Configure DMA transmit tail pointer       |
    | @ref XGMAC_DMATransmitEnable         | Enable DMA transmit                       |
    | @ref XGMAC_DMAReceiveEnable          | Enable DMA receive                        |
    | @ref XGMAC_DMAChannelStatusGet       | DMA channel status get                    |
    | @ref XGMAC_DMAChannelStatusClear     | DMA channel status clear                  |
    | @ref XGMAC_EnableVlanTag             | Enable VLAN Tag                           |
    | @ref XGMAC_SetVlanTag                | Set VLAN Tag                              |
    | @ref XGMAC_GetDriverStatus           | Get driver initialization status          |
    | @ref XGMAC_DMASetRxDescListAddr      | Set RX descriptor base address            |
    | @ref XGMAC_DMASetTxDescListAddr      | Set Tx Descriptor base address            |
    | @ref XGMAC_DMASetRecBufferSize       | Set receive buffer size                   |
    | @ref XGMAC_DMAInitChannel            | Initialize DMA channel                    |
    | @ref XGMAC_DMAInitTxChan             | Initialize DMA transmit channel           |
    | @ref XGMAC_DMAInitRxChan             | Initialize DMA receive channel            |
    | @ref XGMAC_EnableCoreInterrupt       | Enable MAC core interrupt                 |
    | @ref XGMAC_EnableDMAChannelInterrupt | Enable DMA channel Interrupt              |
    | @ref XGMAC_ClearCoreInterrupt        | Clear MAC core interrupt                  |
    | @ref XGMAC_GetCoreInterruptStatus    | MAC core interrupt status get             |
    | @ref XGMAC_GetLinkStatus             | MAC core link status get                  |
    | @ref XGMAC_GetMIBCounterStats        | Gets the MIB counter statistics           |
    @file xgmac.h
    @brief Interface for XGMAC driver
    This header file defines interface for XGMAC driver.

    @version 0.1 Initial Version
*/

#ifndef XGMAC_H
#define XGMAC_H

/**
    @name XGMAC Driver API IDs
    @{
    @brief API IDs for XGMAC Driver
*/
#define BRCM_SWARCH_XGMAC_HWID_TYPE                             (0x8500U) /**< @brief #XGMAC_HwIDType                  */
#define BRCM_SWARCH_XGMAC_ETH_SPEED_MODE_TYPE                   (0x8501U) /**< @brief #XGMAC_EthSpeedModeType          */
#define BRCM_SWARCH_XGMAC_ETH_DUPLEX_MODE_TYPE                  (0x8502U) /**< @brief #XGMAC_EthDuplexModeType         */
#define BRCM_SWARCH_XGMAC_PKT_FILTER_MODE_TYPE                  (0x8503U) /**< @brief #XGMAC_PktFilterModeType         */
#define BRCM_SWARCH_XGMAC_IPG_CTRL_TYPE                         (0x8504U) /**< @brief #XGMAC_IPGCtrlType               */
#define BRCM_SWARCH_XGMAC_IPG_LENGTH_STEP_TYPE                  (0x8505U) /**< @brief #XGMAC_IPGLengthStepType         */
#define BRCM_SWARCH_XGMAC_TXQ_THR_CTRL_TYPE                     (0x8506U) /**< @brief #XGMAC_TxQThrCtrlType            */
#define BRCM_SWARCH_XGMAC_RXQ_THR_CTRL_TYPE                     (0x8507U) /**< @brief #XGMAC_RxQThrCtrlType            */
#define BRCM_SWARCH_XGMAC_TXQ_MAP_TRAFFIC_CLASS_TYPE            (0x8508U) /**< @brief #XGMAC_TxQMapTrafficClassType    */
#define BRCM_SWARCH_XGMAC_VLAN_TAG_TYPE                         (0x8509U) /**< @brief #XGMAC_VlanTagType               */
#define BRCM_SWARCH_XGMAC_JUMBO_PKT_SUPPORT_TYPE                (0x850AU) /**< @brief #XGMAC_JumboPktSupportType       */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_TYPE               (0x850BU) /**< @brief #XGMAC_DMAChannelStatusType      */
#define BRCM_SWARCH_XGMAC_DMA_QMODE_TYPE                        (0x850CU) /**< @brief #XGMAC_DMAQModeType              */
#define BRCM_SWARCH_XGMAC_MAC_ADDR_TYPE                         (0x850DU) /**< @brief #XGMAC_MACAddrType               */
#define BRCM_SWARCH_XGMAC_ETH_LINK_INFO_TYPE                    (0x850EU) /**< @brief #XGMAC_EthLinkInfoType           */
#define BRCM_SWARCH_XGMAC_ETH_MAC_ADDR_TYPE                     (0x850FU) /**< @brief #XGMAC_EthMacAddrType            */
#define BRCM_SWARCH_XGMAC_ETH_PKT_FILTER_TYPE                   (0x8510U) /**< @brief #XGMAC_EthPktFilterType          */
#define BRCM_SWARCH_XGMAC_FLOW_CTRL_CFG_TYPE                    (0x8511U) /**< @brief #XGMAC_FlowCtrlCfgType           */
#define BRCM_SWARCH_XGMAC_ETH_CONFIG_TYPE                       (0x8512U) /**< @brief #XGMAC_EthConfigType             */
#define BRCM_SWARCH_XGMAC_MTL_TXQ_CONFIG_TYPE                   (0x8513U) /**< @brief #XGMAC_MtlTxQConfigType          */
#define BRCM_SWARCH_XGMAC_MTL_RXQ_CONFIG_TYPE                   (0x8514U) /**< @brief #XGMAC_MtlRxQConfigType          */
#define BRCM_SWARCH_XGMAC_DMA_CFG_TYPE                          (0x8515U) /**< @brief #XGMAC_DMACfgType                */
#define BRCM_SWARCH_XGMAC_DRIVER_STATUS_TYPE                    (0x8516U) /**< @brief #XGMAC_DriverStatusType          */
#define BRCM_SWARCH_XGMAC_DMA_BURST_LENGTH_TYPE                 (0x8517U) /**< @brief #XGMAC_DMABurstLengthType        */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_CFG_TYPE                  (0x8518U) /**< @brief #XGMAC_DMAChannelCfgType         */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_TX_CTRL_TYPE              (0x8519U) /**< @brief #XGMAC_DMAChannelTxCtrlType      */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_RX_CTRL_TYPE              (0x851AU) /**< @brief #XGMAC_DMAChannelRxCtrlType      */
#define BRCM_SWARCH_XGMAC_CORE_INTERRUPT_TYPE                   (0x851BU)  /**< @brief #XGMAC_CoreInterruptType         */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_INTERRUPT_TYPE            (0x851CU)  /**< @brief #XGMAC_DMAChannelInterruptType   */
#define BRCM_SWARCH_XGMAC_CORE_INTERRUPT_STATUS_TYPE            (0x851DU)  /**< @brief #XGMAC_CoreInterruptStatusType   */
#define BRCM_SWARCH_XGMAC_LINK_STATUS_TYPE                      (0x851EU)  /**< @brief #XGMAC_LinkStatusType            */
#define BRCM_SWARCH_XGMAC_MIB_COUNTER_TYPE                      (0x851FU)  /**< @brief #XGMAC_MIBCounterType            */

#define BRCM_SWARCH_XGMAC_DRV_INIT_PROC                         (0x8520U) /**< @brief #XGMAC_DrvInit                   */
#define BRCM_SWARCH_XGMAC_DRV_DEINIT_PROC                       (0x8521U) /**< @brief #XGMAC_DrvDeInit                 */
#define BRCM_SWARCH_XGMAC_SPEED_MODE_SET_PROC                   (0x8522U) /**< @brief #XGMAC_SpeedModeSet              */
#define BRCM_SWARCH_XGMAC_SET_MAC_ADDRESS_PROC                  (0x8523U) /**< @brief #XGMAC_SetMacAddress             */
#define BRCM_SWARCH_XGMAC_MAC_PACKET_FILTER_ENABLE_PROC         (0x8524U) /**< @brief #XGMAC_MacPacketFilterEnable     */
#define BRCM_SWARCH_XGMAC_SET_ADDRESS_FILTER_PROC               (0x8525U) /**< @brief #XGMAC_SetAddressFilter          */
#define BRCM_SWARCH_XGMAC_SET_FLOW_CTRL_PROC                    (0x8526U) /**< @brief #XGMAC_SetFlowCtrl               */
#define BRCM_SWARCH_XGMAC_MAC_ENABLE_PROC                       (0x8527U) /**< @brief #XGMAC_MacEnable                 */

#define BRCM_SWARCH_XGMAC_SET_MAC_LOOPBACK_PROC                 (0x8529U) /**< @brief #XGMAC_SetMacLoopback            */
#define BRCM_SWARCH_XGMAC_SET_MTL_TXQ_CONFIGURATION_PROC        (0x852AU) /**< @brief #XGMAC_SetMTLTxQConfiguration    */
#define BRCM_SWARCH_XGMAC_SET_MTL_RXQ_CONFIGURATION_PROC        (0x852BU) /**< @brief #XGMAC_SetMTLRxQConfiguration    */
#define BRCM_SWARCH_XGMAC_FLUSH_TX_FIFO_PROC                    (0x852CU) /**< @brief #XGMAC_FlushTxFifo               */
#define BRCM_SWARCH_XGMAC_DMA_MODE_SW_RESET_PROC                (0x852DU) /**< @brief #XGMAC_DMAModeSwReset            */
#define BRCM_SWARCH_XGMAC_DMA_INIT_PROC                         (0x852EU) /**< @brief #XGMAC_DMAInit                   */
#define BRCM_SWARCH_XGMAC_SET_DMA_QMODE_PROC                    (0x852FU) /**< @brief #XGMAC_SetDMAQMode               */
#define BRCM_SWARCH_XGMAC_DMA_SET_TX_RING_LEN_PROC              (0x8531U) /**< @brief #XGMAC_DMASetTxRingLen           */
#define BRCM_SWARCH_XGMAC_DMA_SET_RX_RING_LEN_PROC              (0x8532U) /**< @brief #XGMAC_DMASetRxRingLen           */
#define BRCM_SWARCH_XGMAC_DMA_SET_RX_TAILPTR_PROC               (0x8533U) /**< @brief #XGMAC_DMASetRxTailPtr           */
#define BRCM_SWARCH_XGMAC_DMA_SET_TX_TAILPTR_PROC               (0x8534U) /**< @brief #XGMAC_DMASetTxTailPtr           */
#define BRCM_SWARCH_XGMAC_DMA_TRANSMIT_ENABLE_PROC              (0x8535U) /**< @brief #XGMAC_DMATransmitEnable         */
#define BRCM_SWARCH_XGMAC_DMA_RECEIVE_ENABLE_PROC               (0x8536U) /**< @brief #XGMAC_DMAReceiveEnable          */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_GET_PROC           (0x8537U) /**< @brief #XGMAC_DMAChannelStatusGet       */
#define BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_CLEAR_PROC         (0x8538U) /**< @brief #XGMAC_DMAChannelStatusClear     */
#define BRCM_SWARCH_XGMAC_ENABLE_VLAN_TAG_PROC                  (0x8539U) /**< @brief #XGMAC_EnableVlanTag             */
#define BRCM_SWARCH_XGMAC_SET_VLAN_TAG_PROC                     (0x853AU) /**< @brief #XGMAC_SetVlanTag                */
#define BRCM_SWARCH_XGMAC_GET_DRIVER_STATUS_PROC                (0x853BU) /**< @brief #XGMAC_GetDriverStatus           */
#define BRCM_SWARCH_XGMAC_DMA_SET_RX_DESC_LIST_ADDR_PROC        (0x853CU) /**< @brief #XGMAC_DMASetRxDescListAddr      */
#define BRCM_SWARCH_XGMAC_DMA_SET_TX_DESC_LIST_ADDR_PROC        (0x853DU) /**< @brief #XGMAC_DMASetTxDescListAddr      */
#define BRCM_SWARCH_XGMAC_DMA_SET_REC_BUFFER_SIZE_PROC          (0x853EU) /**< @brief #XGMAC_DMASetRecBufferSize       */
#define BRCM_SWARCH_XGMAC_DMA_INIT_CHANNEL_PROC                 (0x853FU) /**< @brief #XGMAC_DMAInitChannel            */
#define BRCM_SWARCH_XGMAC_DMA_INIT_TX_CHAN_PROC                 (0x8540U) /**< @brief #XGMAC_DMAInitTxChan             */
#define BRCM_SWARCH_XGMAC_DMA_INIT_RX_CHAN_PROC                 (0x8541U) /**< @brief #XGMAC_DMAInitRxChan             */
#define BRCM_SWARCH_XGMAC_ENABLE_CORE_INTERRUPT_PROC            (0x8542U) /**< @brief #XGMAC_EnableCoreInterrupt       */
#define BRCM_SWARCH_XGMAC_ENABLE_DMA_CHANNEL_INTERRUPT_PROC     (0x8543U) /**< @brief #XGMAC_EnableDMAChannelInterrupt */
#define BRCM_SWARCH_XGMAC_CLEAR_CORE_INTERRUPT_PROC             (0x8544U) /**< @brief #XGMAC_ClearCoreInterrupt        */
#define BRCM_SWARCH_XGMAC_GET_CORE_INTERRUPT_STATUS_PROC        (0x8545U) /**< @brief #XGMAC_GetCoreInterruptStatus    */
#define BRCM_SWARCH_XGMAC_GET_LINK_STATUS_PROC                  (0x8546U) /**< @brief #XGMAC_GetLinkStatus             */
#define BRCM_SWARCH_XGMAC_GET_MIB_COUNTER_STATS_PROC            (0x8547U) /**< @brief #XGMAC_GetMIBCounterStats        */
#define BRCM_SWARCH_XGMAC_RXQ_DMA_MAP_TYPE                      (0x8548U)  /**< @brief #XGMAC_RxQDmaMapType */
#define BRCM_SWARCH_XGMAC_RXQ_DMA_CHAN_TYPE                     (0x8549U)  /**< @brief #XGMAC_RxQDmaChanType */
#define BRCM_SWARCH_XGMAC_MTL_TC_TSA_ALGO_TYPE                  (0x854AU)  /**< @brief #XGMAC_MtlTcTsaAlgoType */
#define BRCM_SWARCH_XGMAC_MTL_TC_ETS_ALGO_TYPE                  (0x854BU)  /**< @brief #XGMAC_MtlTcEtsAlgoType */
#define BRCM_SWARCH_XGMAC_MTL_RXQ_ARBITRATION_TYPE              (0x854CU)  /**< @brief #XGMAC_MTLRxQArbitrationType */
#define BRCM_SWARCH_XGMAC_MTL_RXQ_ARBITRATION_ALGO_TYPE         (0x854DU)  /**< @brief #XGMAC_MTLRxQArbitrationAlgoType */

/** @} */


/**
    @name XGMAC_HwIDType
    @{
    @brief Index of the XGMAC controller

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
 */
typedef uint32_t XGMAC_HwIDType;
#define XGMAC_HWID_0 (0UL)  /**< @brief XGMAC HW controller index 0 */
/** @} */

/**
    @name XGMAC_EthSpeedModeType
    @{
    @brief XGMAC link speed

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
 */
typedef uint32_t XGMAC_EthSpeedModeType;
#define XGMAC_ETH_SPEED_MODE_10M_MII       (0x7UL) /**< @brief 10 Mbps link speed            */
#define XGMAC_ETH_SPEED_MODE_100M_MII      (0x4UL) /**< @brief 100 Mbps link speed           */
#define XGMAC_ETH_SPEED_MODE_1G_GMII       (0x3UL) /**< @brief 1 Gpbs link speed             */
#define XGMAC_ETH_SPEED_MODE_2P5G_GMII     (0x2UL) /**< @brief 2.5 Gpbs GMII link speed      */
#define XGMAC_ETH_SPEED_MODE_2P5G_XGMII    (0x6UL) /**< @brief 2.5Gbps Gpbs XGMII link speed */
/** @} */

/**
    @name XGMAC_EthDuplexModeType
    @{
    @brief Ethernet duplex mode

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_EthDuplexModeType;
#define XGMAC_ETH_DUPLEX_MODE_FULL         (0UL) /**< @brief Full duplex link */
#define XGMAC_ETH_DUPLEX_MODE_HALF         (1UL) /**< @brief Half duplex link */
/** @} */

/**
    @name XGMAC_PktFilterModeType
    @{
    @brief XGMAC packet filter type enable

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_PktFilterModeType;
#define XGMAC_PKT_FILTER_MODE_GRP0                     (0xFUL)     /**< @brief Packet filter mode group0                          */
#define XGMAC_PKT_FILTER_MODE_REC_ALL                  (1UL<<0UL)  /**< @brief Receive all packets irrespective of whether they
                                                                         pass the address filter or not.                          */
#define XGMAC_PKT_FILTER_MODE_REC_BROADCAST            (1UL<<1UL)  /**< @brief Receive frame with Broadcast address               */
#define XGMAC_PKT_FILTER_MODE_REC_MULTICAST            (1UL<<2UL)  /**< @brief Receive frame with MULTICAST address               */
#define XGMAC_PKT_FILTER_MODE_PR                       (1UL<<3UL)  /**< @brief Enable Promiscuous Mode when this bit is set       */
#define XGMAC_PKT_FILTER_MODE_GRP1                     (0xF0UL)    /**< @brief Packet filter mode group1                          */
#define XGMAC_PKT_FILTER_MODE_PCF                      (1UL<<4UL)  /**< @brief Enable to pass control packets                     */
#define XGMAC_PKT_FILTER_MODE_SAF                      (1UL<<5UL)  /**< @brief Enable Source address filter                       */
#define XGMAC_PKT_FILTER_MODE_VLAN_TAG_FILTER_ENABLE   (1UL<<6UL)  /**< @brief Enable VLAN tag filter.                            */
#define XGMAC_PKT_FILTER_MODE_HPF_FILTER_ENABLE        (1UL<<7UL)  /**< @brief Enable Hash or Perfect Filter when this bit is set */
#define XGMAC_PKT_FILTER_MODE_GRP2                     (0xF00UL)   /**< @brief Packet filter mode group2                          */
#define XGMAC_PKT_FILTER_MODE_HUC                      (1UL<<8UL)  /**< @brief Hash Unicast                                       */
#define XGMAC_PKT_FILTER_MODE_HMC                      (1UL<<9UL)  /**< @brief Hash Multicast                                     */
#define XGMAC_PKT_FILTER_MODE_SAIF                     (1UL<<10UL) /**< @brief Enable Inverse Source address filter               */
#define XGMAC_PKT_FILTER_MODE_DAIF                     (1UL<<11UL) /**< @brief Enable Inverse Destination address filter          */
/** @} */

/**
    @name XGMAC_IPGCtrlType
    @{
    @brief Select IPG (Inter-Packet Gap) control type

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_IPGCtrlType;
#define XGMAC_IPG_CTRL_DEC_8BITS     (0UL)    /**< @brief When this is set, the minimum IPG is decreased in steps of 8 bits
                                                  from the default 96 bits */
#define XGMAC_IPG_CTRL_INC_32BITS    (1UL)    /**< @brief When this is set, the minimum IPG (96 Bits) is increased in steps of 32 bits */
/** @} */

/**
    @name XGMAC_IPGLengthStepType
    @{
    @brief Select IPG (Inter-Packet Gap) length step
    Actual IPG length is determined based on selected IPG control type
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_IPGLengthStepType;
#define XGMAC_IPG_LENGTH_STEP_0    (0UL)   /**< @brief Default IPG Length 96 bits */
#define XGMAC_IPG_LENGTH_STEP_1    (1UL)   /**< @brief IPG Length 96 bits + 1 * 32 Bits or 96 - 1 * 8 Bits depending on IPG control type */
#define XGMAC_IPG_LENGTH_STEP_2    (2UL)   /**< @brief IPG Length 96 bits + 2 * 32 Bits or 96 - 2 * 8 Bits depending on IPG control type */
#define XGMAC_IPG_LENGTH_STEP_3    (3UL)   /**< @brief IPG Length 96 bits + 3 * 32 Bits or 96 - 3 * 8 Bits depending on IPG control type */
#define XGMAC_IPG_LENGTH_STEP_4    (4UL)   /**< @brief IPG Length 96 bits + 4 * 32 Bits or 96 - 4 * 8 Bits depending on IPG control type */
/** @} */

/**
    @name XGMAC_TxQThrCtrlType
    @{
    @brief Transmit Queue Threshold Control
    Transmission starts when the packet size within the MTL Tx Queue is larger than the threshold.
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_TxQThrCtrlType;
#define XGMAC_TXQ_THR_CTRL_64    (0UL)  /**< @brief Transmit threshold 64 byte */
#define XGMAC_TXQ_THR_CTRL_96    (2UL)  /**< @brief Transmit threshold 96 byte */
#define XGMAC_TXQ_THR_CTRL_128   (3UL)  /**< @brief Transmit threshold 128 byte */
#define XGMAC_TXQ_THR_CTRL_192   (4UL)  /**< @brief Transmit threshold 192 byte */
#define XGMAC_TXQ_THR_CTRL_256   (5UL)  /**< @brief Transmit threshold 256 byte */
#define XGMAC_TXQ_THR_CTRL_384   (6UL)  /**< @brief Transmit threshold 384 byte */
#define XGMAC_TXQ_THR_CTRL_512   (7UL)  /**< @brief Transmit threshold 512 byte */
/** @} */

/**
    @name XGMAC_RxQThrCtrlType
    @{
    @brief Receive Queue Threshold Control
    The received packet is transferred to the application or DMA when the packet size within the
    MTL Rx Queue is larger than the threshold.
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_RxQThrCtrlType;
#define XGMAC_RXQ_THR_CTRL_64    (0UL)  /**< @brief Receive threshold 64 byte */
#define XGMAC_RXQ_THR_CTRL_96    (2UL)  /**< @brief Receive threshold 96 byte */
#define XGMAC_RXQ_THR_CTRL_128   (3UL)  /**< @brief Receive threshold 128 byte */
/** @} */

/**
    @name XGMAC_RxQDmaMapType
    @{
    @brief Receive Queue DMA mapping type
    This will select if RX queue DMA mapping is dynamic or static
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_RxQDmaMapType;
#define XGMAC_RXQ_DMA_MAP_STATIC    (0UL)  /**< @brief Receive Queue and DMA channel mapping is static, DMA channel corressponding RX Queue 0 or RX Queue 1
                                                                                      is selected using QMDMACH field of MTL_RxQ_DMA_Map0 Register  */
#define XGMAC_RXQ_DMA_MAP_DYNAMIC   (1UL)  /**< @brief Receive Queue Enabled for Dynamic DMA Channel Selection. When this is set indicates that each packet
                                                                                     received in Receive Queue  is routed to a DMA channel as decided in the MAC Receiver based on the DMA channel number
                                                                                     programmed in the L3-L4 filter registers, the Ethernet DA address or VLAN filter registers.  */
/** @} */

/**
    @name XGMAC_RxQDmaChanType
    @{
    @brief Receive Queue DMA channel
    Receive Queue DMA channel selection.
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_RxQDmaChanType;
#define XGMAC_RXQ_DMA_CHAN_0    (0UL)  /**< @brief Receive Queue mapped to DMA channel 0  */
#define XGMAC_RXQ_DMA_CHAN_1    (1UL)  /**< @brief Receive Queue mapped to DMA channel 1  */
/** @} */


/**
    @name XGMAC_TxQMapTrafficClassType
    @{
    @brief Transmit queue to traffic class mapping
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_TxQMapTrafficClassType;
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC0    (0UL)    /**< @brief Map TXQ to traffic class TC0 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC1    (1UL)    /**< @brief Map TXQ to traffic class TC1 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC2    (2UL)    /**< @brief Map TXQ to traffic class TC2 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC3    (3UL)    /**< @brief Map TXQ to traffic class TC3 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC4    (4UL)    /**< @brief Map TXQ to traffic class TC4 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC5    (5UL)    /**< @brief Map TXQ to traffic class TC5 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC6    (6UL)    /**< @brief Map TXQ to traffic class TC6 */
#define XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC7    (7UL)    /**< @brief Map TXQ to traffic class TC7 */
/** @} */

/**
    @name XGMAC_VlanTagType
    @{
    @brief Enable Vlan Tag Comparion type
    @trace #BRCM_SWREQ_XGMAC_DRIVER_VLAN_CONFIG
*/
typedef uint32_t XGMAC_VlanTagType;
#define XGMAC_VLAN_TAG_16BIT               (1UL<<0UL)  /**< @brief Enable 16-Bit default Vlan Tag Comparision */
#define XGMAC_VLAN_TAG_12BIT               (1UL<<1UL)  /**< @brief Enable 12-Bit Vlan Tag Comparision         */
#define XGMAC_VLAN_TAG_DIS_VLAN_TYPE_CHECK (1UL<<2UL)  /**< @brief Disable Vlan Type Check                    */
#define XGMAC_VLAN_TAG_EN_REC_SVLAN        (1UL<<3UL)  /**< @brief Enable Receive S-VLAN Match                */
#define XGMAC_VLAN_TAG_EN_SVLAN            (1UL<<4UL)  /**< @brief Enable S-VLAN detection for transmitter and receiver as valid VLAN tagged
                                                                   packets */

/** @} */

/**
    @name XGMAC_JumboPktSupportType
    @{
    @brief Select Jumbo packet support enable

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_JumboPktSupportType;
#define XGMAC_JUMBO_PKT_SUPPORT_DISABLE     (0UL) /**< @brief Jumbo packet is disabled */
#define XGMAC_JUMBO_PKT_SUPPORT_ENABLE      (1UL) /**< @brief Jumbo packet is enabled  */
/** @} */

/**
    @name XGMAC_DMAChannelStatusType
    @{
    @brief DMA channel status

    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef uint32_t XGMAC_DMAChannelStatusType;
#define XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_INTR                (1UL<<0UL)    /**< @brief Transmit interrupt is set when the transfer of packet to the TxQ is completed.    */
#define XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_PROCESS_STOPPED     (1UL<<1UL)    /**< @brief This bit is set when transmit DMA process has entered STOP state.                 */
#define XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_BUFFER_UNAVAILABLE  (1UL<<2UL)    /**< @brief Transmit buffer unavailable and DMA is in suspended state                         */
#define XGMAC_DMA_CHANNEL_STATUS_RECEIVE_INTR                 (1UL<<6UL)    /**< @brief Receive interrupt indicates that transfer of Rx packet to host memory is complete.*/
#define XGMAC_DMA_CHANNEL_STATUS_RECEIVE_BUFFER_UNAVAILABLE   (1UL<<7UL)    /**< @brief Receive buffer unavailable and DMA is in suspended state                          */
#define XGMAC_DMA_CHANNEL_STATUS_RECEIVE_PROCESS_STOPPED      (1UL<<8UL)    /**< @brief This bit is set when receive DMA process has entered STOP state.                  */
#define XGMAC_DMA_CHANNEL_STATUS_DDE_ERROR                    (1UL<<9UL)    /**< @brief This bit is set when descriptor definition error occured.                         */
#define XGMAC_DMA_CHANNEL_STATUS_FATAL_BUS_ERROR              (1UL<<12UL)   /**< @brief This bit is set when BUS error occured.                                           */
#define XGMAC_DMA_CHANNEL_STATUS_CTXT_DES_ERROR               (1UL<<13UL)   /**< @brief Context descriptor error   */
#define XGMAC_DMA_CHANNEL_STATUS_ABNORMAL_INTR_SUMMARY        (1UL<<14UL)   /**< @brief Abnormal interrupt summary */
#define XGMAC_DMA_CHANNEL_STATUS_NORMAL_INTR_SUMMARY          (1UL<<15UL)   /**< @brief Normal interrupt summary   */
#define XGMAC_DMA_CHANNEL_STATUS_TX_DMA_BUS_ERROR             (0x7UL<<16UL) /**< @brief Transmit DMA bus error     */
#define XGMAC_DMA_CHANNEL_STATUS_RX_DMA_BUS_ERROR             (0x7UL<<19UL) /**< @brief Receive DMA bus error      */
#define XGMAC_DMA_CHANNEL_STATUS_MTL_RXQ_FULL_INTR            (1UL<<20UL)   /**< @brief MTL RX Queue full interrupt status   */
#define XGMAC_DMA_CHANNEL_STATUS_MTL_TXQ_UNDER_FLOW_INTR      (1UL<<21UL)   /**< @brief MTL TX Queue under flow interrupt status */
/** @} */

/**
    @name XGMAC_DMAQModeType
    @{
    @brief DMA Queue mode type.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_DMAQModeType;
#define XGMAC_DMA_QMODE_DISABLE (0x00UL) /**< @brief DMA Queue is in disabled state            */
#define XGMAC_DMA_QMODE_AVB     (0x01UL) /**< @brief DMA Queue is in audio video bridging mode, reserved for future use. */
#define XGMAC_DMA_QMODE_ENABLE  (0x2UL)  /**< @brief DMA Queue is in Normal enabled state      */
/** @} */

/**
    @name XGMAC_MtlTcTsaAlgoType
    @{
    @brief Transmission selection algorithm for traffic class related to transmit queue.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_MtlTcTsaAlgoType;
#define XGMAC_MTL_TC_TSA_ALGO_SP   (0x00UL) /**< @brief Select transmit queue priority as strict priority, TC0 has highest priority and TC7 lowest. This is the default priority.    */
#define XGMAC_MTL_TC_TSA_ALGO_CBS  (0x01UL) /**< @brief Select CBS (credit based shaper algorithm) for AV (Audio/video bridging) application (Reserved for future use). */
#define XGMAC_MTL_TC_TSA_ALGO_ETS  (0x2UL)  /**< @brief Select ETS transmission selection algorithm for DCB (Data Centre bridging) application. */
/** @} */

/**
    @name XGMAC_MtlTcEtsAlgoType
    @{
    @brief ETS algorithm to be applied for traffic classes whose transmission selection algorithm (TSA) is set to ETS.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_MtlTcEtsAlgoType;
#define XGMAC_MTL_TC_ETS_ALGO_WRR   (0x00UL) /**< @brief Select Weighted round robin (WRR) algorithm.    */
#define XGMAC_MTL_TC_ETS_ALGO_WFQ   (0x01UL) /**< @brief Select Weighted Fair Queuing (WFQ) algorithm. */
#define XGMAC_MTL_TC_ETS_ALGO_DWRR  (0x2UL)  /**< @brief Deficit Weighted Round Robin (DWRR) algorithm.  */
/** @} */

/**
    @name XGMAC_MTLRxQArbitrationType
    @{
    @brief MTL Receive Queue packet arbitration control.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_MTLRxQArbitrationType;
#define XGMAC_MTL_RXQ_ARBITRATION_PBL (0x00UL) /**< @brief MTL enables the PBL based arbitration.  */
#define XGMAC_MTL_RXQ_ARBITRATION_PKT (0x01UL) /**< @brief MTL enables the PKT based arbitration. */
/** @} */

/**
    @name XGMAC_MTLRxQArbitrationAlgoType
    @{
    @brief MTL Receive Queue arbitration Algorithm.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef uint32_t XGMAC_MTLRxQArbitrationAlgoType;
#define XGMAC_MTL_RXQ_ARBITRATION_ALGO_SP  (0x00UL) /**< @brief Receive arbitration algorithm selected as strict priority.  */
#define XGMAC_MTL_RXQ_ARBITRATION_ALGO_WSP (0x01UL) /**< @brief Receive arbitration algorithm algorithm as weighted strict priority. RXQ_WEGT weight assigned to RXQ. */
/** @} */

/**
    @name XGMAC_MACAddrType
    @{
    @brief Select MAC address type as source address or destination address.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint8_t XGMAC_MACAddrType;
#define XGMAC_MAC_ADDR_SA   (0U)    /**< @brief MAC address is source address      */
#define XGMAC_MAC_ADDR_DA   (1U)    /**< @brief MAC address is destination address */
/** @} */

/**
    @name XGMAC_DriverStatusType
    @{
    @brief XGMAC Driver status.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_DriverStatusType;
#define XGMAC_DRIVER_STATUS_CORE_INITIALIZED     (1UL<<0UL)  /**< @brief XGMAC core initialized    */
#define XGMAC_DRIVER_STATUS_MTL_TXQ_INITIALIZED  (1UL<<1UL)  /**< @brief XGMAC MTL TXQ initialized */
#define XGMAC_DRIVER_STATUS_MTL_RXQ_INITIALIZED  (1UL<<2UL)  /**< @brief XGMAC MTL RXQ initialized */
#define XGMAC_DRIVER_STATUS_DMA_INITIALIZED      (1UL<<3UL)  /**< @brief XGMAC DMA initialized     */
#define XGMAC_DRIVER_STATUS_PTP_INITIALIZED      (1UL<<4UL)  /**< @brief XGMAC PTP initialized     */
/** @} */

/**
    @name XGMAC_LinkStatusType
    @{
    @brief XGMAC Link status.

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_LinkStatusType;
#define XGMAC_LINK_STATUS_OK            (0UL)  /**< @brief Link OK on the XGMII receive interface.          */
#define XGMAC_LINK_STATUS_LOCAL_FAULT   (2UL)  /**< @brief Local Link fault on the XGMII receive interface. */
#define XGMAC_LINK_STATUS_REMOTE_FAULT  (3UL)  /**< @brief Remote Link fault on the XGMII receive interface.*/
/** @} */


/**
    @name XGMAC_DMABurstLengthType
    @{
    @brief XGMAC Select Burst Length in byte

    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef uint32_t XGMAC_DMABurstLengthType;
#define XGMAC_DMA_BURST_LENGTH_UBL     (1UL<<0UL)  /**< @brief XGMAC DMA Burst Length undefined */
#define XGMAC_DMA_BURST_LENGTH_4       (1UL<<1UL)  /**< @brief XGMAC DMA Burst Length 4         */
#define XGMAC_DMA_BURST_LENGTH_8       (1UL<<2UL)  /**< @brief XGMAC DMA Burst Length 8         */
#define XGMAC_DMA_BURST_LENGTH_16      (1UL<<3UL)  /**< @brief XGMAC DMA Burst Length 16        */
#define XGMAC_DMA_BURST_LENGTH_32      (1UL<<4UL)  /**< @brief XGMAC DMA Burst Length 32        */
#define XGMAC_DMA_BURST_LENGTH_64      (1UL<<5UL)  /**< @brief XGMAC DMA Burst Length 64        */
#define XGMAC_DMA_BURST_LENGTH_128     (1UL<<6UL)  /**< @brief XGMAC DMA Burst Length 128       */
#define XGMAC_DMA_BURST_LENGTH_256     (1UL<<7UL)  /**< @brief XGMAC DMA Burst Length 256       */
/** @} */

/**
    @name XGMAC_CoreInterruptType
    @{
    @brief Select XGMAC core Interrupt type

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_CoreInterruptType;         /**< XGMAC core interrupt                          */
#define XGMAC_CORE_INTERRUPT_LSIE    (1UL<<0UL)   /**< @brief Link Status Change Interrupt Enable.   */
#define XGMAC_CORE_INTERRUPT_PMTIE   (1UL<<1UL)   /**< @brief PMT Interrupt Enable.                  */
#define XGMAC_CORE_INTERRUPT_LPIE    (1UL<<2UL)   /**< @brief LPI Interrupt Enable.                  */
#define XGMAC_CORE_INTERRUPT_TSIE    (1UL<<3UL)   /**< @brief Timestamp Interrupt Enable.            */
#define XGMAC_CORE_INTERRUPT_TXESIE  (1UL<<4UL)   /**< @brief Transmit Error Status Interrupt Enable.*/
#define XGMAC_CORE_INTERRUPT_RXESIE  (1UL<<5UL)   /**< @brief Receive Error Status Interrupt Enable. */
#define XGMAC_CORE_INTERRUPT_ALL     (0x3FUL)     /**< @brief Select all the interrupt.              */
/** @} */

/**
    @name XGMAC_CoreInterruptStatusType
    @{
    @brief Select XGMAC core Interrupt status type

    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef uint32_t XGMAC_CoreInterruptStatusType;         /**< XGMAC core interrupt status                              */
#define XGMAC_CORE_INTERRUPT_STATUS_LSIS    (1UL<<0UL)  /**< @brief Link Status Change Interrupt status.              */
#define XGMAC_CORE_INTERRUPT_STATUS_SMIS    (1UL<<1UL)  /**< @brief SMA (mdio station management) interrupt           */
#define XGMAC_CORE_INTERRUPT_STATUS_PMTIS   (1UL<<2UL)  /**< @brief PMT Interrupt status.                             */
#define XGMAC_CORE_INTERRUPT_STATUS_LPIIS   (1UL<<3UL)  /**< @brief LPI Interrupt status.                             */
#define XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS (1UL<<4UL)  /**< @brief MMC Receive Interrupt status.                     */
#define XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS (1UL<<5UL)  /**< @brief MMC Transmit Interrupt status.                    */


#define XGMAC_CORE_INTERRUPT_STATUS_TSIS    (1UL<<6UL)  /**< @brief Timestamp Interrupt status.                       */
#define XGMAC_CORE_INTERRUPT_STATUS_TXESIS  (1UL<<7UL)  /**< @brief Transmit Error Status Interrupt status.           */
#define XGMAC_CORE_INTERRUPT_STATUS_RXESIS  (1UL<<8UL)  /**< @brief Receive Error Status Interrupt status.            */
#define XGMAC_CORE_INTERRUPT_STATUS_GPIIS   (1UL<<9UL)  /**< @brief GPI Interrupt status when MAC GPIO status changes.*/
#define XGMAC_CORE_INTERRUPT_STATUS_ALL     (0x3FFUL)   /**< @brief Select all the interrupt.                         */
/** @} */


/**
    @name XGMAC_DMAChannelInterruptType
    @{
    @brief Select XGMAC DMA channel Interrupt type

    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef uint32_t XGMAC_DMAChannelInterruptType;
#define XGMAC_DMA_CHANNEL_INTERRUPT_GRP0  (0xFUL)      /**< @brief DMA Channel interrupt Group0.                     */
#define XGMAC_DMA_CHANNEL_INTERRUPT_TIE   (1UL<<0UL)   /**< @brief DMA Transmit Interrupt enable.                    */
#define XGMAC_DMA_CHANNEL_INTERRUPT_TXSE  (1UL<<1UL)   /**< @brief DMA Transmit Stopped Interrupt enable.            */
#define XGMAC_DMA_CHANNEL_INTERRUPT_TBUE  (1UL<<2UL)   /**< @brief DMA Transmit Buffer Unavailable Interrupt enable. */
#define XGMAC_DMA_CHANNEL_INTERRUPT_RIE   (1UL<<3UL)   /**< @brief DMA Receive Interrupt enable.                     */
#define XGMAC_DMA_CHANNEL_INTERRUPT_GRP1  (0xF0UL)     /**< @brief DMA Channel interrupt Group1.                     */
#define XGMAC_DMA_CHANNEL_INTERRUPT_RBUE  (1UL<<4UL)   /**< @brief DMA Receive Buffer Unavailable  Interrupt enable. */
#define XGMAC_DMA_CHANNEL_INTERRUPT_RSE   (1UL<<5UL)   /**< @brief DMA Receive Stopped Interrupt enable.             */
#define XGMAC_DMA_CHANNEL_INTERRUPT_DDE   (1UL<<6UL)   /**< @brief DMA Descriptor Definition Error Interrupt enable. */
#define XGMAC_DMA_CHANNEL_INTERRUPT_FBE   (1UL<<7UL)   /**< @brief DMA Fatal Bus Error Interrupt enable.             */
#define XGMAC_DMA_CHANNEL_INTERRUPT_GRP2  (0xF00UL)    /**< @brief DMA Channel interrupt Group2.                     */
#define XGMAC_DMA_CHANNEL_INTERRUPT_CDE   (1UL<<8UL)   /**< @brief DMA Context Descriptor Error Interrupt enable.    */
#define XGMAC_DMA_CHANNEL_INTERRUPT_AIE   (1UL<<9UL)   /**< @brief DMA Abnormal Interrupt Summary Enable             */
#define XGMAC_DMA_CHANNEL_INTERRUPT_NIE   (1UL<<10UL)  /**< @brief DMA Normal Interrupt Summary Enable               */
#define XGMAC_DMA_CHANNEL_INTERRUPT_ALL   (0x7FFUL)    /**< @brief Select all the DMA interrupt                      */

/** @} */

/**
    @name XGMAC_EthLinkInfoType
    @{
    @brief Ethernet link information structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INFO
*/
typedef struct sXGMAC_EthLinkInfoType {
    XGMAC_EthSpeedModeType speed;      /**< @brief Link speed: 0= 10 MBit, 1= 100 MBit, 2= 1 GBit */
    XGMAC_EthDuplexModeType duplex;    /**< @brief Duplex mode: 0= Half, 1= Full                  */
} XGMAC_EthLinkInfoType;
/** @} */

/**
   @name XGMAC_EthMacAddrType
    @{
    @brief Ethernet MAC Address
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef struct sXGMAC_EthMacAddrType {
    XGMAC_MACAddrType macAddrType;    /**< @brief Select MAC address type as SA or DA to compare with received packet used in #XGMAC_SetAddressFilter API */
    uint8_t macAddr[6U];              /**< @brief MAC Address (6 bytes), MSB first    */
} XGMAC_EthMacAddrType;
/** @} */

/**
   @name XGMAC_EthPktFilterType
    @{
    @brief XGMAC enable packet filter type
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef struct sXGMAC_EthPktFilterType {
    XGMAC_PktFilterModeType pktFilter;   /**< @brief Select packet filter type to be enabled */
} XGMAC_EthPktFilterType;
/** @} */

/**
   @name XGMAC_FlowCtrlCfgType
    @{
    @brief XGMAC flow control configuration
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef struct sXGMAC_FlowCtrlCfgType {
    uint32_t txQIndex;                 /**< @brief Tx flow control Queue index from 0-7 */
    uint32_t pauseTime;                /**< @brief Select pause time                    */
    uint8_t disableZeroQuantaPause;    /**< @brief Disable Zero-Quanta Pause            */
    uint32_t pauseLowThr;               /**< @brief Set Pause low threshold             */
    uint8_t txFE;                      /**< @brief Transmit flow enable                 */
    uint8_t rxFE;                      /**< @brief Receive flow enable                  */
    uint8_t flowCtrlBusy;              /**< @brief Select flow control busy             */
    uint8_t unicastPausePktDet;        /**< @brief Unicast pause packet detect          */
    uint8_t priorityFlowCtrlEn;        /**< @brief Priority based flow control enable   */
} XGMAC_FlowCtrlCfgType;
/** @} */

/**
   @name XGMAC_EthConfigType
    @{
    @brief XGMAC configuration structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef struct sXGMAC_EthConfigType {
    XGMAC_EthMacAddrType macAddr;         /**< @brief Set MAC address                                         */
    XGMAC_EthSpeedModeType speed;         /**< @brief Link speed: 10 MBit, 100 MBit, 2.5G, 1 GBit             */
    XGMAC_EthDuplexModeType duplex;       /**< @brief Duplex mode: Half duplex, Full duplex                   */
    XGMAC_IPGCtrlType       IPGCtrl;      /**< @brief Select IPG control type                                 */
    XGMAC_IPGLengthStepType IPGLenStep;   /**< @brief Select IPG length step                                  */
    XGMAC_JumboPktSupportType jumboPktEn; /**< @brief Select Jumbo packet Disable = 0, Enable = 1             */
    uint8_t watchdogDisable;              /**< @brief MAC disables the watchdog timer on the receiver         */
    uint8_t autoCRCstripping;             /**< @brief Automatic CRC striping enable                           */
    uint8_t disableCRCChk;                /**< @brief Disable CRC checking for received packets               */
    uint8_t checksumOffload;              /**< @brief When set this enables the IPV4 header checksum checking */
    uint8_t arpEnable;                    /**< @brief ARP offload enable                                      */
    uint8_t rcwEn;                        /**< @brief Register clear on write enable otherwise registers defined under this block will read on clear
                                                                                            e.g. interrupt status registers.  */
} XGMAC_EthConfigType;
/** @} */


/**
   @name XGMAC_MtlTxQConfigType
    @{
    @brief XGMAC MTL TXQ configuration structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef struct sXGMAC_MtlTxQConfigType {
    uint8_t flushTxQ;                      /**< @brief Flush Transmit Queue.                    */
    uint8_t txQStoreForward;               /**< @brief Transmit Store and Forward.              */
    uint8_t txQUnderflowIntrEn;            /**< @brief Enable txQ underflow interrupt enable */
    XGMAC_DMAQModeType txQEnable;          /**< @brief Transmit Queue enable options.           */
    XGMAC_TxQThrCtrlType txQThrCtrl;       /**< @brief Transmit Threshold Control.              */
    XGMAC_TxQMapTrafficClassType txQMapTC; /**< @brief Transmit Queue to Traffic Class Mapping. */
    XGMAC_MtlTcTsaAlgoType tsaAlgo;        /**< @brief Transmit algorithm (TSA) selection.  */
    XGMAC_MtlTcEtsAlgoType etsAlgo;        /**< @brief When transmit algorithm (TSA) is selected as ETS, select the ets algorithm type. */
    uint32_t mtlTcQuantWeight;             /**< @brief Assign quantum weight to traffic class. */
    uint32_t psTC;                         /**< @brief Assign priority mapped to traffic class.  */
    uint32_t txQSize;                      /**< @brief Transmit Queue Size. The size of the allocated transmit queues in blocks of 256 bytes, the valid range is 0-127 */
} XGMAC_MtlTxQConfigType;
/** @} */

/**
   @name XGMAC_MtlRxQConfigType
    @{
    @brief XGMAC MTL RXQ configuration structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
typedef struct sXGMAC_MtlRxQConfigType {
    XGMAC_RxQThrCtrlType rxQThrCtrl; /**< @brief Receive Queue Threshold Control.                  */
    XGMAC_RxQDmaMapType  rxQDmaMap;  /**< @brief Rx Queue DMA mapping type. */
    XGMAC_RxQDmaChanType rxQDmaChan; /**< @brief Rx Queue DMA channel number. */
    XGMAC_MTLRxQArbitrationType rxArbType;     /**< @brief Select MTL Receive Queue arbitration type as PBL based or packet based. */
    XGMAC_MTLRxQArbitrationAlgoType rxArbAlgo; /**< @brief Select MTL Receive Queue arbitration algorithm. */
    uint32_t rxQWegt;                /**< Receive Queue Weight.  */
    uint32_t rxQsize;                /**< @brief Receive Queue Size.                               */
    uint32_t rxQThrFlowDisable;      /**< @brief Threshold for deactivating flow control.          */
    uint32_t rxQThrFlowEnable;       /**< @brief Threshold for Activating flow control.            */
    uint8_t rxQFullIntrEnable;       /**< @brief RXQ full interrupt enable.            */
    uint8_t fwdUndersizedPkts;       /**< @brief Forward Undersized Good Packets.                  */
    uint8_t fwdErrorPkts;            /**< @brief Forward Error Packets.                            */
    uint8_t rxQStoreForward;         /**< @brief Receive Queue Store and forward.                  */
    uint8_t disDropTcpCsErrorPkts;   /**< @brief Disable Dropping of TCP/IP Checksum Error Packets */
    uint8_t enableHwFlowCtrl;        /**< @brief Enable hardware flow control.                     */
} XGMAC_MtlRxQConfigType;
/** @} */

/**
   @name XGMAC_DMACfgType
    @{
    @brief XGMAC DMA configuration structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef struct sXGMAC_DMACfgType {
    XGMAC_DMABurstLengthType burstLen;    /**< @brief If fixed burst length is enabled set fixed burst length */
    uint8_t EAME;           /**< @brief Enhanced Address Mode Enable */
    uint8_t AALE;                 /**< @brief Automatic AXI LPI enable     */
} XGMAC_DMACfgType;
/** @} */

/**
   @name sXGMAC_DMAChannelCfgType
    @{
    @brief XGMAC DMA channel configuration structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef struct sXGMAC_DMAChannelCfgType {
    uint32_t DSL;     /**< @brief Descriptor Skip Length. This bit specifies number of Word, DWord, or LWord depending on
                                               the 32 bit, 64 bit, or 128 bit bus to skip between two descriptors */
    uint8_t PBLx8;    /**< @brief 8xPBL mode when this bit is set PBL value programmed in Tx/RX PBL field in channel
                                              configuration is multiplied by 8 times. */
    uint8_t SPH;     /**< @brief Header-Payload Split When this bit is set the DMA splits the header and payload in receive path */
} XGMAC_DMAChannelCfgType;
/** @} */

/**
   @name XGMAC_DMAChannelTxCtrlType
    @{
    @brief XGMAC DMA transmit channel control structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef struct sXGMAC_DMAChannelTxCtrlType {
    uint32_t txPBL;    /**< @brief Transmit Programmable Burst Length, PBL value is programmed with any of the following
                                   values 1, 2, 4, 8, 16, 32.*/
    uint8_t EDSE;      /**< @brief Enhanced Descriptor Enable */
    uint8_t TSE;       /**< @brief TCP Segmentation Enabled */
    uint8_t OSP;       /**< @brief Operate on Second Packet. When this bit is set, it instructs the DMA to process the second packet of the Transmit
                                   data even before closing the descriptor of the first packet */
} XGMAC_DMAChannelTxCtrlType;
/** @} */

/**
   @name XGMAC_DMAChannelRxCtrlType
    @{
    @brief XGMAC DMA receive channel control structure
    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
typedef struct sXGMAC_DMAChannelRxCtrlType {
    uint32_t rxPBL;         /**< @brief Receive Programmable Burst Length, PBL, value is programmed with any of of the following
                                        values 1, 2, 4, 8, 16, 32.*/
    uint32_t recBuffSize;   /**< @brief Receive Buffer Size  */
    uint8_t rxDMAFlush;     /**< @brief When this bit is set to 1 XGMAC automaticaly flushes the packet from Rx Queues destined to this DMA RX
                                        channel when it is stopped or suspended due to unavailability of descriptors. */
} XGMAC_DMAChannelRxCtrlType;
/** @} */

/**
   @name XGMAC_MIBCounterType
    @{
    @brief XGMAC MIB Counter Statistics.
    @trace #BRCM_SWREQ_XGMAC_MIB_COUNTER_STATUS
*/
typedef struct sXGMAC_MIBCounterType {
    uint32_t RXOCTGLO;           /**< @brief Rx Good Octet Count Low                                */
    uint32_t RXOCTGHI;           /**< @brief Rx Good Octet Count High                               */
    uint32_t RXBCASTGLO;         /**< @brief Rx Good Broadcast Packet Count Low                     */
    uint32_t RXMCASTGLO;         /**< @brief Rx Good Multicast Packet Count Low                     */
    uint32_t RXCRCERLO;          /**< @brief Rx CRC Error Packet Count Low                          */
    uint32_t RX64OCTGBLO;        /**< @brief Rx Good Bad 64 Octet Size Packet Count Low             */
    uint32_t RX65TO127OCTGBLO;   /**< @brief Rx Good Bad 65-to-127 Octet Size Packet Count Low      */
    uint32_t RX128TO255OCTGBLO;  /**< @brief Rx Good Bad 128-to-255 Octet Size Packet Count Low     */
    uint32_t RX256TO511OCTGBLO;  /**< @brief Rx Good Bad 256-to-511 Octet Size Packet Count Low     */
    uint32_t RX512TO1023OCTGBLO; /**< @brief Rx Good Bad 512-to-1023 Octet Size Packet Count Low    */
    uint32_t RXUCASTGLO;         /**< @brief Rx Good Unicast Packet Count Low                       */
    uint32_t RXPAUSELO;          /**< @brief Rx Pause Packet Count Low                              */
    uint32_t RXVLANGBLO;         /**< @brief Rx Good Bad VLAN Packet Count Low                      */
    uint32_t TXPKTGBLO;          /**< @brief Tx Good Bad Packet Count Low                           */
    uint32_t TXPKTGLO;           /**< @brief Tx Good Packet Count Low                               */
    uint32_t TXBCASTGLO;         /**< @brief Tx Good Broadcast Packet Count Low                     */
    uint32_t TXMCASTGLO;         /**< @brief Tx Good Multicast Packet Count Low                     */
    uint32_t TX64OCTGBLO;        /**< @brief Tx Good Bad 64 Octet Size Packet Count Low             */
    uint32_t TX65TO127OCTGBLO;   /**< @brief Tx Good Bad 65-to-127 Octet Size Packet Count Low      */
    uint32_t TX128TO255OCTGBLO;  /**< @brief Tx Good Bad 128-to-255 Octet Size Packet Count Low     */
    uint32_t TX512TO1023OCTGBLO; /**< @brief Tx Good Bad 512-to-1023 Octet Size Packet Count Low    */
    uint32_t TX1024TOMAXOCTGBLO; /**< @brief Tx Good Bad 1024-tomaxsize Octet Size Packet Count Low */
    uint32_t TXOCTGLO;           /**< @brief Tx Good Octet Count Low                                */
    uint32_t TXOCTGHI;           /**< @brief Tx Good Octet Count High                               */
    uint32_t TXPAUSEGLO;         /**< @brief Tx Pause Packet Count Low                              */
} XGMAC_MIBCounterType;
/** @} */

/* XGMAC Core APIs */

/** @brief Initialize XGMAC peripheral.

    This API initializes and configures XGMAC with initial configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the XGMAC
    @param[in]      aConfig     Pointer to XGMAC driver configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_DrvInit(XGMAC_HwIDType aId, XGMAC_EthConfigType *aConfig);

/** @brief De-Initialize XGMAC peripheral.

    This API De-initializes XGMAC in POR state and free any resources allocated.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the XGMAC

    Return values are documented in reverse-chronological order.
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful.
    @retval     #BCM_ERR_UNINIT Device or resource is not un-initialized.
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param.

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_DrvDeInit(XGMAC_HwIDType aId);

/** @brief Configure speed mode

    This API configures XGMAC in specified speed mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID        Index of the XGMAC
    @param[in]      aSpeed     Speed mode Selection

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_SpeedModeSet(XGMAC_HwIDType aId, XGMAC_EthSpeedModeType aSpeed);

/** @brief Set MAC address

    This API sets the MAC address in register MAC_Address0_High and MAC_Address0_Low.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID        Index of the XGMAC
    @param[in]      pMacAddr   Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_SetMacAddress(XGMAC_HwIDType aId, const XGMAC_EthMacAddrType *pMacAddr);

/** @brief XGMAC packet filter enable/disable

    XGMAC packet filter enable/disable for the selected filter type.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      pktFilterType  Pointer to the filter type
    @param[in]      aEnable        Enable the selected filter type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_MacPacketFilterEnable(XGMAC_HwIDType aId, XGMAC_EthPktFilterType *pktFilterType, int aEnable);

/** @brief XGMAC Set address filtering

    XGMAC configures perfect match address filters for specified number of MAC addresses.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      pMacAddr    Pointer to the address filter
    @param[in]      numAddr     Number of addresses to be configured

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_SetAddressFilter(XGMAC_HwIDType aId, const XGMAC_EthMacAddrType *pMacAddr, uint32_t numAddr);

/** @brief XGMAC set flow control

    This API configures XGMAC flow control settings.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      flowCtrl       Pointer to the flow control structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_SetFlowCtrl(XGMAC_HwIDType aId, XGMAC_FlowCtrlCfgType *flowCtrl);

/** @brief XGMAC MAC enable/disable

    This API enables/disables the MAC block to transmit and receive packets. This API
    is called when MAC configuration is completed.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      aEnable        Enable/disable the MAC block

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_MacEnable(XGMAC_HwIDType aId, int aEnable);

/** @brief XGMAC Looback set

    This API sets internal loopback.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID      Index of the XGMAC
    @param[in]      lb       lb mode 0: Normal Mode 1: Loopback mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_SetMacLoopback(XGMAC_HwIDType aId, uint32_t lb);

/* MTL configuration APIs */

/** @brief XGMAC Set MTL TX Queue configuration

    This API configures XGMAC TX Queue settings.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID          Index of the XGMAC
    @param[in]      mtlCfg       Pointer to the MTL TX Queue Fifo config structure
    @param[in]      txQIndex     MTL TX Queue index number range [0-7]

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_SetMTLTxQConfiguration(XGMAC_HwIDType aId, XGMAC_MtlTxQConfigType *mtlCfg, uint8_t txQIndex);

/** @brief XGMAC Set MTL RX Queue configuration

    This API configures XGMAC RX Queue settings.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID          Index of the XGMAC
    @param[in]      mtlCfg       Pointer to the MTL RX Queue Fifo config structure
    @param[in]      rxQIndex     MTL RX Queue index number range [0-7]

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_SetMTLRxQConfiguration(XGMAC_HwIDType aId, XGMAC_MtlRxQConfigType *mtlCfg, uint8_t rxQIndex);

/** @brief XGMAC flush the TX Queue FIFO

    This API flushes XGMAC TX Queue FIFO

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID          Index of the XGMAC
    @param[in]      txQIndex     MTL TX Queue index number range [0-7]

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_FlushTxFifo(XGMAC_HwIDType aId, uint8_t txQIndex);

/* DMA configuration APIs */
/** @brief XGMAC DMA mode SW reset

    This API resets internal DMA, MTL and MAC core logic.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_DMAModeSwReset(XGMAC_HwIDType aId);

/** @brief Initializes XGMAC DMA mode transfer.

    This API initializes and configures XGMAC in DMA mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID        Index of the XGMAC
    @param[in]      dmaCfg     Pointer to DMA configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAInit(XGMAC_HwIDType aId, XGMAC_DMACfgType *dmaCfg);

/** @brief Sets DMA Queue configuration type

    This API selects MTL TX Queue for DMA transfer in specified mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID       Index of the XGMAC
    @param[in]      qMode     TX Queue in selected mode AVB or Normal.
    @param[in]      txQIndex     MTL TX Queue index number range [0-7]

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG

    @limitations None
*/
int XGMAC_SetDMAQMode(XGMAC_HwIDType aId, XGMAC_DMAQModeType qMode, uint8_t txQIndex);

/** @brief DMA initialize channel

    This API initializes DMA channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Index of the XGMAC
    @param[in]      chanCfg      Pointer
    @param[in]      chan           DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAInitChannel(XGMAC_HwIDType aId, XGMAC_DMAChannelCfgType *chanCfg, uint32_t chan);

/** @brief DMA Initialize transmit channel

    This API initializes DMA transmit channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Index of the XGMAC
    @param[in]      chanTxCtrl   Pointer to DMA transmit channel control
    @param[in]      chan            DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAInitTxChan(XGMAC_HwIDType aId, XGMAC_DMAChannelTxCtrlType *chanTxCtrl, uint32_t chan);

/** @brief DMA Initialize receive channel

    This API initializes DMA receive channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Index of the XGMAC
    @param[in]      chanRxCtrl   Pointer to DMA transmit channel control
    @param[in]      chan            DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAInitRxChan(XGMAC_HwIDType aId, XGMAC_DMAChannelRxCtrlType *chanRxCtrl, uint32_t chan);

/** @brief Sets DMA TX fifo ring length

    This API configures the DMA TX fifo ring length.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID    Index of the XGMAC
    @param[in]      len    Set TX fifo ring length
    @param[in]      chan   DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMASetTxRingLen(XGMAC_HwIDType aId, uint32_t len, uint32_t chan);

/** @brief Sets DMA RX fifo ring length

    This API configures the DMA RX fifo ring length.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID    Index of the XGMAC
    @param[in]      len    Set RX fifo ring length
    @param[in]      chan   DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMASetRxRingLen(XGMAC_HwIDType aId, uint32_t len, uint32_t chan);

/** @brief Sets DMA RX descriptor tail pointer

    This API configures the receive descriptor tail pointer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID       Index of the XGMAC
    @param[in]      tailPtr  RX descriptor tail pointer
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMASetRxTailPtr(XGMAC_HwIDType aId, uint32_t tailPtr, uint32_t chan);

/** @brief Sets DMA TX descriptor tail pointer

    This API configures the transmit descriptor tail pointer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID       Index of the XGMAC
    @param[in]      tailPtr  TX descriptor tail pointer
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMASetTxTailPtr(XGMAC_HwIDType aId, uint32_t tailPtr, uint32_t chan);

/** @brief Sets DMA RX descriptor Base address pointer

    This API configures RX descriptor base address pointer.

    @behavior Sync, Non-reentrant

    @pre The base address can be changed only when DMA receive (SR) is in stopped state.

    @param[in]      aID                 Index of the XGMAC
    @param[in]      listAddrPtr        RX descriptor 32 bit Base address pointer
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMASetRxDescListAddr(XGMAC_HwIDType aId, uint32_t listAddrPtr, uint32_t chan);

/** @brief Sets DMA TX descriptor Base address pointer

    This API configures TX descriptor base address pointer.

    @behavior Sync, Non-reentrant

    @pre The base address can be changed only when DMA Transmit (ST) is in stopped state.

    @param[in]      aID                 Index of the XGMAC
    @param[in]      listAddrPtr       TX descriptor 32 Bit Base address pointer
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMASetTxDescListAddr(XGMAC_HwIDType aId, uint32_t listAddrPtr, uint32_t chan);

/** @brief Sets DMA Receive Buffer Size

    This API configures receive buffer size in bytes allocated by the software to store the packets for
    the Rx DMA to transfer to the host memory.

    @behavior Sync, Non-reentrant

    @pre The base address can be changed only when DMA receive (SR) is in stopped state.

    @param[in]      aID          Index of the XGMAC
    @param[in]      buffSize    Receive buffer Size in bytes
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations The buffer size must be a multiple of 8 or 16 and maximum buffer size is limited to 16 K bytes.
*/
int XGMAC_DMASetRecBufferSize(XGMAC_HwIDType aId, uint32_t buffSize, uint32_t chan);

/** @brief Enable DMA transmit

    This API starts/stops DMA tansfer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID        Index of the XGMAC
    @param[in]      aEnable    Enable/Disable (1/0) DMA transmit
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMATransmitEnable(XGMAC_HwIDType aId, int aEnable, uint32_t chan);

/** @brief Enable DMA receive

    This API starts/stops DMA receive.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID        Index of the XGMAC
    @param[in]      aEnable    Enable/Disable (1/0) DMA receive
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAReceiveEnable(XGMAC_HwIDType aId, int aEnable, uint32_t chan);

/** @brief Get the DMA channel status

    This API fetches DMA transfer status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the XGMAC
    @param[out]     DMAStatus   Pointer to DMA channel status structure
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAChannelStatusGet(XGMAC_HwIDType aId, XGMAC_DMAChannelStatusType *DMAStatus, uint32_t chan);

/** @brief DMA channel status clear

    This API writes to status bit to clear the status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Index of the XGMAC
    @param[in]      DMAStatusClear  DMA status bit to clear
    @param[in]      chan      DMA channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_DMAChannelStatusClear(XGMAC_HwIDType aId, XGMAC_DMAChannelStatusType DMAStatusClear, uint32_t chan);

/* VLAN APIs */

/** @brief Enable the VLAN Tag

    This API enables the VLAN tag for selected VLAN type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      vlanTag        VLAN Tag type
    @param[in]      aEnable        Enable/Disable (1/0) the VLAN match filter for selected VLAN type
    @param[in]      vlanTagFltrEn  Enable VLAN Tag Filter (VTFE) if this is not enabled VLAN tag
                                   is not checked for any selected VLAN filter match.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_VLAN_CONFIG

    @limitations None
*/
int XGMAC_EnableVlanTag(XGMAC_HwIDType aId, XGMAC_VlanTagType vlanTag, int aEnable, int vlanTagFltrEn);

/** @brief Set the VLAN Tag

    This API configures the VLAN tag.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the XGMAC
    @param[in]      vlanTag       Full VLAN tag [0:15]
    @param[in]      innerVlanTag  Inner VLAN Tag
    @param[in]      innerVlanType If only inner VLAN type need to be configured this is set to 1.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_VLAN_CONFIG

    @limitations None
*/
int XGMAC_SetVlanTag(XGMAC_HwIDType aId, uint16_t vlanTag, uint16_t innerVlanTag, uint32_t innerVlanType);

/** @brief Get the driver status

    This API gives driver status if it is initialized or not for core, MTL, DMA and PTP module.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the XGMAC
    @param[out]    driverStatus    Driver status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_GetDriverStatus(XGMAC_HwIDType aId, XGMAC_DriverStatusType *driverStatus);

/* Interrupt APIs */

/** @brief Enable the MAC core interrupt

    This API enables the interrupt for the selected interrupt types.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      intrType      Interrupt type
    @param[in]      aEnable       Enable or disable the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_EnableCoreInterrupt(XGMAC_HwIDType aId, XGMAC_CoreInterruptType intrType, uint32_t aEnable);

/** @brief Enable the MAC DMA channel interrupt

    This API enables the interrupt for the selected interrupt types.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID                Index of the XGMAC
    @param[in]      chanIntrType  DMA channel Interrupt type
    @param[in]      chan      DMA channel number
    @param[in]      aEnable   Enable or disable the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

    @limitations None
*/
int XGMAC_EnableDMAChannelInterrupt(XGMAC_HwIDType aId, XGMAC_DMAChannelInterruptType chanIntrType, uint32_t chan, uint32_t aEnable);

/** @brief Clear the MAC core interrupt

    This API clears the interrupt for the selected interrupt types.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      intrType      Clear the selected interrupt types

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_ClearCoreInterrupt(XGMAC_HwIDType aId, XGMAC_CoreInterruptStatusType intrType);

/** @brief Get the MAC core interrupt status

    This API gets the interrupt status for the selected interrupt types.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[in]      intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get MAC core interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_GetCoreInterruptStatus(XGMAC_HwIDType aId, XGMAC_CoreInterruptStatusType intrType, uint32_t *intrStatus);

/** @brief Get the MAC Link status

    This API gets the Link status received on the XGMII receive interface.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the XGMAC
    @param[out]    linkStatus   Link status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @limitations None
*/
int XGMAC_GetLinkStatus(XGMAC_HwIDType aId, XGMAC_LinkStatusType *linkStatus);

/** @brief Gets the MIB counter statistics.

    This API gets transmit and receive MIB counter (Management Information Base) statistics for good and bad packets.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId           Index of the XGMAC
    @param[out]    mibStats   MIB counter statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   XGMAC driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_XGMAC_MIB_COUNTER_STATUS

    @limitations None
*/
int XGMAC_GetMIBCounterStats(XGMAC_HwIDType aId, XGMAC_MIBCounterType *mibStats);

#endif /* XGMAC_H */
/** @} */

