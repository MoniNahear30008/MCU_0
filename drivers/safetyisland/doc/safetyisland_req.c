/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_safetyislanddrv_req Requirements
    @ingroup grp_safetyislanddrv

    @addtogroup grp_safetyislanddrv_req
    @{

    @file safetyisland_req.c
    @brief Requirements for SAFETYISLAND driver
    This file contains the requirements for the BCM8915x SAFETYISLAND driver
    @version 0.1 Initial version
*/

/**
    @brief SAFETYISLAND initialization and de-initialization

    SAFETYISLAND driver shall provide a mechanism for initialization.
    SAFETYISLAND driver shall be de-initialized after all the operations are completed.

    @rationale
    Initialize SAFETYISLAND peripheral

    @analysis
    Initialize SAFETYISLAND fault driver

    @verification
    SAFETYISLAND driver initialization and driver state needs to be verified

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_INIT

/**
    @brief SAFETYISLAND fault control configuration APIs

    SAFETYISLAND driver shall provide mechanism to perform fault control configuration details.

    @rationale
    Fault control configuration of safety island driver

    @analysis
    Fault control configuration of safety island driver.

    @verification
    Fault control configuration needs to be verified.

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG

/**
    @brief Fault mask configuration, retrieve fault status and fault clear APIs
    SAFETYISLAND driver shall provide mechanism to configure faults type based on fault mask.
    SAFETYISLAND driver shall provide mechanism to retrieve fault status based on fault mask.
    SAFETYISLAND driver shall provide mechanism to clear fault status based on fault mask.

    @rationale
    To configure fault type, retrieve fault status and clear faults

    @analysis
    To configure fault mask, retrieve fault status and clear faults

    @verification
    Fault and Fault safety status needs to be verified based on configured mask

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR

/**
    @brief Retrieves built in self repair status

    SAFETYISLAND driver shall provide mechanism to retrieve built in self repair status.

    @rationale
    To retrieve built in self repair status.

    @analysis
    Retrieve built in self repair status.

    @verification
    Built in self repair status needs to be verified

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_BISR_STATUS

/**
    @brief Retrieves safety fault status and clears safety faults

    SAFETYISLAND driver shall provide mechanism to retrieve safety fault status and clear safety faults

    @rationale
    To retrieve safety fault status and clear safety faults

    @analysis
    Retrieves safety fault status and clears safety faults

    @verification
    Retrieving of safety fault status and clearing of safety faults needs to be verified

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR

/**
    @brief Generation of CPU reset and fault

    Generates CPU reset/fault and send out to the pin.

    @rationale
    CPU reset and fault generation

    @analysis
    CPU reset request, cpu reset and fault generation

    @verification
    CPU reset request, reset and fault generation needs to be verified

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_CPU_ACTION

/**
    @brief Fault safety debug register access

    SAFETYISLAND driver shall provide mechanism to access fault safety debug registers.

    @rationale
    To access fault safety debug registers

    @analysis
    To access fault safety debug registers

    @verification
    Fault safety debug register read/write operation needs to be verified.

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND_DEBUG

/**
    @brief Provides interface to initialize and configure SAFETYISLAND

    SAFETYISLAND driver provides API to Initialize and configure safetyisland driver.
    The API's should be portable, platform independent and MISRA complaint.

    @rationale
    SAFETYISLAND driver APIs initializes and configures safetyisland module.

    @analysis
    Initialization and configuration of SAFETYISLAND driver module.

    @verification
    Safetyisland driver needs to be verified based on configuration settings.

    @dependencies #BRCM_SWREQ_SAFETYISLAND_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_SAFETYISLAND_INIT

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5713
*/
#define BRCM_SWREQ_SAFETYISLAND

/** @} */
