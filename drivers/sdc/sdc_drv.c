/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_sdcdrv_drv_impl SDC Driver Design
    @ingroup grp_sdcdrv

    @addtogroup grp_sdcdrv_drv_impl
    @{

    @file drivers/sdc/sdc_drv.c
    @brief SDC Driver Design

    @version 0.1 Initial version
*/

#include <sdc.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <sdc_csr_rdb.h>
#include <ddr_llc_cmn_rdb.h>

/**
    @name SDC Driver Design IDs
    @{
    @brief Driver Design IDs for SDC

*/
#define BRCM_SWDSGN_SDC_DRV_INIT_PROC                   (0xB101U)    /**< @brief #SDC_DrvInit*/
#define BRCM_SWDSGN_SDC_DRV_DEINIT_PROC                 (0xB102U)    /**< @brief #SDC_DrvDeInit*/
#define BRCM_SWDSGN_SDC_DRV_SECURE_MEM_CONFIG_PROC      (0xB103U)    /**< @brief #SDC_DrvSecureMemConfig*/
#define BRCM_SWDSGN_SDC_DRV_SECURE_MEM_CONTROL_PROC     (0xB104U)    /**< @brief #SDC_DrvSecureMemControl*/
#define BRCM_SWDSGN_SDC_DRV_STATISTICS_CONTROL_PROC     (0xB105U)    /**< @brief #SDC_DrvStatisticsControl*/
#define BRCM_SWDSGN_SDC_DRV_STATISTICS_GET_PROC         (0xB106U)    /**< @brief #SDC_DrvStatisticsGet*/
#define BRCM_SWDSGN_SDC_DRV_PWR_SAVING_CONFIG_PROC      (0xB107U)    /**< @brief #SDC_DrvPwrSavingConfig*/


#define BRCM_SWDSGN_SDC_CSR_REGS_GLOBAL                  (0xB10FU)    /**< @brief #SDC_CsrRegs*/
#define BRCM_SWDSGN_SDC_CMN_REGS_GLOBAL                  (0xB110U)    /**< @brief #SDC_DdrCmnRegs*/
#define BRCM_SWDSGN_SDC_DRV_DEV_TYPE                     (0xB111U)    /**< @brief #SDC_DrvDevType*/
#define BRCM_SWDSGN_SDC_DRV_DEV_GLOBAL                   (0xB112U)    /**< @brief #SDC_DrvDev*/


/** @} */

/**
    @trace #BRCM_SWREQ_SDC_INIT
    @trace #BRCM_SWARCH_SDC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_SDC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONFIG_PROC
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONTROL_PROC
    @trace #BRCM_SWARCH_SDC_DRV_STATISTICS_CONTROL_PROC
    @trace #BRCM_SWARCH_SDC_DRV_STATISTICS_GET_PROC
*/
static SDC_CSR_RDBType * const SDC_CsrRegs[SDC_CSR_MAX_HW_ID] =
{
#if (SDC_CSR_MAX_HW_ID == 0U)
#error "SDC_CSR_MAX_HW_ID == 0U"
#endif
    (SDC_CSR_RDBType*)SDC_CSR_BASE,
#if (SDC_CSR_MAX_HW_ID > 1U)
#error "SDC_CSR_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_SDC_INIT
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONTROL_PROC
*/
static DDR_LLC_CMN_RDBType * const SDC_DdrCmnRegs[DDR_LLC_CMN_MAX_HW_ID] =
{
#if (DDR_LLC_CMN_MAX_HW_ID == 0U)
#error "DDR_LLC_CMN_MAX_HW_ID == 0U"
#endif
    (DDR_LLC_CMN_RDBType*)RIG_DDR_LLC_COMMON_BASE,
#if (DDR_LLC_CMN_MAX_HW_ID > 1U)
#error "DDR_LLC_CMN_MAX_HW_ID > 1U is not supported by the driver"
#endif
};


/**
    @trace #BRCM_SWREQ_SDC_INIT
    @trace #BRCM_SWARCH_SDC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_SDC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONFIG_PROC
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONTROL_PROC
    @trace #BRCM_SWARCH_SDC_DRV_STATISTICS_CONTROL_PROC
    @trace #BRCM_SWARCH_SDC_DRV_STATISTICS_GET_PROC

*/
typedef struct sSDC_DrvDevType {
    SDC_StateType   state;            /**< State of the driver                */
} SDC_DrvDevType;

/**
    @trace #BRCM_SWARCH_SDC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_SDC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONFIG_PROC
    @trace #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONTROL_PROC
    @trace #BRCM_SWARCH_SDC_DRV_STATISTICS_CONTROL_PROC
    @trace #BRCM_SWARCH_SDC_DRV_STATISTICS_GET_PROC

    @trace #BRCM_SWREQ_SDC_INIT
*/
static SDC_DrvDevType SDC_DrvDev[SDC_CSR_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/** @brief Initialize SDC driver

    This API initializes the SDC hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the SDC controller

    @return     int32_t

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_SDC_INIT
    @trace  #BRCM_SWARCH_SDC_DRV_INIT_PROC

    @code{.unparsed}
    Validate Hardware index
    Mode driver State to SDC_STATE_INITIALIZED
    @endcode

    @limitations None
*/

int32_t SDC_DrvInit(SDC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       SDC_DrvDev[hwId].state = SDC_STATE_INITIALIZED;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief De-Initialize SDC

    This API de-initializes the specified cache

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the SDC

    @return     #BCM_ERR_OK               De-Initialized SDC instance
    @return     #BCM_ERR_INVAL_PARAMS     Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_SDC_INIT
    @trace  #BRCM_SWARCH_SDC_DRV_DEINIT_PROC

    @code{.unparsed}
    Validate Hardware index
    Mode driver State to SDC_STATE_UNINIT
    @endcode


    @limitations None
*/

int32_t SDC_DrvDeInit(SDC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*DeInit SDC*/
        SDC_DrvDev[hwId].state = SDC_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Power saving configuration

    This API configures power saving mode to reduce power consumption when the
    memory is not being accessed.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId        Index of the SDRAM controller
    @param[in]     pwrConfig   Power saving configurations.

    @return     #BCM_ERR_OK               Initialized SDRAM instance
    @return     #BCM_ERR_INVAL_PARAMS     Null pointer for power configuration
    @return     #BCM_ERR_INVAL_PARAMS     if powerConfig->opMode is not correct
    @return     #BCM_ERR_NODEV            SDC instance not initialized

    @post None

    @trace  #BRCM_SWREQ_SDC_PWR_SAVING
    @trace  #BRCM_SWARCH_SDC_DRV_PWR_SAVING_CONFIG_PROC
    @code{.unparsed}
     Validate Hardware index, Device state, powerConfig and opMode.
     If opMode is Self refresh
        Program hold off period and restart 
        Enable selfRefresh mode
     else if opMode is powersave mode
        program hold off period
        enable powersave mode
     else 
        clear power save and selfrefresh mode
    @endcode

    @limitations None
*/

int32_t SDC_DrvPwrSavingConfig(SDC_HwIDType  hwId, SDC_PowerSavingConfigurationType *powerConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (SDC_DrvDev[hwId].state != SDC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (powerConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (powerConfig->opMode > SDC_PWR_SAVING_OPMODE_NONE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (powerConfig->opMode == SDC_PWR_SAVING_OPMODE_SELF_REFRESH) {
           SDC_CsrRegs[hwId]->sd &= ~SDC_CSR_SD_STBY_T_MASK;
           SDC_CsrRegs[hwId]->sd |= (((uint32_t)powerConfig->holdOffPeriod << SDC_CSR_SD_STBY_T_SHIFT) & SDC_CSR_SD_STBY_T_MASK);
           if (powerConfig->restartPowerSaving == 1U) {
               SDC_CsrRegs[hwId]->cs |= SDC_CSR_CS_RESTRT_MASK;
           } else {
               SDC_CsrRegs[hwId]->cs &= ~SDC_CSR_CS_RESTRT_MASK;
           }
           /* StandBy mask*/
           SDC_CsrRegs[hwId]->cs |= SDC_CSR_CS_STBY_MASK ;
        } else if (powerConfig->opMode == SDC_PWR_SAVING_OPMODE_PWR_SAVE) {
           SDC_CsrRegs[hwId]->sd &= ~SDC_CSR_SD_POWSAV_T_MASK;
           SDC_CsrRegs[hwId]->sd |= (powerConfig->holdOffPeriod & SDC_CSR_SD_POWSAV_T_MASK);
           /* Power Save mask*/
           SDC_CsrRegs[hwId]->sd |= SDC_CSR_SD_POWSAVE_MASK;
        } else {
            /* Clear Standby and power save feature*/
           SDC_CsrRegs[hwId]->sd &= ~SDC_CSR_SD_POWSAVE_MASK;
           SDC_CsrRegs[hwId]->cs &= ~SDC_CSR_CS_STBY_MASK ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}

/** @brief Configure secure memory area.

    This API configures secure memory area based on the memory index. SDRAM Controller has 32 secure memory areas.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the SDRAM Controller.
    @param[in]      sMemIndex        Secure Memory Index (0-31)
    @param[in]      protMask         It is a 8-bit bit mask, Each bit in the field specifies whether or
                                     not an incoming access request to the SDRAM controller with a particular value of
                                     AXI APROT is allowed to access this protected region.
                                     If protMask is set to 0x1, then requests with APROT=0 are allowed.
    @param[in]      startAddr        Secure memory Area start address bits 35-13, The ADDR field specifies the start (byte)
                                     address of the protected secure memory region.  Protected regions must start and end on 8kByte
                                     address boundaries. The Address specified should be a SDC local address.
                                     Ex: DDR Start address (0x080000000) should be given as (0x000000000).

    @param[in]      endAddr          Secure memory Area start address bits 35-13, The ADDR field specifies the start (byte)
                                     address of the protected secure memory region.  Protected regions must start and end on 8kByte
                                     address boundaries. The Address specified should be a SDC local address.
                                     Ex: DDR Start address (0x090000000) should be given as (0x010000000).



    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory index
    @return     #BCM_ERR_NODEV          SDC instance not initialized

    @post None

    @trace  #BRCM_SWREQ_SDC_SECURE_AREA
    @trace  #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONFIG_PROC
    @code{.unparsed}
     Validate Hardware index, Device state and memory index.
     Write startAddress, endAddress and protMask
    @endcode

    @limitations None
*/
int32_t SDC_DrvSecureMemConfig(SDC_HwIDType hwId, uint8_t sMemIndex, uint8_t protMask, uint32_t startAddress, uint32_t endAddress)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (SDC_DrvDev[hwId].state != SDC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (sMemIndex > 32U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        SDC_CsrRegs[hwId]->secsrt[sMemIndex] = (((startAddress << SDC_CSR_SECSRT0_ADDR_MS_SHIFT) & SDC_CSR_SECSRT0_ADDR_MS_MASK) |
                                                (((uint32_t)protMask << SDC_CSR_SECSRT0_MASK_SHIFT) & SDC_CSR_SECSRT0_MASK_MASK));
        SDC_CsrRegs[hwId]->secend[sMemIndex] = (((endAddress << SDC_CSR_SECEND0_ADDR_MS_SHIFT) & SDC_CSR_SECEND0_ADDR_MS_MASK) | (0x1FFUL)); /* all one for low address*/
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Enables/Disables secure memory

    This API enables/disables secure memory based on the specified index.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the SDC
    @param[in]      sMemIndex        Secure Memory Index (0-31)
    @param[in]      enDis            Enables(1) or Disables(0) secure memory region.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid Memory index
    @return     #BCM_ERR_NODEV          SDC instance not initialized

    @post None

    @trace  #BRCM_SWREQ_SDC_SECURE_AREA
    @trace  #BRCM_SWARCH_SDC_DRV_SECURE_MEM_CONTROL_PROC
    @code{.unparsed}
     Validate Hardware index, Device state and memory index.
     Enable/Disable Secure region
    @endcode

    @limitations None
*/
int32_t SDC_DrvSecureMemControl(SDC_HwIDType hwId, uint8_t sMemIndex, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t control = 0;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (sMemIndex > 32U) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (SDC_DrvDev[hwId].state != SDC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Enable apb_psecure always*/
        SDC_DdrCmnRegs[hwId]->sdc_cntrl1_reg |= (1UL << DDR_LLC_CMN_SDC_CNTRL1_CTYPE_RG_SD_APB_PSECURE_SHIFT);
        control = SDC_CsrRegs[hwId]->secsrt[sMemIndex];
        control &= ~SDC_CSR_SECSRT0_ENABLE_MASK; 
        control |= ((enDis > 0U) ? 1UL : 0UL);
        SDC_CsrRegs[hwId]->secsrt[sMemIndex] = control;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Enable SDRAM Statistics

    This API enables/disables counter collection on SDRAM Controller.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the SDRAM controller
    @param[in]      enDis                 Disable(0)/Enable(1) counter collection

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid HW index
    @return     #BCM_ERR_NODEV          SDC instance not initialized

    @post None

    @trace  #BRCM_SWREQ_SDC_DIAG
    @trace  #BRCM_SWARCH_SDC_DRV_STATISTICS_CONTROL_PROC
    @code{.unparsed}
     Validate Hardware index, Device state.
     Enable/Disable SDC statistics 

    @endcode

    @limitations None
*/
int32_t SDC_DrvStatisticsControl(SDC_HwIDType hwId, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
    uint32_t control = 0;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if (SDC_DrvDev[hwId].state != SDC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        control = SDC_CsrRegs[hwId]->cs;
        control &= ~SDC_CSR_CS_STATEN_MASK; 
        control |= ((enDis > 0U) ? SDC_CSR_CS_STATEN_MASK : 0UL);
        SDC_CsrRegs[hwId]->cs = control;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Retrieve SDRAM Statistics

    This API retrieves various counters of SDRAM Controller.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the SDRAM controller
    @param[out]     counter              Counter of SDRAM Controller

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid HW index
    @return     #BCM_ERR_NODEV          SDC instance not initialized

    @post None

    @trace  #BRCM_SWREQ_SDC_DIAG
    @trace  #BRCM_SWARCH_SDC_DRV_STATISTICS_GET_PROC

    @code{.unparsed}
     Validate Hardware index, Device state.
     Read all SDC statistics
    @endcode

    @limitations None
*/
int32_t SDC_DrvStatisticsGet(SDC_HwIDType hwId, SDC_StatisticsCounterType *counter)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= SDC_CSR_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (SDC_DrvDev[hwId].state != SDC_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    counter->idleCycleCount = SDC_CsrRegs[hwId]->idl;
    counter->readTrafficCount = SDC_CsrRegs[hwId]->rtc;
    counter->writeTrafficCount = SDC_CsrRegs[hwId]->wtc;
    counter->readDataCount = SDC_CsrRegs[hwId]->rdc;
    counter->writeDataCount = SDC_CsrRegs[hwId]->wdc;
    counter->rowActivationCount = SDC_CsrRegs[hwId]->rac;
    counter->cycleCount = SDC_CsrRegs[hwId]->cyc;
    counter->activeCommandCycleCount = SDC_CsrRegs[hwId]->cmd;
    counter->databusActiveCycleCount = SDC_CsrRegs[hwId]->dat;
    counter->writeCommandCount = SDC_CsrRegs[hwId]->wrcmd;
    counter->suboptimalWriteCommandCount = SDC_CsrRegs[hwId]->wrsub;
    counter->maskedWriteCommandCount = SDC_CsrRegs[hwId]->mwrcmd;
    counter->suboptimalMaskedWriteCommandCount = SDC_CsrRegs[hwId]->mwrsub;
    counter->readCommandCount = SDC_CsrRegs[hwId]->rdcmd;
    counter->suboptimalReadCommandCount = SDC_CsrRegs[hwId]->rdsub;
    counter->maxQueueOccupancyCount = SDC_CsrRegs[hwId]->qmx;
    counter->requestReorderCount = SDC_CsrRegs[hwId]->reord;
    counter->lookAheadCommandCount = SDC_CsrRegs[hwId]->lac;
    counter->prechargeCommandCount = SDC_CsrRegs[hwId]->pre;
    counter->requestWriteReorderCount = SDC_CsrRegs[hwId]->wreord;
    counter->accessProcessorActivationCount = SDC_CsrRegs[hwId]->proc_act;
    counter->refreshInitManagementCycleCount = SDC_CsrRegs[hwId]->raaimtcyc;
    counter->refreshMaxManagementCycleCount = SDC_CsrRegs[hwId]->raammtcyc;
    counter->perBankRefreshCmdCount = SDC_CsrRegs[hwId]->rfmpbcnt;
    counter->allBankRefreshCmdCount = SDC_CsrRegs[hwId]->rfmabcnt;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @} */


