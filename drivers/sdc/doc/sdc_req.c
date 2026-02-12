/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_sdcdrv_req Requirements
    @ingroup grp_sdcdrv

    @addtogroup grp_sdcdrv_req
    @{

    This is a requirement to implement the SDC driver

    @file sdc_req.c
    @brief Requirements for SDC driver
    @version 0.1 Initial version
*/

/**
    @brief SDC initialization and de-initialization

    SDC driver shall provide a mechanism for initialization.
    SDC driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_SDC_CSR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_DDR_LLC_CMN_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_SDC_INIT

/**
    @brief SDC secure memory area configuration and enable

    SDC driver shall provide interfaces to configure and control secure memory area.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_SDC_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_SDC_CSR_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_SDC_PWR_SAVING

/**
    @brief SDC secure memory area configuration and enable

    SDC driver shall provide interfaces to configure and control secure memory area.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_SDC_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_SDC_CSR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_DDR_LLC_CMN_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_SDC_SECURE_AREA

/**
    @brief SDC diagnostic operation

    SDC driver shall provide interfaces to SDC diagnostic operation.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_SDC_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_SDC_CSR_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_SDC_DIAG

/** @} */
