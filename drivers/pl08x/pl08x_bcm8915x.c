/*****************************************************************************
  Copyright 2024 Broadcom Limited.  All rights reserved.
  Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_dma_bcm8958x_impl BCM8958x Platform Implementation
    @ingroup grp_dma_v2

    @addtogroup grp_dma_bcm8958x_impl
    @{

    @file dma_bcm8958x.c
    @brief DMA platform layer implementation for BCM8958x

    This source file contains the implementation of platform
    layer functions

    @version 0.1 Initial version
*/

#include <dma_v2.h>
#include <bcm_utils.h>

#include <bcm_err.h>
#include "pl08x_plat.h"
#include "pl08x_rdb.h"

/**
    @name DMA platform layer Design IDs
    @{
    @brief Design IDs for DMA platform layer
*/
#define BRCM_SWDSGN_DMA_QSPI0_RECEIVE_FIFO_MACRO        (0xB200U) /**< @brief #DMA_QSPI0_RECEIVE_ADDRESS  */
#define BRCM_SWDSGN_DMA_QSPI1_RECEIVE_FIFO_MACRO        (0xB201U) /**< @brief #DMA_QSPI1_RECEIVE_ADDRESS  */
#define BRCM_SWDSGN_DMA_QSPI2_RECEIVE_FIFO_MACRO        (0xB202U) /**< @brief #DMA_QSPI2_RECEIVE_ADDRESS  */
#define BRCM_SWDSGN_DMA_QSPI0_TRANSMIT_FIFO_MACRO       (0xB203U) /**< @brief #DMA_QSPI0_TRANSMIT_ADDRESS */
#define BRCM_SWDSGN_DMA_QSPI1_TRANSMIT_FIFO_MACRO       (0xB204U) /**< @brief #DMA_QSPI1_TRANSMIT_ADDRESS */
#define BRCM_SWDSGN_DMA_QSPI2_TRANSMIT_FIFO_MACRO       (0xB205U) /**< @brief #DMA_QSPI2_TRANSMIT_ADDRESS */

#define BRCM_SWDSGN_DMA_UART0_RECEIVE_FIFO_MACRO        (0xB206U) /**< @brief #DMA_UART0_RECEIVE_ADDRESS  */
#define BRCM_SWDSGN_DMA_UART1_RECEIVE_FIFO_MACRO        (0xB207U) /**< @brief #DMA_UART1_RECEIVE_ADDRESS  */
#define BRCM_SWDSGN_DMA_UART0_TRANSMIT_FIFO_MACRO       (0xB208U) /**< @brief #DMA_UART0_TRANSMIT_ADDRESS */
#define BRCM_SWDSGN_DMA_UART1_TRANSMIT_FIFO_MACRO       (0xB209U) /**< @brief #DMA_UART1_TRANSMIT_ADDRESS */

#define BRCM_SWDSGN_DMA_FLASH_TRANSMIT_FIFO_MACRO       (0xB20AU) /**< @brief #DMA_FLASH_TRANSMIT_ADDRESS */
#define BRCM_SWDSGN_DMA_FLASH_RECEIVE_FIFO_MACRO        (0xB20BU) /**< @brief #DMA_FLASH_RECEIVE_ADDRESS  */

#define BRCM_SWDSGN_DMA_PLAT_GET_CFG_PROC               (0xB20EU) /**< @brief #DMA_PlatGetCfg             */
#define BRCM_SWDSGN_DMA_PLAT_GET_CHANNEL_MASK_PROC      (0xB20FU) /**< @brief #DMA_PlatGetChannelMask     */
#define BRCM_SWDSGN_DMA_INT_FILLPLAT_CFG0_PROC          (0xB210U) /**< @brief #DMA_IntFillPlatcfg0     */
#define BRCM_SWDSGN_DMA_INT_FILLPLAT_CFG1_PROC          (0xB211U) /**< @brief #DMA_IntFillPlatcfg1     */
#define BRCM_SWDSGN_DMA_INT_FILLPLAT_CFG1_HKADC_PROC    (0xB212U) /**< @brief #DMA_IntFillPlatcfg1Hkdac*/
#define BRCM_SWDSGN_DMA_MEM_XFER_WIDTH_INCMODE_PROC     (0xB213U) /**< @brief #DMA_GetMemXferWidthAndIncMode    */

/** @} */

/**
    @{
    @brief Base Address

    @trace #BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC
    @trace #BRCM_SWARCH_DMA_PLAT_GET_CHMASK_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/

#define BCM8915x_DMA_BASE_ADDRESS    0x10000000UL
#define DMA_QSPI0_RECEIVE_ADDRESS    (BCM8915x_DMA_BASE_ADDRESS | 0x00246030UL)
#define DMA_QSPI1_RECEIVE_ADDRESS    (BCM8915x_DMA_BASE_ADDRESS | 0x00247030UL)
#define DMA_QSPI2_RECEIVE_ADDRESS    (BCM8915x_DMA_BASE_ADDRESS | 0x00248030UL)
#define DMA_QSPI0_TRANSMIT_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00246030UL)
#define DMA_QSPI1_TRANSMIT_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00247030UL)
#define DMA_QSPI2_TRANSMIT_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00248030UL)
#define DMA_UART0_RECEIVE_ADDRESS    (BCM8915x_DMA_BASE_ADDRESS | 0x00242000UL)
#define DMA_UART1_RECEIVE_ADDRESS    (BCM8915x_DMA_BASE_ADDRESS | 0x00243000UL)
#define DMA_UART0_TRANSMIT_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00242000UL)
#define DMA_UART1_TRANSMIT_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00243000UL)
#define DMA_FLASH_RECEIVE_ADDRESS    (BCM8915x_DMA_BASE_ADDRESS | 0x0024b030UL)
#define DMA_FLASH_TRANSMIT_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x0024b030UL)

#define DMA_HKADC_0_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608004UL)
#define DMA_HKADC_1_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608084UL)
#define DMA_HKADC_2_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608104UL)
#define DMA_HKADC_3_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608184UL)
#define DMA_HKADC_4_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608204UL)
#define DMA_HKADC_5_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608284UL)
#define DMA_HKADC_6_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608304UL)
#define DMA_HKADC_7_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00608384UL)

#define DMA_HKDAC_0_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00609004UL)
#define DMA_HKDAC_1_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00609044UL)
#define DMA_HKDAC_2_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00609084UL)
#define DMA_HKDAC_3_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x006090C4UL)
#define DMA_HKDAC_4_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00609104UL)
#define DMA_HKDAC_5_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00609144UL)
#define DMA_HKDAC_6_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x00609184UL)
#define DMA_HKDAC_7_ADDRESS   (BCM8915x_DMA_BASE_ADDRESS | 0x006091C4UL)


/**
    @code{.unparsed}
    ret = BCM_ERR_OK

    if (1UL == aRowSize)
        *aProgXferWidth = DMA_WORD_SIZE_8
    else if (2UL == aRowSize)
        *aProgXferWidth = DMA_WORD_SIZE_16
    else if (4UL == aRowSize)
        *aProgXferWidth = DMA_WORD_SIZE_32
    else
        ret = BCM_ERR_NOSUPPORT

    if (aRowSize == aAddrInc)
        *aProgIncMode = 1UL
    else if (0UL == aAddrInc)
        *aProgIncMode = 0UL
    else
        ret = BCM_ERR_NOSUPPORT

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
int32_t DMA_GetMemXferWidthAndIncMode(uint32_t aRowSize, uint32_t aAddrInc, uint32_t *aProgXferWidth, uint32_t *aProgIncMode)
{
    int32_t ret = BCM_ERR_OK;

    if (1UL == aRowSize) {
        *aProgXferWidth = DMA_WORD_SIZE_8;
    } else if (2UL == aRowSize) {
        *aProgXferWidth = DMA_WORD_SIZE_16;
    } else if (4UL == aRowSize) {
        *aProgXferWidth = DMA_WORD_SIZE_32;
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }

    if (aRowSize == aAddrInc) {
        *aProgIncMode = 1UL;
    } else if (0UL == aAddrInc) {
        *aProgIncMode = 0UL;
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }

    return ret;
}

/**
    @brief Fill platform config for HW instance 0
    @trace #BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X

*/
static int32_t DMA_IntFillPlatcfg0 (uint32_t aIsSrc,
                       uint32_t aAddr, uint32_t aRowSize, uint32_t aAddrInc,
                       DMA_PlatCfgType *aPlatCfg)
{
    int32_t ret = BCM_ERR_OK;
    if (DMA_QSPI0_RECEIVE_ADDRESS  == aAddr) {
        /* QSPI0 Receive Address*/
        aPlatCfg->isPeriph = true;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = (aIsSrc == 1UL) ? 4UL : 5UL;
        aPlatCfg->progBaseAddr = DMA_QSPI0_RECEIVE_ADDRESS;
        aPlatCfg->progIncMode = 0UL;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_32;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_QSPI1_RECEIVE_ADDRESS  == aAddr) {
        /* QSPI1 Receive Address*/
        aPlatCfg->isPeriph = true;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = (aIsSrc == 1UL) ? 6UL : 7UL;
        aPlatCfg->progBaseAddr = DMA_QSPI1_RECEIVE_ADDRESS;
        aPlatCfg->progIncMode = 0UL;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_32;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_QSPI2_RECEIVE_ADDRESS  == aAddr) {
        /* QSPI2 Receive Address*/
        aPlatCfg->isPeriph = true;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = (aIsSrc == 1UL) ? 8UL : 9UL;
        aPlatCfg->progBaseAddr = DMA_QSPI2_RECEIVE_ADDRESS;
        aPlatCfg->progIncMode = 0UL;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_32;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_UART0_RECEIVE_ADDRESS  == aAddr) {
        /* UART Transmit/Receive Address*/
        aPlatCfg->isPeriph = true;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = (aIsSrc == 1UL) ? 0UL : 1UL;
        aPlatCfg->progBaseAddr = DMA_UART0_RECEIVE_ADDRESS;
        aPlatCfg->progIncMode = 0UL;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_UART1_RECEIVE_ADDRESS == aAddr) {
        /* UART1 Receive Address*/
        aPlatCfg->isPeriph = true;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = (aIsSrc == 1UL) ? 2UL : 3UL;
        aPlatCfg->progBaseAddr = DMA_UART1_RECEIVE_ADDRESS;
        aPlatCfg->progIncMode = 0UL;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_FLASH_RECEIVE_ADDRESS == aAddr) {
        /* Flash Receive/Transmit Address*/
        aPlatCfg->isPeriph = true;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = (aIsSrc == 1UL) ? 10UL : 11UL;
        aPlatCfg->progBaseAddr = DMA_FLASH_RECEIVE_ADDRESS;
        aPlatCfg->progIncMode = 0UL;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_32;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (BCM_ERR_OK == DMA_GetMemXferWidthAndIncMode(aRowSize, aAddrInc, &aPlatCfg->progXferWidth, &aPlatCfg->progIncMode)) {
        /* Memory */
        aPlatCfg->isPeriph = false;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = 0UL;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }
    aPlatCfg->xferWidth = (1UL << aPlatCfg->progXferWidth);

    return ret;
}

/**
    @brief Fill platform config for HW instance 1 HKDAC
    @trace #BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X

*/
static int32_t DMA_IntFillPlatcfg1Hkdac (uint32_t aIsSrc,
                       uint32_t aAddr, uint32_t aRowSize, uint32_t aAddrInc,
                       DMA_PlatCfgType *aPlatCfg)
{
    int32_t ret = BCM_ERR_OK;
    if (DMA_HKDAC_0_ADDRESS == aAddr) {
        aPlatCfg->periphId = 8;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKDAC_1_ADDRESS == aAddr) {
        aPlatCfg->periphId = 9;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKDAC_2_ADDRESS == aAddr) {
        aPlatCfg->periphId = 10;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKDAC_3_ADDRESS  == aAddr) {
        aPlatCfg->periphId = 11;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKDAC_4_ADDRESS == aAddr) {
        aPlatCfg->periphId = 12;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKDAC_5_ADDRESS == aAddr) {
        aPlatCfg->periphId = 13;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    }  else if (DMA_HKDAC_6_ADDRESS == aAddr) {
        aPlatCfg->periphId = 14;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    }  else if (DMA_HKDAC_7_ADDRESS == aAddr) {
        aPlatCfg->periphId = 15;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Fill platform config for HW instance 1
    @trace #BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X

*/
static int32_t DMA_IntFillPlatcfg1 (uint32_t aIsSrc,
                       uint32_t aAddr, uint32_t aRowSize, uint32_t aAddrInc,
                       DMA_PlatCfgType *aPlatCfg)
{
    int32_t ret = BCM_ERR_OK;

    aPlatCfg->isPeriph = true;
    aPlatCfg->isFlowCtrlr = false;
    aPlatCfg->progIncMode = 0UL;

    if (DMA_HKADC_0_ADDRESS == aAddr) {
        aPlatCfg->periphId = 0;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKADC_1_ADDRESS == aAddr) {
        aPlatCfg->periphId = 1;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKADC_2_ADDRESS == aAddr) {
        aPlatCfg->periphId = 2;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKADC_3_ADDRESS  == aAddr) {
        aPlatCfg->periphId = 3;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKADC_4_ADDRESS == aAddr) {
        aPlatCfg->periphId = 4;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (DMA_HKADC_5_ADDRESS == aAddr) {
        aPlatCfg->periphId = 5;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    }  else if (DMA_HKADC_6_ADDRESS == aAddr) {
        aPlatCfg->periphId = 6;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    }  else if (DMA_HKADC_7_ADDRESS == aAddr) {
        aPlatCfg->periphId = 7;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progXferWidth = DMA_WORD_SIZE_8;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else if (aAddr >= DMA_HKDAC_0_ADDRESS && aAddr <= DMA_HKDAC_7_ADDRESS) {
        ret = DMA_IntFillPlatcfg1Hkdac (aIsSrc, aAddr, aRowSize, aAddrInc, aPlatCfg);
    } else if (BCM_ERR_OK == DMA_GetMemXferWidthAndIncMode(aRowSize, aAddrInc, &aPlatCfg->progXferWidth, &aPlatCfg->progIncMode)) {
        aPlatCfg->isPeriph = false;
        aPlatCfg->isFlowCtrlr = false;
        aPlatCfg->periphId = 0UL;
        aPlatCfg->progBaseAddr = aAddr;
        aPlatCfg->progBurstSize = DMA_BURST_SIZE_16;
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }
    aPlatCfg->xferWidth = (1UL << aPlatCfg->progXferWidth);

    return ret;
}

/** Get Platform configuration based on the base address
    @param[in] aHwId              Hardware ID
    @param[in] aIssuerId          Reserved
    @param[in] aIsSrc             Reserved
    @param[in] aAddr              Base address
    @param[in] aRowSize           Row Size
    @param[in] aAddrInc           Address Increment mode
    @param[out] aPlatCfg          plafform coniguration

    @retval     #BCM_ERR_OK         The input configs are compatible
    @retval     #BCM_ERR_NOSUPPORT  The input configs are incompatible


    @code{.unparsed}
    for HW instance 0
     call fill platform config 0
    for HW instance 1
     call fill platform config 1
    @endcode

    @trace #BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
int32_t DMA_PlatGetCfg(uint32_t aHwId,
                       DMA_XferIssuerIdType const* aIssuerId,
                       uint32_t aIsSrc,
                       uint32_t aAddr, uint32_t aRowSize, uint32_t aAddrInc,
                       DMA_PlatCfgType *aPlatCfg)
{
    int32_t ret = BCM_ERR_OK;

    if (0UL == aHwId) {
        ret = DMA_IntFillPlatcfg0 (aIsSrc, aAddr, aRowSize, aAddrInc, aPlatCfg);
    } else {
        ret = DMA_IntFillPlatcfg1 (aIsSrc, aAddr, aRowSize, aAddrInc, aPlatCfg);
    }

    return ret;
}

/** Get channel Mask
    Internal API to get channel mask based on source and destination address.

    @param[in]   aHwId    DMA Instance number
    @param[in]   aSrcAddr Source Address
    @param[in]   aDstAddr Destination address

    @retval     channel Mask

    @code{.unparsed}
    Allocation based on channel handle
        Based on the value of channel Handle allocate the channel

    Allocation based on base address
        For hwid == 0
           change channel mask based on the address,
        for hwid == 1
           Allocate channnel ID based on the quad.
    @endcode

    @trace #BRCM_SWARCH_DMA_PLAT_GET_CHMASK_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
uint32_t DMA_PlatGetChannelMask(uint32_t aHwId, uint32_t aSrcAddr, uint32_t aDstAddr, DMA_ChanContextType *chan)
{

#ifndef PL080_ALLOC_CH_BASED_ON_BASEADDRESS
    uint32_t chNum = 0UL;
    for (chNum = 0UL; chNum < PL08X_CHAN_SIZE; chNum++) {
        if (0ULL == chan[chNum].jobHdl) {
            return (1UL << chNum);
        }
    }
    return 0UL;
#else
    uint32_t chMask=0UL;
    if (aHwId == 0) {
        if ((DMA_FLASH_RECEIVE_ADDRESS  == aSrcAddr) || (DMA_FLASH_TRANSMIT_ADDRESS == aDstAddr)) {
            chMask = 0x01UL;
        } else if ((DMA_QSPI0_RECEIVE_ADDRESS == aSrcAddr)  || (DMA_QSPI0_RECEIVE_ADDRESS == aDstAddr)) {
            chMask = 0x02UL;
        } else if ((DMA_QSPI1_RECEIVE_ADDRESS == aSrcAddr)  || (DMA_QSPI1_RECEIVE_ADDRESS == aDstAddr)) {
            chMask = 0x04UL;
        } else if ((DMA_QSPI2_RECEIVE_ADDRESS == aSrcAddr)  || (DMA_QSPI2_RECEIVE_ADDRESS == aDstAddr)) {
            chMask = 0x08UL;
        } else if ((DMA_UART0_RECEIVE_ADDRESS == aSrcAddr)  || (DMA_UART0_RECEIVE_ADDRESS == aDstAddr)) {
            chMask = 0x10UL;
        } else if ((DMA_UART1_RECEIVE_ADDRESS == aSrcAddr)  || (DMA_UART1_RECEIVE_ADDRESS == aDstAddr)) {
            chMask = 0x20UL;
        } else {
            chMask = 0x80UL; /* Lowest priority for Mem to Mem*/
        }
    } else {
        if ((aSrcAddr == DMA_HKADC_0_ADDRESS) || (aDstAddr == DMA_HKADC_0_ADDRESS) ||
            (aSrcAddr == DMA_HKADC_4_ADDRESS) || (aDstAddr == DMA_HKADC_4_ADDRESS)) {
            chMask = 0x01UL;
        } else if ((aSrcAddr == DMA_HKADC_1_ADDRESS) || (aDstAddr == DMA_HKADC_1_ADDRESS) ||
            (aSrcAddr == DMA_HKADC_5_ADDRESS) || (aDstAddr == DMA_HKADC_5_ADDRESS)) {
            chMask = 0x02UL;
        } else if ((aSrcAddr == DMA_HKADC_2_ADDRESS) || (aDstAddr == DMA_HKADC_2_ADDRESS) ||
            (aSrcAddr == DMA_HKADC_6_ADDRESS) || (aDstAddr == DMA_HKADC_6_ADDRESS)) {
            chMask = 0x04UL;
        } else if ((aSrcAddr == DMA_HKADC_3_ADDRESS) || (aDstAddr == DMA_HKADC_3_ADDRESS) ||
            (aSrcAddr == DMA_HKADC_7_ADDRESS) || (aDstAddr == DMA_HKADC_7_ADDRESS)) {
            chMask = 0x08UL;
        } else if ((aSrcAddr == DMA_HKDAC_0_ADDRESS) || (aDstAddr == DMA_HKDAC_0_ADDRESS) ||
            (aSrcAddr == DMA_HKDAC_4_ADDRESS) || (aDstAddr == DMA_HKDAC_4_ADDRESS)) {
            chMask = 0x10UL;
        } else if ((aSrcAddr == DMA_HKDAC_1_ADDRESS) || (aDstAddr == DMA_HKDAC_1_ADDRESS) ||
            (aSrcAddr == DMA_HKDAC_5_ADDRESS) || (aDstAddr == DMA_HKDAC_5_ADDRESS)) {
            chMask = 0x20UL;
        } else if ((aSrcAddr == DMA_HKDAC_2_ADDRESS) || (aDstAddr == DMA_HKDAC_2_ADDRESS) ||
            (aSrcAddr == DMA_HKDAC_6_ADDRESS) || (aDstAddr == DMA_HKDAC_6_ADDRESS)) {
            chMask = 0x40UL;
        } else if ((aSrcAddr == DMA_HKDAC_3_ADDRESS) || (aDstAddr == DMA_HKDAC_3_ADDRESS) ||
            (aSrcAddr == DMA_HKDAC_7_ADDRESS) || (aDstAddr == DMA_HKDAC_7_ADDRESS)) {
            chMask = 0x80UL;
        } else {
            chMask = 0x80UL; /* Lowest priority for Mem to Mem*/
        }
    }
    return chMask;
#endif
}
/** @} */
