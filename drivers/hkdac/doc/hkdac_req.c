/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_hkdacdrv_req Requirements
    @ingroup grp_hkdacdrv

    @addtogroup grp_hkdacdrv_req
    @{

    @file hkdac_req.c
    @brief Requirements for house keeping ADC driver
    This is a requirement to implement the House Keeping DAC driver
    @version 0.1 Initial version
*/

/**
    @brief hkdac initialization and de-initialization

    hkdac driver shall provide a mechanism for initialization.
    hkdac driver shall be de-initialized after all the operations
    are completed.

    @rationale
    Initialize HKDAC peripheral

    @analysis
    Initialize HKDAC driver

    @verification
    HKDAC driver initialization and driver state need to be verified

    @dependencies #BRCM_SWREQ_HKDAC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKDAC_INIT

/**
    @brief house keeping DAC configuration

    HKDAC driver shall provide interfaces to configure ADC.

    @rationale
    Configure HKDAC peripheral

    @analysis
    Configure HKDAC driver

    @verification
    HKDAC driver configuration needs to be verified

    @dependencies #BRCM_SWREQ_HKDAC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HKDAC_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HKDAC_CONFIGURATION

/**
    @brief house keeping DAC Interrupt configuration

    HKDAC driver shall provide interfaces to configure Interrupt.

    @rationale
    Interrupt related operation for HKDAC driver

    @analysis
    Interrupt related operation for HKDAC driver

    @verification
    Interrupt related API's needs to be verified.

    @dependencies #BRCM_SWREQ_HKDAC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKDAC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HKDAC_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HKDAC_INTERRUPT


/** @} */
