/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_xcvr_brphy1gdrv_v2_req BRPHY1G V2 (MMAP based access) Requirements
    @ingroup grp_xcvr_brphydrv_impl

    @addtogroup grp_xcvr_brphy1gdrv_v2_req
    @{
    @section sec_xcvr_brphy1g_v2_req_overview BRPHY1G V2 (MMAP based access) Requirements Overview

    This is a requirement to implement the BRPHY1G V2 (MMAP based access) driver

    @file brphy1g_v2_req.c
    @brief Requirements for BRPHY1G V2 (MMAP based access) driver
    @version 0.1 Initial version
*/

/**
    @brief Transceiver function table for BRPHY1G V2 (MMAP based access)

    Provide the function table implementation for integrating the BRPHY1G V2
    (MMAP based access) in the transceiver driver. This would be expected to
    be used as a PHY function table in the integrated bus mode

    @rationale
    The function table abstracts the differences in across IPs implementing the
    transceiver functionality

    @analysis
    -# BRPHY1G is different from the existing BRPHY (100M) and Denali (1G). A
       new driver would be required and it would be named BRPHY1GV2
    -# This driver shall comply with the ETHXCVR function table interface
    -# The driver shall support Auto-negotiation as well as Forced (1G/100M)
       modes. In autoneg mode, both 100M and 1G speeds shall be advertised
    -# The driver shall allow setting master-slave only in forced mode. It shall
       retrieve the negotiated master/slave in autoneg mode as well
    -# All 'SET' APIs other than SetMode, SetLoopback and SetMasterSlave shall
       return BCM_ERR_NOSUPPORT
    -# There is no support for ISOLATE mode
    -# The 'GET' APIs other than GetSpeed, GetLinkState, GetSQIValue shall
       return cached values
    -# TC10 support may be added in the future. For now, all TC10 related APIs
       shall return BCM_ERR_NOSUPPORT
    -# Since this IP has not been integrated to endpoints, support for link
       status interrupt may be added in the future

    @verification
    - Verify that the PHY can be initialized and linked up (if the peer is
      configured and connected) in both forced and autoneg modes
    - Verify that the PHY can be put to lower power mode and that this causes
      a link down
    - Verify that master/slave can be configured in forced mode
    - Verify that loopback can be set and this causes the traffic from the MII
      side to return back
    - Verify that the link state, speed, duplex, flow control master-slave and
      SQI, autoneg status etc.  are reported accurately in both autoneg and
      forced modes (as and where applicable)
    - Verify that bidirectional traffic is through when the PHY reports a
      link up. Throughput should be close to the reported speed
    - Verify that Traffic should not be through when the PHY is configured in
      low power mode or when link is down

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_BCM_TIME
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_ETHXCVR
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
    @dependencies #BRCM_SWREQ_ETHXCVR_FUNCTBL
    @dependencies #BRCM_SWREQ_BRPHY1GV2_AUTONEGIEEE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_PMAPMDIEEE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_PHYCONTROL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_T1CORE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_EC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_CORE_MISC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_PCS_IEEE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_LINK_SYNC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_PCS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BRPHY1GV2_AFE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
    @dependencies #BRCM_SWREQ_BRPHY1GV2_TEST_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MCU

    @vtrace #ENABLE_ETH_BRPHY1G_V2
    @vtrace #BRPHY1GV2_ACD_SUPPORT
*/
#define BRCM_SWREQ_BRPHY1GV2_FUNCTBL


/** @} */
