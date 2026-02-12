/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_i2cdrv_req Requirements
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_req
    @{
    @section sec_i2c_req_overview Overview
    Broadcom I2C (BSC) is multi-master multi-slave serial bus. I2C bus is a two
    wire protocol, one wire for data (SDA) and another for the clock (SCL).
    Communication on the bus always starts by master and followed by slave
    response.

    @file i2c_req.c
    @brief Requirements for the I2C driver.

    @version 0.1 Initial Version
*/


/**
    @brief I2C Interface
       I2C interface configurations are defined.

    @rationale
      I2C interface data structures are defined and used in I2C initialization function.

    @analysis
      I2C configuration is done in initialization function.

    @verification
      I2C initialization should be completed before I2C transfer.

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_IIC_KERNEL_INTERFACE


/**
    @brief I2C Initialization


    I2C configuration is done in initialization function.

    @rationale
    Any peripheral driver shall have options to configure and initialize before
    it can be used for its intended purpose.

    @analysis
    Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment [Memory Budget, CPU Budget, Time/Latency Budget,
    and any other Software/Hardware Resource Budget].

    @verification
    Develop the verification criteria for each software
    requirement that define the qualitative and quantitative measures for
    the verification of a requirement
    Describe any constraints for verification and alternate verification
    approaches.

    @dependencies #BRCM_SWREQ_IIC_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_IIC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_TIME
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_LWQ
    @vtrace #BCM8915X_I2C_POLLING
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5588
*/
#define BRCM_SWREQ_IIC_INIT

/**
    @brief I2C Communication


    I2C transfer for transmit and receive.

    @rationale
    Any peripheral driver shall have options to configure and initialize before
    it can be used for its intended purpose.

    @analysis
    Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment [Memory Budget, CPU Budget, Time/Latency Budget,
    and any other Software/Hardware Resource Budget].

    @verification
    Develop the verification criteria for each software
    requirement that define the qualitative and quantitative measures for
    the verification of a requirement
    Describe any constraints for verification and alternate verification
    approaches.

    @dependencies #BRCM_SWREQ_IIC_INIT
    @dependencies #BRCM_SWREQ_IIC_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_IIC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_TIME
    @dependencies #BRCM_SWREQ_LWQ
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5589
*/
#define BRCM_SWREQ_IIC_TRANSFER
/**
    @brief I2C Kernel Handler

    Interrupt handling for i2c transfer.

    @rationale
    Use interrupt mode for low latency.

    @analysis
    Analyze the specified software requirements including their
    interdependencies to ensure correctness, technical feasibility and
    verifiability, and to support risk identification. Analyze the impact
    on cost, schedule and the technical impact. Analyze the impact that the
    software requirements will have on interfaces of system elements and
    the operating environment [Memory Budget, CPU Budget, Time/Latency Budget,
    and any other Software/Hardware Resource Budget].

    @verification
    Develop the verification criteria for each software
    requirement that define the qualitative and quantitative measures for
    the verification of a requirement
    Describe any constraints for verification and alternate verification
    approaches.

    @dependencies #BRCM_SWREQ_IIC_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_IIC_INIT
    @dependencies #BRCM_SWREQ_IIC_TRANSFER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_IIC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_LWQ
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_IIC_KERNEL_HANDLER

/** @} */
