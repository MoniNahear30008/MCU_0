/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_hkadcdrv_drv_impl House Keeping ADC Driver Design
    @ingroup grp_hkadcdrv

    @addtogroup grp_hkadcdrv_drv_impl
    @{

    @file drivers/hkadc/hkadc_drv.c
    @brief HKADC Driver Design

    @version 0.1 Initial version
*/

#include <hkadc.h>
#include <hkadc_rdb.h>
#include <hkadc_intr_rdb.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>

/**
    @name HKADC Driver Design IDs
    @{
    @brief Driver Design IDs for HKADC

*/
#define BRCM_SWDSGN_HKADC_DRV_INIT_PROC               (0xB101U)    /**< @brief #HKADC_DrvInit          */
#define BRCM_SWDSGN_HKADC_DRV_DEINIT_PROC             (0xB102U)    /**< @brief #HKADC_DrvDeInit        */
#define BRCM_SWDSGN_HKADC_DRV_CAPTURE_CONFIG_PROC     (0xB103U)    /**< @brief #HKADC_DrvCaptureConfig */
#define BRCM_SWDSGN_HKADC_DRV_CAPTURE_CONTROL_PROC    (0xB104U)    /**< @brief #HKADC_DrvCaptureControl*/
#define BRCM_SWDSGN_HKADC_DRV_FIFO_CFG_PROC           (0xB105U)    /**< @brief #HKADC_DrvFifoConfig    */
#define BRCM_SWDSGN_HKADC_DRV_GET_FIFO_DATA_PROC      (0xB106U)    /**< @brief #HKADC_DrvGetFifoData   */
#define BRCM_SWDSGN_HKADC_DRV_COMP_CFG_PROC           (0xB107U)    /**< @brief #HKADC_DrvComparatorConfig  */
#define BRCM_SWDSGN_HKADC_DRV_COMP_CTRL_PROC          (0xB108U)    /**< @brief #HKADC_DrvComparatorControl */
#define BRCM_SWDSGN_HKADC_DRV_CFG_CALIB_PROC          (0xB109U)    /**< @brief #HKADC_DrvConfigCalibration */
#define BRCM_SWDSGN_HKADC_DRV_GET_CALIB_STS_PROC      (0xB10AU)    /**< @brief #HKADC_DrvGetCalibStatus    */
#define BRCM_SWDSGN_HKADC_DRV_CALIB_CTRL_PROC         (0xB10BU)    /**< @brief #HKADC_DrvCalibrationControl*/
#define BRCM_SWDSGN_HKADC_DRV_CFG_CH_INTR_PROC        (0xB10CU)    /**< @brief #HKADC_DrvConfigChIntr      */
#define BRCM_SWDSGN_HKADC_DRV_CLEAR_CH_INTR_PROC      (0xB10DU)    /**< @brief #HKADC_DrvClearChIntr */
#define BRCM_SWDSGN_HKADC_DRV_GET_CH_INTR_STS_PROC    (0xB10EU)    /**< @brief #HKADC_DrvGetChIntrStatus   */
#define BRCM_SWDSGN_HKADC_INT_GET_ALL_INT_STS_PROC    (0xB10FU)    /**< @brief #HKADC_IntGetAllInterruptStatus*/
#define BRCM_SWDSGN_HKADC_INT_GET_EVENT_STS_PROC      (0xB110U)    /**< @brief #HKADC_IntGetEventStatus*/

#define BRCM_SWDSGN_HKADC_REGS_GLOBAL                  (0xB112U)    /**< @brief #HKADC_Regs */
#define BRCM_SWDSGN_HKADC_INTR_REGS_GLOBAL             (0xB113U)    /**< @brief #HKADC_IntrRegs */
#define BRCM_SWDSGN_HKADC_DRV_DEV_TYPE                 (0xB114U)    /**< @brief #HKADC_DrvDevType */
#define BRCM_SWDSGN_HKADC_DRV_DEV_GLOBAL               (0xB115U)    /**< @brief #HKADC_DrvDev */
#define BRCM_SWDSGN_HKADC_DRV_SEQSPICFG_PROC           (0xB116U)    /**< @brief #HKADC_DrvSeqSpiCfg*/
#define BRCM_SWDSGN_HKADC_DRV_SEQCONTROL_PROC          (0xB117U)    /**< @brief #HKADC_DrvSeqControl*/
#define BRCM_SWDSGN_HKADC_DRV_SEQMEMRDWR_PROC          (0xB118U)    /**< @brief #HKADC_DrvSeqMemRdWr*/
#define BRCM_SWDSGN_HKADC_DRV_SWSPIRDWR_PROC           (0xB119U)    /**< @brief #HKADC_DrvSwSpiRdWr*/
#define BRCM_SWDSGN_HKADC_DRV_SEQ_TYPE                 (0xB11AU)    /**< @brief #HKADC_DrvSeqType */
#define BRCM_SWDSGN_HKADC_SEQREGS_GLOBAL               (0xB11BU)    /**< @brief #HKADC_SeqRegs */

/** @} */

/**
    @trace #BRCM_SWARCH_HKADC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONFIG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_FIFO_CONFIG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CFG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CFG_CAL_PROC
    @trace #BRCM_SWARCH_HKADC_GET_CAL_STS_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_FIFO_DATA_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAL_CONTROL_PROC
    @trace #BRCM_SWREQ_HKADC_INIT
*/
static HKADC_RDBType * const HKADC_Regs[HKADC_MAX_HW_ID] =
{
#if (HKADC_MAX_HW_ID == 0U)
#error "HKADC_MAX_HW_ID == 0U"
#endif
    (HKADC_RDBType *)HKADC_BASE,
#if (HKADC_MAX_HW_ID > 1U)
#error "HKADC_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWARCH_HKADC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_FIFO_DATA_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWREQ_HKADC_INTERRUPT
*/
static HKADC_INTR_RDBType * const HKADC_IntrRegs[HKADC_INTR_MAX_HW_ID] =
{
#if (HKADC_INTR_MAX_HW_ID == 0U)
#error "HKADC_INTR_MAX_HW_ID == 0U"
#endif
    (HKADC_INTR_RDBType *)HKADC_INTR_BASE,
#if (HKADC_INTR_MAX_HW_ID > 1U)
#error "HKADC_INTR_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_HKADC_INIT
    @trace #BRCM_SWARCH_HKADC_DRV_SEQSPICFG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_SEQCONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_SEQMEMRDWR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_SWSPIRDWR_PROC
*/
typedef volatile struct sHKADC_DrvSeqType {
    HKADC_TXSLZR_SEQ_CONTROL_TYPE             seqCtrl;
    HKADC_TXSLZR_SEQ_MEM_IND_WR_CONTROL_TYPE  memIndWrCtrl;
    HKADC_SEQ_MEM_IND_WR_DATA0_TYPE           memIndWrData0;
    HKADC_SEQ_MEM_IND_WR_DATA1_TYPE           memIndWrData1;
    HKADC_TXSLZR_SEQ_MEM_IND_RD_CONTROL_TYPE  memIndRdCtrl;
    HKADC_SEQ_MEM_IND_RD_DATA0_TYPE           memIndRdData0;
    HKADC_SEQ_MEM_IND_RD_DATA1_TYPE           memIndRdData1;
    HKADC_TXSLZR_SEQ_SPI_CONTROL_TYPE         spiCtrl;
    HKADC_TXSLZR_SEQ_SPI_SFT_CONTROL_TYPE     spiSwCtrl;
    HKADC_SPI_SOFT_WR_DATA_TYPE               spiSwWrData;
    HKADC_SPI_SOFT_RD_DATA_TYPE               spiSwRdData;
    HKADC_TXSLZR_SEQ_MISC_SPI_CONTROL_TYPE    spiSwMiscCtrl;
} HKADC_DrvSeqType;

/**
    @trace #BRCM_SWARCH_HKADC_DRV_SEQSPICFG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_SEQCONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_SEQMEMRDWR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_SWSPIRDWR_PROC
    @trace #BRCM_SWREQ_HKADC_INIT
*/
static HKADC_DrvSeqType * const HKADC_SeqRegs[HKADC_SEQ_MAX_CH] =
{
    (HKADC_DrvSeqType *)HKADC_SEQ6_BASE_ADDR,
    (HKADC_DrvSeqType *)HKADC_SEQ7_BASE_ADDR,
};


/**
    @trace #BRCM_SWREQ_HKADC_INIT
    @trace #BRCM_SWARCH_HKADC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONFIG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_FIFO_CONFIG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CFG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CFG_CAL_PROC
    @trace #BRCM_SWARCH_HKADC_GET_CAL_STS_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_FIFO_DATA_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAL_CONTROL_PROC
*/
typedef struct sHKADC_DrvDevType {
    HKADC_StateType   state;            /**< State of the driver                */
} HKADC_DrvDevType;

/**
    @trace #BRCM_SWREQ_HKADC_INIT
    @trace #BRCM_SWARCH_HKADC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONFIG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_FIFO_CONFIG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CFG_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CFG_CAL_PROC
    @trace #BRCM_SWARCH_HKADC_GET_CAL_STS_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_GET_FIFO_DATA_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CONTROL_PROC
    @trace #BRCM_SWARCH_HKADC_DRV_CAL_CONTROL_PROC
*/
#ifdef BCM8915X_PARAM_VALIDATION
static HKADC_DrvDevType HKADC_DrvDev[HKADC_MAX_HW_ID] COMP_SECTION(".bss.drivers");
#endif


/** @brief Initialize HKADC

    This API initializes the given HKADC hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKADC controller

    @return     int32_t

    @post None

    @trace  #BRCM_SWREQ_HKADC_INIT
    @trace  #BRCM_SWARCH_HKADC_DRV_INIT_PROC
    @code{.unparsed}
    Validate hardware ID
    Clear AFE config channel power down for all the channels
    Move device state to initialized
    @endcode

    @limitations None
*/

int32_t HKADC_DrvInit(HKADC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
    uint32_t chIndex = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    /* Step 1 Power up all the HKADC channel*/
    for (chIndex = 0UL; chIndex < HKADC_CHAN_SIZE; chIndex++) {
       HKADC_Regs[hwId]->chan[chIndex].afe0_cfg0 &= ~HKADC_AUXADC_CONFIG0_I_ADC_PD_MASK;
    }
#ifdef BCM8915X_PARAM_VALIDATION
    HKADC_DrvDev[hwId].state = HKADC_STATE_INITIALIZED;
    }
#endif

    return ret;
}

/** @brief De-initialize HKADC

    This API de-initializes HKADC. Resets the given HKADC controller and put back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HKADC controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_INIT
    @trace  #BRCM_SWARCH_HKADC_DRV_DEINIT_PROC
    @code{.unparsed}
    Validate hardware ID.
    Clear AFE config channel power down for all the channels.
    Move device state to uninitialized.

    @endcode

    @limitations None
*/

int32_t HKADC_DrvDeInit (HKADC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
    uint32_t chIndex = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    /* Power down all the HKADC channel*/
    for (chIndex = 0UL; chIndex < HKADC_CHAN_SIZE; chIndex++) {
       HKADC_Regs[hwId]->chan[chIndex].afe0_cfg0 |= HKADC_AUXADC_CONFIG0_I_ADC_PD_MASK;
    }
#ifdef BCM8915X_PARAM_VALIDATION
    HKADC_DrvDev[hwId].state = HKADC_STATE_UNINIT;
    }
#endif

    return ret;
}

/** @brief HKADC capture configuration

    This API configures HKADC with user specified configurations.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      capConfig    House keeping ADC configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONFIG_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId and capConfig pointer.
    Clear Contineous mode, Soft mode, Sequence mode and program rate count in capture control register.
    Program capture mode, sequence mode, trigger type and program rate in to capture control register

    @endcode

    @limitations None
*/
int32_t HKADC_DrvCaptureConfig (HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, HKADC_CaptureConfigType *capConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (capConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    HKADC_Regs[hwId]->chan[chId].adc0_capt_ctrl &= ~(HKADC_CAPT_CTRL_CONT_MODE_MASK | HKADC_CAPT_CTRL_SOFT_MODE_MASK |
                                                        HKADC_CAPT_CTRL_SEQ_MODE_MASK | HKADC_CAPT_CTRL_PROG_RATE_CNT_MASK);
    HKADC_Regs[hwId]->chan[chId].adc0_capt_ctrl |=  (((uint32_t)capConfig->capMode << HKADC_CAPT_CTRL_CONT_MODE_SHIFT) |
                                                        ((uint32_t)capConfig->triggerType << HKADC_CAPT_CTRL_SOFT_MODE_SHIFT) |
                                                        ((uint32_t)capConfig->seqMode << HKADC_CAPT_CTRL_SEQ_MODE_SHIFT) |
                                                        capConfig->programRate);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKADC capture enable

    This API enables/disables HKADC capture.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      enDis        Enables/Disables capture configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKADC_DRV_CAPTURE_CONFIG_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId .
    Get value of capture control register.
    Clear capture enable mask bit.
    Based on enDis update the control variable.
    Write adc0 capture control register with updated register value.

    @endcode

    @limitations None
*/
int32_t HKADC_DrvCaptureControl (HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    control = HKADC_Regs[hwId]->chan[chId].adc0_capt_ctrl;
    control &= ~HKADC_CAPT_CTRL_CAPT_EN_MASK;
    control |= (enDis == 0UL) ? 0UL : HKADC_CAPT_CTRL_CAPT_EN_MASK;
    HKADC_Regs[hwId]->chan[chId].adc0_capt_ctrl = control;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif


    return ret;
}

/** @brief HKADC FIFO configuration

    This API perform FIFO configuration user can also perform FIFO soft reset based on
    the specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      softReset    0 - Don't Care 1 - Perform Softreset on FIFO
    @param[in]      thresholdValue FIFO threshold value, valid values are 0-127.
                                   A value of 0xFF will not write anything.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKADC_DRV_FIFO_CONFIG_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId .
    Perform soft reset only if softReset is 1.
    Write thresholdValue to fifoControl register only if the thresholdValue not equals to 0xFF.

    @endcode

    @limitations None
*/
int32_t HKADC_DrvFifoConfig(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint8_t softReset,  uint8_t thresholdValue)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (softReset == 1UL) {
            HKADC_Regs[hwId]->chan[chId].adc0_fifo_ctrl |= HKADC_ADC_FIFO_CTRL_SFT_RST_MASK;
        }
        if (thresholdValue != 0xFFUL) {
            HKADC_Regs[hwId]->chan[chId].adc0_fifo_ctrl &= ~HKADC_ADC_FIFO_CTRL_THRES_VAL_MASK;
            HKADC_Regs[hwId]->chan[chId].adc0_fifo_ctrl |= thresholdValue;
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief HKADC FIFO Read Data

    This API retreives FIFO data.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[out]     fifoData     ADC FIFO read data

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HKADC_DRV_GET_FIFO_DATA_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId and fifoData .
    Read adc fifo data and store in to *fifoData

    @endcode

    @limitations None
*/
int32_t HKADC_DrvGetFifoData(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint16_t *fifoData)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (fifoData == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *fifoData = (uint16_t)(HKADC_Regs[hwId]->chan[chId].adc0_fifo_data & 0xFFFFUL);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief HKADC comparator configuration

    This API configures comparators of HKADC of a specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      compConfig     Comparator configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HKADC driver state changes to #HKADC_ENABLE

    @trace  #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION
    @trace  #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CFG_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId and compConfig .
    Clear comparator0 threshold value and threshold mode.
    Write threshold value and threshold mode to comparator0 register
    Clear comparator1 threshold value and threshold mode.
    Write threshold value and threshold mode to comparator1 register

    @endcode

    @limitations None
*/
int32_t HKADC_DrvComparatorConfig(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId , HKADC_ComparatorType *compConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (compConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       HKADC_Regs[hwId]->chan[chId].adc0_cmp0 &= ~(HKADC_ADC_CMP_CTRL_THRES_VAL_MASK | HKADC_ADC_CMP_CTRL_THRES_MODE_MASK);
       HKADC_Regs[hwId]->chan[chId].adc0_cmp0 |= ((uint32_t)compConfig->comparator1.thresholdMode << HKADC_ADC_CMP_CTRL_THRES_MODE_SHIFT) |
                compConfig->comparator1.thresholdValue ;

       HKADC_Regs[hwId]->chan[chId].adc0_cmp1 &= ~(HKADC_ADC_CMP_CTRL_THRES_VAL_MASK | HKADC_ADC_CMP_CTRL_THRES_MODE_MASK);
       HKADC_Regs[hwId]->chan[chId].adc0_cmp1 |= ((uint32_t)compConfig->comparator2.thresholdMode << HKADC_ADC_CMP_CTRL_THRES_MODE_SHIFT) |
                compConfig->comparator2.thresholdValue ;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief Enables/Disables HKADC comparator

    This API enables/disables comparators of HKADC of a specified channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HKADC controller
    @param[in]      chId                Index (0-7) of the HKADC channel to be configured
    @param[in]      comInstance         0 : Comparator Instance 1 , 1 : Comparator Instance 2
    @param[in]      enDis               0 : Disable Comparator 1 : Enable Comparator

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_COMPARATOR_CONFIGURATION
    @trace  #BRCM_SWARCH_HKADC_DRV_COMPARATOR_CONTROL_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId.
    If compInstance == 0
        Enable/Disable specified comparator0.
    else
        Enable/Disable specified comparator1.

    @endcode

    @limitations None
*/
int32_t HKADC_DrvComparatorControl(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId , uint8_t comInstance, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    if (comInstance == 0UL) {
       control = HKADC_Regs[hwId]->chan[chId].adc0_cmp0;
       control &= ~(HKADC_ADC_CMP_CTRL_EN_COMP_MASK);
       control |= ((uint32_t)enDis << HKADC_ADC_CMP_CTRL_EN_COMP_SHIFT);
       HKADC_Regs[hwId]->chan[chId].adc0_cmp0 = control;
    } else {
       control = HKADC_Regs[hwId]->chan[chId].adc0_cmp1;
       control &= ~(HKADC_ADC_CMP_CTRL_EN_COMP_MASK);
       control |= ((uint32_t)enDis << HKADC_ADC_CMP_CTRL_EN_COMP_SHIFT);
       HKADC_Regs[hwId]->chan[chId].adc0_cmp1 = control;
    }


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief HKADC Calibration configuration

    This API configures and enables HKADC hardware background or foreground calibration.
    This API should be called after configuring HKADC using HKADC_DrvConfig.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      calibConfig    HKADC calibration configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CALIBRATION
    @trace  #BRCM_SWARCH_HKADC_DRV_CFG_CAL_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId and calibConfig.
    Clear ADC calibration average and mode in afe0_cfg0 register.
    Write afe0_cfg0 with calibration average and mode.

    @endcode

    @limitations None
*/
int32_t HKADC_DrvConfigCalibration(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, HKADC_CalibConfigType *calibConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (calibConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       HKADC_Regs[hwId]->chan[chId].afe0_cfg0 &= ~(HKADC_AUXADC_CONFIG0_I_ADC_CALAVG_MASK |
                                                      HKADC_AUXADC_CONFIG0_I_ADC_CALMODE_MASK);
       HKADC_Regs[hwId]->chan[chId].afe0_cfg0 |= (((uint32_t)calibConfig->calibrationAverage << HKADC_AUXADC_CONFIG0_I_ADC_CALAVG_SHIFT) |
                                                  ((uint32_t)calibConfig->calibMode << HKADC_AUXADC_CONFIG0_I_ADC_CALMODE_SHIFT));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief HKADC Calibration Enable

    This API enables HKADC hardware background or foreground calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HKADC controller
    @param[in]      chId           Index (0-7) of the HKADC channel to be configured
    @param[in]      enCalib        Enables(1)/Disables(0) HKADC calibration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HKADC_CALIBRATION
    @trace  #BRCM_SWARCH_HKADC_DRV_CAL_CONTROL_PROC
    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId .
    Clear calibration enable mask in afe0_calib register
    Write enCalib valur in to afe0_calib register

    @endcode

    @limitations None
*/
int32_t HKADC_DrvCalibrationControl(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint8_t enCalib)
{
    int ret = BCM_ERR_OK;
    uint32_t control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
      control = HKADC_Regs[hwId]->chan[chId].afe0_calib;
      control &= ~(HKADC_AFE_CALIB_CALEN_MASK);
      control |= ((uint32_t)enCalib << HKADC_AFE_CALIB_CALEN_SHIFT);
      HKADC_Regs[hwId]->chan[chId].afe0_calib = control;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief HKADC Calibration Status

    This API gets HKADC calibration status. Calibration status in not valid if calibration is not enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId         Index (0-7) of the HKADC channel to be configured
    @param[in]      calibStatus   HKADC calibration status

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HKADC driver state changes to #HKADC_CALIB_INPROGRESS/HKADC_ENABLE

    @trace  #BRCM_SWREQ_HKADC_CALIBRATION
    @trace  #BRCM_SWARCH_HKADC_GET_CAL_STS_PROC

    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId and calibStatus.
    Read afe0_calib register bit 31 and store value in calibStatus.

    @endcode

    @limitations None
*/
int32_t HKADC_DrvGetCalibStatus(HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, HKADC_CalibStatusType *calibStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (calibStatus == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
      calibStatus->calStatus = (uint16_t)(HKADC_Regs[hwId]->chan[chId].afe0_calib >> HKADC_AFE_CALIB_CALIBRATING_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief HKADC Enable Channel interrupt

    This API enables/disables HKADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId                 Index of the HKADC Channel
    @param[in]      intrType             HKADC interrupt type
    @param[in]      enaDis               0 - Disable specified interrupt 1 - Enable Specified interrupt

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT
    @trace  #BRCM_SWARCH_HKADC_DRV_CONFIG_CH_INTR_PROC

    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId.
    if intrType is HKADC_CHANNEL_INTR_FIFO_EMPTY
        clear acq_hkadc_adc0_fifo_intr_eier FIFO empty bit
        Write enaDis to acq_hkadc_adc0_fifo_intr_eier FIFO empty bit
    if intrType is HKADC_CHANNEL_INTR_FIFO_OVERFLOW
        clear acq_hkadc_adc0_fifo_intr_eier FIFO overflow bit
        Write enaDis to acq_hkadc_adc0_fifo_intr_eier FIFO overflow bit
    if intrType is HKADC_CHANNEL_INTR_ADC_THRES_0
        clear acq_hkadc_adc0_threshold_intr_eier threshold0 bit
        Write enaDis to acq_hkadc_adc0_threshold_intr_eier threshold0 bit
    if intrType is HKADC_CHANNEL_INTR_ADC_THRES_1
        clear acq_hkadc_adc0_threshold_intr_eier threshold 1 bit
        Write enaDis to acq_hkadc_adc0_threshold_intr_eier threshold 1 bit
    if intrType is HKADC_CHANNEL_INTR_FIFO_THRES
        clear acq_hkadc_adc0_fifo_intr_eier FIFO threshold  bit
        Write enaDis to acq_hkadc_adc0_fifo_intr_eier FIFO threshold  bit
    if intrType is HKADC_CHANNEL_INTR_SEQUENCER
        if chId is greater than or equals to 6
            clear acq_hkadc_adc0_fifo_intr_eier sequencer interrupt mask
            Write enaDis to acq_hkadc_adc0_fifo_intr_eier to sequencer interrupt mask

    @endcode

    @limitations None
*/
int32_t HKADC_DrvConfigChIntr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_ChannelIntrType intrType, uint32_t enaDis)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if ((intrType & HKADC_CHANNEL_INTR_FIFO_EMPTY) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier &= ~(HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_MASK);
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier |= ((uint32_t)enaDis << HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_SHIFT);
        }
        if ((intrType & HKADC_CHANNEL_INTR_FIFO_OVERFLOW) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier &= ~(HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_MASK);
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier |= ((uint32_t)enaDis << HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_SHIFT);
        }
        if ((intrType & HKADC_CHANNEL_INTR_ADC_THRES_0) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eier &= ~HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_MASK;
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eier |= ((uint32_t)enaDis << HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_SHIFT);
        }
        if ((intrType & HKADC_CHANNEL_INTR_ADC_THRES_1) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eier &= ~HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_MASK;
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eier |= ((uint32_t)enaDis << HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_SHIFT);
        }
        if ((intrType & HKADC_CHANNEL_INTR_FIFO_THRES) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier &= ~HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_MASK;
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier |= ((uint32_t)enaDis << HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_SHIFT);
        }
        if ((intrType & HKADC_CHANNEL_INTR_SEQUENCER) > 0UL) {
            if (chId >= 6UL) {
                HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier &= ~HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_MASK;
                HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier |= ((uint32_t)enaDis << HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_SHIFT);
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
        /*if (intrType == HKADC_CHANNEL_INTR_ALL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier &= ~(HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_MASK |
                                                                                   HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_MASK |
                                                                                   HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_MASK |
                                                                                   HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_MASK);
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eier &= ~(HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_MASK |
                                                                                       HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_MASK);
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eier |= (((uint32_t)enaDis << HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_SHIFT) |
                                                                                  ((uint32_t)enaDis << HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_SHIFT) |
                                                                                  ((uint32_t)enaDis << HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_SHIFT) |
                                                                                  ((uint32_t)enaDis << HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_SHIFT));

            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eier |= (((uint32_t)enaDis << HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_SHIFT) |
                                                                                       ((uint32_t)enaDis << HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_SHIFT));
        }*/
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HKADC clear Channel interrupt

    This API clears HKADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId                 Index of the HKADC Channel
    @param[in]      HKADC_IntType        HKADC interrupt type

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT
    @trace  #BRCM_SWARCH_HKADC_DRV_CLEAR_CH_INTR_PROC

    @code{.unparsed}

    Validate hardware ID, Device State, ChannelId.
    if intrType is HKADC_CHANNEL_INTR_FIFO_EMPTY
        Write 1 to acq_hkadc_adc0_fifo_intr_eisr FIFO empty bit
    if intrType is HKADC_CHANNEL_INTR_FIFO_OVERFLOW
        Write 1 to acq_hkadc_adc0_fifo_intr_eisr FIFO overflow bit
    if intrType is HKADC_CHANNEL_INTR_ADC_THRES_0
        Write 1 to acq_hkadc_adc0_threshold_intr_eisr threshold0 bit
    if intrType is HKADC_CHANNEL_INTR_ADC_THRES_1
        Write 1 to acq_hkadc_adc0_threshold_intr_eisr threshold 1 bit
    if intrType is HKADC_CHANNEL_INTR_FIFO_THRES
       Write 1 to acq_hkadc_adc0_fifo_intr_eisr FIFO threshold  bit
    if intrType is HKADC_CHANNEL_INTR_SEQUENCER
        if chId is greater than or equals to 6
            Write 1 to acq_hkadc_adc0_fifo_intr_eisr to sequencer interrupt mask

    @endcode

    @limitations None
*/
int32_t HKADC_DrvClearChIntr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_ChannelIntrType intrType)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if ((intrType & HKADC_CHANNEL_INTR_FIFO_EMPTY) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eisr |= (HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_MASK);
        }
        if ((intrType & HKADC_CHANNEL_INTR_FIFO_OVERFLOW) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eisr |= (HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_MASK);
        }
        if ((intrType & HKADC_CHANNEL_INTR_ADC_THRES_0) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eisr |= (HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_MASK);
        }
        if ((intrType & HKADC_CHANNEL_INTR_ADC_THRES_1) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eisr |= (HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_MASK);
        }
        if ((intrType & HKADC_CHANNEL_INTR_FIFO_THRES) > 0UL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eisr |= (HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_MASK);
        }
        if ((intrType & HKADC_CHANNEL_INTR_SEQUENCER) > 0UL) {
            if (chId >= 6UL) {
                HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eisr |= (HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_MASK);
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
        /*if (intrType == HKADC_CHANNEL_INTR_ALL) {
            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eisr |= (HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_MASK |
                                                                                   HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_MASK |
                                                                                   HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_MASK |
                                                                                   HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_MASK);

            HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eisr |= (HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_MASK |
                                                                                       HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_MASK);
        }*/
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Get all the Interrupt status
    @trace  #BRCM_SWREQ_HKADC_INTERRUPT
    @trace  #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC
*/
static void HKADC_IntGetAllInterruptStatus (HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint32_t *aStatus)
{
    uint32_t status = 0;

    /* Read FIFO interrupt */
    status = HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eipr;
    *aStatus = (((status &  HKADC_INTR_AHA0FIE_P_ADC0_FIFO_EMPTY_INTR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_FIFO_EMPTY :0UL) |
                (((status & HKADC_INTR_AHA0FIE_P_ADC0_FIFO_OVERFLOW_INTR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_FIFO_OVERFLOW : 0UL) |
                (((status & HKADC_INTR_AHA0FIE_P_ADC0_FIFO_THESHOLD_INTR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_FIFO_THRES : 0UL) |
                (((status & HKADC_INTR_AHA6FIE_P_ADC6_SEQ_INTR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_SEQUENCER : 0UL);

    /* Read Threshold Status*/
    status = HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eipr;
    *aStatus |= (((status & HKADC_INTR_AHA0TIE_P_ADC0_VALUE_THRESHOLD0_INTR_MASK) > 0U) ? HKADC_CHANNEL_INTR_ADC_THRES_0 : 0UL) |
        (((status & HKADC_INTR_AHA0TIE_P_ADC0_VALUE_THRESHOLD1_INTR_MASK) > 0UL)? HKADC_CHANNEL_INTR_ADC_THRES_1 : 0UL);

}

/**
    @brief Get all the event status

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT
    @trace  #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC
*/
static void HKADC_IntGetEventStatus (HKADC_HwIDType  hwId, HKADC_ChannelIDType chId, uint32_t *aStatus)
{
    uint32_t status = 0;

    /* Read FIFO interrupt */
    status = HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eisr;
    *aStatus = (((status &  HKADC_INTR_AHA0FIE1_ADC0_FIFO_EMPTY_INTR_W2CLR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_FIFO_EMPTY :0UL) |
                (((status & HKADC_INTR_AHA0FIE1_ADC0_FIFO_OVERFLOW_INTR_W2CLR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_FIFO_OVERFLOW : 0UL) |
                (((status & HKADC_INTR_AHA0FIE1_ADC0_FIFO_THESHOLD_INTR_W2CLR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_FIFO_THRES : 0UL) |
                (((status & HKADC_INTR_AHA6FIE1_ADC6_FIFO_OVERFLOW_INTR_W2CLR_MASK) > 0UL) ? HKADC_CHANNEL_INTR_SEQUENCER : 0UL);

    /* Read Threshold Status*/
    status = HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eisr;
    *aStatus |= (((status & HKADC_INTR_AHA0TIE1_ADC0_VALUE_THRESHOLD0_INTR_W2CLR_MASK) > 0U) ? HKADC_CHANNEL_INTR_ADC_THRES_0 : 0UL) |
        (((status & HKADC_INTR_AHA0TIE1_ADC0_VALUE_THRESHOLD1_INTR_W2CLR_MASK) > 0UL)? HKADC_CHANNEL_INTR_ADC_THRES_1 : 0UL);
}

/** @brief HKADC Gets Channel interrupt/Event Status

    This API gets HKADC channel based interrupts/Event status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HKADC controller
    @param[in]      chId                 Index of the HKADC Channel
    @param[in]      HKADC_IntType        HKADC interrupt type
    @param[out]     intrStatus           HKADC interrupt/Event Status

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_HKADC_INTERRUPT
    @trace  #BRCM_SWARCH_HKADC_DRV_GET_CH_INTR_STS_PROC

    @code{.unparsed}
    Validate hardware ID, Device State, ChannelId.
    if intrType is HKADC_CHANNEL_INTR_FIFO_EMPTY
        Read acq_hkadc_adc0_fifo_intr_eipr FIFO empty bit
        Update intrStatus with value as 1 if eipr is set else 0
    else if intrType is HKADC_CHANNEL_INTR_FIFO_OVERFLOW
        Read acq_hkadc_adc0_fifo_intr_eipr FIFO overflow bit
        Update intrStatus with value as 1 if eipr is set else 0
    else if intrType is HKADC_CHANNEL_INTR_ADC_THRES_0
        Read acq_hkadc_adc0_threshold_intr_eipr threshold0 bit
        Update intrStatus with value as 1 if eipr is set else 0
    else if intrType is HKADC_CHANNEL_INTR_ADC_THRES_1
        Read acq_hkadc_adc0_threshold_intr_eipr threshold 1 bit
        Update intrStatus with value as 1 if eipr is set else 0
    else if intrType is HKADC_CHANNEL_INTR_FIFO_THRES
        Read acq_hkadc_adc0_fifo_intr_eipr FIFO threshold  bit
        Update intrStatus with value as 1 if eipr is set else 0
    else if intrType is HKADC_CHANNEL_INTR_SEQUENCER
        if chId is greater than or equals to 6
            Read acq_hkadc_adc0_fifo_intr_eipr sequencer interrupt mask
            Update intrStatus with value as 1 if eipr is set else 0
    else if  intrType is HKADC_CHANNEL_INTR_EVENT_ALL
        call HKADC_IntGetEventStatus
    else
        call HKADC_IntGetAllInterruptStatus

    @endcode

    @limitations None
*/

int32_t HKADC_DrvGetChIntrStatus(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_ChannelIntrType intrType, uint32_t *intrStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (chId >= HKADC_CHAN_SIZE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (intrType == HKADC_CHANNEL_INTR_ALL) {
            HKADC_IntGetAllInterruptStatus (hwId, chId, intrStatus);
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            if (intrType == HKADC_CHANNEL_INTR_FIFO_EMPTY) {
                *intrStatus = (HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eipr & HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_MASK) >>
                           HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_EMPTY_INTR_SHIFT ;
            } else if (intrType == HKADC_CHANNEL_INTR_FIFO_OVERFLOW) {
                *intrStatus = (HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eipr & HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_MASK) >>
                           HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_OVERFLOW_INTR_SHIFT;
            } else if (intrType == HKADC_CHANNEL_INTR_ADC_THRES_0) {
                *intrStatus = (HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eipr & HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_MASK) >>
                           HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD0_INTR_SHIFT;
            } else if (intrType  == HKADC_CHANNEL_INTR_ADC_THRES_1) {
                *intrStatus = (HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_threshold_intr_eipr & HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_MASK) >>
                           HKADC_INTR_AHA0TIE2_ENABLE_ADC0_VALUE_THRESHOLD1_INTR_SHIFT;
            } else if (intrType == HKADC_CHANNEL_INTR_FIFO_THRES) {
                *intrStatus = (HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eipr  & HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_MASK) >>
                           HKADC_INTR_AHA0FIE2_ENABLE_ADC0_FIFO_THESHOLD_INTR_SHIFT;
            } else if (intrType == HKADC_CHANNEL_INTR_SEQUENCER) {
                if (chId >= 6UL) {
                    *intrStatus = (HKADC_IntrRegs[hwId]->channel[chId].acq_hkadc_adc0_fifo_intr_eipr & HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_MASK) >>
                               HKADC_INTR_AHA6FIE2_ENABLE_ADC6_SEQ_INTR_SHIFT;
                } else {
                    ret = BCM_ERR_INVAL_PARAMS;
                }
            }
        }
#endif
        if (intrType == HKADC_CHANNEL_INTR_EVENT_ALL) {
            HKADC_IntGetEventStatus (hwId, chId, intrStatus);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HKADC sequencer SPI configuration

    @code{.unparsed}
    validate hwId, Device state and channel Id
    Configure Spi control and sequencer control
    @endcode

    @trace #BRCM_SWREQ_HKADC_SEQUENCER
    @trace #BRCM_SWARCH_HKADC_DRV_SEQSPICFG_PROC
*/
int32_t HKADC_DrvSeqSpiCfg(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_SpiConfigType* spiCfg)
{
    int ret = BCM_ERR_OK;
    /*Coverity error forced not to use % */
    uint32_t channelId = (chId == 6UL) ? 0UL : 1UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((chId < 6UL) || (chId >= HKADC_CHAN_SIZE)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        HKADC_SeqRegs[channelId]->spiCtrl = spiCfg->spiControl;
        HKADC_SeqRegs[channelId]->seqCtrl |= (spiCfg->spiSeqOut << HKADC_TXSLZR_SEQ_CONTROL_OUTPUT_CFG_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief HKADC sequencer control

    @code{.unparsed}
    validate hwId, Device state and channel Id
    Configure ADC capture control
    @endcode

    @trace #BRCM_SWREQ_HKADC_SEQUENCER
    @trace #BRCM_SWARCH_HKADC_DRV_SEQCONTROL_PROC
*/
int32_t HKADC_DrvSeqControl(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t captCtrl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((chId < 6UL) || (chId >= HKADC_CHAN_SIZE)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    captCtrl = HKADC_Regs[hwId]->chan[chId].adc0_capt_ctrl;
    captCtrl &= (~HKADC_CAPT_CTRL_SEQ_MODE_MASK);
    captCtrl |= ((uint32_t)enDis << HKADC_CAPT_CTRL_SEQ_MODE_SHIFT);
    HKADC_Regs[hwId]->chan[chId].adc0_capt_ctrl = captCtrl;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/**
    @brief HKADC sequencer SPI Memory configuration

    @code{.unparsed}
    Validate hwId, Device state and channel Id
    Based on read/write configure read control and memory / Write control and memory
    @endcode

    @trace #BRCM_SWREQ_HKADC_SEQUENCER
    @trace #BRCM_SWARCH_HKADC_DRV_SEQMEMRDWR_PROC
*/

int32_t HKADC_DrvSeqMemRdWr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId,  HKADC_SeqMemRdWrType *seqMem)
{
    int ret = BCM_ERR_OK;
    /*Coverity error forced not to use % */
    uint32_t channelId = (chId == 6UL) ? 0UL : 1UL;
    uint8_t index = 0U;
    uint32_t data = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((chId < 6UL) || (chId >= HKADC_CHAN_SIZE)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (seqMem->readWrite > 0U) {
            HKADC_SeqRegs[channelId]->memIndWrCtrl = (HKADC_TXSLZR_SEQ_MEM_IND_WR_CONTROL_INCR_MODE_MASK |
                                                         seqMem->startAddress);
            for (index = 0U; index < seqMem->dataSize; index++) {
                HKADC_SeqRegs[channelId]->memIndWrData1 = ((uint32_t)seqMem->seqID[index] << HKADC_SEQ_MEM_IND_WR_DATA1_ID_SHIFT|
                        seqMem->postWaitCnt[index]);
                /* Data0 to be written at the end for increment mode*/
                HKADC_SeqRegs[channelId]->memIndWrData0 = (seqMem->dataEndFlag[index] |
                        ((uint32_t)seqMem->dataRptCnt[index] << HKADC_SEQ_MEM_IND_WR_DATA0_RPT_CNT_SHIFT)|
                        ((uint32_t)seqMem->spiData[index] << HKADC_SEQ_MEM_IND_WR_DATA0_SPI_DATA_SHIFT));
            }
        } else {
            HKADC_SeqRegs[channelId]->memIndRdCtrl = (HKADC_TXSLZR_SEQ_MEM_IND_RD_CONTROL_INCR_MODE_MASK |
                                                      seqMem->startAddress);
            for (index = 0U; index < seqMem->dataSize; index++) {
                data = HKADC_SeqRegs[channelId]->memIndRdData1;
                seqMem->seqID[index] = (uint8_t)((data & HKADC_SEQ_MEM_IND_RD_DATA1_ID_MASK) >> HKADC_SEQ_MEM_IND_RD_DATA1_ID_SHIFT);
                seqMem->postWaitCnt[index] = (uint16_t)(data & HKADC_SEQ_MEM_IND_RD_DATA1_POST_WAIT_CNT_MASK);

                /* Data0 to be read at the end for increment mode*/
                data = HKADC_SeqRegs[channelId]->memIndRdData0;
                seqMem->dataEndFlag[index] = (uint8_t)(data & HKADC_SEQ_MEM_IND_RD_DATA0_END_FLAG_MASK)  ;
                seqMem->dataRptCnt[index] =  (uint8_t)((data & HKADC_SEQ_MEM_IND_RD_DATA0_RPT_CNT_MASK) >> HKADC_SEQ_MEM_IND_RD_DATA0_RPT_CNT_SHIFT);
                seqMem->spiData[index] = (uint16_t)((data &  HKADC_SEQ_MEM_IND_RD_DATA0_SPI_DATA_MASK) >> HKADC_SEQ_MEM_IND_RD_DATA0_SPI_DATA_SHIFT);
            }
        }


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/**
    @brief HKADC sequencer Software SPI control

    @code{.unparsed}
    Validate hwId, Device state and channel Id
    Based on read/write configure software SPI control
    @endcode

    @trace #BRCM_SWREQ_HKADC_SEQUENCER
    @trace #BRCM_SWARCH_HKADC_DRV_SWSPIRDWR_PROC
*/
int32_t HKADC_DrvSwSpiRdWr(HKADC_HwIDType  hwId,  HKADC_ChannelIDType chId, HKADC_SwSpiRdWrType *swSpi)
{
    int ret = BCM_ERR_OK;
    /*Coverity error forced not to use % */
    uint32_t channelId = (chId == 6UL) ? 0UL : 1UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HKADC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (HKADC_DrvDev[hwId].state != HKADC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((chId < 6UL) || (chId >= HKADC_CHAN_SIZE)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    if (swSpi->readWrite > 0U) {
        HKADC_SeqRegs[channelId]->spiSwCtrl |= (HKADC_TXSLZR_SEQ_SPI_SFT_CONTROL_WR_MASK);
        HKADC_SeqRegs[channelId]->spiSwWrData = swSpi->data;
        HKADC_SeqRegs[channelId]->spiSwMiscCtrl = swSpi->turnaround;
    } else {
        HKADC_SeqRegs[channelId]->spiSwCtrl = (HKADC_TXSLZR_SEQ_SPI_SFT_CONTROL_RD_MASK|
                                               swSpi->readCmd | swSpi->readCfg);
        swSpi->readDone = (uint8_t)(HKADC_SeqRegs[channelId]->spiSwRdData >> HKADC_SPI_SOFT_RD_DATA_RD_DONE_SHIFT);
        swSpi->data = (uint16_t)(HKADC_SeqRegs[channelId]->spiSwRdData & HKADC_SPI_SOFT_RD_DATA_SFT_RD_DATA_MASK);

    }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}


/** @} */


