/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/** @defgroup grp_cachedrv_req Requirements
    @ingroup grp_cachedrv

    @addtogroup grp_cachedrv_req
    @{

    This is a requirement to implement the cache driver

    @file cache_req.c
    @brief Requirements for cache driver
    @version 0.1 Initial version
*/

/**
    @brief cache initialization and de-initialization

    Cache driver shall provide a mechanism for initialization.
    Cache driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CACHE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CLKRST_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_DDR_LLC_CMN_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CACHE_INIT

/**
    @brief Cache Configuration

    Cache driver shall provide interfaces to configure cache.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_CACHE_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CACHE_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_CACHE_CONFIGURE

/**
    @brief Cache maintenance operation

    Cache driver shall provide interfaces to cache maintenance operation.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_CACHE_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CACHE_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_CACHE_MAINTENANCE_OP

/**
    @brief Cache Performance monitor

    Cache driver shall provide interfaces to monitor cache performance.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_CACHE_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CACHE_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_CACHE_PERFORMANCE_MONITOR

/**
    @brief Cache functional safety controller(FSC)

    Cache driver shall provide interfaces to configure functional safety controller.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_CACHE_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CACHE_FSC_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_CACHE_FSC

/**
    @brief Cache Interrupts

    Cache driver shall provide interfaces to configure, clear and get Interrupt status.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_CACHE_INIT
    @dependencies #BRCM_SWREQ_CACHE_CONFIGURE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_CACHE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_SDC_LLC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_CACHE_INTERRUPT

/** @} */
