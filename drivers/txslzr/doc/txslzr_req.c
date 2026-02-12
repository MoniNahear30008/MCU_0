/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_txslzrdrv_req Requirements
    @ingroup grp_txslzrdrv

    @addtogroup grp_txslzrdrv_req
    @{

    @file txslzr_req.c
    @brief Requirements for TXSLZR driver
    This file contains the requirements for the BCM8915x TXSLZR driver
    @version 0.1 Initial version
*/

/**
    @brief Tx serializer initialization and de-initialization

    TXSLZR driver shall provide a mechanism for initialization.
    TXSLZR driver shall be de-initialized after all the operations
    are completed.

    @rationale
    Initialize TXSLZR peripheral

    @analysis
    Initialize TXSLZR driver

    @verification
    TXSLZR driver initialization and driver state need to be verified

    @dependencies #BRCM_SWREQ_TXSLZR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_TXSLZR_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_TXSLZR_INIT

/**
    @brief Tx serializer pattern generator memory access

    TXSLZR driver shall provide a mechanism for page configuration.

    TXSLZR driver shall provide a mechanism for Indirect memory
    read/write access control for Pattern Generator memory.

    @rationale
    Pattern generator memory access

    @analysis
    Pattern generator memory access

    @verification
    TXSLZR driver pattern generator memory access needs to be verified

    @dependencies #BRCM_SWREQ_TXSLZR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_TXSLZR_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_TXSLZR_PAT_GEN

/**
    @brief Tx serializer PRBS configuration

    TXSLZR driver shall provide a mechanism for configuring PRBS
    parameters and enable/disable of PRBS generator.

    @rationale
    PRBS configuration and enable/disable control

    @analysis
    PRBS configuration and enable/disable control

    @verification
    TXSLZR driver PRBS configuration needs to be verified

    @dependencies #BRCM_SWREQ_TXSLZR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_TXSLZR_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_TXSLZR_PRBS

/**
    @brief TXSLZR Interrupts

    TXSLZR driver shall provide interfaces to configure, clear and get Interrupt status.

    @rationale
    Interrupt related operation for TXSLZR driver

    @analysis
    Interrupt related operation for TXSLZR driver

    @verification
    Interrupt related API's needs to be verified.

    @dependencies #BRCM_SWREQ_TXSLZR_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_TXSLZR_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_TXSLZR_INTERRUPT

/**
    @brief TXSLZR sequencer

    TXSLZR driver shall provide interfaces to configure, read, write TXSLZR Sequencer SPI memory and SW SPI.

    @rationale
    The sequencer is used to generate signals based on a pre-programmed set of entries.

    @analysis
    The sequencer specifies which beam must be used in the next acquisition cycle (communicated via SPI) and the laser pulse
    pattern for the next laser pulse train.

    @verification
    Sequencer related API's needs to be verified by checking the generated signals.

    @dependencies #BRCM_SWREQ_TXSLZR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_TXSLZR_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_TXSLZR_SEQUENCER

/** @} */
