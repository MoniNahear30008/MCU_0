/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_macsecdrv_plat_ifc Platform Interface
    @ingroup grp_macsecdrv

    @addtogroup grp_macsecdrv_plat_ifc
    @{
    @file macsec_platform.h

    @brief MACSec driver platform interface
    This file defines MACSec Driver platform interfaces

    @version 0.1 Initial revision
*/

#ifndef MACSEC_PLAT_H
#define MACSEC_PLAT_H

#include <stdint.h>
#include <bcm_comp.h>
#include <crypto_secy_types.h>
#include <crypto_secy.h>

/**
    @name MACSec driver API IDs
    @{
    @brief API IDs for MACSec Platform
*/
#define BRCM_SWARCH_MACSEC_GET_BUS_IFC_PROC           (0x8100U) /**< @brief #MACSEC_GetBusIfc              */
#define BRCM_SWARCH_MACSEC_GET_HWID_PROC              (0x8101U) /**< @brief #MACSEC_GetHWId                */
#define BRCM_SWARCH_MACSEC_ENABLE_SYS_INTERRUPT       (0x8102U) /**< @brief #MACSEC_EnableSysInterrupt     */
#define BRCM_SWARCH_MACSEC_DISABLE_SYS_INTERRUPT      (0x8103U) /**< @brief #MACSEC_DisableSysInterrupt    */
#define BRCM_SWARCH_MACSEC_CLEAR_SYS_INTERRUPT        (0x8104U) /**< @brief #MACSEC_ClearSysInterrupt      */
#define BRCM_SWARCH_MACSEC_CONV_PORT2_CTX_IDX_PROC    (0x8105U) /**< @brief #MACSEC_ConvPort2CtxIdx        */
/** @} */

/**
    @brief MACSEC Get MACSec hardware id

    @note None

    @behavior Sync, Rentrant

    @pre None

    @param[in]      aPort       Switch Port number

    Return values are documented in reverse-chronological order
    @return     #MACSEC_HwIDType              MACSEC Hardware Id

    @post None

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

    @limitations None
*/
MACSEC_HwIDType MACSEC_GetHWId(uint32_t aPort);

/**
    @brief MACSEC Get Bus Interface

    @note None

    @behavior Sync, Rentrant

    @pre None

    @param[in]      aPort       Switch Port number

    Return values are documented in reverse-chronological order
    @return     #SECY_BusType              MACSEC Bus interface

    @post None

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

    @limitations None
*/
SECY_BusType MACSEC_GetBusIfc(uint32_t aPort);

/**
    @brief MACSEC Enable system (CPU) interrupt

    This API shall enable system side interrupt
    (MACSec interrupt connected to CPU subsystem).
    This API is called by the macsec driver during
    the initialization to enable the system side
    interrupt for the MACSec.

    @note None

    @behavior Sync, Rentrant

    @pre None

    @param[in]      aPort       Switch Port number

    Return values are documented in reverse-chronological order
    @return     void

    @post None

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

    @limitations None
*/
void MACSEC_EnableSysInterrupt(uint32_t aPort);

/**
    @brief MACSEC disable system interrupt

    This API shall disable the MACSec system side
    interrupt.

    @note None

    @behavior Sync, Rentrant

    @pre None

    @param[in]      aPort       Switch Port number

    Return values are documented in reverse-chronological order
    @return     #SECY_BusType              MACSEC Bus interface

    @post None

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

    @limitations None
*/
void MACSEC_DisableSysInterrupt(uint32_t aPort);

/**
    @brief MACSEC clear system interrupt

    This API shall clear the MACSec system side interrupt.

    @note None

    @behavior Sync, Rentrant

    @pre None

    @param[in]      aPort       Switch Port number

    Return values are documented in reverse-chronological order
    @return     #SECY_BusType              MACSEC Bus interface

    @post None

    @trace #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

    @limitations None
*/
void MACSEC_ClearSysInterrupt(uint32_t aPort);

/**
    @brief Get MACSEC context index from port number

    There are only needed number of context memory allocated.
    This function maps the port number to context memory index.

    @param[in]     aPortNum     Port Number

    @return     MACSEC Context Index corresponds to the port number

    @note

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
uint32_t MACSEC_ConvPort2CtxIdx(uint32_t aPortNum);

#endif /* MACSEC_PLAT_H */
/** @} */
