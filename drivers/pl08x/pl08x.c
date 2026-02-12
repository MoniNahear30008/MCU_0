/*****************************************************************************
  Copyright 2024 Broadcom Limited.  All rights reserved.
  Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_pl080_impl Implementation
    @ingroup grp_dma_v2

    @addtogroup grp_pl080_impl
    @{

    @file pl080.c
    @brief PL080 DMA driver implementation

    This source file contains the implementation of driver functions
    for PL080 DMA

    @version 0.1 Initial version
*/

#include <string.h>
#include <stdbool.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <dma_v2.h>
#include "pl08x_plat.h"

#include "pl08x_rdb.h"

/**
    @name PL080 DMA driver Design IDs
    @{
    @brief Design IDs for PL080 DMA driver
*/
#define BRCM_SWDSGN_DMA_FLOWCTL_MACRO                   (0xB100U) /**< @brief #DMA_FLOWCTL_MEM_MEM_DMA          */
#define BRCM_SWDSGN_DMA_MAX_NUM_PER_CHANNEL_LLIS_MACRO  (0xB101U) /**< @brief #DMA_MAX_NUM_PER_CHANNEL_LLIS     */
#define BRCM_SWDSGN_DMA_MAX_CHANNELS_MACRO              (0xB102U) /**< @brief #DMA_MAX_CHANNELS                 */
#define BRCM_SWDSGN_DMA_JOB_HDL_MACRO                   (0xB103U) /**< @brief #DMA_JOB_HDL_MAGIC                */
#define BRCM_SWDSGN_DMA_PL081_COMMON_CFG_TYPE           (0xB104U) /**< @brief #DMA_Pl081CommonCfgType           */
#define BRCM_SWDSGN_DMA_LINKED_LIST_TYPE                (0xB105U) /**< @brief #DMA_LinkedListType               */
#define BRCM_SWDSGN_DMA_CONTEXT_TYPE                    (0xB107U) /**< @brief #DMA_ContextType                  */
#define BRCM_SWDSGN_DMA_LINKED_LIST_NODES_GLOBAL        (0xB108U) /**< @brief #DMA_LinkedListNodes              */
#define BRCM_SWDSGN_DMA_CONTEXT_GLOBAL                  (0xB109U) /**< @brief #DMA_Context                      */
#define BRCM_SWDSGN_DMA_REG_GLOBAL                      (0xB10AU) /**< @brief #DMA_REG                          */
#define BRCM_SWDSGN_DMA_DRV_INIT_PROC                   (0xB10CU) /**< @brief #DMA_DrvInit                      */
#define BRCM_SWDSGN_DMA_DRV_DEINIT_PROC                 (0xB10DU) /**< @brief #DMA_DrvDeinit                    */
#define BRCM_SWDSGN_DMA_ALLOC_CHANNEL_PROC              (0xB10EU) /**< @brief #DMA_AllocChannel                 */
#define BRCM_SWDSGN_DMA_GET_FLOW_CTRL_PROC              (0xB10FU) /**< @brief #DMA_GetFlowCtrl                  */
#define BRCM_SWDSGN_DMA_CHECK_CFG_COMPATIBLE_PROC       (0xB110U) /**< @brief #DMA_CheckCfgCompatible           */
#define BRCM_SWDSGN_DMA_GET_CONFIG_PROC                 (0xB111U) /**< @brief #DMA_GetConfig                    */
#define BRCM_SWDSGN_DMA_DRV_START_XFER_PROC             (0xB112U) /**< @brief #DMA_DrvStartXfer                 */
#define BRCM_SWDSGN_DMA_DRV_GET_XFER_STATUS_PROC        (0xB113U) /**< @brief #DMA_DrvGetXferStatus             */
#define BRCM_SWDSGN_DMA_SERVICE_IRQ_PROC                (0xB114U) /**< @brief #DMA_ServiceIRQ                   */
#define BRCM_SWDSGN_DMA_PLAT_GET_LLI_PROG_ADDR_PROC     (0xB115U) /**< @brief #DMA_PlatGetLliProgAddr     */
#define BRCM_SWDSGN_DMA_PLAT_TRANSLATE_TCM_ADDR_PROC    (0xB116U) /**< @brief #DMA_PlatTranslateTCMAddr   */
/** @} */

/**
   @name DMA PL08X flow control
   @{

   @brief DMA PL08X flow control macros

   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
#define DMA_FLOWCTL_MEM_MEM_DMA     (0UL)
#define DMA_FLOWCTL_MEM_PER_DMA     (1UL)
#define DMA_FLOWCTL_PER_MEM_DMA     (2UL)
#define DMA_FLOWCTL_PER_PER_DMA     (3UL)
#define DMA_FLOWCTL_PER_PER_DST     (4UL)
#define DMA_FLOWCTL_MEM_PER_DST     (5UL)
#define DMA_FLOWCTL_PER_MEM_SRC     (6UL)
#define DMA_FLOWCTL_PER_PER_SRC     (7UL)
/** @} */

/**
    @brief Maximum number of LLI's supported per channel
    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
    @trace #BRCM_SWARCH_DMA_DEINIT_PROC
    @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
#define DMA_MAX_NUM_PER_CHANNEL_LLIS    (DMA_MAX_NUM_XFER_REQ - 1UL)

/**
   @brief Maximum Channels per DMA instance
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC
   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
#define DMA_MAX_CHANNELS        PL08X_CHAN_SIZE

/**
   @name JOB Handle
   @{
   @brief JOB Handle macros
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
#define DMA_JOB_HDL_MAGIC           (0x444D414A00000000ULL)
#define DMA_JOB_HDL_MAGIC_MASK      (0xFFFFFFFF00000000ULL)
#define DMA_JOB_HDL_COUNT_MASK      (0x00000000FFFFFF00ULL)
#define DMA_JOB_HDL_COUNT_SHIFT     (8UL)
#define DMA_JOB_HDL_CHAN_MASK       (0x00000000000000FFULL)
#define DMA_JOB_HDL_INVAL           (0x0000000000000000ULL)
/** @} */

/**
    @brief PL08X common configuration (across all LLI's)
    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
    @trace #BRCM_SWARCH_DMA_DEINIT_PROC
    @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
typedef struct sDMA_Pl081CommonCfgType {
    uint32_t flowCtrl;      /**< @brief flow control                            */
    uint32_t privilege;     /**< @brief enable privilege access                 */
    uint32_t srcPeriph;     /**< @brief Source peripheral id                    */
    uint32_t dstPeriph;     /**< @brief Destination peripheral id               */
} DMA_Pl081CommonCfgType;

/**
   @brief DMA Linked list node structure
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
typedef struct sDMA_LinkedListType {
    uint32_t srcAddr;
    uint32_t dstAddr;
    uint32_t next;
    uint32_t control;
} DMA_LinkedListType;

/**
   @brief PL08X Context structure
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
typedef struct sDMA_ContextType {
    uint32_t handleCount;
    DMA_ChanContextType chan[PL08X_MAX_HW_ID][DMA_MAX_CHANNELS];
} DMA_ContextType;

/**
    @brief Memory for the PL08X linked list nodes
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
/*static DMA_LinkedListType DMA_LinkedListNodes[PL08X_MAX_HW_ID][DMA_MAX_CHANNELS][DMA_MAX_NUM_PER_CHANNEL_LLIS] COMP_SECTION(DMA_LLINODES_MEM_SECTION);*/
static DMA_LinkedListType DMA_LinkedListNodes[PL08X_MAX_HW_ID][DMA_MAX_CHANNELS][DMA_MAX_NUM_PER_CHANNEL_LLIS] ;

/**
   @brief PL08X Context
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
static DMA_ContextType DMA_Context COMP_SECTION(".bss.dma.DMA_Context");

/**
   @brief PL08X Register list for each instance
   @trace #BRCM_SWARCH_DMA_START_XFER_PROC
   @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
   @trace #BRCM_SWARCH_DMA_DEINIT_PROC
   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC

   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
static PL08X_RDBType *const DMA_REG[PL08X_MAX_HW_ID] = {
    (PL08X_RDBType *)(DMA_PL081_BASE),
#if PL08X_MAX_HW_ID > 1UL
    (PL08X_RDBType *)(DMA_PL081_2_BASE),
#endif
#if PL08X_MAX_HW_ID > 2UL
#error "PL08X base address not populated"
#endif
};


/** @brief Initialize the PL08X DMA instance

    @behavior Sync, reentrant

    @pre DMA instance not initialized

    @param[in]   aHwId    DMA Instance number

    @retval     #BCM_ERR_OK           Init successful
    @retval     #BCM_ERR_INVAL_PARAMS aHwId invalid

    @post if successful, DMA instance initialized

    @code{.unparsed}
    ret = BCM_ERR_OK

    if aHwId >= PL08X_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS
    else
        DMA_REG[aHwId]->dmacconfiguration = PL08X_DMACCONFIGURATION_ENABLE_MASK

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_INIT_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
int32_t DMA_DrvInit(uint32_t aHwId)
{
    int32_t ret = BCM_ERR_OK;

    if (aHwId >= PL08X_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (0UL != (DMA_REG[aHwId]->dmacconfiguration &
                       PL08X_DMACCONFIGURATION_E_MASK)) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        DMA_REG[aHwId]->dmacconfiguration = PL08X_DMACCONFIGURATION_E_MASK;
    }

    return ret;
}

/** @brief Deinitialize the PL08X DMA instance

    @behavior Sync, reentrant

    @pre DMA instance is initialized

    @param[in]  aHwId    DMA Instance number

    @retval     #BCM_ERR_OK           Deinit successful
    @retval     #BCM_ERR_INVAL_PARAMS aHwId invalid
    @retval     #BCM_ERR_UNINIT       Not initialized

    @post if successful, DMA instance de-initialized

    @code{.unparsed}
    ret = BCM_ERR_OK
    if aHwId >= PL08X_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS
    elif DMA_REG[aHwId]->dmacconfiguration == 0
        ret = BCM_ERR_UNINIT
    else
        DMA_REG[aHwId]->dmacconfiguration = 0UL

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_DEINIT_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
int32_t DMA_DrvDeinit(uint32_t aHwId)
{
    int32_t ret = BCM_ERR_OK;

    if (aHwId >= PL08X_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (0UL == DMA_REG[aHwId]->dmacconfiguration) {
        ret = BCM_ERR_UNINIT;
    } else {
        uint32_t i;
        for (i = 0UL; i < PL08X_CHAN_SIZE; i++) {
            /* If any of the channel in progress */
            if (0UL != (DMA_REG[aHwId]->chan[i].dmacc0configuration &
                         PL08X_DMACC0CONFIGURATION_C0ENABLE_MASK)) {
                ret = BCM_ERR_BUSY;
                break;
            }
            DMA_Context.chan[aHwId][i].jobHdl = DMA_JOB_HDL_INVAL;
        }
    }

    if (BCM_ERR_OK == ret) {
        DMA_REG[aHwId]->dmacconfiguration = 0UL;
    }

    return ret;
}

/** @brief Allocate a channel capable of handling the requested Xfer

    @behavior Sync, reentrant

    @pre DMA instance is initialized

    @param[in]  aHwId           DMA Instance number
    @param[in]  aXferReq        Xfer requests
    @param[in]  aNumReq         Number of Xfer requests
    @param[out] aChNum          Free channel number

    @retval     #BCM_ERR_OK         Channel allocation done
    @retval     #BCM_ERR_NOSUPPORT  No DMA channel in the system to support the
                                    requested Xfers
    @retval     #BCM_ERR_NOMEM      All capable DMA channels busy

    @post None

    @code{.unparsed}
    chAllocMask = (1UL << PL08X_CHAN_SIZE) - 1UL;

    for (i = 0UL; i < aNumReq; i++)
        chAllocMask &= DMA_PlatGetChannelMask(aHwId, aXferReq[i].srcAddr, aXferReq[i].dstAddr)

    if 0 == chAllocMask
        return BCM_ERR_NOSUPPORT

    ret = BCM_ERR_NOMEM
    for (chNum = 0UL; chNum < PL08X_CHAN_SIZE; chNum++)
        chBit = (1UL << chNum)
        if (0UL != (chBit & chAllocMask)) && (DMA_JOB_HDL_INVAL == DMA_Context.chan[aHwId][chNum].jobHdl)
            *aChNum = chNum
            ret = BCM_ERR_OK
            break

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
static int32_t DMA_AllocChannel(uint32_t aHwId,
                                DMA_XferReqType const *aXferReq,
                                uint32_t aNumReq,
                                uint32_t *aChNum)
{
    int32_t ret;
    uint32_t i;
    uint32_t chAllocMask = (1UL << PL08X_CHAN_SIZE) - 1UL;

    for (i = 0UL; i < aNumReq; i++) {
        chAllocMask &= DMA_PlatGetChannelMask(aHwId, aXferReq[i].srcAddr, aXferReq[i].dstAddr, DMA_Context.chan[aHwId]);
    }

    if (0UL != chAllocMask) {
        uint32_t chNum;

        ret = BCM_ERR_NOMEM;
        for (chNum = 0UL; chNum < PL08X_CHAN_SIZE; chNum++) {
            const uint32_t chBit = (1UL << chNum);

            if ((0UL != (chBit & chAllocMask)) &&
                (DMA_JOB_HDL_INVAL == DMA_Context.chan[aHwId][chNum].jobHdl)) {
                *aChNum = chNum;
                ret = BCM_ERR_OK;
                break;
            }
        }
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }

    return ret;
}

/** @brief Determine the PL08X flow control settings for the requested Xfer

    @behavior Sync, reentrant

    @pre None

    @param[in]  aSrcPlatCfg         Xfer src configuration
    @param[out] aDstPlatCfg         Xfer dst configuration

    @retval     #BCM_ERR_OK         Flow control configuration computed sucessfully
    @retval     #BCM_ERR_NOSUPPORT  Flow control could not be determined

    @post None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if !(aSrcPlatCfg->isPeriph) && !(aDstPlatCfg->isPeriph)
        *aFlowCtrl = DMA_FLOWCTL_MEM_MEM_DMA
        *aIsDMAFlowCtrlr = true
    else if (aSrcPlatCfg->isPeriph) && !(aDstPlatCfg->isPeriph)
        if aSrcPlatCfg->isFlowCtrlr
            *aFlowCtrl = DMA_FLOWCTL_PER_MEM_SRC
            *aIsDMAFlowCtrlr = false
        else
            *aFlowCtrl = DMA_FLOWCTL_PER_MEM_DMA
            *aIsDMAFlowCtrlr = true
    else if !(aSrcPlatCfg->isPeriph) && (aDstPlatCfg->isPeriph)
        if (aDstPlatCfg->isFlowCtrlr)
            *aFlowCtrl = DMA_FLOWCTL_MEM_PER_DST
            *aIsDMAFlowCtrlr = false
        else
            *aFlowCtrl = DMA_FLOWCTL_MEM_PER_DMA
            *aIsDMAFlowCtrlr = true
    else
        if !(aSrcPlatCfg->isFlowCtrlr) && (aDstPlatCfg->isFlowCtrlr)
            *aFlowCtrl = DMA_FLOWCTL_PER_PER_DST
            *aIsDMAFlowCtrlr = false
        else if (aSrcPlatCfg->isFlowCtrlr) && !(aDstPlatCfg->isFlowCtrlr)
            *aFlowCtrl = DMA_FLOWCTL_PER_PER_SRC
            *aIsDMAFlowCtrlr = false
        else if !(aSrcPlatCfg->isFlowCtrlr) && !(aDstPlatCfg->isFlowCtrlr)
            *aFlowCtrl = DMA_FLOWCTL_PER_PER_DMA
            *aIsDMAFlowCtrlr = true
        else
            ret = BCM_ERR_NOSUPPORT

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
static int32_t DMA_GetFlowCtrl(DMA_PlatCfgType const* aSrcPlatCfg,
                               DMA_PlatCfgType const* aDstPlatCfg,
                               uint32_t *aFlowCtrl,
                               bool *aIsDMAFlowCtrlr)
{
    int32_t ret = BCM_ERR_OK;

    if (!(aSrcPlatCfg->isPeriph) && !(aDstPlatCfg->isPeriph)) {
        /* Memory to memory transfer */
        *aFlowCtrl = DMA_FLOWCTL_MEM_MEM_DMA;
        *aIsDMAFlowCtrlr = true;
    } else if (aSrcPlatCfg->isPeriph && !(aDstPlatCfg->isPeriph)) {
        /* Peripheral to memory */
        if (aSrcPlatCfg->isFlowCtrlr) {
            *aFlowCtrl = DMA_FLOWCTL_PER_MEM_SRC;
            *aIsDMAFlowCtrlr = false;
        } else {
            *aFlowCtrl = DMA_FLOWCTL_PER_MEM_DMA;
            *aIsDMAFlowCtrlr = true;
        }
    } else if (!(aSrcPlatCfg->isPeriph) && aDstPlatCfg->isPeriph) {
        /* Memory to peripheral */
        if (aDstPlatCfg->isFlowCtrlr) {
            *aFlowCtrl = DMA_FLOWCTL_MEM_PER_DST;
            *aIsDMAFlowCtrlr = false;
        } else {
            *aFlowCtrl = DMA_FLOWCTL_MEM_PER_DMA;
            *aIsDMAFlowCtrlr = true;
        }
    } else {
        /* Peripheral to peripheral */
        if (!(aSrcPlatCfg->isFlowCtrlr) && aDstPlatCfg->isFlowCtrlr) {
            *aFlowCtrl = DMA_FLOWCTL_PER_PER_DST;
            *aIsDMAFlowCtrlr = false;
        } else if (aSrcPlatCfg->isFlowCtrlr && !(aDstPlatCfg->isFlowCtrlr)) {
            *aFlowCtrl = DMA_FLOWCTL_PER_PER_SRC;
            *aIsDMAFlowCtrlr = false;
        } else if (!(aSrcPlatCfg->isFlowCtrlr) && !(aDstPlatCfg->isFlowCtrlr)) {
            *aFlowCtrl = DMA_FLOWCTL_PER_PER_DMA;
            *aIsDMAFlowCtrlr = true;
        } else {
            ret = BCM_ERR_NOSUPPORT;
        }
    }

    return ret;
}

/** @brief Check if two source or two destination configurations are compatible

    @behavior Sync, reentrant

    @pre None

    @param[in]  aPlatCfg0           src/dst config 0
    @param[out] aPlatCfg1           src/dst config 1

    @retval     #BCM_ERR_OK         The input configs are compatible
    @retval     #BCM_ERR_NOSUPPORT  The input configs are incompatible

    @post None

    @code{.unparsed}
    if aPlatCfg0->isPeriph && aPlatCfg1->periphId && (aPlatCfg0->periphId == aPlatCfg1->periphId)
        return BCM_ERR_OK

    if !aPlatCfg0->isPeriph && !aPlatCfg1->isPeriph
        return BCM_ERR_OK

    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
static int32_t DMA_CheckCfgCompatible(DMA_PlatCfgType const* aPlatCfg0, DMA_PlatCfgType const* aPlatCfg1)
{
    int32_t ret;

    if ((true == aPlatCfg0->isPeriph) && (true == aPlatCfg1->isPeriph) &&
        (aPlatCfg0->periphId == aPlatCfg1->periphId)) {
        ret = BCM_ERR_OK;
    } else if ((false == aPlatCfg0->isPeriph) && (false == aPlatCfg1->isPeriph)) {
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_NOSUPPORT;
    }

    return ret;
}
/** @brief Translate the M7 TCM address to the HSM address

    @behavior Sync, reentrant

    @pre None

    @param[in]  aCpuAddr            CPU address to translate
    @param[out] aTranslatedAddr     Translated address output

    @retval     #BCM_ERR_OK         Address translation sucessful
    @retval     #BCM_ERR_NOSUPPORT  Address not in M7 TCM

    @post None

    @code{.unparsed}
        Convert aCpuAddr to NOC address and assign it to aTranslatedAddr
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
static int32_t DMA_PlatTranslateTCMAddr(const uint32_t aCpuAddr, uint32_t *const aTranslatedAddr)
{
    int32_t ret;

    *aTranslatedAddr = (aCpuAddr & 0xF0FFFFFFUL);
    ret = BCM_ERR_OK;

    return ret;
}


/** @brief Place holder Internal API

    @behavior Sync, reentrant

    @pre None

    @param[in]  aCpuAddr            CPU address to translate

    @retval     #lliProgAddr

    @post None

    @code{.unparsed}
    uint32_t lliProgAddr = 0UL
    DMA_PlatTranslateTCMAddr((const uint32_t) aCpuAddr, &lliProgAddr)
    return lliProgAddr
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_PLAT_PL08X
*/
static uint32_t DMA_PlatGetLliProgAddr(const void* aCpuAddr)
{
    uint32_t lliProgAddr = 0UL;

    const int32_t ret = DMA_PlatTranslateTCMAddr((const uint32_t)(const uint32_t*)aCpuAddr, &lliProgAddr);
    if (BCM_ERR_OK != ret) {
        lliProgAddr = 0UL;
    }

    return lliProgAddr;
}
/** @brief Compute the DMA configuration for the requested Xfer operations

    @behavior Sync, reentrant

    @pre None

    @param[in]   aHwId              DMA Instance number
    @param[in]   aIssuerId          Xfer issuer identifier
    @param[in]   aXferReq           List of transfer requests
    @param[in]   aNumReq            Number of transfer requests
    @param[in]   aChNum             DMA channel to use
    @param[in]   aRegCfg            LLI register initial configuration
    @param[out]  aCommonCfg         DMA channel common configuration

    @retval     #BCM_ERR_OK         The input configs are compatible
    @retval     #BCM_ERR_NOSUPPORT  The input configs are incompatible

    @post None

    @code{.unparsed}
    srcCfgValid = DMA_PlatGetCfg(aHwId, aIssuerId, 1UL,
        aXferReq[0].srcAddr, aXferReq[0].rowSize, aXferReq[0].srcAddrInc,
        &srcCfg
    )
    dstCfgValid = DMA_PlatGetCfg(aHwId, aIssuerId, 0UL,
        aXferReq[0].dstAddr, aXferReq[0].rowSize, aXferReq[0].dstAddrInc,
        &dstCfg
    )

    if  (BCM_ERR_OK == srcCfgValid) && (BCM_ERR_OK == dstCfgValid) &&
        (BCM_ERR_OK == DMA_GetFlowCtrl(&srcCfg, &dstCfg, &flowCtrl, &isDMAFlowCtrlr)

        *nextAddr = &aRegCfg->next

        aCommonCfg->flowCtrl = flowCtrl
        aCommonCfg->srcPeriph = srcCfg.periphId
        aCommonCfg->dstPeriph = dstCfg.periphId

        for (i = 0; i < aNumReq; i++)
            srcCfgValidInner = DMA_PlatGetCfg(aHwId, aIssuerId, 1UL,
                aXferReq[i].srcAddr, aXferReq[i].rowSize, aXferReq[i].srcAddrInc,
                &srcCfgInner
            )
            dstCfgValidInner = DMA_PlatGetCfg(aHwId, aIssuerId, 0UL,
                aXferReq[i].dstAddr, aXferReq[i].rowSize, aXferReq[i].dstAddrInc,
                &dstCfgInner
            )

            srcCfgCompatible = DMA_CheckCfgCompatible(&srcCfg, &srcCfgInner)
            dstCfgCompatible = DMA_CheckCfgCompatible(&dstCfg, &dstCfgInner)
            if ((BCM_ERR_OK == srcCfgValidInner) && (BCM_ERR_OK == dstCfgValidInner) &&
                (BCM_ERR_OK == srcCfgCompatible) && (BCM_ERR_OK == dstCfgCompatible))

                Compute ctrl register value from srcCfgInner and srcCfgInner params

                if (0UL == i)
                    aRegCfg->srcAddr = srcCfgInner.progBaseAddr
                    aRegCfg->dstAddr = dstCfgInner.progBaseAddr
                    aRegCfg->control = ctrl
                    nextAddr = &aRegCfg->next
                else
                    DMA_LinkedListType *lli = NULL;
                    lli = &DMA_LinkedListNodes[aHwId][aChNum][i - 1UL];
                    lli->srcAddr = srcCfgInner.progBaseAddr
                    lli->dstAddr = dstCfgInner.progBaseAddr
                    lli->control = ctrl
                    *nextAddr = DMA_PlatGetLliProgAddr(lli)
                    nextAddr = &lli->next

            else
                break

        if (aNumReq == i)
            *nextAddr = 0UL;
            ret = BCM_ERR_OK;
        else
            ret = BCM_ERR_NOSUPPORT
    else
        ret = BCM_ERR_NOSUPPORT

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
static int32_t DMA_GetConfig(uint32_t aHwId,
                             DMA_XferIssuerIdType const *aIssuerId,
                             DMA_XferReqType const *aXferReq,
                             uint32_t aNumReq,
                             uint32_t aChNum,
                             DMA_LinkedListType *aRegCfg,
                             DMA_Pl081CommonCfgType *aCommonCfg)
{
    int32_t ret;
    uint32_t flowCtrl = 0UL;
    bool isDMAFlowCtrlr = false;
    DMA_PlatCfgType srcCfg = {};
    DMA_PlatCfgType dstCfg = {};

    const int32_t srcCfgValid = DMA_PlatGetCfg(aHwId, aIssuerId, 1UL,
        aXferReq[0].srcAddr, aXferReq[0].rowSize, aXferReq[0].srcAddrInc,
        &srcCfg
    );
    const int32_t dstCfgValid = DMA_PlatGetCfg(aHwId, aIssuerId, 0UL,
        aXferReq[0].dstAddr, aXferReq[0].rowSize, aXferReq[0].dstAddrInc,
        &dstCfg
    );

    if ((BCM_ERR_OK == srcCfgValid) &&
        (BCM_ERR_OK == dstCfgValid) &&
        (BCM_ERR_OK == DMA_GetFlowCtrl(&srcCfg, &dstCfg, &flowCtrl, &isDMAFlowCtrlr))) {
        uint32_t i;
        uint32_t *nextAddr = &aRegCfg->next;

        aCommonCfg->flowCtrl = flowCtrl;
        aCommonCfg->srcPeriph = srcCfg.periphId;
        aCommonCfg->dstPeriph = dstCfg.periphId;

        for (i = 0UL; i < aNumReq; i++) {
            DMA_PlatCfgType srcCfgInner = {};
            DMA_PlatCfgType dstCfgInner = {};

            const int32_t srcCfgValidInner = DMA_PlatGetCfg(aHwId, aIssuerId, 1UL,
                aXferReq[i].srcAddr, aXferReq[i].rowSize, aXferReq[i].srcAddrInc,
                &srcCfgInner
            );
            const int32_t dstCfgValidInner = DMA_PlatGetCfg(aHwId, aIssuerId, 0UL,
                aXferReq[i].dstAddr, aXferReq[i].rowSize, aXferReq[i].dstAddrInc,
                &dstCfgInner
            );
            const int32_t srcCfgCompatible = DMA_CheckCfgCompatible(&srcCfg, &srcCfgInner);
            const int32_t dstCfgCompatible = DMA_CheckCfgCompatible(&dstCfg, &dstCfgInner);

            if ((BCM_ERR_OK == srcCfgValidInner) && (BCM_ERR_OK == dstCfgValidInner) &&
                (BCM_ERR_OK == srcCfgCompatible) && (BCM_ERR_OK == dstCfgCompatible)) {

                const uint32_t xferSize = (aXferReq[i].numRows * aXferReq[i].rowSize) / srcCfgInner.xferWidth;
                uint32_t ctrl =
                    (dstCfgInner.progIncMode << PL08X_DMACC0CONTROL_C0DESTINC_SHIFT)            |
                    (srcCfgInner.progIncMode << PL08X_DMACC0CONTROL_C0SRCINC_SHIFT)             |
                    (dstCfgInner.progXferWidth << PL08X_DMACC0CONTROL_C0DESTWIDTH_SHIFT)        |
                    (srcCfgInner.progXferWidth << PL08X_DMACC0CONTROL_C0SRCWIDTH_SHIFT)         |
                    (dstCfgInner.progBurstSize << PL08X_DMACC0CONTROL_C0DESTBURSTSIZE_SHIFT)    |
                    (srcCfgInner.progBurstSize << PL08X_DMACC0CONTROL_C0SRCBURSTSIZE_SHIFT)     |
                    (xferSize << PL08X_DMACC0CONTROL_C0TRANSFERSIZE_SHIFT);

                if (i == (aNumReq-1U)) {
                    /* Enable interrupt only for the last request */
                    ctrl |= PL08X_DMACC0CONTROL_C0I_MASK;
                }

                if (0UL == i) {
                    aRegCfg->srcAddr = srcCfgInner.progBaseAddr;
                    aRegCfg->dstAddr = dstCfgInner.progBaseAddr;
                    aRegCfg->control = ctrl;
                    nextAddr = &aRegCfg->next;
                } else {
                    DMA_LinkedListType *lli = NULL;
                    lli = &DMA_LinkedListNodes[aHwId][aChNum][i - 1UL];
                    lli->srcAddr = srcCfgInner.progBaseAddr;
                    lli->dstAddr = dstCfgInner.progBaseAddr;
                    lli->control = ctrl;
                    *nextAddr = DMA_PlatGetLliProgAddr(lli);
                    nextAddr = &lli->next;
                }

            } else {
                break;
            }
        }

        if (aNumReq == i) {
            *nextAddr = 0UL; /* Last linked list address is 0 */
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_NOSUPPORT;
        }

    } else {
        ret = BCM_ERR_NOSUPPORT;
    }

    return ret;
}

/** @brief Start a PL08X DMA transfer

    @behavior Sync, reentrant

    @pre DMA instance initalized

    @param[in]   aHwId          DMA Instance number
    @param[in]   aXferReq       List of transfer requests
    @param[in]   aNumReq        Number of transfer requests
    @param[in]   aEnableNotif   Non zero value indicates that the issuer
                                is to be notified after Xfer completion
    @param[in]   aIssuerId      Issuer identifier
    @param[out]  aJobHdl        Job handler for this transfer

    @retval     #BCM_ERR_OK           Transfer started successfully
    @retval     #BCM_ERR_UNINIT       Driver Not initialized
    @retval     #BCM_ERR_INVAL_PARAMS aHwId or aXferReq or aNumReq invalid
    @retval     #BCM_ERR_NOSUPPORT    Given request is not supported in this
                                      platform
    @retval     #BCM_ERR_NOMEM        No channel / memory slot for storing
                                      the request

    @post if successful, DMA transfer started

    @code{.unparsed}

    ret = BCM_ERR_OK

    validate parameters
        if invalid return BCM_ERR_INVAL_PARAMS

    ret = DMA_AllocChannel(aHwId, ..., &chNum)

    if BCM_ERR_OK
        ret = DMA_GetConfig(..., &regCfg, &commonCfg)

    if BCM_ERR_OK
        Set jobHdl, error value and taskid in the channel context
        CPU_MemoryBarrier()
        Setup DMA registers for the allocated channel
            Clear interrupts
            set source and destination address
            Setup linked list address [NOC address]
            Setup channel control register computed in the above loop
            Setup channel config register to start the DMA

        *aJobHdl = DMA_JOB_HDL_MAGIC | chNum

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_START_XFER_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
int32_t DMA_DrvStartXfer(uint32_t aHwId,
                                DMA_XferReqType const *aXferReq,
                                uint32_t aNumReq,
                                uint32_t aEnableNotif,
                                DMA_XferIssuerIdType const *aIssuerId,
                                BCM_HandleType *aJobHdl)
{
    uint32_t chNum = 0UL;
    int32_t ret = BCM_ERR_OK;
    DMA_LinkedListType regCfg = {};
    DMA_Pl081CommonCfgType commonCfg = {};
    uint32_t config = 0UL;

    if (aHwId >= PL08X_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NULL == aXferReq) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if ((0UL == aNumReq) || (DMA_MAX_NUM_XFER_REQ < aNumReq)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (0UL == DMA_REG[aHwId]->dmacconfiguration) {
        ret = BCM_ERR_UNINIT;
    } else {
        /* Do nothing */
    }

    if (BCM_ERR_OK == ret) {
        ret = DMA_AllocChannel(aHwId, aXferReq, aNumReq, &chNum);
    }

    if (BCM_ERR_OK == ret) {
        ret = DMA_GetConfig(aHwId, aIssuerId, aXferReq, aNumReq, chNum, &regCfg, &commonCfg);
    }

    if (BCM_ERR_OK == ret) {
        /* Program the DMA engine */
        DMA_Context.chan[aHwId][chNum].error = BCM_ERR_BUSY;
        DMA_Context.chan[aHwId][chNum].issuerId = *aIssuerId;
        DMA_Context.chan[aHwId][chNum].enableNotif = aEnableNotif;

        /* Memory barrier to ensure that all the writes to the memory are complete */
        /*CPU_MemoryBarrier();*/
        DMA_REG[aHwId]->dmacinttcclear = PL08X_DMACINTTCCLEAR_INTTCCLEAR_MASK;
        DMA_REG[aHwId]->dmacinterrclr  = PL08X_DMACINTERRCLR_INTERRCLR_MASK;
        DMA_REG[aHwId]->chan[chNum].dmacc0srcaddr = regCfg.srcAddr;
        DMA_REG[aHwId]->chan[chNum].dmacc0destaddr = regCfg.dstAddr;
        DMA_REG[aHwId]->chan[chNum].dmacc0lli = (regCfg.next & 0xF0FFFFFFUL);
        DMA_REG[aHwId]->chan[chNum].dmacc0control = regCfg.control;
        config =(PL08X_DMACC0CONFIGURATION_C0INTTCMASK_MASK |
                        PL08X_DMACC0CONFIGURATION_C0INTERRORMASK_MASK |
                        (commonCfg.flowCtrl << PL08X_DMACC0CONFIGURATION_C0FLOWCNTRL_SHIFT) |
                        (commonCfg.dstPeriph << PL08X_DMACC0CONFIGURATION_C0DESTPERIPHERAL_SHIFT) |
                        (commonCfg.srcPeriph << PL08X_DMACC0CONFIGURATION_C0SRCPERIPHERAL_SHIFT) |
                        PL08X_DMACC0CONFIGURATION_C0ENABLE_MASK) ;
        DMA_REG[aHwId]->chan[chNum].dmacc0configuration = config ;
        DMA_Context.handleCount++;
        *aJobHdl = DMA_JOB_HDL_MAGIC |
                   chNum             |
                   ((((uint64_t)DMA_Context.handleCount) <<
                       DMA_JOB_HDL_COUNT_SHIFT) &
                       DMA_JOB_HDL_COUNT_MASK);
        DMA_Context.chan[aHwId][chNum].jobHdl = *aJobHdl;
    }

    return ret;
}

/** @brief Get DMA transfer status

    @behavior Sync, reentrant

    @pre DMA transfer already started

    @param[in]   aHwId    DMA Instance number
    @param[in]   aJobHdl  DMA transfer job handle returned by the DMA_DrvStartXfer()

    @retval     #BCM_ERR_OK     DMA transfer completed successfully
    @retval     #BCM_ERR_BUSY   DMA transfer in progress
    @retval     #BCM_ERR_INVAL_PARAMS aHwId or aJobHdl is invalid
    @retval     #BCM_ERR_UNINIT Driver Not initialized

    @code{.unparsed}
    ret = BCM_ERR_OK
    if aHwId >= PL08X_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS
    elif aJobHdl == DMA_JOB_HDL_INVAL
        ret = BCM_ERR_INVAL_PARAMS
    elif DMA_Context.chan[aHwId][chNum].jobHdl != aJobHdl
        ret = BCM_ERR_INVAL_PARAMS
    elif DMA_REG[aHwId]->dmacconfiguration == 0
        ret = BCM_ERR_UNINIT
    elif chNum >= PL08X_CHAN_SIZE
        ret = BCM_ERR_INVAL_PARAMS
    elif DMA_Context.chan[aHwId][chNum].error == BCM_ERR_BUSY
        ret = BCM_ERR_BUSY
    else
        DMA_FreeLLConfig(DMA_Context.chan[aHwId][chNum].llIndex)
        DMA_Context.chan[aHwId][chNum].inUse = 0UL
        ret = DMA_Context.chan[aHwId][chNum].error

    return ret
    @endcode

    @trace #BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC
    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
int32_t DMA_DrvGetXferStatus(uint32_t aHwId, BCM_HandleType aJobHdl)
{
    int32_t ret;
    uint32_t chNum = (uint32_t)(aJobHdl&DMA_JOB_HDL_CHAN_MASK);

    if (aHwId >= PL08X_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (DMA_JOB_HDL_INVAL == aJobHdl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aJobHdl != DMA_Context.chan[aHwId][chNum].jobHdl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (0UL == DMA_REG[aHwId]->dmacconfiguration) {
        ret = BCM_ERR_UNINIT;
    } else if (chNum >= PL08X_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (BCM_ERR_BUSY == DMA_Context.chan[aHwId][chNum].error) {
        ret = BCM_ERR_BUSY;
    } else {
        DMA_Context.chan[aHwId][chNum].jobHdl = DMA_JOB_HDL_INVAL;
        ret = DMA_Context.chan[aHwId][chNum].error;
    }

    return ret;
}

/**
   @code{.unparsed}
   read tc interrupt status
   read error interrupt status

   clear the interrupts

   for each channel
    if channel in use
        if tc interrupt or error interrupt
            disable DMA (dmacc0configuration = 0)

        if error interrupt
            DMA_Context.chan[aHwId][chNum].error = BCM_ERR_UNKNOWN
        else
            DMA_Context.chan[aHwId][chNum].error = BCM_ERR_OK

        if (issuerNeedsNotification)
            if CPU_PRIVILEGE_LEVEL_USER == issuerPrivilage
                DMA_NotifyTask(DMA_Context.chan[aHwId][chNum].taskId)
            else
                DMA_NotifyComp(DMA_Context.chan[aHwId][chNum].issuerId.compId)
   @endcode

   @trace #BRCM_SWARCH_DMA_SERVICE_IRQ_PROC
   @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X
*/
void DMA_ServiceIRQ(uint32_t aHwId)
{
    uint32_t chNum;

    if (aHwId < PL08X_MAX_HW_ID) {
        uint32_t tcIntStat = DMA_REG[aHwId]->dmacinttcstatus;
        uint32_t errIntStat = DMA_REG[aHwId]->dmacinterrorstatus;

        /* Clear off the interrupts */
        DMA_REG[aHwId]->dmacinttcstatus = tcIntStat;
        DMA_REG[aHwId]->dmacinterrorstatus = errIntStat;

        for (chNum = 0UL; chNum < DMA_MAX_CHANNELS; chNum++) {
            if (DMA_JOB_HDL_INVAL == DMA_Context.chan[aHwId][chNum].jobHdl) {
                continue;
            }

            uint32_t tcIntMask = tcIntStat & (1UL << chNum);
            uint32_t errIntMask = errIntStat & (1UL << chNum);

            if ((0UL != tcIntMask) || (0UL != errIntMask)) {
                /* Disable DMA channel */
                DMA_REG[aHwId]->chan[chNum].dmacc0configuration = 0x0UL;

                if (0UL != errIntMask) {
                    DMA_Context.chan[aHwId][chNum].error = BCM_ERR_UNKNOWN;
                } else {
                    DMA_Context.chan[aHwId][chNum].error = BCM_ERR_OK;
                }

                /* Handle notifications */
                if (0UL != DMA_Context.chan[aHwId][chNum].enableNotif) {
                }
            }
        }
    } else {
        /*BCM_ASSERT(aHwId < PL08X_MAX_HW_ID);*/
    }
}
/** @} */
