/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_xgmacmdio_drv_req Requirements
    @ingroup grp_xgmacdrv

    @addtogroup grp_xgmacmdio_drv_req
    @{

    @file ptp_req.c
    @brief Requirements for XGMAC MDIO driver
    This file contains the requirements for the BCM8915x XGMAC MDIO driver
    @version 0.1 Initial version
*/

/**
    @brief XGMAC MDIO initialization and de-initialization

    XGMAC MDIO driver shall provide a mechanism for initialization.
    XGMAC MDIO driver shall be de-initialized after all the operations are completed.

    @rationale
    Initialize XGMAC MDIO peripheral

    @analysis
    Initialize XGMAC MDIO driver

    @verification
    MDIO driver initialization is completed after that MDIO read write needs to be verified.

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_TIME
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT
    @dependencies #BRCM_SWREQ_HSIO_COMMON_RDB_REFERENCE

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5526
*/
#define BRCM_SWREQ_XGMAC_MDIO_INIT

/**
    @brief XGMAC MDIO write operation

    XGMAC MDIO driver shall provide an interface for read/write/reset operation.

    @rationale
    XGMAC MDIO read/write/reset operation

    @analysis
    XGMAC MDIO read/write/reset operation

    @verification
    XGMAC MDIO read/write/reset operation needs to be verified

    @dependencies #BRCM_SWREQ_XGMAC_CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_XGMAC_MDIO_INIT
    @dependencies #BRCM_SWREQ_XGMAC_DRIVER_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_XGMAC_MDIO_OPERATION

/** @} */
