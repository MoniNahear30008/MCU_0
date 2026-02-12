/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_uconsoledrv_req Requirements
    @ingroup grp_uartdrv

    @addtogroup grp_uconsoledrv_req
    @{
    Requirements for UART Console

    @file uconsole_req.c
    @brief Requirements for UART Console

    @version 0.1 Initial version
*/

/**
    @brief Kernel Interface for UCONSOLE

    @todo Update details
    Detailed Description of what is needed from this requirement

    @rationale
    Reason why this requirement is needed

    @analysis
    Software Requirement Analysis shall consist of
        -# Correctness and Completeness of description
        -# Supporting information is captured well in rationale
        -# Technical Feasibility
        -# Identification of Technical Risks and Impact
        -# Identification of Dependent components/requirements
        -# Identification of components that can be reused
        -# Identification of impact on Memory, CPU, Hardware and Latency Budgets
        -# Verifiability

    @alternate_arch
    Alternate architectures (if applicable) evaluated during Architectural design
    shall be documented here.

    @verification
        -# Develop the verification criteria for each software
            requirement that define the qualitative and quantitative
            measures
        -# Describe any constraints for verification and alternate verification
            approaches.

    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @vtrace #UCONSOLE_RX_EVENT
    @vtrace #UCONSOLE_TX_EVENT
    @vtrace #UCONSOLE_TASK
*/
#define BRCM_SWREQ_UCONSOLE_KERNEL_INTERFACE

/**
    @brief Transmit and Receive characters over UART Console

    @todo Update details
    Detailed Description of what is needed from this requirement

    @rationale
    Reason why this requirement is needed

    @analysis
    Software Requirement Analysis shall consist of
        -# Correctness and Completeness of description
        -# Supporting information is captured well in rationale
        -# Technical Feasibility
        -# Identification of Technical Risks and Impact
        -# Identification of Dependent components/requirements
        -# Identification of components that can be reused
        -# Identification of impact on Memory, CPU, Hardware and Latency Budgets
        -# Verifiability

    @alternate_arch
    Alternate architectures (if applicable) evaluated during Architectural design
    shall be documented here.

    @verification
        -# Develop the verification criteria for each software
            requirement that define the qualitative and quantitative
            measures
        -# Describe any constraints for verification and alternate verification
            approaches.

    @dependencies #BRCM_SWREQ_UART_COMMUNICATION
    @dependencies #BRCM_SWREQ_UART_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_UCONSOLE_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @dependencies #BRCM_SWREQ_CIRCQ_CONFIGURATION
    @dependencies #BRCM_SWREQ_CIRCQ_INITIALIZATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @vtrace #UCONSOLE_TxDataBuffer
    @vtrace #UCONSOLE_TxDataIdx
    @vtrace #UCONSOLE_RxDataBuffer
    @vtrace #UCONSOLE_RxDataIdx
*/
#define BRCM_SWREQ_UCONSOLE_COMMUNICATION

/**
    @brief Kernel Handlers for UCONSOLE

    @todo Update details
    Detailed Description of what is needed from this requirement

    @rationale
    Reason why this requirement is needed

    @analysis
    Software Requirement Analysis shall consist of
        -# Correctness and Completeness of description
        -# Supporting information is captured well in rationale
        -# Technical Feasibility
        -# Identification of Technical Risks and Impact
        -# Identification of Dependent components/requirements
        -# Identification of components that can be reused
        -# Identification of impact on Memory, CPU, Hardware and Latency Budgets
        -# Verifiability

    @alternate_arch
    Alternate architectures (if applicable) evaluated during Architectural design
    shall be documented here.

    @verification
        -# Develop the verification criteria for each software
            requirement that define the qualitative and quantitative
            measures
        -# Describe any constraints for verification and alternate verification
            approaches.

    @dependencies #BRCM_SWREQ_UCONSOLE_COMMUNICATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_UART_INIT
    @dependencies #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @dependencies #BRCM_SWREQ_UCONSOLE_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
*/
#define BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER

/** @} */
