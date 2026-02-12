/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvrdrv_req Requirements
    @ingroup grp_transceiverdrv
    @addtogroup grp_xcvrdrv_req
    @{

    @file xcvr_req.c
    @brief Requirements for XCVR driver
    This file contains the requirements for BCM8915x XCVR driver
    @version 0.1 Initial version
*/

/**
    @brief Execution in kernel context

    Ethernet transceiver driver shall define commands and data structures for
    the commands to be passed and executed in kernel context

    @rationale
    -# Driver shall require privileged mode since it is accessing the hardware
    -# Driver memory/buffers and data structures should be protected from user
       space
    -# This also helps serialize requests from multiple clients

    @todo analysis

    @verification
    The requirement will be verified by source code inspection


    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ETHXCVR
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE

/**
    @brief Execution in kernel context

    Ethernet transceiver driver shall define commands and data structures for
    the commands to be passed and executed in kernel context

    @rationale
    -# Driver shall require privileged mode since it is accessing the hardware
    -# Driver memory/buffers and data structures should be protected from user
       space
    -# This also helps serialize requests from multiple clients

    @analysis
    -# Driver shall provide privileged mode  to control Wakeup mode feature
       on the capable PHY. It should also support to Read the wakeup Reason
       when wakeup mode is enabled.

    @verification
    The requirement will be verified by source code inspection

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER

/**
    @brief Transceiver initialization and configuration

    Ethernet transceiver driver shall provide a hardware independent
    interface to initialize and configure the transceivers in a system
    The configuration for each port shall consume around 100 bytes of memory

    @rationale
    Broadcom products support various interfaces and modes of operation. The
    transceiver driver is responsible for configuring and establishing the physical
    medium for communication. It should provide a uniform interface to the upper
    layers irrespective of the capabilities and implementation at the hardware.
    Although the expectation is that an automotive network is statically configured
    and devices boot up with the appropriate modes, SDK should support configuration
    of any of the (supported) modes chosen by the operator.

    @analysis
    -# Transceivers are identified using a 0 based indexing scheme
    -# Transceivers might not be monolithic devices but could consist of different
       operational blocks connected via buses. The architecture should be flexible enough
       to cater to the following cases:
       -# Integrated PHY
       -# External PHY connected over MII-like bus
       -# MII-like bus signals directly connected to external world
    -# Configuration will include parameters to configure the 'PHY device' which converts
       the electrical signals between the MII-like interface and the ones used on the actual
       physical interface. The PHY's control interface could be mapped directly on CPU's memory
       or could be accessed via the MDIO bus.
    -# Configuration will contain parameters to configure the 'bus device' which implements
       the MII-like interface which connects a MAC and a PHY as per the IEEE 802.3 model.
       The bus device if present is assumed to be mapped on CPU's memory.
    -# There shall be support for configuration of the following parameters:
       -# Speed
       -# Duplex
       -# Flow Control
       -# Auto-negotiation
       -# Jumbo frame support
       -# Administrative mode
       -# Loopback mode
       -# Master/Slave
    -# Initialization is expected to be the first step to be performed. Any
       other APIs shall fail before invoking this.
    -# The transceiver driver shall cache the configuration received during initialization
       (after validation with the platform/board specific layer) and update its state. It
       will wait for a subsequent API (to power up the transceiver) to programme the hardware
    -# The driver shall support at most 16 ports. This number takes into account
       the number of ports on current multi-port switches and light-stacked devices.

    @verification
    The requirement will be verified by source code inspection

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ETHXCVR
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_INIT

/**
    @brief Transceiver driver status

    The transceiver driver shall provide interfaces to query hardware parameters.
    It shall also provide an indication on link status change


    @rationale
    Clients of an ethernet interface would need to know parameters like link state
    to trigger their state machines. In case autonegotiation is enabled, user would
    like to know the final speed at which the link is operating. Other use cases are
    diagnostics and monitoring.

    @analysis
    -# Transceiver driver shall provide hardware independent interfaces encompassing
       most of the parameters of interest for querying
    -# In case the system supports interrupts, the device
    -# Device specific drivers can chose to implement a subset of the functionality
       according to the hardware capabilities
    -# Query APIs shall by synchronous and the driver will provide cached values. The
       parameters shall be polled at 50-100ms interval in case interrupts are not
       enabled. The following parameters will be supported:
       -# Speed
       -# Duplex mode
       -# Flow control
       -# Jumbo frame support
       -# Auto-negotiation status
       -# Master/Slave
       -# Loopback mode
       -# Signal quality indicator
       -# Link status
       -# Admin mode
       -# WakeUp Mode
       -# WakeUp Reason
    -# Transceiver shall support API to read the identifier (PHY ID).
    -# Transceiver shall support APIs to read registers of the PHY (for debug purposes).
    -# Driver shall provide an interface for periodically updating the cached
       parameters from the hardware. The same API can be hooked up to the interrupt
       handler in case interrupts are enabled
    -# Transceiver driver shall interact with the MCU driver to access
       any integration hardware blocks
    -# Transceiver driver shall interact with the MDIO driver for accessing
       PHYs connected over the MDIO bus. It shall also notify the individual
       device drivers on an MDIO event.
    -# Transceiver driver shall interact with the Timer driver for starting
       and stopping the high resolution timer for MDIO operations. The timer
       indication is used by the MDIO driver for triggering its state machine.
       The transceiver driver shall query the MDIO driver after each driver
       invocation to find out if it needs the timeout or not
    -# Transceiver driver should able to Read the WakeUp Mode Feature Status
       on the Capable PHY. It should also able to Read the wakeUp reason when the
       wakeUp mode Enabled on the capable PHY

    @verification
    The requirement will be verified by source code inspection

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ETHXCVR_INIT
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
	@dependencies #BRCM_SWREQ_ETHXCVR

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_STATUS

/**
    @brief Transceiver driver dynamic configuration

    The transceiver driver shall provide interfaces to configure standardized
    hardware parameters at run time

    @rationale
    Operator needs transceiver interface to reconfigure the device at run time

    @analysis
    -# Transceiver driver shall provide hardware independent interfaces encompassing
       most of the parameters of interest for configuration
    -# Device specific drivers can chose to implement a subset of the functionality
       according to the hardware capabilities
    -# Access to the device hardware might be asynchronous when it is over MDIO . The
       driver shall abstract this and provide synchronous APIs for configuration.
       The following parameters shall be supported:
       -# Speed
       -# Duplex mode
       -# Flow control
       -# Jumbo frame support
       -# Auto-negotiation
       -# Master/Slave
       -# Loopback mode
       -# Reset
    -# Transceiver shall support APIs to write registers of the PHY (for debug purposes).
    -# Driver shall update the received parameters in the cached configuration
    -# Transceiver driver shall provide an interface to configure the transceiver mode.
       When it receives the mode as #ETHXCVR_MODE_ACTIVE, it configures the hardware
       with all the parameters in the received configuration. Once it is done, it
       shall indicate completion via a callback
    -# There shall be a platform helper to identify the exact device driver for the
       interface. It shall return the appropriate function tables for bus and PHY
       depending on the configuration for that interface
    -# Transceiver driver shall interact with the MCU driver to configure
       any integration hardware blocks which are required for setting up
       the communication path
    -# Transceiver driver shall interact with the MDIO driver for accessing
       PHYs connected over the MDIO bus. It shall also notify the individual
       device drivers on an MDIO event.
    -# Transceiver driver shall interact with the Timer driver for starting
       and stopping the high resolution timer for MDIO operations.
    -# Transceiver driver shall Enable WakeUp Mode on the Capable PHY's.
       It should able to Request Sleep/Wakeup when the PHY had capability.
       when wakeup mode is enabled.

    @verification
    The requirement will be verified by source code inspection

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ETHXCVR_INIT
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
	@dependencies #BRCM_SWREQ_ETHXCVR

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG

/**
    @brief Pluggability of different devices
    The transceiver driver shall provide an interface to cater to plug in various
    devices whether they are integrated or external. The device driver shall be
    modeled as per the bus and PHY modular architecture.

    @rationale
    Devices from within Broadcom and across vendors might differ in the way the control
    and data interfaces are connected and configured. The specific device driver
    shall abstract this from the upper layers.

    @analysis
    -# A generic function table which encompasses all required interfaces
       from both bus and PHY devices has been proposed. An entry in the table corresponds
       to a function pointer which implements an expected functionality.
    -# Each individual device driver should chose the optimal way to implement the requested
       functionality. If it is not possible the driver should indicate this with an appropriate
       error value.
    -# Wakeup Mode Enable or Disable Support to be supported on the capable PHY
       It should able to read the wakeup reason when wakeup mode is enabled
       when wakeup mode is enabled.

    @verification
    The requirement will be verified by source code inspection

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
	@dependencies #BRCM_SWREQ_ETHXCVR

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_FUNCTBL

/**
    @brief Common interface across XCVR

    @rationale
    @todo

    @analysis
    @todo

    @verification
        -# Review of the interface

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR_COMMON

/**
    @brief Ethernet transceiver abstraction

    Purpose of this requirement is to define a common
    interface to user for ethernet transceiver.

    @rationale
    Host system should have a common interface to
    configure/debug the transceivers on the target.
    Interface shall be generic enough to cater the cases
    where PHY is connected on a switch port or PHY is
    connected independently on the board.

    @analysis
        -# Host/User shall be able to initialize, de-initialize a transceiver.
        -# Host/User shall be able to power up/power down the PHY.
        -# Host/User shall be able to read the PHY identifier.
        -# Host/User shall be able to read/write the PHY registers.
        -# Host/User shall be able to run the cable diagnostics and obtain the results.
        -# Host/User shall be able to read the general configurations like speed, mode, autoneg mode
           etc of the PHY.

    @verification
        -# Review of the interface

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
	@dependencies #BRCM_SWREQ_ETHXCVR_COMMON

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHXCVR

/**
    @brief Provide abstraction for ethernet

    Provide abstractions for data structures and types
    present within the ethernet subsystem

    @rationale
    @todo

    @analysis
    @todo

    @verification
    @todo

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_TIME
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_HSIO_COMMON_RDB_REFERENCE

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_ETHER

/**
    @brief Provide abstraction for ethernet

    Provide abstractions for data structures and types
    present within the ethernet subsystem

    @rationale
    @todo

    @analysis
    @todo

    @verification
    @todo

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
	@dependencies #BRCM_SWREQ_ETHXCVR_COMMON

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_S16UP

/**
    @brief Provide abstraction for ethernet

    Provide abstractions for data structures and types
    present within the ethernet subsystem

    @rationale
    @todo

    @analysis
    @todo

    @verification
    @todo

    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
	@dependencies #BRCM_SWREQ_ETHXCVR_COMMON

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_BRPHY1GV2

/** @} */

