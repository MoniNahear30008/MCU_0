/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_clkrstdrv_req Requirements
    @ingroup grp_clkrstdrv

    @addtogroup grp_clkrstdrv_req
    @{

    Requirements to implement clock and reset driver for various
    subsystems.

    @file clock_reset_req.c
    @brief Requirements for clock and reset driver
    @version 0.1 Initial version
*/

/**
    @brief System Clock configuration

    clock and reset driver shall provide a mechanism to configure dividers for
    various clocks available in the system.

    @rationale
    Configures clock divider of system clock.

    @analysis
    Configures clock divider of system clock.

    @verification
    Clock divider configuration needs to be verified

    @dependencies #BRCM_SWREQ_CLKRST_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_SYSPLL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CPU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CLKRST_CONFIG
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CLKRST_SYSCLK_CONFIG

/**
    @brief M7 Reset control

    Driver shall provide interfaces to control M7, A55 and Q8 IP reset.

    @rationale
    M7, A55 and Q8 reset control.

    @analysis
    M7, A55 and Q8 reset control.

    @verification
    M7, A55 and Q8 reset needs to be verified

    @dependencies #BRCM_SWREQ_CLKRST_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_CLKRST_CONFIG

/** @} */
