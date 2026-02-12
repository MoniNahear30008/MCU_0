/*****************************************************************************
  Copyright 2024 Broadcom Limited.  All rights reserved.
  Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_dma_plat_impl BCM8915x Platform - Helper
    @ingroup grp_dma_v2

    @addtogroup grp_dma_plat_impl
    @{
*/

#ifndef PL08X_PLAT_H
#define PL08X_PLAT_H

#include <stddef.h>
#include <stdbool.h>

/*#include <cpu.h>*/
#include <bcm_err.h>

/**
    @name DMA V2 platform layer Architecture IDs
    @{
    @brief Architecture IDs for DMA V2 platform layer
*/
#define BRCM_SWARCH_DMA_BURST_SIZE_MACRO                 (0x8100U) /**< @brief #DMA_BURST_SIZE_1               */
#define BRCM_SWARCH_DMA_ACC_MACRO                        (0x8101U) /**< @brief #DMA_ACC_PRIVILEGE              */
#define BRCM_SWARCH_DMA_WORD_SIZE_MACRO                  (0x8102U) /**< @brief #DMA_WORD_SIZE_8                */
#define BRCM_SWARCH_DMA_PLAT_CFG_TYPE                    (0x8103U) /**< @brief #DMA_PlatCfgType                */
#define BRCM_SWARCH_DMA_PLAT_GET_CHMASK_PROC             (0x8105U) /**< @brief #DMA_PlatGetChannelMask         */
#define BRCM_SWARCH_DMA_PLAT_GET_CFG_PROC                (0x8106U) /**< @brief #DMA_PlatGetCfg                 */
#define BRCM_SWARCH_DMA_GET_MEM_XFER_WIDTH_INCMODE_PROC  (0x8107U) /**< @brief #DMA_GetMemXferWidthAndIncMode  */
#define BRCM_SWARCH_DMA_CHAN_CONTEXT_TYPE                (0x8108U) /**< @brief #DMA_ChanContextType              */
/** @} */


/**
   @name DMA PL08X burst sizes
   @{
   @brief DMA PL08X burst size macros

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
#define DMA_BURST_SIZE_1      (0UL)
#define DMA_BURST_SIZE_4      (1UL)
#define DMA_BURST_SIZE_8      (2UL)
#define DMA_BURST_SIZE_16     (3UL)
#define DMA_BURST_SIZE_32     (4UL)
#define DMA_BURST_SIZE_64     (5UL)
#define DMA_BURST_SIZE_128    (6UL)
#define DMA_BURST_SIZE_256    (7UL)
/** @} */

/**
   @name DMA PL08X access modes
   @{
   @brief DMA PL08X access mode macros

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
#define DMA_ACC_PRIVILEGE     (1UL << 28)
#define DMA_ACC_BUFFERABLE    (1UL << 29)
#define DMA_ACC_CACHEABLE     (1UL << 30)
/** @} */

/**
   @name PL080 DMA Word sizes
   @{
   @brief PL080 DMA Word size macros

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
#define DMA_WORD_SIZE_8       (0UL)
#define DMA_WORD_SIZE_16      (1UL)
#define DMA_WORD_SIZE_32      (2UL)
/** @} */

/**
   @brief PL08X Channel Context structure

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
typedef struct sDMA_ChanContextType {
    BCM_HandleType jobHdl;
    int32_t error;
    uint32_t enableNotif;
    DMA_XferIssuerIdType issuerId;
} DMA_ChanContextType;


/**
   @brief DMA PL08X Platform config

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
typedef struct sDMA_PlatCfgType {
   bool        isPeriph;       /**< @brief True if the Xfer address is a peripheral FIFO                   */
   bool        isFlowCtrlr;    /**< @brief True if the peripheral needs to be the flow controller          */
   uint32_t    periphId;       /**< @brief Peripheral ID                                                   */
   uint32_t    xferWidth;      /**< @brief Xfer width to be configured                                     */
   uint32_t    progBaseAddr;   /**< @brief Base address of the Xfer to be programmed into a PL080 register */
   uint32_t    progIncMode;    /**< @brief IncMode to be programmed into a PL080 register                  */
   uint32_t    progXferWidth;  /**< @brief Xfer width enum to be programmed into a PL080 register          */
   uint32_t    progBurstSize;  /**< @brief Burst size enum to be programmed into a PL080 register          */
} DMA_PlatCfgType;


/** @brief Get the set of channels capable of handling a DMA operation

   @behavior Sync, reentrant

   @pre None

   @param[in]  aHwId          HW ID of the DMA engine
   @param[in]  aSrcAddr       Source address of the DMA operation
   @param[in]  aDstAddr       Destination address of the DMA operation
   @param[in]  chan           Channel information

   @return Bitmask with set-bits indicating the channel indices
      that can handle a DMA operation between the given source
      and destination addresses against the given HwId. Zero if
      there are no capable channels to handle this operation

   @post None

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
uint32_t DMA_PlatGetChannelMask(uint32_t aHwId, uint32_t aSrcAddr, uint32_t aDstAddr, DMA_ChanContextType *chan);

/** @brief Get the DMA platform specific configuration for the sorce or destination of the Xfer

   @behavior Sync, reentrant

   @pre None

   @param[in]  aHwId                HW ID of the DMA engine
   @param[in]  aIssuerId            Issuer identifier
   @param[in]  aIsSrc               Set to 1 if the source side configuration
                                    is being requested. 0 for the destination
                                    side configuration
   @param[in]  aAddr                Xfer address (ie.. source or destination address)
   @param[in]  aRowSize             Requested row size of the Xfer
   @param[in]  aAddrInc             Requested addrInc operation
   @param[out] aPlatCfg             The platform configuration output

   @retval     #BCM_ERR_OK          Platform configuration populated sucessfully
   @retval     #BCM_ERR_NOSUPPORT   No Support for the requested params

   @post None

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
int32_t DMA_PlatGetCfg(uint32_t aHwId,
                       DMA_XferIssuerIdType const* aIssuerId,
                       uint32_t aIsSrc,
                       uint32_t aAddr, uint32_t aRowSize, uint32_t aAddrInc,
                       DMA_PlatCfgType *aPlatCfg);

/** @brief Get the Xfer width and incMode settings for the memory side of the Xfer (if there is one)

   @behavior Sync, reentrant

   @pre None

   @param[in]  aRowSize             HW ID of the DMA engine
   @param[in]  aAddrInc             Requested addrInc operation on this side of the Xfer
   @param[out] aProgXferWidth       Xfer width enum to be programmed into the HW register
   @param[out] aProgXferIncMode     IncMode enum to be programmed into the HW register

   @retval     #BCM_ERR_OK          aProgXferWidth and aProgIncMode populated correctly
   @retval     #BCM_ERR_NOSUPPORT   The combination of aRowSize and aAddrInc is not supported

   @post None

   @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
int32_t DMA_GetMemXferWidthAndIncMode(uint32_t aRowSize, uint32_t aAddrInc, uint32_t *aProgXferWidth, uint32_t *aProgIncMode);

#endif /* DMA_BCM8958X_H */
/** @} */
