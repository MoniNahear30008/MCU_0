/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @addtogroup grp_macsecdrv_plat_ifc
    @{

    @file macsec_platform.c
    @brief MacSec platform API
    This source file contains the implementation of functions for platform specific APIs

    @version 1.0 Initial version
*/

#include <bcm_utils.h>
#include <crypto_secy_types.h>
#include "macsec_plat.h"
#include "macsec_platform.h"

/**
    @name Component Design IDs
    @{
    @brief Design IDs for Component
*/
#define BRCM_SWDSGN_MACSEC_GET_BUS_IFC_PROC           (0xB900U) /**< @brief #MACSEC_GetBusIfc              */
#define BRCM_SWDSGN_MACSEC_GET_HWID_PROC              (0xB901U) /**< @brief #MACSEC_GetHWId                */
#define BRCM_SWDSGN_MACSEC_DRV_PORT2_CTX_IDX_PROC     (0xB902U) /**< @brief #MACSEC_ConvPort2CtxIdx        */
/** @} */

/**
    SecY Get Bus Mode based on port number

    @trace #BRCM_SWARCH_MACSEC_GET_BUS_IFC_PROC
    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

    @code{.c}
    if (port < BCM_ARRAY_LEN(MACSEC_BusLookUpTbl))
        bus = MACSEC_BusLookUpTbl[port].busType;
    @endcode
*/
SECY_BusType MACSEC_GetBusIfc(uint32_t aPort)
{
    SECY_BusType busIfc = 0xFF;

    if (aPort < BCM_ARRAY_LEN(MACSEC_BusLookUpTbl)) {
        busIfc = MACSEC_BusLookUpTbl[aPort].busType;
    }

    return busIfc;
}

/**
    @brief Helper API to Map Port number to MACSEC HwID

    @code{.unparsed}
    ret = BCM_ERR_INVAL_PARAMS

    if port < BCM_ARRAY_LEN(RACSEC_Port2HwTbl)
        hwID = MACSEC_BusLookUpTbl[aPort].ctxId;

    return hwID
    @endcode

    @trace #BRCM_SWARCH_MACSEC_GET_HWID_PROC
    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
MACSEC_HwIDType MACSEC_GetHWId(uint32_t aPort)
{
    MACSEC_HwIDType hwID = 0xFF;

    if (aPort < BCM_ARRAY_LEN(MACSEC_BusLookUpTbl)) {
        hwID = MACSEC_BusLookUpTbl[aPort].ctxId;
    }

    return hwID;
}

/**
    @code{.unparsed}
        return MACSEC_BusLookUpTbl[aPortNum].ctxId
    @endcode

    @trace #BRCM_SWARCH_MACSEC_CONV_PORT2_CTX_IDX_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
uint32_t MACSEC_ConvPort2CtxIdx(uint32_t aPortNum)
{
    return (uint32_t)MACSEC_BusLookUpTbl[aPortNum].ctxId;
}

/** @} */
