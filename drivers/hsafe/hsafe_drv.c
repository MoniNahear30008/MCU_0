/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_hsafedrv_drv_impl High speed analog frontend driver Design
    @ingroup grp_hsafedrv

    @addtogroup grp_hsafedrv_drv_impl
    @{

    @file drivers/hsafe/hsafe_drv.c
    @brief HSAFE Driver Design

    @version 0.1 Initial version
*/

#include <hsafe.h>
#include <hsafe_rdb.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>

/**
    @name HSAFE Driver Design IDs
    @{
    @brief Driver Design IDs for HSAFE

*/
#define BRCM_SWDSGN_HSAFE_DRV_INIT_PROC                    (0xB101U)    /**< @brief #HSAFE_DrvInit          */
#define BRCM_SWDSGN_HSAFE_DRV_DEINIT_PROC                  (0xB102U)    /**< @brief #HSAFE_DrvDeInit        */
#define BRCM_SWDSGN_HSAFE_DRV_PLL_CONFIG_PROC              (0xB104U)    /**< @brief #HSAFE_DrvPllConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_PLL_CLK_GEN_RSTB_PROC        (0xB105U)    /**< @brief #HSAFE_DrvPllClkGenRstb*/
#define BRCM_SWDSGN_HSAFE_DRV_PLL_POWER_CONFIG_PROC        (0xB106U)    /**< @brief #HSAFE_DrvPllPowerConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_PLL_RESET_PROC               (0xB107U)    /**< @brief #HSAFE_DrvPllReset*/
#define BRCM_SWDSGN_HSAFE_DRV_PLL_LOCK_GET_PROC            (0xB108U)    /**< @brief #HSAFE_DrvPllLockGet*/
#define BRCM_SWDSGN_HSAFE_DRV_HSADC_POWER_CONFIG_PROC      (0xB109U)    /**< @brief #HSAFE_DrvHsAdcPowerConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_HSADC_RESET_PROC             (0xB10AU)    /**< @brief #HSAFE_DrvHsAdcReset*/
#define BRCM_SWDSGN_HSAFE_DRV_HSREF_POWERCONFIG_PROC       (0xB10BU)    /**< @brief #HSAFE_DrvHsRefPowerConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_HSREF_RESET_PROC             (0xB10CU)    /**< @brief #HSAFE_DrvHsRefReset*/
#define BRCM_SWDSGN_HSAFE_DRV_HSADC_CONFIG_PROC            (0xB10DU)    /**< @brief #HSAFE_DrvHsAdcConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_RCCAL_RESET_PROC             (0xB10EU)    /**< @brief #HSAFE_DrvRccalReset*/
#define BRCM_SWDSGN_HSAFE_DRV_RCCAL_POWERCONFIG_PROC       (0xB10FU)    /**< @brief #HSAFE_DrvRccalPowerConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_RCCAL_STATUS_PROC            (0xB110U)    /**< @brief #HSAFE_DrvRcCalStatus*/
#define BRCM_SWDSGN_HSAFE_DRV_RESCAL_RESETB_PROC           (0xB111U)    /**< @brief #HSAFE_DrvRescalResetb*/
#define BRCM_SWDSGN_HSAFE_DRV_RESCAL_POWERCONFIG_PROC      (0xB112U)    /**< @brief #HSAFE_DrvRescalPowerConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_RESCAL_STATUS_PROC           (0xB113U)    /**< @brief #HSAFE_DrvRescalStatus*/
#define BRCM_SWDSGN_HSAFE_DRV_ADC_CLKCFG_PROC              (0xB114U)    /**< @brief #HSAFE_DrvAdcClkCfg*/
#define BRCM_SWDSGN_HSAFE_DRV_AWGDAC_CLOCK_CFG_PROC        (0xB115U)    /**< @brief #HSAFE_DrvAwgDacClockCfg*/
#define BRCM_SWDSGN_HSAFE_DRV_OCP_CLOCK_CFG_PROC           (0xB116U)    /**< @brief #HSAFE_DrvOcpClockCfg*/
#define BRCM_SWDSGN_HSAFE_DRV_TXSLZR_RESET_PROC            (0xB117U)    /**< @brief #HSAFE_DrvTxslzrReset*/
#define BRCM_SWDSGN_HSAFE_DRV_TXSLZR_POWERCONFIG_PROC      (0xB118U)    /**< @brief #HSAFE_DrvTxslzrPowerConfig*/
#define BRCM_SWDSGN_HSAFE_DRV_RESCAL_CONFIG_PROC           (0xB11CU)    /**< @brief #HSAFE_DrvResCalConfig*/


#define BRCM_SWDSGN_HSAFE_REGS_GLOBAL            (0xB119U)    /**< @brief #HSAFE_Regs */
#define BRCM_SWDSGN_HSAFE_DRV_DEV_TYPE           (0xB11AU)    /**< @brief #HSAFE_DrvDevType */
#define BRCM_SWDSGN_HSAFE_DRV_DEV_GLOBAL         (0xB11BU)    /**< @brief #HSAFE_DrvDev */
/** @} */

/**
    @trace #BRCM_SWARCH_HSAFE_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_CLK_GEN_RSTB_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_LOCK_GET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSREF_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSREF_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_STATUS_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_RESETB_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_STATUS_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_ADC_CLKCFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_AWGDAC_CLOCK_CFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_OCP_CLOCK_CFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_TXSLZR_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_TXSLZR_POWERCONFIG_PROC
    @trace #BRCM_SWREQ_HSAFE_INIT
*/
static HSAFE_RDBType * const HSAFE_Regs[HSAFE_MAX_HW_ID] =
{
#if (HSAFE_MAX_HW_ID == 0U)
#error "HSAFE_MAX_HW_ID == 0U"
#endif
    (HSAFE_RDBType *)HSAFE_BASE,
#if (HSAFE_MAX_HW_ID > 1U)
#error "HSAFE_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_HSAFE_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_CLK_GEN_RSTB_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_LOCK_GET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSREF_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSREF_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_STATUS_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_RESETB_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_STATUS_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_ADC_CLKCFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_AWGDAC_CLOCK_CFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_OCP_CLOCK_CFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_TXSLZR_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_TXSLZR_POWERCONFIG_PROC
    @trace #BRCM_SWREQ_HSAFE_INIT
*/
typedef struct sHSAFE_DrvDevType {
    HSAFE_StateType   state;            /**< State of the driver                */
} HSAFE_DrvDevType;

/**
    @trace #BRCM_SWREQ_HSAFE_INIT
    @trace #BRCM_SWARCH_HSAFE_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_CLK_GEN_RSTB_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_PLL_LOCK_GET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSREF_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSREF_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_HSADC_CONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RCCAL_STATUS_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_RESETB_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_POWERCONFIG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_RESCAL_STATUS_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_ADC_CLKCFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_AWGDAC_CLOCK_CFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_OCP_CLOCK_CFG_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_TXSLZR_RESET_PROC
    @trace #BRCM_SWARCH_HSAFE_DRV_TXSLZR_POWERCONFIG_PROC
*/
static HSAFE_DrvDevType HSAFE_DrvDev[HSAFE_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/** @brief Initialize HSAFE

    This API initializes the given HSAFE hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller

    @return     int32_t

    @post None

    @trace  #BRCM_SWREQ_HSAFE_INIT
    @trace  #BRCM_SWARCH_HSAFE_DRV_INIT_PROC

    @code{.unparsed}
    Validate HW index.
    Move driver state to HSAFE_STATE_INITIALIZED
    @endcode

    @limitations None
*/

int32_t HSAFE_DrvInit(HSAFE_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HSAFE_DrvDev[hwId].state = HSAFE_STATE_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief De-initialize HSAFE

    This API de-initializes HSAFE. Resets the given HSAFE controller and puts back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSAFE_INIT
    @trace  #BRCM_SWARCH_HSAFE_DRV_DEINIT_PROC

    @code{.unparsed}
    Validate HW index.
    Move driver state to HSAFE_STATE_UNINIT
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvDeInit (HSAFE_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HSAFE_DrvDev[hwId].state = HSAFE_STATE_UNINIT;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSAFE PLL configuration

    This API configures HSAFE PLL.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_PLL_CONFIG_PROC

    @code{.unparsed}
    Validate HW index and driver state.
    Write bias config register with pre calculated values.
    Program pll_cal_config3 with PLL lock speed up.
    Deassert PLL power down and Resetb.
    Take PLL out of Reset.
    @endcode

    @limitations None
*/

int32_t HSAFE_DrvPllConfig(HSAFE_HwIDType hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        HSAFE_Regs[hwId]->bias_config2 = 0x9248UL;
        HSAFE_Regs[hwId]->bias_config3 = 0x9248UL;
        HSAFE_Regs[hwId]->bias_config4 = 0x9248UL;
        HSAFE_Regs[hwId]->bias_config5 = 0x9248UL;
        HSAFE_Regs[hwId]->bias_config6 = 0x9248UL;
        HSAFE_Regs[hwId]->bias_config7 = 0x9248UL;
        HSAFE_Regs[hwId]->pll_config11 = 0UL;

        /* PLL lock Speed up*/
        HSAFE_Regs[hwId]->pll_cal_config3 = 0xB003UL;

        /* Deassert power down and reset b*/
        HSAFE_Regs[hwId]->pll_control0 = 0x2UL;

        /* Assert bit 0*/
        HSAFE_Regs[hwId]->pll_control0 = 0x1UL;


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSAFE Clock gen reset

    This API resets main PLL clock generator.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      clkGenRstb   0 - put under reset 1 - Take out of reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid clkGenRstb value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_PLL_CLK_GEN_RSTB_PROC

    @code{.unparsed}
    Validate HW index, driver state and clkGenRstb.
    Modify pll_control0 register with user provided clkGenRstb

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvPllClkGenRstb(HSAFE_HwIDType hwId, uint8_t clkGenRstb)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (clkGenRstb > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    }else {
#endif
        /* Clock gen reset b programming*/
        rdData = HSAFE_Regs[hwId]->pll_control0;
        rdData &= ~HSAFE_PLL_CONTROL0_CLKGEN_RSTB_MASK;
        rdData |= ((uint32_t)clkGenRstb << HSAFE_PLL_CONTROL0_CLKGEN_RSTB_SHIFT);
        HSAFE_Regs[hwId]->pll_control0 = rdData;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSAFE PLL power

    This API powers down or powers up PLL .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      pwrUpDown    0 - Power Down 1 - Power Up

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_PLL_POWER_CONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state and pwrUpDown.
    Modify pll_control0 register with user provided pwrUpDown
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvPllPowerConfig(HSAFE_HwIDType hwId, uint8_t pwrUpDown)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (pwrUpDown > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* PLL power up/down programming*/
        rdData = HSAFE_Regs[hwId]->pll_control0;
        rdData &= ~HSAFE_PLL_CONTROL0_PWRDN_MASK;
        rdData |= (uint32_t)(((pwrUpDown == 0U) ? 1UL:0UL) << HSAFE_PLL_CONTROL0_PWRDN_SHIFT);
        HSAFE_Regs[hwId]->pll_control0 = rdData;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSAFE PLL Reset

    This API resets the whole PLL .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rstPll       0 - reset whole PLL 1 - Bring PLL out of reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_PLL_RESET_PROC

    @code{.unparsed}
    Validate HW index, driver state and rstPll.
    Modify pll_control0 register with user provided rstPll.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvPllReset(HSAFE_HwIDType hwId, uint8_t rstPll)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    }  else if (rstPll > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* PLL power down programming*/
        rdData = HSAFE_Regs[hwId]->pll_control0;
        rdData &= ~HSAFE_PLL_CONTROL0_RESETB_MASK;
        rdData |= ((uint32_t)rstPll << HSAFE_PLL_CONTROL0_RESETB_SHIFT);
        HSAFE_Regs[hwId]->pll_control0 = rdData;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get PLL lock status

    This API gets PLL lock status .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[out]     pllLock      PLL lock status

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for pllLock
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_PLL_LOCK_GET_PROC

    @code{.unparsed}
    Validate HW index, driver state and pllLock.
    Get Pll lock status from pll_status0 register.
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvPllLockGet(HSAFE_HwIDType hwId, uint8_t *pllLock)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (pllLock == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *pllLock = (uint8_t)(HSAFE_Regs[hwId]->pll_status0 & HSAFE_PLL_STATUS0_LOCK_MASK);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief High speed ADC power

    This API configures high speed ADC power based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      adcInstance  HSADC instance [0-3]
    @param[in]      pwr          0 - Disables selected HSADC
                                 1 - Enables selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcInstance
    @return     #BCM_ERR_INVAL_PARAMS   Invalid pwr
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_HSADC_POWER_CONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state, adcInstance and pllLock.
    Based on adcInstance read adcx_control0.
    Modify adcx_control0 power down based on user provided pwr.
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvHsAdcPowerConfig(HSAFE_HwIDType hwId, uint8_t adcInstance, uint8_t pwr)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (adcInstance >= 4U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (pwr > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        switch (adcInstance) {
            case 0U:
                rdData = HSAFE_Regs[hwId]->adc0_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_PWRDN_MASK;
                rdData |= (((pwr == 0U)? 1UL:0UL) << HSAFE_ADC_CONTROL0_PWRDN_SHIFT) ;
                HSAFE_Regs[hwId]->adc0_control0 = rdData;
                break;
            case 1U:
                rdData = HSAFE_Regs[hwId]->adc1_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_PWRDN_MASK;
                rdData |= (((pwr == 0U)? 1UL:0UL) << HSAFE_ADC_CONTROL0_PWRDN_SHIFT) ;
                HSAFE_Regs[hwId]->adc1_control0 = rdData;
                break;
            case 2U:
                rdData = HSAFE_Regs[hwId]->adc2_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_PWRDN_MASK;
                rdData |= (((pwr == 0U)? 1UL:0UL) << HSAFE_ADC_CONTROL0_PWRDN_SHIFT) ;
                HSAFE_Regs[hwId]->adc2_control0 = rdData;
                break;
            case 3U:
                rdData = HSAFE_Regs[hwId]->adc3_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_PWRDN_MASK;
                rdData |= (((pwr == 0U)? 1UL:0UL) << HSAFE_ADC_CONTROL0_PWRDN_SHIFT) ;
                HSAFE_Regs[hwId]->adc3_control0 = rdData;
                break;
            default :
                ret = BCM_ERR_INVAL_PARAMS;
                break;

        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief High speed ADC reset

    This API configures high speed ADC reset based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      adcInstance  HSADC instance [0-3]
    @param[in]      rstAdc       0 - Resets selected HSADC
                                 1 - out of reset selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcInstance
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rstAdc
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_HSADC_RESET_PROC

    @code{.unparsed}
    Validate HW index, driver state, adcInstance and rstAdc.
    Based on adcInstance read adcx_control0.
    Modify adcx_control0 resetB based on user provided rstAdc.
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvHsAdcReset(HSAFE_HwIDType hwId, uint8_t adcInstance, uint8_t rstAdc)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (adcInstance >= 4U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (rstAdc > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        switch (adcInstance) {
            case 0U:
                rdData = HSAFE_Regs[hwId]->adc0_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_RESETB_MASK;
                rdData |= ((uint32_t)rstAdc << HSAFE_ADC_CONTROL0_RESETB_SHIFT) ;
                HSAFE_Regs[hwId]->adc0_control0 = rdData;
                break;
            case 1U:
                rdData = HSAFE_Regs[hwId]->adc1_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_RESETB_MASK;
                rdData |= ((uint32_t)rstAdc << HSAFE_ADC_CONTROL0_RESETB_SHIFT) ;
                HSAFE_Regs[hwId]->adc1_control0 = rdData;
                break;
            case 2U:
                rdData = HSAFE_Regs[hwId]->adc2_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_RESETB_MASK;
                rdData |= ((uint32_t)rstAdc << HSAFE_ADC_CONTROL0_RESETB_SHIFT) ;
                HSAFE_Regs[hwId]->adc2_control0 = rdData;
                break;
            case 3U:
                rdData = HSAFE_Regs[hwId]->adc3_control0;
                rdData &= ~HSAFE_ADC_CONTROL0_RESETB_MASK;
                rdData |= ((uint32_t)rstAdc << HSAFE_ADC_CONTROL0_RESETB_SHIFT) ;
                HSAFE_Regs[hwId]->adc3_control0 = rdData;
                break;
            default :
                ret = BCM_ERR_INVAL_PARAMS;
                break;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief High speed Reference ADC power

    This API configures high speed reference ADC power based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      pwr          0 - Disables selected HSADC
                                 1 - Enables selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid pwr
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_HSREF_POWERCONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state and pwr.
    Modify adcr_control0 power down based on user provided pwr.
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvHsRefPowerConfig(HSAFE_HwIDType hwId,  uint8_t pwr)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (pwr > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       rdData = HSAFE_Regs[hwId]->adcr_control0;
       rdData &= ~HSAFE_ADC_CONTROL0_PWRDN_MASK;
       rdData |= (((pwr == 0U)? 1UL:0UL) << HSAFE_ADC_CONTROL0_PWRDN_SHIFT);
       HSAFE_Regs[hwId]->adcr_control0 = rdData;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief High speed reference ADC reset

    This API configures high speed reference ADC reset based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rstAdc       0 - Resets selected HSADC
                                 1 - out of reset selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rstAdc
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_HSREF_RESET_PROC

    @code{.unparsed}
    Validate HW index, driver state and rstAdc.
    Modify adcr_control0 resetb based on user provided rstAdc.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvHsRefReset(HSAFE_HwIDType hwId, uint8_t rstAdc)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (rstAdc > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       rdData = HSAFE_Regs[hwId]->adcr_control0;
       rdData &= ~HSAFE_ADC_CONTROL0_RESETB_MASK;
       rdData |= ((uint32_t)rstAdc << HSAFE_ADC_CONTROL0_RESETB_SHIFT);
       HSAFE_Regs[hwId]->adcr_control0 = rdData;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief High speed ADC configuration

    This API configures high speed ADC clock based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      adcInstance  HSADC instance [0-3]
    @param[in]      adcMode      0 - 5G
                                 1 - 1.25G

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcInstance
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcMode
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_HSADC_CONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state, adcInstance and adcMode.
    Update mode select, pd_5G, en_clk_5g, en_clk_1.25G, sae_CLKGE, THCLK1.25G based on adcMode.
    Based on adcInstance, write adcx_control0 register.

    @endcode

    @limitations None
*/

int32_t HSAFE_DrvHsAdcConfig(HSAFE_HwIDType hwId, uint8_t adcInstance, uint8_t adcMode)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (adcInstance >= 4U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (adcMode > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdData =  (((adcMode == 0U) ? 0UL : HSAFE_ADC_CONFIG0_ADC_MODE_SEL_MASK) |
                   ((uint32_t)adcMode << HSAFE_ADC_CONFIG0_PD_5G_SHIFT) |
                   ((adcMode == 0U) ? 0xFUL : 0UL) |
                   ((1U - adcMode) << HSAFE_ADC_CONFIG0_EN_CLK_5G_SHIFT) |
                   ((uint32_t)adcMode << HSAFE_ADC_CONFIG0_EN_CLK_1P25G_SHIFT) |
                   ((adcMode == 0U) ? (2UL << HSAFE_ADC_CONFIG0_SAR_CLKGEN_DLY_SHIFT) : (0xEUL << HSAFE_ADC_CONFIG0_SAR_CLKGEN_DLY_SHIFT)) |
                   ((uint32_t)((adcMode == 0U) ? (0UL) : (0xFUL << HSAFE_ADC_CONFIG0_EN_TH_CLK_1P25G_SHIFT)))); /* Enable All 4 channels for 1.25G*/

        switch (adcInstance) {
            case 0U:
                HSAFE_Regs[hwId]->adc0_config0 = rdData;
                break;
            case 1U:
                HSAFE_Regs[hwId]->adc1_config0 = rdData;
                break;
            case 2U:
                HSAFE_Regs[hwId]->adc2_config0 = rdData;
                break;
            case 3U:
                HSAFE_Regs[hwId]->adc3_config0 = rdData;
                break;
            default :
                ret = BCM_ERR_INVAL_PARAMS;
                break;

        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Calibration reset

    This API resets the RC calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rccalReset   1 : RCCAL under reset 0: RCCAL release reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rccalReset
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RCCAL_RESET_PROC

    @code{.unparsed}
    Validate HW index, driver state and rccalReset.
    Update rccal_control0 register based on rccalReset value.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvRccalReset(HSAFE_HwIDType hwId, uint8_t rccalReset)
{
    int ret = BCM_ERR_OK;
    uint32_t rdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (rccalReset > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdData = HSAFE_Regs[hwId]->rccal_control0 ;
        rdData &= ~HSAFE_RCCAL_CONTROL0_RCCAL_RESET_MASK;
        rdData |= rccalReset;
        HSAFE_Regs[hwId]->rccal_control0 = rdData;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Bias power configuration

    This API configures bias power.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      power        0 : Power Down Bias 1 : Power Up Bias

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid power
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RCCAL_POWERCONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state and power.
    Update afe_common0 register based on power value.
    @endcode

    @limitations None
*/

int32_t HSAFE_DrvRccalPowerConfig(HSAFE_HwIDType hwId, uint8_t power)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (power > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HSAFE_Regs[hwId]->afe_common0 = (uint32_t)((power == 0U) ? 1UL:0UL);


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief RC calibration status

    This API gets RC calibration completion state.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[out]     calibStat    1 : calibration done
                                 0: calibration not completed

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for calibStat
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RCCAL_STATUS_PROC

    @code{.unparsed}
    Validate HW index, driver state and calibStat.
    Read rccal_control0 and update calibStat with calibration status.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvRcCalStatus(HSAFE_HwIDType hwId, uint8_t *calibStat)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (calibStat == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
      *calibStat = (uint8_t)((HSAFE_Regs[hwId]->rccal_control0  & HSAFE_RCCAL_CONTROL0_RCCAL_DONE_MASK ) >>
                    HSAFE_RCCAL_CONTROL0_RCCAL_DONE_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Rescal resetB

    This API resets Resistor calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rescalReset  0 : Reset Rescal 1: bring Rescal out of reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rescalReset
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RESCAL_RESETB_PROC

    @code{.unparsed}
    Validate HW index, driver state and rescalReset.
    Modify rescal_control0 reset based on rescalReset value.
    @endcode

    @limitations None
*/

int32_t HSAFE_DrvRescalResetb(HSAFE_HwIDType hwId, uint8_t rescalReset)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (rescalReset > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdVal = HSAFE_Regs[hwId]->rescal_control0 ;
        rdVal &= ~HSAFE_RESCAL_CONTROL0_RSTB_MASK;
        rdVal |= rescalReset;
        HSAFE_Regs[hwId]->rescal_control0 = rdVal;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Rescal power configuration

    This API configures Rescal power.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rescalPower  0 : power down Rescal 1: power up Rescal

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rescalPower
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RESCAL_POWERCONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state and rescalPower.
    Modify rescal_control0 powerdown based on rescalPower value.
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvRescalPowerConfig(HSAFE_HwIDType hwId, uint8_t rescalPower)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (rescalPower > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdVal = HSAFE_Regs[hwId]->rescal_control0 ;
        rdVal &= ~HSAFE_RESCAL_CONTROL0_PWRDNB_MASK;
        rdVal |= (((rescalPower == 0U) ? 1UL:0UL) << HSAFE_RESCAL_CONTROL0_PWRDNB_SHIFT);
        HSAFE_Regs[hwId]->rescal_control0 = rdVal;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get Resistor calibration status

    This API gets Resistor calibration status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[out]     rescalStatus 1 : Res calibration completed
                                 0 : Res calibration not done
    @param[out]     valid        0 : Res calibration not valid
                                 1 : Res calibration valid
    @param[out]     value        Res calibration value output

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for rescalStatus
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for valid
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RESCAL_STATUS_PROC

    @code{.unparsed}
    Validate HW index, driver state, rescalStatus and valid.
    Read rescal_status0 and update rescalStatus and valid.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvRescalStatus(HSAFE_HwIDType hwId, uint8_t *rescalStatus, uint8_t *valid, uint8_t *value)
{
    int ret = BCM_ERR_OK;
    uint32_t rescalSts = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (rescalStatus == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (valid == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (value == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    }else {
#endif
        rescalSts = HSAFE_Regs[hwId]->rescal_status0;
        *rescalStatus = (uint8_t)(rescalSts & HSAFE_RESCAL_STATUS0_DONE_MASK);
        *valid = (uint8_t)(rescalSts & HSAFE_RESCAL_STATUS0_VALID_MASK) >> HSAFE_RESCAL_STATUS0_VALID_SHIFT;
        *value = (uint8_t) ((rescalSts & HSAFE_RESCAL_STATUS0_PON_MASK) >> HSAFE_RESCAL_STATUS0_PON_SHIFT); 

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Enables or Disables ADC Clock

    This API Enables or Disables ADC clock .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      enDis        0 - Disables ADC clock 1 - Enables ADC clock

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid enDis value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_ADC_CLKCFG_PROC

    @code{.unparsed}
    Validate HW index, driver state and enDis.
    Modify pll_control0 ADC clock based on enDis.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvAdcClkCfg(HSAFE_HwIDType hwId, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (enDis > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
      rdVal = HSAFE_Regs[hwId]->pll_control0 ;
      rdVal &= ~HSAFE_PLL_CONTROL0_EN_ADC_CLK_MASK;
      rdVal |= ((uint32_t)enDis << HSAFE_PLL_CONTROL0_EN_ADC_CLK_SHIFT);
      HSAFE_Regs[hwId]->pll_control0 = rdVal;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Enables or Disables AWG/HKDAC Clock

    This API Enables or Disables AWG/HKDAC clock .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSAFE controller
    @param[in]      clkSel        0 : 100MHz  1 : 125MHz 2 : 200MHz
                                  3 : 250MHz 4 : 156.25MHz 6 : 312.5MHz
    @param[in]      clkgenEn      0 - Disables AWG/DAC clock 1 - Enables AWG/DAC clock

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid clkSel
    @return     #BCM_ERR_INVAL_PARAMS   Invalid clkgenEn
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_AWGDAC_CLOCK_CFG_PROC

    @code{.unparsed}
    Validate HW index, driver state, clkSel, clkgenEn .
    Modify adcr_config3 with clkSel and clkgenEn.
    @endcode

    @limitations None
*/

int32_t HSAFE_DrvAwgDacClockCfg(HSAFE_HwIDType hwId, uint8_t clkSel, uint8_t clkgenEn)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (clkSel > 6U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (clkgenEn > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdVal = HSAFE_Regs[hwId]->adcr_config3 ;
        rdVal &= ~(HSAFE_ADCR_CONFIG3_DAC_CLKGEN_SEL_MASK |
                   HSAFE_ADCR_CONFIG3_DAC_CLKGEN_EN_MASK);
        rdVal |= (((uint32_t)clkSel << HSAFE_ADCR_CONFIG3_DAC_CLKGEN_SEL_SHIFT) |
                  ((uint32_t)clkgenEn << HSAFE_ADCR_CONFIG3_DAC_CLKGEN_EN_SHIFT));
        HSAFE_Regs[hwId]->adcr_config3 = rdVal;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Clockrate for OCP clock

    This API configures OCP clock rate for HSADC and HSREF.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId              Index of the HSAFE controller
    @param[in]      hsadcOcpClkSel    0 : ck16 (625Mhz) 1 : ck20 (500MHz) 2 : ck24 (416.67MHz) 3 : ck32 (312.5MHz)
    @param[in]      hsrefOcpClkSel    0 : ck32 (312.5MHz) 1 : ck24 (416.67MHz) 2 : ck20 (500MHz) 3 : no clock

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hsadcOcpClkSel
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hsrefOcpClkSel
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_OCP_CLOCK_CFG_PROC

    @code{.unparsed}
    Validate HW index, driver state,hsadcOcpClkSel and hsrefOcpClkSel .
    Modify pll_config10  vco16_to_vco32 and vco20_to_vco32 with hsadcOcpClkSel and hsrefOcpClkSel.
    @endcode

    @limitations None
*/

int32_t HSAFE_DrvOcpClockCfg(HSAFE_HwIDType hwId, uint8_t hsadcOcpClkSel, uint8_t hsrefOcpClkSel)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((hsadcOcpClkSel > 3U) || (hsrefOcpClkSel > 3U)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdVal = HSAFE_Regs[hwId]->pll_config10;
        rdVal &= ~(HSAFE_PLL_CONFIG10_SEL_VCO16_TO_VCO32_MASK | HSAFE_PLL_CONFIG10_SEL_VCO20_TO_VCO32_MASK);
        rdVal |= (((uint32_t)hsadcOcpClkSel << HSAFE_PLL_CONFIG10_SEL_VCO16_TO_VCO32_SHIFT) |
                  ((uint32_t)hsrefOcpClkSel << HSAFE_PLL_CONFIG10_SEL_VCO20_TO_VCO32_SHIFT));
        HSAFE_Regs[hwId]->pll_config10 = rdVal;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Configure Tx serializer Reset

    This API configures Tx serializer Reset .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId       Index of the HSAFE controller
    @param[in]      resetb     0: Assert TX reset 1: De-assert TX reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_TXSLZR_RESET_PROC

    @code{.unparsed}
    Validate HW index, driver state and resetb.
    Modify txafe_control0 resetb with user provided resetb.

    @endcode

    @limitations None
*/
int32_t HSAFE_DrvTxslzrReset(HSAFE_HwIDType hwId, uint8_t resetb)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (resetb > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdVal = HSAFE_Regs[hwId]->txafe_control0;
        rdVal &= ~HSAFE_TXAFE_CONTROL0_TXSLZR_VIPER_RESETB_MASK;
        rdVal |= ((uint32_t)resetb << HSAFE_TXAFE_CONTROL0_TXSLZR_VIPER_RESETB_SHIFT);
        HSAFE_Regs[hwId]->txafe_control0 = rdVal;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief Configure Tx serializer power

    This API Enables/Disables Tx serializer power configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId       Index of the HSAFE controller
    @param[in]      power      0: power Down 1: power up

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid power value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_TXSLZR_POWERCONFIG_PROC

    @code{.unparsed}
    Validate HW index, driver state and power.
    Modify txafe_control0 power down with user provided power.
    @endcode

    @limitations None
*/
int32_t HSAFE_DrvTxslzrPowerConfig(HSAFE_HwIDType hwId, uint8_t power)
{
    int ret = BCM_ERR_OK;
    uint32_t rdVal = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (power > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rdVal = HSAFE_Regs[hwId]->txafe_control0;
        rdVal &= ~HSAFE_TXAFE_CONTROL0_TXSLZR_VIPER_PD_MASK;
        rdVal |= (((power == 0U) ? 1UL:0UL) << HSAFE_TXAFE_CONTROL0_TXSLZR_VIPER_PD_SHIFT);
        HSAFE_Regs[hwId]->txafe_control0 = rdVal;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Configure Rescal value

    This API configures rescal value going to HSADC and Tx serializer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSAFE controller
    @param[in]      rescalValue   Rescal value going to HSADC and Tx serializer

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rescalValue
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @trace  #BRCM_SWARCH_HSAFE_DRV_RESCAL_CONFIG_PROC

    @limitations None
*/
int32_t HSAFE_DrvResCalConfig(HSAFE_HwIDType hwId, uint8_t rescalValue)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSAFE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HSAFE_DrvDev[hwId].state != HSAFE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (HSAFE_STATE_INITIALIZED > 0xFU) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HSAFE_Regs[hwId]->rescal_out = rescalValue;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @} */


