/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_macsecdrv_bus_ifc Interface
    @ingroup grp_macsecdrv

    @addtogroup grp_macsecdrv_bus_ifc
    @{
    @file macsec_bus.h

    @brief MACSec driver bus interface
    This file defines MACSec Driver bus interfaces

    @version 0.1 Initial revision
*/

#ifndef MACSEC_BUS_H
#define MACSEC_BUS_H

#include <stdint.h>
#include <bcm_comp.h>
#include <crypto_secy_types.h>
#include <xcvr_common.h>
#include "rdb/macsec_mspu_mac_rdb.h"

/**
    @name MACSec driver API IDs
    @{
    @brief API IDs for MACSec driver
*/
#define BRCM_SWARCH_MACSEC_BLOCK_TYPE                        (0x8201U) /**< @brief #MACSEC_BlkType                */
#define BRCM_SWARCH_MACSEC_BLK_ADDR_TYPE                     (0x8202U) /**< @brief #MACSEC_BlkAddrType            */
#define BRCM_SWARCH_MACSEC_READ_PROC                         (0x8203U) /**< @brief #MACSEC_Read                   */
#define BRCM_SWARCH_MACSEC_WRITE_PROC                        (0x8204U) /**< @brief #MACSEC_Write                  */
#define BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC            (0x8205U) /**< @brief #MACSEC_ReadModifyWrite        */
#define BRCM_SWARCH_MACSEC_BUSDRV_GLOBAL_INIT_PROC           (0x8206U) /**< @brief #MACSEC_BusDrvGlobalInit       */
#define BRCM_SWARCH_MACSEC_BUSDRV_EN_PROC                    (0x8207U) /**< @brief #MACSEC_BusDrvEnable           */
#define BRCM_SWARCH_MACSEC_BUSDRV_DIS_PROC                   (0x8208U) /**< @brief #MACSEC_BusDrvDisable          */
#define BRCM_SWARCH_MACSEC_BUSDRV_DEINIT_PROC                (0x8209U) /**< @brief #MACSEC_BusDrvDeInit           */
#define BRCM_SWARCH_MACSEC_BUSDRV_UNIMAC_INIT_PROC           (0x820AU) /**< @brief #MACSEC_DrvUniMacInit          */
#define BRCM_SWARCH_MACSEC_DUMMY_READ_PROC                   (0x820BU) /**< @brief #MACSEC_DummyRead              */
#define BRCM_SWARCH_MACSEC_BUSDRV_UNIMAC_SPEED_SET_PROC      (0x820CU) /**< @brief #MACSEC_BusDrvUnimacSpeedSet   */
/** @} */

/**
    @name MACSEC_BlkType;
    @{
    @brief MACsec Block type

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_BlkType;       /**< @brief  MACsec block type */
#define MACSEC_BLOCK_MSPU_TOP         (0UL)   /**< @brief MSPU Blocks */
#define MACSEC_BLOCK_MSPU_MAC         (1UL)   /**< @brief MAC Blocks */
#define MACSEC_BLOCK_CORE             (2UL)   /**< @brief Ingress/Egress EIP160 Blocks */
#define MACSEC_BLOCK_COUNT_MAX        (3UL)   /**< @brief MAX Block Count */
/** @} */

/**
    @brief Crypto Per Port Block Base Address Type

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_BlkAddrType;       /**< @brief  MACsec block addr type */


/**
    @brief MACSec Dummy Read

    Read the MACSec register to check the MACSEC support.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port
    @param[in]      aBlkType    #MACSEC_BlkType
    @param[in]      aOffset     Register Offset

    Return values are documented in reverse-chronological order
    @return     BCM_ERR_OK          MACSEC read was successful
    @return     BCM_ERR_NOSUPPORT   MACSEC read was Failed

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
int32_t MACSEC_DummyRead(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset);

/**
    @brief MACSec Read

    Read the MACSec register.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port
    @param[in]      aBlkType    #MACSEC_BlkType
    @param[in]      aOffset     Register Offset

    Return values are documented in reverse-chronological order
    @return     read data        MACSEC read was successful
    @return     0UL              MACSEC read was Failed

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
uint32_t MACSEC_Read(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset);

/**
    @brief MACSec Write

    Write the MACSec register.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port
    @param[in]      aBlkType    #MACSEC_BlkType
    @param[in]      aOffset     Register Offset
    @param[in]      aData       Data to write.

    Return values are documented in reverse-chronological order
    @return    void

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
void MACSEC_Write(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset, uint32_t aData);

/**
    @brief MACSec Read-Modify-Write

    Modify the MACSec register.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port
    @param[in]      aBlkType    #MACSEC_BlkType
    @param[in]      aOffset     Register Offset
    @param[in]      aData       Data to write.

    Return values are documented in reverse-chronological order
    @return    void

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
void MACSEC_ReadModifyWrite(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset, uint32_t aData);

/**
    @brief MACSec Driver UniMac Init

    Configure MACSec MAC Block.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port
    @param[in]      aConfig     #SECY_HwCfgType

    Return values are documented in reverse-chronological order
    @return     BCM_ERR_OK

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
int32_t MACSEC_DrvUniMacInit(BCM_HandleType aConnHdl, uint32_t aID, const SECY_HwCfgType *const aCfg);

/**
    @brief MACSec Bus dependent Global Init

    Configure MACSec register via bus interface.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port

    @return     void

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
void MACSEC_BusDrvGlobalInit(BCM_HandleType aConnHdl, uint32_t aPort);

/**
    @brief MACSec block Init

    Init MACSec Block.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None:%


    @param[in]      aPort       Switch Port
    @param[in]      aConfig     #SECY_HwCfgType

    Return values are documented in reverse-chronological order
    @return     BCM_ERR_OK

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
int32_t MACSEC_BusDrvEnable(BCM_HandleType aConnHdl, uint32_t aPort, const SECY_HwCfgType *const aCfg);

/**
    @brief MACSec Driver Disable Port

    Disable Port.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port

    @return     void

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
void MACSEC_BusDrvDisable(BCM_HandleType aConnHdl, uint32_t aPort);

/**
    @brief MACSec Driver UniMac Init

    Configure MACSec MAC Block.

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port

    @return     void

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/
void MACSEC_BusDrvDeInit(BCM_HandleType aConnHdl, uint32_t aPort);

/**
    @brief MACSec Driver UniMac speed set

    Configure MACSec UniMac Block speed

    @note None

    @behavior Sync, Non-Rentrant

    @pre None

    @param[in]      aPort       Switch Port
    @param[in]      aSpeed    Unimac speed to be configured

    @return     int32_t

    @post None

    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @limitations None
*/

int32_t MACSEC_BusDrvUnimacSpeedSet(uint32_t aPort, ETHXCVR_SpeedType aSpeed);

#endif /* MACSEC_BUS_H */
/** @} */

