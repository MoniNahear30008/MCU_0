/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_ptpdrv_req Requirements
    @ingroup grp_xgmacdrv

    @addtogroup grp_ptpdrv_req
    @{

    @file ptp_req.c
    @brief Requirements for PTP driver
    This file contains the requirements for the BCM8915x PTP driver
    @version 0.1 Initial version
*/

/**
    @brief PTP initialization and de-initialization

    PTP driver shall provide a mechanism for initialization.
    PTP driver shall be de-initialized after all the operations are completed.

    @rationale
    Initialize PTP peripheral

    @analysis
    Initialize PTP driver

    @verification
    PTP driver initialization and driver state needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_PTP_INIT

/**
    @brief PTP operation mode configuration

    PTP driver shall provide a mechanism for configuring operation mode.

    @rationale
    To configure PTP operation mode

    @analysis
    To configure PTP operation mode

    @verification
    Configured operation modes needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_PTP_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_PTP_OPERATION_MODE

/**
    @brief Set PTP configuration

    PTP driver shall provide a mechanism to set PTP configuration.

    @rationale
    To set PTP configuration

    @analysis
    To set PTP configuration

    @verification
    PTP configuration needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_PTP_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_PTP_CONFIGURATION

/**
    @brief Automatic PTP Message Generation

    PTP driver shall provide a mechanism to generate automatic PTP message

    @rationale
    To generate automatic PTP message

    @analysis
    To generate automatic PTP message

    @verification
    PTP configuration needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_PTP_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_PTP_PKT_GENERATION

/**
    @brief Provides interface to initialize and configure PTP

    PTP driver provides API to Initialize and configure PTP driver.
    The API's should be portable, platform independent and MISRA complaint.

    @rationale
    PTP driver APIs initializes and configures PTP module.

    @analysis
    Initialization and configuration of PTP driver module.

    @verification
    PTP driver needs to be verified based on configuration settings.

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_PTP_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_PTP

/** @} */
