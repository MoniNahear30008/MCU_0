/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
   @defgroup grp_macsecdrv_axi_impl MACSec driver AXI Bus implementation
   @ingroup grp_macsecdrv

   @addtogroup grp_macsecdrv_axi_impl
   @{
   @file macsec_axi.c
   @brief macsec driver AXI implementation.
   This file contains the MACSec driver AXI implementation

*/
#include <string.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <bcm_time.h>
#include "macsec_bus.h"
#include <crypto_secy.h>
#include "macsec_platform.h"
#include <system_mcu.h>

#include "rdb/macsec_base_addr_rdb.h"
#include "rdb/macsec_sysmisc_rdb.h"

#include "../../ethernet/transceiver/rdb/et_rdb.h"

/* MSPU top */
#include "rdb/macsec_mspu_mac_rdb.h"
#include "rdb/macsec_mspu_top_rdb.h"

/* BRCM global */
#include "rdb/macsec_e_brcm_global_rdb.h"
#include "rdb/macsec_i_brcm_global_rdb.h"


/**
    @name MACSEC driver Design IDs
    @{
    @brief Design IDs for MACSec driver
*/
#define BRCM_SWDSGN_MACSEC_MISC_ADDRTBL_GLOBAL                  (0xB200U) /**< @brief #MACSEC_MiscAddrTbl                */
#define BRCM_SWDSGN_MACSEC_MSPU_ADDRTBL_GLOBAL                  (0xB201U) /**< @brief #MACSEC_MspuAddrTbl                */
#define BRCM_SWDSGN_MACSEC_MAC_ADDRTBL_GLOBAL                   (0xB202U) /**< @brief #MACSEC_MacAddrTbl                 */
#define BRCM_SWDSGN_MACSEC_EIP_ADDRTBL_GLOBAL                   (0xB203U) /**< @brief #MACSEC_EIPAddrTbl                 */
#define BRCM_SWDSGN_MACSEC_BLK_ADDRTBL_GLOBAL                   (0xB204U) /**< @brief #MACSEC_BlkAddrTbl                 */
#define BRCM_SWDSGN_MACSEC_READ_PROC                            (0xB2B5U) /**< @brief #MACSEC_Read                       */
#define BRCM_SWDSGN_MACSEC_WRITE_PROC                           (0xB2B6U) /**< @brief #MACSEC_Write                      */
#define BRCM_SWDSGN_MACSEC_READ_MODIFY_WRITE_PROC               (0xB2B7U) /**< @brief #MACSEC_ReadModifyWrite            */
#define BRCM_SWDSGN_MACSEC_MSPU_TOPOFFSETOF_MACRO               (0xB2B8U) /**< @brief #MACSEC_MSPU_TOP_OFFSETOF          */

#define BRCM_SWDSGN_MACSEC_BUSDRV_GLOBAL_INIT_PROC              (0xB2BAU) /**< @brief #MACSEC_BusDrvGlobalInit           */
#define BRCM_SWDSGN_MACSEC_BUSDRV_EN_PROC                       (0xB2BBU) /**< @brief #MACSEC_BusDrvEnable               */
#define BRCM_SWDSGN_MACSEC_BUSDRV_DIS_PROC                      (0xB2BCU) /**< @brief #MACSEC_BusDrvDisable              */
#define BRCM_SWDSGN_MACSEC_BUSDRV_DEINIT_PROC                   (0xB2BDU) /**< @brief #MACSEC_BusDrvDeInit               */
#define BRCM_SWDSGN_MACSEC_MSPU_TOP_INTR_EN2_EIP_MASK_MACRO     (0xB2BEU) /**< @brief #MACSEC_MSPU_TOP_INTR_EN2_EIP_MASK */
#define BRCM_SWDSGN_MACSEC_MISC_REG1_ADDRTBL_GLOBAL             (0xB2BFU) /**< @brief #MACSEC_MiscReg1AddrTbl            */
#define BRCM_SWDSGN_MACSEC_MISC_REG1_A0_ADDRTBL_GLOBAL          (0xB2C0U) /**< @brief #MACSEC_MiscReg1A0AddrTbl          */

#define BRCM_SWDSGN_MACSEC_DUMMY_READ_PROC                      (0xB2C2U) /**< @brief #MACSEC_DummyRead                  */

/** @} */

/**
    @trace #BRCM_SWARCH_MACSEC_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
#define MACSEC_MSPU_TOP_OFFSETOF(member)  offsetof(MACSEC_MSPU_TOP_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MSPU_MAC_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_MSPU_MAC_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DE_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
#define MACSEC_MSPU_TOP_INTR_EN2_EIP_MASK      (0x1UL)

/**
    @brief ET block base address

    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWARCH_MACSEC_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
static ET_RDBType * const MACSEC_ET_REGS[ET_MAX_HW_ID] =
{
#if (ET_MAX_HW_ID == 0UL)
#error "ET_MAX_HW_ID = 0"
#endif
    (ET_RDBType *const)ET_BASE,
#if (ET_MAX_HW_ID > 1UL)
#error "ET_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief Crypto MSPU Block Base Address Table

    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWARCH_MACSEC_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
static const MACSEC_BlkAddrType MACSEC_MspuAddrTbl[MACSEC_MSPU_TOP_MAX_HW_ID] = {
#if (MACSEC_MSPU_TOP_MAX_HW_ID > 0)
        MSPU_TOP_REGS0_BASE,
#endif
#if (MACSEC_MSPU_TOP_MAX_HW_ID > 1UL)
#error "MACSEC_MSPU_TOP_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief Crypto MAC Block Base Address Table

    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWARCH_MACSEC_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
static const MACSEC_BlkAddrType MACSEC_MacAddrTbl[MACSEC_MSPU_MAC_MAX_HW_ID] = {
#if (MACSEC_MSPU_MAC_MAX_HW_ID > 0)
        MSPU_MAC0_BASE,
#endif
#if (MACSEC_MSPU_MAC_MAX_HW_ID > 1UL)
#error "MACSEC_MSPU_MAC_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief Crypto Ingress/Egress EIP Block Base Address Table

    (Base address of Ingress and Egress Block are same)

    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWARCH_MACSEC_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
static const MACSEC_BlkAddrType MACSEC_EIPAddrTbl[MACSEC_I_BRCM_GLOBAL_MAX_HW_ID] = {
#if MACSEC_I_BRCM_GLOBAL_MAX_HW_ID > 0
        EIP_160S_I_32_BRCM_GLOBAL0_BASE,
#endif
#if (MACSEC_I_BRCM_GLOBAL_MAX_HW_ID > 1UL)
#error "MACSEC_I_BRCM_GLOBAL_MAX_HW_ID > 1UL"
#endif
};


/**
    @brief Crypto Per Port Block Base Address Table

    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWARCH_MACSEC_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS
*/
static volatile const MACSEC_BlkAddrType * const MACSEC_BlkAddrTbl[MACSEC_BLOCK_COUNT_MAX+1] = {
   [MACSEC_BLOCK_MSPU_TOP] =  MACSEC_MspuAddrTbl,
   [MACSEC_BLOCK_MSPU_MAC] =  MACSEC_MacAddrTbl,
   [MACSEC_BLOCK_CORE] =  MACSEC_EIPAddrTbl,
};

/**
    @brief MACSec Dummy Read
    @code{.unparsed}
        return BCM_ERR_OK
    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
int32_t MACSEC_DummyRead(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset)
{
    return BCM_ERR_OK;
}

/**
    @brief MACSec Read
    @code{.unparsed}
    volatile uint32_t* blkAddr = NULL;
    for i = 0 to MACSEC_BLOCK_COUNT_MAX
        if MACSEC_BlkAddrTbl[i]->block == aBlkType
            blkAddr = (uint32_t *)MACSEC_BlkAddrTbl[i]->baseAddr[hwID]
            break;

    if blkAddr != NULL
        data = *(blkAddr + offset)

    return data

    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
uint32_t MACSEC_Read(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset)
{
    uint32_t rData = 0UL;
    volatile uint32_t *addr = NULL;
    MACSEC_HwIDType hwID;

    hwID =  MACSEC_GetHWId(aPort);

    if (hwID != 0xFFUL) {
        if ((NULL != MACSEC_BlkAddrTbl[aBlkType])
                && (hwID < MACSEC_MSPU_TOP_MAX_HW_ID)) {
            /* @event      CERT INT30-C
                          @issue       Coverity infers that MACSEC_BlkAddrTbl[aBlkType][hwID] + aOffset can wrap
                          @risk           No risk
                          @justification It is register base address plus offset, it will not wrap around. */
            /* coverity[cert_int30_c_violation] */
            addr = (volatile uint32_t *) ((uintptr_t)(MACSEC_BlkAddrTbl[aBlkType][hwID] + aOffset));
        }
    }

    if (NULL != addr) {
        rData = *addr;
    }

    return rData;
}

/**
    @brief MACSec Write
    @code{.unparsed}
    volatile uint32_t* blkAddr = NULL;
    for i = 0 to MACSEC_BLOCK_COUNT_MAX
        if MACSEC_BlkAddrTbl[i]->block == aBlkType
            blkAddr = (uint32_t *)MACSEC_BlkAddrTbl[i]->baseAddr[hwID]
            break;

    if blkAddr != NULL
        *(blkAddr + offset) = aData;

    @endcode
    @trace #BRCM_SWARCH_MACSEC_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
void MACSEC_Write(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset, uint32_t aData)
{
    volatile uint32_t *addr = NULL;
    MACSEC_HwIDType hwID;

    hwID =  MACSEC_GetHWId(aPort);
    if (hwID != 0xFFUL) {

        if ((NULL != MACSEC_BlkAddrTbl[aBlkType])
                && (hwID < MACSEC_MSPU_TOP_MAX_HW_ID)) {
            /* @event      CERT INT30-C
                          @issue       Coverity infers that MACSEC_BlkAddrTbl[aBlkType][hwID] + aOffset can wrap
                          @risk           No risk
                          @justification It is register base address plus offset, it will not wrap around. */
            /* coverity[cert_int30_c_violation] */
            addr = (volatile uint32_t *) ((uintptr_t)(MACSEC_BlkAddrTbl[aBlkType][hwID] + aOffset));
        }
    }

    if (NULL != addr) {
        *addr = aData;
    }
}

/**
    @brief MACSec Write
    @code{.unparsed}
    rData = MACSEC_Read(aConnHdl, aPort, aBlkType, aOffset);
    rData |= aData;
    MACSEC_Write(aConnHdl, aPort, aBlkType, aOffset, rData);

    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
void MACSEC_ReadModifyWrite(BCM_HandleType aConnHdl, uint32_t aPort, MACSEC_BlkType aBlkType, uint32_t aOffset, uint32_t aData)
{
    uint32_t rData;

    rData = MACSEC_Read(aConnHdl, aPort, aBlkType, aOffset);
    rData |= aData;
    MACSEC_Write(aConnHdl, aPort, aBlkType, aOffset, rData);
}

/**
    @brief
    @code{.unparsed}
    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
void MACSEC_BusDrvGlobalInit(BCM_HandleType aConnHdl, uint32_t aPort)
{
    MCU_VersionType mcuVer = {0};
    MACSEC_HwIDType hwID = MACSEC_GetHWId(aPort);

    (void)MCU_GetVersion(&mcuVer);

    /* Enable the top level interrupt */
    if (hwID != 0xFFUL) {
        /* MIB Control: Do not clear the MIB on read */
        MACSEC_ReadModifyWrite(aConnHdl, aPort, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSETOF(mib_ctrl), MACSEC_MSPU_TOP_MIB_CTRL_CNT_RD_CLR_DIS_MASK);
        MACSEC_ReadModifyWrite(aConnHdl, aPort, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSETOF(sw_mib_ctrl), MACSEC_MSPU_TOP_SW_MIB_CTRL_CNT_RD_CLR_DIS_MASK);

        /* enable interrupt from EIP engine in the MSPU Top */
        MACSEC_ReadModifyWrite(aConnHdl, aPort, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSETOF(interrupt_enable2), MACSEC_MSPU_TOP_INTR_EN2_EIP_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSETOF(interrupt_status2), 0x0UL);
    }
}

/**
    @brief
    @code{.unparsed}
    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
int32_t MACSEC_BusDrvEnable(BCM_HandleType aConnHdl, uint32_t aPort, const SECY_HwCfgType *const aCfg)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t data  = 0UL;
    MACSEC_HwIDType hwID = MACSEC_GetHWId(aPort);
    MCU_VersionType mcuVer = {0};

    (void)MCU_GetVersion(&mcuVer);

    if (hwID != 0xFFUL) {
         if(MCU_CHIP_FAMILY_8915X  == (mcuVer.model & MCU_CHIP_FAMILY_MASK)) {
                MACSEC_ET_REGS[hwID]->top_c0_reg &= ~ET_TOP_0_CTYPE_ETH_MSPU_EN_MASK;
                BCM_DelayUs(100UL);
                MACSEC_ET_REGS[hwID]->top_c0_reg |= ET_TOP_0_CTYPE_ETH_MSPU_EN_MASK;
                BCM_DelayUs(100UL);
                data = MACSEC_ET_REGS[hwID]->mspu_c0_reg;
                data |= ET_MSPU_0_CTYPE_MSPU_SWMAC_SERDES_SPEED_1000_MASK;
                data &= ~ET_MSPU_0_CTYPE_MSPU_SWMAC_SERDES_SPEED_100_MASK;
                data |= ET_MSPU_0_CTYPE_MSPU_SWMAC_SERDES_LINK_STAT_MASK;
                data |= ET_MSPU_0_CTYPE_MSPU_OUTBOUND_SEL_MASK;
                data |= ET_MSPU_0_CTYPE_MSPU_LNMAC_SERDES_SPEED_1000_MASK;
                data &= ~ET_MSPU_0_CTYPE_MSPU_LNMAC_SERDES_SPEED_100_MASK;
                data |= ET_MSPU_0_CTYPE_MSPU_INBOUND_SEL_MASK;
                MACSEC_ET_REGS[hwID]->mspu_c0_reg = data;
                BCM_DelayUs(100UL);
         } else {
         /*Support for further revision*/
         }
         ret = MACSEC_DrvUniMacInit(aConnHdl, aPort, aCfg);
    }
    return ret;
}

/**
    @brief
    @code{.unparsed}
    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
void MACSEC_BusDrvDisable(BCM_HandleType aConnHdl, uint32_t aPort)
{
    MACSEC_HwIDType hwID = MACSEC_GetHWId(aPort);

    uint32_t data = 0UL;

    MCU_VersionType mcuVer = {0};

    (void)MCU_GetVersion(&mcuVer);

    if (hwID != 0xFFUL) {
        if(MCU_CHIP_FAMILY_8915X  == (mcuVer.model & MCU_CHIP_FAMILY_MASK)) {
            data = MACSEC_ET_REGS[hwID]->mspu_c0_reg;
            data &= ~ET_MSPU_0_CTYPE_MSPU_OUTBOUND_SEL_MASK;
            data &= ~ET_MSPU_0_CTYPE_MSPU_INBOUND_SEL_MASK;
            MACSEC_ET_REGS[hwID]->mspu_c0_reg = data;
        } else {
             /*Support for further revision*/
        }
    }
}

/**
    @brief
    @code{.unparsed}
    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
void MACSEC_BusDrvDeInit(BCM_HandleType aConnHdl, uint32_t aPort)
{
    MCU_VersionType mcuVer = {0};
    MACSEC_HwIDType hwID = MACSEC_GetHWId(aPort);

    (void)MCU_GetVersion(&mcuVer);

    if (hwID != 0xFFUL) {
        /* Disable the MSPU Top EIP interrupt */
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSETOF(interrupt_enable2), 0x0UL);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSETOF(interrupt_status2), 0x0UL);

        if(MCU_CHIP_FAMILY_8915X  == (mcuVer.model & MCU_CHIP_FAMILY_MASK)) {
                /* Now, disable the MACSEC on that port */
                (void)MACSEC_BusDrvDisable(aConnHdl, aPort);
        } else {
                 /*Support for further revision*/
        }
    }
}

/**
    @brief
    @code{.unparsed}
    @endcode
    @trace #BRCM_SWARCH_MACSEC_READ_MODIFY_WRITE_PROC
    @trace #BRCM_SWREQ_MACSEC_SECY_OPERATIONS

*/
int32_t MACSEC_BusDrvUnimacSpeedSet(uint32_t aPort, ETHXCVR_SpeedType aSpeed)
{
    int32_t ret = BCM_ERR_OK;
    BCM_HandleType aConnHdl = 0ULL;
    uint32_t cmdConfig = 0UL;
    uint32_t flushControl = 0UL;
    /* configure the line side and system side mac speed  */
    if((MACSEC_ET_REGS[aPort]->mspu_c0_reg & ET_MSPU_0_CTYPE_MSPU_INBOUND_SEL_MASK) > 0UL) {
        /* Disable pause functions. */
        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config));
        cmdConfig |= (MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_PAUSE_IGNORE_MASK | MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_IGNORE_TX_PAUSE_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config), cmdConfig);

        /* Enable flush control enable. */
        flushControl = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_flush_control));
        flushControl |= MACSEC_MSPU_MAC_LUFC_FLUSH_CONTROL_FLUSH_MASK;
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_flush_control), flushControl);

        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config));
        cmdConfig |= MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_SW_RESET_MASK;
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config), cmdConfig);
        BCM_CpuNDelay(10);
        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config));
        cmdConfig &= ~MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_MASK;
        if(aSpeed == ETHXCVR_SPEED_100MBPS) {
            cmdConfig |= (MACSEC_MACSPEED_100MBPS << MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        } else {
            cmdConfig |= (MACSEC_MACSPEED_1000MBPS << MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        }

        cmdConfig &= ~(MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_TX_ENA_MASK | MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_RX_ENA_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config), cmdConfig);

        /* Disable pause functions. */
        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config));
        cmdConfig |= (MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_PAUSE_IGNORE_MASK | MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_IGNORE_TX_PAUSE_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config), cmdConfig);

        /* Enable flush control enable. */
        flushControl = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_flush_control));
        flushControl |= MACSEC_MSPU_MAC_SUFC_FLUSH_CONTROL_FLUSH_MASK;
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_flush_control), flushControl);

        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config));
        cmdConfig |= MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_SW_RESET_MASK;
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config), cmdConfig);
        BCM_CpuNDelay(10);

        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config));
        cmdConfig &= ~MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_MASK;
        if(aSpeed == ETHXCVR_SPEED_100MBPS) {
            cmdConfig |= (MACSEC_MACSPEED_100MBPS << MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        } else {
            cmdConfig |= (MACSEC_MACSPEED_1000MBPS << MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        }

        cmdConfig &= ~(MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_TX_ENA_MASK | MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_RX_ENA_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config), cmdConfig);
        /* Bring MAC out of reset */
        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config));
        cmdConfig &= ~MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_SW_RESET_MASK;
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config), cmdConfig);
        BCM_CpuNDelay(10);
        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config));
        cmdConfig &= ~MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_SW_RESET_MASK;
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config), cmdConfig);

        BCM_CpuNDelay(10);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_flush_control), 0UL);

        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_flush_control), 0UL);

        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config));
        cmdConfig &= ~(MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_PAUSE_IGNORE_MASK | MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_IGNORE_TX_PAUSE_MASK);
        cmdConfig |= (MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_TX_ENA_MASK | MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_RX_ENA_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config), cmdConfig);

        cmdConfig = MACSEC_Read(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config));
        cmdConfig &= ~(MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_PAUSE_IGNORE_MASK | MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_IGNORE_TX_PAUSE_MASK);
        cmdConfig |= (MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_TX_ENA_MASK | MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_RX_ENA_MASK);
        MACSEC_Write(aConnHdl, aPort, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config), cmdConfig);
    }
    return ret;
}

/** @} */

