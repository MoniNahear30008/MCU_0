/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential.
******************************************************************************/

/** @defgroup grp_unicam_req Requirements
    @ingroup grp_unicamdrv
    @addtogroup grp_unicam_req
    @{
    @section sec_unicam_req_overview Overview
    UNICAM driver shall abstract underlying Camera data receiver(UNICAM) to
    provide Bayer streaming functionality.

    UNICAM driver shall be capable of
    -# Initialize and configure UNICAM
    -# Receiving RAW (Bayer) frame through Camera Serial Interface (CSI)
    -# Receive embedded data transmitted by Camera Sensor

    @file unicam_req.c
    @brief UNICAM requirements
    This file contains the requirements for UNICAM driver.
    @version 0.1 Initial version
*/

/**
    @brief UNICAM configurations

    UNICAM driver shall provide support for configuring dynamic configurations
    as listed below.
    -# Camera Sensor parameters
        -# Mode details:
           Width, Height, Data format, Number of embedded
           data lines before frame, Number of embedded data lines after frame
        -# CSI parameters
        -# Custom image IDs

    @rationale
    UNICAM configurations enable customers to design use cases with following
    benefits,
    -# Change resolutions, fps etc without changing firmware
    -# Plug in their own sensor modules with minimal code change
    -# Custom image IDs lets users to channelize non-image data to image buffer.
       This could mean saving of embedded data buffer memory.

    @analysis
    -# Camera Sensor parameters
        Camera sensor interface parameters are programmed to various blocks of
        UNICAM.
        -# Mode details:
           Parameters like width, height and format shall be used for allocating
           and programming image addresses and image stride.
           <BR>Embedded data lines details shall be used for programming data
           memory addresses and interrupts.
        -# CSI parameters
           CSI parameters of camera sensor serves as the input for the CSI
           blocks configurations.
    @verification
    Code review to ensure that all the configurations requirements are
    satisfied. Actual verification of the configuration shall be done for each
    specific configuration.

    @dependencies #BRCM_SWREQ_UNICAM_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_UNICAM_INIT
    @dependencies #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_TIME
    @vtrace #BCM_NS_PER_SEC

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE


    @todo Review requirement details
*/
#define BRCM_SWREQ_UNICAM_CONFIGURATION

/**
    @brief Brief description of the requirement

    Detailed Description of what is needed from this requirement

    @rationale
    Reason why this requirement is needed

    @todo update analysis
    @analysis
    Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment.

    @verification
    Develop the verification criteria for each software
    requirement that define the qualitative and quantitative measures for
    the verification of a requirement


    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_UNICAM_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_UNICAM_INIT

/**
    @brief UNICAM driver to enable RAW streaming from Sensor

    UNICAM driver shall abstract underlying Camera data receiver(UNICAM) to
    provide RAW streaming functionality.

    @rationale
    UNICAM and ISP co-work to receive RAW stream from camera sensor and process
    it to YUV.
    <BR> UNICAM driver configures UNICAM to stream lines of RAW to the memory
    which could work with in synchronization with ISP.

    @analysis
    Abstracting UNICAM imposes following responsibilities on UNICAM driver,
        -# Initialize and reset
           This initializes and resets various modules in the camera receiver.
        -# Configure
           All configurations including clock lane, data lane, image buffer
           parameters, data buffer parameters, timing parameters and control
           parameters including interrupt enables shall programmed in this
           stage.
        -# Enable
           UNICAM's various modules shall be enabled in appropriate sequence to
           start receiving CSI data/TG pattern.
        -# Disable
           UNICAM shall process disable and stop reception appropriately.
        -# De-initialize
           This shall deinitialise various modules and reset the UNICAM.
        -# Process asynchronous events from UNICAM
           UNICAM driver shall process interrupts and signal client with
           appropriate event.
    <BR>Following is the output supported from UNICAM,
        -# Up to two 10-16 bit Bayer pattern stream.
        -# Embedded data stream from sensor
        -# TG pattern
    <BR>UNICAM driver shall use 22KB of memory for code and 14KB of
    memory for data.

    @verification
    Use TG pattern from UNICAM to validate the data integrity and functionality
    of UNICAM driver.
    <BR>CSI configuration shall be verified after integrating with other
    components.

    @dependencies #BRCM_SWREQ_UNICAM_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_UNICAM_CONFIGURATION
    @dependencies #BRCM_SWREQ_UNICAM_INIT
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER

    @todo Review requirement details

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_UNICAM

/**
    @brief Brief description of the requirement

    Detailed Description of what is needed from this requirement

    @rationale
    Reason why this requirement is needed

    @todo update analysis
    @analysis
    Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment.

    @verification
    Develop the verification criteria for each software
    requirement that define the qualitative and quantitative measures for
    the verification of a requirement

    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_UNICAM_KERNEL_INTERFACE

/**
    @brief Brief description of the requirement

    Detailed Description of what is needed from this requirement

    @rationale
    Reason why this requirement is needed

    @todo update analysis
    @analysis
    Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment.

    @verification
    Develop the verification criteria for each software
    requirement that define the qualitative and quantitative measures for
    the verification of a requirement

    @dependencies #BRCM_SWREQ_UNICAM_CONFIGURATION
    @dependencies #BRCM_SWREQ_UNICAM
    @dependencies #BRCM_SWREQ_UNICAM_INIT
    @dependencies #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
    @dependencies #BRCM_SWREQ_UNICAM_RDB_REFERENCE

    @vtrace #UNICAM_TASK
    @vtrace #UNICAM_TUNER_TASK
    @vtrace #UNICAM_DISABLE_TUNERS
    @vtrace #UNICAM_EVENT
    @vtrace #UNICAM_META_EVENT
    @vtrace #UNICAM_STATS_EVENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_UNICAM_KERNEL_HANDLER

/** @} */
