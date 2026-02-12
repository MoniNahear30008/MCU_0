/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @addtogroup grp_macsecdrv_ifc
    @{
    @file macsec_plat.h

    @brief MACSec internal platfrom interface
    This file defines MACSec internal platfrom interface

    @version 0.1 Initial revision
*/

#ifndef MACSEC_PLAT_INT_H
#define MACSEC_PLAT_INT_H

#include <stdint.h>
#include <bcm_comp.h>
#include <crypto_secy_types.h>
#include <crypto_secy.h>
/**
    @name MACSec platfrom API IDs
    @{
    @brief API IDs for MACSec platfrom layer
*/
#define BRCM_SWARCH_MACSEC_BUS_LOOKUP_TYPE            (0x8300U) /**< @brief #MACSEC_BusLookupType */
#define BRCM_SWARCH_MACSEC_BUS_LOOKUP_TBL_GLOBAL      (0x8301U) /**< @brief #MACSEC_BusLookUpTbl  */
/** @} */

/**
    @brief MACSEC Bus LookUp Table Type

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
typedef struct sMACSEC_BusLookupType {
    uint32_t ctxId;         /**< @brief Context ID of port */
    SECY_BusType busType;   /**< @brief Bus type           */
} MACSEC_BusLookupType;

/**
    @brief MACSEC Port to Bus LookUp Table

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

*/
extern const MACSEC_BusLookupType MACSEC_BusLookUpTbl[MACSEC_MAX_PORT_NUM];
#endif /* MACSEC_PLAT_INT_H */

/** @} */
