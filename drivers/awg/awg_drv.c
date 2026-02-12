/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_awgdrv_drv_impl AWG Driver Design
    @ingroup grp_awgdrv

    @addtogroup grp_awgdrv_drv_impl
    @{

    @file drivers/awg/awg_drv.c
    @brief AWG Driver Design

    @version 0.1 Initial version
*/

#include <awg.h>
#include <awg_rdb.h>
#include <awg_intr_rdb.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>

/**
    @name AWG Driver Design IDs
    @{
    @brief Driver Design IDs for AWG

*/
#define BRCM_SWDSGN_AWG_DRV_INIT_PROC                    (0xB101U)    /**< @brief #AWG_DrvInit        */
#define BRCM_SWDSGN_AWG_DRV_DEINIT_PROC                  (0xB102U)    /**< @brief #AWG_DrvDeInit        */
#define BRCM_SWDSGN_AWG_DRV_CHANNEL_CONFIG_PROC          (0xB103U)    /**< @brief #AWG_DrvChannelConfig */
#define BRCM_SWDSGN_AWG_DRV_CHANNEL_RAMP_CONFIG_PROC     (0xB104U)    /**< @brief #AWG_DrvChannelRampConfig      */
#define BRCM_SWDSGN_AWG_DRV_CHANNEL_RAMP_CONTROL_PROC    (0xB105U)    /**< @brief #AWG_DrvChannelRampControl    */
#define BRCM_SWDSGN_AWG_DRV_CHANNEL_CONFIG_FREQ_PROC     (0xB106U)    /**< @brief #AWG_DrvChannelConfigFrequency   */
#define BRCM_SWDSGN_AWG_DRV_CHANNEL_ENABLE_CONTROL_PROC  (0xB107U)    /**< @brief #AWG_DrvChannelEnableControl    */
#define BRCM_SWDSGN_AWG_DRV_CHANNEL_RUN_CONTROL_PROC     (0xB108U)    /**< @brief #AWG_DrvChannelRunControl    */
#define BRCM_SWDSGN_AWG_DRV_WAVEFORM_MEM_ACCESS_PROC     (0xB109U)    /**< @brief #AWG_DrvWaveformMemAccess      */
#define BRCM_SWDSGN_AWG_DRV_GET_CURR_ACTIVE_PAGE_PROC    (0xB10AU)    /**< @brief #AWG_DrvGetCurrActivePage  */
#define BRCM_SWDSGN_AWG_DRV_SET_ACTIVE_PAGE_PROC         (0xB10BU)    /**< @brief #AWG_DrvSetActivePage     */
#define BRCM_SWDSGN_AWG_DRV_GET_CURRENT_PHASE_PROC       (0xB10CU)    /**< @brief #AWG_DrvGetCurrentPhase       */
#define BRCM_SWDSGN_AWG_DRV_CONFIG_CH_INTR_PROC          (0xB10DU)    /**< @brief #AWG_DrvConfigChIntr      */
#define BRCM_SWDSGN_AWG_DRV_CLEAR_CH_INTR_PROC           (0xB10EU)    /**< @brief #AWG_DrvClearChIntr          */
#define BRCM_SWDSGN_AWG_DRV_GET_CH_INTR_STS_PROC         (0xB10FU)    /**< @brief #AWG_DrvGetChIntrStatus           */
#define BRCM_SWDSGN_AWG_REGS_GLOBAL                      (0xB110U)    /**< @brief #AWG_Regs*/
#define BRCM_SWDSGN_AWG_INTR_REGS_GLOBAL                 (0xB111U)    /**< @brief #AWG_IntrRegs*/
#define BRCM_SWDSGN_AWG_DRV_DEV_TYPE                     (0xB112U)    /**< @brief #AWG_DrvDevType*/
#define BRCM_SWDSGN_AWG_DRV_DEV_GLOBAL                   (0xB113U)    /**< @brief #AWG_DrvDev*/

#define BRCM_SWDSGN_AWG_INT_GET_ALL_INTERRUPT_STATUS_PROC (0xB114U)   /**< @brief #AWG_IntGetAllInterruptStatus*/
#define BRCM_SWDSGN_AWG_INT_GET_EVENT_STATUS_PROC         (0xB115U)   /**< @brief #AWG_IntGetEventStatus*/

/** @} */

/**
    @trace #BRCM_SWREQ_AWG_INIT
    @trace #BRCM_SWARCH_AWG_DRV_INIT_PROC
    @trace #BRCM_SWARCH_AWG_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_ENABLE_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_WAVEFORM_MEM_ACCESS_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CURRENT_PHASE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CURR_ACTIVE_PAGE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_SET_ACTIVE_PAGE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RUN_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_FREQUENCY_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONFIG_PROC
*/
static AWG_RDBType * const AWG_Regs[AWG_MAX_HW_ID] =
{
#if (AWG_MAX_HW_ID == 0U)
#error "AWG_MAX_HW_ID == 0U"
#endif
    (AWG_RDBType *)AWG_BASE,
#if (AWG_MAX_HW_ID > 1U)
#error "AWG_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_AWG_INIT
    @trace #BRCM_SWARCH_AWG_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
*/
static AWG_INTR_RDBType * const AWG_IntrRegs[AWG_MAX_HW_ID] =
{
#if (AWG_MAX_HW_ID == 0U)
#error "AWG_MAX_HW_ID == 0U"
#endif
    (AWG_INTR_RDBType *)AWG_INTR_BASE,
#if (AWG_MAX_HW_ID > 1U)
#error "AWG_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_AWG_INIT
    @trace #BRCM_SWARCH_AWG_DRV_INIT_PROC
    @trace #BRCM_SWARCH_AWG_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_ENABLE_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_WAVEFORM_MEM_ACCESS_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CURRENT_PHASE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CURR_ACTIVE_PAGE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_SET_ACTIVE_PAGE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RUN_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_FREQUENCY_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONFIG_PROC
*/
typedef struct sAWG_DrvDevType {
    AWG_StateType   state;            /**< State of the driver                */
} AWG_DrvDevType;

/**
    @trace #BRCM_SWREQ_AWG_INIT
    @trace #BRCM_SWARCH_AWG_DRV_INIT_PROC
    @trace #BRCM_SWARCH_AWG_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_ENABLE_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_WAVEFORM_MEM_ACCESS_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CURRENT_PHASE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWARCH_AWG_DRV_GET_CURR_ACTIVE_PAGE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_SET_ACTIVE_PAGE_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RUN_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_FREQUENCY_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONTROL_PROC
    @trace #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONFIG_PROC
*/
static AWG_DrvDevType AWG_DrvDev[AWG_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/** @brief Initialize AWG

    This API initializes the given AWG hardware.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the AWG controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_UNINIT  Device or resource not initialized
    @retval     #BCM_ERR_OK      Initialized

    @post AWG driver state changes to #AWG_STATE_INITIALIZED

    @trace  #BRCM_SWREQ_AWG_INIT
    @trace  #BRCM_SWARCH_AWG_DRV_INIT_PROC

    @code{.unparsed}
    for chIndex less than AWG_CHAN_SIZE
       write memory enable
       AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control = AWG_MEM_INIT_CONTROL_CH_RG_INIT_EN_MASK;

    for chIndex less than AWG_CHAN_SIZE
       write memory check
       AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control |= AWG_MEM_INIT_CONTROL_CH_RG_INIT_CHK_EN_MASK ;

    for chIndex less than AWG_CHAN_SIZE
       Release resetB
       AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control |= AWG_MEM_INIT_CONTROL_CH_RG_RSTB_MASK;

    for chIndex less than AWG_CHAN_SIZE
        maxRetry = 200U;
        do until get init done read status
            if AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr &
                    (1U << (chIndex + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT)))
                AWG_DrvDev[hwId].state = AWG_STATE_INITIALIZED;
                break;
        while (maxRetry--)
        if maxRetry less than 0U
            ret = BCM_ERR_UNINIT;
            AWG_DrvDev[hwId].state = AWG_STATE_UNINIT;
            break;
    @endcode
    @limitations None
*/

int32_t AWG_DrvInit(AWG_HwIDType  hwId)
{
    int ret = BCM_ERR_OK, maxRetry = 0;
    uint32_t chIndex = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif

        /* Step 1U Enable Memory for all the AWG channels*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control = AWG_MEM_INIT_CONTROL_CH_RG_INIT_EN_MASK;
        }

        /* Step2 Check enable*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control |= AWG_MEM_INIT_CONTROL_CH_RG_INIT_CHK_EN_MASK ;
        }

        /* Step 3 release all resetb*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control |= AWG_MEM_INIT_CONTROL_CH_RG_RSTB_MASK;
        }

        /* Step 4 Get the status*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            maxRetry = 200;
            do {
                if ((AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr &
                            (1UL << (chIndex + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT))) > 0UL) {
                    AWG_DrvDev[hwId].state = AWG_STATE_INITIALIZED;
                    break;
                }
            } while ((maxRetry--) >= 0);
            if (maxRetry < 0) {
                ret = BCM_ERR_UNINIT;
                AWG_DrvDev[hwId].state = AWG_STATE_UNINIT;
                break;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief De-initialize AWG

    This API de-initializes AWG. Resets the given AWG controller and puts back
    to its original state (same as Power on reset).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the AWG controller

    @return int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_OK             Success

    @post AWG driver state changes to #AWG_STATE_UNINIT

    @trace  #BRCM_SWREQ_AWG_INIT
    @trace  #BRCM_SWARCH_AWG_DRV_DEINIT_PROC

    @code{.unparsed}
    Disable memory for all channels
    Disable memory check
    PutAWG under reset
    @endcode

    @limitations None
*/

int32_t AWG_DrvDeInit (AWG_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
    uint32_t chIndex = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif

        /* Step 1U Disable Memory for all the AWG channels*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control &= ~AWG_MEM_INIT_CONTROL_CH_RG_INIT_EN_MASK;
        }

        /* Step2 Check Disable*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control &= ~AWG_MEM_INIT_CONTROL_CH_RG_INIT_CHK_EN_MASK ;
        }

        /* Step 3 resetb*/
        for (chIndex = 0UL; chIndex < AWG_CHAN_SIZE; chIndex++) {
            AWG_Regs[hwId]->chan[chIndex].ch0_mem_init_control &= ~AWG_MEM_INIT_CONTROL_CH_RG_RSTB_MASK;
        }

        AWG_DrvDev[hwId].state = AWG_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}


/** @brief AWG channel configuration

    This API configures AWG channel with user specified configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the AWG controller
    @param[in]      chId         Index of the AWG channel
    @param[in]      chConfig   channel configuration

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_PROC

    @code{.unparsed}
    #ifdef BCM8915X_PARAM_VALIDATION
        if hwId greater than AWG_MAX_HW_ID
            ret = BCM_ERR_INVAL_PARAMS
        else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
            ret = BCM_ERR_NODEV
        else if chId greater than equal AWG_CHAN_SIZE
          ret = BCM_ERR_INVAL_PARAMS;
        else if (chConfig->incrementMode greater AWG_INCREMENT_MODE_PHASE_INCREMENT
          ret = BCM_ERR_INVAL_PARAMS;
        else if (chConfig->numberPages greater AWG_CHANNEL_NUM_PAGES_TWO
          ret = BCM_ERR_INVAL_PARAMS
        else if (chConfig->activePageLatch greater 1U
          ret = BCM_ERR_INVAL_PARAMS;
        else if (chConfig->incrementCtrl greater 1U
          ret = BCM_ERR_INVAL_PARAMS;
        else if (chConfig->syncCtrl greater 1U
          ret = BCM_ERR_INVAL_PARAMS;
        else if (chConfig->rampResetCtrl greater 1U
          ret = BCM_ERR_INVAL_PARAMS;
        else
    #endif
            chCtrl = AWG_Regs[hwId]->chan[chId].ch0_control;
            chCtrl &= ~(AWG_CH_CONTROL_CLKDIV_MASK | AWG_CH_CONTROL_INCR_MODE_MASK | AWG_CH_CONTROL_NUM_PAGES_MASK |
                    AWG_CH_CONTROL_ACTIVE_PAGE_LATCH_CTRL_MASK | AWG_CH_CONTROL_RAMP_RST_CTRL_MASK| AWG_CH_CONTROL_SYNC_CTRL_MASK
                    | AWG_CH_CONTROL_HW_INCR_CTRL_MASK);

            chCtrl |= ((chConfig->incrementMode << AWG_CH_CONTROL_INCR_MODE_SHIFT) |
                       (chConfig->numberPages << AWG_CH_CONTROL_NUM_PAGES_SHIFT) |
                       (chConfig->activePageLatch << AWG_CH_CONTROL_ACTIVE_PAGE_LATCH_CTRL_SHIFT) |
                       (chConfig->rampResetCtrl << AWG_CH_CONTROL_RAMP_RST_CTRL_SHIFT) |
                       (chConfig->syncCtrl << AWG_CH_CONTROL_SYNC_CTRL_SHIFT) |
                       (chConfig->clockDivider << AWG_CH_CONTROL_CLKDIV_SHIFT) |
                       (chConfig->incrementMode == 1U ? (chConfig->incrementCtrl << AWG_CH_CONTROL_HW_INCR_CTRL_SHIFT) : 0U));

            AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl;

    @endcode
    @limitations None
*/
int32_t AWG_DrvChannelConfig (AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_ChannelConfigType *chConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t chCtrl = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chConfig->incrementMode > AWG_INCREMENT_MODE_PHASE_INCREMENT) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chConfig->numberPages > AWG_CHANNEL_NUM_PAGES_TWO) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chConfig->activePageLatch > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chConfig->incrementCtrl > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chConfig->syncCtrl > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chConfig->rampResetCtrl > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        chCtrl = AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_CLKDIV_MASK | AWG_CH_CONTROL_INCR_MODE_MASK | AWG_CH_CONTROL_NUM_PAGES_MASK |
                AWG_CH_CONTROL_ACTIVE_PAGE_LATCH_CTRL_MASK | AWG_CH_CONTROL_RAMP_RST_CTRL_MASK| AWG_CH_CONTROL_SYNC_CTRL_MASK
                | AWG_CH_CONTROL_HW_INCR_CTRL_MASK);

        chCtrl |= (((uint32_t)chConfig->incrementMode << AWG_CH_CONTROL_INCR_MODE_SHIFT) |
                   ((uint32_t)chConfig->numberPages << AWG_CH_CONTROL_NUM_PAGES_SHIFT) |
                   ((uint32_t)chConfig->activePageLatch << AWG_CH_CONTROL_ACTIVE_PAGE_LATCH_CTRL_SHIFT) |
                   ((uint32_t)chConfig->rampResetCtrl << AWG_CH_CONTROL_RAMP_RST_CTRL_SHIFT) |
                   ((uint32_t)chConfig->syncCtrl << AWG_CH_CONTROL_SYNC_CTRL_SHIFT) |
                   ((uint32_t)chConfig->clockDivider << AWG_CH_CONTROL_CLKDIV_SHIFT) |
                   ((chConfig->incrementMode == 1UL) ? ((uint32_t)chConfig->incrementCtrl << AWG_CH_CONTROL_HW_INCR_CTRL_SHIFT) : 0UL));

        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief AWG Ramp Configration

    This API configures ramp on a AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      rampIncrementVal     Ramp increment value for AWG Channel
    @param[in]      defaultRampVal Default ramp value for AWG channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONFIG_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater or equal AWG_MAX_HW_ID
      ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId  greater or equal AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if rampIncrementVal  greater 0xFFFU
      ret = BCM_ERR_INVAL_PARAMS;
    else if defaultRampVal greater 0xFFFFU
      ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        AWG_Regs[hwId]->chan[chId].ch0_ramp_control = (defaultRampVal << AWG_CH_RAMP_CONTROL_RAMP_INCR_VAL_SHIFT) |
                                                         rampIncrementVal ;
    @endcode
    @limitations None
*/
int32_t AWG_DrvChannelRampConfig(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint16_t rampIncrementVal, uint16_t defaultRampVal)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (rampIncrementVal > 0xFFFU) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        AWG_Regs[hwId]->chan[chId].ch0_ramp_control = ((uint32_t)defaultRampVal << AWG_CH_RAMP_CONTROL_RAMP_INCR_VAL_SHIFT) |
                                                         rampIncrementVal ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief AWG Ramp control

    This API enables/disables ramp on a AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      rampCtrl      Enable(1)/Disable(0) ramp on channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_CHANNEL_RAMP_CONTROL_PROC
    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater or equal AWG_MAX_HW_ID
      ret = BCM_ERR_INVAL_PARAMS;
    else if (AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater equal AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS
    else if rampCtrl greater 1
      ret = BCM_ERR_INVAL_PARAMS
    else
#endif
        chCtrl =  AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_RAMP_EN_MASK);
        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl | (rampCtrl << AWG_CH_CONTROL_RAMP_EN_SHIFT);
    @endcode
    @limitations None
*/

int32_t AWG_DrvChannelRampControl(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t rampCtrl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t chCtrl = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (rampCtrl > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        chCtrl =  AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_RAMP_EN_MASK);
        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl | ((uint32_t)rampCtrl << AWG_CH_CONTROL_RAMP_EN_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief AWG channel frequency

    This API configures channel frequency. This is valid for Phase Accumulator mode only.
    It specifies the step through which the accumulator increments on every clock cycle.

    @behavior Sync, Non-reentrant

    @pre None

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_CHANNEL_CONFIG_FREQUENCY_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if frequency greater AWG_CH_FREQ_MASK
      ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        AWG_Regs[hwId]->chan[chId].ch0_freq = frequency;
    @endcode
    @limitations None
*/
int32_t AWG_DrvChannelConfigFrequency(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint32_t frequency)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (frequency > AWG_CH_FREQ_MASK) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        AWG_Regs[hwId]->chan[chId].ch0_freq = frequency;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief AWG channel control

    This API enables/disables AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      chControl Enables/Disables AWG channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success


    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_CHANNEL_ENABLE_CONTROL_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater than or equals to AWG_MAX_HW_ID
      ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if (chControl.enable greater 1U
      ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        chCtrl =  AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_EN_MASK);
        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl | (chControl.enable <<AWG_CH_CONTROL_EN_SHIFT);
    @endcode
    @limitations None
*/
int32_t AWG_DrvChannelEnableControl(AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_ChannelControlType chControl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t chCtrl = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chControl.enable > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        chCtrl =  AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_EN_MASK);
        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl | ((uint32_t)chControl.enable <<AWG_CH_CONTROL_EN_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief AWG channel run control

    This API pauses/resumes AWG channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the AWG controller
    @param[in]      chId          Index of the AWG channel
    @param[in]      runControl    Pauses/Resumes AWG channel. 0 - Pause 1 - Resume

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_CHANNEL_RUN_CONTROL_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater than or equals to AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT)
        ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
        ret = BCM_ERR_INVAL_PARAMS;
    else if chRunControl greater 1
        ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        chCtrl = AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_RUN_EN_MASK);
        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl | (chRunControl << AWG_CH_CONTROL_RUN_EN_SHIFT);

    @endcode
    @limitations None
*/

int32_t AWG_DrvChannelRunControl(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t chRunControl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t chCtrl = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chRunControl > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        chCtrl = AWG_Regs[hwId]->chan[chId].ch0_control;
        chCtrl &= ~(AWG_CH_CONTROL_RUN_EN_MASK);
        AWG_Regs[hwId]->chan[chId].ch0_control = chCtrl | ((uint32_t)chRunControl << AWG_CH_CONTROL_RUN_EN_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief AWG waveform memory configuration
    This API reads/writes AWG waveform memory based on channel.
    Write always uses auto address increment mode. Writes all the data on a page
    based on number of pages selected on @ref AWG_DrvChannelConfig.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the AWG controller
    @param[in]      chId                Index of the AWG channel
    @param[in]      chWaveFormMem       Read/write data

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_WAVEFORM_MEM_CONFIG
    @trace  #BRCM_SWARCH_AWG_DRV_WAVEFORM_MEM_ACCESS_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater than or equals to AWG_MAX_HW_ID
      ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId  greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS
    else if chWaveFormMem->opMode greater AWG_WAVEFORM_MEM_OP_WRITE
      ret = BCM_ERR_INVAL_PARAMS;
    else if chWaveFormMem->pageNumber greater 1U
      ret = BCM_ERR_INVAL_PARAMS;
    else if Not chWaveFormMem->data)
      ret = BCM_ERR_INVAL_PARAMS;
    else if chWaveFormMem->numberOfLocation greater 8192U
      ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        address = (chWaveFormMem->pageNumber == 0U) ? 0U : 4096U;
        if chWaveFormMem->opMode == AWG_WAVEFORM_MEM_OP_WRITE
            AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_wr_control = (1U << AWG_ACWMIWC_CH_WF_MEM_IND_WR_INCR_MODE_SHIFT) | address;
            for ( chWaveFormMem->numberOfLocation )
                AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_wr_data = (chWaveFormMem->data[index]);
        else Read from Waveform memory
            AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_rd_control = (1U << AWG_ACWMIRC_CH_WF_MEM_IND_RD_INCR_MODE_SHIFT) | address;
            for (chWaveFormMem->numberOfLocation )
                chWaveFormMem->data[index] = AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_rd_data;

    @endcode
    @limitations None
*/
int32_t AWG_DrvWaveformMemAccess(AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_ChannelWaveformMemType *chWaveFormMem)
{
    int32_t ret = BCM_ERR_OK;
    uint16_t index = 0U, address = 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chWaveFormMem->opMode > AWG_WAVEFORM_MEM_OP_WRITE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chWaveFormMem->pageNumber >  1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chWaveFormMem->data == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chWaveFormMem->numberOfLocation > 8192U) {/* Calculated based on 13-bit address*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        address = (chWaveFormMem->pageNumber == 0U) ? 0U : 4096U;
        if (chWaveFormMem->opMode == AWG_WAVEFORM_MEM_OP_WRITE) {  /* Write to Waveform Memory*/
            AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_wr_control = (1UL << AWG_ACWMIWC_CH_WF_MEM_IND_WR_INCR_MODE_SHIFT) | address;
            for (index = 0U; index < chWaveFormMem->numberOfLocation ; index ++) {
                AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_wr_data = (chWaveFormMem->data[index]);
            }
        } else {   /* Read from Waveform memory*/
            AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_rd_control = (1UL << AWG_ACWMIRC_CH_WF_MEM_IND_RD_INCR_MODE_SHIFT) | address;
            for (index = 0U; index < chWaveFormMem->numberOfLocation ; index ++) {
                chWaveFormMem->data[index] = AWG_Regs[hwId]->chan[chId].ch0_wf_mem_ind_rd_data;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief AWG get current active page

    This API enables user to get current active page.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the AWG controller
    @param[in]      chId              Index of the AWG channel
    @param[out]     currActivePage    Current Active page. 0 - Page0 1 - Page1

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_SET_ACTIVE_PAGE_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equal AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater equal AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if Not currActivePage
      ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
    *currActivePage = (AWG_Regs[hwId]->chan[chId].ch0_active_page_control >> AWG_CH_ACTIVE_PAGE_CONTROL_CURR_ACTIVE_PAGE_SHIFT)

    @endcode
    @limitations None
*/
int32_t AWG_DrvGetCurrActivePage(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t *currActivePage)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (currActivePage == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    *currActivePage = ((AWG_Regs[hwId]->chan[chId].ch0_active_page_control & AWG_CH_ACTIVE_PAGE_CONTROL_CURR_ACTIVE_PAGE_MASK) > 0U) ? 1U : 0U ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief AWG Set channel current active page

    This API configures AWG channel active page. This API can be used only if
    numberPages is AWG_CHANNEL_NUM_PAGES_TWO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the AWG controller
    @param[in]      chId              Index of the AWG channel
    @param[in]      activePage        Set channel Active page. 0 - Page0 1 - Page1

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None
    @trace  #BRCM_SWREQ_AWG_CH_CONFIGURATION
    @trace  #BRCM_SWARCH_AWG_DRV_SET_ACTIVE_PAGE_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if activePage greater 1U
      ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
    AWG_Regs[hwId]->chan[chId].ch0_active_page_control = activePage;

    @endcode
    @limitations None
*/
int32_t AWG_DrvSetActivePage(AWG_HwIDType  hwId, AWG_ChIDType  chId, uint8_t activePage)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (activePage > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    AWG_Regs[hwId]->chan[chId].ch0_active_page_control = activePage;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief AWG gets current phase information

    This API gets AWG channel current phase information.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG channel
    @param[out]     AWG_CurPhase         Current phase of the specified channel

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post NONE

    @trace  #BRCM_SWREQ_AWG_GET_PHASE
    @trace  #BRCM_SWARCH_AWG_DRV_GET_CURRENT_PHASE_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if NOT AWG_CurPhase
        ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
    *AWG_CurPhase =  (AWG_Regs[hwId]->chan[chId].ch0_curr_phase)

    @endcode
    @limitations None
*/
int32_t AWG_DrvGetCurrentPhase(AWG_HwIDType  hwId, AWG_ChIDType  chId, AWG_CurrentPhaseType *curPhase)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (curPhase == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    *curPhase =  (AWG_Regs[hwId]->chan[chId].ch0_curr_phase) ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief AWG Enable Channel interrupt

    This API enables/disables AWG channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG Channel
    @param[in]      intrType             AWG interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post   None

    @trace  #BRCM_SWREQ_AWG_INTERRUPT
    @trace  #BRCM_SWARCH_AWG_DRV_CONFIG_CH_INTR_PROC

    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if enaDis greater 1U
        ret = BCM_ERR_INVAL_PARAMS;
    else if intrType greater AWG_CHANNEL_INTR_ALL
        ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        switch (intrType)
            case AWG_CHANNEL_INTR_MEM_SEC:
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~(1UL << chId);
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << chId);
            break;
            case AWG_CHANNEL_INTR_MEM_DED:
                AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier &= ~(1UL << chId);
                AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier |= ((uint32_t)enaDis << chId);
            break;
            case AWG_CHANNEL_INTR_MEM_INIT_ERR:
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~(1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT));
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT));
            break;
            case AWG_CHANNEL_INTR_MEM_INIT_DONE:
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~(1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT));
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT)) ;
            break;
            case AWG_CHANNEL_INTR_PG_SWITCH_DONE:
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~1UL;
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= enaDis ;
            break;
            case AWG_CHANNEL_INTR_RESET:
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~(1UL << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT);
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= (uint32_t)enaDis  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT;
            break;
            default:
            if (intrType == AWG_CHANNEL_INTR_ALL)
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~1UL;
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= enaDis;

              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~(1UL << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT);
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= ((uint32_t)enaDis  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT) ;

              AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier &= ~(1UL << chId);
              AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier |= ((uint32_t)enaDis << chId);
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~((1UL << chId) | (1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT)) |
                                                                      (1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT)));
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |=  ((uint32_t)enaDis << chId) ;
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT));
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT));
            break;
    @endcode
    @limitations None
*/
/* @event           CERT-C INT34
   @issue          Coverity infers that the shifting cause overflow
   @risk           No risk
   @justification  chId got validated, so it will not cause overflow.*/
 
int32_t AWG_DrvConfigChIntr(AWG_HwIDType  hwId,  AWG_ChIDType chId, AWG_ChannelIntrType intrType, uint8_t enaDis)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (enaDis > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (intrType > AWG_CHANNEL_INTR_ALL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        switch (intrType) {
            case AWG_CHANNEL_INTR_MEM_SEC:
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~(1UL << chId);
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << chId);
            break;
            case AWG_CHANNEL_INTR_MEM_DED:
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier &= ~(1UL << chId);
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier |= ((uint32_t)enaDis << chId);
            break;
            case AWG_CHANNEL_INTR_MEM_INIT_ERR:
                 /* @event           CERT-C INT30
                             @issue          Coverity infers that the addition would overflow
                             @risk           No risk
                             @justification  chId got validated.*/
                 /* coverity[cert_int30_c_violation] */
                 /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~(1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT));
                /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT));
            break;
            case AWG_CHANNEL_INTR_MEM_INIT_DONE:
                 /* coverity[cert_int30_c_violation] */
                 /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~(1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT));
                 /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT)) ;
            break;
            case AWG_CHANNEL_INTR_PG_SWITCH_DONE:
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~1UL;
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= enaDis ;
            break;
            case AWG_CHANNEL_INTR_RESET:
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~(1UL << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT);
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= (uint32_t)enaDis  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT;
            break;
            default:
            if (intrType == AWG_CHANNEL_INTR_ALL) {
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~1UL;
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= enaDis;

              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier &= ~(1UL << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT);
                /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eier |= ((uint32_t)enaDis  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EIER_ENABLE_RESET_INTR_SHIFT) ;

              /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier &= ~(1UL << chId);
              /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->acq_awg_ded_intr_eier |= ((uint32_t)enaDis << chId);
              /* coverity[cert_int30_c_violation] */
              /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier &= ~((1UL << chId) | (1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT))| (1UL << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT)));
              /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |=  ((uint32_t)enaDis << chId) ;
              /* coverity[cert_int30_c_violation] */
              /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_ERR_INTR_SHIFT));
              /* coverity[cert_int30_c_violation] */
              /* coverity[cert_int34_c_violation] */
              AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eier |= ((uint32_t)enaDis << (chId + AWG_INTR_AAMISIE2_ENABLE_CH0_MEM_INIT_DONE_INTR_SHIFT));

            }
            break;
        }


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief AWG clears Channel interrupt

    This API clears AWG channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG Channel
    @param[in]      AWG_IntType          AWG interrupt type

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post   NA

    @trace  #BRCM_SWREQ_AWG_INTERRUPT
    @trace  #BRCM_SWARCH_AWG_DRV_CLEAR_CH_INTR_PROC
    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if intrType greater AWG_CHANNEL_INTR_ALL
        ret = BCM_ERR_INVAL_PARAMS;
    if intrType == AWG_CHANNEL_INTR_ALL
         Claer MEM_SEC, MEM_INIT_ERR, MEM_INIT_DONE
        AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr = (1U << chId) | (1U << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_ERR_INTR_W2CLR_SHIFT)) |
            (1U << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT));
         Clear DED INTR
        AWG_IntrRegs[hwId]->acq_awg_ded_intr_eisr |= 1U << chId;
         Clear SWitch Done INTR, INTR_RESET
        AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr = (1U |  (1U  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EISR_RESET_INTR_W2CLR_SHIFT)) ;
    else
        if intrType & AWG_CHANNEL_INTR_MEM_SEC
           AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr |= 1U << chId;
        if intrType & AWG_CHANNEL_INTR_MEM_DED
           AWG_IntrRegs[hwId]->acq_awg_ded_intr_eisr |= 1U << chId;
        if intrType & AWG_CHANNEL_INTR_MEM_INIT_ERR
           AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr |= 1U << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_ERR_INTR_W2CLR_SHIFT);
        if intrType & AWG_CHANNEL_INTR_MEM_INIT_DONE
           AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr |= 1U << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT);
        if intrType & AWG_CHANNEL_INTR_PG_SWITCH_DONE
           AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr |= 1U ;
        if intrType & AWG_CHANNEL_INTR_RESET
            AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr |= 1U  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EISR_RESET_INTR_W2CLR_SHIFT;

#endif
    @endcode
    @limitations None
*/
/* @event           CERT-C INT34
   @issue          Coverity infers that the shifting cause overflow
   @risk           No risk
   @justification  chId got validated, so it will not cause overflow.*/
 
int32_t AWG_DrvClearChIntr(AWG_HwIDType  hwId,  AWG_ChIDType chId, AWG_ChannelIntrType intrType)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (intrType > AWG_CHANNEL_INTR_ALL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (intrType == AWG_CHANNEL_INTR_ALL) {
            /* Clear MEM_SEC, MEM_INIT_ERR, MEM_INIT_DONE*/
            /* @event           CERT-C INT30
                        @issue          Coverity infers that the addition would overflow
                        @risk           No risk
                        @justification  chId got validated.*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr = (1UL << chId) | (1UL << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_ERR_INTR_W2CLR_SHIFT)) | (1UL << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT));
            /* Clear DED INTR*/
            /* coverity[cert_int34_c_violation] */
            AWG_IntrRegs[hwId]->acq_awg_ded_intr_eisr |= 1UL << chId;
            /* Clear SWitch Done INTR, INTR_RESET*/
            AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr = (1UL |  (1UL  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EISR_RESET_INTR_W2CLR_SHIFT)) ;
        } else {
            if ((intrType & AWG_CHANNEL_INTR_MEM_SEC) > 0UL) {
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr |= (1UL << chId);
            }
            if ((intrType & AWG_CHANNEL_INTR_MEM_DED) > 0UL) {
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_ded_intr_eisr |= (1UL << chId);
            }
            if ((intrType & AWG_CHANNEL_INTR_MEM_INIT_ERR) > 0UL) {
                 /* coverity[cert_int30_c_violation] */
                 /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr |= 1UL << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_ERR_INTR_W2CLR_SHIFT);
            }
            if ((intrType & AWG_CHANNEL_INTR_MEM_INIT_DONE) > 0UL) {
                 /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr |= 1UL << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT);
            }
            if ((intrType & AWG_CHANNEL_INTR_PG_SWITCH_DONE) > 0UL) {
                AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr |= 1UL ;
            }
            if ((intrType & AWG_CHANNEL_INTR_RESET) > 0UL) {
                /* coverity[cert_int34_c_violation] */
                AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr |= (1UL  << AWG_INTR_ACQ_AWG0_STATUS_INTR_EISR_RESET_INTR_W2CLR_SHIFT);
            }
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/**
    @brief Get all the Interrupt status

    @trace  #BRCM_SWREQ_AWG_INTERRUPT
    @trace  #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
*/

/* @event           CERT-C INT34
   @issue          Coverity infers that the shifting cause overflow
   @risk           No risk
   @justification  chId got validated in the calling function*/
 
static void AWG_IntGetAllInterruptStatus (AWG_HwIDType  hwId, AWG_ChIDType chId, uint32_t *aStatus)
{
    uint32_t status = 0;

    /* Read MEM_SEC, MEM_INIT_ERR, MEM_INIT_DONE*/
    status = AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr;
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    *aStatus = (((status & (1UL << chId)) > 0UL) ? AWG_CHANNEL_INTR_MEM_SEC : 0UL) | (((status & (1UL << (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_ERR_INTR_SHIFT))) > 0UL) ? AWG_CHANNEL_INTR_MEM_INIT_ERR : 0UL) | (((status & (1UL << (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_DONE_INTR_SHIFT))) > 0UL) ? AWG_CHANNEL_INTR_MEM_INIT_DONE : 0UL);
    /* Read DED INTR*/
    /* coverity[cert_int34_c_violation] */
    *aStatus |= ((AWG_IntrRegs[hwId]->acq_awg_ded_intr_eipr & (1UL << chId)) > 0UL) ? AWG_CHANNEL_INTR_MEM_DED : 0UL;
    /* Read Switch Done INTR, INTR_RESET*/
    status = AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eipr;
    *aStatus |= (((status & 1U) > 0U)? AWG_CHANNEL_INTR_PG_SWITCH_DONE : 0UL) |
        (((status & AWG_INTR_ACQ_AWG0_STATUS_INTR_EIPR_P_RESET_INTR_MASK) > 0UL)? AWG_CHANNEL_INTR_RESET : 0UL);

}

/**
    @brief Get all the event status

    @trace  #BRCM_SWREQ_AWG_INTERRUPT
    @trace  #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
*/
static void AWG_IntGetEventStatus (AWG_HwIDType  hwId, AWG_ChIDType chId, uint32_t *aStatus)
{
    uint32_t status = 0;
    /* Read MEM_SEC, MEM_INIT_ERR, MEM_INIT_DONE event*/
    status = AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eisr;

    /* coverity[cert_int34_c_violation] */
    *aStatus = (((status & (1UL << chId)) > 0UL) ? AWG_CHANNEL_INTR_MEM_SEC :0UL) |
        (((status & (1UL << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_ERR_INTR_W2CLR_SHIFT))) > 0UL) ? AWG_CHANNEL_INTR_MEM_INIT_ERR : 0UL) |
        (((status & (1UL << (chId + AWG_INTR_AAMISIE1_CH0_MEM_INIT_DONE_INTR_W2CLR_SHIFT))) > 0UL) ? AWG_CHANNEL_INTR_MEM_INIT_DONE : 0UL);
    /* Read DED INTR*/
    *aStatus |= ((AWG_IntrRegs[hwId]->acq_awg_ded_intr_eisr & (1UL << chId)) > 0UL) ? AWG_CHANNEL_INTR_MEM_DED : 0UL;
    /* Read Switch Done INTR, INTR_RESET*/
    /* coverity[cert_int34_c_violation] */
    status = AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eisr;
    *aStatus |= (((status & 1UL) > 0UL) ? AWG_CHANNEL_INTR_PG_SWITCH_DONE : 0UL) |
        (((status & AWG_INTR_ACQ_AWG0_STATUS_INTR_EISR_RESET_INTR_W2CLR_MASK) > 0UL) ? AWG_CHANNEL_INTR_RESET : 0UL) ;
}

/** @brief AWG Get Channel interrupt Status

    This API gets AWG channel based interrupts status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the AWG controller
    @param[in]      chId                 Index of the AWG Channel
    @param[in]      AWG_IntType          AWG interrupt/Event type
                                         User can use AWG_CHANNEL_INTR_EVENT_ALL to get HW event for all the events @ref AWG_ChannelIntrType
    @param[out]     intrStatus           AWG interrupt Status

    @return int32_t
    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post   None

    @trace  #BRCM_SWREQ_AWG_INTERRUPT
    @trace  #BRCM_SWARCH_AWG_DRV_GET_CH_INTR_STS_PROC
    @code{.unparsed}
#ifdef BCM8915X_PARAM_VALIDATION
    if hwId greater equal AWG_MAX_HW_ID
        ret = BCM_ERR_INVAL_PARAMS;
    else if AWG_DrvDev[hwId].state equals AWG_STATE_UNINIT
      ret = BCM_ERR_NODEV;
    else if chId greater than or equals to AWG_CHAN_SIZE
      ret = BCM_ERR_INVAL_PARAMS;
    else if intrType greater AWG_CHANNEL_INTR_ALL
        ret = BCM_ERR_INVAL_PARAMS;
    else
#endif
        *intrStatus = 0;
        switch intrType
            case AWG_CHANNEL_INTR_ALL:
                AWG_IntGetAllInterruptStatus(hwId, chId, intrStatus);
            break;
            case AWG_CHANNEL_INTR_MEM_SEC:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr & (1UL << chId)) >> (chId);
                break;
            case AWG_CHANNEL_INTR_MEM_DED:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_ded_intr_eipr & (1UL << chId)) >> chId;
                break;
            case AWG_CHANNEL_INTR_MEM_INIT_ERR:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr & (1UL << (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_ERR_INTR_SHIFT))) >>
                                (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_ERR_INTR_SHIFT);
                break;
            case AWG_CHANNEL_INTR_MEM_INIT_DONE:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr & (1UL << (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_DONE_INTR_SHIFT))) >>
                               (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_DONE_INTR_SHIFT);
                break;
            case AWG_CHANNEL_INTR_PG_SWITCH_DONE:
                *intrStatus = (AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eipr & 1UL) ;
                break;
            case AWG_CHANNEL_INTR_RESET:
                *intrStatus = (AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eipr & AWG_INTR_ACQ_AWG0_STATUS_INTR_EIPR_P_RESET_INTR_MASK) >>
                               AWG_INTR_ACQ_AWG0_STATUS_INTR_EIPR_P_RESET_INTR_SHIFT;
                break;
            case AWG_CHANNEL_INTR_EVENT_ALL:
                AWG_IntGetEventStatus (hwId, chId, intrStatus) ;
                break;
            default:
                AWG_IntGetAllInterruptStatus(hwId, chId, intrStatus);
            break;

    @endcode
    @limitations None
*/

int32_t AWG_DrvGetChIntrStatus(AWG_HwIDType  hwId,  AWG_ChIDType chId, AWG_ChannelIntrType intrType, uint32_t *intrStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= AWG_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chId >= AWG_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if ((intrType > AWG_CHANNEL_INTR_ALL) && (intrType != AWG_CHANNEL_INTR_EVENT_ALL)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (AWG_DrvDev[hwId].state == AWG_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        *intrStatus = 0;
        switch (intrType) {
            case AWG_CHANNEL_INTR_ALL:
                AWG_IntGetAllInterruptStatus(hwId, chId, intrStatus);
            break;
#ifdef BCM8915X_INDIVIDUAL_INTR
            case AWG_CHANNEL_INTR_MEM_SEC:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr & (1UL << chId)) >> (chId);
                break;
            case AWG_CHANNEL_INTR_MEM_DED:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_ded_intr_eipr & (1UL << chId)) >> chId;
                break;
            case AWG_CHANNEL_INTR_MEM_INIT_ERR:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr & (1UL << (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_ERR_INTR_SHIFT))) >>
                                (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_ERR_INTR_SHIFT);
                break;
            case AWG_CHANNEL_INTR_MEM_INIT_DONE:
                *intrStatus = (AWG_IntrRegs[hwId]->acq_awg_mem_init_sec_intr_eipr & (1UL << (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_DONE_INTR_SHIFT))) >>
                               (chId + AWG_INTR_AAMISIE_P_CH0_MEM_INIT_DONE_INTR_SHIFT);
                break;
            case AWG_CHANNEL_INTR_PG_SWITCH_DONE:
                *intrStatus = (AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eipr & 1UL) ;
                break;
            case AWG_CHANNEL_INTR_RESET:
                *intrStatus = (AWG_IntrRegs[hwId]->chan[chId].acq_awg0_status_intr_eipr & AWG_INTR_ACQ_AWG0_STATUS_INTR_EIPR_P_RESET_INTR_MASK) >>
                               AWG_INTR_ACQ_AWG0_STATUS_INTR_EIPR_P_RESET_INTR_SHIFT;
                break;
#endif
            case AWG_CHANNEL_INTR_EVENT_ALL:
                AWG_IntGetEventStatus (hwId, chId, intrStatus) ;
                break;
            default:
                AWG_IntGetAllInterruptStatus(hwId, chId, intrStatus);
            break;
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }

#endif
    return ret;

}

/** @} */


