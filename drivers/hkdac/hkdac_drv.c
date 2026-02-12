/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_hkdacdrv_impl house Keeping DAC Driver Design
    @ingroup grp_hkdacdrv

    @addtogroup grp_hkdacdrv_impl
    @{

    @file drivers/hkdac/hkdac_drv.c
    @brief HKDAC Driver Design

    @version 0.1 Initial version
*/

#include <hkdac.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <hkdac_rdb.h>
#include <hkdac_intr_rdb.h>
#include <string.h>

/**
    @name HKDAC Driver Design IDs
    @{
    @brief Driver Design IDs for HKDAC
*/
#define BRCM_SWDSGN_HKDAC_DRV_INIT_PROC                    (0xB101U)    /**< @brief #HKDAC_DrvInit            */
#define BRCM_SWDSGN_HKDAC_DRV_DEINIT_PROC                  (0xB102U)    /**< @brief #HKDAC_DrvDeInit          */
#define BRCM_SWDSGN_HKDAC_DRV_CONFIG_PROC                  (0xB103U)    /**< @brief #HKDAC_DrvConfig          */
#define BRCM_SWDSGN_HKDAC_DRV_DAC_CONTROL_PROC             (0xB104U)    /**< @brief #HKDAC_DrvDacControl      */
#define BRCM_SWDSGN_HKDAC_DRV_POWER_CONFIG_PROC            (0xB105U)    /**< @brief #HKDAC_DrvPowerConfig     */
#define BRCM_SWDSGN_HKDAC_DRV_GET_DATA_PROC                (0xB106U)    /**< @brief #HKDAC_DrvGetData         */
#define BRCM_SWDSGN_HKDAC_DRV_CONFIG_CH_INTR_PROC          (0xB107U)    /**< @brief #HKDAC_DrvConfigChIntr    */
#define BRCM_SWDSGN_HKDAC_DRV_CLER_CH_INTR_PROC            (0xB108U)    /**< @brief #HKDAC_DrvClearChIntr     */
#define BRCM_SWDSGN_HKDAC_DRV_GET_CH_INTR_STATUS_PROC      (0xB109U)    /**< @brief #HKDAC_DrvGetChIntrStatus */
#define BRCM_SWDSGN_HKDAC_REGS_GLOBAL                      (0xB10AU)    /**< @brief #HKDAC_Regs               */
#define BRCM_SWDSGN_HKDAC_INTR_REGS_GLOBAL                 (0xB10BU)    /**< @brief #HKDAC_IntrRegs           */
#define BRCM_SWDSGN_HKDAC_DRV_DEV_TYPE                     (0xB10CU)    /**< @brief #HKDAC_DrvDevType         */
#define BRCM_SWDSGN_HKDAC_DRV_DEV_GLOBAL                   (0xB10DU)    /**< @brief #HKDAC_DrvDev             */
#define BRCM_SWDSGN_HKDAC_DRV_CONFIG_DATA_PROC             (0xB10EU)    /**< @brief #HKDAC_DrvConfigData   */
/** @} */

/**
    @trace #BRCM_SWARCH_HKDAC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CFG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DAC_CONTROL_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_DATA_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWREQ_HKDAC_INIT
*/
static HKDAC_RDBType * const HKDAC_Regs[HKDAC_MAX_HW_ID] =
{
#if (HKDAC_MAX_HW_ID == 0U)
#error "HKDAC_MAX_HW_ID == 0U"
#endif
    (HKDAC_RDBType*)HKDAC_BASE,
#if (HKDAC_MAX_HW_ID > 1U)
#error "HKDAC_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_HKDAC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CFG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DAC_CONTROL_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_DATA_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWREQ_HKDAC_INIT
*/
typedef struct sHKDAC_DrvDevType {
    HKDAC_StateType   state;            /**< State of the driver */
} HKDAC_DrvDevType;

/**
    @trace #BRCM_SWARCH_HKDAC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CFG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DAC_CONTROL_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_DATA_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWREQ_HKDAC_INIT
*/
static HKDAC_DrvDevType HKDAC_DrvDev[HKDAC_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWARCH_HKDAC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CFG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_POWER_CONFIG_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_DAC_CONTROL_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_DATA_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKDAC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWREQ_HKDAC_INTERRUPT
*/
static HKDAC_INTR_RDBType * const HKDAC_IntrRegs[HKDAC_MAX_HW_ID] =
{
#if (HKDAC_MAX_HW_ID == 0U)
#error "HKDAC_MAX_HW_ID == 0U"
#endif
    (HKDAC_INTR_RDBType*)HKDAC_INTR_BASE,
#if (HKDAC_MAX_HW_ID > 1U)
#error "HKDAC_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/** @brief Initialize HKDAC
    @code{.unparsed}
        Validate hardware instance with max supported device
        Take HKDAC out of reset
        Move state to HKDAC_STATE_INITIALIZED
    @endcode

    @trace  #BRCM_SWARCH_HKDAC_DRV_INIT_PROC
    @trace  #BRCM_SWREQ_HKDAC_INIT
*/

int32_t HKDAC_DrvInit(HKDAC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*Initialize HKDAC*/
        HKDAC_DrvDev[hwId].state = HKDAC_STATE_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief De-initialize HKDAC

    @code{.unparsed}
        Validate hardware instance with max supported device
        Put HKDAC in to reset
        Move state to HKDAC_STATE_UNIT
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_INIT
    @trace  #BRCM_SWARCH_HKDAC_DRV_DEINIT_PROC
*/

int32_t HKDAC_DrvDeInit (HKDAC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) {/* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*uninitialize HKDAC*/
        HKDAC_DrvDev[hwId].state = HKDAC_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKDAC configuration

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state and DAC Sync group and sync Mode
        Write programRateCount , syncGroup, syncMode, enAwg, lowPowerMode in to DAC Control register
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKDAC_DRV_CFG_PROC

*/
int32_t HKDAC_DrvConfig (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, HKDAC_ChannelConfigType *hkdacConfig)
{
    int ret = BCM_ERR_OK;
    uint32_t dacCtrl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (hkdacConfig == NULL) { /*Validate for Null pointer*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (hkdacConfig->syncGroup > 0xFFU) { /*Validate Sync Group*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (hkdacConfig->syncMode > HKDAC_SYNCMODE_FIFO_RD_GRP_MEM_EXT_TRG) { /*Validate Sync mode*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*Configure Program rate, enAWG, LowPower, sync mode and sync group*/
        dacCtrl = HKDAC_Regs[hwId]->chan[chId].dac0_ctrl;
        dacCtrl &= ~(HKDAC_DAC_CTRL_PROG_RATE_CNT_MASK | HKDAC_DAC_CTRL_EN_AWG_MASK | HKDAC_DAC_CTRL_EN_LOWPOWER_MASK|
                    HKDAC_DAC_CTRL_SYNC_MODE_MASK | HKDAC_DAC_CTRL_SYNC_GRP_MASK);
        HKDAC_Regs[hwId]->chan[chId].dac0_ctrl = dacCtrl | hkdacConfig->programRateCount |
            ((uint32_t)hkdacConfig->syncGroup <<  HKDAC_DAC_CTRL_SYNC_GRP_SHIFT) | ((uint32_t)hkdacConfig->syncMode << HKDAC_DAC_CTRL_SYNC_MODE_SHIFT) |
            ((uint32_t)hkdacConfig->enAwg << HKDAC_DAC_CTRL_EN_AWG_SHIFT) | ((uint32_t)hkdacConfig->lowPowerMode << HKDAC_DAC_CTRL_EN_LOWPOWER_SHIFT) ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKDAC AFE power configuration

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state
        Write dac0_control power down bit based on pwrConfig value
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKDAC_DRV_POWER_CONFIG_PROC

*/
int32_t HKDAC_DrvPowerConfig (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, uint8_t pwrConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (pwrConfig > 0U) { /* Power up*/
            HKDAC_Regs[hwId]->chan[chId].dac0_ctrl |= HKDAC_DAC_CTRL_PDB_MASK ;
        } else { /*power down*/
            HKDAC_Regs[hwId]->chan[chId].dac0_ctrl &= ~(HKDAC_DAC_CTRL_PDB_MASK) ;
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief HKDAC controller configuration

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state
        Write dac0_control enable DAC bit based on dacControl value
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKDAC_DRV_DAC_CONTROL_PROC
*/
int32_t HKDAC_DrvDacControl (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, uint8_t dacControl)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (dacControl > 0U) { /* Enable DAC controller*/
            HKDAC_Regs[hwId]->chan[chId].dac0_ctrl |= HKDAC_DAC_CTRL_EN_DAC_CTRL_MASK ;
        } else { /*Disable DAC controller*/
            HKDAC_Regs[hwId]->chan[chId].dac0_ctrl &= ~(HKDAC_DAC_CTRL_EN_DAC_CTRL_MASK);
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKDAC get DAC data

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state
        Validate for Null Pointer
        Read dac0_data and assign it to loData and hiData
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKDAC_DRV_GET_DATA_PROC
*/
int32_t HKDAC_DrvGetData (HKDAC_HwIDType  hwId, HKDAC_ChannelIDType chId, uint32_t *loData,  uint32_t *hiData)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (loData == NULL) {   /*Validate for null parameter*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (hiData == NULL) {   /*Validate for null parameter*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* Read DAC lower data*/
        *loData = (HKDAC_Regs[hwId]->chan[chId].dac0_data & HKDAC_DAC_DATA_LOWER_DATA_REG_MASK);

        /* Read DAC upper data*/
        *hiData = (HKDAC_Regs[hwId]->chan[chId].dac0_data >> HKDAC_DAC_DATA_UPPER_DATA_REG_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKDAC Enable Channel interrupt

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state
        Validate Interrupt type with HKDAC_CHANNEL_INTR_ALL
        Enable/disable all the interrupts if user chooses intrType as HKDAC_CHANNEL_INTR_ALL
            else enable/disable HKDAC_CHANNEL_INTR_FIFO_EMPTY or HKDAC_CHANNEL_INTR_FIFO_OVERFLOW
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_INTERRUPT
    @trace  #BRCM_SWARCH_HKDAC_DRV_CONFIG_CH_INTR_PROC
*/
int32_t HKDAC_DrvConfigChIntr(HKDAC_HwIDType  hwId,  HKDAC_ChannelIDType chId, HKDAC_ChannelIntrType intrType, uint32_t enaDis)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (intrType > HKDAC_CHANNEL_INTR_ALL) { /* Validate interrupt Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (intrType == HKDAC_CHANNEL_INTR_ALL) {
            if (enaDis != 0UL) { /*Enable all the interrupt*/
                HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eier = (HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_OVERFLOW_INTR_MASK |
                    HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_EMPTY_INTR_MASK);
            } else { /*disable all the interrupt*/
                HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eier = (uint32_t)~(HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_OVERFLOW_INTR_MASK |
                    HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_EMPTY_INTR_MASK);
            }
        } else {
            if ((intrType & HKDAC_CHANNEL_INTR_FIFO_EMPTY) > 0UL) { /*FIFO Empty Interrupt*/
                if (enaDis != 0UL) { /*Enable FIFO empty the interrupt*/
                    HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eier |= (HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_EMPTY_INTR_MASK);
                } else { /*disable fifo empty interrupt*/
                    HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eier &= ~(HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_EMPTY_INTR_MASK);
                }
            } else { /*FIFO Overflow Interrupt*/
                if (enaDis != 0UL) { /*Enable FIFO empty the interrupt*/
                    HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eier |= (HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_OVERFLOW_INTR_MASK);
                } else { /*disable overflow interrupt*/
                    HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eier &= ~(HKDAC_INTR_AHD0FIE2_ENABLE_DAC0_FIFO_OVERFLOW_INTR_MASK);
                }
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKDAC clear Channel interrupt

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state
        Validate Interrupt type with HKDAC_CHANNEL_INTR_ALL
        Clear all the interrupts if user chooses intrType as HKDAC_CHANNEL_INTR_ALL
            else clear HKDAC_CHANNEL_INTR_FIFO_EMPTY or HKDAC_CHANNEL_INTR_FIFO_OVERFLOW based on user input
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_INTERRUPT
    @trace  #BRCM_SWARCH_HKDAC_DRV_CLEAR_CH_INTR_PROC
*/
int32_t HKDAC_DrvClearChIntr(HKDAC_HwIDType  hwId,  HKDAC_ChannelIDType chId, HKDAC_ChannelIntrType intrType)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (intrType > HKDAC_CHANNEL_INTR_ALL) { /* Validate interrupt Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (intrType == HKDAC_CHANNEL_INTR_ALL) {
            /*Clear all the interrupt*/
            HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eisr = (HKDAC_INTR_AHD0FIE1_DAC0_FIFO_OVERFLOW_INTR_W2CLR_MASK |
                    HKDAC_INTR_AHD0FIE1_DAC0_FIFO_EMPTY_INTR_W2CLR_MASK);
        } else {
            if ((intrType & HKDAC_CHANNEL_INTR_FIFO_EMPTY) > 0UL) { /*Clear FIFO Empty Interrupt*/
                HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eisr |= (HKDAC_INTR_AHD0FIE1_DAC0_FIFO_EMPTY_INTR_W2CLR_MASK);
            } else { /*Clear FIFO Overflow Interrupt*/
                HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eisr |= (HKDAC_INTR_AHD0FIE1_DAC0_FIFO_OVERFLOW_INTR_W2CLR_MASK);
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKDAC Get Channel interrupt Status

    @code{.unparsed}
        Validate hardware instance, Channel with max supported device
        Validate HW state
        Validate Interrupt type with HKDAC_CHANNEL_INTR_ALL
        Read interrupt Status of all the interrupts if user chooses intrType as HKDAC_CHANNEL_INTR_ALL
            else read interrupt status for HKDAC_CHANNEL_INTR_FIFO_EMPTY or HKDAC_CHANNEL_INTR_FIFO_OVERFLOW based on user input
        Read Event status if user chooses HKDAC_CHANNEL_INTR_EVENT_ALL
    @endcode

    @trace  #BRCM_SWREQ_HKDAC_INTERRUPT
    @trace  #BRCM_SWARCH_HKDAC_DRV_GET_CH_INTR_STS_PROC
*/
int32_t HKDAC_DrvGetChIntrStatus(HKDAC_HwIDType  hwId,  HKDAC_ChannelIDType chId, HKDAC_ChannelIntrType intrType, uint32_t *intrStatus)
{
    int ret = BCM_ERR_OK;
    uint32_t status = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (intrType == HKDAC_CHANNEL_INTR_ALL) {
            /*Get all interrupt status */
            status = HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eipr;
            *intrStatus = ((((status & HKDAC_INTR_AHD0FIE_P_DAC0_FIFO_OVERFLOW_INTR_MASK) > 0UL) ? HKDAC_CHANNEL_INTR_FIFO_OVERFLOW : 0UL) |
                           (((status & HKDAC_INTR_AHD0FIE_P_DAC0_FIFO_EMPTY_INTR_MASK) > 0UL) ? HKDAC_CHANNEL_INTR_FIFO_EMPTY : 0UL));
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            if ((intrType & HKDAC_CHANNEL_INTR_FIFO_EMPTY) > 0UL) { /*Clear FIFO Empty Interrupt*/
                *intrStatus = (((HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eipr & HKDAC_INTR_AHD0FIE_P_DAC0_FIFO_EMPTY_INTR_MASK) > 0UL) ?
                                 HKDAC_CHANNEL_INTR_FIFO_EMPTY : 0UL) ;
            } else { /*Clear FIFO Overflow Interrupt*/
                *intrStatus = (((HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eipr & HKDAC_INTR_AHD0FIE_P_DAC0_FIFO_OVERFLOW_INTR_MASK) > 0UL) ?
                                 HKDAC_CHANNEL_INTR_FIFO_OVERFLOW : 0UL) ;
            }
        }
#endif
        if (intrType == HKDAC_CHANNEL_INTR_EVENT_ALL) {
            /*Get all event status */
            *intrStatus = (((HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eisr & HKDAC_INTR_AHD0FIE1_DAC0_FIFO_OVERFLOW_INTR_W2CLR_MASK) > 0UL) ?
                HKDAC_CHANNEL_INTR_FIFO_OVERFLOW : 0UL) | (((HKDAC_IntrRegs[hwId]->chan[chId].acq_hkdac_dac0_fifo_intr_eisr & HKDAC_INTR_AHD0FIE1_DAC0_FIFO_EMPTY_INTR_W2CLR_MASK) > 0UL) ?
                HKDAC_CHANNEL_INTR_FIFO_EMPTY : 0UL) ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HKDAC data config

    @code{.unparsed}
    Validate Hardware instance, Device state and chData.
    Based on size write lower and upper data to dac0_data register.
    @endcode

    @trace #BRCM_SWARCH_HKDAC_DRV_CONFIG_DATA_PROC
    @trace #BRCM_SWREQ_HKDAC_CONFIGURATION
*/
int32_t HKDAC_DrvConfigData (HKDAC_HwIDType  hwId, HKDAC_ChannelDataType *chData) 
{
    int ret = BCM_ERR_OK;
    uint16_t index = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKDAC_MAX_HW_ID) { /* Validate Hw instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKDAC_DrvDev[hwId].state == HKDAC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if ((chData == NULL) || chData->chId >= HKDAC_CHAN_SIZE) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chData->dataSize > 16U) { /* Validate Channel Max*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        for (index = 0U; index < chData->dataSize; index ++) {
            HKDAC_Regs[hwId]->chan[chData->chId].dac0_data = ((chData->loData & HKDAC_DAC_DATA_LOWER_DATA_REG_MASK) |
                 ((uint32_t)chData->hiData << HKDAC_DAC_DATA_UPPER_DATA_REG_SHIFT));
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @} */
