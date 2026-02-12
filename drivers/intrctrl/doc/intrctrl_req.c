/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/** @defgroup grp_intrctrldrv_req Requirements
    @ingroup grp_intrctrldrv

    @addtogroup grp_intrctrldrv_req
    @{

    This is a requirement to implement Interrupt Controller driver

    @file intrctrl_req.c
    @brief Requirements for interrupt controller driver.
    @version 0.1 Initial version
*/

/**
    @brief Shared interrupt status

    Interrupt controller driver shall provide a mechanism for reading
    interrupt pending status of a shared interrupt.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_INTRCTRL_RDB_REFERENCE

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

/**
    @brief Safety monitor interrupt configuration and status

    Interrupt controller driver shall provide a mechanism to configure and read safety monitor
    interrupt .

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_INTRCTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_INTRCTRL_SM_INTR

/**
    @brief Non Maskable interrupt configuration and status

    Interrupt controller driver shall provide a mechanism to configure and read Non Maskable
    interrupt.

    @todo analysis

    @todo verification

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_INTRCTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_INTRCTRL_NMI_INTR

/**
    @brief INTRCTRL General Interrupts

    INTRCTRL driver shall provide interfaces to configure, clear and get general control interrupt status.

    @rationale
    General Interrupt related operation for INTRCTRL

    @analysis
    General Interrupt related operation for INTRCTRL

    @verification
    General Interrupt related API's needs to be verified.

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_INTRCTRL_GEN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_INTRCTRL_GENERAL_INTR

/** @} */
