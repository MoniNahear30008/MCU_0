/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_hsrefdrv_impl HSREF Driver Design
    @ingroup grp_hsrefdrv

    @addtogroup grp_hsrefdrv_impl
    @{

    @file hsref_drv.c
    @brief HSREF Driver Design

    @version 0.1 Initial version
*/

#include <hsref.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <hsref_rdb.h>
#include <hsref_intr_rdb.h>

/**
    @name HSREF Driver Design IDs
    @{
    @brief Driver Design IDs for HSREF
*/

#define BRCM_SWDSGN_HSREF_DRV_DEV_TYPE                           (0xB181U)  /**< @brief #HSREF_DrvDevType                   */
#define BRCM_SWDSGN_HSREF_DRV_DEV_GLOBAL                         (0xB182U)  /**< @brief #HSREF_DrvDev                       */
#define BRCM_SWDSGN_HSREF_REGS_GLOBAL                            (0xB183U)  /**< @brief #HSREF_Regs                         */
#define BRCM_SWDSGN_HSREF_INTR_REGS_GLOBAL                       (0xB184U)  /**< @brief #HSREF_IntrRegs                     */

#define BRCM_SWDSGN_HSREF_DRV_INIT_PROC                          (0xB190U)  /**< @brief #HSREF_DrvInit                      */
#define BRCM_SWDSGN_HSREF_DRV_DEINIT_PROC                        (0xB191U)  /**< @brief #HSREF_DrvDeInit                    */
#define BRCM_SWDSGN_HSREF_DRV_INIT_CALIB_PROC                    (0xB192U)  /**< @brief #HSREF_DrvInitCalib                 */
#define BRCM_SWDSGN_HSREF_DRV_TRIGGER_CALIB_PROC                 (0xB193U)  /**< @brief #HSREF_DrvTriggerCalib              */
#define BRCM_SWDSGN_HSREF_DRV_CALIB_STATUS_PROC                  (0xB194U)  /**< @brief #HSREF_DrvCalibStatus               */
#define BRCM_SWDSGN_HSREF_DRV_CFG_SVF_PROC                       (0xB195U)  /**< @brief #HSREF_DrvCfgSvf                    */
#define BRCM_SWDSGN_HSREF_DRV_CTRL_SVF_PROC                      (0xB196U)  /**< @brief #HSREF_DrvCtrlSvf                   */
#define BRCM_SWDSGN_HSREF_DRV_CFG_ZCROSS_PROC                    (0xB197U)  /**< @brief #HSREF_DrvCfgZcross                 */
#define BRCM_SWDSGN_HSREF_DRV_CTRL_ZCROSS_PROC                   (0xB198U)  /**< @brief #HSREF_DrvCtrlZcross                */
#define BRCM_SWDSGN_HSREF_DRV_FIFO_CTRL_ZCROSS_PROC              (0xB199U)  /**< @brief #HSREF_DrvFifoCtrlZcross            */
#define BRCM_SWDSGN_HSREF_DRV_FIFO_STS_ZCROSS_PROC               (0xB19AU)  /**< @brief #HSREF_DrvFifoStsZcross             */
#define BRCM_SWDSGN_HSREF_DRV_FIFO_READ_ZCROSS_PROC              (0xB19BU)  /**< @brief #HSREF_DrvFifoReadZcross            */
#define BRCM_SWDSGN_HSREF_DRV_CFG_CAP_BUF_PROC                   (0xB19CU)  /**< @brief #HSREF_DrvCfgCapBuf                 */
#define BRCM_SWDSGN_HSREF_DRV_CTRL_CAP_BUF_PROC                  (0xB19DU)  /**< @brief #HSREF_DrvCtrlCapBuf                */
#define BRCM_SWDSGN_HSREF_DRV_FIFO_CTRL_CAP_BUF_PROC             (0xB19EU)  /**< @brief #HSREF_DrvFifoCtrlCapBuf            */
#define BRCM_SWDSGN_HSREF_DRV_FIFO_STS_CAP_BUF_PROC              (0xB19FU)  /**< @brief #HSREF_DrvFifoStsCapBuf             */

#define BRCM_SWDSGN_HSREF_DRV_ENABLE_INTERRUPT_PROC              (0xB1A0U)  /**< @brief #HSREF_DrvEnableInterrupt           */
#define BRCM_SWDSGN_HSREF_DRV_INTERRUPT_STATUS_PROC              (0xB1A1U)  /**< @brief #HSREF_DrvInterruptStatus           */
#define BRCM_SWDSGN_HSREF_DRV_EVENT_STATUS_PROC                  (0xB1A2U)  /**< @brief #HSREF_DrvEventStatus               */
#define BRCM_SWDSGN_HSREF_DRV_CLEAR_INTERRUPT_PROC               (0xB1A3U)  /**< @brief #HSREF_DrvClearInterrupt            */

#define BRCM_SWDSGN_HSREF_INT_ENABLE_ADC_CALIB_INTERRUPT_PROC    (0xB1B0U)  /**< @brief #HSREF_IntEnableAdcCalibInterrupt   */
#define BRCM_SWDSGN_HSREF_INT_ENABLE_CAP_BUFF_INTERRUPT_PROC     (0xB1B1U)  /**< @brief #HSREF_IntEnableCapBufInterrupt     */
#define BRCM_SWDSGN_HSREF_INT_ENABLE_ZCROSS_FIFO_INTERRUPT_PROC  (0xB1B2U)  /**< @brief #HSREF_IntEnableZcrossFifoInterrupt */
#define BRCM_SWDSGN_HSREF_INT_INTR_STATUS_PROC                   (0xB1B3U)  /**< @brief #HSREF_IntIntrStatus                */
#define BRCM_SWDSGN_HSREF_INT_ADC_CALIB_INTR_STATUS_PROC         (0xB1B4U)  /**< @brief #HSREF_IntAdcCalibIntrStatus        */
#define BRCM_SWDSGN_HSREF_INT_CAP_BUFF_INTR_STATUS_PROC          (0xB1B5U)  /**< @brief #HSREF_IntCapBufIntrStatus          */
#define BRCM_SWDSGN_HSREF_INT_ZCROSS_FIFO_INTR_STATUS_PROC       (0xB1B6U)  /**< @brief #HSREF_IntZcrossFifoIntrStatus      */
#define BRCM_SWDSGN_HSREF_INT_CLEAR_ADC_CALIB_INTERRUPT_PROC     (0xB1B7U)  /**< @brief #HSREF_IntClearAdcCalibInterrupt    */
#define BRCM_SWDSGN_HSREF_INT_CLEAR_CAP_BUFF_INTERRUPT_PROC      (0xB1B8U)  /**< @brief #HSREF_IntClearCapBufInterrupt      */
#define BRCM_SWDSGN_HSREF_INT_CLEAR_ZCROSS_FIFO_INTERRUPT_PROC   (0xB1B9U)  /**< @brief #HSREF_IntClearZcrossFifoInterrupt  */

/** @} */

/**
    @trace #BRCM_SWARCH_HSREF_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_INIT_CALIB_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_TRIGGER_CALIB_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CALIB_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_SVF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_SVF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_READ_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_INIT
*/
static HSREF_RDBType *const HSREF_Regs[HSREF_MAX_HW_ID] = {
#if (HSREF_MAX_HW_ID == 0UL)
#error "HSREF_MAX_HW_ID == 0"
#endif
    (HSREF_RDBType *) HSREF_ACQ_BASE,
#if (HSREF_MAX_HW_ID > 1UL)
#error "HSREF_MAX_HW_ID > 1 is not supported by the driver"
#endif /* HSREF_MAX_HW_ID > 1 */
};

/**
    @trace #BRCM_SWARCH_HSREF_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INIT
*/
static HSREF_INTR_RDBType * const HSREF_IntrRegs[HSREF_INTR_MAX_HW_ID] =
{
#if (HSREF_INTR_MAX_HW_ID == 0UL)
#error "HSREF_MAX_HW_ID == 0U"
#endif
    (HSREF_INTR_RDBType *)HSREF_INTR_BASE,
#if (HSREF_INTR_MAX_HW_ID > 1UL)
#error "HSREF_MAX_HW_ID > 1 is not supported by the driver"
#endif /* HSREF_INTR_MAX_HW_ID > 1 */
};

/**
    @name HSREF Driver device type
    @{
    @brief HSREF Driver device type struct

    @trace #BRCM_SWARCH_HSREF_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_INIT_CALIB_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_TRIGGER_CALIB_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CALIB_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_SVF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_SVF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_READ_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_INIT
 */
typedef struct sHSREF_DrvDevType {
    HSREF_StateType   state; /**< @brief State of the driver */
} HSREF_DrvDevType;
/** @} */

/**
    @name HSREF Driver device type
    @{
    @brief HSREF Driver device type struct

    @trace #BRCM_SWARCH_HSREF_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_INIT_CALIB_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_TRIGGER_CALIB_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CALIB_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_SVF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_SVF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_READ_ZCROSS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CFG_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_INIT
 */
static HSREF_DrvDevType HSREF_DrvDev[HSREF_MAX_HW_ID] COMP_SECTION(".bss.drivers");
/** @} */

/**
    @brief Enable ADC calibration related interrupts

    @trace #BRCM_SWARCH_HSREF_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntEnableAdcCalibInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & HSREF_INTERRUPT_ADC_CALIB_DONE)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eier &= ~HSREF_INTR_AHCIE2_ENABLE_ADC_CALIB_DONE_INTR_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eier |=  HSREF_INTR_AHCIE2_ENABLE_ADC_CALIB_DONE_INTR_MASK ;
            }
        }
        if(0UL != (intrType & HSREF_INTERRUPT_ADC_CALIB_ERROR)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eier &= ~HSREF_INTR_AHCIE2_ENABLE_ADC_CALIB_ERR_INTR_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eier |=  HSREF_INTR_AHCIE2_ENABLE_ADC_CALIB_ERR_INTR_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Enable capture buffer related interrupts

    @trace #BRCM_SWARCH_HSREF_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntEnableCapBufInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_MEM_INIT_DONE)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eier &= ~HSREF_INTR_AHMISIE2_ENABLE_CAPBUF_MEM_INIT_DONE_INTR_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eier |=  HSREF_INTR_AHMISIE2_ENABLE_CAPBUF_MEM_INIT_DONE_INTR_MASK ;
            }
        }
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_MEM_INIT_ERROR)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eier &= ~HSREF_INTR_AHMISIE2_ENABLE_CAPBUF_MEM_INIT_ERR_INTR_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eier |=  HSREF_INTR_AHMISIE2_ENABLE_CAPBUF_MEM_INIT_ERR_INTR_MASK ;
            }
        }

        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN0)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eier &= ~HSREF_INTR_AHCFL0IE2_EL0C0FOI_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eier |=  HSREF_INTR_AHCFL0IE2_EL0C0FOI_MASK ;
            }
        }
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN0)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eier &= ~HSREF_INTR_AHCFL0IE2_EL0C0FTI_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eier |=  HSREF_INTR_AHCFL0IE2_EL0C0FTI_MASK ;
            }
        }

        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN1)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eier &= ~HSREF_INTR_AHCFL1IE2_EL1C0FOI_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eier |=  HSREF_INTR_AHCFL1IE2_EL1C0FOI_MASK ;
            }
        }
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN1)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eier &= ~HSREF_INTR_AHCFL1IE2_EL1C0FTI_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eier |=  HSREF_INTR_AHCFL1IE2_EL1C0FTI_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Enable zero cross related interrupts

    @trace #BRCM_SWARCH_HSREF_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntEnableZcrossFifoInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & HSREF_INTERRUPT_ZCROSS_FIFO_OVF)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eier &= ~HSREF_INTR_AHZFIE2_ENABLE_ZCROSS_FIFO_OVERFLOW_INTR_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eier |=  HSREF_INTR_AHZFIE2_ENABLE_ZCROSS_FIFO_OVERFLOW_INTR_MASK ;
            }
        }
        if(0UL != (intrType & HSREF_INTERRUPT_ZCROSS_FIFO_TSH)) {
            if(0UL == aEnable) {
                HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eier &= ~HSREF_INTR_AHZFIE2_ENABLE_ZCROSS_FIFO_THRESHOLD_INTR_MASK ;
            } else {
                HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eier |=  HSREF_INTR_AHZFIE2_ENABLE_ZCROSS_FIFO_THRESHOLD_INTR_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Get ADC calibration interrupt/event status

    @trace #BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntAdcCalibIntrStatus(HSREF_HwIDType hwId, uint8_t event, HSREF_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acq_hsref_calib_intr = 0UL;

    if(0U == event) {
        acq_hsref_calib_intr = HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eipr ;
    } else {
        acq_hsref_calib_intr = HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eisr ;
    }
    if(0UL != (acq_hsref_calib_intr & HSREF_INTR_AHCIE_P_ADC_CALIB_DONE_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_ADC_CALIB_DONE ;
    }
    if(0UL != (acq_hsref_calib_intr & HSREF_INTR_AHCIE_P_ADC_CALIB_ERR_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_ADC_CALIB_ERROR ;
    }
    return ret;
}

/**
    @brief Get capture buffer interrupt/event status

    @trace #BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntCapBufIntrStatus(HSREF_HwIDType hwId, uint8_t event, HSREF_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;

    uint32_t acq_hsref_capbuf_fifo_ln0_intr = 0UL;
    uint32_t acq_hsref_capbuf_fifo_ln1_intr = 0UL;
    uint32_t acq_hsref_mem_init_sec_intr    = 0UL;

    if(0U == event) {
        acq_hsref_capbuf_fifo_ln0_intr = HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eipr ;
        acq_hsref_capbuf_fifo_ln1_intr = HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eipr ;
        acq_hsref_mem_init_sec_intr    = HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eipr    ;
    } else {
        acq_hsref_capbuf_fifo_ln0_intr = HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eisr ;
        acq_hsref_capbuf_fifo_ln1_intr = HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eisr ;
        acq_hsref_mem_init_sec_intr    = HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eisr    ;
    }

    if(0UL != (acq_hsref_mem_init_sec_intr & HSREF_INTR_AHMISIE_P_CAPBUF_MEM_INIT_DONE_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_CAP_BUF_MEM_INIT_DONE ;
    }
    if(0UL != (acq_hsref_mem_init_sec_intr & HSREF_INTR_AHMISIE_P_CAPBUF_MEM_INIT_ERR_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_CAP_BUF_MEM_INIT_ERROR ;
    }
    if(0UL != (acq_hsref_capbuf_fifo_ln0_intr & HSREF_INTR_AHCFL0IE_P_LN0_CAPBUF0_FIFO_OVERFLOW_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN0 ;
    }
    if(0UL != (acq_hsref_capbuf_fifo_ln0_intr & HSREF_INTR_AHCFL0IE_P_LN0_CAPBUF0_FIFO_THRESHOLD_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN0 ;
    }
    if(0UL != (acq_hsref_capbuf_fifo_ln1_intr & HSREF_INTR_AHCFL1IE_P_LN1_CAPBUF0_FIFO_OVERFLOW_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN1 ;
    }
    if(0UL != (acq_hsref_capbuf_fifo_ln1_intr & HSREF_INTR_AHCFL1IE_P_LN1_CAPBUF0_FIFO_THRESHOLD_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN1 ;
    }
    return ret ;
}

/**
    @brief Get zero cross FIFO interrupt/event status

    @trace #BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntZcrossFifoIntrStatus(HSREF_HwIDType hwId, uint8_t event, HSREF_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acq_hsref_zcross_fifo_intr = 0UL;

    if(0U == event) {
        acq_hsref_zcross_fifo_intr = HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eipr ;
    } else {
        acq_hsref_zcross_fifo_intr = HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eisr ;
    }

    if(0UL != (acq_hsref_zcross_fifo_intr & HSREF_INTR_AHZFIE_P_ZCROSS_FIFO_OVERFLOW_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_ZCROSS_FIFO_OVF ;
    }
    if(0UL != (acq_hsref_zcross_fifo_intr & HSREF_INTR_AHZFIE_P_ZCROSS_FIFO_THRESHOLD_INTR_MASK)) {
        *intrSts |= HSREF_INTERRUPT_ZCROSS_FIFO_TSH ;
    }
    return ret ;
}

/**
    @brief Get HREF interrupt/event status

    @trace #BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntIntrStatus(HSREF_HwIDType hwId, uint8_t event, HSREF_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
    *intrSts = 0UL;
    ret = HSREF_IntAdcCalibIntrStatus(hwId, event, intrSts);

    if(BCM_ERR_OK == ret) {
        ret = HSREF_IntCapBufIntrStatus(hwId, event, intrSts);
    }

    if(BCM_ERR_OK == ret) {
        ret = HSREF_IntZcrossFifoIntrStatus(hwId, event, intrSts);
    }
    return ret;
}

/**
    @brief Clear ADC calibration related interrupts

    @trace #BRCM_SWARCH_HSREF_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntClearAdcCalibInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    HSREF_INTR_AHCIE1_TYPE acq_hsref_calib_intr_eisr = 0UL ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        acq_hsref_calib_intr_eisr = HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eisr ;
        (void)acq_hsref_calib_intr_eisr ;
        if(0UL != (intrType & HSREF_INTERRUPT_ADC_CALIB_DONE)) {
            HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eisr |= HSREF_INTR_AHCIE1_ADC_CALIB_DONE_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & HSREF_INTERRUPT_ADC_CALIB_ERROR)) {
            HSREF_IntrRegs[hwId]->acq_hsref_calib_intr_eisr |= HSREF_INTR_AHCIE1_ADC_CALIB_ERR_INTR_W2CLR_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Clear capture buffer related interrupts

    @trace #BRCM_SWARCH_HSREF_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntClearCapBufInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    HSREF_INTR_AHMISIE1_TYPE  acq_hsref_mem_init_sec_intr_eisr    = 0UL;
    HSREF_INTR_AHCFL0IE1_TYPE acq_hsref_capbuf_fifo_ln0_intr_eisr = 0UL;
    HSREF_INTR_AHCFL1IE1_TYPE acq_hsref_capbuf_fifo_ln1_intr_eisr = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        acq_hsref_mem_init_sec_intr_eisr    = HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eisr ;
        acq_hsref_capbuf_fifo_ln0_intr_eisr = HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eisr ;
        acq_hsref_capbuf_fifo_ln1_intr_eisr = HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eisr ;
        (void)acq_hsref_mem_init_sec_intr_eisr ;
        (void)acq_hsref_capbuf_fifo_ln0_intr_eisr;
        (void)acq_hsref_capbuf_fifo_ln1_intr_eisr;

        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_MEM_INIT_DONE)) {
            HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eisr |= HSREF_INTR_AHMISIE1_CAPBUF_MEM_INIT_DONE_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_MEM_INIT_ERROR)) {
            HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eisr |= HSREF_INTR_AHMISIE1_CAPBUF_MEM_INIT_ERR_INTR_W2CLR_MASK ;
        }

        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN0)) {
            HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eisr |= HSREF_INTR_AHCFL0IE1_LN0_CAPBUF0_FIFO_OVERFLOW_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN0)) {
            HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln0_intr_eisr |= HSREF_INTR_AHCFL0IE1_L0C0FTIW2_MASK ;
        }

        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_OVF_LN1)) {
            HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eisr |= HSREF_INTR_AHCFL1IE1_LN1_CAPBUF0_FIFO_OVERFLOW_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & HSREF_INTERRUPT_CAP_BUF_FIFO_TSH_LN1)) {
            HSREF_IntrRegs[hwId]->acq_hsref_capbuf_fifo_ln1_intr_eisr |= HSREF_INTR_AHCFL1IE1_L1C0FTIW2_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Clear zero cross related interrupts

    @trace #BRCM_SWARCH_HSREF_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
static int32_t HSREF_IntClearZcrossFifoInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
    HSREF_INTR_AHZFIE1_TYPE acq_hsref_zcross_fifo_intr_eisr;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        acq_hsref_zcross_fifo_intr_eisr = HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eisr ;
        (void)acq_hsref_zcross_fifo_intr_eisr;
        if(0UL != (intrType & HSREF_INTERRUPT_ZCROSS_FIFO_OVF)) {
            HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eisr |=  HSREF_INTR_AHZFIE1_ZCROSS_FIFO_OVERFLOW_INTR_W2CLR_MASK ;
        }
        if(0UL != (intrType & HSREF_INTERRUPT_ZCROSS_FIFO_TSH)) {
            HSREF_IntrRegs[hwId]->acq_hsref_zcross_fifo_intr_eisr |=  HSREF_INTR_AHZFIE1_ZCROSS_FIFO_THRESHOLD_INTR_W2CLR_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief initializes HSREF driver

    @code{.unparsed}
    check hardware instance with max supported device
    Enable Memory for capture buffer channels
    Enable Memory check enable for capture buffer
    Release resetb for capture buffer
    Poll init done for capture buffer till set
    if initdone
        state to HSREF_STATE_INITIALIZED state
    else
        state to HSREF_STATE_UNINIT;
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_INIT_PROC
    @trace #BRCM_SWREQ_HSREF_INIT
*/
int32_t HSREF_DrvInit(HSREF_HwIDType  hwId)
{
    int32_t ret = BCM_ERR_OK;
    int maxRetry = 0L;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID || (HSREF_DrvDev[hwId].state != HSREF_STATE_UNINIT)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HSREF_Regs[hwId]->mem_init_ctrl = HSREF_MEM_INIT_CONTROL_CAPBUF_INIT_EN_MASK;
        HSREF_Regs[hwId]->mem_init_ctrl |= HSREF_MEM_INIT_CONTROL_CAPBUF_INT_CHK_EN_MASK;
        HSREF_Regs[hwId]->mem_init_ctrl |= HSREF_MEM_INIT_CONTROL_CAPBUF_RSTB_MASK;

        maxRetry = 200L;
        do {
            if ((HSREF_IntrRegs[hwId]->acq_hsref_mem_init_sec_intr_eisr &
                 HSREF_INTR_AHMISIE1_CAPBUF_MEM_INIT_DONE_INTR_W2CLR_MASK) ==
                 HSREF_INTR_AHMISIE1_CAPBUF_MEM_INIT_DONE_INTR_W2CLR_MASK) {
                HSREF_DrvDev[hwId].state = HSREF_STATE_INITIALIZED;
                break;
            }
        } while ((maxRetry--) > 0);
        if (maxRetry < 0L) {
            ret = BCM_ERR_MAX_ATTEMPS;
            HSREF_DrvDev[hwId].state = HSREF_STATE_UNINIT;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief De-initializes HSREF driver

    @code{.unparsed}
     Check hardware instance with max supported device
     Put capture buffer to reset state
     State to HSREF_STATE_UNINIT;
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_HSREF_INIT
*/
int32_t HSREF_DrvDeInit(HSREF_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Put back device to reset state*/
        HSREF_Regs[hwId]->mem_init_ctrl &= ~HSREF_MEM_INIT_CONTROL_CAPBUF_RSTB_MASK;
        HSREF_DrvDev[hwId].state = HSREF_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF Calibration configuration

    @code{.unparsed}
    Check hardware instance and calibration configuration.
    Write calibStateEn on adccal_control0 register
    Write rdb duration on adccal_control1 register
    Write adccal_control2, adccal_control4 to reset values
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_INIT_CALIB_PROC
    @trace #BRCM_SWREQ_HSREF_CALIBRATION
*/
int32_t HSREF_DrvInitCalib(HSREF_HwIDType hwId,  HSREF_CalibConfigType *calibConfig)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if(calibConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (calibConfig->calStatesEn > 0xFU) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HSREF_Regs[hwId]->adccal_control0 &= ~HSREF_ADCCAL_CONTROL0_CAL_STATES_EN_MASK ;
        HSREF_Regs[hwId]->adccal_control0 |= (calibConfig->calStatesEn & HSREF_ADCCAL_CONTROL0_CAL_STATES_EN_MASK);
        HSREF_Regs[hwId]->adccal_control1  = (calibConfig->calBgRdbDuration & HSREF_ADCCAL_CONTROL1_CAL_BG_RDB_DURATION_MASK);

        /* Update Reset Value now, These register will be updated based on
         * lab results - Design team*/
        HSREF_Regs[hwId]->adccal_control2 = 0x3400UL;
        HSREF_Regs[hwId]->adccal_control4 = 0x1UL;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Triggers HSREF Calibration

    @code{.unparsed}
    Check hardware instance.
    Write calibration trigger into adccal_control0
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_TRIGGER_CALIB_PROC
    @trace #BRCM_SWREQ_HSREF_CALIBRATION
*/
int32_t HSREF_DrvTriggerCalib(HSREF_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Trigger calibration*/
        HSREF_Regs[hwId]->adccal_control0 |= (HSREF_ADCCAL_CONTROL0_CAL_START_MASK);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF Calibration Status

    @code{.unparsed}
    Check hardware instance.
    Get calibStatus from adccal_control0
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CALIB_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_CALIBRATION
*/
int32_t HSREF_DrvCalibStatus(HSREF_HwIDType hwId, HSREF_CalibStatusType *calibStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* read Calibration busy status*/
        calibStatus->calStatus = (uint8_t)(HSREF_Regs[hwId]->adccal_control0 >> HSREF_ADCCAL_CONTROL0_CAL_BUSY_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF state variable filter configuration

    @code{.unparsed}
    Check hardware instance and SVF configuration.
    Write svfSel in svf_control0 register
    Write svfQvar in svf_control1 register
    Write svfFvar in svf_control1 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CFG_SVF_PROC
    @trace #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER
*/
int32_t HSREF_DrvCfgSvf(HSREF_HwIDType hwId, HSREF_SvfConfigType *svfConfig)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == svfConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (svfConfig->mask & HSREF_SVF_CONFIG_MASK_SEL)) {
            if(HSREF_SVF_SEL_BP == svfConfig->svfSel) {
                HSREF_Regs[hwId]->svf_control0 &= ~HSREF_SVF_CONTROL0_SEL_MASK ;
            } else {
                HSREF_Regs[hwId]->svf_control0 |=  HSREF_SVF_CONTROL0_SEL_MASK ;
            }
        }

        if(0UL != (svfConfig->mask & HSREF_SVF_CONFIG_MASK_QVAR)) {
            HSREF_Regs[hwId]->svf_control1 &= ~HSREF_SVF_CONTROL1_Q_VAR_MASK ;
            HSREF_Regs[hwId]->svf_control1 |= ((svfConfig->svfQvar << HSREF_SVF_CONTROL1_Q_VAR_SHIFT) &
                                               HSREF_SVF_CONTROL1_Q_VAR_MASK) ;
        }

        if(0UL != (svfConfig->mask & HSREF_SVF_CONFIG_MASK_FVAR)) {
            HSREF_Regs[hwId]->svf_control1 &= ~HSREF_SVF_CONTROL1_F_VAR_MASK ;
            HSREF_Regs[hwId]->svf_control1 |= ((svfConfig->svfFvar << HSREF_SVF_CONTROL1_F_VAR_SHIFT) &
                                               HSREF_SVF_CONTROL1_F_VAR_MASK) ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF state variable filter control

    @code{.unparsed}
    Check hardware instance and SVF control.
    Write svfEnable in svf_control0 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_SVF_PROC
    @trace #BRCM_SWREQ_HSREF_STATE_VARIABLE_FILTER
*/
int32_t HSREF_DrvCtrlSvf(HSREF_HwIDType hwId, HSREF_SvfControlType *svfControl)
{
    int32_t ret = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == svfControl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0U == svfControl->svfEnable) {
            HSREF_Regs[hwId]->svf_control0 &= ~HSREF_SVF_CONTROL0_EN_MASK ;
        } else {
            HSREF_Regs[hwId]->svf_control0 |=  HSREF_SVF_CONTROL0_EN_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF zero cross configuration

    @code{.unparsed}
    Check hardware instance and zero cross configuration.
    Write zCrossPreMskCapDuration in zcross_control1 register
    Write zCrossCapDuration in zcross_control1 register
    Write zCrossHysRange in zcross_control0 register
    Write zCrossHysEnable in zcross_control0 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CFG_ZCROSS_PROC
    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
*/
int32_t HSREF_DrvCfgZcross(HSREF_HwIDType hwId, HSREF_ZcrossConfigType *zCrossConfig)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == zCrossConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (zCrossConfig->mask & HSREF_ZCROSS_CONFIG_MASK_PRE_CAP_DURATION)) {
            HSREF_Regs[hwId]->zcross_control1 &= ~HSREF_ZCROSS_CONTROL1_PRE_MASK_CAP_DURATION_MASK ;
            HSREF_Regs[hwId]->zcross_control1 |= ((zCrossConfig->zCrossPreMskCapDuration << HSREF_ZCROSS_CONTROL1_PRE_MASK_CAP_DURATION_SHIFT) &
                                                  HSREF_ZCROSS_CONTROL1_PRE_MASK_CAP_DURATION_MASK) ;
        }
        if(0UL != (zCrossConfig->mask & HSREF_ZCROSS_CONFIG_MASK_CAP_DURATION)) {
            HSREF_Regs[hwId]->zcross_control1 &= ~HSREF_ZCROSS_CONTROL1_CAP_DURATION_MASK ;
            HSREF_Regs[hwId]->zcross_control1 |= ((zCrossConfig->zCrossCapDuration << HSREF_ZCROSS_CONTROL1_CAP_DURATION_SHIFT) &
                                                  HSREF_ZCROSS_CONTROL1_CAP_DURATION_MASK) ;
        }
        if(0UL != (zCrossConfig->mask & HSREF_ZCROSS_CONFIG_MASK_HYS_RANGE)) {
            HSREF_Regs[hwId]->zcross_control0 &= ~HSREF_ZCROSS_CONTROL0_HYS_RANGE_MASK ;
            HSREF_Regs[hwId]->zcross_control0 |= ((zCrossConfig->zCrossHysRange << HSREF_ZCROSS_CONTROL0_HYS_RANGE_SHIFT) &
                                              HSREF_ZCROSS_CONTROL0_HYS_RANGE_MASK) ;
        }
        if(0UL != (zCrossConfig->mask & HSREF_ZCROSS_CONFIG_MASK_HYS_ENABLE)) {
            if(0U == zCrossConfig->zCrossHysEnable) {
                HSREF_Regs[hwId]->zcross_control0 &= ~HSREF_ZCROSS_CONTROL0_HYS_EN_MASK ;
            } else {
                HSREF_Regs[hwId]->zcross_control0 |=  HSREF_ZCROSS_CONTROL0_HYS_EN_MASK ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF zero cross circuit control

    @code{.unparsed}
    Check hardware instance and zero cross control.
    Write zCrossEnable in zcross_control0 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_ZCROSS_PROC
    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
*/
int32_t HSREF_DrvCtrlZcross(HSREF_HwIDType hwId, HSREF_ZcrossControlType *zCrossControl)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == zCrossControl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0U == zCrossControl->zCrossEnable) {
            HSREF_Regs[hwId]->zcross_control0 &= ~HSREF_ZCROSS_CONTROL0_EN_MASK ;
        } else {
            HSREF_Regs[hwId]->zcross_control0 |=  HSREF_ZCROSS_CONTROL0_EN_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF zero cross circuit FIFO control

    @code{.unparsed}
    Check hardware instance and zero cross FIFO control.
    Write rdStrobe in zcross_fifo_control register
    Write reset in zcross_fifo_control register
    Write threshold in zcross_fifo_control register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_ZCROSS_PROC
    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
*/
int32_t HSREF_DrvFifoCtrlZcross(HSREF_HwIDType hwId, HSREF_ZcrossFifoControlType *zCrossFifoControl)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == zCrossFifoControl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (zCrossFifoControl->mask & HSREF_ZCROSS_FIFO_CONTROL_MASK_READ)) {
            if(0U == zCrossFifoControl->rdStrobe) {
                HSREF_Regs[hwId]->zcross_fifo_control &= ~HSREF_ZCROSS_FIFO_CONTROL_RD_MASK ;
            } else {
                HSREF_Regs[hwId]->zcross_fifo_control |=  HSREF_ZCROSS_FIFO_CONTROL_RD_MASK ;
            }
        }

        if(0UL != (zCrossFifoControl->mask & HSREF_ZCROSS_FIFO_CONTROL_MASK_RESET)) {
            if(0U == zCrossFifoControl->reset) {
                HSREF_Regs[hwId]->zcross_fifo_control &= ~HSREF_ZCROSS_FIFO_CONTROL_RESET_MASK ;
            } else {
                HSREF_Regs[hwId]->zcross_fifo_control |=  HSREF_ZCROSS_FIFO_CONTROL_RESET_MASK ;
            }
        }
        if(0UL != (zCrossFifoControl->mask & HSREF_ZCROSS_FIFO_CONTROL_MASK_THRESHOLD)) {
            HSREF_Regs[hwId]->zcross_fifo_control &= ~HSREF_ZCROSS_FIFO_CONTROL_THRESHOLD_MASK ;
            HSREF_Regs[hwId]->zcross_fifo_control |= ((zCrossFifoControl->threshold << HSREF_ZCROSS_FIFO_CONTROL_THRESHOLD_SHIFT) &
                                                      HSREF_ZCROSS_FIFO_CONTROL_THRESHOLD_MASK) ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF zero cross FIFO status

    @code{.unparsed}
    Check hardware instance and zero cross FIFO status.
    Read fifo_status from zcross_fifo_status register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_ZCROSS_PROC
    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
*/
int32_t HSREF_DrvFifoStsZcross(HSREF_HwIDType hwId, HSREF_ZcrossFifoStatusType *zCrossFifoStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == zCrossFifoStatus) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        zCrossFifoStatus->status = HSREF_Regs[hwId]->zcross_fifo_status ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF zero cross FIFO read

    @code{.unparsed}
    Check hardware instance and zero cross FIFO read parameter.

    int32_t ret = BCM_ERR_OK
    uint8_t idx = 0U
    Validate function parameters
    read zcross_fifo_rd_data[0] to zcross_fifo_rd_data[7] registers and populate.
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_READ_ZCROSS_PROC
    @trace #BRCM_SWREQ_HSREF_ZERO_CROSS
*/
int32_t HSREF_DrvFifoReadZcross(HSREF_HwIDType hwId, HSREF_ZcrossFifoReadType *zCrossFifoRead)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t idx = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == zCrossFifoRead) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        for(idx=0U; idx<8U; idx++) {
            zCrossFifoRead->data[idx] = HSREF_Regs[hwId]->zcross_fifo_rd_data[idx] ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF capture buffer configuration

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Check hardware instance and capture buffer configuration.
    Write capSampleCnt in cap_buf_control1 register
    Write svfPostDiv in cap_buf_control0 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CFG_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
*/
int32_t HSREF_DrvCfgCapBuf(HSREF_HwIDType hwId, HSREF_CapBufConfigType *capBufConfig)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == capBufConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (capBufConfig->mask & HSREF_CAP_BUF_CONFIG_MASK_SAMPLE_CNT)) {
            HSREF_Regs[hwId]->cap_buf_control1 &= ~HSREF_CAP_BUF_CONTROL1_SAMPLES_CNT_MASK ;
            HSREF_Regs[hwId]->cap_buf_control1 |= ((capBufConfig->capSampleCnt << HSREF_CAP_BUF_CONTROL1_SAMPLES_CNT_SHIFT) &
                                                   HSREF_CAP_BUF_CONTROL1_SAMPLES_CNT_MASK);
        }
        if(0UL != (capBufConfig->mask & HSREF_CAP_BUF_CONFIG_MASK_SVF_POST_DIV)) {
            HSREF_Regs[hwId]->cap_buf_control0 &= ~HSREF_CAP_BUF_CONTROL0_SVF_POST_DIV_MASK ;
            HSREF_Regs[hwId]->cap_buf_control0 |= ((capBufConfig->svfPostDiv << HSREF_CAP_BUF_CONTROL0_SVF_POST_DIV_SHIFT) &
                                                   HSREF_CAP_BUF_CONTROL0_SVF_POST_DIV_MASK);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF capture buffer control

    @code{.unparsed}
    Check hardware instance and capture buffer control.
    Write capBufEnable in cap_buf_control0 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
*/
int32_t HSREF_DrvCtrlCapBuf(HSREF_HwIDType hwId, HSREF_CapBufControlType *capBufControl)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == capBufControl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0U == capBufControl->capBufEnable) {
            HSREF_Regs[hwId]->cap_buf_control0 &= ~HSREF_CAP_BUF_CONTROL0_CAP_BUF_EN_MASK ;
        } else {
            HSREF_Regs[hwId]->cap_buf_control0 |=  HSREF_CAP_BUF_CONTROL0_CAP_BUF_EN_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF capture buffer FIFO control

    @code{.unparsed}
    Check hardware instance and capture buffer FIFO control.
    Write reset in cap_buf_fifo_control0 register
    Write threshold in cap_buf_fifo_control0 register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_CTRL_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
*/
int32_t HSREF_DrvFifoCtrlCapBuf(HSREF_HwIDType hwId, HSREF_CapBufFifoControlType *capBufFifoControl)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == capBufFifoControl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(0UL != (capBufFifoControl->mask & HSREF_CAP_BUF_FIFO_CONTROL_MASK_RESET)) {
            if(0U == capBufFifoControl->reset) {
                HSREF_Regs[hwId]->cap_buf_fifo_control0 &= ~HSREF_CAP_BUF_FIFO_CONTROL0_RESET_MASK ;
            } else {
                HSREF_Regs[hwId]->cap_buf_fifo_control0 |=  HSREF_CAP_BUF_FIFO_CONTROL0_RESET_MASK ;
            }
        }
        if(0UL != (capBufFifoControl->mask & HSREF_CAP_BUF_FIFO_CONTROL_MASK_THRESHOLD)) {
            HSREF_Regs[hwId]->cap_buf_fifo_control0 &= ~HSREF_CAP_BUF_FIFO_CONTROL0_THRESHOLD_MASK ;
            HSREF_Regs[hwId]->cap_buf_fifo_control0 |= ((capBufFifoControl->threshold << HSREF_CAP_BUF_FIFO_CONTROL0_THRESHOLD_SHIFT) &
                                                        HSREF_CAP_BUF_FIFO_CONTROL0_THRESHOLD_MASK) ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF capture buffer FIFO status

    @code{.unparsed}
    Check hardware instance and capture buffer FIFO status.
    Read status from cap_buf_fifo_status_ln register based on lane capBufLane
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_FIFO_STS_CAP_BUF_PROC
    @trace #BRCM_SWREQ_HSREF_CAPTURE_BUFFER
*/
int32_t HSREF_DrvFifoStsCapBuf(HSREF_HwIDType hwId, HSREF_CapBufLaneType capBufLane, HSREF_CapBufFifoStatusType *capBufFifoStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == capBufFifoStatus) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        capBufFifoStatus->status = HSREF_Regs[hwId]->cap_buf_fifo_status_ln[capBufLane] ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF enable interrupts

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    Check hardware instance.
    Enable ADC calibration interrupt by writing acq_hsref_calib_intr_eier register
    Enable capture buffer interrupt by writing acq_hsref_capbuf_fifo_lnX_intr_eier, acq_hsref_mem_init_sec_intr_eier registers
    Enable zero cross FIFO interrupt by writing acq_hsref_zcross_fifo_intr_eier register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_ENABLE_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
int32_t HSREF_DrvEnableInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & HSREF_INTERRUPT_ADC)) {
            ret = HSREF_IntEnableAdcCalibInterrupt(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & HSREF_INTERRUPT_CAP_BUF))) {
            ret = HSREF_IntEnableCapBufInterrupt(hwId, intrType, aEnable);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & HSREF_INTERRUPT_ZCROSS_FIFO))) {
            ret = HSREF_IntEnableZcrossFifoInterrupt(hwId, intrType, aEnable);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF interrupt status

    @code{.unparsed}
    Check hardware instance.
    Get ADC calibration intrSts by reading acq_hsref_calib_intr_eipr register
    Get capture buffer intrSts by reading acq_hsref_capbuf_fifo_lnX_intr_eisr, acq_hsref_mem_init_sec_intr_eisr registers
    Get zero cross FIFO intrSts by reading acq_hsref_zcross_fifo_intr_eisr register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_INTERRUPT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
int32_t HSREF_DrvInterruptStatus(HSREF_HwIDType hwId, HSREF_InterruptType *intrSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    }  else if(NULL == intrSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = HSREF_IntIntrStatus(hwId, 0U, intrSts);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF event status

    @code{.unparsed}
    Check hardware instance.
    Get ADC calibration eventSts by reading acq_hsref_calib_intr_eisr register
    Get capture buffer eventSts by reading acq_hsref_capbuf_fifo_lnX_intr_eisr, acq_hsref_mem_init_sec_intr_eisr registers
    Get zero cross FIFO eventSts by reading acq_hsref_zcross_fifo_intr_eisr register
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_EVENT_STATUS_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
int32_t HSREF_DrvEventStatus(HSREF_HwIDType hwId, HSREF_InterruptType *eventSts)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if(NULL == eventSts) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = HSREF_IntIntrStatus(hwId, 1U, eventSts);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HSREF clear interrupts

    @code{.unparsed}
    Check hardware instance.
    Clear ADC calibration, Capture buffer and Zero cross FIFO interrupts
    by reading the interrupt status register and then writing 1 into the
    specific bits of the interrupt status registers based on intrType.
    @endcode

    @trace #BRCM_SWARCH_HSREF_DRV_CLEAR_INTERRUPT_PROC
    @trace #BRCM_SWREQ_HSREF_INTERRUPT
*/
int32_t HSREF_DrvClearInterrupt(HSREF_HwIDType hwId, HSREF_InterruptType intrType)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSREF_INTR_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(HSREF_DrvDev[hwId].state == HSREF_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if(0UL != (intrType & HSREF_INTERRUPT_ADC)) {
            ret = HSREF_IntClearAdcCalibInterrupt(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & HSREF_INTERRUPT_CAP_BUF))) {
            ret = HSREF_IntClearCapBufInterrupt(hwId, intrType);
        }
        if((BCM_ERR_OK == ret) && (0UL != (intrType & HSREF_INTERRUPT_ZCROSS_FIFO))) {
            ret = HSREF_IntClearZcrossFifoInterrupt(hwId, intrType);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @} */
