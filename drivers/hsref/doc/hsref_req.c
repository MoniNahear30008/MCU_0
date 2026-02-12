/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_hsrefdrv_req Requirements
    @ingroup grp_hsrefdrv

    @addtogroup grp_hsrefdrv_req
    @{

    @file hsref_req.c
    @brief Requirements for HSREF driver
    This file contains the requirements for the BCM8915x HSREF driver
    @version 0.1 Initial version
*/

/**
    @brief hsref initialization and de-initialization

    hsref driver shall provide a mechanism for initialization.
    hsref driver shall be de-initialized after all the operations
    are completed.

    @rationale
    Initialize HSREF peripheral

    @analysis
    Initialize HSREF driver

    @verification
    HSREF driver initialization and driver state needs to be verified

    @dependencies #BRCM_SWREQ_HSREF_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HSREF_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSREF_INIT

/**
    @brief HSREF ADC calibration

    HSREF driver shall provide interfaces to initialize calibration.
    HSREF driver shall provide interfaces to trigger calibration.
    HSREF driver shall provide interfaces to get calibration status.

    @rationale
    Calibration related operation for HSREF driver

    @analysis
    Calibration related operation for HSREF driver

    @verification
    Calibration related API's needs to be verified.

    @dependencies #BRCM_SWREQ_HSREF_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSREF_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSREF_CALIBRATION

/**
    @brief HSREF state variable filter configuration

    HSREF driver shall provide interfaces to configure SVF.
    HSREF driver shall provide interfaces to control enable/disable of SVF.

    @rationale
    SVF related operation for HSREF driver

    @analysis
    SVF related operation for HSREF driver

    @verification
    SVF related API's needs to be verified.

    @dependencies #BRCM_SWREQ_HSREF_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSREF_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER

/**
    @brief HSREF zero cross circuit configuration

    HSREF driver shall provide interfaces to configure zero cross circuit.
    HSREF driver shall provide interfaces to control enable/disable of zero cross circuit.
    HSREF driver shall provide interfaces to control zero cross FIFO.
    HSREF driver shall provide interfaces to get zero cross FIFO status.
    HSREF driver shall provide interfaces to read zero cross FIFO.

    @rationale
    Zero cross circuit related operation for HSREF driver

    @analysis
    Zero cross circuit related operation for HSREF driver

    @verification
    Zero cross circuit related API's needs to be verified.

    @dependencies #BRCM_SWREQ_HSREF_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSREF_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSREF_ZERO_CROSS

/**
    @brief HSREF capture buffer configuration

    HSREF driver shall provide interfaces to configure capture buffer.
    HSREF driver shall provide interfaces to control enable/disable of capture buffer.
    HSREF driver shall provide interfaces to control capture buffer FIFO.
    HSREF driver shall provide interfaces to get capture buffer FIFO status.

    @rationale
    Capture buffer related operation for HSREF driver

    @analysis
    Capture buffer related operation for HSREF driver

    @verification
    capture buffer related API's needs to be verified.

    @dependencies #BRCM_SWREQ_HSREF_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSREF_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSREF_CAPTURE_BUFFER

/**
    @brief HSREF Interrupts

    HSREF driver shall provide interfaces to configure, clear and get Interrupt status.

    @rationale
    Interrupt related operation for HSREF driver

    @analysis
    Interrupt related operation for HSREF driver

    @verification
    Interrupt related API's needs to be verified.

    @dependencies #BRCM_SWREQ_HSREF_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSREF_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HSREF_INTERRUPT

/** @} */
