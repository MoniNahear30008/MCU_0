/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @addtogroup grp_macsecdrv_il
    @{

    @file macsec_plat_osil.c
    @brief MACSEC Platform layer
    This source file contains the implementation of functions for MACSEC Platform

    @version 1.0 Initial version
*/

#include <bcm_utils.h>
#include <crypto_secy.h>
#include "macsec_plat.h"
#include "macsec_platform.h"

/**
    @name Component Design IDs
    @{
    @brief Design IDs for Component
*/
#define BRCM_SWDSGN_MACSEC_BUSLOOKUP_TBL_GLOBAL                (0xC200U)   /**< @brief #MACSEC_BusLookUpTbl */
#define BRCM_SWDSGN_MACSEC_ENABLE_SYS_INTERRUPT_PROC           (0xC202U)   /**< @brief #MACSEC_EnableSysInterrupt */
#define BRCM_SWDSGN_MACSEC_DISABLE_SYS_INTERRUPT_PROC          (0xC203U)   /**< @brief #MACSEC_DisableSysInterrupt */
#define BRCM_SWDSGN_MACSEC_CLEAR_SYS_INTERRUPT_PROC            (0xC204U)   /**< @brief #MACSEC_ClearSysInterrupt */
/** @} */

/**
    @brief Port to Bus LookUp Table

    @trace #BRCM_SWARCH_MACSEC_BUS_LOOKUP_TBL_GLOBAL
    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
const MACSEC_BusLookupType MACSEC_BusLookUpTbl[MACSEC_MAX_PORT_NUM] = {
    {.ctxId = 0UL, .busType = SECY_BUS_MODE_AXI},       /**@brief Switch Port-0  */
};

/**
    @brief Enable system/cpu interrupt for macsec

    @trace #BRCM_SWARCH_MACSEC_ENABLE_SYS_INTERRUPT
    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
void MACSEC_EnableSysInterrupt(uint32_t aPort)
{
}

/**
    @brief Disable system/cpu interrupt for macsec

    @trace #BRCM_SWARCH_MACSEC_DISABLE_SYS_INTERRUPT
    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
void MACSEC_DisableSysInterrupt(uint32_t aPort)
{
}

/**
    @brief clear system/cpu interrupt for macsec

    @trace #BRCM_SWARCH_MACSEC_CLEAR_SYS_INTERRUPT
    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
*/
void MACSEC_ClearSysInterrupt(uint32_t aPort)
{
}
/** @} */
