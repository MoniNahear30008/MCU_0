/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_dma_v2_pl08x_req PL08X DMA Driver Requirement
    @ingroup grp_dma_v2
    @addtogroup grp_dma_v2_pl08x_req
    @{

    @file dma_v2_pl08x_req.c
    @brief PL08X DMA Driver Requirements

    @version 0.1 Initial Version
*/

/**
    @brief PL08X DMA driver kernel handler

    PL08X DMA diver shall implement a handler function that shall be invoked
    from the privileged mode (kernel space).
    This handler shall implement, PL08X IP init, deinit and transfer functions.

    @rationale
    PL08X ip is present in various platforms that can be used for transferring
    data between memory - memory and memory - peripheral in the background.

    @analysis
    Command handler shall support init, deinit, start transfer functions.

    @verification
    -# Able to initialize deinitialize the pl08x ip
    -# Able to Start transfer and get status

    @vtrace #DMA_LLINODES_MEM_SECTION

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_DMA_V2_PLAT_PL08X
    @dependencies #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_DMA_V2_KERNEL_HANDLER
    @dependencies #BRCM_SWREQ_PL08X_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
*/
#define BRCM_SWREQ_DMA_V2_KERNEL_HANDLER_PL08X


/** @} */
