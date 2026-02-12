/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xcvr_brphydrv_impl BRPHY Driver
    @ingroup grp_transceiverdrv

    @addtogroup grp_xcvr_brphydrv_v3_impl
    @{

    @file drivers/ethernet/transceiver/brphy/brphy_drv.c
    @brief This file implements BRPHY driver for bcm8915X.

    @version 0.1 Imported from docx
*/

#include <string.h>
#include <compiler.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include "macsec_bus.h"
#include "brphy1gv2_autonegieee_rdb.h"
#include "brphy1gv2_pmapmdieee_rdb.h"
#include "brphy1gv2_t1core_rdb.h"
#include "brphy1gv2_phycontrol_rdb.h"
#include "brphy1gv2_core_misc_rdb.h"
#include "brphy1gv2_pcs_ieee_rdb.h"
#include "brphy1gv2_link_sync_rdb.h"
#include "brphy1gv2_pcs_rdb.h"
#include "brphy1gv2_afe_rdb.h"
/* MSPU top */
#include <rdb/macsec_mspu_mac_rdb.h>
#include <rdb/macsec_mspu_top_rdb.h>

#include "./../inc/xcvr_functbl.h"
#include "brphy1g_platform.h"
#include <system_mcu.h>
#include "brphy1gv2_test_rdb.h"
#include "et_rdb.h"

/* F1 reference */
/* Revision 1.22  2022/04/01 01:27:10 */

/**
    @name BRPHY1G driver Design IDs
    @{
    @brief Design IDs for BRPHY driver
*/
#define BRCM_SWDSGN_BRPHY1GV2_AUTONEG_IEEE_REGS_GLOBAL    (0xC701U) /**< @brief #BRPHY1GV2_AUTONEG_IEEE_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_PMA_PMD_IEEE_REGS_GLOBAL    (0xC702U) /**< @brief #BRPHY1GV2_PMAPMD_IEEE_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_FUNCTBL_GLOBAL              (0xC703U) /**< @brief #BRPHY1GV2_FuncTbl */
#define BRCM_SWDSGN_BRPHY1GV2_RWDATA_GLOBAL               (0xC704U) /**< @brief #BRPHY1GV2_RWData */
#define BRCM_SWDSGN_BRPHY1GV2_INIT_PROC                   (0xC706U) /**< @brief #BRPHY1GV2_Init */
#define BRCM_SWDSGN_BRPHY1GV2_RESET_PROC                  (0xC707U) /**< @brief #BRPHY1GV2_Reset */
#define BRCM_SWDSGN_BRPHY1GV2_GETSQIVALUE_PROC            (0xC708U) /**< @brief #BRPHY1GV2_GetSQIValue */
#define BRCM_SWDSGN_BRPHY1GV2_GETDUPLEXMODE_PROC          (0xC709U) /**< @brief #BRPHY1GV2_GetDuplexMode */
#define BRCM_SWDSGN_BRPHY1GV2_GETSPEED_PROC               (0xC70AU) /**< @brief #BRPHY1GV2_GetSpeed */
#define BRCM_SWDSGN_BRPHY1GV2_GETLINKSTATE_PROC           (0xC70BU) /**< @brief #BRPHY1GV2_GetLinkState */
#define BRCM_SWDSGN_BRPHY1GV2_GETMODE_PROC                (0xC70CU) /**< @brief #BRPHY1GV2_GetMode */
#define BRCM_SWDSGN_BRPHY1GV2_IRQHANDLER_PROC             (0xC70DU) /**< @brief #BRPHY1GV2_IRQHandler */
#define BRCM_SWDSGN_BRPHY1GV2_LINKCHANGEINDHANDLER_PROC   (0xC70EU) /**< @brief #BRPHY1GV2_LinkChangeIndHandler */
#define BRCM_SWDSGN_BRPHY1GV2_DEINIT_PROC                 (0xC70FU) /**< @brief #BRPHY1GV2_DeInit */
#define BRCM_SWDSGN_BRPHY1GV2_SETSPEED_PROC               (0xC710U) /**< @brief #BRPHY1GV2_SetSpeed */
#define BRCM_SWDSGN_BRPHY1GV2_GETJUMBOMODE_PROC           (0xC711U) /**< @brief #BRPHY1GV2_GetJumboMode */
#define BRCM_SWDSGN_BRPHY1GV2_SETJUMBOMODE_PROC           (0xC715U) /**< @brief #BRPHY1GV2_SetJumboMode */
#define BRCM_SWDSGN_BRPHY1GV2_SETAUTONEGMODE_PROC         (0xC716U) /**< @brief #BRPHY1GV2_SetAutoNegMode */
#define BRCM_SWDSGN_BRPHY1GV2_GETAUTONEGSTATUS_PROC       (0xC717U) /**< @brief #BRPHY1GV2_GetAutoNegStatus */
#define BRCM_SWDSGN_BRPHY1GV2_GETSTATS_PROC               (0xC718U) /**< @brief #BRPHY1GV2_GetStats */
#define BRCM_SWDSGN_BRPHY1GV2_SETDUPLEXMODE_PROC          (0xC719U) /**< @brief #BRPHY1GV2_SetDuplexMode */
#define BRCM_SWDSGN_BRPHY1GV2_SETFLOWCONTROL_PROC         (0xC71AU) /**< @brief #BRPHY1GV2_SetFlowControl */
#define BRCM_SWDSGN_BRPHY1GV2_GETFLOWCONTROL_PROC         (0xC71BU) /**< @brief #BRPHY1GV2_GetFlowControl */
#define BRCM_SWDSGN_BRPHY1GV2_RESTARTAUTONEG_PROC         (0xC71CU) /**< @brief #BRPHY1GV2_RestartAutoNeg */
#define BRCM_SWDSGN_BRPHY1GV2_STATEHANDLER_PROC           (0xC71DU) /**< @brief #BRPHY1GV2_StateHandler */
#define BRCM_SWDSGN_BRPHY1GV2_UPDATEHWSTATUS_PROC         (0xC71EU) /**< @brief #BRPHY1GV2_UpdateHWStatus */
#define BRCM_SWDSGN_BRPHY1GV2_SETMODE_PROC                (0xC71FU) /**< @brief #BRPHY1GV2_SetMode */
#define BRCM_SWDSGN_BRPHY1GV2_GETMASTERMODE_PROC          (0xC720U) /**< @brief #BRPHY1GV2_GetMasterMode */
#define BRCM_SWDSGN_BRPHY1GV2_SETMASTERMODE_PROC          (0xC721U) /**< @brief #BRPHY1GV2_SetMasterMode */
#define BRCM_SWDSGN_BRPHY1GV2_GETLOOPBACKMODE_PROC        (0xC722U) /**< @brief #BRPHY1GV2_GetLoopbackMode */
#define BRCM_SWDSGN_BRPHY1GV2_SETLOOPBACKMODE_PROC        (0xC723U) /**< @brief #BRPHY1GV2_SetLoopbackMode */
#define BRCM_SWDSGN_BRPHY1GV2_T1CORE_REGS_GLOBAL          (0xC725U) /**< @brief #BRPHY1GV2_T1CORE_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_PHYCONTROL_REGS_GLOBAL      (0xC726U) /**< @brief #BRPHY1GV2_PHYCONTROL_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_CORE_MISC_REGS_GLOBAL       (0xC727U) /**< @brief #BRPHY1GV2_CORE_MISC_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_PCS_IEEE_REGS_GLOBAL        (0xC728U) /**< @brief #BRPHY1GV2_PCS_IEEE_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_LINKSYNC_REGS_GLOBAL        (0xC72AU) /**< @brief #BRPHY1GV2_LINKSYNC_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_PCS_REGS_GLOBAL             (0xC72BU) /**< @brief #BRPHY1GV2_PCS_REGS */
#define BRCM_SWDSGN_BRPHY1GV2_AFE_REGS_GLOBAL             (0xC72DU) /**< @brief #BRPHY1GV2_AFE_Regs  */
#define BRCM_SWDSGN_BRPHY1GV2_RWDATA_TYPE                 (0xC72FU) /**< @brief #BRPHY1GV2_RWDataType */
#define BRCM_SWDSGN_BRPHY1GV2_RESET_TIMEOUT_LOOPCNT_MACRO (0xC730U) /**< @brief #BRPHY1GV2_RESET_TIMEOUT_LOOPCNT */
#define BRCM_SWDSGN_BRPHY1GV2_INT_SPEED_CONFIG_SET_PROC   (0xC731U) /**< @brief #BRPHY1GV2_IntSpeedConfigSet */
#define BRCM_SWDSGN_BRPHY1GV2_RUN_CABLE_DIAG_PROC         (0xC732U) /**< @brief #BRPHY1GV2_RunCableDiag */

/** @} */


/**
    @brief BRPHY1G driver data type

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
typedef struct sBRPHY1GV2_RWDataType {
    ETHXCVR_ModeType  mode; /**< @brief Cached mode */
    ETHXCVR_SpeedType speed; /**< @brief Cached speed */
} BRPHY1GV2_RWDataType;

/**
    @brief BRPHY1G driver private data

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_RWDataType BRPHY1GV2_RWData[BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @brief ET block base address

    @trace #BRCM_SWARCH_S16UP_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_S16UP_FUNCTBL
*/
static ET_RDBType * const BRPHY1GV2_ET_REGS[ET_MAX_HW_ID] =
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
    @brief BRPHY1G AUTONEG IEEE block base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_AUTONEGIEEE_RDBType * const BRPHY1GV2_AUTONEG_IEEE_REGS[BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID] =
{
#if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_AUTONEGIEEE_RDBType *const)BRPHY0_1G_AUTONEG_IEEE_BASE,
#if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G PMA PMD IEEE block base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_PMAPMDIEEE_RDBType * const BRPHY1GV2_PMAPMD_IEEE_REGS[BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID] =
{
#if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_PMAPMDIEEE_RDBType *const)BRPHY0_1G_PMA_PMD_IEEE_BASE,
#if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID > 1UL"
#endif
};


/**
    @brief BRPHY1G PMA PMD IEEE block base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_PCS_IEEE_RDBType * const BRPHY1GV2_PCS_IEEE_REGS[BRPHY1GV2_PCS_IEEE_MAX_HW_ID] =
{
#if (BRPHY1GV2_PCS_IEEE_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_PCS_IEEE_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_PCS_IEEE_RDBType *const)BRPHY0_1G_PCS_IEEE_BASE,
#if (BRPHY1GV2_PCS_IEEE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_PCS_IEEE_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G T1 CORE block base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_T1CORE_RDBType * const BRPHY1GV2_T1CORE_REGS[BRPHY1GV2_T1CORE_MAX_HW_ID] =
{
#if (BRPHY1GV2_T1CORE_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_T1CORE_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_T1CORE_RDBType *const)BRPHY0_1G_T1_CORE_BASE,
#if (BRPHY1GV2_T1CORE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_T1CORE_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G CORE MISC block base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_CORE_MISC_RDBType * const BRPHY1GV2_CORE_MISC_REGS[BRPHY1GV2_CORE_MISC_MAX_HW_ID] =
{
#if (BRPHY1GV2_CORE_MISC_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_CORE_MISC_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_CORE_MISC_RDBType *const)BRPHY0_1G_CORE_MISC_BASE,
#if (BRPHY1GV2_CORE_MISC_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_CORE_MISC_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G PHYCONTROL block base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_PHYCONTROL_RDBType * const BRPHY1GV2_PHYCONTROL_REGS[BRPHY1GV2_PHYCONTROL_MAX_HW_ID] =
{
#if (BRPHY1GV2_PHYCONTROL_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_PHYCONTROL_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_PHYCONTROL_RDBType *const)BRPHY0_1G_PHYCONTROL_BASE,
#if (BRPHY1GV2_PHYCONTROL_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_PHYCONTROL_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G LINK SYNC base address

    @trace #BRCM_SWARCH_BRPHY1GV2_LINKSYNC_REGS_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
BRPHY1GV2_LINK_SYNC_RDBType * const BRPHY1GV2_LINKSYNC_REGS[BRPHY1GV2_LINK_SYNC_MAX_HW_ID] =
{
#if (BRPHY1GV2_LINK_SYNC_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_LINK_SYNC_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_LINK_SYNC_RDBType *const)BRPHY0_1G_LINK_SYNC_BASE,
#if (BRPHY1GV2_LINK_SYNC_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_LINK_SYNC_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G PCS base address

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_PCS_RDBType * const BRPHY1GV2_PCS_REGS[BRPHY1GV2_PCS_MAX_HW_ID] =
{
#if (BRPHY1GV2_PCS_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_PCS_MAX_HW_ID = 0"
#endif
    (BRPHY1GV2_PCS_RDBType *const)BRPHY1GV2_0_PCS_BASE,
#if (BRPHY1GV2_PCS_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_PCS_MAX_HW_ID > 1UL"
#endif
};

/**
    @brief BRPHY1G AFE base addresses

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static BRPHY1GV2_AFE_RDBType * const BRPHY1GV2_AFE_Regs[BRPHY1GV2_AFE_MAX_HW_ID] =
{
#if (BRPHY1GV2_AFE_MAX_HW_ID == 0UL)
#error "BRPHY1GV2_AFE_MAX_HW_ID == 0UL"
#endif
    (BRPHY1GV2_AFE_RDBType *const)BRPHY1GV2_0_AFE_BASE,
#if (BRPHY1GV2_AFE_MAX_HW_ID > 1UL)
#error "BRPHY1GV2_AFE_MAX_HW_ID > 1UL is not supported"
#endif
};

/**
    @brief BRPHY1G reset timeout loop counter

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
#define BRPHY1GV2_RESET_TIMEOUT_LOOPCNT (2000UL)

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
        if aMode is ETHXCVR_MODE_ACTIVE or ETHXCVR_MODE_ACTIVE_WAKEUP
            Disable low power mode (PMA_PMD_IEEE_CONTROL_REG1)
            if autonegotiation is enabled
                Advertize 1000Base-T1 and 100Base-T1 (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG1)
                Set selector field (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG0)
                Enable and restart autoneg (AUTONEG_IEEE_AN_CONTROL)
            if autonegotiation is disabled
                Reset advertizement (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG1)
                Set forced master/slave and speed (PMA_PMD_IEEE_BASET1_PMA_PMD_CONTROL)
                Disable and restart autoneg (AUTONEG_IEEE_AN_CONTROL)
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static void BRPHY1GV2_IntSpeedConfigSet(uint8_t aPhyIdx, const ETHXCVR_PortConfigType *const aConfig)
{
    uint16_t reg0Val = 0U;
    uint16_t expReg0Val = 0U;
    uint16_t reg1Val = 0U;
    uint16_t expReg1Val = 0U;
    uint16_t ctlVal = 0U;
    uint16_t expCtlVal = 0U;
    uint16_t m2100Val = 0U;
    uint16_t expM2100Val = 0U;
    uint16_t m0Val = 0U;
    uint16_t expM0Val = 0U;
    uint16_t anCtlVal = 0U;
    if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
        reg0Val = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg0;
        expReg0Val = (BRPHY1GV2_AUTONEGIEEE_ADVREG0_SELECTOR_VAL &
             BRPHY1GV2_AUTONEGIEEE_B1AAR0_SELECTOR_FIELD_MASK);
        reg1Val = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1;
        expReg1Val = (aConfig->phy.masterMode ? BRPHY1GV2_AUTONEGIEEE_B1AAR1_MASTER_SLAVE_MASK : 0U);
        if (0U != (aConfig->speed & ETHXCVR_SPEED_100MBPS)) {
            expReg1Val |= BRPHY1GV2_AUTONEGIEEE_B1AAR1_ADVERISE_100BASE_T1_MASK;
        }
        if (0U != (aConfig->speed & ETHXCVR_SPEED_1000MBPS)) {
            expReg1Val |= BRPHY1GV2_AUTONEGIEEE_B1AAR1_ADVERISE_1000BASE_T1_MASK;
        }
        ctlVal = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control;
        expCtlVal = BRPHY1GV2_AUTONEGIEEE_AB1AC_AUTONEG_ENABLE_MASK;

        /* Configure and restart autoneg only if the configuration required is different from the one already present */
        if ((reg0Val != expReg0Val)
            || (reg1Val != expReg1Val)
            || (ctlVal != expCtlVal)) {

            /* adv 1000T1 and 100T1 */
            BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1 = expReg1Val;
            /* push changes to REG1 (per std) */
            BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg0 = expReg0Val;
            /* enable autoneg & restart */
            BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control = expCtlVal | BRPHY1GV2_AUTONEGIEEE_AB1AC_RESTART_AUTO_NEGOTIATION_MASK;
        }
    } else {
        BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1 = 0U;

        m2100Val = BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m2100;
        expM2100Val = (uint16_t)( ((ETHXCVR_BOOLEAN_TRUE == aConfig->phy.masterMode) ?
                                       BRPHY1GV2_PMAPMDIEEE_B1PPC_MASTER_SLAVE_MASK  :  0x0U)  |
                                       BRPHY1GV2_PMAPMDIEEE_B1PPC_BASET1_PMA_PMD_CONTROL_BIT_15_MASK );
        if (ETHXCVR_SPEED_1000MBPS == aConfig->speed) {
            expM2100Val |= ((BRPHY1GV2_PMAPMD_BASET1_CTRL_T1_BASE_SEL_1000 <<
                        BRPHY1GV2_PMAPMDIEEE_B1PPC_T1_BASE_SEL_SHIFT) &
                    BRPHY1GV2_PMAPMDIEEE_B1PPC_T1_BASE_SEL_MASK);
        }

        m0Val = BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0;
        if (ETHXCVR_SPEED_1000MBPS == aConfig->speed) {
            expM0Val = BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_1_MASK;
        } else {
            expM0Val = BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_0_MASK;
        }
        anCtlVal = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control;
        if ((expM0Val != m0Val) || (expM2100Val != m2100Val)
             || (0U != (anCtlVal & BRPHY1GV2_AUTONEGIEEE_AB1AC_AUTONEG_ENABLE_MASK))) {
            BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m2100 = expM2100Val;
            BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 = expM0Val;

            /* disable autoneg: restart autoneg is still required to restart link_sync statemachine */
            BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control = BRPHY1GV2_AUTONEGIEEE_AB1AC_RESTART_AUTO_NEGOTIATION_MASK;
        }
    }
}

/**
    @code{.unparsed}
        Config  BRPHY1GV2_PHYCONTROL_REGS / SUPER_ISOLATE_REG
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetModeConfigPhyControl(uint8_t aPhyIdx, uint16_t field)
{
    int32_t retVal = BCM_ERR_OK;

    if ( aPhyIdx < BRPHY1GV2_PHYCONTROL_MAX_HW_ID ) {
        if ( field == BRPHY1GV2_SQI_CONST0 ) {
            BRPHY1GV2_PHYCONTROL_RDBType *phyCtrlReg =  BRPHY1GV2_PHYCONTROL_REGS[aPhyIdx];
            /* TC1 DCQ SQI threasholds */
            phyCtrlReg->tc1_dcq_sqi_threshold_0     =  BRPHY1GV2_SQI_CONST0;  /* OFFSET 0x14c */
            phyCtrlReg->tc1_dcq_sqi_threshold_1     =  BRPHY1GV2_SQI_CONST1;  /* OFFSET 0x150 */
            phyCtrlReg->tc1_dcq_sqi_threshold_2     =  BRPHY1GV2_SQI_CONST2;  /* OFFSET 0x154 */
            phyCtrlReg->tc1_dcq_sqi_threshold_3     =  BRPHY1GV2_SQI_CONST3;  /* OFFSET 0x158 */
            phyCtrlReg->tc1_dcq_sqi_threshold_4     =  BRPHY1GV2_SQI_CONST4;  /* OFFSET 0x15c */
            phyCtrlReg->tc1_dcq_sqi_threshold_5     =  BRPHY1GV2_SQI_CONST5;  /* OFFSET 0x160 */
            phyCtrlReg->tc1_dcq_sqi_threshold_6     =  BRPHY1GV2_SQI_CONST6;  /* OFFSET 0x164 */
        } else
        if ( field == BRPHY1GV2_PHYC_PATCH_CONST1 ) {
            BRPHY1GV2_PHYCONTROL_RDBType *phyCtrlReg =  BRPHY1GV2_PHYCONTROL_REGS[aPhyIdx];
            /* State E0 branch controls  */
            phyCtrlReg->state_e0_branch_ctl         =  BRPHY1GV2_PHYC_PATCH_CONST1;  /* OFFSET 0x400 */
            phyCtrlReg->state_e0_branch_0_cond_ctl  =  BRPHY1GV2_PHYC_PATCH_CONST2;  /* OFFSET 0x404 */
            phyCtrlReg->state_e0_branch_1_cond_ctl  =  BRPHY1GV2_PHYC_PATCH_CONST3;  /* OFFSET 0x408 */
            /* State E0 outputs        */
            phyCtrlReg->state_e0_output_0           =  BRPHY1GV2_PHYC_PATCH_CONST4;  /* OFFSET 0x40c */
            phyCtrlReg->state_e0_output_1           =  BRPHY1GV2_PHYC_PATCH_CONST5;  /* OFFSET 0x410 */
            phyCtrlReg->state_e0_output_2           =  BRPHY1GV2_PHYC_PATCH_CONST6;  /* OFFSET 0x414 */
            phyCtrlReg->state_e0_output_3           =  BRPHY1GV2_PHYC_PATCH_CONST7;  /* OFFSET 0x418 */
            phyCtrlReg->state_e0_output_4           =  BRPHY1GV2_PHYC_PATCH_CONST8;  /* OFFSET 0x41c */
            /* Patch state control     */
            phyCtrlReg->patch_state_ctl_0           =  BRPHY1GV2_PHYC_PATCH_CONST9;  /* OFFSET 0x0c0 */
        } else
        if ( field == BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK ) {
            BRPHY1GV2_CORE_MISC_RDBType *phyCtrlReg =  BRPHY1GV2_CORE_MISC_REGS[aPhyIdx];
            /* Disable SuperIsolate  */
            phyCtrlReg->shd18_010 &= (uint16_t)~field;
        }
        else {
            retVal = BCM_ERR_INVAL_PARAMS;  /* invalid field name */
        }
    }

    return (retVal);
}

/**
    @code{.unparsed}
        ETHXCVR_Mode is in  ETHXCVR_MODE_ACTIVE or ETHXCVR_MODE_ACTIVE_WAKEUP  mode :
            Disable low power mode (PMA_PMD_IEEE_CONTROL_REG1)
            if autonegotiation is enabled
                Advertize 1000Base-T1 and 100Base-T1 (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG1)
                Set selector field (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG0)
                Enable and restart autoneg (AUTONEG_IEEE_AN_CONTROL)
            if autonegotiation is disabled
                Reset advertizement (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG1)
                Set forced master/slave and speed (PMA_PMD_IEEE_BASET1_PMA_PMD_CONTROL)
                Disable and restart autoneg (AUTONEG_IEEE_AN_CONTROL)
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetModeActiveWakeup(MCU_VersionExtType *mcuVerExt, uint8_t aPhyIdx,
                                                  const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t temp = 0U;

    if ( ETHXCVR_MODE_ACTIVE != BRPHY1GV2_RWData[aPhyIdx].mode ) {
        BCM_BoolType  mcuIs8915x8958xA0 = (BCM_BoolType) ( (MCU_CHIP_FAMILY_8915X == mcuVerExt->family) ||
                                                          ((MCU_CHIP_FAMILY_8958X == mcuVerExt->family) &&
                                                           (MCU_CHIP_REVISION_A0  == mcuVerExt->rev   ) &&
                                                           (MCU_CHIP_MODEL_89581  != mcuVerExt->model )) );
        temp = BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_LOW_POWER_MASK ;
        BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 &= (uint16_t)~temp;
        /* Disable super isolate on bcm8957x A0 */
        if ( (MCU_CHIP_FAMILY_8957X == mcuVerExt->family) && (MCU_CHIP_REVISION_A0  == mcuVerExt->rev) ) {
            retVal = BRPHY1GV2_SetModeConfigPhyControl(aPhyIdx, BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK);
        }
      #if (ENABLE_BRPHY1G_AFE_CONFIG == 1UL)
        /* BRPHY1GV2_PlatformAfeConfig(aPhyIdx, aConfig, aMode); */
      #endif
        BRPHY1GV2_IntSpeedConfigSet(aPhyIdx, aConfig);
        /* Bypass AN Digital HPF */
        BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_test_reg4 = BRPHY1GV2_AUTONEGIEEE_HPF_VAL;

        /* Fast link down mode == Not yet supported by SDK */

        /* SQI threshold Tables */
        temp = BRPHY1GV2_T1CORE_TCOV_DISABLE_DSP_RDB_CLOCK_MASK ;
        BRPHY1GV2_T1CORE_REGS[aPhyIdx]->tx_clock_override_value &= (uint16_t)~temp;
        if ( (MCU_CHIP_FAMILY_8957X == mcuVerExt->family) || (BCM_BOOL_TRUE == mcuIs8915x8958xA0) ) {
            /* For interop with Competition */
            BRPHY1GV2_LINKSYNC_REGS[aPhyIdx]->control_b = BRPHY1GV2_LINK_SYNC_CONST;
        }

        /* TC10 LPS Interoperability fixes */
        BRPHY1GV2_PCS_REGS[aPhyIdx]->exp93 = BRPHY1GV2_PCS_EXP93_TX_LPS_WUR_OPT_MASK;

        /* This check is added to fix the issue where upon doing set mode down, */
        /* Switch port is not linking down */
        /* FYI: 57X A0 doesn't have this issue and Address of PLL_CTRL in RDB is different in A0 and B0*/
        if ( ! ((MCU_CHIP_FAMILY_8957X == mcuVerExt->family) && (MCU_CHIP_REVISION_A0 == mcuVerExt->rev)) ) {  /* other than 8957x_A0 */
            BRPHY1GV2_AFE_Regs[aPhyIdx]->pll_ctrl |= (BRPHY1GV2_AFE_AFE_PLL_CTRL_PLL_POWER_ON_MASK | BRPHY1GV2_AFE_AFE_PLL_CTRL_PLL_POWER_ON_OV_MASK);
            retVal = BRPHY1GV2_SetModeConfigPhyControl(aPhyIdx, BRPHY1GV2_SQI_CONST0);
        }
        /* Valid for:
         * BCM8957X - B0 and B1 and not A0,
         * BCM8958X - A0
         * PHYC patch code to fix SEND_IDLE1 bug in 1000BT1
        */
        if ( ((MCU_CHIP_FAMILY_8957X == mcuVerExt->family) && (MCU_CHIP_REVISION_A0 != mcuVerExt->rev)) ||
                                        (BCM_BOOL_TRUE == mcuIs8915x8958xA0) ) {    /* 8957x (not A0) / 8915x / 8958x_A0 */
            retVal = BRPHY1GV2_SetModeConfigPhyControl(aPhyIdx, BRPHY1GV2_PHYC_PATCH_CONST1);
        }

        /* Configuration is complete disable super isolate */
        if( MCU_CHIP_FAMILY_8915X == mcuVerExt->family ) {
            retVal = BRPHY1GV2_SetModeConfigPhyControl(aPhyIdx, BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK);
            BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c2_reg &= (uint32_t)~ET_BRPHY_2_CTYPE_SUPER_ISOLATE_DEF_MASK;
        }
    }

    return (retVal);
}

/**
    @code{.unparsed}
        ETHXCVR_Mode is  ETHXCVR_MODE_DOWN  mode :
            Enable low power mode (PMA_PMD_IEEE_CONTROL_REG1)
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetModeDown(MCU_VersionExtType *mcuVerExt,uint8_t aPhyIdx,
                                        const ETHXCVR_PortConfigType *const aConfig)
{
    if ( ETHXCVR_BOOLEAN_FALSE == aConfig->tc10Enable ) {  /* TC10 functionality is disabled */
        /* Enable super isolate on bcm8957x A0 */
        if ( (MCU_CHIP_FAMILY_8957X == mcuVerExt->family) && (MCU_CHIP_REVISION_A0  == mcuVerExt->rev) ) {
            /* 8957X A0 chips */
            BRPHY1GV2_CORE_MISC_REGS[aPhyIdx]->shd18_010 |= BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK;
        }
        /* Set Low Power mode  (PMA/PMD IEEE Control Register 1) */
        BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 |= BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_LOW_POWER_MASK;
    }
    return BCM_ERR_OK;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
        if aMode is ETHXCVR_MODE_ACTIVE or ETHXCVR_MODE_ACTIVE_WAKEUP
            Disable low power mode (PMA_PMD_IEEE_CONTROL_REG1)
            if autonegotiation is enabled
                Advertize 1000Base-T1 and 100Base-T1 (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG1)
                Set selector field (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG0)
                Enable and restart autoneg (AUTONEG_IEEE_AN_CONTROL)
            if autonegotiation is disabled
                Reset advertizement (AUTONEG_IEEE_BASET1_AN_ADVERTISEMENT_REG1)
                Set forced master/slave and speed (PMA_PMD_IEEE_BASET1_PMA_PMD_CONTROL)
                Disable and restart autoneg (AUTONEG_IEEE_AN_CONTROL)
        else if aMode is ETHXCVR_MODE_DOWN
            Enable low power mode (PMA_PMD_IEEE_CONTROL_REG1)
        else if aMode is ETHXCVR_MODE_ISOLATE
            return BCM_ERR_NOSUPPORT
        else
            return BCM_ERR_INVAL_PARAMS
    if retVal is BCM_ERR_OK
        cache aMode
    return retVal
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetMode(uint8_t aPhyIdx, ETHXCVR_CntxtType      *const aPhyCntxt,
                                            const ETHXCVR_PortConfigType *const aConfig,
                                                  ETHXCVR_ModeType              aMode    )
{
    int32_t retVal = BCM_ERR_OK;
    MCU_VersionExtType mcuVerExt = {0};

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        MCU_GetVersionExt(&mcuVerExt);  /* Retrieves the MCU version along with the chip family info */

        if ( (ETHXCVR_MODE_ACTIVE == aMode) || (ETHXCVR_MODE_ACTIVE_WAKEUP == aMode) ) { /* Active or WakeUp mode */
            retVal = BRPHY1GV2_SetModeActiveWakeup(&mcuVerExt, aPhyIdx, aConfig);  /* set Active or WakeUp mode */
        } else
        if (  ETHXCVR_MODE_DOWN   == aMode ) {  /* ETHXCVR_Mode is in  ETHXCVR_MODE_DOWN   mode */
            retVal = BRPHY1GV2_SetModeDown(&mcuVerExt, aPhyIdx, aConfig);  /* set to Down mode */
        } else
        if ( ETHXCVR_MODE_ISOLATE == aMode ) {  /* ETHXCVR_Mode is in ETHXCVR_MODE_ISOLATE mode */
            BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c2_reg     |= ET_BRPHY_2_CTYPE_SUPER_ISOLATE_DEF_MASK;
            BRPHY1GV2_CORE_MISC_REGS[aPhyIdx]->shd18_010 |= BRPHY1GV2_CORE_MISC_SHD18_010_SUPER_ISOLATE_MASK;
        }
        else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    }

    /* Cache the mode */
    if ( BCM_ERR_OK == retVal ) {
        if ( ETHXCVR_MODE_ACTIVE_WAKEUP == aMode ) {
            BRPHY1GV2_RWData[aPhyIdx].mode = ETHXCVR_MODE_ACTIVE;
        } else {
            BRPHY1GV2_RWData[aPhyIdx].mode = aMode;
        }
        BRPHY1GV2_RWData[aPhyIdx].speed = aConfig->speed;
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if local_loopback is set (PMA_PMD_IEEE_CONTROL_REG1)
        set *aMode to ETHXCVR_BOOLEAN_TRUE
    else
        set *aMode to ETHXCVR_BOOLEAN_FALSE
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetLoopbackMode(uint8_t aPhyIdx,
                                  ETHXCVR_CntxtType *const aPhyCntxt,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LoopbackModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (0U != (aConfig->speed & ETHXCVR_SPEED_1000MBPS)) {
            if (0U != (BRPHY1GV2_T1CORE_REGS[aPhyIdx]->testability_reg_c &
                       BRPHY1GV2_T1CORE_TRC_INTERNAL_LOOPBACK_MASK)) {
                *aMode = ETHXCVR_LOOPBACK_MODE_INTERNAL;
            } else {
                *aMode = ETHXCVR_LOOPBACK_MODE_NONE;
            }
        } else {
            if (0U != (BRPHY1GV2_PCS_IEEE_REGS[aPhyIdx]->reg_3_0 &
                       BRPHY1GV2_PCS_IEEE_PCS_CONTROL_REG1_LOOPBACK_MASK)) {
                *aMode = ETHXCVR_LOOPBACK_MODE_INTERNAL;
            } else {
                *aMode = ETHXCVR_LOOPBACK_MODE_NONE;
            }
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if autoneg is enabled
        if highest_common_denominator field (AUTONEG_IEEE_TEST_REG2) is 100T1 or 100T1-legacy
            set *aSpeed to ETHXCVR_SPEED_100MBPS
        else
            set *aSpeed to ETHXCVR_SPEED_1000MBPS
    if autoneg is disabled
        if speed_sel_1 is set (PMA_PMD_IEEE_CONTROL_REG1)
            set *aSpeed to ETHXCVR_SPEED_1000MBPS
        else if speed_sel_1 is set (PMA_PMD_IEEE_CONTROL_REG1)
            set *aSpeed to ETHXCVR_SPEED_100MBPS
        else return BCM_ERR_UNKNOWN
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetSpeed(uint8_t aPhyIdx,
                           ETHXCVR_CntxtType *const aPhyCntxt,
                           const ETHXCVR_PortConfigType *const aConfig,
                           ETHXCVR_SpeedType *const aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    ETHXCVR_BooleanType lbMode = ETHXCVR_BOOLEAN_FALSE;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BRPHY1GV2_GetLoopbackMode(aPhyIdx, aPhyCntxt, aConfig, &lbMode);
        if (BCM_ERR_OK == retVal) {
            if ((ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) && (ETHXCVR_BOOLEAN_FALSE == lbMode)) {
                switch (BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_test_reg2 &
                        BRPHY1GV2_AUTONEGIEEE_ATR2_HIGHEST_COMMON_DENOMINATOR_MASK) {
                    case BRPHY1GV2_AUTONEGIEEE_TESTREG2_HCD_LEGACY_100BASET1:
                    case BRPHY1GV2_AUTONEGIEEE_TESTREG2_HCD_100BASET1:
                        /* Fall through intentional */
                        *aSpeed = ETHXCVR_SPEED_100MBPS;
                        break;
                    default:
                        *aSpeed = ETHXCVR_SPEED_1000MBPS;
                        break;
                }
            } else {
                uint16_t regVal = BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0;
                if (BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_1_MASK ==
                    (BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_1_MASK & regVal)) {
                    *aSpeed = ETHXCVR_SPEED_1000MBPS;
                } else if (BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_0_MASK ==
                          (BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_0_MASK & regVal)) {
                    *aSpeed = ETHXCVR_SPEED_100MBPS;
                } else {
                    retVal = BCM_ERR_UNKNOWN;
                }
            }
        }
    }

    return retVal;
}


/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
        set *aLinkState to ETHXCVR_LINKSTATE_DOWN
        if autonegotiation is enabled
            Read status register twice (AUTONEG_IEEE_BASET1_AN_STATUS)
            if link_status field is set
                set *aLinkState to ETHXCVR_LINKSTATE_ACTIVE
        if autonegotiation is disabled
            Read status register twice (PMA_PMD_IEEE_STATUS_REG1)
            if link_status field is set
                set *aLinkState to ETHXCVR_LINKSTATE_ACTIVE
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetLinkState(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig,
                               ETHXCVR_LinkStateType *const aLinkState)
{
    uint16_t reg;
    int32_t  retVal;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
            reg = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_status;
            reg = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_status;

            if ((reg & BRPHY1GV2_AUTONEGIEEE_AB1AS_LINK_STATUS_MASK) ==
                    BRPHY1GV2_AUTONEGIEEE_AB1AS_LINK_STATUS_MASK) {
                *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
            } else {
                *aLinkState = ETHXCVR_LINKSTATE_DOWN;
            }
        } else {
            reg = BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m1;
            reg = BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m1;

            if ((reg & BRPHY1GV2_PMAPMDIEEE_STATUS_REG1_RECEIVE_LINK_STATUS_MASK) ==
                    BRPHY1GV2_PMAPMDIEEE_STATUS_REG1_RECEIVE_LINK_STATUS_MASK) {
                *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
            } else {
                *aLinkState = ETHXCVR_LINKSTATE_DOWN;
            }
        }
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_Reset(uint8_t aPhyIdx,
                        ETHXCVR_CntxtType *const aPhyCntxt,
                        const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t loopCnt = BRPHY1GV2_RESET_TIMEOUT_LOOPCNT;

    if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 |= BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_RESET_MASK;
        /* Sleeping for 10 us as accessing immidiately leading to a crash */
        BCM_CpuNDelay(10000UL);
        /* Wait for Reset bit to auto clear */
        while (((BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0
                        & BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_RESET_MASK) == BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_RESET_MASK)
                && (loopCnt > 0UL)) {
            loopCnt--;
        }
        if (0UL == loopCnt) {
            retVal = BCM_ERR_TIME_OUT;
        }
    }
    if (BCM_ERR_OK == retVal) {
        BRPHY1GV2_RWData[aPhyIdx].mode = ETHXCVR_MODE_RESET;
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    set *aMode to cached mode
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetMode(uint8_t aPhyIdx,
                          ETHXCVR_CntxtType *const aPhyCntxt,
                          const ETHXCVR_PortConfigType *const aConfig,
                          ETHXCVR_ModeType *const aMode)
{
    int32_t retVal;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if((BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 & BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_LOW_POWER_MASK) != 0U) {
            *aMode = ETHXCVR_MODE_DOWN;
        } else {
            *aMode = ETHXCVR_MODE_ACTIVE;
        }
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if autoneg is enabled return BCM_ERR_NOSUPPORT
    if master
        set master_slave (PMA_PMD_IEEE_BASET1_PMA_PMD_CONTROL)
    else
        clear master_slave (PMA_PMD_IEEE_BASET1_PMA_PMD_CONTROL)
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetMasterMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_BooleanType aMasterMode)
{
    int32_t  retVal = BCM_ERR_OK;
    uint16_t temp   = 0U ;

    if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
        retVal = BCM_ERR_NOSUPPORT;
    } else {
        temp = BRPHY1GV2_PMAPMDIEEE_B1PPC_MASTER_SLAVE_MASK ;
        if (ETHXCVR_BOOLEAN_TRUE == aMasterMode) {
            BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m2100 |= (uint16_t)temp;
        } else {
            BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m2100 &= (uint16_t)~temp;
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if autoneg is enabled
        if master_slave is set (AUTONEG_IEEE_TEST_REG2)
            set *aMasterMode to ETHXCVR_BOOLEAN_TRUE
        else
            set *aMasterMode to ETHXCVR_BOOLEAN_FALSE
    if autoneg is disabled
        if master_slave is set (PMA_PMD_IEEE_BASET1_PMA_PMD_CONTROL)
            set *aMasterMode to ETHXCVR_BOOLEAN_TRUE
        else
            set *aMasterMode to ETHXCVR_BOOLEAN_FALSE
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetMasterMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_BooleanType *const aMasterMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID<= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
            if (BRPHY1GV2_AUTONEGIEEE_ATR2_MASTER_SLAVE_MASK ==
                (BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_test_reg2 & BRPHY1GV2_AUTONEGIEEE_ATR2_MASTER_SLAVE_MASK)) {
                *aMasterMode = ETHXCVR_BOOLEAN_TRUE;
            } else {
                *aMasterMode = ETHXCVR_BOOLEAN_FALSE;
            }
        } else {
            if (BRPHY1GV2_PMAPMDIEEE_B1PPC_MASTER_SLAVE_MASK ==
                    (BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m2100 & BRPHY1GV2_PMAPMDIEEE_B1PPC_MASTER_SLAVE_MASK)) {
                *aMasterMode = ETHXCVR_BOOLEAN_TRUE;
            } else {
                *aMasterMode = ETHXCVR_BOOLEAN_FALSE;
            }
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetSpeed(uint8_t aPhyIdx,
                           ETHXCVR_CntxtType *const aPhyCntxt,
                           const ETHXCVR_PortConfigType *const aConfig,
                           ETHXCVR_SpeedType aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    ETHXCVR_PortConfigType aConfig1;
    BCM_MemCpy(&aConfig1, aConfig, sizeof(ETHXCVR_PortConfigType));
    if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        aConfig1.speed = aSpeed;
        BRPHY1GV2_IntSpeedConfigSet(aPhyIdx, &aConfig1);
        /* Set the unimac speed */
        retVal = MACSEC_BusDrvUnimacSpeedSet(aPhyIdx, aSpeed);
        BRPHY1GV2_RWData[aPhyIdx].speed = aSpeed;
    }
    return retVal;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if aMode is ETHXCVR_LOOPBACK_MODE_INTERNAL
        if autoneg is enabled, disable it
        set local_loopback (PCS Loopback)
    else if aMode is ETHXCVR_LOOPBACK_MODE_NONE
        clear local_loopback (PCS Loopback)
        if autoneg should be enabled, re-enable it
    else
        return BCM_ERR_NOSUPPORT ;
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetLoopbackMode(uint8_t aPhyIdx,
                                  ETHXCVR_CntxtType *const aPhyCntxt,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LoopbackModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t temp = 0U;

    if (BRPHY1GV2_PMAPMDIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (aMode == ETHXCVR_LOOPBACK_MODE_INTERNAL) {
            /* When trying to enable int.lpbk, the AUTONEG must be turned off. */
            if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
                BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1 = 0U;
                BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m2100 = BRPHY1GV2_PMAPMDIEEE_B1PPC_BASET1_PMA_PMD_CONTROL_BIT_15_MASK;
                if (0U != (aConfig->speed & ETHXCVR_SPEED_1000MBPS)) {
                    BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 = BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_1_MASK;
                } else {
                    BRPHY1GV2_PMAPMD_IEEE_REGS[aPhyIdx]->m0 = BRPHY1GV2_PMAPMDIEEE_CONTROL_REG1_SPEED_SEL_0_MASK;
                }
                BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control =
                    BRPHY1GV2_AUTONEGIEEE_AB1AC_RESTART_AUTO_NEGOTIATION_MASK;
            }
            if (0U != (aConfig->speed & ETHXCVR_SPEED_1000MBPS)) {
                BRPHY1GV2_T1CORE_REGS[aPhyIdx]->testability_reg_c |= BRPHY1GV2_T1CORE_TRC_INTERNAL_LOOPBACK_MASK;
            } else {
                BRPHY1GV2_PCS_IEEE_REGS[aPhyIdx]->reg_3_0 |= (BRPHY1GV2_PCS_IEEE_PCS_CONTROL_REG1_LOOPBACK_MASK);
            }
        } else if(aMode == ETHXCVR_LOOPBACK_MODE_NONE) {
            if (0U != (aConfig->speed & ETHXCVR_SPEED_1000MBPS)) {
                temp = BRPHY1GV2_T1CORE_TRC_INTERNAL_LOOPBACK_MASK ;
                BRPHY1GV2_T1CORE_REGS[aPhyIdx]->testability_reg_c &= (uint16_t)~temp;
            } else {
                temp = BRPHY1GV2_PCS_IEEE_PCS_CONTROL_REG1_LOOPBACK_MASK ;
                BRPHY1GV2_PCS_IEEE_REGS[aPhyIdx]->reg_3_0 &= (uint16_t)~temp;
            }
            /* Re-enable autonegotiation (if required) */
            if (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg) {
                BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1 =
                    (aConfig->phy.masterMode ? BRPHY1GV2_AUTONEGIEEE_B1AAR1_MASTER_SLAVE_MASK : 0U);
                /* adv 1000T1 or 100T1 */
                if (0U != (aConfig->speed & ETHXCVR_SPEED_100MBPS)) {
                    BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1 |= BRPHY1GV2_AUTONEGIEEE_B1AAR1_ADVERISE_100BASE_T1_MASK;
                }
                if (0U != (aConfig->speed & ETHXCVR_SPEED_1000MBPS)) {
                    BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg1 |= BRPHY1GV2_AUTONEGIEEE_B1AAR1_ADVERISE_1000BASE_T1_MASK;
                }

                /* push changes to REG1 (per std) */
                BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->baset1_an_advertisement_reg0 =
                    (BRPHY1GV2_AUTONEGIEEE_ADVREG0_SELECTOR_VAL &
                     BRPHY1GV2_AUTONEGIEEE_B1AAR0_SELECTOR_FIELD_MASK);

                BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control =
                    (BRPHY1GV2_AUTONEGIEEE_AB1AC_AUTONEG_ENABLE_MASK |
                     BRPHY1GV2_AUTONEGIEEE_AB1AC_RESTART_AUTO_NEGOTIATION_MASK);
            }
        } else {
            retVal = BCM_ERR_NOSUPPORT ;
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetDuplexMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetDuplexMode(uint8_t aPhyIdx,
                                ETHXCVR_CntxtType *const aPhyCntxt,
                                const ETHXCVR_PortConfigType *const aConfig,
                                ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetFlowControl(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetFlowControl(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
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
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
        Disable DSP RDB clock (T1_CORE_TX_CLOCK_OVERRIDE_VALUE)
        set *aSQIValue to sqi field (PHYCONTROL_TC1_DCQ_SQI)
        Enable DSP RDB clock (T1_CORE_TX_CLOCK_OVERRIDE_VALUE)
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetSQIValue(uint8_t aPhyIdx,
                              ETHXCVR_CntxtType *const aPhyCntxt,
                              const ETHXCVR_PortConfigType *const aConfig,
                              uint32_t *const aSQIValue)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* This check had to be added here to adderss the JIRA:SWAUTO-15481 */
        /* Read the register only if the mode is either active or active wake up where low power bit is not set */
        if ((BRPHY1GV2_RWData[aPhyIdx].mode == ETHXCVR_MODE_ACTIVE) || (BRPHY1GV2_RWData[aPhyIdx].mode == ETHXCVR_MODE_ACTIVE_WAKEUP)) {
            *aSQIValue = ((uint32_t)BRPHY1GV2_PHYCONTROL_REGS[aPhyIdx]->tc1_dcq_sqi & BRPHY1GV2_PHYCONTROL_TC1_DCQ_SQI_MASK)
                                                                                   >> BRPHY1GV2_PHYCONTROL_TC1_DCQ_SQI_SHIFT;
        } else {
            *aSQIValue = 0UL;
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_Init(uint8_t aPhyIdx,
                       ETHXCVR_CntxtType *const aPhyCntxt,
                       const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
        /* Disable TC10 */
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c4_reg &= ~ET_BRPHY_4_CTYPE_TC10_CTRL_DEF_MASK;
        /* Disable TC10  */
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c4_reg |= (1UL << ET_BRPHY_4_CTYPE_TC10_CTRL_DEF_SHIFT);
        /* BR 1G speed  */
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c2_reg |= ET_BRPHY_2_CTYPE_BR_1G_SPD_DEF_MASK;
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c2_reg |= ET_BRPHY_2_CTYPE_BRPHY_1PORT_SOURCE_SYNC_MII_MASK;
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c4_reg |= ET_BRPHY_4_CTYPE_BR_MS_DEF_MASK;
        BCM_DelayUs(100UL);
        /* Clock and PLL programming */
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c3_reg |= ET_BRPHY_3_CTYPE_I_AFE_PD_CML_LCREFOUT_MASK;
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c3_reg |= ET_BRPHY_3_CTYPE_I_AFE_REF_CMOS_CLK_HZ_MASK;
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c3_reg |= ET_BRPHY_3_CTYPE_SAFETY_PIN_N_PHY0_MASK;
        BCM_DelayUs(100UL);
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c3_reg |= ET_BRPHY_3_CTYPE_I_PLLRST_STARTUP_RSTB_MASK;
        BCM_DelayUs(100UL);
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c3_reg &= ~ET_BRPHY_3_CTYPE_I_PLLRST_STARTUP_RSTB_MASK;
        BCM_DelayUs(100UL);
        BRPHY1GV2_ET_REGS[aPhyIdx]->brphy_c3_reg |= ET_BRPHY_3_CTYPE_I_PLLRST_STARTUP_RSTB_MASK;
        BCM_DelayUs(5000UL);
        BRPHY1GV2_RWData[aPhyIdx].mode = ETHXCVR_MODE_RESET;
    }
    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_DeInit(uint8_t aPhyIdx,
                         ETHXCVR_CntxtType *const aPhyCntxt,
                         const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_OK;
}

/**
    @code{.unparsed}
    if aPhyIdx is invalid return BCM_ERR_INVAL_PARAMS
    if aMode is ETHXCVR_BOOLEAN_TRUE return BCM_ERR_NOSUPPORT
    return BCM_ERR_OK
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetJumboMode(uint8_t aPhyIdx,
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetJumboMode(uint8_t aPhyIdx,
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_SetAutoNegMode(uint8_t aPhyIdx,
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_RestartAutoNeg(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_BOOLEAN_FALSE == aConfig->autoNeg) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_control |=
            BRPHY1GV2_AUTONEGIEEE_AB1AC_RESTART_AUTO_NEGOTIATION_MASK;
    }

    return retVal;
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_GetAutoNegStatus(uint8_t aPhyIdx,
                                   ETHXCVR_CntxtType *const aPhyCntxt,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t reg;

    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aPhyIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        reg = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_status;
        reg = BRPHY1GV2_AUTONEG_IEEE_REGS[aPhyIdx]->autoneg_baset1_an_status;
        if (0U == (reg & BRPHY1GV2_AUTONEGIEEE_AB1AS_AUTONEG_ABILITY_MASK)) {
            *aStatus = ETHXCVR_AUTONEGSTATUS_NO_ABILITY;
        } else if ((0U == (reg & BRPHY1GV2_AUTONEGIEEE_AB1AS_AUTONEG_COMPLETE_MASK)) ||
                   (ETHXCVR_BOOLEAN_FALSE == aConfig->autoNeg)) {
            *aStatus = ETHXCVR_AUTONEGSTATUS_INCOMPLETE;
        } else {
            *aStatus = ETHXCVR_AUTONEGSTATUS_COMPLETE;
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    return BCM_ERR_NOSUPPORT
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_StateHandler(uint8_t aPhyIdx,
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_UpdateHWStatus(uint8_t aPhyIdx,
                                 ETHXCVR_CntxtType *const aPhyCntxt,
                                 const ETHXCVR_PortConfigType *const aConfig)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    if chip is not bcm8957x A0
        return BRPHY1GV2_TC10IRQHandler(aPhyIdx, aParams, &BRPHY1GV2_RWData[aPhyIdx])
    else
        return BCM_ERR_OK;
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_IRQHandler(uint8_t aPhyIdx,
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

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_LinkChangeIndHandler(uint8_t aPhyIdx,
                                       ETHXCVR_CntxtType *const aPhyCntxt,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_LinkStateType *const aLinkState,
                                       uint32_t *const aIsLinkStateChanged)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @code{.unparsed}
    Read id0 and id1 register
    Calculate manufacture id, model and revision id based on id0 and id1 register values.
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_PhyGetID (ETHXCVR_IDType aID,
                                     ETHXCVR_PhyIDType *const aPhyID)
{
    int32_t   retVal = BCM_ERR_OK;
    uint32_t  id0 = 0U, id1 = 0U;
    volatile uint32_t *id0Reg = (volatile uint32_t *)(uintptr_t)0xE0540008UL;
    volatile uint32_t *id1Reg = (volatile uint32_t *)(uintptr_t)0xE054000CUL;
    if (BRPHY1GV2_AUTONEGIEEE_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* PMA_PMD_IEEE_DEVICE_ID_REG0 and PMA_PMD_IEEE_DEVICE_ID_REG1 */
        id0 = *id0Reg;
        id1 = *id1Reg;
        aPhyID->manuf = ETHXCVR_PHY_OUI(id0, id1);
        aPhyID->model = ETHXCVR_PHY_MODEL(id0, id1);
        aPhyID->rev   = ETHXCVR_PHY_REV(id0, id1);
    }
    return retVal;
}

/**
    @code{.unparsed}
    Disable the AN (If AN is enabled)
    Run cable diagnostics
    Perform phy reset
    Set the mode again with cached configuration
    @endcode

    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
static int32_t BRPHY1GV2_RunCableDiag(uint8_t aPhyIdx,
                               ETHXCVR_CntxtType *const aPhyCntxt,
                               const ETHXCVR_PortConfigType *const aConfig,
                               const ETHXCVR_AcdParamsType aParams,
                               ETHXCVR_CableDiagResultType *const aResult)
{
    int32_t retVal = BCM_ERR_OK;
    ETHXCVR_PortConfigType  aConfigTmp ;
    ETHXCVR_ModeType        aMode ;

    BCM_MemCpy(&aConfigTmp, aConfig, sizeof(ETHXCVR_PortConfigType)) ;

    retVal = BRPHY1GV2_GetMode(aPhyIdx, aPhyCntxt, aConfig, &aMode) ;

    /* When trying to enable Cable Diag, the AUTONEG must be turned off. */
    if ((BCM_ERR_OK == retVal) && (ETHXCVR_BOOLEAN_TRUE == aConfig->autoNeg)) {
        aConfigTmp.autoNeg = ETHXCVR_BOOLEAN_FALSE ;
        retVal = BRPHY1GV2_SetMode(aPhyIdx, aPhyCntxt, &aConfigTmp, aMode);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = BRPHY1GV2_PlatformRunCableDiag(aPhyIdx, aPhyCntxt, aConfig, aParams, aResult);
    }
    if(BCM_ERR_OK == retVal) {
        retVal = BRPHY1GV2_Reset(aPhyIdx, aPhyCntxt, aConfig) ;
    }
    if (BCM_ERR_OK == retVal) {
        retVal = BRPHY1GV2_SetMode(aPhyIdx, aPhyCntxt, aConfig, aMode);
    }
    return retVal ;
}

/**
    @trace #BRCM_SWARCH_BRPHY1GV2_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_BRPHY1GV2_FUNCTBL
*/
const ETHXCVR_FuncTblType BRPHY1GV2_FuncTbl = {
    .init = BRPHY1GV2_Init,
    .deinit = BRPHY1GV2_DeInit,
    .reset = BRPHY1GV2_Reset,
    .setMode = BRPHY1GV2_SetMode,
    .getMode = BRPHY1GV2_GetMode,
    .setMasterMode = BRPHY1GV2_SetMasterMode,
    .getMasterMode = BRPHY1GV2_GetMasterMode,
    .getSpeed = BRPHY1GV2_GetSpeed,
    .setSpeed = BRPHY1GV2_SetSpeed,
    .getDuplexMode = BRPHY1GV2_GetDuplexMode,
    .setDuplexMode = BRPHY1GV2_SetDuplexMode,
    .setFlowControl = BRPHY1GV2_SetFlowControl,
    .getFlowControl = BRPHY1GV2_GetFlowControl,
    .getLinkState = BRPHY1GV2_GetLinkState,
    .getSQIValue = BRPHY1GV2_GetSQIValue,
    .setLoopbackMode = BRPHY1GV2_SetLoopbackMode,
    .getLoopbackMode = BRPHY1GV2_GetLoopbackMode,
    .setJumboMode = BRPHY1GV2_SetJumboMode,
    .getJumboMode = BRPHY1GV2_GetJumboMode,
    .setAutoNegMode = BRPHY1GV2_SetAutoNegMode,
    .getAutoNegStatus = BRPHY1GV2_GetAutoNegStatus,
    .restartAutoNeg = BRPHY1GV2_RestartAutoNeg,
    .stateHandler = BRPHY1GV2_StateHandler,
    .updateHWStatus = BRPHY1GV2_UpdateHWStatus,
    .linkChangeIndHandler = BRPHY1GV2_LinkChangeIndHandler,
    .irqHandler = BRPHY1GV2_IRQHandler,
    .runCableDiag = BRPHY1GV2_RunCableDiag,
    .getID = BRPHY1GV2_PhyGetID,
};

/** @} */
