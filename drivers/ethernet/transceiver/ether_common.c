/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_ether_common_impl Transceiver Driver Design
    @ingroup grp_transceiverdrv

    @addtogroup grp_ether_common_impl
    @{

    @file drivers/ethernet/transceiver/ether_common.c
    @brief This file implements the ethernet common driver API.

    @version 0.1 Initial version
*/

#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <ethernet_common.h>
#include "hsio_common_rdb.h"

/**
    @name Ethernet common driver  Design IDs
    @{
    @brief Design IDs for Ethernet common driver.
*/
#define BRCM_SWDSGN_ETHER_COMMON_REGS_GLOBAL                    (0xBAA0U) /**< @brief #ETHER_COMMON_REGS  */
#define BRCM_SWDSGN_ETHER_COMMON_INIT_PROC                      (0xBAA1U) /**< @brief #ETHER_CommonInit  */

/** @} */

/**
    @brief ET block base address

    @trace #BRCM_SWARCH_ETHER_COMMON_INIT_PROC
    @trace #BRCM_SWREQ_ETHER
*/
static HSIO_COMMON_RDBType * const ETHER_COMMON_REGS[HSIO_COMMON_MAX_HW_ID] =
{
#if (HSIO_COMMON_MAX_HW_ID == 0UL)
#error "HSIO_COMMON_MAX_HW_ID = 0"
#endif
    (HSIO_COMMON_RDBType *const)HSIO_COMMON_BASE,
#if (HSIO_COMMON_MAX_HW_ID > 1UL)
#error "HSIO_COMMON_MAX_HW_ID > 1UL"
#endif
};


/** @brief Initialize Ethernet common

    This API initializes the Ethernet top level common properties.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      hwId            Index of the Ethernet Hw.
    @param[in]      extPhySel     Select external PHY 0 = BRPHY (CU-PHY 100M/1G), 1 = S16UP (External PHY 2.5G)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID

    @post None

    @limitations None

    @code{.unparsed}
    Reset HSIO ethernet block using hsio top register
    Select external PHY select
    @endcode

    @trace #BRCM_SWARCH_ETHER_COMMON_INIT_PROC
    @trace #BRCM_SWREQ_ETHER
*/
int32_t ETHER_CommonInit(ETHER_HwIDType hwId, uint8_t extPhySel)
{
    int32_t retVal = BCM_ERR_OK;
    if(hwId >= HSIO_COMMON_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Reset HSIO ethernet blocks */
        ETHER_COMMON_REGS[hwId]->hsio_ctrl_0 |= (HSIO_COMMON_HSIO_CTRL_32BIT_0_ETH_SYS_RESET_N_MASK | HSIO_COMMON_HSIO_CTRL_32BIT_0_BRPHY_PLL_RESET_N_MASK | HSIO_COMMON_HSIO_CTRL_32BIT_0_ETH_CRST_N_MASK
                                               | HSIO_COMMON_HSIO_CTRL_32BIT_0_S16U_REFCLK_RESET_N_MASK);
        BCM_DelayUs(10UL);
        /* Valid voltage calibration enable */
        ETHER_COMMON_REGS[hwId]->hsio_ctrl_5 |= (HSIO_COMMON_HSIO_CTRL_32BIT_5_RES_CAL_VLD_OVRD_SEL_MASK | HSIO_COMMON_HSIO_CTRL_32BIT_5_RES_CAL_VLD_OVRD_VAL_MASK |
                                                HSIO_COMMON_HSIO_CTRL_32BIT_5_ETH_RES_CAL_OVRD_SEL_MASK);
        /* Enable ethernet block. */
        ETHER_COMMON_REGS[hwId]->hsio_ctrl_1 &= (~HSIO_COMMON_HSIO_CTRL_32BIT_1_GPIO_SEL_MASK);
        ETHER_COMMON_REGS[hwId]->hsio_ctrl_1 |= HSIO_COMMON_HSIO_CTRL_32BIT_1_RESERVED_12_MASK;
        /* Select PHY 0 = BRPHY (100M/1G), 1 = S16UP (2.5G) */
        if(extPhySel != 0U) {
            ETHER_COMMON_REGS[hwId]->eth_ctrl_0 |= HSIO_COMMON_ETH_CTRL_32BIT_0_EXTPHY_SEL_MASK;
        } else {
            ETHER_COMMON_REGS[hwId]->eth_ctrl_0 &= ~HSIO_COMMON_ETH_CTRL_32BIT_0_EXTPHY_SEL_MASK;
        }
        BCM_DelayUs(5000UL);
    }
    return retVal;
}

/** @} */
