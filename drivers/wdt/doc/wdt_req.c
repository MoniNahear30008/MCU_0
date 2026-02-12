/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_wdtdrv_wdt_req Requirements
    @ingroup grp_wdtdrv

    @addtogroup grp_wdtdrv_wdt_req
    @{
    @section sec_wdtdrv_req_overview Requirements Overview

    This is a requirement to implement the WatchDog driver

    @file wdt_req.c
    @brief Requirements for WatchDog driver
    @version 0.1 Initial version
*/

/**
    @brief WDT initialization and de-initialization

    WDT driver shall provide a mechanism for initialization.

    @rationale
    Initialize WDT peripheral

    @analysis
    Initialize WDT driver

    @verification
    WDT driver initialization and driver state need to be verified

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_WDT_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5449, RGL-5450
*/
#define BRCM_SWREQ_WDT_INIT

/**
    @brief WDT Start operation

    Watch Dog driver shall provide a mechanism to start/stop.

    @rationale
    Watch Dog driver shall provide a mechanism to start/stop.

    @analysis
    Watch Dog driver shall provide a mechanism to start/stop.

    @verification
    Watch dog driver start/stop feature needs to be verified

    @dependencies #BRCM_SWREQ_WDT_INIT
    @dependencies #BRCM_SWREQ_WDT_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5451
*/
#define BRCM_SWREQ_WDT_DYNAMIC_CONFIG

/**
    @brief WDT Service operation

    Watch Dog driver shall provide a mechanism to service and restart.

    @rationale
    Watch Dog driver shall provide a mechanism to service and restart.

    @analysis
    Watch Dog driver shall provide a mechanism to service and restart.

    @verification
    The timers when started, MUST be periodically serviced (re-triggered) by software tasks

    @dependencies #BRCM_SWREQ_WDT_INIT
    @dependencies #BRCM_SWREQ_WDT_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_WDT_SERVICE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5451
*/
#define BRCM_SWREQ_WDT_SERVICE


/** @} */
