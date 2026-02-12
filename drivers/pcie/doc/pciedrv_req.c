/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_pciedrv_req Requirements
    @ingroup grp_pciedrv
    @addtogroup grp_pciedrv_req
    @{

    @file pciedrv_req.c
    @brief Requirements for PCIe Driver

    @version 0.1 Initial version
*/

/**
    @brief PCIe Endpoint and root complex Driver

    The PCIe Driver shall support following feature

    + Initializing PCIe PHY
    + Configure PCIe Block with endpoint parameters
        + Device ID
        + BAR space
        + Internal memory mapping and attributes
    + Enable and disable PCIe link
    + Deinitialize PCIe driver

    Hotplug and dynamic re-enumeration shall be handled by HW and no
    software intervention is needed.

    PCIe driver memory usage shall not exceed few bytes as it required
    to store only simple driver state.

    @rationale
    PCIe driver is needed for accessing the PCIe HW block in a
    streamlined manner.

    @analysis
    + PCIe init API shall initialize PHY and the PCIe device
    + PCIe init API shall accept endpoint configuration option that contains
        + Device ID
        + Number of BAR spaces
        + BAR space size and internal mapping
    + Enable / disable the PCIe interface is done by asserting
        reset for the PCIe logic
    + Deinit API shall disable all the BARs and Pages configured

    @alternate_arch
    None

    @verification
    + PCIe endpoint is correctly enumerated in the host
    + BAR Spaces are correctly visible in the host as configured
      in the driver
    + Able to read and write to the BAR space from host
    + Disable and enable PCIe interface correctly reflected in the host

    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_TIME
    @dependencies #BRCM_SWREQ_HSIO_COMMON_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_PRIV0_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_PRIV1_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_IMAP1_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_IMAP_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_PCIE_COMMON_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_HSAFE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CFG_AER_RDB_REFERENCE
    @vtrace #volatile
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5474, RGL-5475, RGL-5476, RGL-5477, RGL-5478, RGL-5481,
                       RGL-5482, RGL-5483, RGL-5484, RGL-5487, RGL-5494, RGL-5495,
              RGL-5496, RGL-5497, RGL-5499, RGL-5500, RGL-5501, RGL-5507,
              RGL-5508, RGL-5509
*/
#define BRCM_SWREQ_PCIE_DRV

/** @} */
