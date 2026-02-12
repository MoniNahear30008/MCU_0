/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_hsafedrv_req Requirements
    @ingroup grp_hsafedrv

    @addtogroup grp_hsafedrv_req
    @{

    This is a requirement to implement the High Speed Analog Front End driver

    @file hsafe_req.c
    @brief Requirements for High Speed AFE driver
    @version 0.1 Initial version
*/

/**
    @brief hsafe initialization and de-initialization

    HSAFE driver shall provide a mechanism for initialization.
    HSAFE driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSAFE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSAFE_INIT

/**
    @brief HSAFE PLL configuration

    HSAFE driver shall provide interfaces to configure PLL.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSAFE_INIT
    @dependencies #BRCM_SWREQ_HSAFE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSAFE_PLL_CONFIGURATION

/**
    @brief HSAFE ADC configuration

    HSAFE driver shall provide interfaces to configure HSAFE for ADC(HSADC and HSREF ADC), DAC and
    serializer.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSAFE_INIT
    @dependencies #BRCM_SWREQ_HSAFE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

/**
    @brief HSAFE calibration

    HSAFE driver shall provide interfaces to calibrate.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSAFE_INIT
    @dependencies #BRCM_SWREQ_HSAFE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSAFE_CALIBRATION


/** @} */
