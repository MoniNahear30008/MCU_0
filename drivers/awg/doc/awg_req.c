/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_awgdrv_req Requirements
    @ingroup grp_awgdrv

    @addtogroup grp_awgdrv_req
    @{

    This is a requirement to implement arbitrary waveform generator driver

    @file awg_req.c
    @brief Requirements for arbitrary waveform generator driver.
    @version 0.1 Initial version
*/

/**
    @brief AWG initialization and de-initialization

    AWG driver shall provide a mechanism for initialization.
    AWG driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_AWG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_AWG_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE


*/
#define BRCM_SWREQ_AWG_INIT

/**
    @brief arbitrary waveform generator channel configuration

    AWG driver shall provide interfaces for channel configuration.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_AWG_INIT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_AWG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE


*/
#define BRCM_SWREQ_AWG_CH_CONFIGURATION

/**
    @brief Arbitrary Waveform Memory configuration

    AWG driver shall provide interfaces to load/retreive data to/from waveform memory.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_AWG_INIT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_AWG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE


*/
#define BRCM_SWREQ_AWG_WAVEFORM_MEM_CONFIG

/**
    @brief Get AWG phase

    AWG driver shall provide interfaces to get current AWG phase.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_AWG_INIT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_AWG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_AWG_GET_PHASE

/**
    @brief Configure AWG interrupt

    AWG driver shall provide interfaces to configure AWG interrupt.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_AWG_INIT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_AWG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_AWG_INTR_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_AWG_INTERRUPT

/** @} */
