/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @addtogroup grp_xcvr_brphy1g_plat_impl
    @{

    @file brphy1g_platform.c
    @brief This file implements the platform layer for BRPHY1G Transceiver driver.

    @version 0.1 Initial draft
*/


#include "./../inc/xcvr_functbl.h"
#include "./../rdb/brphy1gv2_afe_rdb.h"
#include "brphy1g_platform.h"
#include <system_mcu.h>
#if defined BRPHY1GV2_ACD_SUPPORT
#include "./../rdb/brphy1gv2_acd_rdb.h"
#endif
#include <cfg_rdb.h>

/**
    @name Transceiver driver platform Design IDs
    @{
    @brief Design IDs for Transceiver driver platform layer
*/
#define BRCM_SWDSGN_BRPHY1GV2_AFE_CTRL_REGS_GLOBAL             (0xC401U) /**< @brief #BRPHY1GV2_AFE_CTRL_REGS   */
#define BRCM_SWARCH_BRPHY1GV2_PLATFORM_TC10_CONFIG_PROC        (0xC402U) /**< @brief #BRPHY1GV2_PlatformTc10Config */
#define BRCM_SWDSGN_BRPHY1GV2_PLATFORM_AFE_CONFIG_PROC         (0xC403U) /**< @brief #BRPHY1GV2_PlatformAfeConfig  */
#define BRCM_SWDSGN_BRPHY1GV2_ACD_CTRL_REGS_GLOBAL             (0xC404U) /**< @brief #BRPHY1GV2_ACD_CTRL_REGS         */
#define BRCM_SWDSGN_BRPHY1GV2_PLATFORMRUNCABLEDIAG_PROC        (0xC405U) /**< @brief #BRPHY1GV2_PlatformRunCableDiag  */
#define BRCM_SWDSGN_BRPHY1GV2_CFG_REGS_GLOBAL                  (0xC406U) /**< @brief #BRPHY1GV2_CFG_Regs   */

/** @} */

#if (ENABLE_BRPHY1G_AFE_CONFIG == 1UL)
/**
    @brief BroadReach 1G AFE CTRL base addresses

    @trace #BRCM_SWARCH_BRPHY1GV2_PLATFORM_AFE_CONFIG_PROC
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL_HANDLER_BCM8915X
*/
static BRPHY1GV2_AFE_RDBType * const BRPHY1GV2_AFE_CTRL_REGS[BRPHY1GV2_AFE_MAX_HW_ID] =
{
#if (BRPHY1GV2_AFE_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_AFE_MAX_HW_ID == 0UL"
#endif
    (BRPHY1GV2_AFE_RDBType *const)BRPHY1GV2_0_AFE_BASE,
#if (BRPHY1GV2_AFE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_AFE_MAX_HW_ID > 1UL is not supported"
#endif
};
#endif

#if defined BRPHY1GV2_ACD_SUPPORT
/**
    @brief ACD block base addresses for BRPHY1G

    @trace #BRCM_SWARCH_BRPHY1GV2_PLATFORMRUNCABLEDIAG_PROC
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL_HANDLER_BCM8915X
*/
static BRPHY1GV2_ACD_RDBType * const BRPHY1GV2_ACD_CTRL_REGS[BRPHY1GV2_ACD_MAX_HW_ID] =
{
#if (BRPHY1GV2_ACD_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_ACD_MAX_HW_ID == 0UL"
#endif
    (BRPHY1GV2_ACD_RDBType *const)BRPHY0_1G_ACD_BASE,
#if (BRPHY1GV2_ACD_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_ACD_MAX_HW_ID > 1UL is not supported"
#endif
};
#endif
/**
    @brief Peripheral global configuration register.
    @trace #BRCM_SWARCH_BRPHY1GV2_PLATFORMRUNCABLEDIAG_PROC
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL_HANDLER_BCM8915X
*/
static CFG_RDBType * const BRPHY1GV2_CFG_Regs = (CFG_RDBType *)CFG_BASE;

/** @brief Get version

    API to get version

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]           aVersion      Pointer to version

    @return     void

    @post None

    @trace #BRCM_SWARCH_MCU_INFO_PROC
    @trace #BRCM_SWREQ_MCU_QUERY_BCM8915X

    @limitations None
*/
void MCU_GetVersion(MCU_VersionType *const aVersion)
{
    aVersion->rev
            = ((BRPHY1GV2_CFG_Regs->rev_chip_id & CFG_REV_CHIP_ID_REV_MASK) >> CFG_REV_CHIP_ID_REV_SHIFT);

    aVersion->model = ((BRPHY1GV2_CFG_Regs->rev_chip_id & CFG_REV_CHIP_ID_ID_MASK) >> CFG_REV_CHIP_ID_ID_SHIFT);
}

/** @brief Get MCU extended version

    This API retrieves the MCU version along with the chip family information.

    @behavior Sync, Re-entrant

    @pre MCU is initialized

    @param[in]      aVersion    Pointer to version structure

    @return void

    @post None

    @trace  #BRCM_SWREQ_MCU

    @limitations None
*/
void MCU_GetVersionExt(MCU_VersionExtType *const aVersion)
{
    aVersion->rev
            = ((BRPHY1GV2_CFG_Regs->rev_chip_id & CFG_REV_CHIP_ID_REV_MASK) >> CFG_REV_CHIP_ID_REV_SHIFT);

    aVersion->model = ((BRPHY1GV2_CFG_Regs->rev_chip_id & CFG_REV_CHIP_ID_ID_MASK) >> CFG_REV_CHIP_ID_ID_SHIFT);
	aVersion->family = (aVersion->model & MCU_CHIP_FAMILY_MASK);
}


/**
    @trace #BRCM_SWARCH_BRPHY1GV2_PLATFORM_TC10_CONFIG_PROC
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL_HANDLER_BCM8915X
*/
void BRPHY1GV2_PlatformTc10Config(void)
{
    /* Coverity fix: Function BRPHY1GV2_PlatformTc10Config is declared but never defined */
}

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_PLATFORM_AFE_CONFIG_PROC
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL_HANDLER_BCM8915X
*/
void BRPHY1GV2_PlatformAfeConfig(uint8_t aPhyIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_ModeType aMode)
{
    MCU_VersionExtType mcuVerExt = {0};
    MCU_GetVersionExt(&mcuVerExt);

#if (ENABLE_BRPHY1G_AFE_CONFIG == 1UL)
    /* Taken from phy_cfg script */
    if ( (MCU_CHIP_REVISION_A0 == mcuVerExt.rev) && (MCU_CHIP_MODEL_89581 != mcuVerExt.model) ) {
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->txconfig_9_phyx = BRPHY1GV2_AFE_TXCONFIG_9_PHYX_VAL;
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->txconfig_11_phyx = 0x1;
    }

    /* Taken from phy_cfg for B0 */
    if (MCU_CHIP_REVISION_B0 <= mcuVerExt.rev) {
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->clkgen_config2_phyx = BRPHY1GV2_AFE_CLKGEN_CONFIG2_PHYX_CONST1;
        BCM_CpuNDelay(20UL);
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->clkgen_config2_phyx = BRPHY1GV2_AFE_CLKGEN_CONFIG2_PHYX_CONST2;
        BCM_CpuNDelay(20UL);
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->clkgen_config2_phyx = BRPHY1GV2_AFE_CLKGEN_CONFIG2_PHYX_CONST1;
        BCM_CpuNDelay(20UL);
    }

    if (MCU_CHIP_REVISION_C0 == mcuVerExt.rev) {
        /* DC_Dual_Mode Fix for the Low Temp/Low Voltage (AVDD0p8 No Link Issues) */
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->rxconfig_6_phyx = BRPHY1GV2_AFE_RXCONFIG_6_PHYX_CONST;
        BRPHY1GV2_AFE_CTRL_REGS[aPhyIdx]->rxconfig_7_phyx = BRPHY1GV2_AFE_RXCONFIG_7_PHYX_CONST;
    }
#endif
}

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_PLATFORMRUNCABLEDIAG_PROC
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL_HANDLER_BCM8915X
*/
int32_t BRPHY1GV2_PlatformRunCableDiag(uint8_t aPhyIdx,
                                       ETHXCVR_CntxtType *const aPhyCntxt,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       const ETHXCVR_AcdParamsType aParams,
                                       ETHXCVR_CableDiagResultType *const aResult) {
    int32_t retVal = BCM_ERR_NOSUPPORT;
#if defined BRPHY1GV2_ACD_SUPPORT
    uint8_t cableType = (aParams & ETHXCVR_ACD_PARAMS_CABLETYPE_MASK) >> ETHXCVR_ACD_PARAMS_CABLETYPE_SHIFT;
    uint8_t lpType = (aParams & ETHXCVR_ACD_PARAMS_LPTYPE_MASK) >> ETHXCVR_ACD_PARAMS_LPTYPE_SHIFT;
    uint8_t acdEnableStatus = (aParams & ETHXCVR_ACD_PARAMS_STATUS_MASK) >> ETHXCVR_ACD_PARAMS_STATUS_SHIFT;
    uint32_t waitTicks = 0UL;
    uint16_t acdExpC0 = 0U;
    uint32_t lengthFactor = BRPHY1GV2_ACD_LF_1;
    uint32_t peakAmplitude = 0UL;
    uint32_t openThreshold = 0UL;
    uint32_t a,b,c,n = 0UL;

    if (((ETHXCVR_ACD_PARAMS_CABLETYPE_0 != cableType) && (ETHXCVR_ACD_PARAMS_CABLETYPE_1 != cableType)) ||
        ((ETHXCVR_ACD_PARAMS_LPTYPE_0 != lpType) && (ETHXCVR_ACD_PARAMS_LPTYPE_1 != lpType)) ||
        (ETHXCVR_ACD_PARAMS_STATUS_ENABLE != acdEnableStatus)) {
            retVal = BCM_ERR_INVAL_PARAMS;
    } else {
            retVal = BCM_ERR_OK;
        /* ACD init*/
        BRPHY1GV2_LINKSYNC_REGS[aPhyIdx]->control_a = BRPHY1GV2_LINK_SYNC_CONTROL_A_SYNC_LINK_CTRL_FORCE_MASK;

        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc7 = (uint16_t)0x0202U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc8 = (uint16_t)0x7F50U;
        if (ETHXCVR_ACD_PARAMS_LPTYPE_1 == lpType) {
            BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc9 = (uint16_t)0xAC22U;
        } else {
            BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc9 = (uint16_t)0x2C22U;
        }
        if (ETHXCVR_ACD_PARAMS_CABLETYPE_1 == lpType) { /*Leoni 647*/
            BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expca = (uint16_t)0x5252U;
        } else {
            BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expca = (uint16_t)0xD252U;
        }

        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcb = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcc = (uint16_t)0x0014U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expce = (uint16_t)0x1CA3U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcf = (uint16_t)0x0206U;
        /* page '01'*/
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe0 = (uint16_t)0x0010U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe1 = (uint16_t)0x0D0DU;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe2 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe3 = (uint16_t)0x7700U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe4 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe7 = (uint16_t)0U;
        /* Now load values*/
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expef = (uint16_t)0x409FU;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcd = (uint16_t)0x1129U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcd = (uint16_t)0x0129U;

        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe0 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe1 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe2 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe3 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe4 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe7 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expef = (uint16_t)0U;
        /* page '10'*/
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe0 = (uint16_t)0x3619U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe1 = (uint16_t)0x343AU;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe2 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe3 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe4 = (uint16_t)0xA000U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe5 = (uint16_t)0x000FU;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe7 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe9 = (uint16_t)0x0400U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->exped = (uint16_t)0U;
        /* Now load values*/
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expef = (uint16_t)0xA2BFU;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcd = (uint16_t)0x1129U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expcd = (uint16_t)0x0129U;

        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe0 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe1 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe2 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe3 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe4 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe5 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe7 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe8 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expe9 = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->exped = (uint16_t)0U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expef = (uint16_t)0U;
        /* ACD run*/
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc0 = (uint16_t)0x0400U;
        BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc0 = (uint16_t)0x8600U;

        for (waitTicks = 0UL; waitTicks < BRPHY1GV2_ACD_WAIT_LOOP_TIME; ++waitTicks) {
            acdExpC0 = BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc0;
            if (0U == (acdExpC0 & BRPHY1GV2_ACD_EXPC0_CABLE_DIAG_STATUS_MASK)) {
                break;
            }
        }
        if (waitTicks >= BRPHY1GV2_ACD_WAIT_LOOP_TIME) {
            retVal = BCM_ERR_TIME_OUT;
        } else {
            if (0U != (acdExpC0 & BRPHY1GV2_ACD_EXPC0_INVALID_RESULT_MASK)) {
                retVal = BCM_ERR_EAGAIN;
            }
        }
        if (BCM_ERR_OK == retVal) {
            if (0U == (acdExpC0 & BRPHY1GV2_ACD_EXPC0_CABLE_LEN_UNIT_MASK)) {
                lengthFactor = BRPHY1GV2_ACD_LF_100;
            }
            if (ETHXCVR_ACD_PARAMS_CABLETYPE_0 == cableType) {
                a = BRPHY1GV2_ACD_CONST_A_CB0_DEFAULT;
                b = BRPHY1GV2_ACD_CONST_B_CB0_DEFAULT;
                c = BRPHY1GV2_ACD_CONST_C_CB0_DEFAULT;
            } else {
                a = BRPHY1GV2_ACD_CONST_A_CB1_DEFAULT;
                b = BRPHY1GV2_ACD_CONST_B_CB1_DEFAULT;
                c = BRPHY1GV2_ACD_CONST_C_CB1_DEFAULT;
            }
            aResult->cableLen = 0UL;
            aResult->cableStatus = ETHXCVR_CABLE_STATUS_OK;
            switch(((BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc1 & BRPHY1GV2_ACD_EXPC1_PA_CD_CODE_MASK) >> BRPHY1GV2_ACD_EXPC1_PA_CD_CODE_SHIFT)) {
                case BRPHY1GV2_ACD_STATUS_INVALID:
                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_ERROR;
                    break;
                case BRPHY1GV2_ACD_STATUS_NOFAULT:
                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_OK;
                    break;
                case BRPHY1GV2_ACD_STATUS_OPEN:
                    if (ETHXCVR_ACD_PARAMS_CABLETYPE_0 == cableType) {
                        a = BRPHY1GV2_ACD_CONST_A_CB0_OPEN_DEFAULT;
                        b = BRPHY1GV2_ACD_CONST_B_CB0_OPEN_DEFAULT;
                        c = BRPHY1GV2_ACD_CONST_C_CB0_OPEN_DEFAULT;
                    } else {
                        a = BRPHY1GV2_ACD_CONST_A_CB1_OPEN_DEFAULT;
                        b = BRPHY1GV2_ACD_CONST_B_CB1_OPEN_DEFAULT;
                        c = BRPHY1GV2_ACD_CONST_C_CB1_OPEN_DEFAULT;
                    }

                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_OPEN;
                    peakAmplitude = ((BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc5 + BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc3)/2U) * BRPHY1GV2_ACD_PEAKAMPLITUDE_SCALE;
                    if (ETHXCVR_ACD_PARAMS_LPTYPE_0 == lpType) {
                        if (ETHXCVR_ACD_PARAMS_CABLETYPE_0 == cableType) {
                            openThreshold = BRPHY1GV2_ACD_OPENTHRESHOLD_112_400;
                        } else {
                            n = ((BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc6 + BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc4)/2U);
                            if (n <= BRPHY1GV2_ACD_OPENTHRESHOLD_N_LIMIT) {
                                /* open_threshold = 117 - 2.5 * (n - 6) */
                                openThreshold = (BRPHY1GV2_ACD_OPENTHRESHOLD_117_400 - (BRPHY1GV2_ACD_OPENTHRESHOLD_2_5_400 *(n - (BRPHY1GV2_ACD_OPENTHRESHOLD_N_LIMIT/2UL))));
                            } else {
                                /* open_threshold = 102 - 1.5357 * (n - 12) */
                                openThreshold = (BRPHY1GV2_ACD_OPENTHRESHOLD_102_400 - (BRPHY1GV2_ACD_OPENTHRESHOLD_1_535_400 *(n - BRPHY1GV2_ACD_OPENTHRESHOLD_N_LIMIT)));
                            }
                            if (BRPHY1GV2_ACD_OPENTHRESHOLD_118_400 < openThreshold) {
                                openThreshold = BRPHY1GV2_ACD_OPENTHRESHOLD_118_400;
                            }
                        }
                    } else {
                        openThreshold = BRPHY1GV2_ACD_OPENTHRESHOLD_100_400;
                    }
                    if (peakAmplitude < openThreshold) {
                        aResult->cableStatus = ETHXCVR_CABLE_STATUS_OK;
                        if (ETHXCVR_ACD_PARAMS_LPTYPE_0 == lpType) {
                            if (ETHXCVR_ACD_PARAMS_CABLETYPE_0 == cableType) {
                                a = BRPHY1GV2_ACD_CONST_A_CB0_LP0_OPEN_GOOD;
                                b = BRPHY1GV2_ACD_CONST_B_CB0_LP0_OPEN_GOOD;
                                c = BRPHY1GV2_ACD_CONST_C_CB0_LP0_OPEN_GOOD;
                            } else {
                                a = BRPHY1GV2_ACD_CONST_A_CB1_LP0_OPEN_GOOD;
                                b = BRPHY1GV2_ACD_CONST_B_CB1_LP0_OPEN_GOOD;
                                c = BRPHY1GV2_ACD_CONST_C_CB1_LP0_OPEN_GOOD;
                            }
                        } else {
                            if (ETHXCVR_ACD_PARAMS_CABLETYPE_0 == cableType) {
                                a = BRPHY1GV2_ACD_CONST_A_CB0_LP1_OPEN_GOOD;
                                b = BRPHY1GV2_ACD_CONST_B_CB0_LP1_OPEN_GOOD;
                                c = BRPHY1GV2_ACD_CONST_C_CB0_LP1_OPEN_GOOD;
                            } else {
                                a = BRPHY1GV2_ACD_CONST_A_CB1_LP1_OPEN_GOOD;
                                b = BRPHY1GV2_ACD_CONST_B_CB1_LP1_OPEN_GOOD;
                                c = BRPHY1GV2_ACD_CONST_C_CB1_LP1_OPEN_GOOD;
                            }
                        }
                    }
                    break;
                case BRPHY1GV2_ACD_STATUS_SHORT:
                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_SHORT;
                    if (ETHXCVR_ACD_PARAMS_CABLETYPE_0 == cableType) {
                        a = BRPHY1GV2_ACD_CONST_A_CB0_SHORT;
                        b = BRPHY1GV2_ACD_CONST_B_CB0_SHORT;
                        c = BRPHY1GV2_ACD_CONST_C_CB0_SHORT;
                    } else {
                        a = BRPHY1GV2_ACD_CONST_A_CB1_SHORT;
                        b = BRPHY1GV2_ACD_CONST_B_CB1_SHORT;
                        c = BRPHY1GV2_ACD_CONST_C_CB1_SHORT;
                    }
                break;
                case BRPHY1GV2_ACD_STATUS_PIN_SHORT_OR_XT:
                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_ERROR;
                break;
                case BRPHY1GV2_ACD_STATUS_FORCED:
                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_ERROR;
                break;
                default:
                    aResult->cableStatus = ETHXCVR_CABLE_STATUS_ERROR;
                break;
            }
            /* Length formula (ax-b)*LF - c*/
            aResult->cableLen = ((((((a*((BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc6 + BRPHY1GV2_ACD_CTRL_REGS[aPhyIdx]->expc4)/2U)) - b) * lengthFactor) - c) * 100UL)/ BRPHY1GV2_ACD_CONST_SCALE_DEFAULT);
        }
    }
#endif
    return retVal;
}

/** @} */
