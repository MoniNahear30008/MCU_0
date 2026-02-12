/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xgmacdrv_req Requirements
    @ingroup grp_xgmacdrv
    @addtogroup grp_xgmacdrv_req
    @{

    @file xgmac_req.c
    @brief Requirements for XGMAC driver
    This file contains the requirements for BCM8915x XGMAC driver
    @version 0.1 Initial version
*/

/**
    @brief XGMAC Driver Initialization

    XGMAC driver shall provide mechanism to initialize core
    to work in a defined state.

    @rationale
    Initialize XGMAC peripheral with initial configuration.

    @analysis
    Initialize XGMAC driver configure interrupts.

    @verification
    XGMAC driver initialization needs to be verified, XGMAC transfer is
    possible after initialization is completed.

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH0_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH1_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH2_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH3_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH4_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH5_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH6_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH7_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5521, RGL-5553
*/
#define BRCM_SWREQ_XGMAC_DRIVER_INIT

/**
    @brief XGMAC driver configuration

    XGMAC driver shall provide APIs to configure settings
    that can be changed dynamically.

    @rationale
    Configure XGMAC peripheral

    @analysis
    Configure XGMAC core, MTL and DMA configuration.

    @verification
    XGMAC driver configuration needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ0_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ1_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ2_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ3_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ4_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ5_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ6_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ7_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5537, RGL-5538
*/
#define BRCM_SWREQ_XGMAC_DRIVER_CONFIG

/**
    @brief XGMAC driver VLAN configuration

    XGMAC driver shall provide API to set VLAN configuration.

    @rationale
    Configure XGMAC VLAN

    @analysis
    Configure XGMAC VLAN

    @verification
    XGMAC driver VLAN configuration needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_XGMAC_DRIVER_VLAN_CONFIG

/**
    @brief XGMAC Get device information

    XGMAC driver shall provide API to get driver and device information.

    @rationale
    Get XGMAC device information

    @analysis
    Get XGMAC device information

    @verification
    XGMAC device configuration needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_XGMAC_DRIVER_INFO

/**
    @brief XGMAC driver DMA initialization.

    XGMAC driver shall provide DMA initialization API to configure DMA.

    @rationale
    XGMAC driver DMA initialization

    @analysis
    XGMAC driver DMA initialization

    @verification
    XGMAC driver DMA initialization needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @dependencies #BRCM_SWREQ_XGMAC_DMA_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_MTL_TCQ0_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH0_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH1_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH2_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH3_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH4_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH5_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH6_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DMA_CH7_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_CONFIG
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5533, RGL-5534
*/
#define BRCM_SWREQ_XGMAC_DRIVER_DMA_INIT

/**
    @brief XGMAC MIB counter status.

    XGMAC MIB counter statistics.

    @rationale
    XGMAC MIB counter status

    @analysis
    XGMAC MIB counter status

    @verification
    XGMAC MIB counter statistics needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_XGMAC_MIB_COUNTER_STATUS

/** @} */

