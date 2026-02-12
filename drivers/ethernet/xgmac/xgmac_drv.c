/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xgmacdrv_impl XGMAC Driver Design
    @ingroup grp_xgmacdrv

    @addtogroup grp_xgmacdrv_impl
    @{

    @file drivers/ethernet/xgmac/xgmac_drv.c
    @brief XGMAC Driver Design

    @version 0.1 Initial version
*/

#include "rdb/xgmac_core_rdb.h"
#include "rdb/xgmac_mtl_rdb.h"
#include "rdb/xgmac_mtl_tcq0_rdb.h"
#include "rdb/xgmac_mtl_tcq1_rdb.h"
#include "rdb/xgmac_mtl_tcq2_rdb.h"
#include "rdb/xgmac_mtl_tcq3_rdb.h"
#include "rdb/xgmac_mtl_tcq4_rdb.h"
#include "rdb/xgmac_mtl_tcq5_rdb.h"
#include "rdb/xgmac_mtl_tcq6_rdb.h"
#include "rdb/xgmac_mtl_tcq7_rdb.h"
#include "rdb/xgmac_dma_rdb.h"
#include "rdb/xgmac_dma_ch0_rdb.h"
#include "rdb/xgmac_dma_ch1_rdb.h"
#include "rdb/xgmac_dma_ch2_rdb.h"
#include "rdb/xgmac_dma_ch3_rdb.h"
#include "rdb/xgmac_dma_ch4_rdb.h"
#include "rdb/xgmac_dma_ch5_rdb.h"
#include "rdb/xgmac_dma_ch6_rdb.h"
#include "rdb/xgmac_dma_ch7_rdb.h"
#include <xgmac.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>

/**
    @name XGMAC Driver Design IDs
    @{
    @brief Driver Design IDs for XGMAC

*/
#define BRCM_SWDSGN_XGMAC_DRV_INIT_PROC                         (0xB660U) /**< @brief #XGMAC_DrvInit                      */
#define BRCM_SWDSGN_XGMAC_DRV_DEINIT_PROC                       (0xB661U) /**< @brief #XGMAC_DrvDeInit                    */
#define BRCM_SWDSGN_XGMAC_SPEED_MODE_SET_PROC                   (0xB662U) /**< @brief #XGMAC_SpeedModeSet                 */
#define BRCM_SWDSGN_XGMAC_SET_MAC_ADDRESS_PROC                  (0xB663U) /**< @brief #XGMAC_SetMacAddress                */
#define BRCM_SWDSGN_XGMAC_MAC_PACKET_FILTER_ENABLE_PROC         (0xB664U) /**< @brief #XGMAC_MacPacketFilterEnable        */
#define BRCM_SWDSGN_XGMAC_SET_ADDRESS_FILTER_PROC               (0xB665U) /**< @brief #XGMAC_SetAddressFilter             */
#define BRCM_SWDSGN_XGMAC_SET_FLOW_CTRL_PROC                    (0xB666U) /**< @brief #XGMAC_SetFlowCtrl                  */
#define BRCM_SWDSGN_XGMAC_MAC_ENABLE_PROC                       (0xB667U) /**< @brief #XGMAC_MacEnable                    */
#define BRCM_SWDSGN_XGMAC_SET_MAC_LOOPBACK_PROC                 (0xB669U) /**< @brief #XGMAC_SetMacLoopback               */
#define BRCM_SWDSGN_XGMAC_SET_MTL_TXQ_CONFIGURATION_PROC        (0xB66AU) /**< @brief #XGMAC_SetMTLTxQConfiguration       */
#define BRCM_SWDSGN_XGMAC_SET_MTL_RXQ_CONFIGURATION_PROC        (0xB66BU) /**< @brief #XGMAC_SetMTLRxQConfiguration       */
#define BRCM_SWDSGN_XGMAC_FLUSH_TX_FIFO_PROC                    (0xB66CU) /**< @brief #XGMAC_FlushTxFifo                  */
#define BRCM_SWDSGN_XGMAC_DMA_MODE_SW_RESET_PROC                (0xB66DU) /**< @brief #XGMAC_DMAModeSwReset               */
#define BRCM_SWDSGN_XGMAC_DMA_INIT_PROC                         (0xB66EU) /**< @brief #XGMAC_DMAInit                      */
#define BRCM_SWDSGN_XGMAC_SET_DMA_QMODE_PROC                    (0xB66FU) /**< @brief #XGMAC_SetDMAQMode                  */
#define BRCM_SWDSGN_XGMAC_GET_LINK_STATUS_PROC                  (0xB670U) /**< @brief #XGMAC_GetLinkStatus                */
#define BRCM_SWDSGN_XGMAC_DMA_SET_TX_RING_LEN_PROC              (0xB671U) /**< @brief #XGMAC_DMASetTxRingLen              */
#define BRCM_SWDSGN_XGMAC_DMA_SET_RX_RING_LEN_PROC              (0xB672U) /**< @brief #XGMAC_DMASetRxRingLen              */
#define BRCM_SWDSGN_XGMAC_DMA_SET_RX_TAILPTR_PROC               (0xB673U) /**< @brief #XGMAC_DMASetRxTailPtr              */
#define BRCM_SWDSGN_XGMAC_DMA_SET_TX_TAILPTR_PROC               (0xB674U) /**< @brief #XGMAC_DMASetTxTailPtr              */
#define BRCM_SWDSGN_XGMAC_DMA_TRANSMIT_ENABLE_PROC              (0xB675U) /**< @brief #XGMAC_DMATransmitEnable            */
#define BRCM_SWDSGN_XGMAC_DMA_RECEIVE_ENABLE_PROC               (0xB676U) /**< @brief #XGMAC_DMAReceiveEnable             */
#define BRCM_SWDSGN_XGMAC_DMA_CHANNEL_STATUS_GET_PROC           (0xB677U) /**< @brief #XGMAC_DMAChannelStatusGet          */
#define BRCM_SWDSGN_XGMAC_DMA_CHANNEL_STATUS_CLEAR_PROC         (0xB678U) /**< @brief #XGMAC_DMAChannelStatusClear        */
#define BRCM_SWDSGN_XGMAC_ENABLE_VLAN_TAG_PROC                  (0xB679U) /**< @brief #XGMAC_EnableVlanTag                */
#define BRCM_SWDSGN_XGMAC_SET_VLAN_TAG_PROC                     (0xB67AU) /**< @brief #XGMAC_SetVlanTag                   */
#define BRCM_SWDSGN_XGMAC_GET_DRIVER_STATUS_PROC                (0xB67BU) /**< @brief #XGMAC_GetDriverStatus              */
#define BRCM_SWDSGN_XGMAC_DMA_SET_RX_DESC_LIST_ADDR_PROC        (0xB67CU) /**< @brief #XGMAC_DMASetRxDescListAddr         */
#define BRCM_SWDSGN_XGMAC_DMA_SET_TX_DESC_LIST_ADDR_PROC        (0xB67DU) /**< @brief #XGMAC_DMASetTxDescListAddr         */
#define BRCM_SWDSGN_XGMAC_DMA_SET_REC_BUFFER_SIZE_PROC          (0xB67EU) /**< @brief #XGMAC_DMASetRecBufferSize          */
#define BRCM_SWDSGN_XGMAC_DMA_INIT_CHANNEL_PROC                 (0xB67FU) /**< @brief #XGMAC_DMAInitChannel               */
#define BRCM_SWDSGN_XGMAC_DMA_INIT_TX_CHAN_PROC                 (0xB680U) /**< @brief #XGMAC_DMAInitTxChan                */
#define BRCM_SWDSGN_XGMAC_DMA_INIT_RX_CHAN_PROC                 (0xB681U) /**< @brief #XGMAC_DMAInitRxChan                */
#define BRCM_SWDSGN_XGMAC_ENABLE_CORE_INTERRUPT_PROC            (0xB682U) /**< @brief #XGMAC_EnableCoreInterrupt          */
#define BRCM_SWDSGN_XGMAC_ENABLE_DMA_CHANNEL_INTERRUPT_PROC     (0xB683U) /**< @brief #XGMAC_EnableDMAChannelInterrupt    */
#define BRCM_SWDSGN_XGMAC_CLEAR_CORE_INTERRUPT_PROC             (0xB685U) /**< @brief #XGMAC_ClearCoreInterrupt           */
#define BRCM_SWDSGN_XGMAC_GET_CORE_INTERRUPT_STATUS_PROC        (0xB687U) /**< @brief #XGMAC_GetCoreInterruptStatus       */
#define BRCM_SWDSGN_XGMAC_CORE_REGS_GLOBAL                      (0xB689U) /**< @brief #XGMAC_CoreRegs                     */
#define BRCM_SWDSGN_XGMAC_DMA_REGS_GLOBAL                       (0xB68AU) /**< @brief #XGMAC_DMARegs                      */
#define BRCM_SWDSGN_XGMAC_MTL_TXQ_GLOBAL                        (0xB68BU) /**< @brief #XGMAC_MTLTxQ                       */
#define BRCM_SWDSGN_XGMAC_DMA_CHAN_CFG_GLOBAL                   (0xB68CU) /**< @brief #XGMAC_DMAChanCfg                   */
#define BRCM_SWDSGN_XGMAC_RWDEV_TYPE                            (0xB68DU) /**< @brief #XGMAC_RWDevType                    */
#define BRCM_SWDSGN_XGMAC_HI_REG_ADDRESS_ENABLE_MACRO           (0xB68EU) /**< @brief #XGMAC_HI_REG_ADDRESS_ENABLE        */
#define BRCM_SWDSGN_XGMAC_INTR_DEFAULT_ENABLE_MACRO             (0xB68FU) /**< @brief #XGMAC_INTR_DEFAULT_ENABLE          */
#define BRCM_SWDSGN_XGMAC_INTR_DMA_CHAN_DEFAULT_ENABLE_MACRO    (0xB690U) /**< @brief #XGMAC_INTR_DMA_CHAN_DEFAULT_ENABLE */
#define BRCM_SWDSGN_XGMAC_MAX_MAC_ADDRESS_FILTER_MACRO          (0xB691U) /**< @brief #XGMAC_MAX_MAC_ADDRESS_FILTER       */
#define BRCM_SWDSGN_XGMAC_ADDR_HIGH_MACRO                       (0xB692U) /**< @brief #XGMAC_ADDR_HIGH                    */
#define BRCM_SWDSGN_XGMAC_ADDR_LOW_MACRO                        (0xB693U) /**< @brief #XGMAC_ADDR_LOW                     */
#define BRCM_SWDSGN_XGMAC_RWDEV_GLOBAL                          (0xB694U) /**< @brief #XGMAC_RWDev                        */
#define BRCM_SWDSGN_XGMAC_GIANT_PKT_SIZE_LIMIT_MACRO            (0xB695U) /**< @brief #XGMAC_GIANT_PKT_SIZE_LIMIT         */
#define BRCM_SWDSGN_XGMAC_INT_JUMBO_PACKET_ENABLE_PROC          (0xB696U) /**< @brief #XGMAC_IntJumboPacketEnable         */
#define BRCM_SWDSGN_XGMAC_GET_MIB_COUNTER_STATS_PROC            (0xB697U) /**< @brief #XGMAC_GetMIBCounterStats           */

#define BRCM_SWDSGN_XGMAC_INT_ENABLE_DMA_CHANNEL_INTERRUPT_GRP0_PROC (0xB698U) /**< @brief #XGMAC_IntEnableDMAChannelInterruptGrp0 */
#define BRCM_SWDSGN_XGMAC_INT_ENABLE_DMA_CHANNEL_INTERRUPT_GRP1_PROC (0xB699U) /**< @brief #XGMAC_IntEnableDMAChannelInterruptGrp1 */
#define BRCM_SWDSGN_XGMAC_INT_ENABLE_DMA_CHANNEL_INTERRUPT_GRP2_PROC (0xB69AU) /**< @brief #XGMAC_IntEnableDMAChannelInterruptGrp2 */
#define BRCM_SWDSGN_XGMAC_INT_MAC_PACKET_FILTER_ENABLE_GRP0_PROC     (0xB69BU) /**< @brief #XGMAC_IntMacPacketFilterEnableGrp0 */
#define BRCM_SWDSGN_XGMAC_INT_MAC_PACKET_FILTER_ENABLE_GRP1_PROC     (0xB69CU) /**< @brief #XGMAC_IntMacPacketFilterEnableGrp1 */
#define BRCM_SWDSGN_XGMAC_INT_MAC_PACKET_FILTER_ENABLE_GRP2_PROC     (0xB69DU) /**< @brief #XGMAC_IntMacPacketFilterEnableGrp2 */
#define BRCM_SWDSGN_XGMAC_MTL_REGS_GLOBAL                            (0xB69EU) /**< @brief #XGMAC_MTLRegs                      */
#define BRCM_SWDSGN_XGMAC_INT_DMA_MTL_QUEUE_STATUS_GET_PROC          (0xB69FU) /**< @brief #XGMAC_IntDMAMtlQueueStatusGet */
#define BRCM_SWDSGN_XGMAC_INT_DMA_MTL_QUEUE_STATUS_CLEAR_PROC        (0xB6A0U) /**< @brief #XGMAC_IntDMAMtlQueueStatusClear */

/** @} */

/**
    @trace #BRCM_SWARCH_XGMAC_SET_MAC_ADDRESS_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
#define XGMAC_HI_REG_ADDRESS_ENABLE   BCM_BIT(31UL)

/**
    @brief XGMAC Interrupt bitmap.
    @trace #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
#define XGMAC_INTR_DEFAULT_ENABLE    (XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK | \
                                    XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK | \
                                    XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK)

/**
    @brief XGMAC default DMA interrupt bitmap.
    @trace #BRCM_SWARCH_XGMAC_DMA_INIT_CHANNEL_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
#define XGMAC_INTR_DMA_CHAN_DEFAULT_ENABLE (XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK | \
                                            XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK | \
                                            XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK)


/**
    @brief Maximum number of address filter is defined as 2.
    @trace #BRCM_SWARCH_XGMAC_SET_ADDRESS_FILTER_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
#define XGMAC_MAX_MAC_ADDRESS_FILTER 2

/**
    @brief GIANT packet size limit.
    @trace #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
#define XGMAC_GIANT_PKT_SIZE_LIMIT (16376UL)


/**
    @brief XGMAC address high define.
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace #BRCM_SWARCH_XGMAC_SET_MAC_ADDRESS_PROC
*/
#define XGMAC_ADDR_HIGH(addr, reg)      ((addr) + ((reg) << 3U))
/**
    @brief XGMAC address low define.
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace #BRCM_SWARCH_XGMAC_SET_MAC_ADDRESS_PROC
*/
#define XGMAC_ADDR_LOW(addr, reg)       ((addr) + ((reg) << 3U))

/**
    @brief Driver database structure.

    @trace #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
typedef struct sXGMAC_RWDevType {
    XGMAC_DriverStatusType driverStatus;
} XGMAC_RWDevType;

/**
    @brief Global data to store driver status

    @trace #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
static XGMAC_RWDevType XGMAC_RWDev[XGMAC_CORE_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
static XGMAC_CORE_RDBType * const XGMAC_CoreRegs[XGMAC_CORE_MAX_HW_ID] =
{

    (XGMAC_CORE_RDBType *)XG_CORE_BASE,

#if (XGMAC_CORE_MAX_HW_ID > 1)
#error "XGMAC_CORE_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_XGMAC_DMA_INIT_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_MODE_SW_RESET_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
*/
static XGMAC_DMA_RDBType * const XGMAC_DMARegs[XGMAC_CORE_MAX_HW_ID] =
{

    (XGMAC_DMA_RDBType *)XG_DMA_BASE,

#if (XGMAC_CORE_MAX_HW_ID > 1)
#error "XGMAC_CORE_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @brief MTL Queue configuration structure
    @trace #BRCM_SWARCH_XGMAC_SET_MTL_TXQ_CONFIGURATION_PROC
    @trace #BRCM_SWARCH_XGMAC_SET_MTL_RXQ_CONFIGURATION_PROC
    @trace #BRCM_SWARCH_XGMAC_FLUSH_TX_FIFO_PROC
    @trace #BRCM_SWARCH_XGMAC_SET_DMA_QMODE_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/
static XGMAC_MTL_RDBType * const XGMAC_MTLRegs[XGMAC_CORE_MAX_HW_ID] =
{

    (XGMAC_MTL_RDBType *)XG_MTL_BASE,

#if (XGMAC_CORE_MAX_HW_ID > 1)
#error "XGMAC_CORE_MAX_HW_ID > 1 is not supported by the driver"
#endif
};


/**
    @brief MTL Queue configuration structure
    @trace #BRCM_SWARCH_XGMAC_SET_MTL_TXQ_CONFIGURATION_PROC
    @trace #BRCM_SWARCH_XGMAC_SET_MTL_RXQ_CONFIGURATION_PROC
    @trace #BRCM_SWARCH_XGMAC_FLUSH_TX_FIFO_PROC
    @trace #BRCM_SWARCH_XGMAC_SET_DMA_QMODE_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
*/

static XGMAC_MTL_TCQ0_RDBType * const XGMAC_MTLTxQ[8] = {(XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ0_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ1_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ2_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ3_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ4_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ5_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ6_BASE, (XGMAC_MTL_TCQ0_RDBType *)XG_MTL_TCQ7_BASE};

/**
    @brief XGMAC DMA channel configuration
    @trace #BRCM_SWARCH_XGMAC_DMA_INIT_CHANNEL_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_INIT_TX_CHAN_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_INIT_RX_CHAN_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_TX_RING_LEN_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_RX_RING_LEN_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_TX_TAILPTR_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_RX_TAILPTR_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_TX_DESC_LIST_ADDR_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_RX_DESC_LIST_ADDR_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_SET_REC_BUFFER_SIZE_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_TRANSMIT_ENABLE_PROC
    @trace #BRCM_SWARCH_XGMAC_DMA_RECEIVE_ENABLE_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/

static XGMAC_DMA_CH0_RDBType * const XGMAC_DMAChanCfg[8] = {(XGMAC_DMA_CH0_RDBType *)XG_DMA_CH0_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH1_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH2_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH3_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH4_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH5_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH6_BASE, (XGMAC_DMA_CH0_RDBType *)XG_DMA_CH7_BASE};

/**
    @brief Jumbo packet enable
    @trace #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_XGMAC_DRIVER_INIT
 */
static void XGMAC_IntJumboPacketEnable(XGMAC_HwIDType aId, XGMAC_EthConfigType *aConfig)
{
    /* Enable jumbo packet */
    if (aConfig->jumboPktEn != 0U) {
        XGMAC_CoreRegs[aId]->mac_tx_configuration |= XGMAC_CORE_MAC_TX_CONFIGURATION_JD_MASK;
        XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_JE_MASK;
        XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_WD_MASK;
    } else {
        XGMAC_CoreRegs[aId]->mac_tx_configuration &= ~XGMAC_CORE_MAC_TX_CONFIGURATION_JD_MASK;
        XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_JE_MASK;
        XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_WD_MASK;
    }
    if (aConfig->watchdogDisable != 0U) {
        XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_WD_MASK;
    } else {
        XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_WD_MASK;
    }
}

/** @brief Initialize XGMAC peripheral.
    This API initializes and configures XGMAC with initial configuration.

    @code{.unparsed}
    reset DMA registers
    update mac address
    call XGMAC_SetMacAddress()
    if error
        return error code
    update speed
    call XGMAC_SpeedModeSet()
    if error
        return error code
    set duplex settings
    set IPG control
    Set Jumbo packet
    Set Watchdog disable
    Set checksumOffload
    Set autoCRCstripping
    Set arpEnable
    Enable default XGMAC interrupts.
    @endcode

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_DRV_INIT_PROC
*/
int XGMAC_DrvInit(XGMAC_HwIDType aId, XGMAC_EthConfigType *aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    if (aId >= XGMAC_CORE_MAX_HW_ID || NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Reset DMA registers */
        retVal = XGMAC_DMAModeSwReset(aId);

        if(BCM_ERR_OK == retVal) {
            retVal =  XGMAC_SetMacAddress(aId, &aConfig->macAddr);
        }

        if(BCM_ERR_OK == retVal) {
            if (aConfig->speed > 0UL) {
                retVal = XGMAC_SpeedModeSet(aId, aConfig->speed);
            }
        }
        if(BCM_ERR_OK == retVal) {
            if (aConfig->duplex == XGMAC_ETH_DUPLEX_MODE_HALF) {
                XGMAC_CoreRegs[aId]->mac_extended_configuration |= XGMAC_CORE_MAC_EXTENDED_CONFIGURATION_HD_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_extended_configuration &= ~XGMAC_CORE_MAC_EXTENDED_CONFIGURATION_HD_MASK;
            }
            if(aConfig->speed == XGMAC_ETH_SPEED_MODE_2P5G_XGMII) {
                XGMAC_CoreRegs[aId]->mac_extended_configuration |= XGMAC_CORE_MAC_EXTENDED_CONFIGURATION_TPRE_MASK;
            }
            if(aConfig->IPGCtrl != 0U) {
                XGMAC_CoreRegs[aId]->mac_tx_configuration |= XGMAC_CORE_MAC_TX_CONFIGURATION_IFP_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_tx_configuration &= ~XGMAC_CORE_MAC_TX_CONFIGURATION_IFP_MASK;
            }
            /* Set Inter-Packet Gap */
            XGMAC_CoreRegs[aId]->mac_tx_configuration &= ~XGMAC_CORE_MAC_TX_CONFIGURATION_IPG_MASK;
            XGMAC_CoreRegs[aId]->mac_tx_configuration |= (aConfig->IPGLenStep << XGMAC_CORE_MAC_TX_CONFIGURATION_IPG_SHIFT);
            /* Set Giant packet enable */
            XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_GPSLCE_MASK;
            XGMAC_CoreRegs[aId]->mac_rx_configuration |= (XGMAC_GIANT_PKT_SIZE_LIMIT << XGMAC_CORE_MAC_RX_CONFIGURATION_GPSL_SHIFT);

            XGMAC_IntJumboPacketEnable(aId, aConfig);

            if (aConfig->checksumOffload != 0U) {
                XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_IPC_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_IPC_MASK;
            }
            if (aConfig->autoCRCstripping != 0U) {
                XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_ACS_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_ACS_MASK;
            }
            if (aConfig->arpEnable != 0U) {
                XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_ARPEN_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_ARPEN_MASK;
            }

            if(aConfig->rcwEn != 0U) {
                XGMAC_CoreRegs[aId]->mac_csr_sw_ctrl |= XGMAC_CORE_MAC_CSR_SW_CTRL_RCWE_MASK;
            } else {
               XGMAC_CoreRegs[aId]->mac_csr_sw_ctrl &= ~XGMAC_CORE_MAC_CSR_SW_CTRL_RCWE_MASK;
            }
            /* Enable RXQ0EN and RXQ1EN */
            XGMAC_CoreRegs[aId]->mac_rxq_ctrl0 |= (2UL << XGMAC_CORE_MAC_RXQ_CTRL0_0EN_SHIFT);
            XGMAC_CoreRegs[aId]->mac_rxq_ctrl0 |= (2UL << XGMAC_CORE_MAC_RXQ_CTRL0_1EN_SHIFT);
            /* TPQC  (Tagged PTP over Ethernet Packets Queuing Control) */
            XGMAC_CoreRegs[aId]->mac_rxq_ctrl1 |= XGMAC_CORE_MAC_RXQ_CTRL1_TPQC_MASK;
            /* Enable VLAN tag in RX status */
            XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_EVLRXS_MASK;
            /* Program the MAC interrupt enable */
            XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_INTR_DEFAULT_ENABLE;
            XGMAC_RWDev[aId].driverStatus |= XGMAC_DRIVER_STATUS_CORE_INITIALIZED;
        }
    }
    return retVal;
}

/** @brief De-Initialize XGMAC peripheral.

    This API De-initializes XGMAC and free any resources allocated.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_DRV_DEINIT_PROC
    @code{.unparsed}
    Soft reset XGMAC core
    XGMAC_DMAModeSwReset
    @endcode
*/
int XGMAC_DrvDeInit(XGMAC_HwIDType aId)
{
    int retVal = BCM_ERR_OK;
    if(aId < XGMAC_CORE_MAX_HW_ID) {
        retVal = XGMAC_DMAModeSwReset(aId);
        XGMAC_RWDev[aId].driverStatus = 0;
    }
    return retVal;
}

/** @brief Configure speed mode
    This API configures XGMAC in specified speed mode.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_SPEED_MODE_SET_PROC
    @code{.unparsed}
    Set speed XGMAC_ETH_SPEED_MODE_2P5G_XGMII or XGMAC_ETH_SPEED_MODE_1G_GMII
    or XGMAC_ETH_SPEED_MODE_100M_MII or XGMAC_ETH_SPEED_MODE_10M_MII.
    @endcode
*/
int XGMAC_SpeedModeSet(XGMAC_HwIDType aId, XGMAC_EthSpeedModeType aSpeed)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID || 0U == aSpeed) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        XGMAC_CoreRegs[aId]->mac_tx_configuration &= ~XGMAC_CORE_MAC_TX_CONFIGURATION_SS_MASK;
        switch(aSpeed)
            {
                case XGMAC_ETH_SPEED_MODE_2P5G_XGMII:
                    XGMAC_CoreRegs[aId]->mac_tx_configuration |= (XGMAC_ETH_SPEED_MODE_2P5G_XGMII << XGMAC_CORE_MAC_TX_CONFIGURATION_SS_SHIFT);
                    break;
                case XGMAC_ETH_SPEED_MODE_2P5G_GMII:
                    XGMAC_CoreRegs[aId]->mac_tx_configuration |= (XGMAC_ETH_SPEED_MODE_2P5G_GMII << XGMAC_CORE_MAC_TX_CONFIGURATION_SS_SHIFT);
                    break;
                case XGMAC_ETH_SPEED_MODE_1G_GMII:
                    XGMAC_CoreRegs[aId]->mac_tx_configuration |= (XGMAC_ETH_SPEED_MODE_1G_GMII << XGMAC_CORE_MAC_TX_CONFIGURATION_SS_SHIFT);
                    break;
                case XGMAC_ETH_SPEED_MODE_100M_MII:
                    XGMAC_CoreRegs[aId]->mac_tx_configuration |= (XGMAC_ETH_SPEED_MODE_100M_MII << XGMAC_CORE_MAC_TX_CONFIGURATION_SS_SHIFT);
                    break;
                case XGMAC_ETH_SPEED_MODE_10M_MII:
                    XGMAC_CoreRegs[aId]->mac_tx_configuration |= (XGMAC_ETH_SPEED_MODE_10M_MII << XGMAC_CORE_MAC_TX_CONFIGURATION_SS_SHIFT);
                    break;
                default :
                    retVal = BCM_ERR_INVAL_PARAMS;
                    break;

          }
#ifdef BCM8915X_PARAM_VALIDATION
    }

    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
#endif
    return retVal;
}

/** @brief Set MAC address

    This API set the MAC address in register MAC_Address0_High and MAC_Address0_Low.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_SET_MAC_ADDRESS_PROC
    @code{.unparsed}
    Set MAC address.
    @endcode
*/
int XGMAC_SetMacAddress(XGMAC_HwIDType aId, const XGMAC_EthMacAddrType *pMacAddr)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t data = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID || NULL == pMacAddr) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        data = ((uint32_t)(pMacAddr->macAddr[5U]) << 8UL) | (uint32_t)(pMacAddr->macAddr[4U]);

        /* Select default channel 0 for receive packet */
        data |= (0UL << XGMAC_CORE_MAC_ADDRESS0_HIGH_DCS_SHIFT) ;
        /* @event            CERT PRE31-C
        issue            Coverity infers macro definition is unsafe
        risk             No risk
        justification    This macro is calculating the base address of the register, it has no side effect. */
        /* coverity[cert_pre31_c_violation] */
        *(uint32_t *)(uintptr_t)(XGMAC_ADDR_HIGH(((uint32_t)&XGMAC_CoreRegs[aId]->mac_address0_high), 0UL)) = data | XGMAC_HI_REG_ADDRESS_ENABLE;
        data = ((uint32_t)(pMacAddr->macAddr[3U]) << 24UL) | ((uint32_t)(pMacAddr->macAddr[2U]) << 16UL) | ((uint32_t)(pMacAddr->macAddr[1U]) << 8UL) | ((uint32_t)(pMacAddr->macAddr[0U]));
        /* @event            CERT PRE31-C
        issue            Coverity infers macro definition is unsafe
        risk             No risk
        justification    This macro is calculating the base address of the register, it has no side effect. */
        /* coverity[cert_pre31_c_violation] */
        *(uint32_t *)(uintptr_t)(XGMAC_ADDR_LOW(((uint32_t)&XGMAC_CoreRegs[aId]->mac_address0_low), 0UL)) = data;
#ifdef BCM8915X_PARAM_VALIDATION
    }
    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
#endif
    return retVal;
}

/** @brief XGMAC packet filter enable Group0

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_MAC_PACKET_FILTER_ENABLE_PROC
*/
static int XGMAC_IntMacPacketFilterEnableGrp0(XGMAC_HwIDType aId, XGMAC_PktFilterModeType pktFilterMode, int aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_REC_ALL)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_RA_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_RA_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_REC_BROADCAST)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_DBF_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_DBF_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_REC_MULTICAST)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_PM_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_PM_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_PR)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_PR_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_PR_MASK;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret ;
}

/** @brief XGMAC packet filter enable Group1

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_MAC_PACKET_FILTER_ENABLE_PROC
*/
static int XGMAC_IntMacPacketFilterEnableGrp1(XGMAC_HwIDType aId, XGMAC_PktFilterModeType pktFilterMode, int aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_PCF)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_PCF_MASK;
            XGMAC_CoreRegs[aId]->mac_packet_filter |= ((uint32_t)aEnable << XGMAC_CORE_MAC_PACKET_FILTER_PCF_SHIFT);
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_PCF_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_SAF)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_SAF_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_SAF_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_VLAN_TAG_FILTER_ENABLE)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_VTFE_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_VTFE_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_HPF_FILTER_ENABLE)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_HPF_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_HPF_MASK;
        }
    }  else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret ;
}

/** @brief XGMAC packet filter enable Group2

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_MAC_PACKET_FILTER_ENABLE_PROC
*/
static int XGMAC_IntMacPacketFilterEnableGrp2(XGMAC_HwIDType aId, XGMAC_PktFilterModeType pktFilterMode, int aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_HUC)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_HUC_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_HUC_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_HMC)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_HMC_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_HMC_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_SAIF)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_SAIF_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_SAIF_MASK;
        }
    } else if(0UL != (pktFilterMode & XGMAC_PKT_FILTER_MODE_DAIF)) {
        if (aEnable > 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_DAIF_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_packet_filter &= ~XGMAC_CORE_MAC_PACKET_FILTER_DAIF_MASK;
        }
    }  else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret ;
}

/** @brief XGMAC packet filter enable

    XGMAC packet filter enable for the selected filter type.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_MAC_PACKET_FILTER_ENABLE_PROC
    @code{.unparsed}
    Set selected packet filter type.
    @endcode
*/
int XGMAC_MacPacketFilterEnable(XGMAC_HwIDType aId, XGMAC_EthPktFilterType *pktFilterType, int aEnable)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID || NULL == pktFilterType) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        if(0UL != (pktFilterType->pktFilter & XGMAC_PKT_FILTER_MODE_GRP0)) {
            retVal = XGMAC_IntMacPacketFilterEnableGrp0(aId, pktFilterType->pktFilter, aEnable);
        } else if(0UL != (pktFilterType->pktFilter & XGMAC_PKT_FILTER_MODE_GRP1)) {
            retVal = XGMAC_IntMacPacketFilterEnableGrp1(aId, pktFilterType->pktFilter, aEnable);
        } else if(0UL != (pktFilterType->pktFilter & XGMAC_PKT_FILTER_MODE_GRP2)) {
            retVal = XGMAC_IntMacPacketFilterEnableGrp2(aId, pktFilterType->pktFilter, aEnable);
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief XGMAC Set address filtering

    XGMAC configures perfect match address filters for specified number of MAC addresses.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_ADDRESS_FILTER_PROC
    @code{.unparsed}
    Set the number of address filter SA or DA need to be prgrammed
    to compare for the received packet.
    @endcode
*/
int XGMAC_SetAddressFilter(XGMAC_HwIDType aId, const XGMAC_EthMacAddrType *pMacAddr, uint32_t numAddr)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t index = 0UL;
    uint32_t data = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID || numAddr > XGMAC_MAX_MAC_ADDRESS_FILTER) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        for(index = 1UL; index<= numAddr; index++) {
            data = ((uint32_t)(pMacAddr[index - 1UL].macAddr[5U]) << 8UL) | (uint32_t)(pMacAddr[index - 1UL].macAddr[4U]);
            if(pMacAddr[index - 1UL].macAddrType == XGMAC_MAC_ADDR_DA) {
                data &= ~XGMAC_CORE_MAC_ADDRESS1_HIGH_SA_MASK;
            } else {
                data |= XGMAC_CORE_MAC_ADDRESS1_HIGH_SA_MASK;
            }

            /* Select default channel 0 for receive packet */
            data |= (0UL << XGMAC_CORE_MAC_ADDRESS0_HIGH_DCS_SHIFT) ;
            /* @event            CERT PRE31-C
            issue            Coverity infers macro definition is unsafe
            risk             No risk
            justification    This macro is calculating the base address of the register, it has no side effect. */
            /* coverity[cert_pre31_c_violation] */
            *(uint32_t *)(uintptr_t)(XGMAC_ADDR_HIGH(((uint32_t)&XGMAC_CoreRegs[aId]->mac_address0_high), index)) = data | XGMAC_CORE_MAC_ADDRESS1_HIGH_AE_MASK;
            data = ((uint32_t)(pMacAddr[index - 1UL].macAddr[3U]) << 24UL) | ((uint32_t)(pMacAddr[index - 1UL].macAddr[2U]) << 16UL) | ((uint32_t)(pMacAddr[index - 1UL].macAddr[1U]) << 8UL) | ((uint32_t)(pMacAddr[index - 1UL].macAddr[0U]));
            /* @event            CERT PRE31-C
            issue            Coverity infers macro definition is unsafe
            risk             No risk
            justification    This macro is calculating the base address of the register, it has no side effect. */
            /* coverity[cert_pre31_c_violation] */
            *(uint32_t *)(uintptr_t)(XGMAC_ADDR_LOW(((uint32_t)&XGMAC_CoreRegs[aId]->mac_address0_low), index)) = data;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief XGMAC Set flow control

    XGMAC configures flow control settings.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_FLOW_CTRL_PROC
    @code{.unparsed}
    if (flowCtrl->pauseTime) {
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PT_MASK
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= (flowCtrl->pauseTime <<  XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PT_SHIFT)
    }
    if (flowCtrl->disableZeroQuantaPause) {
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_DZPQ_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_DZPQ_MASK
    }
    if (flowCtrl->pauseLowThr) {
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PLT_MASK
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= (flowCtrl->pauseLowThr <<  XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PLT_SHIFT)
    }
    if (flowCtrl->txFE){
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_TFE_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_TFE_MASK
    }
    if (flowCtrl->flowCtrlBusy){
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_FCB_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_FCB_MASK
    }
    if (flowCtrl->rxFE){
        XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl|= XGMAC_CORE_MAC_RX_FLOW_CTRL_RFE_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl &= ~XGMAC_CORE_MAC_RX_FLOW_CTRL_RFE_MASK
    }
    if (flowCtrl->unicastPausePktDet){
        XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl|= XGMAC_CORE_MAC_RX_FLOW_CTRL_UP_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl &= ~XGMAC_CORE_MAC_RX_FLOW_CTRL_UP_MASK
    }
    if (flowCtrl->priorityFlowCtrlEn){
        XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl|= XGMAC_CORE_MAC_RX_FLOW_CTRL_PFCE_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl &= ~XGMAC_CORE_MAC_RX_FLOW_CTRL_PFCE_MASK
    }
    @endcode
*/
int XGMAC_SetFlowCtrl(XGMAC_HwIDType aId, XGMAC_FlowCtrlCfgType *flowCtrl)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID || NULL == flowCtrl) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        if (flowCtrl->pauseTime != 0U) {
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PT_MASK;
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= (flowCtrl->pauseTime <<  XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PT_SHIFT);
        }
        if (flowCtrl->disableZeroQuantaPause != 0U) {
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_DZPQ_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_DZPQ_MASK;
        }
        if (flowCtrl->pauseLowThr != 0U) {
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PLT_MASK;
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= (flowCtrl->pauseLowThr <<  XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_PLT_SHIFT);
        }
        if (flowCtrl->txFE != 0U){
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_TFE_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_TFE_MASK;
        }
        if (flowCtrl->flowCtrlBusy != 0U){
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] |= XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_FCB_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_q0_tx_flow_ctrl[flowCtrl->txQIndex] &= ~XGMAC_CORE_MAC_Q0_TX_FLOW_CTRL_FCB_MASK;
        }
        if (flowCtrl->rxFE != 0U){
            XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl|= XGMAC_CORE_MAC_RX_FLOW_CTRL_RFE_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl &= ~XGMAC_CORE_MAC_RX_FLOW_CTRL_RFE_MASK;
        }
        if (flowCtrl->unicastPausePktDet != 0U){
            XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl|= XGMAC_CORE_MAC_RX_FLOW_CTRL_UP_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl &= ~XGMAC_CORE_MAC_RX_FLOW_CTRL_UP_MASK;
        }
        if (flowCtrl->priorityFlowCtrlEn != 0U){
            XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl|= XGMAC_CORE_MAC_RX_FLOW_CTRL_PFCE_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_rx_flow_ctrl &= ~XGMAC_CORE_MAC_RX_FLOW_CTRL_PFCE_MASK;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
#endif
    return retVal;
}

/** @brief XGMAC MAC enable/disable

    This API enable/disable the MAC block to transmit and receive packets. This API
    is called when MAC configuration is completed.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_MAC_ENABLE_PROC
    @code{.unparsed}
    if(aEnable) {
        XGMAC_CoreRegs[aId]->mac_tx_configuration |= XGMAC_CORE_MAC_TX_CONFIGURATION_TE_MASK
        XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_RE_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_tx_configuration &= ~XGMAC_CORE_MAC_TX_CONFIGURATION_TE_MASK
        XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_RE_MASK
    }
    @endcode
*/
int XGMAC_MacEnable(XGMAC_HwIDType aId, int aEnable)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        /* Enable MAC transmit and receive */
        if(aEnable != 0) {
            XGMAC_CoreRegs[aId]->mac_tx_configuration |= XGMAC_CORE_MAC_TX_CONFIGURATION_TE_MASK;
            XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_RE_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_tx_configuration &= ~XGMAC_CORE_MAC_TX_CONFIGURATION_TE_MASK;
            XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_RE_MASK;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }

    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
#endif
    return retVal;
}

/** @brief XGMAC Looback set

    This API enables/disables internal loopback.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_MAC_LOOPBACK_PROC
    @code{.unparsed}
    if (lb) {
        XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_LM_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_LM_MASK
    }
    @endcode
*/
int XGMAC_SetMacLoopback(XGMAC_HwIDType aId, uint32_t lb)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    }   else {
#endif
        if (lb != 0U) {
            XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_LM_MASK;
        } else {
            XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_LM_MASK;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }

    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
#endif
    return retVal;
}

/* MTL configuration APIs */

/** @brief XGMAC Set MTL TX Queue configuration

    XGMAC configures TX Queue configuration settings.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_MTL_TXQ_CONFIGURATION_PROC
    @code{.unparsed}
    XGMAC_MTL_TCQ0_RDBType *pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[txQIndex]
    if(mtlCfg->flushTxQ) {
        pTCQ->txq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_FTQ_MASK
    }
    if(mtlCfg->txQStoreForward) {
        pTCQ->txq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TSF_MASK
    }
    pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TXQEN_MASK
    pTCQ->txq0_operation_mode |= (mtlCfg->txQEnable << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TXQEN_SHIFT)
    pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TTC_MASK
    pTCQ->txq0_operation_mode |= (mtlCfg->txQThrCtrl << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TTC_SHIFT)
    pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_Q2TCMAP_MASK
    pTCQ->txq0_operation_mode |= (mtlCfg->txQMapTC << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_Q2TCMAP_SHIFT)
    pTCQ->txq0_operation_mode |= (mtlCfg->txQSize << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TQS_SHIFT)
    Select TSA (Transmission Selection Algorithm) as strict priority.
    @endcode
*/
int XGMAC_SetMTLTxQConfiguration(XGMAC_HwIDType aId, XGMAC_MtlTxQConfigType *mtlCfg, uint8_t txQIndex)

{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_MTL_TCQ0_RDBType *pTCQ = NULL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID || (NULL == mtlCfg) || (txQIndex > 7U) || (mtlCfg->txQSize > 0x7FUL) ||
        (mtlCfg->psTC > 7UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[txQIndex];
        if(mtlCfg->flushTxQ != 0U) {
            pTCQ->txq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_FTQ_MASK;
        }
        if(mtlCfg->txQStoreForward != 0U) {
            pTCQ->txq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TSF_MASK;
        }

        if(mtlCfg->txQUnderflowIntrEn > 0U) {
            pTCQ->q0_interrupt_enable |= XGMAC_MTL_TCQ0_MTL_Q0_INTERRUPT_ENABLE_TXUIE_MASK;
        }
        /* TBD: RXQEN->TXQEN */
        pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TXQEN_MASK;
        pTCQ->txq0_operation_mode |= (mtlCfg->txQEnable << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TXQEN_SHIFT) ;
        pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TTC_MASK;
        pTCQ->txq0_operation_mode |= (mtlCfg->txQThrCtrl << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TTC_SHIFT);
        pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_Q2TCMAP_MASK;
        pTCQ->txq0_operation_mode |= (mtlCfg->txQMapTC << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_Q2TCMAP_SHIFT);
        pTCQ->txq0_operation_mode |= (mtlCfg->txQSize << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TQS_SHIFT);
        /* Select TSA algorithm. */
        pTCQ->tc1_ets_control &= ~XGMAC_MTL_TCQ0_MTL_TC1_ETS_CONTROL_TSA_MASK;
        pTCQ->tc1_ets_control |= (mtlCfg->tsaAlgo << XGMAC_MTL_TCQ0_MTL_TC1_ETS_CONTROL_TSA_SHIFT);
        /* Select ETS algorithm type. */
        XGMAC_MTLRegs[aId]->operation_mode &= ~XGMAC_MTL_MTL_OPERATION_MODE_ETSALG_MASK;
        XGMAC_MTLRegs[aId]->operation_mode |= (mtlCfg->etsAlgo << XGMAC_MTL_MTL_OPERATION_MODE_ETSALG_SHIFT);
        pTCQ->tc1_quantum_weight &= ~XGMAC_MTL_TCQ0_MTL_TC1_QUANTUM_WEIGHT_QW_MASK;
        /* Assign quantum weight to the traffic class. */
        pTCQ->tc1_quantum_weight |= (mtlCfg->mtlTcQuantWeight << XGMAC_MTL_TCQ0_MTL_TC1_QUANTUM_WEIGHT_QW_SHIFT);
        /* MAP MTL traffic class priority. */
        if(mtlCfg->txQMapTC < XGMAC_TXQ_MAP_TRAFFIC_CLASS_TC4) {
            XGMAC_MTLRegs[aId]->tc_prty_map0 &= ~(XGMAC_MTL_MTL_TC_PRTY_MAP0_PSTC0_MASK << ((mtlCfg->txQMapTC % 4UL) * 8UL));
            XGMAC_MTLRegs[aId]->tc_prty_map0 |= ((1UL << (mtlCfg->psTC & 7UL)) << ((mtlCfg->txQMapTC % 4UL) * 8UL));
        } else {
            XGMAC_MTLRegs[aId]->tc_prty_map1 &= ~(XGMAC_MTL_MTL_TC_PRTY_MAP0_PSTC0_MASK << ((mtlCfg->txQMapTC % 4UL) * 8UL));
            XGMAC_MTLRegs[aId]->tc_prty_map1 |= ((1UL << (mtlCfg->psTC & 7UL)) << ((mtlCfg->txQMapTC % 4UL) * 8UL));
        }
        XGMAC_RWDev[aId].driverStatus |= XGMAC_DRIVER_STATUS_MTL_TXQ_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }

#endif
    return retVal;
}

/** @brief XGMAC Set MTL RX Queue configuration

    XGMAC configures RX Queue configuration settings.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_MTL_RXQ_CONFIGURATION_PROC
    @code{.unparsed}
    if(mtlCfg->rxQStoreForward) {
        pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RSF_MASK
    }
    if(mtlCfg->fwdUndersizedPkts) {
        pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_FUF_MASK
    }
    if(mtlCfg->fwdErrorPkts) {
        pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_FEF_MASK
    }

    pTCQ->rxq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RTC_MASK
    pTCQ->rxq0_operation_mode |= (mtlCfg->rxQThrCtrl << XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RTC_SHIFT)
    if(mtlCfg->disDropTcpCsErrorPkts) {
        pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_DIS_TCP_EF_MASK
    }
    if(mtlCfg->enableHwFlowCtrl) {
        pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_EHFC_MASK
    }
    pTCQ->rxq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RQS_MASK
    pTCQ->rxq0_operation_mode |= (mtlCfg->rxQsize << XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RQS_SHIFT)

    pTCQ->rxq0_flow_control &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFA_MASK
    pTCQ->rxq0_flow_control |= (mtlCfg->rxQThrFlowEnable << XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFA_SHIFT)

    pTCQ->rxq0_flow_control &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFD_MASK
    pTCQ->rxq0_flow_control |= (mtlCfg->rxQThrFlowDisable << XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFD_SHIFT)
    @endcode
*/
int XGMAC_SetMTLRxQConfiguration(XGMAC_HwIDType aId, XGMAC_MtlRxQConfigType *mtlCfg, uint8_t rxQIndex)

{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_MTL_TCQ0_RDBType *pTCQ = NULL;
#ifdef BCM8915X_PARAM_VALIDATION
    uint32_t line = __LINE__;
    if ((aId >= XGMAC_CORE_MAX_HW_ID) || (NULL == mtlCfg) || (rxQIndex > 1U) || (mtlCfg->rxQsize > 0x7FUL)) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    }   else {
#endif
        pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[rxQIndex];
        if(mtlCfg->rxQStoreForward > 0U) {
            pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RSF_MASK;
        }
        if(mtlCfg->fwdUndersizedPkts > 0U) {
            pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_FUF_MASK;
        }
        if(mtlCfg->fwdErrorPkts > 0U) {
            pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_FEF_MASK;
        }
        if(mtlCfg->rxQFullIntrEnable > 0U) {
            pTCQ->q0_interrupt_enable |= XGMAC_MTL_TCQ0_MTL_Q0_INTERRUPT_ENABLE_RXOIE_MASK;
        }

        pTCQ->rxq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RTC_MASK;
        pTCQ->rxq0_operation_mode |= (mtlCfg->rxQThrCtrl << XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RTC_SHIFT) ;
        if(mtlCfg->disDropTcpCsErrorPkts > 0U) {
            pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_DIS_TCP_EF_MASK;
        }
        if(mtlCfg->enableHwFlowCtrl > 0U) {
            pTCQ->rxq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_EHFC_MASK;
        }
        pTCQ->rxq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RQS_MASK;
        pTCQ->rxq0_operation_mode |= (mtlCfg->rxQsize << XGMAC_MTL_TCQ0_MTL_RXQ0_OPERATION_MODE_RQS_SHIFT);

        pTCQ->rxq0_flow_control &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFA_MASK;
        pTCQ->rxq0_flow_control |= (mtlCfg->rxQThrFlowEnable << XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFA_SHIFT);

        pTCQ->rxq0_flow_control &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFD_MASK;
        pTCQ->rxq0_flow_control |= (mtlCfg->rxQThrFlowDisable << XGMAC_MTL_TCQ0_MTL_RXQ0_FLOW_CONTROL_RFD_SHIFT);
        if(rxQIndex == 0U) {
            XGMAC_MTLRegs[aId]->rxq_dma_map0 &= ~XGMAC_MTL_MTL_RXQ_DMA_MAP0_Q0DDMACH_MASK;
            XGMAC_MTLRegs[aId]->rxq_dma_map0 |= (mtlCfg->rxQDmaMap << XGMAC_MTL_MTL_RXQ_DMA_MAP0_Q0DDMACH_SHIFT);
            XGMAC_MTLRegs[aId]->rxq_dma_map0 |= (mtlCfg->rxQDmaChan << XGMAC_MTL_MTL_RXQ_DMA_MAP0_Q0MDMACH_SHIFT);
        } else {
            XGMAC_MTLRegs[aId]->rxq_dma_map0 &= ~XGMAC_MTL_MTL_RXQ_DMA_MAP0_Q1DDMACH_MASK;
            XGMAC_MTLRegs[aId]->rxq_dma_map0 |= (mtlCfg->rxQDmaMap << XGMAC_MTL_MTL_RXQ_DMA_MAP0_Q1DDMACH_SHIFT);
            XGMAC_MTLRegs[aId]->rxq_dma_map0 |= (mtlCfg->rxQDmaChan << XGMAC_MTL_MTL_RXQ_DMA_MAP0_Q1MDMACH_SHIFT);
        }
        XGMAC_MTLRegs[aId]->operation_mode &= ~XGMAC_MTL_MTL_OPERATION_MODE_RAA_MASK;
        XGMAC_MTLRegs[aId]->operation_mode |= (mtlCfg->rxArbAlgo << XGMAC_MTL_MTL_OPERATION_MODE_RAA_SHIFT);
        pTCQ->rxq0_control &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_CONTROL_RXQ_PKT_ARBIT_MASK;
        pTCQ->rxq0_control |= (mtlCfg->rxArbType << XGMAC_MTL_TCQ0_MTL_RXQ0_CONTROL_RXQ_PKT_ARBIT_SHIFT);
        pTCQ->rxq0_control &= ~XGMAC_MTL_TCQ0_MTL_RXQ0_CONTROL_RXQ_WEGT_MASK;
        pTCQ->rxq0_control |= (mtlCfg->rxQWegt << XGMAC_MTL_TCQ0_MTL_RXQ0_CONTROL_RXQ_WEGT_SHIFT);
        XGMAC_RWDev[aId].driverStatus |= XGMAC_DRIVER_STATUS_MTL_RXQ_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
#endif
    return retVal;
}

/** @brief XGMAC flush the TX Queue FIFO

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_FLUSH_TX_FIFO_PROC
    @code{.unparsed}
    if (lb) {
        XGMAC_CoreRegs[aId]->mac_rx_configuration |= XGMAC_CORE_MAC_RX_CONFIGURATION_LM_MASK
    } else {
        XGMAC_CoreRegs[aId]->mac_rx_configuration &= ~XGMAC_CORE_MAC_RX_CONFIGURATION_LM_MASK
    }
    @endcode
*/
int XGMAC_FlushTxFifo(XGMAC_HwIDType aId, uint8_t txQIndex)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_MTL_TCQ0_RDBType *pTCQ = NULL;

    if (aId >= XGMAC_CORE_MAX_HW_ID|| txQIndex > 7U) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[txQIndex];
        pTCQ->txq0_operation_mode |= XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_FTQ_MASK;
    }
    return retVal;
}

/* DMA configuration APIs */
/** @brief XGMAC DMA mode SW reset
     This API resets internal DMA, MTL and MAC core logic.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_MODE_SW_RESET_PROC
    @code{.unparsed}
    XGMAC_DMARegs[aId]->dma_mode |= XGMAC_DMA_DMA_MODE_SWR_MASK
    do {
        if (!(XGMAC_DMARegs[aId]->dma_mode & XGMAC_DMA_DMA_MODE_SWR_MASK)) {
            break
        }
    } while(--loopCounter)
    if(loopCounter == 0) {
        line = __LINE__
        retVal = BCM_ERR_TIME_OUT

    }
    @endcode
*/
int XGMAC_DMAModeSwReset(XGMAC_HwIDType aId)
{
    int32_t retVal = BCM_ERR_OK;
    int loopCounter = 1000;
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        XGMAC_DMARegs[aId]->dma_mode |= XGMAC_DMA_DMA_MODE_SWR_MASK;
        do {
            if ((XGMAC_DMARegs[aId]->dma_mode & XGMAC_DMA_DMA_MODE_SWR_MASK) == 0UL) {
                break;
            }
            --loopCounter;
        } while(loopCounter > 0);
        if(loopCounter == 0) {
            retVal = BCM_ERR_TIME_OUT;
        }
    }
    return retVal;
}

/** @brief Initializes XGMAC DMA mode transfer.

    This API initializes and configures XGMAC in DMA mode.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_INIT_PROC
    @code{.unparsed}
    rv = XGMAC_DMAModeSwReset(aId)
    if(rv) {
        BCM_LOG_ERROR("DMA SW reset failed rv:%d \n", rv)
        return rv
    }
    XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_AAL_MASK

    switch(dmaCfg->burstLen) {
        case XGMAC_DMA_BURST_LENGTH_UBL :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_UBL_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_4 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN4_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_8 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN8_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_16 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN16_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_32 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN32_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_64 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN64_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_128 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN128_MASK
            break
        case XGMAC_DMA_BURST_LENGTH_256 :
            XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN256_MASK
            break
        default :
            break
    }
    @endcode
*/
int XGMAC_DMAInit(XGMAC_HwIDType aId, XGMAC_DMACfgType *dmaCfg)
{
    int32_t retVal = BCM_ERR_OK;

    if (aId >= XGMAC_CORE_MAX_HW_ID || NULL == dmaCfg) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {

        XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_AAL_MASK;
        XGMAC_DMARegs[aId]->dma_sysbus_mode |= (1UL << XGMAC_DMA_DMA_SYSBUS_MODE_RD_OSR_LMT_SHIFT);
        XGMAC_DMARegs[aId]->dma_sysbus_mode |= (3UL << XGMAC_DMA_DMA_SYSBUS_MODE_WR_OSR_LMT_SHIFT);
        /* Set fixed burst length for AXI bus */
        switch(dmaCfg->burstLen) {
            case XGMAC_DMA_BURST_LENGTH_UBL :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_UBL_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_4 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN4_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_8 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN8_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_16 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN16_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_32 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN32_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_64 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN64_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_128 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN128_MASK;
                break;
            case XGMAC_DMA_BURST_LENGTH_256 :
                XGMAC_DMARegs[aId]->dma_sysbus_mode |= XGMAC_DMA_DMA_SYSBUS_MODE_BLEN256_MASK;
                break;
            default :
                retVal = BCM_ERR_NOSUPPORT;
                break;
        }
        XGMAC_RWDev[aId].driverStatus |= XGMAC_DRIVER_STATUS_DMA_INITIALIZED;
    }
    return retVal;
}

/** @brief Sets DMA Queue configuration type

    This API enables or selct MTL TX Queue for DMA transfer in specified mode.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_DMA_QMODE_PROC
    @code{.unparsed}
    Enable TXQ in normal mode or AVB mode
    @endcode
*/
int XGMAC_SetDMAQMode(XGMAC_HwIDType aId, XGMAC_DMAQModeType qMode, uint8_t txQIndex)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_MTL_TCQ0_RDBType *pTCQ = NULL;
    if(txQIndex > 7U) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[txQIndex];
        if (qMode == XGMAC_DMA_QMODE_ENABLE) {
            pTCQ->txq0_operation_mode &= ~XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TXQEN_MASK;
            pTCQ->txq0_operation_mode |= XGMAC_DMA_QMODE_ENABLE << XGMAC_MTL_TCQ0_MTL_TXQ0_OPERATION_MODE_TXQEN_SHIFT;
        } else if (qMode == XGMAC_DMA_QMODE_AVB) {
            retVal = BCM_ERR_NOSUPPORT;
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }
    return retVal;
}

/** @brief DMA initialize channel

    This API initialize DMA channel.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_INIT_CHANNEL_PROC
    @code{.unparsed}
    if(chanCfg->PBLx8) {
        pchanCfg->control |= XGMAC_DMA_CH0_DMA_CH0_CONTROL_PBLX8_MASK
    }
    if(chanCfg->SPH) {
        pchanCfg->control |= XGMAC_DMA_CH0_DMA_CH0_CONTROL_SPH_MASK
    }
    pchanCfg->control &= ~XGMAC_DMA_CH0_DMA_CH0_CONTROL_DSL_MASK
    pchanCfg->control |= (chanCfg->DSL << XGMAC_DMA_CH0_DMA_CH0_CONTROL_DSL_SHIFT)
    pchanCfg->interrupt_enable |= XGMAC_INTR_DMA_CHAN_DEFAULT_ENABLE
    @endcode
*/
int XGMAC_DMAInitChannel(XGMAC_HwIDType aId, XGMAC_DMAChannelCfgType *chanCfg, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 7UL) || (NULL == chanCfg)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];
        if(chanCfg->PBLx8 != 0U) {
            pchanCfg->control |= XGMAC_DMA_CH0_DMA_CH0_CONTROL_PBLX8_MASK;
        }
        if(chanCfg->SPH != 0U) {
            pchanCfg->control |= XGMAC_DMA_CH0_DMA_CH0_CONTROL_SPH_MASK;
        }
        /* Configure Descriptor Skip Length */
        pchanCfg->control &= ~XGMAC_DMA_CH0_DMA_CH0_CONTROL_DSL_MASK;
        pchanCfg->control |= (chanCfg->DSL << XGMAC_DMA_CH0_DMA_CH0_CONTROL_DSL_SHIFT);
        /* Enable the default DMA channel interrupt */
        pchanCfg->interrupt_enable |= XGMAC_INTR_DMA_CHAN_DEFAULT_ENABLE;
    }
    return retVal;
}

/** @brief DMA initialize transmit channel

    This API initialize DMA transmit channel.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_INIT_TX_CHAN_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->tx_control &= ~XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_TXPBL_MASK
    pchanCfg->tx_control |= (chanTxCtrl->txPBL << XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_TXPBL_SHIFT)
    @endcode
*/
int XGMAC_DMAInitTxChan(XGMAC_HwIDType aId, XGMAC_DMAChannelTxCtrlType *chanTxCtrl, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 7UL)  || (NULL == chanTxCtrl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];
        pchanCfg->tx_control &= ~XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_TXPBL_MASK;
        pchanCfg->tx_control |= (chanTxCtrl->txPBL << XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_TXPBL_SHIFT);
    }
    return retVal;
}

/** @brief DMA initialize receive channel
    This API initialize DMA receive channel.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_INIT_RX_CHAN_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RXPBL_MASK
    pchanCfg->rx_control |= (chanRxCtrl->rxPBL << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RXPBL_SHIFT)
    if(chanRxCtrl->rxDMAFlush) {
        pchanCfg->rx_control |= XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RPF_MASK
    }
    pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_MASK
    pchanCfg->rx_control |= (chanRxCtrl->recBuffSize << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_SHIFT)
    @endcode
*/
int XGMAC_DMAInitRxChan(XGMAC_HwIDType aId, XGMAC_DMAChannelRxCtrlType *chanRxCtrl, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 1UL) || (NULL == chanRxCtrl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];
        pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RXPBL_MASK;
        pchanCfg->rx_control |= (chanRxCtrl->rxPBL << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RXPBL_SHIFT);
        if(chanRxCtrl->rxDMAFlush != 0U) {
            pchanCfg->rx_control |= XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RPF_MASK;
        }
        pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_MASK;
        pchanCfg->rx_control |= (chanRxCtrl->recBuffSize << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_SHIFT);
    }
    return retVal;
}

/** @brief Sets DMA TX fifo ring length

    This API configures the DMA TX fifo ring length

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_TX_RING_LEN_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->tx_control2 &= ~XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL2_TDRL_MASK
    pchanCfg->tx_control2 |= (len << XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL2_TDRL_SHIFT)
    @endcode
*/
int XGMAC_DMASetTxRingLen(XGMAC_HwIDType aId, uint32_t len, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if(chan > 7UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        pchanCfg->tx_control2 &= ~XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL2_TDRL_MASK;
        pchanCfg->tx_control2 |= (len << XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL2_TDRL_SHIFT);
    }
    return retVal;
}

/** @brief Sets DMA RX fifo ring length

    This API configures the DMA RX fifo ring length

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_RX_RING_LEN_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->rx_control2 &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL2_RDRL_MASK
    pchanCfg->rx_control2 |= (len << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL2_RDRL_SHIFT)
    @endcode
*/
int XGMAC_DMASetRxRingLen(XGMAC_HwIDType aId, uint32_t len, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if(chan > 1UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        pchanCfg->rx_control2 &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL2_RDRL_MASK;
        pchanCfg->rx_control2 |= (len << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL2_RDRL_SHIFT);
    }
    return retVal;
}

/** @brief Sets DMA RX descriptor tail pointer

    This API configures the receive descriptor tail pointer.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_RX_TAILPTR_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->rxdesc_tail_lpointer &= ~XGMAC_DMA_CH0_DMA_CH0_RXDESC_TAIL_LPOINTER_RDT_MASK
    pchanCfg->rxdesc_tail_lpointer |= (tail_ptr << XGMAC_DMA_CH0_DMA_CH0_RXDESC_TAIL_LPOINTER_RDT_SHIFT)
    @endcode
*/
int XGMAC_DMASetRxTailPtr(XGMAC_HwIDType aId, uint32_t tailPtr, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 1UL) || ((tailPtr & 0x3UL) > 0UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        pchanCfg->rxdesc_tail_lpointer &= ~XGMAC_DMA_CH0_DMA_CH0_RXDESC_TAIL_LPOINTER_RDT_MASK;
        pchanCfg->rxdesc_tail_lpointer |= (tailPtr);
    }
    return retVal;
}

/** @brief Sets DMA TX descriptor tail pointer

    This API configures the transmit descriptor tail pointer.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_TX_TAILPTR_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->txdesc_tail_lpointer &= ~XGMAC_DMA_CH0_DMA_CH0_TXDESC_TAIL_LPOINTER_TDT_MASK
    pchanCfg->txdesc_tail_lpointer |= (tail_ptr << XGMAC_DMA_CH0_DMA_CH0_TXDESC_TAIL_LPOINTER_TDT_SHIFT)
    @endcode
*/
int XGMAC_DMASetTxTailPtr(XGMAC_HwIDType aId, uint32_t tailPtr, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 7UL) || ((tailPtr & 0x3UL) > 0UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        pchanCfg->txdesc_tail_lpointer &= ~XGMAC_DMA_CH0_DMA_CH0_TXDESC_TAIL_LPOINTER_TDT_MASK;
        pchanCfg->txdesc_tail_lpointer |= (tailPtr);
    }
    return retVal;
}

/** @brief Sets DMA RX descriptor Base address pointer

    This API configures RX descriptor base address pointer.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_RX_DESC_LIST_ADDR_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->rxdesc_list_laddress &= ~XGMAC_DMA_CH0_DMA_CH0_RXDESC_LIST_LADDRESS_RDESLA_MASK
    pchanCfg->rxdesc_list_laddress |= (listAddrPtr << XGMAC_DMA_CH0_DMA_CH0_RXDESC_LIST_LADDRESS_RDESLA_SHIFT)
    @endcode
*/
int XGMAC_DMASetRxDescListAddr(XGMAC_HwIDType aId, uint32_t listAddrPtr, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 1UL) || ((listAddrPtr & 0x3UL) > 0UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        pchanCfg->rxdesc_list_laddress &= ~XGMAC_DMA_CH0_DMA_CH0_RXDESC_LIST_LADDRESS_RDESLA_MASK;
        pchanCfg->rxdesc_list_laddress |= (listAddrPtr);
    }
    return retVal;
}

/** @brief Sets DMA TX descriptor Base address pointer

    This API configures TX descriptor base address pointer.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_TX_DESC_LIST_ADDR_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->txdesc_list_laddress &= ~XGMAC_DMA_CH0_DMA_CH0_TXDESC_LIST_LADDRESS_TDESLA_MASK
    pchanCfg->txdesc_list_laddress |= (listAddrPtr << XGMAC_DMA_CH0_DMA_CH0_TXDESC_LIST_LADDRESS_TDESLA_SHIFT)
    @endcode
*/
int XGMAC_DMASetTxDescListAddr(XGMAC_HwIDType aId, uint32_t listAddrPtr, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if((chan > 7UL) || ((listAddrPtr & 0x3UL) > 0UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        pchanCfg->txdesc_list_laddress &= ~XGMAC_DMA_CH0_DMA_CH0_TXDESC_LIST_LADDRESS_TDESLA_MASK;
        pchanCfg->txdesc_list_laddress |= (listAddrPtr);
    }
    return retVal;
}

/** @brief Sets DMA Receive Buffer Size

    This API configures Receive buffer Size in bytes allocated by the software to store the packets
    the Rx DMA transfers to the host memory.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_SET_REC_BUFFER_SIZE_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_MASK
    pchanCfg->rx_control |= (buffSize << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_SHIFT)
    @endcode
*/
int XGMAC_DMASetRecBufferSize(XGMAC_HwIDType aId, uint32_t buffSize, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if(chan > 1UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];
        pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_MASK;
        pchanCfg->rx_control |= (buffSize << XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_RBSZ_SHIFT);
    }
    return retVal;
}


/** @brief Enable DMA transmit

    This API start/stop DMA tansfer.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_TRANSMIT_ENABLE_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    if(aEnable) {
        pchanCfg->tx_control |= XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_ST_MASK
    } else {
        pchanCfg->tx_control &= ~XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_ST_MASK
    }
    @endcode
*/
int XGMAC_DMATransmitEnable(XGMAC_HwIDType aId, int aEnable, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if(chan > 7UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        if(aEnable != 0) {
            pchanCfg->tx_control |= XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_ST_MASK;
        } else {
            pchanCfg->tx_control &= ~XGMAC_DMA_CH0_DMA_CH0_TX_CONTROL_ST_MASK;
        }
    }
    return retVal;
}

/** @brief Enable DMA receive

    This API start/stop DMA receive.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_RECEIVE_ENABLE_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    if(aEnable) {
        pchanCfg->rx_control |= XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_SR_MASK
    } else {
        pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_SR_MASK
    }
    @endcode
*/
int XGMAC_DMAReceiveEnable(XGMAC_HwIDType aId, int aEnable, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if(chan > 1UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        if(aEnable != 0) {
            pchanCfg->rx_control |= XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_SR_MASK;
        } else {
            pchanCfg->rx_control &= ~XGMAC_DMA_CH0_DMA_CH0_RX_CONTROL_SR_MASK;
        }
    }
    return retVal;
}

/** @brief Get the driver status

    This API gives driver status if it is initialized or not for core, MTL, DMA and PTP module.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_GET_DRIVER_STATUS_PROC
    @code{.unparsed}
        if(aId < XGMAC_CORE_MAX_HW_ID) {
        *driverStatus = XGMAC_RWDev[aId].driverStatus
    }
    @endcode
*/
int XGMAC_GetDriverStatus(XGMAC_HwIDType aId, XGMAC_DriverStatusType *driverStatus)
{
   if((aId < XGMAC_CORE_MAX_HW_ID) && (NULL != driverStatus)) {
       *driverStatus = XGMAC_RWDev[aId].driverStatus;
   }
   return BCM_ERR_OK;
}

/** @brief Get the DMA MTL TxQ and RxQ status

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_GET_PROC
    @code{.unparsed}
    Read the MTL queue interrupt status
    @endcode
*/
static void XGMAC_IntDMAMtlQueueStatusGet(XGMAC_HwIDType aId, XGMAC_DMAChannelStatusType *DMAStatus, uint32_t chan)
{
    XGMAC_MTL_TCQ0_RDBType *pTCQ = NULL;
    pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[chan];
    if(chan < 2UL) {
        if((pTCQ->q0_interrupt_status & XGMAC_MTL_TCQ0_MTL_Q0_INTERRUPT_STATUS_RXOVFIS_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_MTL_RXQ_FULL_INTR;
        }
    }
    if((pTCQ->q0_interrupt_status & XGMAC_MTL_TCQ0_MTL_Q0_INTERRUPT_STATUS_TXUNFIS_MASK) > 0UL) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_MTL_TXQ_UNDER_FLOW_INTR;
    }
}

/** @brief Get the DMA channel status

    This API DMA transfer status.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_GET_PROC
    @code{.unparsed}
     XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    *DMAStatus = 0
    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TI_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_INTR
    }
    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TPS_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_PROCESS_STOPPED
    }
    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TBU_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_BUFFER_UNAVAILABLE
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_RI_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RECEIVE_INTR
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_RBU_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RECEIVE_BUFFER_UNAVAILABLE
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_RPS_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RECEIVE_PROCESS_STOPPED
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_DDE_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_INTERRUPT_DDE
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_FBE_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_FATAL_BUS_ERROR
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_CDE_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_CTXT_DES_ERROR
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_AIS_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_ABNORMAL_INTR_SUMMARY
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_NIS_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_NORMAL_INTR_SUMMARY
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TEB_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TX_DMA_BUS_ERROR
    }

    if(pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_REB_MASK) {
        *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RX_DMA_BUS_ERROR
    }
    @endcode
*/
int XGMAC_DMAChannelStatusGet(XGMAC_HwIDType aId, XGMAC_DMAChannelStatusType *DMAStatus, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;

    if(chan > 7UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

        *DMAStatus = 0UL;
        XGMAC_IntDMAMtlQueueStatusGet(aId, DMAStatus, chan);
        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TI_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_INTR;
        }
        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TPS_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_PROCESS_STOPPED;
        }
        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TBU_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_BUFFER_UNAVAILABLE;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_RI_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RECEIVE_INTR;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_RBU_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RECEIVE_BUFFER_UNAVAILABLE;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_RPS_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RECEIVE_PROCESS_STOPPED;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_DDE_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_INTERRUPT_DDE;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_FBE_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_FATAL_BUS_ERROR;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_CDE_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_CTXT_DES_ERROR;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_AIS_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_ABNORMAL_INTR_SUMMARY;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_NIS_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_NORMAL_INTR_SUMMARY;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_TEB_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_TX_DMA_BUS_ERROR;
        }

        if((pchanCfg->status & XGMAC_DMA_CH0_DMA_CH0_STATUS_REB_MASK) > 0UL) {
            *DMAStatus |= XGMAC_DMA_CHANNEL_STATUS_RX_DMA_BUS_ERROR;
        }
    }
    return retVal;
}

/** @brief Clear the MTL queue status

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_CLEAR_PROC
    @code{.unparsed}
    Read the MTL queue interrupt status
    write 1 to corresponding bits to clear the status
    @endcode
*/
static void XGMAC_IntDMAMtlQueueStatusClear(XGMAC_HwIDType aId, XGMAC_DMAChannelStatusType DMAStatusClear, uint32_t chan)
{
    XGMAC_MTL_TCQ0_RDBType *pTCQ = NULL;
    pTCQ = (XGMAC_MTL_TCQ0_RDBType *)XGMAC_MTLTxQ[chan];
    if(chan < 2UL) {
        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_MTL_RXQ_FULL_INTR)  > 0UL) {
            pTCQ->q0_interrupt_status |= XGMAC_MTL_TCQ0_MTL_Q0_INTERRUPT_STATUS_RXOVFIS_MASK;
        }
    }
    if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_MTL_TXQ_UNDER_FLOW_INTR)  > 0UL) {
        pTCQ->q0_interrupt_status |= XGMAC_MTL_TCQ0_MTL_Q0_INTERRUPT_STATUS_TXUNFIS_MASK;
    }
}

/** @brief DMA channel status clear

    This API writes status bit to clear the status.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_DMA_CHANNEL_STATUS_CLEAR_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_INTR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TI_MASK
    }
    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_PROCESS_STOPPED) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TPS_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_BUFFER_UNAVAILABLE) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TBU_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RECEIVE_INTR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_RI_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RECEIVE_BUFFER_UNAVAILABLE) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_RBU_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RECEIVE_PROCESS_STOPPED) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_RPS_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_DDE_ERROR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_DDE_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_FATAL_BUS_ERROR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_FBE_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_CTXT_DES_ERROR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_CDE_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_ABNORMAL_INTR_SUMMARY) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_AIS_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_NORMAL_INTR_SUMMARY) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_NIS_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TX_DMA_BUS_ERROR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TEB_MASK
    }

    if(DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RX_DMA_BUS_ERROR) {
        pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_REB_MASK
    }
    @endcode
*/
int XGMAC_DMAChannelStatusClear(XGMAC_HwIDType aId, XGMAC_DMAChannelStatusType DMAStatusClear, uint32_t chan)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    if(chan > 7U) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];
        XGMAC_IntDMAMtlQueueStatusClear(aId, DMAStatusClear, chan);
        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_INTR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TI_MASK;
        }
        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_PROCESS_STOPPED) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TPS_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TRANSMIT_BUFFER_UNAVAILABLE) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TBU_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RECEIVE_INTR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_RI_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RECEIVE_BUFFER_UNAVAILABLE) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_RBU_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RECEIVE_PROCESS_STOPPED) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_RPS_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_DDE_ERROR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_DDE_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_FATAL_BUS_ERROR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_FBE_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_CTXT_DES_ERROR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_CDE_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_ABNORMAL_INTR_SUMMARY) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_AIS_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_NORMAL_INTR_SUMMARY) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_NIS_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_TX_DMA_BUS_ERROR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_TEB_MASK;
        }

        if((DMAStatusClear & XGMAC_DMA_CHANNEL_STATUS_RX_DMA_BUS_ERROR) > 0UL) {
            pchanCfg->status |= XGMAC_DMA_CH0_DMA_CH0_STATUS_REB_MASK;
        }
    }
    return retVal;
}

/* VLAN APIs */

/** @brief Enable the VLAN Tag

    This API enables the VLAN tag for selected VLAN type
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_VLAN_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_ENABLE_VLAN_TAG_PROC
    @code{.unparsed}

    if(vlanTagFltrEn) {
        XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_VTFE_MASK
    }
    if(vlanTag & XGMAC_VLAN_TAG_16BIT) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK
        }
    }
    if(vlanTag & XGMAC_VLAN_TAG_12BIT) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK
        }
    }
    if(vlanTag & XGMAC_VLAN_TAG_DIS_VLAN_TYPE_CHECK) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_DOVLTC_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_DOVLTC_MASK
        }
    }
    if(vlanTag & XGMAC_VLAN_TAG_EN_REC_SVLAN) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ERSVLM_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ERSVLM_MASK
        }
    }
    if(vlanTag & XGMAC_VLAN_TAG_EN_SVLAN) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ESVL_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ESVL_MASK
        }
    }
    @endcode
*/
int XGMAC_EnableVlanTag(XGMAC_HwIDType aId, XGMAC_VlanTagType vlanTag, int aEnable, int vlanTagFltrEn)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if(vlanTagFltrEn != 0) {
            XGMAC_CoreRegs[aId]->mac_packet_filter |= XGMAC_CORE_MAC_PACKET_FILTER_VTFE_MASK;
        }
        if((vlanTag & XGMAC_VLAN_TAG_16BIT) > 0UL) {
            if(aEnable > 0) {
                XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK;
            }
        }
        if((vlanTag & XGMAC_VLAN_TAG_12BIT) > 0UL) {
            if(aEnable > 0) {
                XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ETV_MASK;
            }
        }
        if((vlanTag & XGMAC_VLAN_TAG_DIS_VLAN_TYPE_CHECK) > 0UL) {
            if(aEnable > 0) {
                XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_DOVLTC_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_DOVLTC_MASK;
            }
        }
        if((vlanTag & XGMAC_VLAN_TAG_EN_REC_SVLAN) > 0UL) {
            if(aEnable > 0) {
                XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ERSVLM_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ERSVLM_MASK;
            }
        }
        if((vlanTag & XGMAC_VLAN_TAG_EN_SVLAN) > 0UL) {
            if(aEnable > 0) {
                XGMAC_CoreRegs[aId]->mac_vlan_tag |= XGMAC_CORE_MAC_VLAN_TAG_ESVL_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_ESVL_MASK;
            }
        }
    }

    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
    return retVal;
}

/** @brief Set the VLAN Tag

    This API configures the VLAN tag.
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_VLAN_CONFIG
    @trace  #BRCM_SWARCH_XGMAC_SET_VLAN_TAG_PROC
    @code{.unparsed}
    Set single VLAN tag
    XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_VID_MASK;
    XGMAC_CoreRegs[aId]->mac_vlan_tag |=  vlanTag;
    @endcode
*/
int XGMAC_SetVlanTag(XGMAC_HwIDType aId, uint16_t vlanTag, uint16_t innerVlanTag, uint32_t innerVlanType)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t line = __LINE__;
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Set single VLAN tag  */
        XGMAC_CoreRegs[aId]->mac_vlan_tag &= ~XGMAC_CORE_MAC_VLAN_TAG_VID_MASK;
        XGMAC_CoreRegs[aId]->mac_vlan_tag |=  vlanTag;
    }
    if(retVal != BCM_ERR_OK) {
        (void)line;
    }
    return retVal;
}

/** @brief Enable the MAC core interrupt

    This API enable the interrupt for the selected interrupt types
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_ENABLE_CORE_INTERRUPT_PROC
    @code{.unparsed}

    if(intrType == XGMAC_CORE_INTERRUPT_ALL) {
       if(aEnable) {
           XGMAC_CoreRegs[aId]->mac_interrupt_enable |= (XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK |
               XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK |
               XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK)
       } else {
           XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~(XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK |
               XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK | XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK |
               XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK)
       }
    } else if(intrType & XGMAC_CORE_INTERRUPT_LSIE) {
       if(aEnable) {
           XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK
       } else {
           XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK
       }
    } else if(intrType & XGMAC_CORE_INTERRUPT_PMTIE) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK
        }

    } else if(intrType & XGMAC_CORE_INTERRUPT_LPIE) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK
        }

    } else if(intrType & XGMAC_CORE_INTERRUPT_TSIE) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK
        }

    } else if(intrType & XGMAC_CORE_INTERRUPT_TXESIE) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK
        }

    } else if(intrType & XGMAC_CORE_INTERRUPT_RXESIE) {
        if(aEnable) {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK
        } else {
            XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK
        }

    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }
    @endcode
*/
int XGMAC_EnableCoreInterrupt(XGMAC_HwIDType aId, XGMAC_CoreInterruptType intrType, uint32_t aEnable)
{
     int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
     if (aId >= XGMAC_CORE_MAX_HW_ID) {
         retVal = BCM_ERR_INVAL_PARAMS;
     } else {
#endif
         if((intrType & XGMAC_CORE_INTERRUPT_LSIE) > 0UL) {
            if(aEnable > 0UL) {
                XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK;
            } else {
                XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_LSIE_MASK;
            }
         }

         if((intrType & XGMAC_CORE_INTERRUPT_PMTIE) > 0UL) {
             if(aEnable > 0UL) {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK;
             } else {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_PMTIE_MASK;
             }

         }

         if((intrType & XGMAC_CORE_INTERRUPT_LPIE) > 0UL) {
             if(aEnable > 0UL) {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK;
             } else {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_LPIIE_MASK;
             }

         }

         if((intrType & XGMAC_CORE_INTERRUPT_TSIE) > 0UL) {
             if(aEnable > 0UL) {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK;
             } else {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_TSIE_MASK;
             }

         }

         if((intrType & XGMAC_CORE_INTERRUPT_TXESIE) > 0UL) {
             if(aEnable > 0UL) {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK;
             } else {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_TXESIE_MASK;
             }
         }

         if((intrType & XGMAC_CORE_INTERRUPT_RXESIE) > 0UL) {
             if(aEnable > 0UL) {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable |= XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK;
             } else {
                 XGMAC_CoreRegs[aId]->mac_interrupt_enable &= ~XGMAC_CORE_MAC_INTERRUPT_ENABLE_RXESIE_MASK;
             }
         }
#ifdef BCM8915X_PARAM_VALIDATION
     }
#endif
     return retVal;
}

/**
    @brief @brief Enable the MAC DMA channel interrupt Group0

    @trace  #BRCM_SWARCH_XGMAC_ENABLE_DMA_CHANNEL_INTERRUPT_PROC
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

*/
static int XGMAC_IntEnableDMAChannelInterruptGrp0(XGMAC_HwIDType aId, XGMAC_DMAChannelInterruptType chanIntrType, uint32_t chan, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_TIE) > 0UL) {
       if(aEnable > 0UL) {
           pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK;
       } else {
           pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK;
       }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_TXSE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TXSE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TXSE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_TBUE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TBUE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TBUE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_RIE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK;
        }
    }
    return retVal ;
}

/**
    @brief Enable the MAC DMA channel interrupt Group1

    @trace  #BRCM_SWARCH_XGMAC_ENABLE_DMA_CHANNEL_INTERRUPT_PROC
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
static int XGMAC_IntEnableDMAChannelInterruptGrp1(XGMAC_HwIDType aId, XGMAC_DMAChannelInterruptType chanIntrType, uint32_t chan, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_RBUE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RBUE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RBUE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_RSE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RSE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RSE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_DDE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_DDEE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_DDEE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_FBE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK;
        }
    }
    return retVal ;
}

/**
    @brief Enable the MAC DMA channel interrupt Group2

    @trace  #BRCM_SWARCH_XGMAC_ENABLE_DMA_CHANNEL_INTERRUPT_PROC
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
*/
static int XGMAC_IntEnableDMAChannelInterruptGrp2(XGMAC_HwIDType aId, XGMAC_DMAChannelInterruptType chanIntrType, uint32_t chan, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    XGMAC_DMA_CH0_RDBType *pchanCfg = NULL;
    pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan];

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_CDE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_CDEE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_CDEE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_AIE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK;
        }
    }

    if((chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_NIE) > 0UL) {
        if(aEnable > 0UL) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK;
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK;
        }
    }
    return retVal ;
}

/** @brief Enable the MAC DMA channel interrupt

    This API enable the interrupt for the selected interrupt types
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT
    @trace  #BRCM_SWARCH_XGMAC_ENABLE_DMA_CHANNEL_INTERRUPT_PROC
    @code{.unparsed}
    XGMAC_DMA_CH0_RDBType *pchanCfg = (XGMAC_DMA_CH0_RDBType *)XGMAC_DMAChanCfg[chan]
    if(chanIntrType == XGMAC_DMA_CHANNEL_INTERRUPT_ALL) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= (XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TXSE_MASK |
                XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TBUE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RBUE_MASK |
                XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RSE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_DDEE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK |
                XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_CDEE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK)
        } else {
            pchanCfg->interrupt_enable &= ~(XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TXSE_MASK |
                XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TBUE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RBUE_MASK |
                XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RSE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_DDEE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK |
                XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_CDEE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK | XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK)
        }
    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_TIE) {
       if(aEnable) {
           pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK
       } else {
           pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TIE_MASK
       }
    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_TXSE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TXSE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TXSE_MASK
        }
    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_TBUE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TBUE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_TBUE_MASK
        }
    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_RIE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RIE_MASK
        }
    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_RBUE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RBUE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RBUE_MASK
        }

    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_RSE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RSE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_RSE_MASK
        }

    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_DDE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_DDEE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_DDEE_MASK
        }

    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_FBE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_FBEE_MASK
        }

    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_CDE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_CDEE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_CDEE_MASK
        }

    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_AIE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_AIE_MASK
        }

    } else if(chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_NIE) {
        if(aEnable) {
            pchanCfg->interrupt_enable |= XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK
        } else {
            pchanCfg->interrupt_enable &= ~XGMAC_DMA_CH0_DMA_CH0_INTERRUPT_ENABLE_NIE_MASK
        }

    } else {
        retVal = BCM_ERR_NOSUPPORT
    }
    @endcode
*/
int XGMAC_EnableDMAChannelInterrupt(XGMAC_HwIDType aId, XGMAC_DMAChannelInterruptType chanIntrType, uint32_t chan, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if(chan > 7U) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_GRP0)) {
            retVal = XGMAC_IntEnableDMAChannelInterruptGrp0(aId, chanIntrType, chan, aEnable);
        }

        if((BCM_ERR_OK == retVal) && (0UL != (chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_GRP1))) {
            retVal = XGMAC_IntEnableDMAChannelInterruptGrp1(aId, chanIntrType, chan, aEnable);
        }

        if((BCM_ERR_OK == retVal) && (0UL != (chanIntrType & XGMAC_DMA_CHANNEL_INTERRUPT_GRP2))) {
            retVal = XGMAC_IntEnableDMAChannelInterruptGrp2(aId, chanIntrType, chan, aEnable);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Clear the MAC core interrupt

    This API clear the interrupt for the selected interrupt types
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_CLEAR_CORE_INTERRUPT_PROC
    @code{.unparsed}
    if(aId < XGMAC_CORE_MAX_HW_ID && XGMAC_CoreRegs[aId]->mac_csr_sw_ctrl & XGMAC_CORE_MAC_CSR_SW_CTRL_RCWE_MASK != 0) {
        if(intrType ==  XGMAC_CORE_INTERRUPT_STATUS_ALL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status = XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_LSIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_SMIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_PMTIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_LPIIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_TSIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_TXESIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_RXESIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK
        } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_GPIIS) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK
        }  else {
            retVal = BCM_ERR_NOSUPPORT
        }
    }else {
        retVal = BCM_ERR_INVAL_PARAMS
    }
    @endcode
*/
int XGMAC_ClearCoreInterrupt(XGMAC_HwIDType aId, XGMAC_CoreInterruptStatusType intrType)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t macCsrCtrl = (XGMAC_CoreRegs[aId]->mac_csr_sw_ctrl & XGMAC_CORE_MAC_CSR_SW_CTRL_RCWE_MASK);
    if((aId < XGMAC_CORE_MAX_HW_ID) && (macCsrCtrl > 0UL)) {
        if(intrType ==  XGMAC_CORE_INTERRUPT_STATUS_ALL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status = XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK |
            XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK | XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_LSIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_SMIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_PMTIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_LPIIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_TSIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_TXESIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_RXESIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_GPIIS) > 0UL) {
            XGMAC_CoreRegs[aId]->mac_interrupt_status |= XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK;
        }  else {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/** @brief Get the MAC core interrupt status

    This API Get the interrupt status for the selected interrupt types
    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_GET_CORE_INTERRUPT_STATUS_PROC
    @code{.unparsed}
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS

    }
    status = XGMAC_CoreRegs[aId]->mac_interrupt_status
    if(intrType ==  XGMAC_CORE_INTERRUPT_STATUS_ALL) {
        *intrStatus = ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_LSIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_SMIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_PMTIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_LPIIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_TSIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_TXESIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_RXESIS : 0U) |
            ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_GPIIS : 0U)
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_LSIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_LSIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_SMIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_SMIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_PMTIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_PMTIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_LPIIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_LPIIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_TSIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_TSIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_TXESIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_TXESIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_RXESIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_RXESIS : 0U
    } else if(intrType & XGMAC_CORE_INTERRUPT_STATUS_GPIIS) {
        *intrStatus |= (status & XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK) ? XGMAC_CORE_INTERRUPT_STATUS_GPIIS : 0U
    }  else {
        retVal = BCM_ERR_NOSUPPORT
    }
    @endcode
*/
int XGMAC_GetCoreInterruptStatus(XGMAC_HwIDType aId, XGMAC_CoreInterruptStatusType intrType, uint32_t *intrStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t status;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        status = XGMAC_CoreRegs[aId]->mac_interrupt_status;
        if(intrType ==  XGMAC_CORE_INTERRUPT_STATUS_ALL) {
            *intrStatus = (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_LSIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_SMIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_PMTIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_LPIIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_TSIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_TXESIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_RXESIS : 0UL) |
                (((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_GPIIS : 0UL);
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_LSIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LSI_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_LSIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_SMIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_SMI_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_SMIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_PMTIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_PMTIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_PMTIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_LPIIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LPIIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_LPIIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCRXIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_MMCRXIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_MMCTXIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_MMCTXIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_TSIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TSIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_TSIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_TXESIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_TXESIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_TXESIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_RXESIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_RXESIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_RXESIS : 0UL;
        } else if((intrType & XGMAC_CORE_INTERRUPT_STATUS_GPIIS) > 0UL) {
            *intrStatus |= ((status & XGMAC_CORE_MAC_INTERRUPT_STATUS_GPIIS_MASK) > 0UL) ? XGMAC_CORE_INTERRUPT_STATUS_GPIIS : 0UL;
        }  else {
            retVal = BCM_ERR_NOSUPPORT;
        }
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}


/** @brief Get the MAC Link status

    This API gets the Link status received on the XGMII receive interface.

    @trace  #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @trace  #BRCM_SWARCH_XGMAC_GET_LINK_STATUS_PROC
    @code{.unparsed}
    int32_t retVal = BCM_ERR_OK
    uint32_t link_status
    if (aId >= XGMAC_CORE_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS
    }
    link_status = ((XGMAC_CoreRegs[aId]->mac_interrupt_status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LS_MASK) >> XGMAC_CORE_MAC_INTERRUPT_STATUS_LS_SHIFT)
    if(link_status == XGMAC_LINK_STATUS_OK) {
        *linkStatus = XGMAC_LINK_STATUS_OK
    } else if (link_status == XGMAC_LINK_STATUS_LOCAL_FAULT) {
        *linkStatus = XGMAC_LINK_STATUS_LOCAL_FAULT
    } else if (link_status == XGMAC_LINK_STATUS_REMOTE_FAULT) {
        *linkStatus = XGMAC_LINK_STATUS_REMOTE_FAULT
    } else {
        retVal = BCM_ERR_INVAL_STATE
    }
    @endcode
*/
int XGMAC_GetLinkStatus(XGMAC_HwIDType aId, XGMAC_LinkStatusType *linkStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t link_status = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= XGMAC_CORE_MAX_HW_ID) || (NULL == linkStatus)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        link_status = ((XGMAC_CoreRegs[aId]->mac_interrupt_status & XGMAC_CORE_MAC_INTERRUPT_STATUS_LS_MASK) >> XGMAC_CORE_MAC_INTERRUPT_STATUS_LS_SHIFT);
        if(link_status == XGMAC_LINK_STATUS_OK) {
            *linkStatus = XGMAC_LINK_STATUS_OK;
        } else if (link_status == XGMAC_LINK_STATUS_LOCAL_FAULT) {
            *linkStatus = XGMAC_LINK_STATUS_LOCAL_FAULT;
        } else if (link_status == XGMAC_LINK_STATUS_REMOTE_FAULT) {
            *linkStatus = XGMAC_LINK_STATUS_REMOTE_FAULT;
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

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

    @trace  #BRCM_SWARCH_XGMAC_GET_MIB_COUNTER_STATS_PROC
    @trace  #BRCM_SWREQ_XGMAC_MIB_COUNTER_STATUS

    @limitations None
    @code{.unparsed}
    Read MIB registers to get transmit and receive packet counters.
    @endcode
*/
int XGMAC_GetMIBCounterStats(XGMAC_HwIDType aId, XGMAC_MIBCounterType *mibStats)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= XGMAC_CORE_MAX_HW_ID) || (NULL == mibStats)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((XGMAC_RWDev[aId].driverStatus & XGMAC_DRIVER_STATUS_CORE_INITIALIZED) == 0UL) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        mibStats->RXOCTGLO = XGMAC_CoreRegs[aId]->rx_octet_count_good_low;
        mibStats->RXOCTGHI = XGMAC_CoreRegs[aId]->rx_octet_count_good_high;
        mibStats->RXBCASTGLO = XGMAC_CoreRegs[aId]->rx_broadcast_packets_good_low;
        mibStats->RXMCASTGLO = XGMAC_CoreRegs[aId]->rx_multicast_packets_good_low;
        mibStats->RXCRCERLO  =  XGMAC_CoreRegs[aId]->rx_crc_error_packets_low;
        mibStats->RX64OCTGBLO = XGMAC_CoreRegs[aId]->rx_64octets_packets_good_bad_low;
        mibStats->RX65TO127OCTGBLO = XGMAC_CoreRegs[aId]->rx_65to127octets_packets_good_bad_low;
        mibStats->RX128TO255OCTGBLO = XGMAC_CoreRegs[aId]->rx_128to255octets_packets_good_bad_low;
        mibStats->RX256TO511OCTGBLO = XGMAC_CoreRegs[aId]->rx_256to511octets_packets_good_bad_low;
        mibStats->RX512TO1023OCTGBLO = XGMAC_CoreRegs[aId]->rx_512to1023octets_packets_good_bad_low;
        mibStats->RXUCASTGLO = XGMAC_CoreRegs[aId]->rx_unicast_packets_good_low;
        mibStats->RXPAUSELO = XGMAC_CoreRegs[aId]->rx_pause_packets_low;
        mibStats->RXVLANGBLO = XGMAC_CoreRegs[aId]->rx_vlan_packets_good_bad_low;
        mibStats->TXPKTGBLO = XGMAC_CoreRegs[aId]->tx_packet_count_good_bad_low;
        mibStats->TXPKTGLO = XGMAC_CoreRegs[aId]->tx_packet_count_good_low;
        mibStats->TXBCASTGLO = XGMAC_CoreRegs[aId]->tx_broadcast_packets_good_low;
        mibStats->TXMCASTGLO = XGMAC_CoreRegs[aId]->tx_multicast_packets_good_low;
        mibStats->TX64OCTGBLO = XGMAC_CoreRegs[aId]->tx_64octets_packets_good_bad_low;
        mibStats->TX65TO127OCTGBLO = XGMAC_CoreRegs[aId]->tx_65to127octets_packets_good_bad_low;
        mibStats->TX128TO255OCTGBLO = XGMAC_CoreRegs[aId]->tx_128to255octets_packets_good_bad_low;
        mibStats->TX512TO1023OCTGBLO = XGMAC_CoreRegs[aId]->tx_512to1023octets_packets_good_bad_low;
        mibStats->TX1024TOMAXOCTGBLO = XGMAC_CoreRegs[aId]->tx_1024tomaxoctets_packets_good_bad_low;
        mibStats->TXOCTGLO = XGMAC_CoreRegs[aId]->tx_octet_count_good_low;
        mibStats->TXOCTGHI = XGMAC_CoreRegs[aId]->tx_octet_count_good_high;
        mibStats->TXPAUSEGLO = XGMAC_CoreRegs[aId]->tx_pause_packets_low;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @} */

