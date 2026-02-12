/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_gpiodrv_req Requirements
    @ingroup grp_gpiodrv

    @addtogroup grp_gpiodrv_req
    @{

    @file gpio_req.c

    @version 0.1 Initial version
*/

/**
    @brief GPIO initialization, de-initialization and configuration

    GPIO driver shall provide a mechanism for initialization.
    GPIO driver shall be de-initialized after all the operations are completed.
    GPIO driver shall provide a mechanism to change the configuration.

    @rationale
    Initialize/change GPIO peripheral with specified configuration

    @analysis
    Initialize/change GPIO peripheral with specified configuration

    @verification
    Each GPIO pin needs to be verified based on initialization/configuration settings.

    @dependencies #BRCM_SWREQ_GPIO_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5603
*/
#define BRCM_SWREQ_GPIO_INIT_CONFIG

/**
    @brief Provides interface to initialize and configure GPIO

    GPIO driver provides API to Initialize and configure GPIO pins.
    The API's should be portable, platform independent and MISRA complaint.

    @rationale
    GPIO driver APIs initialize and configure GPIO pins.

    @analysis
    Initialization/Configuration parameters are passed using structures.

    @verification
    Each GPIO pins needs to be verified based on configuration settings.

    @dependencies #BRCM_SWREQ_GPIO_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_GPIO_INIT_CONFIG

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5603
*/
#define BRCM_SWREQ_GPIO

/** @} */
