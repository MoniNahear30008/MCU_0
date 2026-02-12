/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_clkrstdrv_impl Clock Reset Driver Design
    @ingroup grp_clkrstdrv

    @addtogroup grp_clkrstdrv_impl
    @{

    @file drivers/clock_reset/clkrst_drv.c
    @brief Clock and reset Driver Design

    @version 0.1 Initial version
*/

#include "clkrst_rdb.h"
#include "syspll_rdb.h"
#include "cpu_rdb.h"
#include <clock_reset.h>
#include <bcm_err.h>

/**
    @name Clock and Reset Driver Design IDs
    @{
    @brief Driver Design IDs for clock and reset
*/
#define BRCM_SWDSGN_CLKRST_REGS_GLOBAL                   (0xB2E0U) /**< @brief #CLKRST_Regs                      */
#define BRCM_SWDSGN_CLKRST_SYSPLL_REGS_GLOBAL            (0xB2E1U) /**< @brief #CLKRST_SyspllRegs                */
#define BRCM_SWDSGN_CLKRST_CPU_REGS_GLOBAL               (0xB2E2U) /**< @brief #CLKRST_CpuRegs                   */

#define BRCM_SWDSGN_CLKRST_M7_RESET_PROC                 (0xB2E3U) /**< @brief #CLKRST_M7Reset                   */
#define BRCM_SWDSGN_CLKRST_Q8_RESET_PROC                 (0xB2E4U) /**< @brief #CLKRST_Q8Reset                   */
#define BRCM_SWDSGN_CLKRST_A55_RESET_PROC                (0xB2E5U) /**< @brief #CLKRST_A55Reset                  */
#define BRCM_SWDSGN_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC  (0xB2E7U) /**< @brief #CLKRST_SystemClockDivConfig      */

#define BRCM_SWDSGN_CLKRST_INT_SUS_CLK_DIV_VALI_PROC     (0xB2E9U) /**< @brief #CLKRST_IntSystemClockDivValidate */
/** @} */

/**
    @trace #BRCM_SWARCH_CLKRST_M7_RESET_PROC
    @trace #BRCM_SWARCH_CLKRST_Q8_RESET_PROC
    @trace #BRCM_SWARCH_CLKRST_A55_RESET_PROC
    @trace #BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC
    @trace #BRCM_SWREQ_CLKRST_CONFIG
*/
static CLKRST_RDBType * const CLKRST_Regs[CLKRST_MAX_HW_ID] =
{
#if (CLKRST_MAX_HW_ID == 0U)
#error "CLKRST_MAX_HW_ID == 0U"
#endif
    (CLKRST_RDBType *)RIG_CLKNRST_BASE,
#if (CLKRST_MAX_HW_ID > 1U)
#error "CLKRST_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC
    @trace #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG
*/
static SYSPLL_RDBType * const CLKRST_SyspllRegs[SYSPLL_MAX_HW_ID] =
{
#if (SYSPLL_MAX_HW_ID == 0U)
#error "SYSPLL_MAX_HW_ID == 0U"
#endif
    (SYSPLL_RDBType *)SYSPLL_BASE,
#if (SYSPLL_MAX_HW_ID > 1U)
#error "SYSPLL_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC
    @trace #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG
*/
static CPU_RDBType * const CLKRST_CpuRegs[CPU_MAX_HW_ID] =
{
#if (CPU_MAX_HW_ID == 0U)
#error "CPU_MAX_HW_ID == 0U"
#endif
    (CPU_RDBType *)MCU_BASE,
#if (CPU_MAX_HW_ID > 1U)
#error "CPU_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/** @brief M7 reset
    @code{.unparsed}
        ret = BCM_ERR_OK;
        if (hwId >= 4U)
            ret = BCM_ERR_INVAL_PARAMS;
        else
          if (m7ResetControl.resetType == CLKRST_M7_RESET_MCU1_RESETB)
            CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRC_MASK;
            if (m7ResetControl.resetValue == 0U)
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
            else if (m7ResetControl.resetValue == 1U)
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
            else if (m7ResetControl.resetValue == 2U)
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
            else
              ret = BCM_ERR_INVAL_PARAMS;
          else if (m7ResetControl.resetType == CLKRST_M7_RESET_DEBUG_PRT_RESETB)
            CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRC_MASK;
            if (m7ResetControl.resetValue == 0U)
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
            else if (m7ResetControl.resetValue == 1U)
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
            else if (m7ResetControl.resetValue == 2U)
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
              CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
            else
              ret = BCM_ERR_INVAL_PARAMS;
         else
            ret = BCM_ERR_INVAL_PARAMS;
       return ret;
    @endcode
    @trace #BRCM_SWARCH_CLKRST_M7_RESET_PROC
    @trace #BRCM_SWREQ_CLKRST_CONFIG

    @limitations None
*/
int32_t CLKRST_M7Reset(CLKRST_HwIDType hwId,  CLKRST_M7ResetControlType m7ResetControl)
{
    int32_t ret = BCM_ERR_OK;

    if (hwId >= CLKRST_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        if (m7ResetControl.resetType == CLKRST_M7_RESET_MCU1_RESETB) {
            /* Set force reset*/
            CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRC_MASK;
            if (m7ResetControl.resetValue == 0U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
            } else if (m7ResetControl.resetValue == 1U) {
                /* Reset de-assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
            } else if (m7ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
                /* Reset de-assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_MCU1_RSTB_FRCVAL_MASK;
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else if (m7ResetControl.resetType == CLKRST_M7_RESET_DEBUG_PRT_RESETB) {
            /* Set force reset*/
            CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRC_MASK;
            if (m7ResetControl.resetValue == 0U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
            } else if (m7ResetControl.resetValue == 1U) {
                /* Reset de-assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
            } else if (m7ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control &= ~CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
                /* Reset de-assertion*/
                CLKRST_Regs[hwId]->cpu_dma_sys_reset_control |= CLKRST_CDSRC_CPU_DMA_SYS_DAP_RSTB_FRCVAL_MASK;
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }
    return ret;
}

/** @brief Q8 reset

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK;
    uint32_t rstCtrl = 0UL;
    if (hwId >= 4U)
        ret = BCM_ERR_INVAL_PARAMS;
    else if (q8ResetControl.resetValue > 2U)
        ret = BCM_ERR_INVAL_PARAMS;
    else
        if (q8ResetControl.resetType == CLKRST_Q8_RESET_HRESET)
            CLKRST_Regs[0]->q8_top_reset_control |= CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRC_MASK;
            if (q8ResetControl.resetValue == 2U)
                 Reset assertion
                CLKRST_Regs[0]->q8_top_reset_control &= ~CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_MASK;
                 Reset de-assertion
                CLKRST_Regs[0]->q8_top_reset_control |= CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_MASK;
            else
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
        else if (q8ResetControl.resetType == CLKRST_Q8_RESET_PRESET)
            CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRC_MASK << (hwId*2U));
            if (q8ResetControl.resetValue == 2U)
                 Reset assertion
                CLKRST_Regs[0]->q8_top_reset_control &= ~(CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_MASK << (hwId*2U));
                 Reset de-assertion
                CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_MASK << (hwId*2U));
            else
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
        else if (q8ResetControl.resetType == CLKRST_Q8_RESET_CRESETB)
            CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRC_MASK << (hwId*2U));
            if (q8ResetControl.resetValue == 2U)
                 Reset assertion
                CLKRST_Regs[0]->q8_top_reset_control &= ~(CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_MASK <<(hwId*2U));
                 Reset de-assertion
                CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_MASK << (hwId*2U));
            else
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
        else if (q8ResetControl.resetType == CLKRST_Q8_RESET_APB_SLAVE_RESET)
            CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRC_MASK << (hwId*2U));
            if (q8ResetControl.resetValue == 2U)
                 Reset assertion
                CLKRST_Regs[0]->q8_top_reset_control &= ~(CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_MASK << (hwId*2U));
                 Reset de-assertion
                CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_MASK << (hwId*2U));
            else
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
        else
            ret = BCM_ERR_INVAL_PARAMS;
    @endcode
    @trace #BRCM_SWARCH_CLKRST_Q8_RESET_PROC
    @trace #BRCM_SWREQ_CLKRST_CONFIG

    @limitations None
*/
int32_t CLKRST_Q8Reset(CLKRST_HwIDType hwId,  CLKRST_Q8ResetControlType q8ResetControl)
{

    int32_t ret = BCM_ERR_OK;
    uint32_t rstCtrl = 0UL;
    if (hwId >= 4U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (q8ResetControl.resetValue > 2U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Set force reset*/
        if (q8ResetControl.resetType == CLKRST_Q8_RESET_HRESET) {
            CLKRST_Regs[0]->q8_top_reset_control |= CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRC_MASK;
            if (q8ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[0]->q8_top_reset_control &= ~CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_MASK;
                /* Reset de-assertion*/
                CLKRST_Regs[0]->q8_top_reset_control |= CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_MASK;
            } else {
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_HRESETN_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
            }
        } else if (q8ResetControl.resetType == CLKRST_Q8_RESET_PRESET) {
            CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRC_MASK << (hwId*2U));
            if (q8ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[0]->q8_top_reset_control &= ~(CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_MASK << (hwId*2U));
                /* Reset de-assertion*/
                CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_MASK << (hwId*2U));
            } else {
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_PRESETN_0_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
            }
        } else if (q8ResetControl.resetType == CLKRST_Q8_RESET_CRESETB) {
            CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRC_MASK << (hwId*2U));
            if (q8ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[0]->q8_top_reset_control &= ~(CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_MASK <<(hwId*2U));
                /* Reset de-assertion*/
                CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_MASK << (hwId*2U));
            } else {
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_CRSTB_0_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
            }
        } else if (q8ResetControl.resetType == CLKRST_Q8_RESET_APB_SLAVE_RESET) {
            CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRC_MASK << (hwId*2U));
            if (q8ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[0]->q8_top_reset_control &= ~(CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_MASK << (hwId*2U));
                /* Reset de-assertion*/
                CLKRST_Regs[0]->q8_top_reset_control |= (CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_MASK << (hwId*2U));
            } else {
                rstCtrl = CLKRST_Regs[0]->q8_top_reset_control;
                rstCtrl &= ~CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_MASK;
                rstCtrl |= (q8ResetControl.resetValue << CLKRST_Q8_TOP_RESET_CONTROL_APBSLAVE_RSTB_0_FRCVAL_SHIFT);
                CLKRST_Regs[0]->q8_top_reset_control = rstCtrl;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }
    return ret;
}

/** @brief A55 reset

    @code{.unparsed}
        ret = BCM_ERR_OK;
        rstCtrl = 0UL;
        if (hwId >= CLKRST_MAX_HW_ID)
            ret = BCM_ERR_INVAL_PARAMS;
        else if (a55ResetControl.resetValue > 2UL )
            ret = BCM_ERR_INVAL_PARAMS;
        else
            if (a55ResetControl.resetType == CLKRST_A55_RESET_HRESET)
                CLKRST_Regs[hwId]->a55_proc_reset_control |= (CLKRST_A55PRC_A55_PROC_HRESETN_FRC_MASK);
                if (a55ResetControl.resetValue == 2U)
                    CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_HRESETN_FRCVAL_MASK;
                    CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_HRESETN_FRCVAL_MASK;
                else
                    rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                    rstCtrl &= ~CLKRST_A55PRC_A55_PROC_HRESETN_FRCVAL_MASK;
                    rstCtrl |= a55ResetControl.resetValue;
                    CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            else if (a55ResetControl.resetType == CLKRST_A55_RESET_PRESET)
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_PRESETN_FRC_MASK;
                if (a55ResetControl.resetValue == 2U)
                    CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_MASK;
                    CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_MASK;
                else
                    rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                    rstCtrl &= ~CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_MASK;
                    rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_SHIFT;
                    CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            else if (a55ResetControl.resetType == CLKRST_A55_RESET_RESETN)
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRC_MASK;
                if (a55ResetControl.resetValue == 2U)
                    CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_MASK;
                    CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_MASK;
                else
                    rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                    rstCtrl &= ~CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_MASK;
                    rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_SHIFT;
                    CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            else if (a55ResetControl.resetType == CLKRST_A55_RESET_DEBUG_PRT_RESETN)
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_DAPRESETN_FRC_MASK;
                if (a55ResetControl.resetValue == 2U)
                    CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_MASK;
                    CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_MASK;
                else
                    rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                    rstCtrl &= ~CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_MASK;
                    rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_SHIFT;
                    CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            else if (a55ResetControl.resetType == CLKRST_A55_RESET_1588_TMR_RESET)
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRC_MASK;
                if (a55ResetControl.resetValue == 2U)
                    CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_MASK;
                    CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_MASK;
                else
                    rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                    rstCtrl &= ~CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_MASK;
                    rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_SHIFT;
                    CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            else
                ret = BCM_ERR_INVAL_PARAMS;
    @endcode
    @trace  #BRCM_SWARCH_CLKRST_A55_RESET_PROC
    @trace  #BRCM_SWREQ_CLKRST_CONFIG

    @limitations None
*/
int32_t CLKRST_A55Reset(CLKRST_HwIDType hwId,  CLKRST_A55ResetControlType a55ResetControl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t rstCtrl = 0UL;
    if (hwId >= CLKRST_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (a55ResetControl.resetValue > 2UL ) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        if (a55ResetControl.resetType == CLKRST_A55_RESET_HRESET) {
            CLKRST_Regs[hwId]->a55_proc_reset_control |= (CLKRST_A55PRC_A55_PROC_HRESETN_FRC_MASK);
            if (a55ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_HRESETN_FRCVAL_MASK;
                /* Reset deassertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_HRESETN_FRCVAL_MASK;
            } else {
                rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                rstCtrl &= ~CLKRST_A55PRC_A55_PROC_HRESETN_FRCVAL_MASK;
                /* Reset assertion/deassertion*/
                rstCtrl |= a55ResetControl.resetValue;
                CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            }
        } else if (a55ResetControl.resetType == CLKRST_A55_RESET_PRESET) {
            CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_PRESETN_FRC_MASK;
            if (a55ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_MASK;
                /* Reset deassertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_MASK;
            } else {
                /* Reset assertion/deassertion*/
                rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                rstCtrl &= ~CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_MASK;
                rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_PRESETN_FRCVAL_SHIFT;
                CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            }
        } else if (a55ResetControl.resetType == CLKRST_A55_RESET_RESETN) {
            CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRC_MASK;
            if (a55ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_MASK;
                /* Reset deassertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_MASK;
            } else {
                rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                rstCtrl &= ~CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_MASK;
                rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_ARM_RESETN_FRCVAL_SHIFT;
                CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            }
        } else if (a55ResetControl.resetType == CLKRST_A55_RESET_DEBUG_PRT_RESETN) {
            CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_DAPRESETN_FRC_MASK;
            if (a55ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_MASK;
                /* Reset deassertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_MASK;
            } else {
                rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                rstCtrl &= ~CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_MASK;
                rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_DAPRESETN_FRCVAL_SHIFT;
                CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            }
        } else if (a55ResetControl.resetType == CLKRST_A55_RESET_1588_TMR_RESET) {
            CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRC_MASK;
            if (a55ResetControl.resetValue == 2U) {
                /* Reset assertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control &= ~CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_MASK;
                /* Reset deassertion*/
                CLKRST_Regs[hwId]->a55_proc_reset_control |= CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_MASK;
            } else {
                rstCtrl = CLKRST_Regs[hwId]->a55_proc_reset_control;
                rstCtrl &= ~CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_MASK;
                rstCtrl |= a55ResetControl.resetValue << CLKRST_A55PRC_A55_PROC_ARM_1588_TIMER_RSTN_FRCVAL_SHIFT;
                CLKRST_Regs[hwId]->a55_proc_reset_control = rstCtrl;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }
    return ret;
}


/**
    @brief Validate clock configuration

    @trace  #BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC
    @trace  #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG
*/
static int CLKRST_IntSystemClockDivValidate(CLKRST_SystemClockControlType systemClockConfig)
{
    int32_t ret = BCM_ERR_OK;

    if ((systemClockConfig.clockType <= CLKRST_SYSTEM_CLOCK_CAN_PHY_CLK_DIV) &&
        (systemClockConfig.divRatio > 15U)) {
            ret = BCM_ERR_INVAL_PARAMS;
    } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_AHB_CLK_DIV) {
        if ((systemClockConfig.divRatio < 2U) || (systemClockConfig.divRatio > 4U)) {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_PERI_AHB_CLK_DIV) {
        if ((systemClockConfig.divRatio < 2U) || (systemClockConfig.divRatio > 4U)) {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_Q8_APB_CLK_DIV) {
        if ((systemClockConfig.divRatio < 2U) || (systemClockConfig.divRatio > 3U)) {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_CPU_CLK_M7_MCU1_DIV) {
        if (systemClockConfig.divRatio > 2U) {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;

}

/** @brief System clock  divider config

    @code{.unparsed}
    if (hwId >= CLKRST_MAX_HW_ID)
        ret = BCM_ERR_INVAL_PARAMS;
    else if (CLKRST_IntSystemClockDivValidate(systemClockConfig) != BCM_ERR_OK)
        ret = BCM_ERR_INVAL_PARAMS;
    else
        if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_CLK_DIV)
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set1 &= ~(SYSPLL_POST_DIVIDERS_6CH_SET1_CH0_MDIV_MASK);
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set1 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET1_CH0_MDIV_SHIFT);
        else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_CPU_CLK_DIV)
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set1 &= ~(SYSPLL_POST_DIVIDERS_6CH_SET1_CH1_MDIV_MASK);
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set1 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET1_CH1_MDIV_SHIFT);
        else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_Q8_CORE_CLK_DIV)
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set3 &= ~SYSPLL_POST_DIVIDERS_6CH_SET3_CH4_MDIV_MASK;
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set3 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET3_CH4_MDIV_SHIFT);
        else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_CAN_PHY_CLK_DIV)
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set3 &= ~SYSPLL_POST_DIVIDERS_6CH_SET3_CH5_MDIV_MASK;
            SYSPLL_Regs[hwId]->aux_post_dividers_6ch_set3 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET3_CH5_MDIV_SHIFT);
        else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_AHB_CLK_DIV)
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl &= ~CLKRST_RIGEL_TOP_CLK_CTRL_TOP_AHB_CK_DIV_RATIO_MASK;
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl |= (systemClockConfig.divRatio << CLKRST_RIGEL_TOP_CLK_CTRL_TOP_AHB_CK_DIV_RATIO_SHIFT);
        else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_PERI_AHB_CLK_DIV)
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl &= ~CLKRST_RIGEL_TOP_CLK_CTRL_LSP_AHB_CK_DIV_RATIO_MASK;
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl |= (systemClockConfig.divRatio << CLKRST_RIGEL_TOP_CLK_CTRL_LSP_AHB_CK_DIV_RATIO_SHIFT);
        else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_Q8_APB_CLK_DIV)
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl &= ~CLKRST_RIGEL_TOP_CLK_CTRL_Q8_PB_CK_DIV_RATIO_MASK;
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl |= (systemClockConfig.divRatio << CLKRST_RIGEL_TOP_CLK_CTRL_Q8_PB_CK_DIV_RATIO_SHIFT);
        else
            CPU_Regs[hwId]->misc_ctl &= ~CPU_MISC_CTL_UTILITY_MODE_MASK;
            CPU_Regs[hwId]->misc_ctl |= (((systemClockConfig.divRatio == 1UL) ? 0UL : 1UL) << (CPU_MISC_CTL_UTILITY_MODE_SHIFT));
    @endcode
    @trace  #BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC
    @trace  #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG

    @limitations None
*/

int32_t CLKRST_SystemClockDivConfig(CLKRST_HwIDType hwId, CLKRST_SystemClockControlType systemClockConfig)
{
    int32_t ret = BCM_ERR_OK;

    if (hwId >= CLKRST_MAX_HW_ID) {
        /*Validate HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CLKRST_IntSystemClockDivValidate(systemClockConfig) != BCM_ERR_OK) {
        /*Validate clock type and supported dividers*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_CLK_DIV) {
            /* SYSPLL clockout on channel 0*/
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set1 &= ~(SYSPLL_POST_DIVIDERS_6CH_SET1_CH0_MDIV_MASK);
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set1 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET1_CH0_MDIV_SHIFT);
        } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_CPU_CLK_DIV) {
            /* SYSPLL clockout on channel 1*/
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set1 &= ~(SYSPLL_POST_DIVIDERS_6CH_SET1_CH1_MDIV_MASK);
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set1 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET1_CH1_MDIV_SHIFT);
        } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_Q8_CORE_CLK_DIV) {
            /* SYSPLL clockout on channel 4*/
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set3 &= ~SYSPLL_POST_DIVIDERS_6CH_SET3_CH4_MDIV_MASK;
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set3 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET3_CH4_MDIV_SHIFT);
        } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_CAN_PHY_CLK_DIV) {
            /* SYSPLL clockout on channel 5*/
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set3 &= ~SYSPLL_POST_DIVIDERS_6CH_SET3_CH5_MDIV_MASK;
            CLKRST_SyspllRegs[hwId]->aux_post_dividers_6ch_set3 |= (systemClockConfig.divRatio << SYSPLL_POST_DIVIDERS_6CH_SET3_CH5_MDIV_SHIFT);
        } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_AHB_CLK_DIV) {
            /* Divider for AHB clock*/
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl &= ~CLKRST_RIGEL_TOP_CLK_CTRL_TOP_AHB_CK_DIV_RATIO_MASK;
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl |= (systemClockConfig.divRatio << CLKRST_RIGEL_TOP_CLK_CTRL_TOP_AHB_CK_DIV_RATIO_SHIFT);
        } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_PERI_AHB_CLK_DIV) {
            /* Divider for AHB peripheral clock*/
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl &= ~CLKRST_RIGEL_TOP_CLK_CTRL_LSP_AHB_CK_DIV_RATIO_MASK;
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl |= (systemClockConfig.divRatio << CLKRST_RIGEL_TOP_CLK_CTRL_LSP_AHB_CK_DIV_RATIO_SHIFT);
        } else if (systemClockConfig.clockType == CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_Q8_APB_CLK_DIV) {
            /* Divider for Q8 clock*/
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl &= ~CLKRST_RIGEL_TOP_CLK_CTRL_Q8_PB_CK_DIV_RATIO_MASK;
            CLKRST_Regs[hwId]->rigel_top_clk_ctrl |= (systemClockConfig.divRatio << CLKRST_RIGEL_TOP_CLK_CTRL_Q8_PB_CK_DIV_RATIO_SHIFT);
        } else {
            /* Divider for MCU1 */
            /* divRatio 1 -> use same clock as MCU0,
             * divRatio 2 -> use DIV2 ratio for MCU1*/
            CLKRST_CpuRegs[hwId]->misc_ctl &= ~CPU_MISC_CTL_UTILITY_MODE_MASK;
            CLKRST_CpuRegs[hwId]->misc_ctl |= (((systemClockConfig.divRatio == 1UL) ? 0UL : 1UL) << (CPU_MISC_CTL_UTILITY_MODE_SHIFT));
        }
    }
    return ret;
}
/** @} */
