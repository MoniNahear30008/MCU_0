/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_nsudrv_req Requirements
    @ingroup grp_nsudrv

    @addtogroup grp_nsudrv_req
    @{

    This is a requirement to implement the Network Synchronization Unit driver

    @file nsu_req.c
    @brief Requirements for NSU driver
    @version 0.1 Initial version
*/

/**
    @brief nsu initialization and de-initialization

    NSU driver shall provide a mechanism for initialization.
    NSU driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_NSU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_NSU_INIT

/**
    @brief NSU reset configuration

    NSU driver shall provide interfaces to configure reset source and reset.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_NSU_INIT
    @dependencies #BRCM_SWREQ_NSU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_NSU_RESET

/**
    @brief NSU counter

    NSU driver shall provide interfaces to read NSU binary and 1588 counters.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_NSU_INIT
    @dependencies #BRCM_SWREQ_NSU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_NSU_COUNTER

/**
    @brief NSU Event

    NSU driver shall provide interfaces to configure and read event counters of generation and capture unit.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_NSU_INIT
    @dependencies #BRCM_SWREQ_NSU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_NSU_EVENT

/**
    @brief NSU System tick

    NSU driver shall provide interfaces to configure and read counters of system time tick.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_NSU_INIT
    @dependencies #BRCM_SWREQ_NSU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/

#define BRCM_SWREQ_NSU_SYSTICK


/** @} */
