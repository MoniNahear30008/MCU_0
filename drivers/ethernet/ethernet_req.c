/**
    @defgroup grp_ethernet_req Requirements
    @ingroup grp_ethernet
    @addtogroup grp_ethernet_req
    @{

    @file ethernet_req.c
    @brief Requirements for Ethernet driver
    This file contains the requirements for BCM8915x Ethernet driver
    @version 0.1
*/

/**
    @brief Top level Ethernet that contains
    XGMAC, XCVR and MACsec drivers.

    @rationale
    XGMAC, XCVR and MACSec driver implementation

    @analysis
    XGMAC, XCVR and MACSec driver implementation. Rigel supports two types
    of transceiver internal PHY and external PHY support, either one of them is active at a time.

    @verification
    XGMAC, XCVR and MACSec driver needs to be verified

    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5519, RGL-5524, RGL-5525
*/
#define BRCM_SWREQ_ETHERNET
