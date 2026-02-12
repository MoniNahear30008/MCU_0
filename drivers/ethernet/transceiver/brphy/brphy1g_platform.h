/*****************************************************************************
 Copyright 2019-2024 Broadcom Limited.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_xcvr_brphy1g_plat_impl BRPHY1G Transceiver Driver Platform Interface
    @ingroup grp_xcvr_brphy1gdrv

    @addtogroup grp_xcvr_brphy1g_plat_impl
    @{

    @file brphy1g_platform.h
*/

#ifndef BRPHY1G_PLAT_H
#define BRPHY1G_PLAT_H
#include "./../rdb/brphy1gv2_link_sync_rdb.h"
#include "./../inc/xcvr_functbl.h"

/**
    @name Ethernet Transceiver API IDs
    @{
    @brief API IDs for Ethernet Transceiver
*/
#define BRCM_SWARCH_BRPHY1GV2_PLATFORM_AFE_CONFIG_PROC  (0x8207U)    /**< @brief #BRPHY1GV2_PlatformAfeConfig  */
#define BRCM_SWARCH_BRPHY1GV2_PLATFORM_TC10_CONFIG      (0x820AU)    /**< @brief #BRPHY1GV2_PlatformTc10Config */
#define BRCM_SWARCH_BRPHY1GV2_LINKSYNC_REGS_GLOBAL      (0x820BU)    /**< @brief #BRPHY1GV2_LINKSYNC_REGS      */
#define BRCM_SWARCH_BRPHY1GV2_PLATFORMRUNCABLEDIAG_PROC (0x820CU)    /**< @brief #BRPHY1GV2_PlatformRunCableDiag  */
/** @} */

/**
    @brief This API configures the AFE block of BRPHY_1G ports

    Programming meant for AFE block of BRPHY_1G is platform specific
    and done based on chipset.

    @behavior Sync, Non-Reeentrant

    @param[in]  aPhyIdx      Phy Index
    @param[in]  aConfig      Pointer to port configuration
    @param[in]  aMode        Ethernet transceiver mode

    @return void

    @post None

    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
void BRPHY1GV2_PlatformAfeConfig(uint8_t aPhyIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_ModeType aMode);

/**
    @brief This API configures the BRPHY_1G ports for TC10

    BRPHY_1G phy need to retain values upon SLEEP and specific configuration
    for that is done through this API

    @behavior Sync, Non-Reeentrant

    @param  None

    @return void

    @post None

    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
void BRPHY1GV2_PlatformTc10Config(void);

/**
    @brief BRPHY1G LINK SYNC base address

    This global variable shall contain Brphy1G link sync base addresses
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL

*/
extern BRPHY1GV2_LINK_SYNC_RDBType * const BRPHY1GV2_LINKSYNC_REGS[BRPHY1GV2_LINK_SYNC_MAX_HW_ID];

/** @brief This API executes cable diagnostics on the given brphy 1G port

    Execute 

    @behavior Sync, Non-Reeentrant

    @param[in]  aPhyIdx      Phy Index
    @param[in]  aPhyCntxt    Pointer to port context
    @param[in]  aConfig      Pointer to port configuration
    @param[in]  aParams      Input cable diagnostics parameters
    @param[out] aResult      Pointer to cable diagnostics result

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                  Cable diagnostics executed successfully
    @retval     #BCM_ERR_NOSUPPORT           Not Supported
    @retval     #BCM_ERR_INVAL_PARAMS        aCfg is NULL or invalid
    @retval     #BCM_ERR_INVAL_PARAMS        aPhyIdx is invalid
    @retval     #BCM_ERR_INVAL_PARAMS        aParams is invalid
    @retval     #BCM_ERR_INVAL_PARAMS        aResult is NULL
    @retval     #BCM_ERR_TIME_OUT            Cable diagnostics execution timed out
    @retval     #BCM_ERR_EAGAIN              Cable diagnostics returned an invalid error, retry again

    @post None

    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
int32_t BRPHY1GV2_PlatformRunCableDiag(uint8_t aPhyIdx,
                                   ETHXCVR_CntxtType *const aPhyCntxt,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   const ETHXCVR_AcdParamsType aParams,
                                   ETHXCVR_CableDiagResultType *const aResult);

#endif /* BRPHY1G_PLAT_H */

/** @} */
