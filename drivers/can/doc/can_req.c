/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_candrv_req Requirements
    @ingroup grp_candrv
    @addtogroup grp_candrv_req
    @{

    @file can_req.c
    @brief Requirements for CAN driver
    This file contains the requirements for BCM8915x CAN driver
    @version 1.0 Changelist1
*/

/**
    @brief CAN kernel interface
      CAN driver provides TX and RX interfaces to configure message RAM for TX BUFFER/FIFO, RX FIFO messages.
    @rationale
       CAN driver provides interface to configure message RAM for TX FIFO, RX FIFO and configure message
       filter.

    @analysis
    CAN Driver Core consists of functions for RX Handling, TX Handling, and Interrupt handling along with Error and Wakeup handling.
    All functions concerning handling of messages are implemented by the RX Handler and the TX Handler components of the CAN Controller.

    @verification
      Verify standard filter and extended filter configuration in transmit and receive functions.
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CAN_KERNEL_INTERFACE

/**
    @brief CAN Initialization
       Configure CAN with message RAM configuration and filter configuration
    @rationale
       Initialize and enable can controller hardware. CAN block should be configured in a state so that it is
       ready to transmit and receive the data.
    @analysis
       CAN driver should be initialized before any other CAN driver interface is called.
    @verification
       CAN hardware should be in enabled state to transmit and receive data.
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CAN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CAN_KERNEL_INTERFACE

    @vtrace #CAN_RXPKTQ_SIZE
    @vtrace #CAN_ERRPKTQ_SIZE
    @vtrace #ENABLE_CAN_MCAN
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CAN_INIT

/**
    @brief CAN communication support

    @rationale
      CAN should be able to transmit data and transmit completion is notified.
      Similarily
    @analysis
       The TX Handler is responsible for the transfer of transmit messages from the Message RAM to the CAN Core as well as providing transmit status information.
       The RX Handler manages message acceptance filtering, the transfer of received messages from the CAN Core to the Message RAM
       as well as providing receive message status information. Deferred processing of the data is done by calling CAN_DrvProcessPkts in client task.
    @verification
      CAN core should be able to transmit and recieve messages.
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_CAN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CAN_INIT
    @dependencies #BRCM_SWREQ_CAN_NOTIFICATION
    @dependencies #BRCM_SWREQ_CAN_KERNEL_INTERFACE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_CAN_COMMUNICATION

/**
    @brief CAN driver power mode support

    @rationale
      CAN driver should provide feature to go in sleep and wakeup for power saving.
    @analysis
      CAN controller to Low-Power/Sleep mode and brings the CAN controller out of Low-Power/Sleep mode.
    @verification
       Sleep and wakeup mode need to be verified.
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_CAN_INIT
    @dependencies #BRCM_SWREQ_CAN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CAN_KERNEL_INTERFACE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CAN_POWER

/**
    @brief CAN notifications

    @rationale
       Transmit completion, RX and error need to be notified to the client so that it can take appropriate action.
    @analysis
       Client gets notification of the asynchronous events, it is user responsibility to register callbacks to get notifications.
    @verification
       Transmit and receive status need to be checked once tx/rx callbacks are called.
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_CAN_INIT
    @dependencies #BRCM_SWREQ_CAN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CAN_KERNEL_INTERFACE
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CAN_NOTIFICATION

/**
    @brief CAN Kernetl handler

    @rationale
      Use interrupt mode for low latency.
    @analysis
      Interrupts for transmit, receive and error need to be handled in interrupt handler and same
      need to be notified.
    @verification
      Transmit, receive and error interrupt need to be verified.
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CAN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CAN_INIT
    @dependencies #BRCM_SWREQ_CAN_COMMUNICATION
    @dependencies #BRCM_SWREQ_CAN_POWER
    @dependencies #BRCM_SWREQ_CAN_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER

    @vtrace #CAN_EVENT
    @vtrace #CAN_IRQ_PROCESS_TASK
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/

#define BRCM_SWREQ_CAN_KERNEL_HANDLER

/** @} */
