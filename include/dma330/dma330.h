/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_dma330drv_ifc DMA330 Driver
    @ingroup grp_dma330drv

    @addtogroup grp_dma330drv_ifc
    @{
    @section sec_dma330_overview Overview
    -# The DMAC contains an instruction processing block that enables it to process program code that controls a DMA transfer. The program code is stored in a region of system memory.
    -# The DMAC stores instructions temporarily in cache.
    -# DMAC supports up to eight DMA channels, with each channel capable of supporting a single concurrent thread of DMA operation.
    -# The DMAC executes one instruction for each AXI clock cycle.
    -# Supports multiple transfer types:<br>
          i) Memory-to-memory.<br>
          ii) Memory-to-peripheral.<br>
          iii) Peripheral-to-memory.<br>
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section api_list List of DMA APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref DMA330_Init | Initialize DMA driver |
    | @ref DMA330_XferSetup | Initiate DMA transfer from Memory to Memory or memory to peripheral |
    | @ref DMA330_GetXferStatus |  Get transfer status |
    | @ref DMA330_XferTrigger | Triggers DMA transfer for Memory to Memory or memory to peripheral |
    | @ref DMA330_DrvChannIRQHandler | DMA channel IRQ Handler |

    @file dma330.h
    @brief System DMA Interface APIs.
    @section apis This section defines the interface for DMA driver
    @version 0.1 Initial revision
*/

#ifndef DMA330_H
#define DMA330_H

/**
    @name DMA 330 driver Architecture IDs
    @{
    @brief Architecture IDs for DMA 330
*/
#define BRCM_SWARCH_DMA330_ADDR_TYPE                     (0x8100U) /**< @brief #DMA330_AddrType */
#define BRCM_SWARCH_DMA330_CHANNEL_ID_TYPE               (0x8101U) /**< @brief #DMA330_ChannelIDType */
#define BRCM_SWARCH_DMA330_PERIPH_ID_TYPE                (0x8102U) /**< @brief #DMA330_PeriphIDType  */
#define BRCM_SWARCH_DMA330_XFER_REQ_TYPE                 (0x8103U) /**< @brief #DMA330_XferReqType      */
#define BRCM_SWARCH_DMA330_BURST_SIZE                    (0x8104U) /**< @brief #DMA330_BurstSize      */
#define BRCM_SWARCH_DMA330_BURST_LEN                     (0x8105U) /**< @brief #DMA330_BurstLen      */
#define BRCM_SWARCH_DMA330_XFER_TYPE                     (0x8106U) /**< @brief #DMA330_XferType      */
#define BRCM_SWARCH_DMA330_XFER_STATUS_TYPE              (0x8107U) /**< @brief #DMA330_XferStatusType      */
#define BRCM_SWARCH_DMA330_INIT_PROC                     (0x8108U) /**< @brief #DMA330_Init             */
#define BRCM_SWARCH_DMA330_XFER_SETUP_PROC               (0x8109U) /**< @brief #DMA330_XferSetup    */
#define BRCM_SWARCH_DMA330_GET_XFER_STATUS_PROC          (0x810AU) /**< @brief #DMA330_GetXferStatus    */
#define BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC             (0x810BU) /**< @brief #DMA330_XferTrigger  */
#define BRCM_SWARCH_DMA330_CHANN_CFG_TYPE                (0x810DU) /**< @brief #DMA330_ChannCfgType      */
#define BRCM_SWARCH_DMA330_CONFIG_TYPE                   (0x810EU) /**< @brief #DMA330_ConfigType      */
#define BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC    (0x8110U) /**< @brief #DMA330_DrvChannIRQHandler  */
#define BRCM_SWARCH_DMA330_CHANN_STATE_TYPE              (0x8111U) /**< @brief #DMA330_ChannStateType*/
#define BRCM_SWARCH_DMA330_OP_TYPE                       (0x8112U) /**< @brief #DMA330_OpType*/
#define BRCM_SWARCH_DMA330_CCR_TYPE                      (0x8113U) /**< @brief #DMA330_CcrType*/


/** @} */


/**
    @brief DMA address type
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint32_t DMA330_AddrType;

/**
    @brief DMA Channel state type

    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint32_t DMA330_ChannStateType;
#define DMA330_CHANN_STATE_UNINIT      (0x0UL)
#define DMA330_CHANN_STATE_IDLE        (0x1UL)
#define DMA330_CHANN_STATE_SETUP       (0x2UL)
#define DMA330_CHANN_STATE_BUSY        (0x3UL)
#define DMA330_CHANN_STATE_ERR         (0x4UL)

/**
    @brief DMA channel IDs
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint32_t DMA330_ChannelIDType;
#define DMA330_CHANNEL_ID_0        (0UL)               /**< channel 0 */
#define DMA330_CHANNEL_ID_1        (1UL)               /**< channel 1 */
#define DMA330_CHANNEL_ID_2        (2UL)               /**< channel 2 */
#define DMA330_CHANNEL_ID_3        (3UL)               /**< channel 3 */
#define DMA330_CHANNEL_ID_4        (4UL)               /**< channel 4 */
#define DMA330_CHANNEL_ID_5        (5UL)               /**< channel 5 */
#define DMA330_CHANNEL_ID_6        (6UL)               /**< channel 6 */
#define DMA330_CHANNEL_ID_7        (7UL)               /**< channel 7 */
#define DMA330_CHANNEL_ID_MAX      (DMA330_CHANNEL_ID_7)  /**< channel max */

/**
    @brief DMA peripheral IDs (refer to TRM for supported peripherals)

    @trace #BRCM_SWREQ_DMA330_INIT

*/
typedef uint8_t DMA330_PeriphIDType;

/**
   @brief DMA burst size (number of bytes within each DMA
          beat)Burst size depends upon the AXI width of DMA,
          refer to the chip TRM for supported width

   @trace #BRCM_SWREQ_DMA330_INIT
 */
typedef uint32_t DMA330_BurstSize;
#define DMA330_BURST_SZ_1        (0UL)   /**< 1 byte per beat */
#define DMA330_BURST_SZ_2        (1UL)   /**< 2 bytes per beat */
#define DMA330_BURST_SZ_4        (2UL)   /**< 4 bytes per beat */
#define DMA330_BURST_SZ_8        (3UL)   /**< 8 bytes per beat */
#define DMA330_BURST_SZ_16       (4UL)   /**< 16 bytes per beat */
#define DMA330_BURST_SZ_32       (5UL)   /**< 32 bytes per beat */
#define DMA330_BURST_SZ_64       (6UL)   /**< 64 bytes per beat */

#define DMA330_BURST_SZ_MAX      (DMA330_BURST_SZ_16) /**< max burst size */

/**
    @brief DMA burst lenght type (number of beats within each burst)

    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint32_t DMA330_BurstLen;

/**
  @brief  DMA transfer type (not all the transfer type may be supported in the chip
          refer to TRM for details

  @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint32_t DMA330_XferType;
#define DMA330_XFER_TYPE_MEM_TO_MEM        (0UL)   /**< Mem to mem transfer */
#define DMA330_XFER_TYPE_MEM_TO_PERIPH     (1UL)   /**< mem to peripheral transfer */
#define DMA330_XFER_TYPE_PERIPH_TO_MEM     (2UL)   /**< peripheral to mem transfer */
#define DMA330_XFER_TYPE_PERIPH_TO_PERIPH  (3UL)   /**< peripheral to peripheral transfer */
#define DMA330_XFER_TYPE_MAX               (DMA330_XFER_TYPE_PERIPH_TO_PERIPH)

/**
   @brief DMA transfer status

   @trace #BRCM_SWREQ_DMA330_INIT

*/
typedef uint32_t DMA330_XferStatusType;
#define DMA330_XFER_STATUS_SUCCESS         (0UL)   /**< DMA Transfer was successful */
#define DMA330_XFER_STATUS_ERROR           (1UL)   /**< Error occured during DMA transfer */
#define DMA330_XFER_STATUS_BUSY            (2UL)   /**< DMA transfer on-going */

/**
   @brief DMA330 opcode

   @trace #BRCM_SWREQ_DMA330_INIT

*/
typedef uint8_t DMA330_OpType;
#define DMA330_OP_MOV                  (0xBCU)     /**< DMA MOVE instruction */
#define DMA330_OP_SRCINC               (0x54U)     /**< DMA SRC INC instruction */
#define DMA330_OP_SRCINC_NEG           (0x58U)     /**< DMA SRC INC instruction */
#define DMA330_OP_DSTINC               (0x56U)     /**< DMA DST INC instruction */
#define DMA330_OP_DSTINC_NEG           (0x5EU)     /**< DMA DST INC instruction */
#define DMA330_OP_LD                   (0x04U)     /**< DMA LOAD instruction */
#define DMA330_OP_ST                   (0x08U)     /**< DMA Store instruction */
#define DMA330_OP_LDP                  (0x25U)     /**< DMA load peripheral instruction */
#define DMA330_OP_STP                  (0x29U)     /**< DMA store peripheral instruction */
#define DMA330_OP_WFP                  (0x30U)     /**< DMA WAIT PERIPHERAL instruction */
#define DMA330_OP_FLUSHP               (0x35U)     /**< DMA flush peripheral instruction */
#define DMA330_OP_LP                   (0x20U)     /**< DMA LOOP COUTER instruction */
#define DMA330_OP_KILL                 (0x01U)     /**< DMA KILL instruction */
#define DMA330_OP_LPEND                (0x28U)     /**< DMA LOOP END instruction */
#define DMA330_OP_SEV                  (0x34U)     /**< DMA SEV instruction */
#define DMA330_OP_WMB                  (0x13U)     /**< DMA WRITE MEM BARRIER instruction */
#define DMA330_OP_END                  (0x00U)     /**< DMA end instruction */

#define DMA330_OP_FLUSHP_TIMEOUT   (20U)

/**
   @brief DMA330 CCR bit position

   @trace #BRCM_SWREQ_DMA330_INIT

*/
typedef uint32_t DMA330_CcrType;

#define DMA330_CCR_HW_SRC_INC_MASK            (0x1UL)
#define DMA330_CCR_SRC_BURST_SZ_MASK       (0x7UL << 1UL)
#define DMA330_CCR_SRC_BURST_SZ_SHIFT      (1UL)
#define DMA330_CCR_HW_SRC_BURST_LEN_SHIFT     (4UL)
#define DMA330_CCR_SRC_PROT_CNTRL_SHIFT    (8UL)
#define DMA330_CCR_DEST_INC_MASK           (0x1UL << 14UL)
#define DMA330_CCR_DEST_BURST_SZ_MASK      (0x7UL << 15UL)
#define DMA330_CCR_DEST_BURST_SZ_SHIFT     (15UL)
#define DMA330_CCR_DEST_BURST_LEN_MASK     (0xFUL << 18UL)
#define DMA330_CCR_DEST_BURST_LEN_SHIFT    (18UL)
#define DMA330_CCR_DEST_PROT_CNTRL_SHIFT   (22UL)
#define DMA330_CCR_DEST_CACHE_CNTRL_SHIFT  (25UL)

/**
   @brief DMA Trasfer Request packet

   @trace #BRCM_SWREQ_DMA330_INIT
 */
typedef struct sDMA330_XferReqType {
    DMA330_AddrType            srcAddr;        /**< source address */
    DMA330_AddrType            destAddr;       /**< destination address */
    uint32_t                rowSize;        /**< DMA transfer size in bytes */
    uint32_t                nRows;          /**< Number of rows to be transferred */
    int16_t                 srcSkip;        /**< Source address skip in bytes
                                                (after copying each row) */
    int16_t                 dstSkip;        /**< Destination address skip in bytes
                                                (after copying each row) */
} DMA330_XferReqType;

/**
  @brief DMA channel configuration structure

  @trace #BRCM_SWREQ_DMA330_INIT
 */
typedef struct sDMA330_ChannCfgType {

    uint64_t            compID; /**< 16-bit System component ID OR 64-bit Media component ID */

    uint32_t            instanceID; /**< instance ID, starting from zero */

    DMA330_XferType        xferType; /**< DMA transfer type allowed on this channel */

    DMA330_PeriphIDType    periphID; /**< DMA peripheral ID if transfer type(@xferType)is either from peripheral to memory or memory to peripheral */

    DMA330_BurstSize       burstSz; /**< DMA burst size for this channel */

    DMA330_BurstLen        burstLen; /**< DMA burst lenght for this channel */

    uint8_t       *codeAddr; /**< DMA code address (Address where DMA instructions will be populated by the driver */

    uint32_t            codeSize; /**< Size of the DMA code region */
} DMA330_ChannCfgType;

/**
  @brief DMA controller configuration structure

  @trace #BRCM_SWREQ_DMA330_INIT
 */
typedef struct sDMA330_ConfigType {
    const DMA330_ChannCfgType          *channCfg;
    uint32_t                  channCfgSz;
} DMA330_ConfigType;

/** @brief Initialize DMA330

    This API initializes the given DMA330 hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCfgTbl         DMA configuration structure

    @return     int32_t
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_OK             Success


    @post None

    @trace  #BRCM_SWREQ_DMA330_INIT

    @limitations None
*/
int32_t DMA330_Init(DMA330_ConfigType aCfgTbl);

/** @brief Get transfer status
 *
 * This API returns the status of the DMA last DMA transfer which
 * was triggered using DMA330_Mem2MemXfer.
 *
 * @param[in]   aID         DMA channel ID
 *
 * @retval      DMA330_XferStatusType
 @trace #BRCM_SWREQ_DMA330_V1_DRIVER
 */
int32_t DMA330_GetXferStatus(DMA330_ChannelIDType aID,
                            DMA330_XferStatusType *aStatus);

/** @brief Initiate DMA transfer from Memory to Memory or memory to peripheral
 *
 * This function initiates Memory to Memory or memory to peripheral transfer using DMA.
 * Only a fixed type of transfer is allowed on a channel
 * (which is passed to DMA driver during initialization).
 * This API will chain the DMA requests (@aXferReq) and trigger
 * the DMA automatically. Upon completion of all transfers
 * it will raise a callback to client.
 *
 * @param[in]  aID         DMA channel ID
 * @param[in]  aXferReq    Pointer to DMA transfer requests array
 * @param[in]  aReqSz      Size of the DMA transfer request array
 * @param[in]  aCfgTbl     DMA configuration structure
 *
 * @retval      BCM_ERR_OK              Setup was successful
 * @retval      BCM_ERR_INVAL_PARAMS    aID is invalid
 * @retval      BCM_ERR_INVAL_PARAMS    aXferReq is NULL
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_NOMEM           Transfer request too big
 @trace #BRCM_SWREQ_DMA330_V1_DRIVER
 */
int32_t DMA330_XferSetup(DMA330_ChannelIDType aID,
        DMA330_XferReqType *aXferReq, uint32_t aReqSZ, DMA330_ConfigType aCfgTbl);

/** @brief Trigger DMA transfer for Memory to Memory or memory to peripheral
 *
 * This function triggers the DMA transfer which was
 * setup using DMA330_XferSetup API. Upon completion of the
 * transfer, DMA driver will raise a callback in interrupt
 * context.
 *
 * @param[in]   aID         DMA channel ID
 * @param[in]   aCfgTbl     DMA configuration structure
 *
 * @retval      BCM_ERR_OK              DMA was triggered successfully
 * @retval      BCM_ERR_INVAL_PARAMS    aReqIdx is invalid
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_INVAL_STATE     DMA is not setup
 @trace #BRCM_SWREQ_DMA330_V1_DRIVER
 */
int32_t DMA330_XferTrigger(DMA330_ChannelIDType aID, uint32_t aReqIdx,DMA330_ConfigType aCfgTbl);

/** @brief  DMA channel sample IRQ Handler

  This API is can be called when DMA channel interrupt is asserted

  @param[in]    aID  Index of the DMA port. Passed by osil.
  @param[in]    aCfgTbl  DMA configuration structure

  @behavior Async, Non-reentrant

  @pre None

  @return       void
  @post None

    @trace #BRCM_SWREQ_DMA330_V1_DRIVER

  @limitations None
 */
uint32_t DMA330_DrvChannIRQHandler(DMA330_ChannelIDType aID, DMA330_ConfigType aCfgTbl);
#endif /* DMA330_H */

/** @} */
