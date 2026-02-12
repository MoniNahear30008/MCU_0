/*****************************************************************************
  Copyright 2024 Broadcom Limited.  All rights reserved.
  Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_dma_v2_ifc PL08X - Driver
    @ingroup grp_dma_v2

    @addtogroup grp_dma_v2_ifc
    @{
    @section sec_dma_overview Overview
    -# Peripheral DMA Bare Metal driver interface provides layer 1 APIs to initialize and configure
    the PL08x in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each PL08x HW instance needs to be initialized separately using DMA_DrvInit API.
    -# User can get the allocated channel number (bit number 0-7) from aJobHdl. 
    -# Each DMA channel has a specific hardware priority. Please refer to TRM [TDB].
    
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_dma_v2_fsm Usage
    -# PL08x HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   DMAC_0      |
       | 1           |   DMAC_1      |
    @section api_list List of PLO8X APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref DMA_DrvInit | Initialize the DMA instance |
    | @ref DMA_DrvStartXfer | Start a DMA transfer, issuer is executing in the un-privilaged CPU mode |
    | @ref DMA_DrvGetXferStatus | Get DMA transfer status |
    | @ref DMA_DrvDeinit | Deinitialize the DMA instance |

    @file dma_v2.h

    @brief Interface for PL08x 
    @section apis This section defines the interface for Pl08x driver

    @version 0.1 Initial revision
*/
#ifndef DMA_V2_H
#define DMA_V2_H

#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <bcm_comp.h>
#include <bcm_err.h>

/**
    @name DMA V2 Architecture IDs
    @{
    @brief Architecture IDs for DMA V2
*/
#define BRCM_SWARCH_DMA_MAX_NUM_XFER_REQ_MACRO  (0x8000U) /**< @brief #DMA_MAX_NUM_XFER_REQ */
#define BRCM_SWARCH_DMA_XFER_REQ_TYPE           (0x8001U) /**< @brief #DMA_XferReqType      */
#define BRCM_SWARCH_DMA_XFER_ISSUER_ID_TYPE     (0x8002U) /**< @brief #DMA_XferIssuerIdType      */
#define BRCM_SWARCH_DMA_INIT_PROC               (0x8003U) /**< @brief #DMA_DrvInit             */
#define BRCM_SWARCH_DMA_START_XFER_PROC         (0x8004U) /**< @brief #DMA_DrvStartXfer        */
#define BRCM_SWARCH_DMA_GET_XFER_STATUS_PROC    (0x8006U) /**< @brief #DMA_DrvGetXferStatus    */
#define BRCM_SWARCH_DMA_DEINIT_PROC             (0x8007U) /**< @brief #DMA_DrvDeinit           */
#define BRCM_SWARCH_DMA_SERVICE_IRQ_PROC        (0x8008U) /**< @brief #DMA_ServiceIRQ          */
/** @} */

/**
    @brief Maximum number of Xfer requests supported by @ref DMA_DrvStartXfer APIs

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
 */
#define DMA_MAX_NUM_XFER_REQ                 (9UL)

/**
    @brief DMA Transfer data structure

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
*/
typedef struct sDMA_XferReqType {
    uint32_t srcAddr;     /**< source address */
    uint32_t dstAddr;     /**< destination address */
    uint32_t rowSize;     /**< DMA transfer size in bytes */
    uint32_t numRows;     /**< Number of rows to be transferred */
    uint32_t srcAddrInc;  /**< Source address increment in bytes */
    uint32_t dstAddrInc;  /**< Destination address increment in bytes */
} DMA_XferReqType;

/**
    @brief Xfer Issuer Identifier

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
*/
typedef struct sDMA_XferIssuerIdType {
    uint32_t                taskId;             /**< @brief TaskID of the issuer                            */
    /*BCM_CompIDType          compId;    */         /**< @brief Component ID of the issuer                      */
    uint32_t                issuerPrivilage;    /**< @brief CPU privilage level of the Xfer issuing code 0-user 1-Privilege mode */
} DMA_XferIssuerIdType;

/** @brief Initialize the DMA instance

    @behavior Sync, reentrant

    @pre DMA instance not initialized

    @param[in]   aHwId    DMA Instance number

    @retval     #BCM_ERR_OK           Init successful
    @retval     #BCM_ERR_INVAL_PARAMS aHwId invalid
    @retval     #BCM_ERR_INVAL_STATE  Already initialized

    @post if successful, DMA instance initialized

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X

    @limitations None
*/
int32_t DMA_DrvInit(uint32_t aHwId);

/** @brief Start a DMA transfer, issuer is executing in the un-privilaged CPU mode

    @behavior Sync, reentrant

    @pre DMA instance initalized

    @param[in]   aHwId    DMA Instance number
    @param[in]   aXferReq List of transfer requests
                          Cannot be greater than @ref DMA_MAX_NUM_XFER_REQ
    @param[in]   aNumReq  Number of transfer requests
    @param[in]   aEnableNotif   Non zero value indicates that the issuer
                                is to be notified after Xfer completion
    @param[in]   aIssuerId      Issuer identifier
    @param[out]  aJobHdl  Job handler for this transfer

    @retval     #BCM_ERR_OK           Transfer started successfully
    @retval     #BCM_ERR_UNINIT       Driver Not initialized
    @retval     #BCM_ERR_INVAL_PARAMS aHwId or aXferReq or aNumReq is invalid
    @retval     #BCM_ERR_INVAL_STATE  If the issuer is not executing in the
                                      un-privilaged CPU mode
    @retval     #BCM_ERR_NOSUPPORT    Given request is not supported in this
                                      platform
    @retval     #BCM_ERR_NOMEM        No free channels to start this DMA request

    @post if successful, DMA transfer started

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER

    @limitations None
*/
int32_t DMA_DrvStartXfer(uint32_t aHwId,
                         DMA_XferReqType const *aXferReq,
                         uint32_t aNumReq,
                         uint32_t aEnableNotif,
                         DMA_XferIssuerIdType const *aIssuerId,
                         BCM_HandleType *aJobHdl);


/** @brief Get DMA transfer status

    @behavior Sync, reentrant

    @pre DMA transfer already started

    @param[in]   aHwId    DMA Instance number
    @param[in]   aJobHdl  DMA transfer job handle returned by the DMA_DrvStartXfer()

    @retval     #BCM_ERR_OK     DMA transfer completed successfully
    @retval     #BCM_ERR_BUSY   DMA transfer in progress
    @retval     #BCM_ERR_INVAL_PARAMS aHwId or aJobHdl is invalid
    @retval     #BCM_ERR_UNINIT Driver Not initialized

    @post None

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER

    @limitations None
*/
int32_t DMA_DrvGetXferStatus(uint32_t aHwId, BCM_HandleType aJobHdl);

/** @brief Deinitialize the DMA instance

    @behavior Sync, reentrant

    @pre DMA instance is initialized

    @param[in]  aHwId    DMA Instance number

    @retval     #BCM_ERR_OK           Deinit successful
    @retval     #BCM_ERR_INVAL_PARAMS aHwId invalid
    @retval     #BCM_ERR_UNINIT       Not initialized
    @retval     #BCM_ERR_BUSY         Transfer in progress

    @post if successful, DMA instance de-initialized

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X

    @limitations None
*/
int32_t DMA_DrvDeinit(uint32_t aHwId);

/** @brief Service the DMA interrupt

    This function implemented in driver layer services the
    interrupts for the given HW id.

    @behavior Sync, Non-reentrant

    @param[in]      aHwId    Hardware ID

    @return     void

    @trace #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER

    @limitations None
*/
void DMA_ServiceIRQ(uint32_t aHwId);

#endif /* DMA_V2_H */

/** @} */
