/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_dma_v2_pl08x_plat_req PL080 Platform Requirement
    @ingroup grp_dma_v2
    @addtogroup grp_dma_v2_pl08x_plat_req
    @{

    @file dma_v2_pl08x_plat_req.c
    @brief PL08X Platform layer Requirements

    @version 0.1 Initial Version
*/

/**
    @brief PL08X platform layer

    The platform layer should provide platform specific
    configurations needed for PL08X

    @rationale
    Different platform requires different configurations like
    burst size, peripheral id, etc.

    @analysis
    A common function shall be implemented for getting the
    configuration based on the source and destination address

    @verification
    -# Able to get configuration for different memory and peripheral address

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_PL08X_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_DMA_V2_KERNEL_INTERFACE
*/
#define BRCM_SWREQ_DMA_V2_PLAT_PL08X


/** @} */
