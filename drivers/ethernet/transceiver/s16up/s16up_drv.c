/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvr_s16up_drv_impl  SerDes PHY Core Driver Design
    @ingroup grp_transceiverdrv

    @addtogroup grp_xcvr_s16up_drv_impl
    @{

    @file drivers/ethernet/transceiver/s16up/s16up_drv.c
    @brief SerDes PHY Core Driver

    @version 0.1 Initial version
*/

#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>
#include <bcm_time.h>
#include <ethxcvr.h>
#include "./../inc/xcvr_functbl.h"

#include "./../rdb/s16up_pll_rdb.h"
#include "./../rdb/s16up_ana_pmd_blk0_rdb.h"
#include "./../rdb/s16up_ana_pmd_blk1_rdb.h"
#include "./../rdb/s16up_mdio_blk_addr_rdb.h"
#include "./../rdb/s16up_mdio_mmdsel_aer_com_rdb.h"
#include "./../rdb/s16up_rx_aeq_rdb.h"
#include "./../rdb/s16up_rx_cdr_rdb.h"
#include "./../rdb/s16up_tx_afe_rdb.h"
#include "./../rdb/s16up_txrx_actrl_rdb.h"
#include "./../rdb/s16up_u_an_x1_usxgmii_rdb.h"
#include "./../rdb/s16up_u_an_x8_usxgmii_rdb.h"
#include "./../rdb/s16up_u_pcs_main0_rdb.h"
#include "./../rdb/s16up_u_pcs_main1_rdb.h"
#include "./../rdb/s16up_u_rx_x1_control0_rdb.h"
#include "./../rdb/s16up_u_rx_x1_control1_rdb.h"
#include "./../rdb/s16up_u_rx_x1_control2_rdb.h"
#include "./../rdb/s16up_u_rx_x1_status0_rdb.h"
#include "./../rdb/s16up_user_cl22_rdb.h"
#include "./../rdb/et_rdb.h"
#include "./../rdb/hsio_eth_intr_ctrl_rdb.h"
#include "./../rdb/s16up_ana_pmd_pmdcom_blk0_rdb.h"
#include "./../rdb/s16up_ana_pmd_pmdcom_blk1_rdb.h"
#include "./../rdb/s16up_ctrl_rdb.h"
#include "./../rdb/s16up_digital_rdb.h"
#include "./../rdb/s16up_digital2_rdb.h"
#include "./../rdb/s16up_digital4_rdb.h"
#include "./../rdb/s16up_digital5_rdb.h"
#include "./../rdb/s16up_ieee_cl22_rdb.h"
#include "./../rdb/s16up_rxln_pcs_rdb.h"
#include "./../rdb/s16up_rx2ln_pcs_rdb.h"
#include "./../rdb/s16up_txln_pcs_rdb.h"
#include "./../rdb/s16up_tx2ln_pcs_rdb.h"
#include "./../rdb/s16up_rx_pcs_rdb.h"
#include "./../rdb/s16up_rx_pmd_test_rdb.h"
#include "./../rdb/s16up_tx_pmd_test_rdb.h"
#include "./../rdb/s16up_u_tx_x1_control0_rdb.h"
#include "./../rdb/s16up_u_tx_x1_control1_rdb.h"
#include "./../rdb/s16up_u_tx_x8_control0_rdb.h"
#include "./../rdb/s16up_u_rx_x8_control0_rdb.h"

/**
    @name S16UP driver Design IDs
    @{
    @brief Design IDs for BRPHY driver
*/
#define BRCM_SWDSGN_S16UP_ET_REGS_GLOBAL                   (0xC701U) /**< @brief #S16UP_ET_REGS */
#define BRCM_SWDSGN_S16UP_PLL_REGS_GLOBAL                  (0xC702U) /**< @brief #S16UP_PLL_REGS */
#define BRCM_SWDSGN_S16UP_FUNCTBL_GLOBAL                   (0xC703U) /**< @brief #S16UP_FuncTbl */
#define BRCM_SWDSGN_S16UP_RWDATA_GLOBAL                    (0xC704U) /**< @brief #S16UP_RWData */
#define BRCM_SWDSGN_S16UP_INIT_PROC                        (0xC706U) /**< @brief #S16UP_Init */
#define BRCM_SWDSGN_S16UP_RESET_PROC                       (0xC707U) /**< @brief #S16UP_Reset */
#define BRCM_SWDSGN_S16UP_GETSQIVALUE_PROC                 (0xC708U) /**< @brief #S16UP_GetSQIValue */
#define BRCM_SWDSGN_S16UP_GETDUPLEXMODE_PROC               (0xC709U) /**< @brief #S16UP_GetDuplexMode */
#define BRCM_SWDSGN_S16UP_GETSPEED_PROC                    (0xC70AU) /**< @brief #S16UP_GetSpeed */
#define BRCM_SWDSGN_S16UP_GETLINKSTATE_PROC                (0xC70BU) /**< @brief #S16UP_GetLinkState */
#define BRCM_SWDSGN_S16UP_GETMODE_PROC                     (0xC70CU) /**< @brief #S16UP_GetMode */
#define BRCM_SWDSGN_S16UP_IRQHANDLER_PROC                  (0xC70DU) /**< @brief #S16UP_IRQHandler */
#define BRCM_SWDSGN_S16UP_LINKCHANGEINDHANDLER_PROC        (0xC70EU) /**< @brief #S16UP_LinkChangeIndHandler */
#define BRCM_SWDSGN_S16UP_DEINIT_PROC                      (0xC70FU) /**< @brief #S16UP_DeInit */
#define BRCM_SWDSGN_S16UP_SETSPEED_PROC                    (0xC710U) /**< @brief #S16UP_SetSpeed */
#define BRCM_SWDSGN_S16UP_GETJUMBOMODE_PROC                (0xC711U) /**< @brief #S16UP_GetJumboMode */
#define BRCM_SWDSGN_S16UP_SETJUMBOMODE_PROC                (0xC715U) /**< @brief #S16UP_SetJumboMode */
#define BRCM_SWDSGN_S16UP_SETAUTONEGMODE_PROC              (0xC716U) /**< @brief #S16UP_SetAutoNegMode */
#define BRCM_SWDSGN_S16UP_GETAUTONEGSTATUS_PROC            (0xC717U) /**< @brief #S16UP_GetAutoNegStatus */
#define BRCM_SWDSGN_S16UP_GETSTATS_PROC                    (0xC718U) /**< @brief #S16UP_GetStats */
#define BRCM_SWDSGN_S16UP_SETDUPLEXMODE_PROC               (0xC719U) /**< @brief #S16UP_SetDuplexMode */
#define BRCM_SWDSGN_S16UP_SETFLOWCONTROL_PROC              (0xC71AU) /**< @brief #S16UP_SetFlowControl */
#define BRCM_SWDSGN_S16UP_GETFLOWCONTROL_PROC              (0xC71BU) /**< @brief #S16UP_GetFlowControl */
#define BRCM_SWDSGN_S16UP_RESTARTAUTONEG_PROC              (0xC71CU) /**< @brief #S16UP_RestartAutoNeg */
#define BRCM_SWDSGN_S16UP_STATEHANDLER_PROC                (0xC71DU) /**< @brief #S16UP_StateHandler */
#define BRCM_SWDSGN_S16UP_UPDATEHWSTATUS_PROC              (0xC71EU) /**< @brief #S16UP_UpdateHWStatus */
#define BRCM_SWDSGN_S16UP_SETMODE_PROC                     (0xC71FU) /**< @brief #S16UP_SetMode */
#define BRCM_SWDSGN_S16UP_GETMASTERMODE_PROC               (0xC720U) /**< @brief #S16UP_GetMasterMode */
#define BRCM_SWDSGN_S16UP_SETMASTERMODE_PROC               (0xC721U) /**< @brief #S16UP_SetMasterMode */
#define BRCM_SWDSGN_S16UP_GETLOOPBACKMODE_PROC             (0xC722U) /**< @brief #S16UP_GetLoopbackMode */
#define BRCM_SWDSGN_S16UP_SETLOOPBACKMODE_PROC             (0xC723U) /**< @brief #S16UP_SetLoopbackMode */
#define BRCM_SWDSGN_S16UP_TXRX_ACTRL_REGS_GLOBAL           (0xC725U) /**< @brief #S16UP_TXRX_ACTRL_REGS */
#define BRCM_SWDSGN_S16UP_PMDCOM_BLK0_REGS_GLOBAL          (0xC726U) /**< @brief #S16UP_PMDCOM_BLK0_REGS */
#define BRCM_SWDSGN_S16UP_DIGITAL_REGS_GLOBAL              (0xC727U) /**< @brief #S16UP_DIGITAL_REGS */
#define BRCM_SWDSGN_S16UP_ANA_PMD_BLK1_REGS_GLOBAL         (0xC728U) /**< @brief #S16UP_ANA_PMD_BLK1_REGS */
#define BRCM_SWDSGN_S16UP_RX_AEQ_REGS_GLOBAL               (0xC72AU) /**< @brief #S16UP_RX_AEQ_REGS */
#define BRCM_SWDSGN_S16UP_COMBO_IEEE0_REGS_GLOBAL          (0xC72BU) /**< @brief #S16UP_COMBO_IEEE0_REGS */
#define BRCM_SWDSGN_S16UP_RX_PMD_TEST_REGS_GLOBAL          (0xC72DU) /**< @brief #S16UP_RX_PMD_TEST_REGS  */
#define BRCM_SWDSGN_S16UP_RWDATA_TYPE                      (0xC72FU) /**< @brief #S16UP_RWDataType */
#define BRCM_SWDSGN_S16UP_RESET_TIMEOUT_LOOPCNT_MACRO      (0xC730U) /**< @brief #S16UP_RESET_TIMEOUT_LOOPCNT */
#define BRCM_SWDSGN_S16UP_POWER_DOWN_BIT_TIMEOUT_CNT_MACRO (0xC731U) /**< @brief #S16UP_POWER_DOWN_BIT_TIMEOUT_CNT */
#define BRCM_SWDSGN_S16UP_INT_POWER_DOWN_PROC              (0xC732U) /**< @brief #S16UP_IntPowerDown */
#define BRCM_SWDSGN_S16UP_INT_DATAPATH_SET_PROC            (0xC733U) /**< @brief #S16UP_IntDataPathSet */
#define BRCM_SWDSGN_S16UP_REF_CLK_TYPE                     (0xC734U) /**< @brief #S16UP_RefClkType */
#define BRCM_SWDSGN_S16UP_CALIB_CAP_CHARGE_TIME_MACRO      (0xC735U) /**< @brief #S16UP_CALIB_CAP_CHARGE_TIME */
#define BRCM_SWDSGN_S16UP_CALIB_DELAY_TIME_MACRO           (0xC736U) /**< @brief #S16UP_CALIB_DELAY_TIME */
#define BRCM_SWDSGN_S16UP_CALIB_STEP_TIME_MACRO            (0xC737U) /**< @brief #S16UP_CALIB_STEP_TIME */
#define BRCM_SWDSGN_S16UP_CAL_DLY_POST_VCO_MACRO           (0xC738U) /**< @brief #S16UP_CAL_DLY_POST_VCO */
#define BRCM_SWDSGN_S16UP_ANA_MDIO_PLL_FRAC_MODE_DFS_MACRO (0xC739U) /**< @brief #S16UP_ANA_MDIO_PLL_FRAC_MODE_DFS */
#define BRCM_SWDSGN_S16UP_ANA_MDIO_PLL_NDIV_INT_DFS_MACRO  (0xC73AU) /**< @brief #S16UP_ANA_MDIO_PLL_NDIV_INT_DFS */
#define BRCM_SWDSGN_S16UP_ANA_MDIO_PLL_NDIV_FRAC_DFS_MSB_BITS_MACRO (0xC73BU) /**< @brief #S16UP_ANA_MDIO_PLL_NDIV_FRAC_DFS_MSB_BITS */
#define BRCM_SWDSGN_S16UP_ANA_MDIO_PLL_NDIV_FRAC_DFS_MACRO (0xC73CU) /**< @brief #S16UP_ANA_MDIO_PLL_NDIV_FRAC_DFS */
#define BRCM_SWDSGN_S16UP_ANA_VIPER16_RX_DATA_RATE_MACRO   (0xC73DU) /**< @brief #S16UP_ANA_VIPER16_RX_DATA_RATE */
#define BRCM_SWDSGN_S16UP_ANA_VIPER16_TX_DATA_RATE_MACRO   (0xC73EU) /**< @brief #S16UP_ANA_VIPER16_TX_DATA_RATE */
#define BRCM_SWDSGN_S16UP_TXRX_ACTRL_TXBIT_63_48_MACRO     (0xC73FU) /**< @brief #S16UP_TXRX_ACTRL_TXBIT_63_48 */
#define BRCM_SWDSGN_S16UP_SW_ACTUAL_SPEED_MACRO            (0xC740U) /**< @brief #S16UP_SW_ACTUAL_SPEED */
#define BRCM_SWDSGN_S16UP_U_PCS_MAIN0_REGS_GLOBAL          (0xC741U) /**< @brief #S16UP_U_PCS_MAIN0_REGS  */
#define BRCM_SWDSGN_S16UP_U_RX_X1_CONTROL1_REGS_GLOBAL     (0xC742U) /**< @brief #S16UP_U_RX_X1_CONTROL1_REGS  */
#define BRCM_SWDSGN_S16UP_U_RX_X8_CONTROL0_REGS_GLOBAL     (0xC743U) /**< @brief #S16UP_U_RX_X8_CONTROL0_REGS  */
#define BRCM_SWDSGN_S16UP_U_TX_X1_CONTROL1_REGS_GLOBAL     (0xC744U) /**< @brief #S16UP_U_TX_X1_CONTROL1_REGS  */
#define BRCM_SWDSGN_S16UP_U_TX_X8_CONTROL0_REGS_GLOBAL     (0xC745U) /**< @brief #S16UP_U_TX_X8_CONTROL0_REGS  */
#define BRCM_SWDSGN_S16UP_TX_PCS_REG1_REGS_GLOBAL          (0xC745U) /**< @brief #S16UP_TX_PCS_REG1_REGS  */
#define BRCM_SWDSGN_S16UP_RX_PCS_REG2_REGS_GLOBAL          (0xC745U) /**< @brief #S16UP_RX_PCS_REG2_REGS  */
#define BRCM_SWDSGN_S16UP_TOGGLE_RESET_GLOBAL              (0xC704U) /**< @brief #S16UP_ToggleReset*/

/** @} */

/**
    @name S16UP_RefClkType
    @{
    @brief External PHY reference clock frequency

    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint16_t S16UP_RefClkType;
#define S16UP_REFCLK_25MHZ  (0U)  /* Reference clock frequency 25MHz */
#define S16UP_REFCLK_125MHZ  (2U)  /* Reference clock frequency 125MHz */
#define S16UP_REFclk_156p25MHz  (3U)  /* Reference clock frequency 156.25MHz */
#define S16UP_REFclk_50Mhz  (6U)  /* Reference clock frequency 50MHz */
#define S16UP_REFclk_80Mhz  (7U)  /* Reference clock frequency 80MHz */
/** @} */

/**
    @brief PLL calibration charge time.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_CALIB_CAP_CHARGE_TIME (168U)

/**
    @brief PLL calibration delay time.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_CALIB_DELAY_TIME      (84U)

/**
    @brief PLL calibration step time.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_CALIB_STEP_TIME       (340U)

/**
    @brief PLL calibration post VCO.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_CAL_DLY_POST_VCO      (22U)

/**
    @brief PLL calibration fraction mode dfs.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_ANA_MDIO_PLL_FRAC_MODE_DFS (3U)

/**
    @brief Analog MDIO PLL divider integer value.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_ANA_MDIO_PLL_NDIV_INT_DFS  (184U)

/**
    @brief Analog MDIO PLL divider fraction value MSB.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_ANA_MDIO_PLL_NDIV_FRAC_DFS_MSB_BITS (0U)

/**
    @brief Analog MDIO PLL divider fraction value dfs.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_ANA_MDIO_PLL_NDIV_FRAC_DFS (0U)

/**
    @brief Analog Rx data rate serdes.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_ANA_VIPER16_RX_DATA_RATE (0U)

/**
    @brief Analog Tx data rate serdes.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_ANA_VIPER16_TX_DATA_RATE (0U)

/**
    @brief TXRX control Tx_Bit_63_48 configuration.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_TXRX_ACTRL_TXBIT_63_48   (582U)

/**
    @brief Software configurable force speed.
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
#define S16UP_SW_ACTUAL_SPEED   (0x28U)

/**
    @brief SW reset timeout counter values.
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
*/
#define S16UP_RESET_TIMEOUT_LOOPCNT         (2000UL)

/**
    @brief Power down PHY and power up conuter values.
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
*/
#define S16UP_POWER_DOWN_BIT_TIMEOUT_CNT    (1000UL)

/**
    @brief S16UP driver data type

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
typedef struct sS16UP_RWDataType {
    ETHXCVR_StateType        state;
    ETHXCVR_ModeType         mode;                /**< @brief Transceiver administrative mode */
    ETHXCVR_LinkStateType    linkState;
    ETHXCVR_BooleanType      isConfigDone;
} S16UP_RWDataType;


/**
    @brief ET block base address

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static ET_RDBType * const S16UP_ET_REGS[ET_MAX_HW_ID] =
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
    @brief PLL block registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_PLL_RDBType * const S16UP_PLL_REGS[S16UP_PLL_MAX_HW_ID] =
{
#if (S16UP_PLL_MAX_HW_ID == 0UL)
#error "S16UP_PLL_MAX_HW_ID = 0"
#endif
    (S16UP_PLL_RDBType *const)PLL_AFE_BASE,
#if (S16UP_PLL_MAX_HW_ID > 1UL)
#error "S16UP_PLL_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief PMDCOM block0 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_ANA_PMD_PMDCOM_BLK0_RDBType * const S16UP_PMDCOM_BLK0_REGS[S16UP_ANA_PMD_PMDCOM_BLK0_MAX_HW_ID] =
{
#if (S16UP_ANA_PMD_PMDCOM_BLK0_MAX_HW_ID == 0UL)
#error "S16UP_ANA_PMD_PMDCOM_BLK0_MAX_HW_ID = 0"
#endif
    (S16UP_ANA_PMD_PMDCOM_BLK0_RDBType *const)ANA_PMD_PMDCOM_BLK0_BASE,
#if (S16UP_ANA_PMD_PMDCOM_BLK0_MAX_HW_ID > 1UL)
#error "S16UP_ANA_PMD_PMDCOM_BLK0_MAX_HW_ID > 1UL"
#endif
};


/**
    @brief Digital control registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_DIGITAL_RDBType * const S16UP_DIGITAL_REGS[S16UP_DIGITAL_MAX_HW_ID] =
{
#if (S16UP_DIGITAL_MAX_HW_ID == 0UL)
#error "S16UP_PLL_MAX_HW_ID = 0"
#endif
    (S16UP_DIGITAL_RDBType *const)DIGITAL_BASE,
#if (S16UP_DIGITAL_MAX_HW_ID > 1UL)
#error "S16UP_PLL_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief Digital control registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_DIGITAL2_RDBType * const S16UP_DIGITAL2_REGS[S16UP_DIGITAL2_MAX_HW_ID] =
{
#if (S16UP_DIGITAL2_MAX_HW_ID == 0UL)
#error "S16UP_DIGITAL2_MAX_HW_ID = 0"
#endif
    (S16UP_DIGITAL2_RDBType *const)SERDESID_BASE,
#if (S16UP_DIGITAL2_MAX_HW_ID > 1UL)
#error "S16UP_DIGITAL2_MAX_HW_ID > 1UL"
#endif
};


/**
    @brief Analog PMD block1 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_ANA_PMD_BLK1_RDBType * const S16UP_ANA_PMD_BLK1_REGS[S16UP_ANA_PMD_BLK1_MAX_HW_ID] =
{
#if (S16UP_ANA_PMD_BLK1_MAX_HW_ID == 0UL)
#error "S16UP_ANA_PMD_BLK1_MAX_HW_ID = 0"
#endif
    (S16UP_ANA_PMD_BLK1_RDBType *const)ANA_PMD_BLK1_BASE,
#if (S16UP_ANA_PMD_BLK1_MAX_HW_ID > 1UL)
#error "S16UP_ANA_PMD_BLK1_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief Analog PMD block1 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_RX_AEQ_RDBType * const S16UP_RX_AEQ_REGS[S16UP_RX_AEQ_MAX_HW_ID] =
{
#if (S16UP_RX_AEQ_MAX_HW_ID == 0UL)
#error "S16UP_RX_AEQ_MAX_HW_ID = 0"
#endif
    (S16UP_RX_AEQ_RDBType *const)RX_AEQ_AFE_BASE,
#if (S16UP_RX_AEQ_MAX_HW_ID > 1UL)
#error "S16UP_RX_AEQ_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief Combo IEEE0 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_USER_CL22_RDBType * const S16UP_COMBO_IEEE0_REGS[S16UP_USER_CL22_MAX_HW_ID] =
{
#if (S16UP_USER_CL22_MAX_HW_ID == 0UL)
#error "S16UP_USER_CL22_MAX_HW_ID = 0"
#endif
    (S16UP_USER_CL22_RDBType *const)COMBO_IEEE0_BASE,
#if (S16UP_USER_CL22_MAX_HW_ID > 1UL)
#error "S16UP_USER_CL22_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief RX PMD test registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_RX_PMD_TEST_RDBType * const S16UP_RX_PMD_TEST_REGS[S16UP_RX_PMD_TEST_MAX_HW_ID] =
{
#if (S16UP_RX_PMD_TEST_MAX_HW_ID == 0UL)
#error "S16UP_RX_PMD_TEST_MAX_HW_ID = 0"
#endif
    (S16UP_RX_PMD_TEST_RDBType *const)RX_PMD_TEST_BASE,
#if (S16UP_RX_PMD_TEST_MAX_HW_ID > 1UL)
#error "S16UP_RX_PMD_TEST_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief TXRX control registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_TXRX_ACTRL_RDBType * const S16UP_TXRX_ACTRL_REGS[S16UP_TXRX_ACTRL_MAX_HW_ID] =
{
#if (S16UP_TXRX_ACTRL_MAX_HW_ID == 0UL)
#error "S16UP_TXRX_ACTRL_MAX_HW_ID = 0"
#endif
    (S16UP_TXRX_ACTRL_RDBType *const)TXRX_ACTRL_AFE_BASE,
#if (S16UP_TXRX_ACTRL_MAX_HW_ID > 1UL)
#error "S16UP_TXRX_ACTRL_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief U PCS MAIN0 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_U_PCS_MAIN0_RDBType * const S16UP_U_PCS_MAIN0_REGS[S16UP_U_PCS_MAIN0_MAX_HW_ID] =
{
#if (S16UP_U_PCS_MAIN0_MAX_HW_ID == 0UL)
#error "S16UP_U_PCS_MAIN0_MAX_HW_ID = 0"
#endif
    (S16UP_U_PCS_MAIN0_RDBType *const)U_PCS_MAIN0_BASE,
#if (S16UP_U_PCS_MAIN0_MAX_HW_ID > 1UL)
#error "S16UP_U_PCS_MAIN0_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief U RX control1 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_U_RX_X1_CONTROL1_RDBType * const S16UP_U_RX_X1_CONTROL1_REGS[S16UP_U_RX_X1_CONTROL1_MAX_HW_ID] =
{
#if (S16UP_U_RX_X1_CONTROL1_MAX_HW_ID == 0UL)
#error "S16UP_U_RX_X1_CONTROL1_MAX_HW_ID = 0"
#endif
    (S16UP_U_RX_X1_CONTROL1_RDBType *const)U_RX_X1_CONTROL1_BASE,
#if (S16UP_U_RX_X1_CONTROL1_MAX_HW_ID > 1UL)
#error "S16UP_U_RX_X1_CONTROL1_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief U RX X8 control0 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_U_RX_X8_CONTROL0_RDBType * const S16UP_U_RX_X8_CONTROL0_REGS[S16UP_U_RX_X8_CONTROL0_MAX_HW_ID] =
{
#if (S16UP_U_RX_X8_CONTROL0_MAX_HW_ID == 0UL)
#error "S16UP_U_RX_X8_CONTROL0_MAX_HW_ID = 0"
#endif
    (S16UP_U_RX_X8_CONTROL0_RDBType *const)U_RX_X8_CONTROL0_BASE,
#if (S16UP_U_RX_X8_CONTROL0_MAX_HW_ID > 1UL)
#error "S16UP_U_RX_X8_CONTROL0_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief U TX X1 control1 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_U_TX_X1_CONTROL1_RDBType * const S16UP_U_TX_X1_CONTROL1_REGS[S16UP_U_TX_X1_CONTROL1_MAX_HW_ID] =
{
#if (S16UP_U_TX_X1_CONTROL1_MAX_HW_ID == 0UL)
#error "S16UP_U_TX_X1_CONTROL1_MAX_HW_ID = 0"
#endif
    (S16UP_U_TX_X1_CONTROL1_RDBType *const)U_TX_X1_CONTROL1_BASE,
#if (S16UP_U_TX_X1_CONTROL1_MAX_HW_ID > 1UL)
#error "S16UP_U_TX_X1_CONTROL1_MAX_HW_ID > 1UL"
#endif
};


/**
    @brief U TX X1 control0 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_U_TX_X8_CONTROL0_RDBType * const S16UP_U_TX_X8_CONTROL0_REGS[S16UP_U_TX_X8_CONTROL0_MAX_HW_ID] =
{
#if (S16UP_U_TX_X8_CONTROL0_MAX_HW_ID == 0UL)
#error "S16UP_U_TX_X8_CONTROL0_MAX_HW_ID = 0"
#endif
    (S16UP_U_TX_X8_CONTROL0_RDBType *const)U_TX_X8_CONTROL0_BASE,
#if (S16UP_U_TX_X8_CONTROL0_MAX_HW_ID > 1UL)
#error "S16UP_U_TX_X8_CONTROL0_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief TX PCS reg1 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_TXLN_PCS_RDBType * const S16UP_TX_PCS_REG1_REGS[S16UP_TXLN_PCS_MAX_HW_ID] =
{
#if (S16UP_TXLN_PCS_MAX_HW_ID == 0UL)
#error "S16UP_TXLN_PCS_MAX_HW_ID = 0"
#endif
    (S16UP_TXLN_PCS_RDBType *const)TX_PCS_REG1_BASE,
#if (S16UP_TXLN_PCS_MAX_HW_ID > 1UL)
#error "S16UP_TXLN_PCS_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief RX PCS reg2 registers

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_RX2LN_PCS_RDBType * const S16UP_RX_PCS_REG2_REGS[S16UP_RX2LN_PCS_MAX_HW_ID] =
{
#if (S16UP_RX2LN_PCS_MAX_HW_ID == 0UL)
#error "S16UP_RX2LN_PCS_MAX_HW_ID = 0"
#endif
    (S16UP_RX2LN_PCS_RDBType *const)RX_PCS_REG2_BASE,
#if (S16UP_RX2LN_PCS_MAX_HW_ID > 1UL)
#error "S16UP_RX2LN_PCS_MAX_HW_ID > 1UL"
#endif
};


/**
    @brief S16UP driver private data

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static S16UP_RWDataType S16UP_RWData[S16UP_PLL_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @code{.unparsed}
    Toggle reset
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static void S16UP_ToggleReset(uint8_t aPhyIdx) 
{
    uint32_t loopCnt = 0UL, regVal = 0UL;
    /* Toggle resetb_hw to latch afe values and start calibration */
    S16UP_ET_REGS[aPhyIdx]->top_c0_reg &= ~ET_TOP_0_CTYPE_ETH_S16U_HW_EN_MASK;
    /* Wait for reset to complete */
    do {
        regVal = S16UP_ET_REGS[aPhyIdx]->top_c0_reg;
        loopCnt++;
    } while ((S16UP_RESET_TIMEOUT_LOOPCNT > loopCnt));
    S16UP_ET_REGS[aPhyIdx]->top_c0_reg |= ET_TOP_0_CTYPE_ETH_S16U_HW_EN_MASK;

    /* Wait for out of reset to complete */
    loopCnt = 0UL;
    do {
        regVal = S16UP_ET_REGS[aPhyIdx]->top_c0_reg;
        loopCnt++;
    } while ((S16UP_RESET_TIMEOUT_LOOPCNT > loopCnt));
    (void) regVal;
}

/**
    @code{.unparsed}
    Set Speed 2500 Base-R or 2500 Base-X
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetSpeed(uint8_t aPhyIdx,
                           ETHXCVR_CntxtType *const aPhyCntxt,
                           const ETHXCVR_PortConfigType *const aConfig,
                           ETHXCVR_SpeedType aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != S16UP_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if ((ETHXCVR_SPEED_2500MBPS_X == aSpeed)) {

            /* ana_mdio_pll_ndiv_int_dfs */
            S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 &= (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPNID_MASK) & 0xFFFFU);
            S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 |= ((uint16_t)234U << S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPNID_SHIFT);

            /* Analog viper16 control registers for tx data rate and rx data rate */
            S16UP_ANA_PMD_BLK1_REGS[aPhyIdx]->viper16_ctrl0 |= (1U << S16UP_ANA_PMD_BLK1_VIPER16_CTRL0_TX_DATA_RATE_SHIFT);
            S16UP_ANA_PMD_BLK1_REGS[aPhyIdx]->viper16_ctrl0 |= (1U << S16UP_ANA_PMD_BLK1_VIPER16_CTRL0_RX_DATA_RATE_SHIFT) ;
            S16UP_ToggleReset(aPhyIdx); /*Toggle resetb*/
            /* Set force speed. */
            S16UP_DIGITAL_REGS[aPhyIdx]->misc1 &= (uint16_t)((~S16UP_DIGITAL_MISC1_FORCE_SPEED_MASK) & 0xFFFFU);
            S16UP_DIGITAL_REGS[aPhyIdx]->misc1 |= 16U;
            S16UP_DIGITAL_REGS[aPhyIdx]->control1000x1 |= S16UP_DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
            if(aConfig->pcsMode == ETHXCVR_PCS_MODE_CL127) {
                S16UP_TX_PCS_REG1_REGS[aPhyIdx]->txmisc1 = 0xC001U;
                S16UP_RX_PCS_REG2_REGS[aPhyIdx]->rx_cl127_ctrl = 0xA1BU;
            }
        } else if(ETHXCVR_SPEED_2500MBPS_R == aSpeed) {
            /* Select PCS_USXGMII */
            S16UP_ET_REGS[aPhyIdx]->s16up_c0_reg |= ET_S16UP_0_CTYPE_PCS_SEL_MASK;
            /* ana_mdio_pll_frac_mode_dfs */
            S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 &= (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPFMD_MASK) & 0xFFFFU);
            S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 |= ((uint16_t)1U << S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPFMD_SHIFT);
            S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 |= ((uint16_t)103U << S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPNID_SHIFT);
            S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->pll_ndiv_frac_dfs = ((uint16_t)32768U << S16UP_ANA_PMD_PMDCOM_BLK0_PNFD_MDIO_PLL_NDIV_FRAC_DFS_SHIFT);
            /* Analog viper16 control registers for tx data rate and rx data rate */
            S16UP_ANA_PMD_BLK1_REGS[aPhyIdx]->viper16_ctrl0 |= (1U << S16UP_ANA_PMD_BLK1_VIPER16_CTRL0_TX_DATA_RATE_SHIFT);
            S16UP_ANA_PMD_BLK1_REGS[aPhyIdx]->viper16_ctrl0 |= (1U << S16UP_ANA_PMD_BLK1_VIPER16_CTRL0_RX_DATA_RATE_SHIFT);
            S16UP_ToggleReset(aPhyIdx); /*Toggle resetb*/

        } else if(ETHXCVR_SPEED_1000MBPS == aSpeed) {
            /* This mode is for internal test pupose 1G.*/
            S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl |= S16UP_USER_CL22_MIICNTL_MANUAL_SPEED1_MASK;
            S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl &= (uint16_t)((~S16UP_USER_CL22_MIICNTL_MANUAL_SPEED0_MASK) & 0xFFFFU);
            S16UP_U_PCS_MAIN0_REGS[aPhyIdx]->setup_1 &= (uint16_t)((~S16UP_U_PCS_MAIN0_USETUP_1_SW_UBAUD_FORCE_EN_MASK) & 0xFFFFU);
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;

}

/**
    @brief  This API selects power down mode for the PHY S16UP.

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_IntPowerDown(uint8_t aPhyIdx, ETHXCVR_BooleanType powerMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t loopCnt = 0UL;
    uint16_t regVal = 0U;
    if(powerMode == ETHXCVR_BOOLEAN_FALSE) {
        regVal = S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl;
        regVal &= (uint16_t)((~S16UP_USER_CL22_MIICNTL_PWRDWN_SW_MASK) & 0xFFFFU);
        S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl = regVal;
        do {
            regVal = S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl;
            loopCnt++;
        } while (((regVal & S16UP_USER_CL22_MIICNTL_PWRDWN_SW_MASK) != 0U)
                && (S16UP_POWER_DOWN_BIT_TIMEOUT_CNT > loopCnt));

    } else {
            regVal = S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl;
            regVal |= (S16UP_USER_CL22_MIICNTL_PWRDWN_SW_MASK);
            S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl = regVal;
            do {
                regVal = S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl;
                loopCnt++;
            } while (((regVal & S16UP_USER_CL22_MIICNTL_PWRDWN_SW_MASK) == 0U)
                    && (S16UP_POWER_DOWN_BIT_TIMEOUT_CNT > loopCnt));
    }
    if (S16UP_POWER_DOWN_BIT_TIMEOUT_CNT == loopCnt) {
        retVal = BCM_ERR_TIME_OUT;
    }
    return retVal;
}

/**
    @brief This API configures data-path settings.

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_IntDataPathSet(uint8_t aPhyIdx)
{
    int32_t retVal = BCM_ERR_OK;

    /* TxRx afe control settings */
    S16UP_TXRX_ACTRL_REGS[aPhyIdx]->tx0_63_48 = S16UP_TXRX_ACTRL_TXBIT_63_48;
    /* Reset TX and RX Lane and enable */
    S16UP_U_TX_X1_CONTROL1_REGS[aPhyIdx]->misc |= S16UP_U_TX_X1_CONTROL1_UMISC_RSTB_TX_LANE_MASK;
    S16UP_U_TX_X1_CONTROL1_REGS[aPhyIdx]->misc |= S16UP_U_TX_X1_CONTROL1_UMISC_ENABLE_TX_LANE_MASK;
    S16UP_U_RX_X1_CONTROL1_REGS[aPhyIdx]->pma_control_0 |= S16UP_U_RX_X1_CONTROL1_UPMA_CONTROL_0_RSTB_LANE_MASK;
    S16UP_U_TX_X8_CONTROL0_REGS[aPhyIdx]->tx_x8_control |= S16UP_U_TX_X8_CONTROL0_UTX_X8_CONT_RSTB_TX_PORT_MASK;
    S16UP_U_RX_X8_CONTROL0_REGS[aPhyIdx]->rx_x8_control |= S16UP_U_RX_X8_CONTROL0_URX_X8_CONTROL_RSTB_RX_PORT_MASK;
    BCM_CpuNDelay(10000UL);
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
        if aMode is ETHXCVR_MODE_ACTIVE or ETHXCVR_MODE_ACTIVE_WAKEUP
            Disable low power mode (S16UP_USER_CL22_MIICNTL_PWRDWN_SW_MASK)
            Set datapath registers.
        if aMode is ETHXCVR_MODE_DOWN
            Enable power down
    if retVal is BCM_ERR_OK
        cache aMode
    return retVal
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetMode(uint8_t aPhyIdx,
                          ETHXCVR_CntxtType *const aPhyCntxt,
                          const ETHXCVR_PortConfigType *const aConfig,
                          ETHXCVR_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((S16UP_PLL_MAX_HW_ID <= aPhyIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_UNINIT != S16UP_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        if ((ETHXCVR_MODE_ACTIVE == aMode) || (ETHXCVR_MODE_ACTIVE_WAKEUP == aMode)) {
            retVal = S16UP_IntPowerDown(aPhyIdx, ETHXCVR_BOOLEAN_FALSE);
            if(retVal == BCM_ERR_OK) {
                retVal =S16UP_SetSpeed(aPhyIdx, aPhyCntxt, aConfig, aConfig->speed);
            }
            S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl |= S16UP_USER_CL22_MIICNTL_MANUAL_SPEED1_MASK;
            S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl &= (uint16_t)((~S16UP_USER_CL22_MIICNTL_MANUAL_SPEED0_MASK) & 0xFFFFU);
            if(retVal == BCM_ERR_OK) {
                retVal = S16UP_IntDataPathSet(aPhyIdx);
            }
        } else if (ETHXCVR_MODE_DOWN == aMode) {
            /* If tc10 is enabled, there is no need to power down */
            if (ETHXCVR_BOOLEAN_FALSE == aConfig->tc10Enable) {
                retVal = S16UP_IntPowerDown(aPhyIdx, ETHXCVR_BOOLEAN_TRUE);
            } else {
               retVal = BCM_ERR_NOSUPPORT;
            }
        } else if (ETHXCVR_MODE_ISOLATE == aMode) {
            retVal = BCM_ERR_NOSUPPORT;
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    if (BCM_ERR_OK == retVal) {
        if (ETHXCVR_MODE_ACTIVE_WAKEUP == aMode) {
            S16UP_RWData[aPhyIdx].mode = ETHXCVR_MODE_ACTIVE;
        } else {
            S16UP_RWData[aPhyIdx].mode = aMode;
        }
    }
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
        set *aLinkState to ETHXCVR_LINKSTATE_DOWN
       Get link sync status
       if true
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE
       else
          *aLinkState = ETHXCVR_LINKSTATE_DOWN
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetLinkState(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig,
                               ETHXCVR_LinkStateType *const aLinkState)
{
    uint32_t reg;
    int32_t  retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_BOOLEAN_FALSE == aConfig->autoNeg) {
            reg = S16UP_ET_REGS[aPhyIdx]->s16up_s0_reg;
            if ((reg & ET_S16UP_0_STYPE_SYNC_STATUS_MASK) ==
                    ET_S16UP_0_STYPE_SYNC_STATUS_MASK) {
                *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
            } else {
                *aLinkState = ETHXCVR_LINKSTATE_DOWN;
            }
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }

    }
    return retVal;
}

/**
    @code{.unparsed}
    soft reset S16UP_USER_CL22_MIICNTL_RST_HW_MASK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_Reset(uint8_t aPhyIdx,
                        ETHXCVR_CntxtType *const aPhyCntxt,
                        const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t loopCnt = S16UP_RESET_TIMEOUT_LOOPCNT;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl |= S16UP_USER_CL22_MIICNTL_RST_HW_MASK;

        /* Wait for Reset bit to auto clear */
        while (((S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl
                        & S16UP_USER_CL22_MIICNTL_RST_HW_MASK) == S16UP_USER_CL22_MIICNTL_RST_HW_MASK)
                && (loopCnt > 0UL)) {
            loopCnt--;
        }
        if (0UL == loopCnt) {
            retVal = BCM_ERR_TIME_OUT;
        }
    }
    if (BCM_ERR_OK == retVal) {
        S16UP_RWData[aPhyIdx].mode = ETHXCVR_MODE_RESET;
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    set *aMode to cached mode
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetMode(uint8_t aPhyIdx,
                          ETHXCVR_CntxtType *const aPhyCntxt,
                          const ETHXCVR_PortConfigType *const aConfig,
                          ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t mask = (S16UP_USER_CL22_MIICNTL_PWRDWN_SW_MASK | S16UP_USER_CL22_MIICNTL_RST_HW_MASK);
#ifdef BCM8915X_PARAM_VALIDATION
    if ((S16UP_PLL_MAX_HW_ID <= aPhyIdx) || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != S16UP_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if ((S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl & mask) != 0U) {
            *aMode = ETHXCVR_MODE_DOWN;
        } else {
            *aMode = ETHXCVR_MODE_ACTIVE;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if autoneg is enabled return BCM_ERR_NOSUPPORT
    if master
        set master_slave (S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK)
    else
        clear master_slave (S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK)
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetMasterMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_BooleanType aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
        retVal = BCM_ERR_NOSUPPORT;
    } else {
        if (ETHXCVR_BOOLEAN_TRUE == aMasterMode) {
            S16UP_DIGITAL_REGS[aPhyIdx]->control1000x1 |=
                ((uint16_t)aMasterMode * S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK);
        } else {
            S16UP_DIGITAL_REGS[aPhyIdx]->control1000x1 &= (uint16_t)((~S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK) & 0xFFFFU);
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS

        if master_slave is set (S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK)
            set *aMasterMode to ETHXCVR_BOOLEAN_TRUE

    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetMasterMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID<= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {

        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
            if (S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK ==
                    (S16UP_DIGITAL_REGS[aPhyIdx]->control1000x1 & S16UP_DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK)) {
                *aMasterMode = ETHXCVR_BOOLEAN_TRUE;
            } else {
                *aMasterMode = ETHXCVR_BOOLEAN_FALSE;
            }
    }
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if aMode is ETHXCVR_LOOPBACK_MODE_INTERNAL
        if autoneg is enabled, disable it
        set local_loopback (PCS Loopback)
    if aMode is ETHXCVR_LOOPBACK_MODE_REMOTE
        Set Remotel_loopback (PCS Loopback)

    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetLoopbackMode(uint8_t aPhyIdx,
                                  ETHXCVR_CntxtType *const aPhyCntxt,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LoopbackModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((S16UP_PLL_MAX_HW_ID <= aPhyIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if((aConfig->speed != ETHXCVR_SPEED_2500MBPS_X) && (aConfig->speed != ETHXCVR_SPEED_2500MBPS_R) &&
             (aConfig->speed != ETHXCVR_SPEED_2500MBPS)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if ((ETHXCVR_LOOPBACK_MODE_INTERNAL == aMode)) {
            if(aConfig->pcsMode == ETHXCVR_PCS_MODE_CL129) {
                S16UP_U_PCS_MAIN0_REGS[aPhyIdx]->loopback_control |= S16UP_U_PCS_MAIN0_ULOO_CONT_LOCAL_PCS_LOOPBACK_ENABLE_MASK;
            } else {
                S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl |= S16UP_USER_CL22_MIICNTL_GLOOPBACK_MASK;
            }
        } else if (ETHXCVR_LOOPBACK_MODE_REMOTE == aMode) {
            if(aConfig->pcsMode != ETHXCVR_PCS_MODE_CL129) {
                S16UP_RX_PMD_TEST_REGS[aPhyIdx]->control0 |= S16UP_RX_PMD_TEST_RXPMD_CONTROL0_PMD_RLOOPBACK_MASK;
                S16UP_RX_PMD_TEST_REGS[aPhyIdx]->control0 |= S16UP_RX_PMD_TEST_RXPMD_CONTROL0_RXCK_MII_GEN_SEL_FORCE_MASK;
            } else {
                S16UP_U_TX_X8_CONTROL0_REGS[aPhyIdx]->tx_x8_control |= S16UP_U_TX_X8_CONTROL0_UTX_X8_CONT_PCS_RLOOP_EN_MASK;
            }
        } else if(ETHXCVR_LOOPBACK_MODE_NONE == aMode) {
            S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl &= (uint16_t)((~S16UP_USER_CL22_MIICNTL_GLOOPBACK_MASK) & 0xFFFFU);
            S16UP_U_PCS_MAIN0_REGS[aPhyIdx]->loopback_control &= (uint16_t)((~S16UP_U_PCS_MAIN0_ULOO_CONT_LOCAL_PCS_LOOPBACK_ENABLE_MASK) & 0xFFFFU);
            S16UP_RX_PMD_TEST_REGS[aPhyIdx]->control0 &= (uint16_t)((~S16UP_RX_PMD_TEST_RXPMD_CONTROL0_PMD_RLOOPBACK_MASK) & 0xFFFFU);
            S16UP_RX_PMD_TEST_REGS[aPhyIdx]->control0 &= (uint16_t)((~S16UP_RX_PMD_TEST_RXPMD_CONTROL0_RXCK_MII_GEN_SEL_FORCE_MASK) & 0xFFFFU);
            S16UP_U_TX_X8_CONTROL0_REGS[aPhyIdx]->tx_x8_control &= (uint16_t)((~S16UP_U_TX_X8_CONTROL0_UTX_X8_CONT_PCS_RLOOP_EN_MASK) & 0xFFFFU);
            /* Reset TX and RX Lane and enable */
            S16UP_U_TX_X1_CONTROL1_REGS[aPhyIdx]->misc |= S16UP_U_TX_X1_CONTROL1_UMISC_RSTB_TX_LANE_MASK;
            S16UP_U_TX_X1_CONTROL1_REGS[aPhyIdx]->misc |= S16UP_U_TX_X1_CONTROL1_UMISC_ENABLE_TX_LANE_MASK;
            S16UP_U_RX_X1_CONTROL1_REGS[aPhyIdx]->pma_control_0 |= S16UP_U_RX_X1_CONTROL1_UPMA_CONTROL_0_RSTB_LANE_MASK;
            S16UP_U_TX_X8_CONTROL0_REGS[aPhyIdx]->tx_x8_control |= S16UP_U_TX_X8_CONTROL0_UTX_X8_CONT_RSTB_TX_PORT_MASK;
            S16UP_U_RX_X8_CONTROL0_REGS[aPhyIdx]->rx_x8_control |= S16UP_U_RX_X8_CONTROL0_URX_X8_CONTROL_RSTB_RX_PORT_MASK;
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }

    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
      if(aConfig->pcsMode == ETHXCVR_PCS_MODE_CL129) {
            if((S16UP_U_PCS_MAIN0_REGS[aPhyIdx]->loopback_control & S16UP_U_PCS_MAIN0_ULOO_CONT_LOCAL_PCS_LOOPBACK_ENABLE_MASK) != 0U) {
                *aMode = ETHXCVR_LOOPBACK_MODE_INTERNAL
            } else if((S16UP_U_TX_X8_CONTROL0_REGS[aPhyIdx]->tx_x8_control & S16UP_U_TX_X8_CONTROL0_UTX_X8_CONT_PCS_RLOOP_EN_MASK) != 0U){
                *aMode = ETHXCVR_LOOPBACK_MODE_REMOTE
             } else
             if(((S16UP_RX_PMD_TEST_REGS[aPhyIdx]->control0 & S16UP_RX_PMD_TEST_RXPMD_CONTROL0_PMD_RLOOPBACK_MASK)  != 0U)) {
                *aMode = ETHXCVR_LOOPBACK_MODE_REMOTE
             }
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetLoopbackMode(uint8_t aPhyIdx,
                                  ETHXCVR_CntxtType *const aPhyCntxt,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LoopbackModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;
    *aMode = ETHXCVR_LOOPBACK_MODE_NONE;
    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if(aConfig->pcsMode == ETHXCVR_PCS_MODE_CL129) {
            if((S16UP_U_PCS_MAIN0_REGS[aPhyIdx]->loopback_control & S16UP_U_PCS_MAIN0_ULOO_CONT_LOCAL_PCS_LOOPBACK_ENABLE_MASK) != 0U) {
                *aMode = ETHXCVR_LOOPBACK_MODE_INTERNAL;
            } else if((S16UP_U_TX_X8_CONTROL0_REGS[aPhyIdx]->tx_x8_control & S16UP_U_TX_X8_CONTROL0_UTX_X8_CONT_PCS_RLOOP_EN_MASK) != 0U){
                *aMode = ETHXCVR_LOOPBACK_MODE_REMOTE;
            } else {
               /* Do nothing */
            }
        } else {
             if(((S16UP_RX_PMD_TEST_REGS[aPhyIdx]->control0 & S16UP_RX_PMD_TEST_RXPMD_CONTROL0_PMD_RLOOPBACK_MASK)  != 0U)) {
                *aMode = ETHXCVR_LOOPBACK_MODE_REMOTE;
             }
             if((S16UP_COMBO_IEEE0_REGS[aPhyIdx]->miicntl & S16UP_USER_CL22_MIICNTL_GLOOPBACK_MASK)  != 0U) {
                 *aMode = ETHXCVR_LOOPBACK_MODE_INTERNAL;
             }

        }
    }
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
         if((reg_val & ET_S16UP_0_STYPE_SPEED_2500R_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_2500MBPS_R
        } else if((reg_val & ET_S16UP_0_STYPE_SPEED_2500X_XGMII_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_2500MBPS_X
        } else if((reg_val & ET_S16UP_0_STYPE_SPEED_2500X_GMII_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_2500MBPS
        } else if((reg_val & ET_S16UP_0_STYPE_SPEED_1000_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_1000MBPS
        } else {
            retVal = BCM_ERR_NOSUPPORT
        }
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetSpeed(uint8_t aPhyIdx,
                           ETHXCVR_CntxtType *const aPhyCntxt,
                           const ETHXCVR_PortConfigType *const aConfig,
                           ETHXCVR_SpeedType *const aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t reg_val;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        reg_val = S16UP_ET_REGS[aPhyIdx]->s16up_s0_reg;
        if((reg_val & ET_S16UP_0_STYPE_SPEED_2500R_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_2500MBPS_R;
        } else if((reg_val & ET_S16UP_0_STYPE_SPEED_2500X_XGMII_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_2500MBPS_X;
        } else if((reg_val & ET_S16UP_0_STYPE_SPEED_2500X_GMII_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_2500MBPS;
        } else if((reg_val & ET_S16UP_0_STYPE_SPEED_1000_MASK) != 0U) {
            *aSpeed = ETHXCVR_SPEED_1000MBPS;
        } else {
            retVal = BCM_ERR_NOSUPPORT;
        }

    }
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    set *aDuplexMode to duplex mode in configuration
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetDuplexMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        *aDuplexMode = aConfig->duplex;
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if aDuplexMode is not ETHXCVR_DUPLEXMODE_FULL return BCM_ERR_NOSUPPORT
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetDuplexMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (aDuplexMode != ETHXCVR_DUPLEXMODE_FULL) {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    set *aFlowControl to flow control mode in configuration
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetFlowControl(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        *aFlowControl = aConfig->flowControl;
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if aFlowControl is not ETHXCVR_FLOWCONTROL_NONE return BCM_ERR_NOSUPPORT
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetFlowControl(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (aFlowControl != ETHXCVR_FLOWCONTROL_NONE) {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetSQIValue(uint8_t aPhyIdx,
                              ETHXCVR_CntxtType *const aPhyCntxt,
                              const ETHXCVR_PortConfigType *const aConfig,
                              uint32_t *const aSQIValue)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_Init(uint8_t aPhyIdx,
                       ETHXCVR_CntxtType *const aPhyCntxt,
                       const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((S16UP_PLL_MAX_HW_ID <= aPhyIdx) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_UNINIT != S16UP_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        S16UP_RWData[aPhyIdx].state = ETHXCVR_STATE_INIT;
        S16UP_RWData[aPhyIdx].isConfigDone = ETHXCVR_BOOLEAN_FALSE;
        S16UP_RWData[aPhyIdx].mode = ETHXCVR_MODE_DOWN;
        /* Default CL36/CL127 */
        S16UP_ET_REGS[aPhyIdx]->s16up_c0_reg &= ~ET_S16UP_0_CTYPE_PCS_SEL_MASK;
        S16UP_PLL_REGS[aPhyIdx]->vco_cal_timer_1 = S16UP_CALIB_CAP_CHARGE_TIME;
        S16UP_PLL_REGS[aPhyIdx]->vco_cal_timer_2 = S16UP_CALIB_DELAY_TIME;
        S16UP_PLL_REGS[aPhyIdx]->vco_cal_timer_3 = S16UP_CALIB_STEP_TIME;
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->cal_dly_ctrl1 &= (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_CAL_DLY_CTRL1_POST_VCO_MASK) & 0xFFFFU);
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->cal_dly_ctrl1 |= ((uint16_t)S16UP_CAL_DLY_POST_VCO << S16UP_ANA_PMD_PMDCOM_BLK0_CAL_DLY_CTRL1_POST_VCO_SHIFT);
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->pll_ndiv_frac_dfs = (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_PNFD_MDIO_PLL_NDIV_FRAC_DFS_MASK) & 0xFFFFU);
        /* Frac_mode */
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 &= (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPFMD_MASK) & 0xFFFFU);
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 |= ((uint16_t)S16UP_ANA_MDIO_PLL_FRAC_MODE_DFS << S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPFMD_SHIFT);
        /* pll_ndiv_int */
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 &= (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPNID_MASK) & 0xFFFFU);
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 |= ((uint16_t)S16UP_ANA_MDIO_PLL_NDIV_INT_DFS << S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPNID_SHIFT);
        /* pll_ndiv_frac_dfs_msb_bits */
        S16UP_PMDCOM_BLK0_REGS[aPhyIdx]->ana_pmd_pmdcom_pll_misc0 &= (uint16_t)((~S16UP_ANA_PMD_PMDCOM_BLK0_APPPM0_AMPNFDMB_MASK) & 0xFFFFU);

        /* T_init, T_aeq1 and T_aeq2 */
        S16UP_RX_AEQ_REGS[aPhyIdx]->control2 &= (uint16_t)((~S16UP_RX_AEQ_AEQ_CONTROL2_T_INIT_MASK) & 0xFFFFU);
        S16UP_RX_AEQ_REGS[aPhyIdx]->control2 &= (uint16_t)((~S16UP_RX_AEQ_AEQ_CONTROL2_T_AEQ1_MASK) & 0xFFFFU);
        S16UP_RX_AEQ_REGS[aPhyIdx]->control2 &= (uint16_t)((~S16UP_RX_AEQ_AEQ_CONTROL2_T_AEQ2_MASK) & 0xFFFFU);
        /* Configure reference clock */
        S16UP_DIGITAL_REGS[aPhyIdx]->misc1 &= (uint16_t)((~S16UP_DIGITAL_MISC1_REFCLK_SEL_MASK) & 0xFFFFU);
        S16UP_DIGITAL_REGS[aPhyIdx]->misc1 &= (uint16_t)((~S16UP_DIGITAL_MISC1_FORCE_SPEED_MASK) & 0xFFFFU);
        S16UP_DIGITAL_REGS[aPhyIdx]->misc1 |= ((uint16_t)S16UP_REFclk_50Mhz << S16UP_DIGITAL_MISC1_REFCLK_SEL_SHIFT);

        S16UP_U_PCS_MAIN0_REGS[aPhyIdx]->setup_1 &= (uint16_t)((~S16UP_U_PCS_MAIN0_USETUP_1_SW_UBAUD_FORCE_EN_MASK) & 0xFFFFU);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_DeInit(uint8_t aPhyIdx,
                         ETHXCVR_CntxtType *const aPhyCntxt,
                         const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (S16UP_PLL_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != S16UP_RWData[aPhyIdx].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        S16UP_RWData[aPhyIdx].state = ETHXCVR_STATE_UNINIT;
        S16UP_RWData[aPhyIdx].isConfigDone = ETHXCVR_BOOLEAN_FALSE;
        /* Reset the S16UP block */
        S16UP_ET_REGS[aPhyIdx]->top_c0_reg &= (~ET_TOP_0_CTYPE_ETH_S16U_HW_EN_MASK);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if aMode is ETHXCVR_BOOLEAN_TRUE return BCM_ERR_NOSUPPORT
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetJumboMode(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig,
                               ETHXCVR_BooleanType aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    set *aMode to ETHXCVR_BOOLEAN_FALSE
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetJumboMode(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig,
                               ETHXCVR_BooleanType *const aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_SetAutoNegMode(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_BooleanType aAutonegMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if autoneg is disabled return BCM_ERR_INVAL_STATE
    set restart_auto_negotiation (AUTONEG_IEEE_AN_CONTROL)
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_RestartAutoNeg(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    read status register twice (AUTONEG_IEEE_BASET1_AN_STATUS)
    if autoneg_ability is not set
        set *aStatus to ETHXCVR_AUTONEGSTATUS_NO_ABILITY
    else if autoneg_complete is not set
        set *aStatus to ETHXCVR_AUTONEGSTATUS_INCOMPLETE
    else
        set *aStatus to ETHXCVR_AUTONEGSTATUS_COMPLETE

    set restart_auto_negotiation (AUTONEG_IEEE_AN_CONTROL)
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_GetAutoNegStatus(uint8_t aPhyIdx,
                                   ETHXCVR_CntxtType *const aPhyCntxt,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_AutoNegStatusType *const aStatus)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_StateHandler(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig,
                               uint32_t *const aIsModeChanged,
                               ETHXCVR_ModeType *const aMode)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_UpdateHWStatus(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
        return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_IRQHandler(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_IrqParamsType *const aParams)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_LinkChangeIndHandler(uint8_t aPhyIdx,
                                       ETHXCVR_CntxtType *const aPhyCntxt,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_LinkStateType *const aLinkState,
                                       uint32_t *const aIsLinkStateChanged)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    Read Serdes id0 register
    Get revision and model number.
    @endcode

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static int32_t S16UP_PhyGetID (ETHXCVR_IDType aID,
                                     ETHXCVR_PhyIDType *const aPhyID)
{
    int32_t   retVal = BCM_ERR_OK;
    uint32_t  id0 = 0UL;

    if (S16UP_PLL_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {

        /* Serdes ID 0 register to get revision number and model number. */
        id0 = S16UP_DIGITAL2_REGS[aID]->serdesid0;
        aPhyID->model = (id0 & S16UP_DIGITAL2_SERDESID0_MODEL_NUMBER_MASK);
        aPhyID->rev   = ((id0 & S16UP_DIGITAL2_SERDESID0_REV_NUMBER_MASK) >> S16UP_DIGITAL2_SERDESID0_REV_NUMBER_SHIFT);
    }
    return retVal;
}



/**
    @brief S16UP PHY function table
    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
const ETHXCVR_FuncTblType S16UP_FuncTble = {
    .init = S16UP_Init,
    .deinit = S16UP_DeInit,
    .reset = S16UP_Reset,
    .setMode = S16UP_SetMode,
    .getMode = S16UP_GetMode,
    .setMasterMode = S16UP_SetMasterMode,
    .getMasterMode = S16UP_GetMasterMode,
    .getSpeed = S16UP_GetSpeed,
    .setSpeed = S16UP_SetSpeed,
    .getDuplexMode = S16UP_GetDuplexMode,
    .setDuplexMode = S16UP_SetDuplexMode,
    .setFlowControl = S16UP_SetFlowControl,
    .getFlowControl = S16UP_GetFlowControl,
    .getLinkState = S16UP_GetLinkState,
    .getSQIValue = S16UP_GetSQIValue,
    .setLoopbackMode = S16UP_SetLoopbackMode,
    .getLoopbackMode = S16UP_GetLoopbackMode,
    .setJumboMode = S16UP_SetJumboMode,
    .getJumboMode = S16UP_GetJumboMode,
    .setAutoNegMode = S16UP_SetAutoNegMode,
    .getAutoNegStatus = S16UP_GetAutoNegStatus,
    .restartAutoNeg = S16UP_RestartAutoNeg,
    .stateHandler = S16UP_StateHandler,
    .updateHWStatus = S16UP_UpdateHWStatus,
    .irqHandler = S16UP_IRQHandler,
    .linkChangeIndHandler = S16UP_LinkChangeIndHandler,
    .getBaseAddr = NULL,
    .runCableDiag = NULL,
    .getID = S16UP_PhyGetID,
};


/** @} */
