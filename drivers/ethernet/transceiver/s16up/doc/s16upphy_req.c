/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_xcvr_s16up_req S16UP (MMAP based access) Requirements
    @ingroup grp_xcvr_s16up_drv_impl

    @addtogroup grp_xcvr_s16up_req
    @{
    @section sec_xcvr_s16up_req_overview S16UP (MMAP based access) Requirements Overview

    This is a requirement to implement the S16UP (MMAP based access) driver

    @file s16up_phy_req.c
    @brief Requirements for S16UP (MMAP based access) driver
    @version 0.1 Initial version
*/

/**
    @brief Transceiver function table for S16UP (MMAP based access)

    Provide the function table implementation for integrating the S16UP
    (MMAP based access) in the transceiver driver. This would be expected to
    be used as a PHY function table in the integrated bus mode

    @rationale
    The function table abstracts the differences in across IPs implementing the
    transceiver functionality

    @analysis
    -# This driver shall comply with the ETHXCVR function table interface
    -# The driver shall support 2.5G BASE-X and 2.5G BASE-R interface to conntect external PHY
	-# This PHY will work in forced speed mode. There is no autonegotiation support.
    -# The driver shall allow setting master-slave only in forced mode.
    -# All 'SET' APIs which is not supported shall
       return BCM_ERR_NOSUPPORT
    -# There is no support for ISOLATE mode
   
    -# TC10 support is not present. For now, all TC10 related APIs
       shall return BCM_ERR_NOSUPPORT
 
    @verification
    - Verify that the PHY can be initialized and linked up (if the peer is
      configured and connected) in both forced
    - Verify that the PHY can be put to lower power mode and that this causes
      a link down
    - Verify that master/slave can be configured in forced mode
    - Verify that loopback can be set and this causes the traffic from the MII
      side to return back
    - Verify that the link state, speed, duplex, flow control master-slave
      etc.  are reported accurately in forced modes (as and where applicable)
    - Verify that bidirectional traffic is through when the PHY reports a
      link up. Throughput should be close to the reported speed
    - Verify that Traffic should not be through when the PHY is configured in
      low power mode or when link is down

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_ETHXCVR
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
    @dependencies #BRCM_SWREQ_ETHXCVR_FUNCTBL
*/
#define BRCM_SWREQ_S16UP_FUNCTBL


/** @} */
