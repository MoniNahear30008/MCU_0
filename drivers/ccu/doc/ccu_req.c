/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_ccudrv_req Requirements
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_req
    @{
    @section grp_ccudrv_req_overview Requirements Overview

    This is a requirement to implement the CCU driver

    @file ccu_req.c
    @brief Requirements for CCU
    This file contains the requirements for CCU

    @version 1.0 Initial version
*/


/**
    @brief CCU Capture compare driver requirement

    @rationale
       Capture compare unit will be used to generate timing and PWM signals for scanner mechanism,
       laser drivers and acquisition control system with flexibly configurable timing.

    @analysis
       Capture compare unit (CCU) provides APIs to select input trigger event from acquisition block, CCU block
       also have APIs to configure events and input signals.
    @verification
       Event counting, timestamping, period measurement and frequency measurement need to be verified.
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_CCU_CONFIGURATION
    @dependencies #BRCM_SWREQ_CCT_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CCU

/**
    @brief CCU Kernel Interface

    @rationale
       Use interrupt mode for low latency.
    @analysis
       Interrupt generation on compare event, capture event.
    @verification
       Periodic time interval based on subsequent timer interrupt, capture event and interrupt on
       matching programmed compare value, timestamp etc.
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_CCU_CONFIGURATION
    @dependencies #BRCM_SWREQ_CCU
    @dependencies #BRCM_SWREQ_CCT_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CCU_KERNEL_HANDLER

/**
    @brief CCU Configuration

    @rationale
       CCU should provide APIs to configure input pins to trigger capture on rising edge/falling edge.
       CCU should provide API to configure compare values to be compared with programmed prescalar
       values.
    @analysis
       Capture event and compare value programming need to be provided by the CCU configuration.
    @verification
       Capture and compare event need to be verified for various applications.

    @dependencies #BRCM_SWREQ_CCT_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @vtrace #BCM_STATE_INIT
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CCU_CONFIGURATION

/** @} */
