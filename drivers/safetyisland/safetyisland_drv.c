/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_safetyislanddrv_impl SAFETYISLAND Driver Design
    @ingroup grp_safetyislanddrv

    @addtogroup grp_safetyislanddrv_impl
    @{

    @file safetyisland_drv.c
    @brief SAFETYISLAND Driver Design

    @version 0.1 Initial version
*/
#include <safetyisland.h>
#include <safetyisland_rdb.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <compiler.h>

/**
    @name SAFETYISLAND driver Design IDs
    @{
    @brief Design IDs for SAFETYISLAND driver
*/
#define BRCM_SWDSGN_SAFETYISLAND_DRV_DEV_TYPE                                  (0xB201U)    /**< @brief #SAFETYISLAND_DrvDevType                         */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_DEV_GLOBAL                                (0xB202U)    /**< @brief #SAFETYISLAND_DrvDev                             */
#define BRCM_SWDSGN_SAFETYISLAND_REGS_GLOBAL                                   (0xB203U)    /**< @brief #SAFETYISLAND_Regs                               */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_0_MACRO                        (0xB204U)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_0                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_1_MACRO                        (0xB205U)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_1                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_2_MACRO                        (0xB206U)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_2                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_3_MACRO                        (0xB207U)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_3                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_4_MACRO                        (0xB208U)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_4                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_5_MACRO                        (0xB209U)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_5                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_6_MACRO                        (0xB20AU)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_6                   */
#define BRCM_SWDSGN_SAFETYISLAND_FAULT_MASK_GRP_7_MACRO                        (0xB20BU)    /**< @brief #SAFETYISLAND_FAULT_MASK_GRP_7                   */

#define BRCM_SWDSGN_SAFETYISLAND_DRV_INIT_PROC                                 (0xB210U)    /**< @brief #SAFETYISLAND_DrvInit                            */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_DEINIT_PROC                               (0xB211U)    /**< @brief #SAFETYISLAND_DrvDeInit                          */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_CONTROL_CONFIG_PROC                 (0xB212U)    /**< @brief #SAFETYISLAND_DrvFaultControlConfig              */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC                    (0xB213U)    /**< @brief #SAFETYISLAND_DrvFaultMaskConfig                 */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_STATUS_PROC                         (0xB214U)    /**< @brief #SAFETYISLAND_DrvFaultStatus                     */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_CLEAR_PROC                          (0xB215U)    /**< @brief #SAFETYISLAND_DrvFaultClear                      */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_SAFETY_STATUS_PROC                  (0xB216U)    /**< @brief #SAFETYISLAND_DrvFaultSafetyStatus               */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_SAFETY_CLEAR_PROC                   (0xB217U)    /**< @brief #SAFETYISLAND_DrvFaultSafetyClear                */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_STATUS_FAULT_SAFETY_RESET_SEQUENCER_PROC  (0xB218U)    /**< @brief #SAFETYISLAND_DrvStatusFaultSafetyResetSequencer */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_CLEAR_FAULT_SAFETY_RESET_SEQUENCER_PROC   (0xB219U)    /**< @brief #SAFETYISLAND_DrvClearFaultSafetyResetSequencer  */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_SAFETY_BISR_STATUS_PROC             (0xB21AU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyBISRStatus           */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_SAFETY_GEN_REQ_CPU_RESET_PROC       (0xB21BU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyGenReqCpuReset       */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC           (0xB21CU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyResetGenCpu          */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_FAULT_SAFETY_FAULT_GEN_CPU_PROC           (0xB21DU)    /**< @brief #SAFETYISLAND_DrvFaultSafetyFaultGenCpu          */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_WRITE_FAULT_SAFETY_DEBUG_PROC             (0xB21EU)    /**< @brief #SAFETYISLAND_DrvWriteFaultSafetyDebug           */
#define BRCM_SWDSGN_SAFETYISLAND_DRV_READ_FAULT_SAFETY_DEBUG_PROC              (0xB21FU)    /**< @brief #SAFETYISLAND_DrvReadFaultSafetyDebug            */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP0_PROC               (0xB220U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp0             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP1_PROC               (0xB221U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp1             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP2_PROC               (0xB222U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp2             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP3_PROC               (0xB223U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp3             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP4_PROC               (0xB224U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp4             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP5_PROC               (0xB225U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp5             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_MASK_CONFIG_GRP6_PROC               (0xB226U)    /**< @brief #SAFETYISLAND_IntFaultMaskConfigGrp6             */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP0_PROC                     (0xB227U)    /**< @brief #SAFETYISLAND_IntFaultClearGrp0                  */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP1_PROC                     (0xB228U)    /**< @brief #SAFETYISLAND_IntFaultClearGrp1                  */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP2_PROC                     (0xB229U)    /**< @brief #SAFETYISLAND_IntFaultClearGrp2                  */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP3_PROC                     (0xB22AU)    /**< @brief #SAFETYISLAND_IntFaultClearGrp3                  */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP4_PROC                     (0xB22BU)    /**< @brief #SAFETYISLAND_IntFaultClearGrp4                  */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP5_PROC                     (0xB22CU)    /**< @brief #SAFETYISLAND_IntFaultClearGrp5                  */
#define BRCM_SWDSGN_SAFETYISLAND_INT_FAULT_CLEAR_GRP6_PROC                     (0xB22DU)    /**< @brief #SAFETYISLAND_IntFaultClearGrp6                  */
/** @} */

/**
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_INIT_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CONTROL_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_CLEAR_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_STATUS_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_CLEAR_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_BISR_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_GEN_REQ_CPU_RESET_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_FAULT_GEN_CPU_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_WRITE_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_READ_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
*/
static SAFETYISLAND_RDBType *const SAFETYISLAND_Regs[SAFETYISLAND_MAX_HW_ID] = {
#if (SAFETYISLAND_MAX_HW_ID == 0UL)
#error "SAFETYISLAND_MAX_HW_ID == 0"
#endif
    (SAFETYISLAND_RDBType *) RIG_SAFETY_CTRL_BASE,
#if (SAFETYISLAND_MAX_HW_ID > 1UL)
#error "SAFETYISLAND_MAX_HW_ID > 1 is not supported by the driver"
#endif /* SAFETYISLAND_MAX_HW_ID > 1 */
};

/**
    @name SAFETYISLAND Driver device type
    @{
    @brief SAFETYISLAND Driver device type struct

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_INIT_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CONTROL_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_CLEAR_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_STATUS_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_CLEAR_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_BISR_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_GEN_REQ_CPU_RESET_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_FAULT_GEN_CPU_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_WRITE_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_READ_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
 */
typedef struct sSAFETYISLAND_DrvDevType {
    SAFETYISLAND_StateType   state; /**< @brief State of the driver */
} SAFETYISLAND_DrvDevType;
/** @} */


/**
    @brief SAFETYISLAND hardware context instance

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_INIT_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CONTROL_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_CLEAR_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_STATUS_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_CLEAR_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_BISR_STATUS_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_GEN_REQ_CPU_RESET_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_FAULT_GEN_CPU_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_WRITE_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_READ_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
*/
static SAFETYISLAND_DrvDevType SAFETYISLAND_DrvDev[SAFETYISLAND_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_0                      0x0000000FUL    /**< @brief Group0 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_1                      0x000000F0UL    /**< @brief Group1 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_2                      0x00000F00UL    /**< @brief Group2 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_3                      0x0000F000UL    /**< @brief Group3 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_4                      0x000F0000UL    /**< @brief Group4 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_5                      0x00F00000UL    /**< @brief Group5 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_6                      0x0F000000UL    /**< @brief Group6 mask */

/**
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
*/
#define SAFETYISLAND_FAULT_MASK_GRP_7                      0xF0000000UL    /**< @brief Group7 mask */

/**
    @brief Fault mask configuration for Group0

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp0(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp0 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_OTP0_FDONE_TIMEOUT)) {
        faultMaskGrp0 |= SAFETYISLAND_FAULT_MASK_OTP0_FDONE_TIMEOUT_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_BISR_FAIL)) {
        faultMaskGrp0 |= SAFETYISLAND_FAULT_MASK_BISR_FAIL_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_LOSS)) {
        faultMaskGrp0 |= SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_LOSS_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_TIMEOUT)) {
        faultMaskGrp0 |= SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_TIMEOUT_MASK_MASK ;
    }
    return faultMaskGrp0 ;
}

/**
    @brief Fault mask configuration for Group1

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp1(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp1 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_MEM_INIT_TIMEOUT)) {
        faultMaskGrp1 |= SAFETYISLAND_FAULT_MASK_MCU_0_MEM_INIT_TIMEOUT_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_MEM_DED)) {
        faultMaskGrp1 |= SAFETYISLAND_FAULT_MASK_MCU_0_MEM_DED_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_FUNCTIONAL)) {
        faultMaskGrp1 |= SAFETYISLAND_FAULT_MASK_MCU_0_FUNCTIONAL_MASK_MASK ;
    }
    return faultMaskGrp1 ;
}

/**
    @brief Fault mask configuration for Group2

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp2(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp2 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_WWDT0_TIMEOUT)) {
        faultMaskGrp2 |= SAFETYISLAND_FAULT_MASK_MCU_0_WWDT0_TIMEOUT_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_WWDT_1_7_TIMEOUT)) {
        faultMaskGrp2 |= SAFETYISLAND_FAULT_MASK_WWDT_1_7_TIMEOUT_MASK_MASK ;
    }
    return faultMaskGrp2 ;
}

/**
    @brief Fault mask configuration for Group3

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp3(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp3 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_SM_PMIC_INT)) {
        faultMaskGrp3 |= SAFETYISLAND_FAULT_MASK_SM_PMIC_INT_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_SM_ACQ_HSADC_FUNC)) {
        faultMaskGrp3 |= SAFETYISLAND_FAULT_MASK_SM_ACQ_HSADC_FUNC_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_SM_GP_SRAM_MEM_PROTECT)) {
        faultMaskGrp3 |= SAFETYISLAND_FAULT_MASK_SM_GP_SRAM_MEM_PROTECT_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_MCU1_LOCKUP_Q8_FATAL)) {
        faultMaskGrp3 |= SAFETYISLAND_FAULT_MASK_MCU1_LOCKUP_Q8_FATAL_MASK_MASK ;
    }
    return faultMaskGrp3 ;
}

/**
    @brief Fault mask configuration for Group4

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp4(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp4 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_RIG_DED)) {
        faultMaskGrp4 |= SAFETYISLAND_FAULT_MASK_RIG_DED_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_RESETB_IN_PIN)) {
        faultMaskGrp4 |= SAFETYISLAND_FAULT_MASK_RESETB_IN_PIN_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_POWER_GOOD_PIN)) {
        faultMaskGrp4 |= SAFETYISLAND_FAULT_MASK_POWER_GOOD_PIN_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_BROWNOUT_DETECTOR)) {
        faultMaskGrp4 |= SAFETYISLAND_FAULT_MASK_BROWNOUT_DETECTOR_MASK_MASK ;
    }
    return faultMaskGrp4 ;
}

/**
    @brief Fault mask configuration for Group5

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp5(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp5 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_OVERHEAT_DETECTOR)) {
        faultMaskGrp5 |= SAFETYISLAND_FAULT_MASK_OVERHEAT_DETECTOR_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_SYSFABRIC_NOC)) {
        faultMaskGrp5 |= SAFETYISLAND_FAULT_MASK_SYSFABRIC_NOC_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_AHB_APB_BUS)) {
        faultMaskGrp5 |= SAFETYISLAND_FAULT_MASK_AHB_APB_BUS_MASK_MASK ;
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_QSPI_FIFO_OVERUNDER_FLOW)) {
        faultMaskGrp5 |= SAFETYISLAND_FAULT_MASK_QSPI_FIFO_OVERUNDER_FLOW_MASK_MASK ;
    }
    return faultMaskGrp5 ;
}

/**
    @brief Fault mask configuration for Group6

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultMaskConfigGrp6(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    SAFETYISLAND_FaultMaskType faultMaskGrp6 = 0UL;
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_CPU_FAULT_OUT)) {
        faultMaskGrp6 |= SAFETYISLAND_FAULT_MASK_CPU_FAULT_OUT_MASK_MASK ;
    }
    return faultMaskGrp6 ;
}

/**
    @brief Clear Group0 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp0(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp0 = 0UL;

    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_OTP0_FDONE_TIMEOUT)) {
        faultStatusGrp0 |= SAFETYISLAND_FAULT_STATUS_OTP0_FDONE_TIMEOUT_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_BISR_FAIL)) {
        faultStatusGrp0 |= SAFETYISLAND_FAULT_STATUS_BISR_FAIL_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_LOSS)) {
        faultStatusGrp0 |= SAFETYISLAND_FAULT_STATUS_SYSPLL_LOCK_LOSS_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_SYSPLL_LOCK_TIMEOUT)) {
        faultStatusGrp0 |= SAFETYISLAND_FAULT_STATUS_SYSPLL_LOCK_TIMEOUT_W2CLR_MASK ;
    }
    return faultStatusGrp0 ;
}

/**
    @brief Clear Group1 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp1(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp1 = 0UL;
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_MBIST_FAIL)) {
        faultStatusGrp1 |= SAFETYISLAND_FAULT_STATUS_MCU_0_MBIST_FAIL_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_MEM_INIT_TIMEOUT)) {
        faultStatusGrp1 |= SAFETYISLAND_FAULT_STATUS_MCU_0_MEM_INIT_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_MEM_DED)) {
        faultStatusGrp1 |= SAFETYISLAND_FAULT_STATUS_MCU_0_MEM_DED_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_FUNCTIONAL)) {
        faultStatusGrp1 |= SAFETYISLAND_FAULT_STATUS_MCU_0_FUNCTIONAL_W2CLR_MASK ;
    }
    return faultStatusGrp1 ;
}

/**
    @brief Clear Group2 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp2(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp2 = 0UL;
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_SYSTEM_RESET_REQ)) {
        faultStatusGrp2 |= SAFETYISLAND_FAULT_STATUS_M0SRRW2_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_NMI_TIMEOUT)) {
        faultStatusGrp2 |= SAFETYISLAND_FAULT_STATUS_MCU_0_NMI_TIMEOUT_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU0_WWDT0_TIMEOUT)) {
        faultStatusGrp2 |= SAFETYISLAND_FAULT_STATUS_MCU_0_WWDT0_TIMEOUT_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_WWDT_1_7_TIMEOUT)) {
        faultStatusGrp2 |= SAFETYISLAND_FAULT_STATUS_WWDT_1_7_TIMEOUT_W2CLR_MASK ;
    }
    return faultStatusGrp2 ;
}

/**
    @brief Clear Group3 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp3(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp3 = 0UL;
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_SM_PMIC_INT)) {
        faultStatusGrp3 |= SAFETYISLAND_FAULT_STATUS_SM_PMIC_INT_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_SM_ACQ_HSADC_FUNC)) {
        faultStatusGrp3 |= SAFETYISLAND_FAULT_STATUS_SM_ACQ_HSADC_FUNC_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_SM_GP_SRAM_MEM_PROTECT)) {
        faultStatusGrp3 |= SAFETYISLAND_FAULT_STATUS_SM_GP_SRAM_MEM_PROTECT_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_MCU1_LOCKUP_Q8_FATAL)) {
        faultStatusGrp3 |= SAFETYISLAND_FAULT_STATUS_MCU1_LOCKUP_Q8_FATAL_W2CLR_MASK ;
    }
    return faultStatusGrp3 ;
}

/**
    @brief Clear Group4 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp4(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp4 = 0UL;
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_RIG_DED)) {
        faultStatusGrp4 |= SAFETYISLAND_FAULT_STATUS_RIG_DED_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_POWER_GOOD_PIN)) {
        faultStatusGrp4 |= SAFETYISLAND_FAULT_STATUS_POWER_GOOD_PIN_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_BROWNOUT_DETECTOR)) {
        faultStatusGrp4 |= SAFETYISLAND_FAULT_STATUS_BROWNOUT_DETECTOR_W2CLR_MASK ;
    }
    return faultStatusGrp4 ;
}

/**
    @brief Clear Group5 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp5(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp5 = 0UL;
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_OVERHEAT_DETECTOR)) {
        faultStatusGrp5 |= SAFETYISLAND_FAULT_STATUS_OVERHEAT_DETECTOR_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_SYSFABRIC_NOC)) {
        faultStatusGrp5 |= SAFETYISLAND_FAULT_STATUS_SYSFABRIC_NOC_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_AHB_APB_BUS)) {
        faultStatusGrp5 |= SAFETYISLAND_FAULT_STATUS_AHB_APB_BUS_W2CLR_MASK ;
    }
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_QSPI_FIFO_OVERUNDER_FLOW)) {
        faultStatusGrp5 |= SAFETYISLAND_FAULT_STATUS_QSPI_FIFO_OVERUNDER_FLOW_W2CLR_MASK ;
    }
    return faultStatusGrp5 ;
}

/**
    @brief Clear Group6 faults

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
static SAFETYISLAND_FaultMaskType SAFETYISLAND_IntFaultClearGrp6(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    SAFETYISLAND_FaultMaskType faultStatusGrp6 = 0UL;
    if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_CPU_FAULT_OUT)) {
        faultStatusGrp6 |= SAFETYISLAND_FAULT_STATUS_CPU_FAULT_OUT_W2CLR_MASK ;
    }
    return faultStatusGrp6 ;
}

/**
    @brief initializes SAFETYISLAND driver

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    SAFETYISLAND_DrvDev[hwId].state = SAFETYISLAND_STATE_UNINIT
    Validate function parameters
    SAFETYISLAND_Regs[hwId]->fault_control |= SAFETYISLAND_FAUL_CONT_FAULT_CAUSE_EN_MASK
    SAFETYISLAND_DrvDev[hwId].state = SAFETYISLAND_STATE_INITIALIZED
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_INIT_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
*/
int32_t SAFETYISLAND_DrvInit(SAFETYISLAND_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK ;
    SAFETYISLAND_DrvDev[hwId].state = SAFETYISLAND_STATE_UNINIT;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        SAFETYISLAND_Regs[hwId]->fault_control |= SAFETYISLAND_FAUL_CONT_FAULT_CAUSE_EN_MASK ;
        SAFETYISLAND_DrvDev[hwId].state = SAFETYISLAND_STATE_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief De-initializes SAFETYISLAND driver

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    SAFETYISLAND_DrvDev[hwId].state = SAFETYISLAND_STATE_UNINIT
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_INIT
*/
int32_t SAFETYISLAND_DrvDeInit(SAFETYISLAND_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        SAFETYISLAND_DrvDev[hwId].state = SAFETYISLAND_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Fault Control Configuration

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    if(faultControlConfig.softSecureBootEn == SAFETYISLAND_SECURE_BOOT_DISABLE)
      SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_SOFT_SECURE_BOOT_EN_MASK
    else
      SAFETYISLAND_Regs[hwId]->fault_control |= SAFETYISLAND_FAUL_CONT_SOFT_SECURE_BOOT_EN_MASK

    if(faultControlConfig.faultCause == SAFETYISLAND_FAULT_CAUSE_DISABLE)
      SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_FAULT_CAUSE_EN_MASK
    else
      SAFETYISLAND_Regs[hwId]->fault_control |= SAFETYISLAND_FAUL_CONT_FAULT_CAUSE_EN_MASK

    SAFETYISLAND_Regs[hwId]->fault_control &=
    ~SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_RESET_MASK
    SAFETYISLAND_Regs[hwId]->fault_control |=
    ((faultControlConfig.mcu0NmiTimeoutRst & 0x1UL) <<
      SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_RESET_SHIFT)

    SAFETYISLAND_Regs[hwId]->fault_control &=
    ~SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_SCALER_MASK
    SAFETYISLAND_Regs[hwId]->fault_control |=
    ((faultControlConfig.mcu0NmiTimeoutScaler & 0xFUL) <<
      SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_SCALER_SHIFT)

    SAFETYISLAND_Regs[hwId]->fault_control &=
    ~SAFETYISLAND_FAUL_CONT_RESETFSM_RESET_OUT_COUNT_SCALER_MASK
    SAFETYISLAND_Regs[hwId]->fault_control |=
    ((faultControlConfig.resetFsmRstOutCntScaler & 0xFUL) <<
      SAFETYISLAND_FAUL_CONT_RESETFSM_RESET_OUT_COUNT_SCALER_SHIFT)
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CONTROL_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONTROL_CONFIG
*/
int32_t SAFETYISLAND_DrvFaultControlConfig(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultControlType faultControlConfig)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(faultControlConfig.softSecureBootEn == SAFETYISLAND_SECURE_BOOT_DISABLE) {
            SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_SOFT_SECURE_BOOT_EN_MASK ;
        } else {
            SAFETYISLAND_Regs[hwId]->fault_control |= SAFETYISLAND_FAUL_CONT_SOFT_SECURE_BOOT_EN_MASK ;
        }

        if(faultControlConfig.faultCause == SAFETYISLAND_FAULT_CAUSE_DISABLE) {
            SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_FAULT_CAUSE_EN_MASK ;
        } else {
            SAFETYISLAND_Regs[hwId]->fault_control |= SAFETYISLAND_FAUL_CONT_FAULT_CAUSE_EN_MASK ;
        }

        SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_RESET_MASK ;
        SAFETYISLAND_Regs[hwId]->fault_control |= ((faultControlConfig.mcu0NmiTimeoutRst & 0x1UL) << SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_RESET_SHIFT);

        SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_SCALER_MASK ;
        SAFETYISLAND_Regs[hwId]->fault_control |= ((faultControlConfig.mcu0NmiTimeoutScaler & 0xFUL) << SAFETYISLAND_FAUL_CONT_MCU_0_NMI_TIMEOUT_SCALER_SHIFT);

        SAFETYISLAND_Regs[hwId]->fault_control &= ~SAFETYISLAND_FAUL_CONT_RESETFSM_RESET_OUT_COUNT_SCALER_MASK ;
        SAFETYISLAND_Regs[hwId]->fault_control |= ((faultControlConfig.resetFsmRstOutCntScaler & 0xFUL) << SAFETYISLAND_FAUL_CONT_RESETFSM_RESET_OUT_COUNT_SCALER_SHIFT);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief This API configures fault mask.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    SAFETYISLAND_FaultMaskType faultMask = 0UL
    Validate function parameters
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_0))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp0(hwId, config)
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_1))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp1(hwId, config)
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_2))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp2(hwId, config)
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_3))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp3(hwId, config)
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_4))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp4(hwId, config)
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_5))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp5(hwId, config)
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_6))
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp6(hwId, config)
    SAFETYISLAND_Regs[hwId]->fault_mask = faultMask
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_MASK_CONFIG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvFaultMaskConfig(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultMaskConfigType config)
{
    int32_t ret = BCM_ERR_OK ;
    SAFETYISLAND_FaultMaskType faultMask = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_0)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp0(hwId, config);
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_1)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp1(hwId, config);
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_2)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp2(hwId, config);
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_3)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp3(hwId, config);
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_4)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp4(hwId, config);
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_5)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp5(hwId, config);
    }
    if(0UL != (config.faultMask & SAFETYISLAND_FAULT_MASK_GRP_6)) {
        faultMask |= SAFETYISLAND_IntFaultMaskConfigGrp6(hwId, config);
    }
    SAFETYISLAND_Regs[hwId]->fault_mask = faultMask ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API gets fault status.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    fault->status = SAFETYISLAND_Regs[hwId]->fault_status
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_STATUS_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvFaultStatus(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultStatusType *fault)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        fault->status = SAFETYISLAND_Regs[hwId]->fault_status ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API clears the faults

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    SAFETYISLAND_FaultMaskType faultStatus = 0UL
    Validate function parameters
    if(0UL == (SAFETYISLAND_Regs[hwId]->fault_status & SAFETYISLAND_FAULT_STATUS_RIGEL_FAULT_OUT_LIVE_MASK))
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_0))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp0(hwId, clear)
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_1))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp1(hwId, clear)
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_2))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp2(hwId, clear)
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_3))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp3(hwId, clear)
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_4))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp4(hwId, clear)
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_5))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp5(hwId, clear)
        if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_6))
            faultStatus |= SAFETYISLAND_IntFaultClearGrp6(hwId, clear)
        SAFETYISLAND_Regs[hwId]->fault_status = faultStatus
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_CONFIG_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvFaultClear(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultClearType clear)
{
    int32_t ret = BCM_ERR_OK ;
    SAFETYISLAND_FaultMaskType faultStatus     = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL == (SAFETYISLAND_Regs[hwId]->fault_status & SAFETYISLAND_FAULT_STATUS_RIGEL_FAULT_OUT_LIVE_MASK)) {
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_0)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp0(hwId, clear);
            }
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_1)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp1(hwId, clear);
            }
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_2)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp2(hwId, clear);
            }
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_3)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp3(hwId, clear);
            }
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_4)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp4(hwId, clear);
            }
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_5)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp5(hwId, clear);
            }
            if(0UL != (clear.faultMask & SAFETYISLAND_FAULT_MASK_GRP_6)) {
                faultStatus |= SAFETYISLAND_IntFaultClearGrp6(hwId, clear);
            }
            SAFETYISLAND_Regs[hwId]->fault_status = faultStatus ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API gets fault safety status.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    SAFETYISLAND_SAFETY_STATUS_TYPE safetyStatus = 0UL
    Validate function parameters
    safetyStatus = SAFETYISLAND_Regs[hwId]->safety_status
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_DONE_W2CLR_MASK))
        faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_DONE
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_GOOD_W2CLR_MASK))
        faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_GOOD
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_DONE_W2CLR_MASK))
        faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_DONE
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_ERROR_W2CLR_MASK))
        faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_ERROR
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_GOOD_W2CLR_MASK))
        faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_GOOD
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_STARTED_W2CLR_MASK))
        faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_STARTED
    return ret;
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_STATUS_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvFaultSafetyStatus(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyStatusType *faultSafety)
{
    int32_t ret = BCM_ERR_OK ;
    SAFETYISLAND_SAFETY_STATUS_TYPE safetyStatus = 0UL ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        safetyStatus = SAFETYISLAND_Regs[hwId]->safety_status ;

        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_DONE_W2CLR_MASK)) {
            faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_DONE ;
        }
        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_GOOD_W2CLR_MASK)) {
            faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_GOOD ;
        }
        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_DONE_W2CLR_MASK)) {
            faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_DONE ;
        }
        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_ERROR_W2CLR_MASK)) {
            faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_ERROR ;
        }
        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_GOOD_W2CLR_MASK)) {
            faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_GOOD ;
        }
        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_STARTED_W2CLR_MASK)) {
            faultSafety->status |= SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_STARTED ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief This API clears the safety faults

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    SAFETYISLAND_SAFETY_STATUS_TYPE safetyStatus = 0UL
    Validate function parameters
    if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_DONE))
        safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_DONE_W2CLR_MASK
    if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_GOOD))
        safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_GOOD_W2CLR_MASK
    if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_DONE))
        safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_DONE_W2CLR_MASK
    if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_ERROR))
        safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_ERROR_W2CLR_MASK
    if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_GOOD))
        safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_GOOD_W2CLR_MASK
    if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_STARTED))
        safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_STARTED_W2CLR_MASK
    SAFETYISLAND_Regs[hwId]->safety_status |= safetyStatus
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_CLEAR_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvFaultSafetyClear(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyClearType clear)
{
    int32_t ret = BCM_ERR_OK ;
    SAFETYISLAND_SAFETY_STATUS_TYPE safetyStatus = 0UL ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_DONE)) {
            safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_DONE_W2CLR_MASK ;
        }
        if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MBIST_GOOD)) {
            safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MBIST_GOOD_W2CLR_MASK ;
        }
        if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_DONE)) {
            safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_DONE_W2CLR_MASK ;
        }
        if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_ERROR)) {
            safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_ERROR_W2CLR_MASK ;
        }
        if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_GOOD)) {
            safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_GOOD_W2CLR_MASK ;
        }
        if(0UL != (clear.faultSafetyMask & SAFETYISLAND_FAULT_SAFETY_STS_MCU_0_MEM_INIT_STARTED)) {
            safetyStatus |= SAFETYISLAND_SAFETY_STATUS_MCU_0_MEM_INIT_STARTED_W2CLR_MASK ;
        }
        SAFETYISLAND_Regs[hwId]->safety_status |= safetyStatus;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API gets the reset sequencer state.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    resetSequencerStatus->state = (SAFETYISLAND_Regs[hwId]->safety_status & SAFETYISLAND_SAFETY_STATUS_RST_SEQUENCER_STATE_W2CLR_MASK)
    return ret;
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_STATUS_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvStatusFaultSafetyResetSequencer(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyResetSequencerStatusType *resetSequencerStatus)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        resetSequencerStatus->state = (SAFETYISLAND_Regs[hwId]->safety_status & SAFETYISLAND_SAFETY_STATUS_RST_SEQUENCER_STATE_W2CLR_MASK);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief This API clears the specified reset sequencer state

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    SAFETYISLAND_Regs[hwId]->safety_status |= (clear.rstFsmMask & SAFETYISLAND_SAFETY_STATUS_RST_SEQUENCER_STATE_W2CLR_MASK)
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_CLEAR_FAULT_SAFETY_RESET_SEQUENCER_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_STATUS_CLEAR
*/
int32_t SAFETYISLAND_DrvClearFaultSafetyResetSequencer(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyResetSequencerClearType clear)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        SAFETYISLAND_Regs[hwId]->safety_status |= (clear.rstFsmMask & SAFETYISLAND_SAFETY_STATUS_RST_SEQUENCER_STATE_W2CLR_MASK);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API gets fault safety built in self repair status

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    SAFETYISLAND_SAFETY_STATUS_TYPE safetyStatus = 0UL
    Validate function parameters
    safetyStatus = SAFETYISLAND_Regs[hwId]->safety_status
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_RIGEL_BISR_DONE_MASK))
        *faultSafetyBISRSts |= SAFETYISLAND_FAULT_SAFETY_BISR_STS_DONE
    if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_RIGEL_BISR_GOOD_MASK))
        *faultSafetyBISRSts |= SAFETYISLAND_FAULT_SAFETY_BISR_STS_GOOD
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_BISR_STATUS_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_FAULT_SAFETY_BISR_STATUS
*/
int32_t SAFETYISLAND_DrvFaultSafetyBISRStatus(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_FaultSafetyBISRStsType *faultSafetyBISRSts)
{
    int32_t ret = BCM_ERR_OK ;
    SAFETYISLAND_SAFETY_STATUS_TYPE safetyStatus = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        safetyStatus = SAFETYISLAND_Regs[hwId]->safety_status ;

        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_RIGEL_BISR_DONE_MASK)) {
            *faultSafetyBISRSts |= SAFETYISLAND_FAULT_SAFETY_BISR_STS_DONE ;
        }
        if (0UL != (safetyStatus & SAFETYISLAND_SAFETY_STATUS_RIGEL_BISR_GOOD_MASK)) {
            *faultSafetyBISRSts |= SAFETYISLAND_FAULT_SAFETY_BISR_STS_GOOD ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API generates CPU reset request and send out to the pin.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW)
        SAFETYISLAND_Regs[hwId]->cpu_reset_req = 0x01F00BAAUL
    else if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH)
        SAFETYISLAND_Regs[hwId]->cpu_reset_req = 0x81F00BAAUL
    else
        SAFETYISLAND_Regs[hwId]->cpu_reset_req = 0x01ACCE55UL
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION
*/
int32_t SAFETYISLAND_DrvFaultSafetyGenReqCpuReset(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_CpuFaultResetGenType cpuResetReqConfig)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(cpuResetReqConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW) {
            SAFETYISLAND_Regs[hwId]->cpu_reset_req = 0x01F00BAAUL ;
        } else if(cpuResetReqConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH) {
            SAFETYISLAND_Regs[hwId]->cpu_reset_req = 0x81F00BAAUL ;
        } else {
            SAFETYISLAND_Regs[hwId]->cpu_reset_req = 0x01ACCE55UL ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API generates CPU reset and send out to the pin.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW)
        SAFETYISLAND_Regs[hwId]->cpu_reset_out = 0x01F00BAAUL
    else if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH)
        SAFETYISLAND_Regs[hwId]->cpu_reset_out = 0x81F00BAAUL
    else
        SAFETYISLAND_Regs[hwId]->cpu_reset_out = 0x01ACCE55UL
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_RESET_GEN_CPU_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION
*/
int32_t SAFETYISLAND_DrvFaultSafetyResetGenCpu(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_CpuFaultResetGenType cpuResetConfig)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(cpuResetConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW) {
            SAFETYISLAND_Regs[hwId]->cpu_reset_out = 0x01F00BAAUL;
        } else if(cpuResetConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH) {
            SAFETYISLAND_Regs[hwId]->cpu_reset_out = 0x81F00BAAUL;
        } else {
            SAFETYISLAND_Regs[hwId]->cpu_reset_out = 0x01ACCE55UL;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief This API generates CPU fault and send out to the pin.

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW)
        SAFETYISLAND_Regs[hwId]->cpu_fault_out = 0x01F00BAAUL
    else if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH)
        SAFETYISLAND_Regs[hwId]->cpu_fault_out = 0x81F00BAAUL
    else
        SAFETYISLAND_Regs[hwId]->cpu_fault_out = 0x01ACCE55UL
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_FAULT_SAFETY_FAULT_GEN_CPU_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_CPU_ACTION
*/
int32_t SAFETYISLAND_DrvFaultSafetyFaultGenCpu(SAFETYISLAND_HwIDType hwId, SAFETYISLAND_CpuFaultResetGenType cpuFaultConfig)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_LOW) {
            SAFETYISLAND_Regs[hwId]->cpu_fault_out = 0x01F00BAAUL ;
        } else if(cpuFaultConfig.outValue == SAFETYISLAND_CPU_FAULT_RESET_VAL_HIGH) {
            SAFETYISLAND_Regs[hwId]->cpu_fault_out = 0x81F00BAAUL ;
        } else {
            SAFETYISLAND_Regs[hwId]->cpu_fault_out = 0x01ACCE55UL ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Writes to specified safety debug register

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    SAFETYISLAND_Regs[hwId]->fault_debug[index] = data
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_WRITE_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_DEBUG
*/
int32_t SAFETYISLAND_DrvWriteFaultSafetyDebug(SAFETYISLAND_HwIDType hwId, uint32_t index, uint32_t data)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (index >= SAFETYISLAND_FAULT_DEBUG_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        SAFETYISLAND_Regs[hwId]->fault_debug[index] = data ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Reads specified safety debug register

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Validate function parameters
    *data =  SAFETYISLAND_Regs[hwId]->fault_debug[index]
    return ret
    @endcode

    @trace #BRCM_SWARCH_SAFETYISLAND_DRV_READ_FAULT_SAFETY_DEBUG_PROC
    @trace #BRCM_SWREQ_SAFETYISLAND_DEBUG
*/
int32_t SAFETYISLAND_DrvReadFaultSafetyDebug(SAFETYISLAND_HwIDType hwId, uint32_t index, uint32_t *data)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= SAFETYISLAND_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SAFETYISLAND_DrvDev[hwId].state == SAFETYISLAND_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (index >= SAFETYISLAND_FAULT_DEBUG_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *data =  SAFETYISLAND_Regs[hwId]->fault_debug[index];

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}
/** @} */
