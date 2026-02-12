/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_hsadcdrv_req Requirements
    @ingroup grp_hsadcdrv

    @addtogroup grp_hsadcdrv_req
    @{

    This is a requirement to implement the high speed ADC driver

    @file hsadc_req.c
    @brief Requirements for high speed ADC driver
    @version 0.1 Initial version
*/

/**
    @brief hsadc initialization and de-initialization

    hsadc driver shall provide a mechanism for initialization.
    hsadc driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_INIT

/**
    @brief high speed ADC configuration

    HSADC driver shall provide interfaces to configure ADC.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_CONFIGURATION

/**
    @brief high speed ADC FIR configuration

    HSADC driver shall provide interfaces to configure FIR co-efficient for HSADC .

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INIT
    @dependencies #BRCM_SWREQ_HSADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_FIR_CONFIGURATION

/**
    @brief high speed ADC FFT configuration

    HSADC driver shall provide interfaces to configure FFT for HSADC .

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INIT
    @dependencies #BRCM_SWREQ_HSADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_FFT_CONFIGURATION

/**
    @brief high speed ADC FFT get Axis information

    HSADC driver shall provide interfaces to get FFT axis information for HSADC .

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INIT
    @dependencies #BRCM_SWREQ_HSADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_FFT_GET_AXIS_INFORMATION

/**
    @brief high speed ADC calibration

    HSADC driver shall provide interfaces to start calibration.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INIT
    @dependencies #BRCM_SWREQ_HSADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_CALIBRATION

/**
    @brief high speed ADC Interrupts

    HSADC driver shall provide interfaces to configure, clear and get Interrupt status.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HSADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSADC_INIT
    @dependencies #BRCM_SWREQ_HSADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HSADC_INTERRUPT

/** @} */
