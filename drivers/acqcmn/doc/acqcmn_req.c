/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_acqcmndrv_req Requirements
    @ingroup grp_acqcmndrv

    @addtogroup grp_acqcmndrv_req
    @{

    @file acqcmn_req.c
    @brief Requirements for ACQCMN driver
    This file contains the requirements for the BCM8915x ACQCMN driver
    @version 0.1 Initial version
*/

/**
    @brief Acquisition common initialization and de-initialization

    ACQCMN driver shall provide a mechanism for initialization.
    ACQCMN driver shall be de-initialized after all the operations
    are completed.

    @rationale
    Initialize ACQCMN peripheral

    @analysis
    Initialize ACQCMN driver

    @verification
    ACQCMN driver initialization and driver state need to be verified

    @dependencies #BRCM_SWREQ_ACQCMN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_ACQCMN_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ACQCMN_INIT

/**
    @brief Acquisition common reset control

    ACQCMN driver shall provide a mechanism for resetting the block.

    @rationale
    Reset control

    @analysis
    Reset control

    @verification
    ACQCMN reset control needs to be verified

    @dependencies #BRCM_SWREQ_ACQCMN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ACQCMN_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ACQCMN_RESET_CONTROL

/**
    @brief Acquisition common clock control

    ACQCMN driver shall provide a mechanism for enable/disable the
    clock based on clock type

    @rationale
    enable/disable clock control

    @analysis
    enable/disable clock control

    @verification
    ACQCMN clock control configuration needs to be verified

    @dependencies #BRCM_SWREQ_ACQCMN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ACQCMN_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ACQCMN_CLOCK_CONTROL

/**
    @brief Acquisition common Axis configuration

    ACQCMN driver shall provide a mechanism to set/get Axis configuration

    @rationale
    Set/Get Axis configuration

    @analysis
    Set/Get Axis configuration

    @verification
    Axis configuration needs to be verified

    @dependencies #BRCM_SWREQ_ACQCMN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ACQCMN_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ACQCMN_AXIS_CONFIG

/**
    @brief Acquisition common source control configuration

    ACQCMN driver shall provide a mechanism for source control configuration

    @rationale
    Source control configuration

    @analysis
    source control configuration

    @verification
    source control configuration needs to be verified

    @dependencies #BRCM_SWREQ_ACQCMN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ACQCMN_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG

/**
    @brief ACQCMN Interrupts

    ACQCMN driver shall provide interfaces to configure, clear and get Interrupt status.

    @rationale
    Interrupt related operation for ACQCMN driver

    @analysis
    Interrupt related operation for ACQCMN driver

    @verification
    Interrupt related API's needs to be verified.

    @dependencies #BRCM_SWREQ_ACQCMN_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ACQCMN_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ACQCMN_INTERRUPT

/** @} */
