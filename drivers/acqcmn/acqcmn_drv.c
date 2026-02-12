/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_acqcmndrv_impl ACQCMN Driver Design
    @ingroup grp_acqcmndrv

    @addtogroup grp_acqcmndrv_impl
    @{

    @file acqcmn_drv.c
    @brief ACQCMN Driver Design

    @version 0.1 Initial version
*/

#include <acqcmn.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <acqcmn_rdb.h>
#include <acqcmn_intr_rdb.h>

/**
    @name ACQCMN Driver Design IDs
    @{
    @brief Driver Design IDs for ACQCMN
*/

#define BRCM_SWDSGN_ACQCMN_DRV_DEV_TYPE                                (0xB640U)  /**< @brief #ACQCMN_DrvDevType           */
#define BRCM_SWDSGN_ACQCMN_DRV_DEV_GLOBAL                              (0xB641U)  /**< @brief #ACQCMN_DrvDev               */
#define BRCM_SWDSGN_ACQCMN_REGS_GLOBAL                                 (0xB642U)  /**< @brief #ACQCMN_Regs                 */
#define BRCM_SWDSGN_ACQCMN_INTR_REGS_GLOBAL                            (0xB643U)  /**< @brief #ACQCMN_IntrRegs             */

#define BRCM_SWDSGN_ACQCMN_DRV_INIT_PROC                               (0xB650U)  /**< @brief #ACQCMN_DrvInit              */
#define BRCM_SWDSGN_ACQCMN_DRV_DEINIT_PROC                             (0xB651U)  /**< @brief #ACQCMN_DrvDeInit            */
#define BRCM_SWDSGN_ACQCMN_DRV_RESET_PROC                              (0xB652U)  /**< @brief #ACQCMN_DrvReset             */
#define BRCM_SWDSGN_ACQCMN_DRV_CLK_CTRL_PROC                           (0xB653U)  /**< @brief #ACQCMN_DrvClkCtrl           */
#define BRCM_SWDSGN_ACQCMN_DRV_AXIS_RES_SET_PROC                       (0xB654U)  /**< @brief #ACQCMN_DrvAxisResSet        */
#define BRCM_SWDSGN_ACQCMN_DRV_AXIS_CFG_SET_PROC                       (0xB655U)  /**< @brief #ACQCMN_DrvAxisCfgSet        */
#define BRCM_SWDSGN_ACQCMN_DRV_AXIS_CFG_GET_PROC                       (0xB656U)  /**< @brief #ACQCMN_DrvAxisCfgGet        */
#define BRCM_SWDSGN_ACQCMN_DRV_ENABLE_INTERRUPT_PROC                   (0xB657U)  /**< @brief #ACQCMN_DrvEnableInterrupt   */
#define BRCM_SWDSGN_ACQCMN_DRV_INTERRUPT_STATUS_PROC                   (0xB658U)  /**< @brief #ACQCMN_DrvInterruptStatus   */
#define BRCM_SWDSGN_ACQCMN_DRV_EVENT_STATUS_PROC                       (0xB659U)  /**< @brief #ACQCMN_DrvEventStatus       */
#define BRCM_SWDSGN_ACQCMN_DRV_CLEAR_INTERRUPT_PROC                    (0xB65AU)  /**< @brief #ACQCMN_DrvClearInterrupt    */

#define BRCM_SWDSGN_ACQCMN_INT_ENABLE_NSU_GEN_EVENT_GRP0_INTR_PROC     (0xB65BU)  /**< @brief #ACQCMN_IntEnableNsuGenEventGrp0Intr    */
#define BRCM_SWDSGN_ACQCMN_INT_ENABLE_NSU_GEN_EVENT_GRP1_INTR_PROC     (0xB65CU)  /**< @brief #ACQCMN_IntEnableNsuGenEventGrp1Intr    */
#define BRCM_SWDSGN_ACQCMN_INT_ENABLE_NSU_TS_CAP_EVENT_GRP0_INTR_PROC  (0xB65DU)  /**< @brief #ACQCMN_IntEnableNsuTsCapEventGrp0Intr  */
#define BRCM_SWDSGN_ACQCMN_INT_ENABLE_NSU_TS_CAP_EVENT_GRP1_INTR_PROC  (0xB65EU)  /**< @brief #ACQCMN_IntEnableNsuTsCapEventGrp1Intr  */
#define BRCM_SWDSGN_ACQCMN_INT_ENABLE_NSU_SYSTICK_GRP0_INTR_PROC       (0xB65FU)  /**< @brief #ACQCMN_IntEnableNsuSystickGrp0Intr     */
#define BRCM_SWDSGN_ACQCMN_INT_NSU_GEN_EVENT_GRP0_INTR_STATUS_PROC     (0xB660U)  /**< @brief #ACQCMN_IntNsuGenEventGrp0IntrStatus    */
#define BRCM_SWDSGN_ACQCMN_INT_NSU_GEN_EVENT_GRP1_INTR_STATUS_PROC     (0xB661U)  /**< @brief #ACQCMN_IntNsuGenEventGrp1IntrStatus    */
#define BRCM_SWDSGN_ACQCMN_INT_NSU_TS_CAP_EVENT_GRP0_INTR_STATUS_PROC  (0xB662U)  /**< @brief #ACQCMN_IntNsuTsCapEventGrp0IntrStatus  */
#define BRCM_SWDSGN_ACQCMN_INT_NSU_TS_CAP_EVENT_GRP1_INTR_STATUS_PROC  (0xB663U)  /**< @brief #ACQCMN_IntNsuTsCapEventGrp1IntrStatus  */
#define BRCM_SWDSGN_ACQCMN_INT_NSU_SYSTICK_GRP0_INTR_STATUS_PROC       (0xB664U)  /**< @brief #ACQCMN_IntNsuSystickGrp0IntrStatus     */
#define BRCM_SWDSGN_ACQCMN_INT_INTR_STATUS_PROC                        (0xB665U)  /**< @brief #ACQCMN_IntIntrStatus                   */
#define BRCM_SWDSGN_ACQCMN_INT_CLEAR_NSU_GEN_EVENT_GRP0_INTR_PROC      (0xB666U)  /**< @brief #ACQCMN_IntClearNsuGenEventGrp0Intr     */
#define BRCM_SWDSGN_ACQCMN_INT_CLEAR_NSU_GEN_EVENT_GRP1_INTR_PROC      (0xB667U)  /**< @brief #ACQCMN_IntClearNsuGenEventGrp1Intr     */
#define BRCM_SWDSGN_ACQCMN_INT_CLEAR_NSU_TS_CAP_EVENT_GRP0_INTR_PROC   (0xB668U)  /**< @brief #ACQCMN_IntClearNsuTsCapEventGrp0Intr   */
#define BRCM_SWDSGN_ACQCMN_INT_CLEAR_NSU_TS_CAP_EVENT_GRP1_INTR_PROC   (0xB669U)  /**< @brief #ACQCMN_IntClearNsuTsCapEventGrp1Intr   */
#define BRCM_SWDSGN_ACQCMN_INT_CLEAR_NSU_SYSTICK_GRP0_INTR_PROC        (0xB66AU)  /**< @brief #ACQCMN_IntClearNsuSystickGrp0Intr      */
#define BRCM_SWDSGN_ACQCMN_DRV_SRC_CONTROL_PROC                        (0xB66BU)  /**< @brief #ACQCMN_DrvSrcControl                   */
#define BRCM_SWDSGN_ACQCMN_INT_SRC_CONTROL_NSU_CNTR_HW_RST_PROC        (0xB66CU)  /**< @brief #ACQCMN_IntSrcControlNsuCntrHwRst       */
#define BRCM_SWDSGN_ACQCMN_INT_SRC_CONTROL_CCU_TMR_PROC                (0xB66DU)  /**< @brief #ACQCMN_IntSrcControlCcuTmr       */
#define BRCM_SWDSGN_ACQCMN_INT_EVENT_SRC_CONTROL_CCU_TMR_PROC          (0xB66EU)  /**< @brief #ACQCMN_IntEventSrcControlCcuTmr */

/** @} */

/**
    @trace #BRCM_SWARCH_ACQCMN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_RESET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_RES_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_GET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INIT
*/
static ACQCMN_RDBType *const ACQCMN_Regs[ACQCMN_MAX_HW_ID] = {
#if (ACQCMN_MAX_HW_ID == 0UL)
#error "ACQCMN_MAX_HW_ID == 0"
#endif
    (ACQCMN_RDBType *) ACQ_COMMON_BASE,
#if (ACQCMN_MAX_HW_ID > 1UL)
#error "ACQCMN_MAX_HW_ID > 1 is not supported by the driver"
#endif /* ACQCMN_MAX_HW_ID > 1 */
};

/**
    @trace #BRCM_SWARCH_ACQCMN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_RESET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_RES_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_GET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INIT
*/
static ACQCMN_INTR_RDBType * const ACQCMN_IntrRegs[ACQCMN_INTR_MAX_HW_ID] =
{
#if (ACQCMN_INTR_MAX_HW_ID == 0UL)
#error "ACQCMN_MAX_HW_ID == 0U"
#endif
    (ACQCMN_INTR_RDBType *)ACQ_COMMON_INTR_BASE,
#if (ACQCMN_INTR_MAX_HW_ID > 1UL)
#error "ACQCMN_MAX_HW_ID > 1 is not supported by the driver"
#endif /* ACQCMN_INTR_MAX_HW_ID > 1 */
};

/**
    @name ACQCMN Driver device type
    @{
    @brief ACQCMN Driver device type structure

    @trace #BRCM_SWARCH_ACQCMN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_RESET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_RES_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_GET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INIT
 */
typedef struct sACQCMN_DrvDevType {
    ACQCMN_StateType   state; /**< @brief State of the driver */
} ACQCMN_DrvDevType;
/** @} */

/**
    @name ACQCMN Driver device type
    @{
    @brief ACQCMN Driver device type structure

    @trace #BRCM_SWARCH_ACQCMN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_RESET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_RES_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_SET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_CFG_GET_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INIT
 */
static ACQCMN_DrvDevType ACQCMN_DrvDev[ACQCMN_MAX_HW_ID] COMP_SECTION(".bss.drivers");
/** @} */

/**
    @brief ACQCMN source control configuration for NSU counter Hardware Reset

    @trace #BRCM_SWARCH_ACQCMN_DRV_SRC_CONTROL_PROC
    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
*/
static int32_t ACQCMN_IntSrcControlNsuCntrHwRst(ACQCMN_HwIDType hwId, ACQCMN_SrcSelEventType nsuCntrHwRstEvent, ACQCMN_SrcCtrlType *srcCtrl)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t srcCtrlData = srcCtrl->srcCtrlData ;
    uint8_t srcSignal   = srcCtrl->srcSignal ;

    if(0UL != (nsuCntrHwRstEvent & ACQCMN_SRC_SEL_EVENT0_NSU)) {
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT0_GPIO_MASK ;
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl |= ((uint32_t)srcCtrlData & ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT0_GPIO_MASK) ;
        if(0U == srcSignal) {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT0_GPIO_EN_MASK ;
        } else {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel |=  ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT0_GPIO_EN_MASK ;
        }
    }
    if(0UL != (nsuCntrHwRstEvent & ACQCMN_SRC_SEL_EVENT1_NSU)) {
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT1_GPIO_MASK ;
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl |= (((uint32_t)srcCtrlData << ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT1_GPIO_SHIFT) &
                                                                                  ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT1_GPIO_MASK) ;
        if(0U == srcSignal) {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT1_GPIO_EN_MASK ;
        } else {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel |=  ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT1_GPIO_EN_MASK ;
        }
    }
    if(0UL != (nsuCntrHwRstEvent & ACQCMN_SRC_SEL_EVENT2_NSU)) {
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT2_GPIO_MASK ;
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl |= (((uint32_t)srcCtrlData << ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT2_GPIO_SHIFT) &
                                                                                  ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT2_GPIO_MASK) ;
        if(0U == srcSignal) {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT2_GPIO_EN_MASK ;
        } else {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel |=  ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT2_GPIO_EN_MASK ;
        }
    }
    if(0UL != (nsuCntrHwRstEvent & ACQCMN_SRC_SEL_EVENT3_NSU)) {
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT3_GPIO_MASK ;
        ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_ctrl |= (((uint32_t)srcCtrlData << ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT3_GPIO_SHIFT) &
                                                                                  ACQCMN_NSU_CNTR_HW_RST_SRC_CTRL_EVENT3_GPIO_MASK) ;
        if(0U == srcSignal) {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel &= ~ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT3_GPIO_EN_MASK ;
        } else {
            ACQCMN_Regs[hwId]->nsu_cntr_hw_rst_src_sel |=  ACQCMN_NSU_CNTR_HW_RST_SRC_SEL_EVENT3_GPIO_EN_MASK ;
        }
    }
    return ret ;
}

/**
    @brief ACQCMN source control configuration for CCU TMR event source control.

    @trace #BRCM_SWARCH_ACQCMN_DRV_SRC_CONTROL_PROC
    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
*/
static void ACQCMN_IntEventSrcControlCcuTmr(ACQCMN_HwIDType hwId, ACQCMN_SrcSelEventType ccuCtrlEvent, ACQCMN_SrcCtrlType *srcCtrl,uint32_t blkType)
{
    uint8_t srcCtrlData = srcCtrl->srcCtrlData ;
    uint8_t srcSignal   = srcCtrl->srcSignal ;
    uint32_t offset = 0UL;
    uint32_t *pSrcCtrl = NULL;
    uint32_t *pSrcSel = NULL;

    if(blkType == ACQCMN_SRC_SEL_BLK_CCU_TMR1){
        offset = 0x80UL;
    } else {
        offset = 0UL;
    }

    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN0)) {

        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event0_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event0_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_EVENT0_SRC_CTRL_NSU_TRG0_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_EVENT0_SRC_CTRL_NSU_TRG0_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_EVENT0_SRC_SEL_NSU_TRG0_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_EVENT0_SRC_SEL_NSU_TRG0_EN_MASK ;
        }
    }
    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN1)) {

        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event1_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event1_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_EVENT1_SRC_CTRL_NSU_TRG1_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_EVENT1_SRC_CTRL_NSU_TRG1_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_EVENT1_SRC_SEL_NSU_TRG1_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_EVENT1_SRC_SEL_NSU_TRG1_EN_MASK ;
        }
    }
    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN2)) {
        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event2_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event2_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_EVENT2_SRC_CTRL_ACQ_TRG_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_EVENT2_SRC_CTRL_ACQ_TRG_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_EVENT2_SRC_SEL_ACQ_TRG_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_EVENT2_SRC_SEL_ACQ_TRG_EN_MASK ;
        }
    }

    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_EVENT_IN3)) {
        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event3_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_event3_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_EVENT3_SRC_CTRL_EVENT_GPIO_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_EVENT3_SRC_CTRL_EVENT_GPIO_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_EVENT3_SRC_SEL_EVENT_GPIO_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_EVENT3_SRC_SEL_EVENT_GPIO_EN_MASK ;
        }
    }
}


/**
    @brief ACQCMN source control configuration for CCU TMR source control.

    @trace #BRCM_SWARCH_ACQCMN_DRV_SRC_CONTROL_PROC
    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
*/
static int32_t ACQCMN_IntSrcControlCcuTmr(ACQCMN_HwIDType hwId, ACQCMN_SrcSelEventType ccuCtrlEvent, ACQCMN_SrcCtrlType *srcCtrl,uint32_t blkType)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t srcCtrlData = srcCtrl->srcCtrlData ;
    uint8_t srcSignal   = srcCtrl->srcSignal ;
    uint32_t offset = 0UL;
    uint32_t *pSrcCtrl = NULL;
    uint32_t *pSrcSel = NULL;

    if(blkType == ACQCMN_SRC_SEL_BLK_CCU_TMR1){
        offset = 0x80UL;
    } else {
        offset = 0UL;
    }

    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_ETR)) {

        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_etr_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_etr_src_sel  + offset));
        *pSrcCtrl &= ~ACQCMN_TMR0_ETR_SRC_CTRL_GPIO_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_ETR_SRC_CTRL_GPIO_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_ETR_SRC_SEL_GPIO_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_ETR_SRC_SEL_GPIO_EN_MASK ;
        }
    }
    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN0)) {

        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in0_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in0_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_IN0_SRC_CTRL_GPIO_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_IN0_SRC_CTRL_GPIO_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_IN0_SRC_SEL_GPIO_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_IN0_SRC_SEL_GPIO_EN_MASK ;
        }
    }
    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN1)) {

        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in1_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in1_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_IN1_SRC_CTRL_GPIO_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_IN1_SRC_CTRL_GPIO_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_IN1_SRC_SEL_GPIO_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_IN1_SRC_SEL_GPIO_EN_MASK ;
        }
    }
    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN2)) {

        pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in2_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in2_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_IN2_SRC_CTRL_GPIO_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_IN2_SRC_CTRL_GPIO_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_IN2_SRC_SEL_GPIO_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_IN2_SRC_SEL_GPIO_EN_MASK ;
        }
    }

    if(0UL != (ccuCtrlEvent & ACQCMN_SRC_SEL_EVENT_CCU_TMR_IN3)) {
		pSrcCtrl = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in3_src_ctrl + offset));
        pSrcSel  = (uint32_t *)((uintptr_t)((uint32_t)&ACQCMN_Regs[hwId]->tmr0_in3_src_sel  + offset));

        *pSrcCtrl &= ~ACQCMN_TMR0_IN3_SRC_CTRL_GPIO_MASK ;
        *pSrcCtrl |= ((uint32_t)srcCtrlData << ACQCMN_TMR0_IN3_SRC_CTRL_GPIO_SHIFT) ;
        if(0U == srcSignal) {
            *pSrcSel &= ~ACQCMN_TMR0_IN3_SRC_SEL_GPIO_EN_MASK ;
        } else {
            *pSrcSel |=  ACQCMN_TMR0_IN3_SRC_SEL_GPIO_EN_MASK ;
        }
    }
    /* Event control settings. */
    if(0UL != (ccuCtrlEvent & 0x1E00UL)) {
        ACQCMN_IntEventSrcControlCcuTmr(hwId, ccuCtrlEvent, srcCtrl, blkType);
    }
    return ret ;
}

/**
    @brief Enable NSU generate event group0 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntEnableNsuGenEventGrp0Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH0)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT0_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT0_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH1)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT1_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT1_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH2)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT2_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT2_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH3)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT3_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT3_INTR_MASK ;
        }
    }
    return ret;
}

/**
    @brief Enable NSU generate event group1 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntEnableNsuGenEventGrp1Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH4)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT4_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT4_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH5)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT5_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT5_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH6)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT6_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT6_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH7)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT7_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_GENEVT7_INTR_MASK ;
        }
    }
    return ret;
}

/**
    @brief Enable NSU time stamp capture event group0 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntEnableNsuTsCapEventGrp0Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH0)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT0_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT0_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH1)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT1_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT1_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH2)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT2_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT2_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH3)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT3_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT3_INTR_MASK ;
        }
    }
    return ret;
}

/**
    @brief Enable NSU time stamp capture event group1 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntEnableNsuTsCapEventGrp1Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH4)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT4_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT4_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH5)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT5_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT5_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH6)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT6_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT6_INTR_MASK ;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH7)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier &= ~ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT7_INTR_MASK ;
        } else {
            ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eier |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EIER_ENABLE_CAPEVT7_INTR_MASK ;
        }
    }
    return ret;
}

/**
    @brief Enable NSU systick group0 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntEnableNsuSystickGrp0Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK0)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->systick[0UL].acq_nsu_systick0_intr_eier &= ~ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        } else {
            ACQCMN_IntrRegs[hwId]->systick[0UL].acq_nsu_systick0_intr_eier |=  ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK1)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->systick[1UL].acq_nsu_systick0_intr_eier &= ~ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        } else {
            ACQCMN_IntrRegs[hwId]->systick[1UL].acq_nsu_systick0_intr_eier |=  ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK2)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->systick[2UL].acq_nsu_systick0_intr_eier &= ~ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        } else {
            ACQCMN_IntrRegs[hwId]->systick[2UL].acq_nsu_systick0_intr_eier |=  ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        }
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK3)) {
        if(0UL == aEnable) {
            ACQCMN_IntrRegs[hwId]->systick[3UL].acq_nsu_systick0_intr_eier &= ~ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        } else {
            ACQCMN_IntrRegs[hwId]->systick[3UL].acq_nsu_systick0_intr_eier |=  ACQCMN_INTR_ANS0IE2_ENABLE_ACQ_SYSTICK0_MASK;
        }
    }
    return ret;
}

/**
    @brief Get ACQCMN NSU generate event group0 interrupt/event status

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntNsuGenEventGrp0IntrStatus(ACQCMN_HwIDType hwId, uint32_t event_intr, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;

    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT0_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH0 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT1_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH1 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT2_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH2 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT3_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH3 ;
    }
    return ret;
}

/**
    @brief Get ACQCMN NSU generate event group1 interrupt/event status

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntNsuGenEventGrp1IntrStatus(ACQCMN_HwIDType hwId, uint32_t event_intr, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;

    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT4_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH4 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT5_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH5 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT6_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH6 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT7_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH7 ;
    }
    return ret;
}

/**
    @brief Get ACQCMN NSU timestamp capture event group0 interrupt/event status

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntNsuTsCapEventGrp0IntrStatus(ACQCMN_HwIDType hwId, uint32_t event_intr, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;

    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT0_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH0 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT1_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH1 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT2_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH2 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT3_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH3 ;
    }
    return ret;
}

/**
    @brief Get ACQCMN NSU timestamp capture event group1 interrupt/event status

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntNsuTsCapEventGrp1IntrStatus(ACQCMN_HwIDType hwId, uint32_t event_intr, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;

    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT4_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH4 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT5_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH5 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT6_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH6 ;
    }
    if(0UL != (event_intr & ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT7_INTR_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH7 ;
    }
    return ret;
}

/**
    @brief Get ACQCMN NSU systick group0 interrupt/event status

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntNsuSystickGrp0IntrStatus(ACQCMN_HwIDType hwId, uint8_t event, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acq_nsu_systick_event_intr[ACQCMN_INTR_SYSTICK_SIZE] = {0UL, 0UL, 0UL, 0UL};
    uint32_t idx = 0UL;

    for(idx=0UL; idx < ACQCMN_INTR_SYSTICK_SIZE; idx++) {
        if(0U == event) {
            acq_nsu_systick_event_intr[idx] = ACQCMN_IntrRegs[hwId]->systick[idx].acq_nsu_systick0_intr_eipr ;
        } else {
            acq_nsu_systick_event_intr[idx] = ACQCMN_IntrRegs[hwId]->systick[idx].acq_nsu_systick0_intr_eisr ;
        }
    }

    if(0UL != (acq_nsu_systick_event_intr[0UL] & ACQCMN_INTR_ANS0IE1_ACQ_SYSTICK0_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_SYSTICK0 ;
    }

    if(0UL != (acq_nsu_systick_event_intr[1UL] & ACQCMN_INTR_ANS1IE1_ACQ_SYSTICK1_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_SYSTICK1 ;
    }

    if(0UL != (acq_nsu_systick_event_intr[2UL] & ACQCMN_INTR_ANS2IE1_ACQ_SYSTICK2_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_SYSTICK2 ;
    }

    if(0UL != (acq_nsu_systick_event_intr[3UL] & ACQCMN_INTR_ANS3IE1_ACQ_SYSTICK3_W2CLR_MASK)) {
        *intrSts |= ACQCMN_INTERRUPT_NSU_SYSTICK3 ;
    }
    return ret;
}

/**
    @brief Get ACQCMN interrupt/event status

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntIntrStatus(ACQCMN_HwIDType hwId, uint8_t event, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acq_nsu_event_intr = 0UL;

    *intrSts = 0UL;
    if(0U == event) {
        acq_nsu_event_intr = ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eipr ;
    } else {
        acq_nsu_event_intr = ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eisr ;
    }

    ret = ACQCMN_IntNsuGenEventGrp0IntrStatus(hwId, acq_nsu_event_intr, intrSts);

    if(BCM_ERR_OK == ret) {
        ret = ACQCMN_IntNsuGenEventGrp1IntrStatus(hwId, acq_nsu_event_intr, intrSts);
    }

    if(BCM_ERR_OK == ret) {
        ret = ACQCMN_IntNsuTsCapEventGrp0IntrStatus(hwId, acq_nsu_event_intr, intrSts);
    }

    if(BCM_ERR_OK == ret) {
        ret = ACQCMN_IntNsuTsCapEventGrp1IntrStatus(hwId, acq_nsu_event_intr, intrSts);
    }

    if(BCM_ERR_OK == ret) {
        ret = ACQCMN_IntNsuSystickGrp0IntrStatus(hwId, event, intrSts);
    }
    return ret;
}

/**
    @brief Clear NSU generate event group0 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntClearNsuGenEventGrp0Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_TYPE acq_nsu_evt_intr_eisr = 0UL ;

    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH0)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT0_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH1)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT1_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH2)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT2_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH3)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT3_INTR_W2CLR_MASK ;
    }
    ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eisr |= acq_nsu_evt_intr_eisr ;
    return ret;
}

/**
    @brief Clear NSU generate event group1 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntClearNsuGenEventGrp1Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_TYPE acq_nsu_evt_intr_eisr = 0UL ;

    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH4)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT4_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH5)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT5_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH6)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT6_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_CH7)) {
        acq_nsu_evt_intr_eisr |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_GENEVT7_INTR_W2CLR_MASK ;
    }
    ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eisr |= acq_nsu_evt_intr_eisr ;
    return ret;
}

/**
    @brief Clear NSU time stamp capture event group0 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntClearNsuTsCapEventGrp0Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_TYPE acq_nsu_evt_intr_eisr = 0UL ;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH0)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT0_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH1)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT1_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH2)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT2_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH3)) {
        acq_nsu_evt_intr_eisr |= ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT3_INTR_W2CLR_MASK ;
    }
    ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eisr |= acq_nsu_evt_intr_eisr ;
    return ret;
}

/**
    @brief Clear NSU time stamp capture event group1 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntClearNsuTsCapEventGrp1Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_TYPE acq_nsu_evt_intr_eisr = 0UL ;
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH4)) {
        acq_nsu_evt_intr_eisr |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT4_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH5)) {
        acq_nsu_evt_intr_eisr |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT5_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH6)) {
        acq_nsu_evt_intr_eisr |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT6_INTR_W2CLR_MASK ;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_CH7)) {
        acq_nsu_evt_intr_eisr |=  ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_CAPEVT7_INTR_W2CLR_MASK ;
    }
    ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eisr |= acq_nsu_evt_intr_eisr ;
    return ret;
}

/**
    @brief Clear NSU systick group0 interrupts

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
static int32_t ACQCMN_IntClearNsuSystickGrp0Intr(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acq_nsu_systick_event_intr[ACQCMN_INTR_SYSTICK_SIZE] = {0UL, 0UL, 0UL, 0UL};
    uint32_t idx = 0UL;

    for(idx=0UL; idx < ACQCMN_INTR_SYSTICK_SIZE; idx++) {
        acq_nsu_systick_event_intr[idx] = ACQCMN_IntrRegs[hwId]->systick[idx].acq_nsu_systick0_intr_eisr ;
        (void)acq_nsu_systick_event_intr[idx];
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK0)) {
        ACQCMN_IntrRegs[hwId]->systick[0UL].acq_nsu_systick0_intr_eisr |= ACQCMN_INTR_ANS0IE1_ACQ_SYSTICK0_W2CLR_MASK;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK1)) {
        ACQCMN_IntrRegs[hwId]->systick[1UL].acq_nsu_systick0_intr_eisr |= ACQCMN_INTR_ANS1IE1_ACQ_SYSTICK1_W2CLR_MASK;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK2)) {
        ACQCMN_IntrRegs[hwId]->systick[2UL].acq_nsu_systick0_intr_eisr |= ACQCMN_INTR_ANS2IE1_ACQ_SYSTICK2_W2CLR_MASK;
    }
    if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK3)) {
        ACQCMN_IntrRegs[hwId]->systick[3UL].acq_nsu_systick0_intr_eisr |= ACQCMN_INTR_ANS3IE1_ACQ_SYSTICK3_W2CLR_MASK;
    }
    return ret;
}

/**
    @brief initializes ACQCMN driver

    @code{.unparsed}
    Check hardware instance with max supported device.
    Move state to ACQCMN_STATE_INITIALIZED state.
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_INIT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INIT
*/
int32_t ACQCMN_DrvInit(ACQCMN_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID || (ACQCMN_DrvDev[hwId].state != ACQCMN_STATE_UNINIT)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ACQCMN_DrvDev[hwId].state = ACQCMN_STATE_INITIALIZED;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief De-initializes ACQCMN driver

    @code{.unparsed}
    Check hardware instance with max supported device.
    Move state to ACQCMN_STATE_UNINIT.
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INIT
*/
int32_t ACQCMN_DrvDeInit(ACQCMN_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ACQCMN_DrvDev[hwId].state = ACQCMN_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief ACQCMN reset control

    @code{.unparsed}
    Check hardware instance with max supported device.
    Perform reset based on reset type
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_RESET_PROC
    @trace #BRCM_SWREQ_ACQCMN_RESET_CONTROL
*/
int32_t ACQCMN_DrvReset(ACQCMN_HwIDType hwId, ACQCMN_ResetType resetType)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ACQCMN_Regs[hwId]->global_rst_en |=  ACQCMN_GLOBAL_RST_EN_MASK ;
        ACQCMN_Regs[hwId]->local_rst_en  |=  resetType ;
        BCM_CpuNDelay(200UL);
        ACQCMN_Regs[hwId]->global_rst_en &= ~ACQCMN_GLOBAL_RST_EN_MASK ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief ACQCMN clock control

    @code{.unparsed}
    Check hardware instance with max supported device.
    Enable/Disable clock based on clock type
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWREQ_ACQCMN_CLOCK_CONTROL
*/
int32_t ACQCMN_DrvClkCtrl(ACQCMN_HwIDType hwId, ACQCMN_ClkType clkType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL == aEnable) {
            ACQCMN_Regs[hwId]->clk_enb |=  clkType ;
        } else {
            ACQCMN_Regs[hwId]->clk_enb &= ~clkType ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Sets the resolution and range from nsu_bin_cntr

    @code{.unparsed}
    Check hardware instance with max supported device.
    Sets the resolution and range from nsu_bin_cntr
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_AXIS_RES_SET_PROC
    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
*/
int32_t ACQCMN_DrvAxisResSet(ACQCMN_HwIDType hwId, ACQCMN_AxisResolutionType res)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(ACQCMN_AXIS_RESOLUTION_640NS == res) {
            ACQCMN_Regs[hwId]->axis_cmn_ctrl &= ~ACQCMN_AXIS_CMN_CTRL_NSU_BIN_CNTR_RES_MASK ;
        } else {
            ACQCMN_Regs[hwId]->axis_cmn_ctrl |=  ACQCMN_AXIS_CMN_CTRL_NSU_BIN_CNTR_RES_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set Axis configuration

    @code{.unparsed}
    Check hardware instance with max supported device.
    Sets Axis configuration based on axis instance
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
*/
int32_t ACQCMN_DrvAxisCfgSet(ACQCMN_HwIDType hwId, ACQCMN_AxisIdType axId, ACQCMN_AxisCfgType *axisCfg)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_AXIS_CTRL0_TYPE axis_ctrl0 = 0UL;
    ACQCMN_AXIS_CTRL1_TYPE axis_ctrl1 = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        axis_ctrl0 = (((axisCfg->awgMux        << ACQCMN_AXIS_CTRL0_AWG_MUX_SHIFT)            & ACQCMN_AXIS_CTRL0_AWG_MUX_MASK)            |
                      ((axisCfg->deglitchEnB   << ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_B_SHIFT) & ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_B_MASK) |
                      ((axisCfg->deglitchEnA   << ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_A_SHIFT) & ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_A_MASK) |
                      ((axisCfg->deglitchEnZ   << ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_Z_SHIFT) & ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_Z_MASK) |
                      ((axisCfg->deglitchWidth << ACQCMN_AXIS_CTRL0_DEGLITCH_WIDTH_SHIFT)     & ACQCMN_AXIS_CTRL0_DEGLITCH_WIDTH_MASK)
                     );
        axis_ctrl1 = (((axisCfg->edgeSel       << ACQCMN_AXIS_CTRL1_EDGE_SEL_SHIFT)           & ACQCMN_AXIS_CTRL1_EDGE_SEL_MASK)           |
                      ((axisCfg->sensorSel     << ACQCMN_AXIS_CTRL1_SENSOR_SEL_SHIFT)         & ACQCMN_AXIS_CTRL1_SENSOR_SEL_MASK)         |
                      ((axisCfg->prescalerEn   << ACQCMN_AXIS_CTRL1_PRESCALER_EN_SHIFT)       & ACQCMN_AXIS_CTRL1_PRESCALER_EN_MASK)       |
                      ((axisCfg->prescalerVal  << ACQCMN_AXIS_CTRL1_PRESCALER_VAL_SHIFT)      & ACQCMN_AXIS_CTRL1_PRESCALER_VAL_MASK)
                     );

        ACQCMN_Regs[hwId]->axis[axId].axis0_ctrl0 = axis_ctrl0 ;
        ACQCMN_Regs[hwId]->axis[axId].axis0_ctrl1 = axis_ctrl1 ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Gets Axis configuration

    @code{.unparsed}
    Check hardware instance with max supported device.
    Gets Axis configuration based on axis instance
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLK_CTRL_PROC
    @trace #BRCM_SWREQ_ACQCMN_AXIS_CONFIG
*/
int32_t ACQCMN_DrvAxisCfgGet(ACQCMN_HwIDType hwId, ACQCMN_AxisIdType axId, ACQCMN_AxisCfgType *axisCfg)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_AXIS_CTRL0_TYPE axis_ctrl0 = 0UL;
    ACQCMN_AXIS_CTRL1_TYPE axis_ctrl1 = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == axisCfg) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        axis_ctrl0 = ACQCMN_Regs[hwId]->axis[axId].axis0_ctrl0 ;
        axis_ctrl1 = ACQCMN_Regs[hwId]->axis[axId].axis0_ctrl1 ;

        axisCfg->awgMux        = ((axis_ctrl0 & ACQCMN_AXIS_CTRL0_AWG_MUX_MASK)            >> ACQCMN_AXIS_CTRL0_AWG_MUX_SHIFT            );
        axisCfg->deglitchEnB   = ((axis_ctrl0 & ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_B_MASK) >> ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_B_SHIFT );
        axisCfg->deglitchEnA   = ((axis_ctrl0 & ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_A_MASK) >> ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_A_SHIFT );
        axisCfg->deglitchEnZ   = ((axis_ctrl0 & ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_Z_MASK) >> ACQCMN_AXIS_CTRL0_DEGLITCH_EN_GPIO_Z_SHIFT );
        axisCfg->deglitchWidth = ((axis_ctrl0 & ACQCMN_AXIS_CTRL0_DEGLITCH_WIDTH_MASK)     >> ACQCMN_AXIS_CTRL0_DEGLITCH_WIDTH_SHIFT     );

        axisCfg->edgeSel       = ((axis_ctrl1 & ACQCMN_AXIS_CTRL1_EDGE_SEL_MASK)           >> ACQCMN_AXIS_CTRL1_EDGE_SEL_SHIFT           );
        axisCfg->sensorSel     = ((axis_ctrl1 & ACQCMN_AXIS_CTRL1_SENSOR_SEL_MASK)         >> ACQCMN_AXIS_CTRL1_SENSOR_SEL_SHIFT         );
        axisCfg->prescalerEn   = ((axis_ctrl1 & ACQCMN_AXIS_CTRL1_PRESCALER_EN_MASK)       >> ACQCMN_AXIS_CTRL1_PRESCALER_EN_SHIFT       );
        axisCfg->prescalerVal  = ((axis_ctrl1 & ACQCMN_AXIS_CTRL1_PRESCALER_VAL_MASK)      >> ACQCMN_AXIS_CTRL1_PRESCALER_VAL_SHIFT      );

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief ACQCMN Source control configuration

    @code{.unparsed}
    Perform source selection based on block type
    Write the source control configuration data
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_SRC_CONTROL_PROC
    @trace #BRCM_SWREQ_ACQCMN_SRC_CTRL_CFG
*/
int32_t ACQCMN_DrvSrcControl(ACQCMN_HwIDType hwId, ACQCMN_SrcSelType *srcSel, ACQCMN_SrcCtrlType *srcCtrl)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if((NULL == srcSel) || (NULL == srcCtrl)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(ACQCMN_SRC_SEL_BLK_NSU_CNTR_HW_RST == srcSel->blkType) {
            ret = ACQCMN_IntSrcControlNsuCntrHwRst(hwId, srcSel->event, srcCtrl);
        }
        if((BCM_ERR_OK == ret) && ((ACQCMN_SRC_SEL_BLK_CCU_TMR0 == srcSel->blkType) || (ACQCMN_SRC_SEL_BLK_CCU_TMR1 == srcSel->blkType))) {
            ret = ACQCMN_IntSrcControlCcuTmr(hwId, srcSel->event, srcCtrl, srcSel->blkType);
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief ACQCMN enable interrupts

    @code{.unparsed}
    Check hardware instance with max supported device.
    Enable interrupts based on channel number and systick index..
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
int32_t ACQCMN_DrvEnableInterrupt(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_GRP0)) {
            ret = ACQCMN_IntEnableNsuGenEventGrp0Intr(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_GRP1))) {
            ret = ACQCMN_IntEnableNsuGenEventGrp1Intr(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_GRP0))) {
            ret = ACQCMN_IntEnableNsuTsCapEventGrp0Intr(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_GRP1))) {
            ret = ACQCMN_IntEnableNsuTsCapEventGrp1Intr(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK_GRPP0))) {
            ret = ACQCMN_IntEnableNsuSystickGrp0Intr(hwId, intrType, aEnable);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief ACQCMN interrupt status

    @code{.unparsed}
    Check hardware instance with max supported device.
    Get interrupt status based on channel number and systick index.
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
int32_t ACQCMN_DrvInterruptStatus(ACQCMN_HwIDType hwId, ACQCMN_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == intrSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = ACQCMN_IntIntrStatus(hwId, 0U, intrSts);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief ACQCMN event status

    @code{.unparsed}
    Check hardware instance with max supported device.
    Get event status based on channel number and systick index.
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
int32_t ACQCMN_DrvEventStatus(ACQCMN_HwIDType hwId, ACQCMN_InterruptType *eventSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == eventSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = ACQCMN_IntIntrStatus(hwId, 1U, eventSts);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief ACQCMN clear interrupts

    @code{.unparsed}
    Check hardware instance with max supported device.
    Clear the interrupt status based on channel number and systick index.
    @endcode

    @trace #BRCM_SWARCH_ACQCMN_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_ACQCMN_INTERRUPT
*/
int32_t ACQCMN_DrvClearInterrupt(ACQCMN_HwIDType hwId, ACQCMN_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    ACQCMN_INTR_ACQ_NSU_EVT_INTR_EISR_TYPE acq_nsu_evt_intr_eisr = 0UL ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= ACQCMN_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(ACQCMN_DrvDev[hwId].state == ACQCMN_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        acq_nsu_evt_intr_eisr = ACQCMN_IntrRegs[hwId]->acq_nsu_evt_intr_eisr ;
        (void)acq_nsu_evt_intr_eisr;

        if(0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_GRP0)) {
            ret = ACQCMN_IntClearNsuGenEventGrp0Intr(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_GEN_EVENT_GRP1))) {
            ret = ACQCMN_IntClearNsuGenEventGrp1Intr(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_GRP0))) {
            ret = ACQCMN_IntClearNsuTsCapEventGrp0Intr(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_TS_CAP_EVENT_GRP1))) {
            ret = ACQCMN_IntClearNsuTsCapEventGrp1Intr(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & ACQCMN_INTERRUPT_NSU_SYSTICK_GRPP0))) {
            ret = ACQCMN_IntClearNsuSystickGrp0Intr(hwId, intrType);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}
/** @} */
