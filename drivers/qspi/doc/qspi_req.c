/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_qspidrv_req Requirements
    @ingroup grp_qspidrv
    @addtogroup grp_qspidrv_req
    @{

    @file qspi_req.c
    @brief Requirements for QSPI driver
    This file contains the requirements for the BCM8915x QSPI driver
    @version 0.1
*/

/**
    @brief  Provides interface to initialize and configure QSPI HW.
    QSPI driver provides API to be configured as Master.
    -# Interface to communicate QSPI slave device
    -# Flash device access
    -# Method to send device specific commands

    @rationale
    This needs to cater to initialize and configure QSPI HW. It provides APIs to communicate with
    QSPI slave device or QSPI supported flash devices using transmit and receive APIs.

    @analysis
    The driver uses synchronous method to wait for command completion bit to set and clear.
    There is provision to enable interrupt for command completion but it is up to the application
    to handle asynchronous events inside the interrupt handler.

    @verification
    API verification with respect to interrupt status and device status is performed.

    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INIT
    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @dependencies #BRCM_SWREQ_QSPI_COMMUNICATION
    @dependencies #BRCM_SWREQ_QSPI_IRQ_HANDLER

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5570

*/
#define BRCM_SWREQ_QSPI

/**
    @brief QSPI Initialization

    @rationale
    Initialize QSPI peripheral with default configuration

    @analysis
    Initialization parameter is passed using configuration structure.

    @verification
    Device needs to be verified that it is ready to transmit and receive data.
    @dependencies #BRCM_SWREQ_QSPI_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BSPI_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5572, RGL-5575, RGL-5576
*/
#define BRCM_SWREQ_QSPI_DRIVER_INIT

/**
    @brief QSPI driver interface

    @rationale
    QSPI driver interface provides API support to change settings for selected feature.

    @analysis
    Change the settings using interface API after initialization is completed.

    @verification
    Device needs to be verified that it is ready to transmit and receive data.
    @dependencies #BRCM_SWREQ_QSPI_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BSPI_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5577, RGL-5578

*/
#define BRCM_SWREQ_QSPI_DRIVER_INTERFACE

/**
    @brief QSPI Communication APIs

    @rationale
    QSPI provides transmit and receive data from on chip QSPI slave device or external flash
    device using read/write APIs.

    @analysis
    Communication APIs has provision to pass device specific seetings.

    @verification
    Device needs to be verified that it is ready to transmit and receive data.
    @dependencies #BRCM_SWREQ_QSPI_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INIT
    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INTERFACE

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5583, RGL-5584, RGL-5580, RGL-5581
*/
#define BRCM_SWREQ_QSPI_COMMUNICATION

/**
    @brief QSPI IRQ handler

    @rationale
     QSPI interrupt handler provides mechanism to handle asynchronous events.
     It is the responsibilty of integration layer to register with platform specific
     interrupt handler.
    @analysis
     Asynchronous event handling.
    @verification
    Device needs to be verified that it is ready to transmit and receive data and events are generated
    asynchronously.
    @dependencies #BRCM_SWREQ_QSPI_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INIT
    @dependencies #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @dependencies #BRCM_SWREQ_QSPI_COMMUNICATION

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5585
*/

#define BRCM_SWREQ_QSPI_IRQ_HANDLER

/** @} */
