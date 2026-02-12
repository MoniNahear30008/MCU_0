/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_hkadcdrv_req Requirements
    @ingroup grp_hkadcdrv

    @addtogroup grp_hkadcdrv_req
    @{

    This is a requirement to implement the House Keeping ADC driver

    @file hkadc_req.c
    @brief Requirements for house keeping ADC driver
    @version 0.1 Initial version
*/

/**
    @brief hkadc initialization and de-initialization

    hkadc driver shall provide a mechanism for initialization.
    hkadc driver shall be de-initialized after all the operations
    are completed.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKADC_INIT

/**
    @brief house keeping ADC configuration

    HKADC driver shall provide interfaces to configure ADC.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HKADC_INIT

    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKADC_CONFIGURATION

/**
    @brief house keeping ADC comparator configuration

    HKADC driver shall provide interfaces to configure HKADC comparator.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HKADC_INIT
    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION

/**
    @brief house keeping ADC calibration

    HKADC driver shall provide interfaces to start calibration.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HKADC_INIT
    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_HKADC_CALIBRATION

/**
    @brief house keeping ADC sequencer configuration

    HKADC driver shall provide interfaces to configure sequencer.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HKADC_INIT
    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKADC_SEQ_CONFIGURATION

/**
    @brief house keeping ADC Interrupt configuration

    HKADC driver shall provide interfaces to configure Interrupt.

    @todo rationale

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_HKADC_INIT
    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKADC_INTERRUPT

/**
    @brief house keeping ADC Sequencer

    HKADC driver shall provide interfaces to configure, read, write HKADC Sequencer SPI memory and SW SPI.

    @rationale
    The sequencer is configured with diffrent sequences. Each of the sequence step instructs 
    the capture of the signal and then configure the next settings of mux/gain selection for the next step.
    The sequencer supports a total of 32 sequences.

    @analysis
    The sequencer specifies which beam must be used in the next acquisition cycle (communicated via SPI) and the laser pulse
    pattern for the next laser pulse train.

    @verification
    Sequencer related API's needs to be verified by checking the generated signals.

    @dependencies #BRCM_SWREQ_HKADC_INIT
    @dependencies #BRCM_SWREQ_HKADC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_INTR_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_HKADC_CONFIGURATION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_HKADC_SEQUENCER

/** @} */
