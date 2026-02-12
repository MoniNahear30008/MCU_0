/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvr_functbl_impl Transceiver Function Table Design
    @ingroup grp_transceiverdrv

    @addtogroup grp_xcvr_functbl_impl
    @{

    @file drivers/ethernet/transceiver/xcvr_functbl.c
    @brief Transceiver function table design

    @version 0.1 Initial version
*/
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>
#include "./inc/xcvr_functbl.h"

/**
    @name Module Design IDs
    @{
    @brief Design IDs for Mdio Module
*/
#define BRCM_SWDSGN_ETHXCVR_EXTFUNTBL_LIST_GLOBAL        (0xBC50U) /**< @brief #ETHXCVR_PhyFnTblList             */
#define BRCM_SWDSGN_ETHXCVR_PHYTC10FNTBLLIST_GLOBAL      (0xBC51U) /**< @brief #ETHXCVR_PhyTC10FnTblList         */
#define BRCM_SWDSGN_ETHXCVR_PHYTC10NOTIFYLIST_GLOBAL     (0xBC52U) /**< @brief #ETHXCVR_PhyTC10NotifyList        */
#define BRCM_SWDSGN_ETHXCVR_PLATFORMGETFNTBLSCNTXT_PROC  (0xBC53U) /**< @brief #ETHXCVR_PlatformGetFnTblsCntxt   */
/** @} */

#define ENABLE_TC10
#define BRPHY1G_TC10_FUNCTBL

static const ETHXCVR_FuncTblType *const ETHXCVR_PhyFnTblList[2] = {
    &BRPHY1GV2_FuncTbl,
    &S16UP_FuncTble,
};

#ifdef ENABLE_TC10
/**
    @brief Global phy TC10 function table list for all external phy

    @trace #BRCM_SWARCH_ETHXCVR_EXTPHYTC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_ETHXCVR
*/
static const ETHXCVR_ExtPhyTC10FuncTblType *const ETHXCVR_PhyTC10FnTblList[] = {
#ifdef BRPHY1G_TC10_FUNCTBL
    &BRPHY1GV2_TC10FuncTbl,
#endif
};

#ifdef ENABLE_XCVR_LOCAL_WAKE_NOTIFICATION
/**
    @brief TC10 Notification List

    @trace #BRCM_SWARCH_ETHXCVR_EXTPHYTC10FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_ETHXCVR
*/
static const  BCM_CompIDType ETHXCVR_PhyTC10NotifyList[] = {
    BCM_TC10MOD_ID,
};
#endif
#endif


/**
    @trace #BRCM_SWARCH_ETHXCVR_PLATFORMGETFNTBLSCNTXT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER_BCM8910X
*/
int32_t ETHXCVR_PlatformGetFnTblsCntxt(ETHXCVR_IDType aID,
                                               const ETHXCVR_FuncTblType **const aPhyFnTbl)
{
    int32_t retVal;
    if (ETHXCVR_ID_CU_PHY == aID) {
           *aPhyFnTbl = ETHXCVR_PhyFnTblList[ETHXCVR_ID_CU_PHY];
           retVal = BCM_ERR_OK;
    } else if (ETHXCVR_ID_SERDES_PHY == aID) {
           *aPhyFnTbl = ETHXCVR_PhyFnTblList[ETHXCVR_ID_SERDES_PHY];
           retVal = BCM_ERR_OK;
    } else {
          retVal = BCM_ERR_NOSUPPORT;
    }
    return retVal;
}

#ifdef ENABLE_TC10
/**
    @trace #BRCM_SWARCH_ETHXCVR_PLATFORMGETFNTBLSCNTXT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER_BCM8910X
*/
int32_t ETHXCVR_PlatformGetTc10FnTblsCntxt(ETHXCVR_IDType aID,
                                               const ETHXCVR_ExtPhyTC10FuncTblType **const aPhyTc10FnTbl)
{
    int32_t retVal;
    if (ETHXCVR_ID_CU_PHY == aID) {
           *aPhyTc10FnTbl = ETHXCVR_PhyTC10FnTblList[ETHXCVR_ID_CU_PHY];
           retVal = BCM_ERR_OK;
    } else {
          retVal = BCM_ERR_NOSUPPORT;
    }
    return retVal;
}
#endif

/** @brief sub-function to calculate the OUI for chip's model ID
      @trace #BRCM_SWREQ_ETHXCVR_COMMON
  */
uint32_t ETHXCVR_PhybitReverse(uint32_t n)
{
    n = (((n & 0xaaaaaaaaU) >> 1U) | ((n & 0x55555555U) << 1U));
    n = (((n & 0xccccccccU) >> 2U) | ((n & 0x33333333U) << 2U));
    n = (((n & 0xf0f0f0f0U) >> 4U) | ((n & 0x0f0f0f0fU) << 4U));

    return n;
}


/** @} */
