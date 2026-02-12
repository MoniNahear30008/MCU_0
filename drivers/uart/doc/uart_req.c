/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_uartdrv_req Requirements
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_req
    @{

    This is a requirement to implement the UART driver

    @file uart_req.c
    @brief Requirements for UART driver
    @version 0.1 Initial version
*/

/**
    @brief UART initialization and de-initialization

    UART driver shall provide a mechanism for initialization.
    UART driver shall be de-initialized after all the operations
    are completed.

    @rationale
    Configure UART parameters during initialization

    @analysis
    Configure UART parameters during initialization

    @verification
    UART Configuration needs to be verified

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_UART_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5592, RGL-5593, RGL-5594

*/
#define BRCM_SWREQ_UART_INIT

/**
    @brief UART communication

    UART driver shall provide interfaces to send and receive data.

    @analysis
    Data reception shall be notified using a callback

    @verification
    Data communication through UART needs to be verified

    @dependencies #BRCM_SWREQ_UART_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_UART_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5598
*/
#define BRCM_SWREQ_UART_COMMUNICATION

/**
    @brief UART interrupts

    UART driver shall provide interfaces to enable, clear and get Interrupt status.

    @rationale
    Interrupt related operation for UART driver

    @analysis
    Interrupt related operation for UART driver

    @verification
    Interrupt related APIs need to be verified.

    @dependencies #BRCM_SWREQ_UART_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_UART_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety yes
    @systemrequirement RGL-5599
*/
#define BRCM_SWREQ_UART_INTERRUPT


/** @} */
