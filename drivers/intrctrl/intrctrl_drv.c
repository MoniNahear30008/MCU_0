/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_intrctrldrv_drv_impl Interrupt Controller Driver Design
    @ingroup grp_intrctrldrv

    @addtogroup grp_intrctrldrv_drv_impl
    @{

    @file drivers/intrctrl/intrctrl_drv.c
    @brief Interrupt controller Driver Design

    @version 0.1 Initial version
*/

#include <intrctrl.h>
#include <intrctrl_rdb.h>
#include <intrctrl_gen_rdb.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>

/**
    @name Interrupt controller Driver Design IDs
    @{
    @brief Driver Design IDs for Interrupt Controller

*/
#define BRCM_SWDSGN_INTRCTRL_MCU_IRQ6_STATUS_PROC             (0xB101U)    /**< @brief #INTRCTRL_McuIrq6Status          */
#define BRCM_SWDSGN_INTRCTRL_MCU_IRQ7_STATUS_PROC             (0xB102U)    /**< @brief #INTRCTRL_McuIrq7Status          */
#define BRCM_SWDSGN_INTRCTRL_Q8_IRQ7_STATUS_PROC              (0xB103U)    /**< @brief #INTRCTRL_Q8Irq7Status           */
#define BRCM_SWDSGN_INTRCTRL_GICSPI_IRQ6_STATUS_PROC          (0xB104U)    /**< @brief #INTRCTRL_GicSpiIrq6Status       */
#define BRCM_SWDSGN_INTRCTRL_DRV_SM_INTR_ENABLE_PROC          (0xB105U)    /**< @brief #INTRCTRL_DrvSmIntrEnable        */
#define BRCM_SWDSGN_INTRCTRL_DRV_NMI_INTR_ENABLE_PROC         (0xB106U)    /**< @brief #INTRCTRL_DrvNmiIntrEnable       */

#define BRCM_SWDSGN_INTRCTRL_REGS_GLOBAL                      (0xB107U)    /**< @brief #INTRCTRL_Regs                   */
#define BRCM_SWDSGN_INTRCTRL_INT_SMINTR_1F_PROC               (0xB108U)    /**< @brief #INTRCTRL_IntSmIntr1F            */
#define BRCM_SWDSGN_INTRCTRL_INT_SMINTR_3E0_PROC              (0xB109U)    /**< @brief #INTRCTRL_IntSmIntr3E0           */
#define BRCM_SWDSGN_INTRCTRL_DRVNMI_INTR_STS_PROC             (0xB10AU)    /**< @brief #INTRCTRL_DrvNmiIntrStatus       */
#define BRCM_SWDSGN_INTRCTRL_GEN_REGS_GLOBAL                  (0xB10BU)    /**< @brief #INTRCTRL_GenRegs                */
#define BRCM_SWDSGN_INTRCTRL_INT_ENABLE_GPIO_INTR_GRP0_PROC   (0xB10CU)    /**< @brief #INTRCTRL_IntEnableGpioIntrGrp0  */
#define BRCM_SWDSGN_INTRCTRL_INT_ENABLE_GPIO_INTR_GRP1_PROC   (0xB10DU)    /**< @brief #INTRCTRL_IntEnableGpioIntrGrp1  */
#define BRCM_SWDSGN_INTRCTRL_INT_GPIO_INTR_STATUS_GRP0_PROC   (0xB10EU)    /**< @brief #INTRCTRL_IntGpioIntrStatusGrp0  */
#define BRCM_SWDSGN_INTRCTRL_INT_GPIO_INTR_STATUS_GRP1_PROC   (0xB10FU)    /**< @brief #INTRCTRL_IntGpioIntrStatusGrp1  */
#define BRCM_SWDSGN_INTRCTRL_INT_GPIO_INTR_STATUS_PROC        (0xB110U)    /**< @brief #INTRCTRL_IntGpioIntrStatus      */
#define BRCM_SWDSGN_INTRCTRL_INT_CLEAR_GPIO_INTR_GRP0_PROC    (0xB111U)    /**< @brief #INTRCTRL_IntClearGpioIntrGrp0   */
#define BRCM_SWDSGN_INTRCTRL_INT_CLEAR_GPIO_INTR_GRP1_PROC    (0xB112U)    /**< @brief #INTRCTRL_IntClearGpioIntrGrp1   */
#define BRCM_SWDSGN_INTRCTRL_DRV_ENABLE_GPIO_INTERRUPT_PROC   (0xB113U)    /**< @brief #INTRCTRL_DrvEnableGpioInterrupt */
#define BRCM_SWDSGN_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC   (0xB114U)    /**< @brief #INTRCTRL_DrvGpioInterruptStatus */
#define BRCM_SWDSGN_INTRCTRL_DRV_GPIO_EVENT_STATUS_PROC       (0xB115U)    /**< @brief #INTRCTRL_DrvGpioEventStatus     */
#define BRCM_SWDSGN_INTRCTRL_DRV_CLEAR_GPIO_INTERRUPT_PROC    (0xB116U)    /**< @brief #INTRCTRL_DrvClearGpioInterrupt  */

/** @} */

/**
    @trace #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
    @trace #BRCM_SWARCH_INTRCTRL_MCU_IRQ6_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_MCU_IRQ7_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_Q8_IRQ7_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_GICSPI_IRQ6_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_DRV_SM_INTR_ENABLE_PROC
    @trace #BRCM_SWARCH_INTRCTRL_DRV_NMI_INTR_ENABLE_PROC
*/
static INTRCTRL_RDBType * const INTRCTRL_Regs[INTRCTRL_MAX_HW_ID] =
{
#if (INTRCTRL_MAX_HW_ID == 0U)
#error "INTRCTRL_MAX_HW_ID == 0U"
#endif
    (INTRCTRL_RDBType*)RIG_TOP_INTRCTRL_BASE,
#if (INTRCTRL_MAX_HW_ID > 1U)
#error "INTRCTRL_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_INTRCTRL_DRV_ENABLE_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_EVENT_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_DRV_CLEAR_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/

static INTRCTRL_GEN_RDBType * const INTRCTRL_GenRegs[INTRCTRL_GEN_MAX_HW_ID] =
{
#if (INTRCTRL_GEN_MAX_HW_ID == 0U)
#error "INTRCTRL_GEN_MAX_HW_ID == 0U"
#endif
    (INTRCTRL_GEN_RDBType*)RIG_GEN_INTRCTRL_BASE,
#if (INTRCTRL_GEN_MAX_HW_ID > 1U)
#error "INTRCTRL_GEN_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @brief Enable Group0 GPIO interrupt

    @trace #BRCM_SWARCH_INTRCTRL_DRV_ENABLE_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntEnableGpioIntrGrp0(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_0)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_0_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_0_INTR_MASK ;
            }
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_1)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_1_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_1_INTR_MASK ;
            }
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_2)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_2_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_2_INTR_MASK ;
            }
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_3)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_3_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_3_INTR_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Enable Group1 GPIO interrupt

    @trace #BRCM_SWARCH_INTRCTRL_DRV_ENABLE_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntEnableGpioIntrGrp1(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_4)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_4_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_4_INTR_MASK ;
            }
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_5)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_5_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_5_INTR_MASK ;
            }
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_6)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_6_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_6_INTR_MASK ;
            }
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_7)) {
            if(0UL == aEnable) {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier &= ~INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_7_INTR_MASK ;
            } else {
                INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eier |=  INTRCTRL_GEN_RIGEL_GPIO_INTR_EIER_ENABLE_SW_7_INTR_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Get Group0 GPIO interrupt status

    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntGpioIntrStatusGrp0(INTRCTRL_HwIDType hwId, uint8_t event, INTRCTRL_GpioIntrType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t rigel_gpio_intr = 0UL;

    if(0U == event) {
        rigel_gpio_intr = INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eipr ;
    } else {
        rigel_gpio_intr = INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_0_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_0 ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_1_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_1 ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_2_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_2 ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_3_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_3 ;
    }
    return ret;
}

/**
    @brief Get Group1 GPIO interrupt status

    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntGpioIntrStatusGrp1(INTRCTRL_HwIDType hwId, uint8_t event, INTRCTRL_GpioIntrType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t rigel_gpio_intr = 0UL;

    if(0U == event) {
        rigel_gpio_intr = INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eipr ;
    } else {
        rigel_gpio_intr = INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_4_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_4 ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_5_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_5 ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_6_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_6 ;
    }
    if(0UL != (rigel_gpio_intr & INTRCTRL_GEN_RIGEL_GPIO_INTR_EIPR_P_SW_7_INTR_MASK)) {
        *intrSts |= INTRCTRL_GPIO_INTR_7 ;
    }
    return ret;
}

/**
    @brief Get GPIO interrupt/event status

    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntGpioIntrStatus(INTRCTRL_HwIDType hwId, uint8_t event, INTRCTRL_GpioIntrType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    *intrSts = 0UL;
    ret = INTRCTRL_IntGpioIntrStatusGrp0(hwId, event, intrSts);

    if(BCM_ERR_OK == ret) {
        ret = INTRCTRL_IntGpioIntrStatusGrp1(hwId, event, intrSts);
    }
    return ret;
}

/**
    @brief Clear Group0 GPIO interrupt

    @trace #BRCM_SWARCH_INTRCTRL_DRV_CLEAR_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntClearGpioIntrGrp0(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType)
{
    int32_t ret = BCM_ERR_OK;
    INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_TYPE rigel_gpio_intr_eisr = 0UL ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rigel_gpio_intr_eisr = INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr ;
        (void)rigel_gpio_intr_eisr ;
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_0)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_0_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_1)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_1_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_2)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_2_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_3)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_3_INTR_W2CLR_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Clear Group1 GPIO interrupt

    @trace #BRCM_SWARCH_INTRCTRL_DRV_CLEAR_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
static int32_t INTRCTRL_IntClearGpioIntrGrp1(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType)
{
    int32_t ret = BCM_ERR_OK;
    INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_TYPE rigel_gpio_intr_eisr = 0UL ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        rigel_gpio_intr_eisr = INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr ;
        (void)rigel_gpio_intr_eisr ;
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_4)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_4_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_5)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_5_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_6)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_6_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_7)) {
            INTRCTRL_GenRegs[hwId]->rigel_gpio_intr_eisr |= INTRCTRL_GEN_RIGEL_GPIO_INTR_EISR_SW_7_INTR_W2CLR_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get MCU Pending interrupt status

    This API gets MCU shared (IRQ line number 6) interrupt pending status of MCU 0/1 .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_McuIrq6StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
    @trace  #BRCM_SWARCH_INTRCTRL_MCU_IRQ6_STATUS_PROC

    @code{.unparsed}
    Validate HW index and interrupt status
    Get MCU IRQ6 status
    @endcode
    @limitations None
*/
int32_t INTRCTRL_McuIrq6Status(INTRCTRL_HwIDType hwId, INTRCTRL_McuIrq6StsType *intrStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (intrStatus == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *intrStatus = (INTRCTRL_Regs[hwId]->top_mem_init_intr_eipr) & INTRCTRL_MCU_IRQ6_STS_MASK;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief Get MCU Pending status

    This API gets MCU shared (IRQ line number 7) interrupt pending status of MCU 0/1 .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_McuIrq7StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
    @trace  #BRCM_SWARCH_INTRCTRL_MCU_IRQ7_STATUS_PROC
    @code{.unparsed}
    Validate HW index and interrupt status
    Get MCU IRQ7 status
    @endcode

    @limitations None
*/
int32_t INTRCTRL_McuIrq7Status(INTRCTRL_HwIDType hwId, INTRCTRL_McuIrq7StsType *intrStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (intrStatus == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *intrStatus = (INTRCTRL_Regs[hwId]->top_gp_sram_fault_intr_eipr) & INTRCTRL_MCU_IRQ7_STS_MASK;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}
/** @brief Get Q8 Pending status

    This API gets Q8 shared (IRQ line number 7) interrupt pending status of Q8 0/1/2/3 .

    @behavior Sync, Non-reentrant

    @pre None
    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_Q8Irq7StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
    @trace  #BRCM_SWARCH_INTRCTRL_Q8_IRQ7_STATUS_PROC
    @code{.unparsed}
    Validate HW index and interrupt status
    Get Q8 IRQ7 status
    @endcode

    @limitations None
*/
int32_t INTRCTRL_Q8Irq7Status(INTRCTRL_HwIDType hwId, INTRCTRL_Q8Irq7StsType *intrStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (intrStatus == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *intrStatus = ((INTRCTRL_Regs[hwId]->top_acq_sys_intr_eipr) & INTRCTRL_Q8_IRQ7_STS_MASK) ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief Get GIC SPI Pending status

    This API gets GIC SPI shared (IRQ line number 6) interrupt pending status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_GicSPIIrq6StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
    @trace  #BRCM_SWARCH_INTRCTRL_GICSPI_IRQ6_STATUS_PROC
    @code{.unparsed}
    Validate HW index and interrupt status
    get GICAPI IRQ6 status
    @endcode

    @limitations None
*/
int32_t INTRCTRL_GicSpiIrq6Status(INTRCTRL_HwIDType hwId, INTRCTRL_GicSPIIrq6StsType *intrStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (intrStatus == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *intrStatus = (INTRCTRL_Regs[hwId]->top_mem_init_intr_eipr) & INTRCTRL_GICSPI_IRQ6_STS_MASK;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/**
    @brief Interrupt enable

    Helper function to enable interrupts for MCU0, MCU1, No Connect, Fabric and AHB monitor.

    @trace  #BRCM_SWREQ_INTRCTRL_SM_INTR
    @trace  #BRCM_SWARCH_INTRCTRL_DRV_SM_INTR_ENABLE_PROC

*/
static void INTRCTRL_IntSmIntr1F(INTRCTRL_HwIDType hwId, INTRCTRL_SmIntrBlkType intrBlock, INTRCTRL_SmBlkIntrType intrType, uint8_t enDis)
{
    uint32_t readEn = 0UL;
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_MCU0) == INTRCTRL_SM_INTR_BLK_MCU0) {
        if ((intrType & 0xFUL) >= 1UL) {
            readEn = INTRCTRL_Regs[hwId]->sm_mcu0_fault_eier;
            readEn &= ~intrType;
            if (enDis == 1U) {
                readEn |= intrType;
            }
            INTRCTRL_Regs[hwId]->sm_mcu0_fault_eier = readEn;
        }
        if ((intrType & 0x70UL) >= 1UL) {
            readEn = INTRCTRL_Regs[hwId]->sm_mcu0_ded_fault_eier;
            readEn &= ~(intrType >> 4UL);
            if (enDis == 1U) {
                readEn |= (intrType >> 4UL);
            }
            INTRCTRL_Regs[hwId]->sm_mcu0_ded_fault_eier = readEn;
        }
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_NO_CONNECT) == INTRCTRL_SM_INTR_BLK_NO_CONNECT ) {
        readEn = INTRCTRL_Regs[hwId]->sm_no_connect_eier ;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_no_connect_eier = readEn;
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_MCU1_LOCKUP)   == INTRCTRL_SM_INTR_BLK_MCU1_LOCKUP) {
        readEn = INTRCTRL_Regs[hwId]->sm_mcu1_lockup_q8_fatal_fault_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_mcu1_lockup_q8_fatal_fault_eier = readEn;
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_AHB_MON) == INTRCTRL_SM_INTR_BLK_AHB_MON) {
        readEn = INTRCTRL_Regs[hwId]->sm_ahb_mon_timeout_fault_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_ahb_mon_timeout_fault_eier = readEn;
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_FABRIC) == INTRCTRL_SM_INTR_BLK_FABRIC) {
        readEn = INTRCTRL_Regs[hwId]->sm_fabric_fault_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_fabric_fault_eier = readEn;
    }
}

/**
    @brief Interrupt enable

    Helper function to enable interrupts for BRWNOUT, ALARM, DED faults, GPSRAM, WWDT.

    @trace  #BRCM_SWREQ_INTRCTRL_SM_INTR
    @trace  #BRCM_SWARCH_INTRCTRL_DRV_SM_INTR_ENABLE_PROC

*/
static void INTRCTRL_IntSmIntr3E0(INTRCTRL_HwIDType hwId, INTRCTRL_SmIntrBlkType intrBlock, INTRCTRL_SmBlkIntrType intrType, uint8_t enDis)
{
    uint32_t readEn = 0UL;
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_BRWNOUT) == INTRCTRL_SM_INTR_BLK_BRWNOUT) {
        readEn = INTRCTRL_Regs[hwId]->sm_brwnout_fault_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_brwnout_fault_eier = readEn;
    }


    if ((intrBlock & INTRCTRL_SM_INTR_BLK_TEMP_ALRM) == INTRCTRL_SM_INTR_BLK_TEMP_ALRM  ) {
        readEn = INTRCTRL_Regs[hwId]->sm_temperature_alarm_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_temperature_alarm_eier = readEn;
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_DED_FAULT) == INTRCTRL_SM_INTR_BLK_DED_FAULT  ) {
        readEn = INTRCTRL_Regs[hwId]->sm_ded_fault_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_ded_fault_eier = readEn;
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_GPSRAM_FAULT)  == INTRCTRL_SM_INTR_BLK_GPSRAM_FAULT) {
        readEn = INTRCTRL_Regs[hwId]->sm_gp_sram_mem_protect_fault_eier;
        readEn &= ~intrType;
        if (enDis == 1U) {
            readEn |= intrType;
        }
        INTRCTRL_Regs[hwId]->sm_gp_sram_mem_protect_fault_eier = readEn;
    }
    if ((intrBlock & INTRCTRL_SM_INTR_BLK_WWDT) == INTRCTRL_SM_INTR_BLK_WWDT) {
        if ((intrType & 0x7FUL) >= 0x1UL) {
            readEn = INTRCTRL_Regs[hwId]->sm_wwdt1_7_intr_eier;
            readEn &= ~intrType;
            if (enDis == 1U) {
                readEn |= intrType;
            }
            INTRCTRL_Regs[hwId]->sm_wwdt1_7_intr_eier = readEn;
        }
        if ((intrType & 0x80UL) >= 0x1UL) {
            readEn = INTRCTRL_Regs[hwId]->sm_wwdt0_intr_eier;
            readEn &= ~(intrType >> 7UL);
            if (enDis == 1U) {
                readEn |= (intrType >> 7UL);
            }
            INTRCTRL_Regs[hwId]->sm_wwdt0_intr_eier = readEn;
        }
    }
}

/** @brief Enables/Disables safety monitor interrupts

    This API enables/disables safety monitor interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId            Index of the Interrupt controller
    @param[in]     intrBlock       Interrupt block to enable/Disable
    @param[in]     intrType        Interrupt type based on the block
    @param[in]     enDis           Enables(1)/Disables(0) interrupt

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   invalid intrBlock

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SM_INTR
    @trace  #BRCM_SWARCH_INTRCTRL_DRV_SM_INTR_ENABLE_PROC
    @code{.unparsed}
    Validate HW index and interrupt Block
    Based on the interrupt block write  EIER registers
    @endcode

    @limitations None
*/
int32_t INTRCTRL_DrvSmIntrEnable(INTRCTRL_HwIDType hwId, INTRCTRL_SmIntrBlkType intrBlock, INTRCTRL_SmBlkIntrType intrType, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t readEn = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (intrBlock > INTRCTRL_SM_INTR_BLK_PMIC) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if ((intrBlock & 0x1FU) >= 1U) {
            INTRCTRL_IntSmIntr1F(hwId, intrBlock, intrType, enDis);
        }
        if ((intrBlock & 0x3E0U) >= 1U) {
            INTRCTRL_IntSmIntr3E0(hwId, intrBlock, intrType, enDis);
        }
        if ((intrBlock & 0x1C00U) >= 1U) {
            if ((intrBlock & INTRCTRL_SM_INTR_BLK_QSPI) == INTRCTRL_SM_INTR_BLK_QSPI ) {
                readEn = INTRCTRL_Regs[hwId]->sm_qspi_overunder_flow_intr_eier;
                readEn &= ~intrType;
                if (enDis == 1U) {
                    readEn |= intrType;
                }
                INTRCTRL_Regs[hwId]->sm_qspi_overunder_flow_intr_eier = readEn;
            }
            if ((intrBlock & INTRCTRL_SM_INTR_BLK_ACQ_HSADC) == INTRCTRL_SM_INTR_BLK_ACQ_HSADC) {
                readEn = INTRCTRL_Regs[hwId]->sm_acq_hsadc_func_fault_eier;
                readEn &= ~intrType;
                if (enDis == 1U) {
                    readEn |= intrType;
                }
                INTRCTRL_Regs[hwId]->sm_acq_hsadc_func_fault_eier = readEn;
            }
            if ((intrBlock & INTRCTRL_SM_INTR_BLK_PMIC) == INTRCTRL_SM_INTR_BLK_PMIC) {
                readEn = INTRCTRL_Regs[hwId]->sm_pmic_int_eier;
                readEn &= ~intrType;
                if (enDis == 1U) {
                    readEn |= intrType;
                }
                INTRCTRL_Regs[hwId]->sm_pmic_int_eier = readEn;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief Configure NMI interrupts

    This API enables/disables NMI interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId            Index of the Interrupt controller
    @param[in]     nmiIntrType        Interrupt type based on the block
    @param[in]     enDis           Enables(1)/Disables(0) interrupt

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid memory for intrType

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_NMI_INTR
    @trace  #BRCM_SWARCH_INTRCTRL_DRV_NMI_INTR_ENABLE_PROC
    @code{.unparsed}
    Validate HW index and interrupt Block
    Based on the NMI interrupt write interrupt enable register accordingly

    @endcode

    @limitations None
*/

int32_t INTRCTRL_DrvNmiIntrEnable(INTRCTRL_HwIDType hwId, INTRCTRL_NmiIntrType *nmiIntrType, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t readEn = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (nmiIntrType == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (nmiIntrType->intrType0 > 1UL) {
            readEn = INTRCTRL_Regs[hwId]->mcu0_nmi_0_eier;
            readEn &= ~nmiIntrType->intrType0;
            if (enDis == 1U) {
                readEn |= nmiIntrType->intrType0;
            }
            INTRCTRL_Regs[hwId]->mcu0_nmi_0_eier = readEn;
        }
        if (nmiIntrType->intrType1 > 1UL) {
            readEn = INTRCTRL_Regs[hwId]->mcu0_nmi_1_eier;
            readEn &= ~nmiIntrType->intrType1;
            if (enDis == 1U) {
                readEn |= nmiIntrType->intrType1;
            }
            INTRCTRL_Regs[hwId]->mcu0_nmi_1_eier = readEn;
        }
        if (nmiIntrType->intrType2 > 1UL) {
            readEn = INTRCTRL_Regs[hwId]->mcu0_nmi_2_eier;
            readEn &= ~nmiIntrType->intrType2;
            if (enDis == 1U) {
                readEn |= nmiIntrType->intrType2;
            }
            INTRCTRL_Regs[hwId]->mcu0_nmi_2_eier = readEn;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get NMI interrupt pending status

    This API gets NMI interrupts pending status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId            Index of the Interrupt controller
    @param[out]     nmiIntrType     Interrupt Status

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid memory for intrType
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_NMI_INTR
    @trace  #BRCM_SWARCH_INTRCTRL_DRV_NMI_INTR_STS_PROC

    @code{.unparsed}
    Validate HW index and interrupt Block
    Based on the NMI interrupt write interrupt enable register accordingly
    @endcode

    @limitations None
*/

int32_t INTRCTRL_DrvNmiIntrStatus(INTRCTRL_HwIDType hwId, INTRCTRL_NmiIntrType *nmiIntrType)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_NODEV;
    } else if (nmiIntrType == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        nmiIntrType->intrType0 = INTRCTRL_Regs[hwId]->mcu0_nmi_0_eipr;
        nmiIntrType->intrType1 = INTRCTRL_Regs[hwId]->mcu0_nmi_1_eipr;
        nmiIntrType->intrType2 = INTRCTRL_Regs[hwId]->mcu0_nmi_2_eipr;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief INTRCTRL enable GPIO interrupts

    @code{.unparsed}
    Enable GPIO interrupt
    @endcode

    @trace #BRCM_SWARCH_INTRCTRL_DRV_ENABLE_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
int32_t INTRCTRL_DrvEnableGpioInterrupt(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_GRP0)) {
            ret = INTRCTRL_IntEnableGpioIntrGrp0(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & INTRCTRL_GPIO_INTR_GRP1))) {
            ret = INTRCTRL_IntEnableGpioIntrGrp1(hwId, intrType, aEnable);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief INTRCTRL GPIO interrupt status

    @code{.unparsed}
    Get GPIO interrupt status
    @endcode

    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
int32_t INTRCTRL_DrvGpioInterruptStatus(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == intrSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = INTRCTRL_IntGpioIntrStatus(hwId, 0U, intrSts);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief INTRCTRL GPIO event status

    @code{.unparsed}
    Get GPIO event status
    @endcode

    @trace #BRCM_SWARCH_INTRCTRL_DRV_GPIO_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
int32_t INTRCTRL_DrvGpioEventStatus(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType *eventSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == eventSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = INTRCTRL_IntGpioIntrStatus(hwId, 1U, eventSts);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief INTRCTRL GPIO clear interrupts

    @code{.unparsed}
    Clear GPIO interrupts
    @endcode

    @trace #BRCM_SWARCH_INTRCTRL_DRV_CLEAR_GPIO_INTERRUPT_PROC
    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
*/
int32_t INTRCTRL_DrvClearGpioInterrupt(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= INTRCTRL_GEN_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (intrType & INTRCTRL_GPIO_INTR_GRP0)) {
            ret = INTRCTRL_IntClearGpioIntrGrp0(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & INTRCTRL_GPIO_INTR_GRP1))) {
            ret = INTRCTRL_IntClearGpioIntrGrp1(hwId, intrType);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @} */

