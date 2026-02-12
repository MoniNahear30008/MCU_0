/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_cachedrv_drv_impl CodaCache Driver Design
    @ingroup grp_cachedrv

    @addtogroup grp_cachedrv_drv_impl
    @{

    @file drivers/cache/cache_drv.c
    @brief Cache Driver Design

    @version 0.1 Initial version
*/

#include <cache.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <cache_rdb.h>
#include <cache_fsc_rdb.h>
#include <sdc_llc_intr_rdb.h>
#include <ddr_llc_cmn_rdb.h>
#include <clkrst_rdb.h>
#include <string.h>

/**
    @name Cache Driver Design IDs
    @{
    @brief Driver Design IDs for Cache

*/
#define BRCM_SWDSGN_CACHE_DRV_INIT_PROC                    (0xB101U)    /**< @brief #CACHE_DrvInit    */
#define BRCM_SWDSGN_CACHE_DRV_DEINIT_PROC                  (0xB102U)    /**< @brief #CACHE_DrvDeInit */
#define BRCM_SWDSGN_CACHE_DRV_CONFIG_PROC                  (0xB103U)    /**< @brief #CACHE_DrvConfigure */
#define BRCM_SWDSGN_CACHE_DRV_DAC_CONTROL_PROC             (0xB104U)    /**< @brief #CACHE_DrvIntrConfig      */
#define BRCM_SWDSGN_CACHE_DRV_POWER_CONFIG_PROC            (0xB105U)    /**< @brief #CACHE_DrvIntrStatus      */
#define BRCM_SWDSGN_CACHE_DRV_GET_DATA_PROC                (0xB106U)    /**< @brief #CACHE_DrvIntrThresholdConfig  */
#define BRCM_SWDSGN_CACHE_DRV_CONFIG_CH_INTR_PROC          (0xB107U)    /**< @brief #CACHE_DrvIntrClear      */
#define BRCM_SWDSGN_CACHE_DRV_CLER_CH_INTR_PROC            (0xB108U)    /**< @brief #CACHE_DrvFscControl      */
#define BRCM_SWDSGN_CACHE_DRV_GET_CH_INTR_STATUS_PROC      (0xB109U)    /**< @brief #CACHE_DrvFscStatus      */
#define BRCM_SWDSGN_CACHE_DRV_CONFIG_WAY_PART_PROC         (0xB10AU)    /**< @brief #CACHE_DrvConfigureWayPartition*/
#define BRCM_SWDSGN_CACHE_DRV_ENABLE_WAY_PART_PROC         (0xB10BU)    /**< @brief #CACHE_DrvEnableWayPartition*/
#define BRCM_SWDSGN_CACHE_DRV_MAINT_CTRL_PROC              (0xB10CU)    /**< @brief #CACHE_DrvMaintenanceControl*/
#define BRCM_SWDSGN_CACHE_DRV_MAINT_STS_PROC               (0xB10DU)    /**< @brief #CACHE_DrvMaintenanceStatus*/
#define BRCM_SWDSGN_CACHE_DRV_CONTROL_PROC                 (0xB10EU)    /**< @brief #CACHE_DrvControl*/


#define BRCM_SWDSGN_CACHE_REGS_GLOBAL                      (0xB10FU)    /**< @brief #CACHE_Regs*/
#define BRCM_SWDSGN_CACHE_FSC_REGS_GLOBAL                  (0xB110U)    /**< @brief #CACHE_FscRegs*/
#define BRCM_SWDSGN_CACHE_INTR_REGS_GLOBAL                 (0xB111U)    /**< @brief #CACHE_IntrRegs*/
#define BRCM_SWDSGN_CACHE_DRV_DEV_TYPE                     (0xB112U)    /**< @brief #CACHE_DrvDevType*/
#define BRCM_SWDSGN_CACHE_DRV_DEV_GLOBAL                   (0xB113U)    /**< @brief #CACHE_DrvDev*/

#define BRCM_SWDSGN_CACHE_INT_MAINTENANCE_OP_PROC          (0xB114U)    /**< @brief #CACHE_IntMaintenanceOp  */
#define BRCM_SWDSGN_CACHE_INT_MAINTENANCE_STS_PROC         (0xB115U)    /**< @brief #CACHE_IntMaintenanceStatus  */
#define BRCM_SWDSGN_CACHE_INT_GET_ALL_INT_STS_PROC         (0xB116U)    /**< @brief #CACHE_IntGetAllIntrStatus*/
#define BRCM_SWDSGN_CACHE_INT_GET_EVENT_STS_PROC           (0xB117U)    /**< @brief #CACHE_IntGetEventStatus*/
#define BRCM_SWDSGN_CACHE_CMN_REGS_GLOBAL                  (0xB118U)    /**< @brief #CACHE_CmnRegs*/
#define BRCM_SWDSGN_CACHE_CLOCK_REGS_GLOBAL                (0xB119U)    /**< @brief #CACHE_ClockRegs*/
#define BRCM_SWDSGN_CACHE_DRV_OVERRIDE_CACHE_POLICY_PROC   (0xB11AU)    /**< @brief #CACHE_DrvOverrideCachePolicy*/

/** @} */

/**
    @trace #BRCM_SWREQ_CACHE_INIT
    @trace #BRCM_SWARCH_CACHE_CONFIGURE_WAY_PARTITION_PROC
    @trace #BRCM_SWARCH_CACHE_ENABLE_WAY_PARTITION_PROC
    @trace #BRCM_SWARCH_CACHE_MAINTENANCE_CONTROL_PROC
    @trace #BRCM_SWARCH_CACHE_MAINTENANCE_STATUS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_ENABLE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_THOLD_CFG_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_CLEAR_PROC
*/
static CACHE_RDBType * const CACHE_Regs[CACHE_MAX_HW_ID] =
{
#if (CACHE_MAX_HW_ID == 0U)
#error "CACHE_MAX_HW_ID == 0U"
#endif
    (CACHE_RDBType*)RIG_LLC_CSR_BASE,
#if (CACHE_MAX_HW_ID > 1U)
#error "CACHE_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_CACHE_FSC
    @trace #BRCM_SWARCH_CACHE_DRV_FSC_CONTROL_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_FSC_STATUS_PROC
*/
static CACHE_FSC_RDBType * const CACHE_FscRegs[CACHE_FSC_MAX_HW_ID] =
{
#if (CACHE_FSC_MAX_HW_ID == 0U)
#error "CACHE_MAX_HW_ID == 0U"
#endif
    (CACHE_FSC_RDBType*)RIG_LLC_FSC_CSR_BASE,
#if (CACHE_FSC_MAX_HW_ID > 1U)
#error "CACHE_FSC_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_CACHE_INIT
    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
*/
static DDR_LLC_CMN_RDBType * const CACHE_CmnRegs[CACHE_MAX_HW_ID] =
{
#if (DDR_LLC_CMN_MAX_HW_ID == 0U)
#error "DDR_LLC_CMN_MAX_HW_ID == 0U"
#endif
    (DDR_LLC_CMN_RDBType*)RIG_DDR_LLC_COMMON_BASE,
#if (DDR_LLC_CMN_MAX_HW_ID > 1U)
#error "CACHE_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_CACHE_INIT
    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
*/
static CLKRST_RDBType * const CACHE_ClockRegs[CLKRST_MAX_HW_ID] =
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
    @trace #BRCM_SWREQ_CACHE_INTERRUPT
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_ENABLE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_CLEAR_PROC
*/
static SDC_LLC_INTR_RDBType * const CACHE_IntrRegs[CACHE_MAX_HW_ID] =
{
#if (SDC_LLC_INTR_MAX_HW_ID == 0U)
#error "SDC_LLC_INTR_MAX_HW_ID == 0U"
#endif
    (SDC_LLC_INTR_RDBType*)RIG_SDC_LLC_INTR_BASE,
#if (SDC_LLC_INTR_MAX_HW_ID > 1U)
#error "SDC_LLC_INTR_MAX_HW_ID > 1U is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_CACHE_INIT
    @trace #BRCM_SWARCH_CACHE_CONFIGURE_WAY_PARTITION_PROC
    @trace #BRCM_SWARCH_CACHE_ENABLE_WAY_PARTITION_PROC
    @trace #BRCM_SWARCH_CACHE_MAINTENANCE_CONTROL_PROC
    @trace #BRCM_SWARCH_CACHE_MAINTENANCE_STATUS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_ENABLE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_THOLD_CFG_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_CLEAR_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_FSC_CONTROL_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_FSC_STATUS_PROC
*/
typedef struct sCACHE_DrvDevType {
    CACHE_StateType   state;            /**< State of the driver                */
} CACHE_DrvDevType;

/**
    @trace #BRCM_SWARCH_CACHE_CONFIGURE_WAY_PARTITION_PROC
    @trace #BRCM_SWARCH_CACHE_ENABLE_WAY_PARTITION_PROC
    @trace #BRCM_SWARCH_CACHE_MAINTENANCE_CONTROL_PROC
    @trace #BRCM_SWARCH_CACHE_MAINTENANCE_STATUS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_ENABLE_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_THOLD_CFG_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_INTR_CLEAR_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_FSC_CONTROL_PROC
    @trace #BRCM_SWARCH_CACHE_DRV_FSC_STATUS_PROC
    @trace #BRCM_SWREQ_CACHE_INIT
*/
static CACHE_DrvDevType CACHE_DrvDev[CACHE_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @brief Maintenance operation

    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
    @trace #BRCM_SWREQ_CACHE_INIT
*/
static int32_t CACHE_IntMaintenanceOp(CACHE_HwIDType hwId, CACHE_MaintenanceControlType *maintenanceControl)
{
    int ret = BCM_ERR_OK;
    uint32_t mntControl = 0;
    uint32_t ccumcr = 0UL;

    CACHE_Regs[hwId]->ccucmlr0 = 0UL; /*Clear Way, Set, and word*/
    CACHE_Regs[hwId]->ccucmlr1 &= ~(CACHE_CCUCMLR1_MNTADDR_MASK);

    switch (maintenanceControl->maintenanceOperation) {
        case CACHE_MAINTENANCE_OP_INIT_VALID_ENT:
            ccumcr  = CACHE_Regs[hwId]->ccucmcr;
            ccumcr &= ~(CACHE_CCUCMCR_ARRAYID_MASK | CACHE_CCUCMCR_MNTOP_MASK);
            ccumcr |= (((uint32_t)maintenanceControl->arrayId << CACHE_CCUCMCR_ARRAYID_SHIFT) |
                        (maintenanceControl->maintenanceOperation)) ;
            CACHE_Regs[hwId]->ccucmcr = ccumcr;
            break;
        case CACHE_MAINTENANCE_OP_FLUSH_VALID_ENT:
            break;
        case CACHE_MAINTENANCE_OP_FLUSH_ENT_SET_WAY:
            CACHE_Regs[hwId]->ccucmlr0 |= (((uint32_t)maintenanceControl->way << CACHE_CCUCMLR0_MNTWAY_SHIFT) |
                            ((uint32_t)maintenanceControl->set << CACHE_CCUCMLR0_MNTSET_SHIFT));
            break;
        case CACHE_MAINTENANCE_OP_FLUSH_ENT_ADDR:
            CACHE_Regs[hwId]->ccucmlr0 |= (((uint32_t)maintenanceControl->way << CACHE_CCUCMLR0_MNTWAY_SHIFT) | ((uint32_t)maintenanceControl->word << CACHE_CCUCMLR0_MNTWORD_SHIFT) |
                            ((uint32_t)maintenanceControl->set << CACHE_CCUCMLR0_MNTSET_SHIFT));
            CACHE_Regs[hwId]->ccucmlr1 = maintenanceControl->hiAddr;
            break;
        case CACHE_MAINTENANCE_OP_FLUSH_ADDR_RANGE:
            CACHE_Regs[hwId]->ccucmlr2 &= ~(CACHE_CCUCMLR2_MNTRANGE_MASK);
            CACHE_Regs[hwId]->ccucmlr0 |= (((uint32_t)maintenanceControl->way << CACHE_CCUCMLR0_MNTWAY_SHIFT) | ((uint32_t)maintenanceControl->word << CACHE_CCUCMLR0_MNTWORD_SHIFT) |
                            ((uint32_t)maintenanceControl->set << CACHE_CCUCMLR0_MNTSET_SHIFT));
            CACHE_Regs[hwId]->ccucmlr1 = maintenanceControl->hiAddr;
            CACHE_Regs[hwId]->ccucmlr2 = maintenanceControl->range;
            break;
        case CACHE_MAINTENANCE_OP_FLUSH_SET_WAY_RANGE:
            CACHE_Regs[hwId]->ccucmlr2 &= ~(CACHE_CCUCMLR2_MNTRANGE_MASK);
            CACHE_Regs[hwId]->ccucmlr0 |= (((uint32_t)maintenanceControl->way << CACHE_CCUCMLR0_MNTWAY_SHIFT) |
                            ((uint32_t)maintenanceControl->set << CACHE_CCUCMLR0_MNTSET_SHIFT));
            CACHE_Regs[hwId]->ccucmlr2 = maintenanceControl->range;
            break;
        default :
           ret = BCM_ERR_INVAL_PARAMS;
           break;
    }
    if ((CACHE_MAINTENANCE_OP_INIT_VALID_ENT != maintenanceControl->maintenanceOperation) && (ret == BCM_ERR_OK)) {
        mntControl = CACHE_Regs[hwId]->ccucmcr;
        mntControl &= ~CACHE_CCUCMCR_MNTOP_MASK;
        mntControl |=  maintenanceControl->maintenanceOperation;
        CACHE_Regs[hwId]->ccucmcr = mntControl;
    }

    return ret;
}

/**
    @brief Maintenance Status

    @trace #BRCM_SWARCH_CACHE_DRV_INIT_PROC
    @trace #BRCM_SWREQ_CACHE_INIT
*/
static void CACHE_IntMaintenanceStatus(CACHE_HwIDType hwId, uint8_t *status)
{
    *status = (uint8_t)(CACHE_Regs[hwId]->ccucmar & CACHE_CCUCMAR_MNTOPACTV_MASK);
}


/** @brief Initialize Cache

    This API initializes the given Cache hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Cache controller

    @return     int32_t

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INIT
    @trace  #BRCM_SWARCH_CACHE_DRV_INIT_PROC

    @code{.unparsed}
        Validate hardware instance with max supported device
        Perform initialize valid entry maintenance operation for Tag array
        Wait till maintenance operation completes
            if Complete without error
                Perform initialize valid entry maintenance operation for data array
                Wait till maintenance operation completes
                if Complete without error
                     Move state to CACHE_STATE_INITIALIZED
                else
                     Move state to CACHE_STATE_UNINIT
                     return error
            else
               Move state to CACHE_STATE_UNINIT
               return error
    @endcode

    @limitations None
*/

int32_t CACHE_DrvInit(CACHE_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
    CACHE_MaintenanceControlType maintenanceControl;
    int maxRetry = 0L;
    uint8_t maintenanceSts = 0;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        CACHE_ClockRegs[0]->ddr_llc_top_reset_control &= ~(CLKRST_DLTRC_DDR_LLC_TOP_SDC_200CLK_RESETN_FRC_MASK|
                CLKRST_DLTRC_DDR_LLC_TOP_CRSTB_FRC_MASK|
                CLKRST_DLTRC_DDR_LLC_TOP_HRESETN_FRC_MASK);
        CACHE_CmnRegs[0]->llc_cntrl0_reg = (DDR_LLC_CMN_LLC_CNTRL0_CTYPE_RG_LLC_EN_MASK  | DDR_LLC_CMN_LLC_CNTRL0_CTYPE_FSC_WR_EN_MASK |
                                           DDR_LLC_CMN_LLC_CNTRL0_CTYPE_CSR_WR_EN_MASK);
        maintenanceControl.maintenanceOperation = 0;
        maintenanceControl.word = 0U;
        maintenanceControl.set = 0U;
        maintenanceControl.way = 0U;
        maintenanceControl.hiAddr = 0U;
        maintenanceControl.range = 0U;
        maintenanceControl.arrayId = 0U; /*Tag array*/
        ret = CACHE_IntMaintenanceOp(hwId, &maintenanceControl);
        maxRetry = 200L;
        do {
            CACHE_IntMaintenanceStatus(hwId, &maintenanceSts);
        } while (((maxRetry--) > 0L) && (maintenanceSts == 1U)); /*Wait till All the memory blocks are initialized*/
        if ((BCM_ERR_OK == ret) && (maintenanceSts == 0U)) {
            maintenanceControl.maintenanceOperation = 0;
            maintenanceControl.word = 0U;
            maintenanceControl.set = 0U;
            maintenanceControl.way = 0U;
            maintenanceControl.hiAddr = 0U;
            maintenanceControl.range = 0U;
            maintenanceControl.arrayId = 1U; /*Data Array*/
            ret = CACHE_IntMaintenanceOp(hwId, &maintenanceControl);
            maxRetry = 2000L;
            do {
                CACHE_IntMaintenanceStatus(hwId, &maintenanceSts);
            } while (((maxRetry--) > 0L) && (maintenanceSts == 1U)); /*Wait till All the memory blocks are initialized*/
            if ((BCM_ERR_OK == ret) && (maintenanceSts == 0U)) {
                CACHE_DrvDev[hwId].state = CACHE_STATE_INITIALIZED;
            } else {
                CACHE_DrvDev[hwId].state = CACHE_STATE_UNINIT;
                if (BCM_ERR_OK == ret) {
                    ret = BCM_ERR_UNINIT;
                }
            }
        } else {
            CACHE_DrvDev[hwId].state = CACHE_STATE_UNINIT;
            if (BCM_ERR_OK == ret) {
                ret = BCM_ERR_UNINIT;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief De-Initialize Cache

    This API de-initializes the specified cache

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the cache controller

    @return     #BCM_ERR_OK               De-Initialized cache instance
    @return     #BCM_ERR_INVAL_PARAMS     Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INIT
    @trace  #BRCM_SWARCH_CACHE_DRV_DEINIT_PROC

    @code{.unparsed}
        Validate hardware instance with max supported device
        Move state to CACHE_STATE_UNINIT
    @endcode


    @limitations None
*/

int32_t CACHE_DrvDeInit(CACHE_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*DeInit Cache*/
        CACHE_DrvDev[hwId].state = CACHE_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Configure Way partition

    This API configure way partition and way vector.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the cache
    @param[in]      partitionId      Cache Partition ID (0-3)
    @param[in]      partitionConfig  Way Partition configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Null pointer for partitionConfig
    @return     #BCM_ERR_INVAL_PARAMS   Invalid partition ID

    @post None
    @trace  #BRCM_SWREQ_CACHE_CONFIGURE
    @trace  #BRCM_SWARCH_CACHE_CONFIGURE_WAY_PARTITION_PROC

    @code{.unparsed}
        Validate hardware instance, device state, partition config and partition ID
        Based on partition ID configure
            way partition AgentID
            Way partition Vector Mask
    @endcode

    @limitations None
*/
int32_t CACHE_DrvConfigureWayPartition(CACHE_HwIDType hwId, uint8_t partitionId, CACHE_WayPartitionConfigType *partitionConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (partitionConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (partitionId > 3U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       if (partitionId == 0U) { /*Partition ID 0*/
          CACHE_Regs[hwId]->ccuwpcr00 &= ~(CACHE_CCUWPCR00_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr10 &= ~(CACHE_CCUWPCR10_WPWAYVECTOR_MASK);

          CACHE_Regs[hwId]->ccuwpcr00 |= (partitionConfig->agentId & CACHE_CCUWPCR00_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr10 |= (partitionConfig->wayVector & CACHE_CCUWPCR10_WPWAYVECTOR_MASK);

       } else if (partitionId == 1U) { /*Partition ID 0*/
          CACHE_Regs[hwId]->ccuwpcr01 &= ~(CACHE_CCUWPCR01_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr11 &= ~(CACHE_CCUWPCR12_WPWAYVECTOR_MASK);

          CACHE_Regs[hwId]->ccuwpcr01 |= (partitionConfig->agentId & CACHE_CCUWPCR00_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr11 |= (partitionConfig->wayVector & CACHE_CCUWPCR11_WPWAYVECTOR_MASK);
       } else if (partitionId == 2U) { /*Partition ID 0*/
          CACHE_Regs[hwId]->ccuwpcr02 &= ~(CACHE_CCUWPCR02_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr12 &= ~(CACHE_CCUWPCR12_WPWAYVECTOR_MASK);

          CACHE_Regs[hwId]->ccuwpcr02 |= (partitionConfig->agentId & CACHE_CCUWPCR00_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr12 |= (partitionConfig->wayVector & CACHE_CCUWPCR12_WPWAYVECTOR_MASK);
       } else {
          CACHE_Regs[hwId]->ccuwpcr03 &= ~(CACHE_CCUWPCR03_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr13 &= ~(CACHE_CCUWPCR13_WPWAYVECTOR_MASK);

          CACHE_Regs[hwId]->ccuwpcr03 |= (partitionConfig->agentId & CACHE_CCUWPCR03_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr13 |= (partitionConfig->wayVector & CACHE_CCUWPCR13_WPWAYVECTOR_MASK);

       }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Enable Way Partition

    This API enables/disables way partition.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the cache
    @param[in]      partitionId      Cache Partition ID (0-3)
    @param[in]      enDis            Enables(1) or Disables(0) way partition

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid partition ID
    @return     #BCM_ERR_INVAL_PARAMS   Invalid value for enDis

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE
    @trace  #BRCM_SWARCH_CACHE_CONFIGURE_WAY_PARTITION_PROC

    @code{.unparsed}
        Validate hardware instance, device state and partition ID
        Based on partition ID enable/diable way partition
    @endcode

    @limitations None
*/
int32_t CACHE_DrvEnableWayPartition(CACHE_HwIDType hwId, uint8_t partitionId, uint8_t enDis)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (partitionId > 3U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (enDis > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       if (partitionId == 0U) { /*Partition ID 0*/
          CACHE_Regs[hwId]->ccuwpcr00 &= ~(CACHE_CCUWPCR00_WPAGENTIDVALID_MASK);
          CACHE_Regs[hwId]->ccuwpcr00 |= ((uint32_t)enDis << CACHE_CCUWPCR00_WPAGENTIDVALID_SHIFT);
       } else if (partitionId == 1U) { /*Partition ID 0*/
          CACHE_Regs[hwId]->ccuwpcr01 &= ~(CACHE_CCUWPCR01_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr01 |= ((uint32_t)enDis << CACHE_CCUWPCR01_WPAGENTIDVALID_SHIFT);
       } else if (partitionId == 2U) { /*Partition ID 0*/
          CACHE_Regs[hwId]->ccuwpcr02 &= ~(CACHE_CCUWPCR02_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr02 |= ((uint32_t)enDis << CACHE_CCUWPCR02_WPAGENTIDVALID_SHIFT);
       } else {
          CACHE_Regs[hwId]->ccuwpcr03 &= ~(CACHE_CCUWPCR03_WPAGENTID_MASK);
          CACHE_Regs[hwId]->ccuwpcr03 |= ((uint32_t)enDis << CACHE_CCUWPCR03_WPAGENTIDVALID_SHIFT);
       }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}



/** @brief Cache policy override

    @code{.unparsed}
        Validate hardware instance, device state and partition ID
        Write rdPolicy and enable override
        Write wrPolicy and enable override
    @endcode

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE
    @trace  #BRCM_SWARCH_CACHE_DRV_OVERRIDE_CACHE_POLICY_PROC

    @limitations None
*/
int32_t CACHE_DrvOverrideCachePolicy(CACHE_HwIDType hwId, CACHE_PolicyType rdPolicy, CACHE_PolicyType wrPolicy)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if (rdPolicy != CACHE_POLICY_NO_OVERRIDE) {
            CACHE_Regs[hwId]->ccucaor &= ~(CACHE_CCUCAOR_ARALLOCEN_MASK | CACHE_CCUCAOR_ARALLOCVALUE_MASK);

            CACHE_Regs[hwId]->ccucaor |= (CACHE_CCUCAOR_ARALLOCEN_MASK |
                                          (rdPolicy <<CACHE_CCUCAOR_ARALLOCVALUE_SHIFT));
        } else {
            CACHE_Regs[hwId]->ccucaor &= ~(CACHE_CCUCAOR_ARALLOCEN_MASK);
        }
        if (wrPolicy != CACHE_POLICY_NO_OVERRIDE) {
            CACHE_Regs[hwId]->ccucaor &= ~(CACHE_CCUCAOR_AWALLOCEN_MASK | CACHE_CCUCAOR_AWALLOCVALUE_MASK);
            CACHE_Regs[hwId]->ccucaor |= ((wrPolicy << CACHE_CCUCAOR_AWALLOCVALUE_SHIFT) & CACHE_CCUCAOR_AWALLOCVALUE_MASK);
            CACHE_Regs[hwId]->ccucaor |= CACHE_CCUCAOR_AWALLOCEN_MASK;
        } else {
            CACHE_Regs[hwId]->ccucaor &= ~(CACHE_CCUCAOR_AWALLOCEN_MASK);
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief Configure specified Cache

    This API configure specified cache with following features based on specified input,
            - ScratchPad
            - Error Detection and
            - Partial Write Allocation

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the cache controller
    @param[in]      errDetection          Enable(1)/Disable(0) Error Detaction
    @param[in]      wrAllocatePartialEn   Enable(1)/Disable(0) Partial write allocation
    @param[in]      scratchPadConfig      ScratchPad Configuration

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_NODEV            Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS     Invalid HW Index
    @return     #BCM_ERR_INVAL_PARAMS     Memory not valid for scratchPadConfig

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE
    @trace  #BRCM_SWARCH_CACHE_DRV_CONFIGURE_PROC

    @code{.unparsed}
        Validate hardware instance, device state, scratch pad config
        Enable/Disable error detection
        Enable/Disable write allocate partial
        Configure scratch pad size, base address, number of scratch pad ways.
        Enable/Disable scratch pad.
    @endcode


    @limitations None
*/
int32_t CACHE_DrvConfigure(CACHE_HwIDType hwId, uint8_t errDetection, uint8_t wrAllocatePartial, CACHE_ScratchPadConfigType *scratchPadConfig)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (scratchPadConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (errDetection == 1U) {
           CACHE_Regs[hwId]->ccucecr |= CACHE_CCUCECR_ERRDETEN_MASK;
        } else {
           CACHE_Regs[hwId]->ccucecr &= ~CACHE_CCUCECR_ERRDETEN_MASK;
        }
        if (wrAllocatePartial == 1U) {
           CACHE_Regs[hwId]->ccucaor |= CACHE_CCUCAOR_WRALLOCPARTIALEN_MASK;
        } else {
           CACHE_Regs[hwId]->ccucaor &= ~CACHE_CCUCAOR_WRALLOCPARTIALEN_MASK;
        }
        CACHE_Regs[hwId]->ccuspcr1 = scratchPadConfig->size;
        CACHE_Regs[hwId]->ccuspbr0 = scratchPadConfig->baseAddress;
        CACHE_Regs[hwId]->ccuspbr1 = (scratchPadConfig->baseAddressHi & CACHE_CCUSPBR1_SCPADBASEADDRHI_MASK);
        CACHE_Regs[hwId]->ccuspcr0 &= ~(CACHE_CCUSPCR0_NUMSCPADWAYS_MASK | CACHE_CCUSPCR0_SCPADEN_MASK);
        CACHE_Regs[hwId]->ccuspcr0 |= (((uint32_t)scratchPadConfig->numPadWay << CACHE_CCUSPCR0_NUMSCPADWAYS_SHIFT) & CACHE_CCUSPCR0_NUMSCPADWAYS_MASK);
        CACHE_Regs[hwId]->ccuspcr0 |= (scratchPadConfig->enable & CACHE_CCUSPCR0_SCPADEN_MASK);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief Enable/Disable specified Cache

    This API configures cache miss and cache lookup on a specified cache.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the cache controller
    @param[in]      fillEn          If the bit is set to 1, cache allocate on miss is enabled else(0) it is not enabled.
    @param[in]      lookupEn        If the bit is set to 1, the cache is accessed, else(0) it is bypassed.

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware ID
    @return     #BCM_ERR_INVAL_PARAMS   Invalid value for fillEn or lookupEn

    @post None

    @trace  #BRCM_SWREQ_CACHE_CONFIGURE
    @trace  #BRCM_SWARCH_CACHE_DRV_CONTROL_PROC

    @code{.unparsed}
        Validate hardware instance, device state, scratch pad config
        Enable/Disable Cache lookup
        Enable/Disable cache miss/Fill enable
    @endcode


    @limitations None
*/
int32_t CACHE_DrvControl(CACHE_HwIDType hwId, uint8_t fillEn, uint8_t lookupEn)
{
    int ret = BCM_ERR_OK;
    uint32_t cacheControl = 0;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((fillEn > 1U) || (lookupEn > 1U)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
       cacheControl = CACHE_Regs[hwId]->ccuctcr;
       cacheControl &= ~(CACHE_CCUCTCR_LOOKUPEN_MASK | CACHE_CCUCTCR_FILLEN_MASK);
       cacheControl |= (((uint32_t)lookupEn << CACHE_CCUCTCR_LOOKUPEN_SHIFT) | (fillEn << CACHE_CCUCTCR_FILLEN_SHIFT));
       CACHE_Regs[hwId]->ccuctcr = cacheControl;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return ret;
}

/** @brief Cache Maintenance operation

    This API perform cache maintenance operation based on user specified operation.
    User also need to configure Address, Way, Set and range based on the operation choosed.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the cache
    @param[in]      maintenanceControl Maintenance configuration and control

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for maintenanceControl

    @post None

    @trace  #BRCM_SWREQ_CACHE_MAINTENANCE_OP
    @trace  #BRCM_SWARCH_CACHE_MAINTENANCE_CONTROL_PROC

    @code{.unparsed}
        Validate hardware instance and maintenance control
        Based on maintenance operation configure,
            - Set
            - Word
            - Address
            - Array ID
            - Way
            - range
        Configure Maintenance opcode in maintenance control register

    @endcode

    @limitations None
*/
int32_t CACHE_DrvMaintenanceControl(CACHE_HwIDType hwId, CACHE_MaintenanceControlType *maintenanceControl)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= 1UL) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (maintenanceControl == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = CACHE_IntMaintenanceOp(hwId, maintenanceControl);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Cache Maintenance operation status

    This API retrieve cache maintenance operation status, Alternatively user can use @ref CACHE_DrvIntrConfig to enable interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId               Index of the cache.
    @param[out]     maintenanceSts     Maintenance status, This is set(1) when any cache maintenance operation is in progress and clear(0) otherwise.

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for maintenanceSts

    @post None

    @trace  #BRCM_SWREQ_CACHE_MAINTENANCE_OP
    @trace  #BRCM_SWARCH_CACHE_MAINTENANCE_STATUS_PROC

    @code{.unparsed}
        Validate hardware instance and maintenance status
        Get maintenance status by reading maintenance active
    @endcode

    @limitations None
*/
int32_t CACHE_DrvMaintenanceStatus(CACHE_HwIDType hwId, uint8_t *maintenanceSts)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= 1UL) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (maintenanceSts == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        CACHE_IntMaintenanceStatus(hwId, maintenanceSts);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief Set Correctable Interrupt Threshold

    This API set the correctable Error Threshold value for CACHE_INTR_MEM_CORR_ERR interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      errThreshold           8-bit threshold value

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @trace  #BRCM_SWARCH_CACHE_DRV_INTR_THOLD_CFG_PROC
    @code{.unparsed}
        Validate hardware instance, Device state and maintenance status
        Configure error threshold to error control register
    @endcode

    @limitations None
*/

int32_t CACHE_DrvIntrThresholdConfig(CACHE_HwIDType hwId, uint8_t  errThreshold)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
       CACHE_Regs[hwId]->ccucecr &= ~CACHE_CCUCECR_ERRTHRESHOLD_MASK;
       CACHE_Regs[hwId]->ccucecr |= (uint32_t)errThreshold << CACHE_CCUCECR_ERRTHRESHOLD_SHIFT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief FSC control

    This API allows user to enable or disable fault safety controller to monitor and logs the faults.
    It also perform step by step BIST or all 5 step at a time in sequence.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the cache controller
    @param[in]      bistStep              Writing 1 will command the BIST FSM to move to the next BIST step in the sequence.
                                          bistStep and bistAllSeq are mutually exclusive, SW driver internally disable bistAllSeq when bistStep is 1.
    @param[in]      bistAllSeq            Writing a 1 will command the BIST FSM to sequence all 5 steps of the BIST sequence automatically.
                                          bistStep and bistAllSeq are mutually exclusive, SW driver internally disable bistAllSeq when bistStep is 1.

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid BIST all sequence/bistStep

    @post None

    @trace  #BRCM_SWREQ_CACHE_FSC
    @trace  #BRCM_SWARCH_CACHE_DRV_FSC_CONTROL_PROC

    @code{.unparsed}
        Validate hardware instance, Device state and bistAllSeq.
        if bistStep is set write bist step bit to 1.
        else write bistAllSeq to 1.
    @endcode

    @limitations None
*/

int32_t CACHE_DrvFscControl(CACHE_HwIDType hwId, uint8_t bistStep, uint8_t bistAllSeq)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_FSC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if ((bistAllSeq > 1U) || (bistStep > 1U)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (bistStep == 1U) {
             CACHE_FscRegs[hwId]->fscbistcr = CACHE_FSC_FSCBISTCR_BIST_STEP_MASK;
        } else {
             CACHE_FscRegs[hwId]->fscbistcr = bistAllSeq;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief FSC activity monitor

    This API allows user to monitor FSC BIST FSM.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the cache controller
    @param[out]     bistErr               Each bit in this register represent one step of the BIST sequence. When the sequence starts,
                                          it resets. As each BIST step completes, bit is set to 1 if the sequence has completed in error
    @param[out]     bistDone              Each bit in this register represent one step of the BIST sequence.
                                          When the sequence starts, it's reset. As each BIST step completes, its bit is set to 1

    @return     #BCM_ERR_OK             On Success
    @return     #BCM_ERR_NODEV          Cache instance not initialized
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for bistErr
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for bistDone

    @post None

    @trace  #BRCM_SWREQ_CACHE_FSC
    @trace  #BRCM_SWARCH_CACHE_DRV_FSC_STATUS_PROC

    @code{.unparsed}
        Validate hardware instance, Device state, bistErr,BistDone.
        Read bist activity register and populate bist err and bist Done
    @endcode

    @limitations None
*/

int32_t CACHE_DrvFscStatus(CACHE_HwIDType hwId, uint8_t *bistErr, uint8_t *bistDone)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_FSC_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else if (bistErr == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (bistDone == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *bistErr = (uint8_t)((CACHE_FscRegs[hwId]->fscbistar & CACHE_FSC_FSCBISTAR_BIST_ERR_MASK) >> CACHE_FSC_FSCBISTAR_BIST_ERR_SHIFT);
        *bistDone = (uint8_t)(CACHE_FscRegs[hwId]->fscbistar & CACHE_FSC_FSCBISTAR_BIST_DONE_MASK);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Enable Interrupt Mask

    This API enable/Disable specified cache interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      intrType              Type of interrupt
    @param[in]      enControl             Enable(1)/Disable(0) specified interrupt

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter
    @return     #BCM_ERR_NODEV          Cache instance not initialized

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @trace  #BRCM_SWARCH_CACHE_DRV_INTR_ENABLE_PROC
    @code{.unparsed}
        Validate hardware instance, Device state, bistErr,BistDone.
        Enable interrupt based on the interrupt type
    @endcode

    @limitations None
*/

int32_t CACHE_DrvIntrConfig(CACHE_HwIDType hwId, CACHE_IntrType intrType, uint8_t  enControl)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if (CACHE_INTR_UNCORR_ERR_DET == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eier &= ~SDC_LLC_INTR_LFIE2_ENABLE_IRQ_UC_LEVEL_INTR_MASK;
            CACHE_IntrRegs[hwId]->llc_func_intr_eier |= ((uint32_t)enControl << SDC_LLC_INTR_LFIE2_ENABLE_IRQ_UC_LEVEL_INTR_SHIFT);
            CACHE_Regs[hwId]->ccucecr |= CACHE_CCUUEIR_MEMERRINTEN_MASK;/* Enable for block level*/
        } else if (CACHE_INTR_MAINTENANCE_PERORMANCE_COMP == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eier &= ~SDC_LLC_INTR_LFIE2_ENABLE_IRQ_STATUS_LEVEL_INTR_MASK;
            CACHE_IntrRegs[hwId]->llc_func_intr_eier |= ((uint32_t)enControl << SDC_LLC_INTR_LFIE2_ENABLE_IRQ_STATUS_LEVEL_INTR_SHIFT);
            CACHE_Regs[hwId]->ccuemr |= 0x3UL; /* Enable for block level*/
        } else if (CACHE_INTR_CORR_ERR_DET == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eier &= ~SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_SINGLE_PULSE_INTR_MASK;
            CACHE_IntrRegs[hwId]->llc_func_intr_eier |= ((uint32_t)enControl << SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_SINGLE_PULSE_INTR_SHIFT);
            CACHE_Regs[hwId]->ccucecr |= CACHE_CCUCECR_ERRDETEN_MASK; /* Enable for block level*/
        } else if (CACHE_INTR_CORR_ERR_ABV_THOLD == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eier &= ~SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_LEVEL_INTR_MASK;
            CACHE_IntrRegs[hwId]->llc_func_intr_eier |= ((uint32_t)enControl << SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_LEVEL_INTR_SHIFT);
            CACHE_Regs[hwId]->ccucecr |= CACHE_CCUCECR_ERRINTEN_MASK; /* Enable for block level*/
        } else {
            CACHE_Regs[hwId]->ccuemr |= 0x3UL; /* Enable for block level*/
            CACHE_Regs[hwId]->ccucecr |= CACHE_CCUCECR_ERRINTEN_MASK | CACHE_CCUCECR_ERRDETEN_MASK | CACHE_CCUUEIR_MEMERRINTEN_MASK;
            if (enControl == 1U) {
                CACHE_IntrRegs[hwId]->llc_func_intr_eier |= (SDC_LLC_INTR_LFIE2_ENABLE_IRQ_UC_LEVEL_INTR_MASK |
                        SDC_LLC_INTR_LFIE2_ENABLE_IRQ_STATUS_LEVEL_INTR_MASK | SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_SINGLE_PULSE_INTR_MASK|
                        SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_LEVEL_INTR_MASK);
            } else {
                CACHE_IntrRegs[hwId]->llc_func_intr_eier &= ~(SDC_LLC_INTR_LFIE2_ENABLE_IRQ_UC_LEVEL_INTR_MASK |
                        SDC_LLC_INTR_LFIE2_ENABLE_IRQ_STATUS_LEVEL_INTR_MASK | SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_SINGLE_PULSE_INTR_MASK|
                        SDC_LLC_INTR_LFIE2_ENABLE_IRQ_C_LEVEL_INTR_MASK);
            }
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Clear Interrupt

    This API clear specified interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      intrType               Type of interrupt to clear

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter
    @return     #BCM_ERR_NODEV          Cache instance not initialized

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @trace  #BRCM_SWARCH_CACHE_DRV_INTR_CLEAR_PROC

    @code{.unparsed}
        Validate hardware instance, Device state, bistErr,BistDone.
        clear interrupt based on the interrupt type
    @endcode

    @limitations None
*/

int32_t CACHE_DrvIntrClear(CACHE_HwIDType hwId, CACHE_IntrType intrType)
{
    int ret = BCM_ERR_OK;
    uint32_t eisr = 0;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        eisr = CACHE_IntrRegs[hwId]->llc_func_intr_eisr; /* read is mandatory*/
        (void)eisr;
        if (CACHE_INTR_UNCORR_ERR_DET == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eisr |= (SDC_LLC_INTR_LFIE1_IRQ_UC_LEVEL_INTR_W2CLR_MASK);
        } else if (CACHE_INTR_MAINTENANCE_PERORMANCE_COMP == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eisr |= SDC_LLC_INTR_LFIE1_IRQ_STATUS_LEVEL_INTR_W2CLR_MASK;
            CACHE_Regs[hwId]->ccuesr |= CACHE_CCUESR_STATUS_MASK;
        } else if (CACHE_INTR_CORR_ERR_DET == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eisr |= SDC_LLC_INTR_LFIE1_IRQ_C_SINGLE_PULSE_INTR_W2CLR_MASK;
        } else if (CACHE_INTR_CORR_ERR_ABV_THOLD == intrType) {
            CACHE_IntrRegs[hwId]->llc_func_intr_eisr |= (SDC_LLC_INTR_LFIE1_IRQ_C_LEVEL_INTR_W2CLR_MASK);
        } else {
            CACHE_Regs[hwId]->ccuesr |= CACHE_CCUESR_STATUS_MASK;
            CACHE_IntrRegs[hwId]->llc_func_intr_eisr |= (SDC_LLC_INTR_LFIE1_IRQ_UC_LEVEL_INTR_W2CLR_MASK|
               SDC_LLC_INTR_LFIE1_IRQ_STATUS_LEVEL_INTR_W2CLR_MASK | SDC_LLC_INTR_LFIE1_IRQ_C_SINGLE_PULSE_INTR_W2CLR_MASK |
               SDC_LLC_INTR_LFIE1_IRQ_C_LEVEL_INTR_W2CLR_MASK);
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Get all interrupt Status

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @trace  #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC
*/
static void CACHE_IntGetAllIntrStatus(CACHE_HwIDType hwId, uint32_t *intrStatus)
{
    *intrStatus = (((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_UC_LEVEL_INTR_MASK) > 0UL) ? CACHE_INTR_UNCORR_ERR_DET : 0UL) |
                         (((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_STATUS_LEVEL_INTR_MASK) > 0UL) ? CACHE_INTR_MAINTENANCE_PERORMANCE_COMP : 0UL) |
                         (((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_C_SINGLE_PULSE_INTR_MASK) > 0UL) ? CACHE_INTR_CORR_ERR_DET : 0UL) |
                         (((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_C_LEVEL_INTR_MASK) > 0UL) ? CACHE_INTR_CORR_ERR_ABV_THOLD : 0UL);

}

/**
    @brief Get Event Status

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @trace  #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC
*/
static void CACHE_IntGetEventStatus(CACHE_HwIDType hwId, uint32_t *intrStatus)
{

    *intrStatus =  (((CACHE_IntrRegs[hwId]->llc_func_intr_eisr & SDC_LLC_INTR_LFIE1_IRQ_UC_LEVEL_INTR_W2CLR_MASK) > 0UL) ? CACHE_INTR_UNCORR_ERR_DET : 0UL) |
                   (((CACHE_IntrRegs[hwId]->llc_func_intr_eisr & SDC_LLC_INTR_LFIE1_IRQ_STATUS_LEVEL_INTR_W2CLR_MASK) > 0UL) ? CACHE_INTR_MAINTENANCE_PERORMANCE_COMP : 0UL) |
                   (((CACHE_IntrRegs[hwId]->llc_func_intr_eisr & SDC_LLC_INTR_LFIE1_IRQ_C_SINGLE_PULSE_INTR_W2CLR_MASK) > 0UL) ? CACHE_INTR_CORR_ERR_DET : 0UL) |
                   (((CACHE_IntrRegs[hwId]->llc_func_intr_eisr & SDC_LLC_INTR_LFIE1_IRQ_C_LEVEL_INTR_W2CLR_MASK ) > 0UL) ? CACHE_INTR_CORR_ERR_ABV_THOLD : 0UL) ;
}

/** @brief Get Interrupt Status

    This API get staus of cache interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                   Index of the cache controller
    @param[in]      intrType              Interrupt type
    @param[out]     intrStatus            Interrupt Status

    @return     #BCM_ERR_OK               On Success
    @return     #BCM_ERR_INVAL_PARAMS     Invalid Parameter
    @return     #BCM_ERR_NODEV          Cache instance not initialized

    @post None

    @trace  #BRCM_SWREQ_CACHE_INTERRUPT
    @trace  #BRCM_SWARCH_CACHE_DRV_INTR_STS_PROC

    @code{.unparsed}
        Validate hardware instance, Device state, bistErr,BistDone.
        Read interrupt status based on the interrupt type
    @endcode

    @limitations None
*/
int32_t CACHE_DrvIntrStatus(CACHE_HwIDType hwId, CACHE_IntrType intrType, uint32_t *intrStatus)
{
    int ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= CACHE_MAX_HW_ID) { /* Validate HW instance*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (CACHE_DrvDev[hwId].state != CACHE_STATE_INITIALIZED) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if (CACHE_INTR_ALL == intrType) {
            CACHE_IntGetAllIntrStatus(hwId, intrStatus);
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            if (CACHE_INTR_UNCORR_ERR_DET == intrType) {
                *intrStatus = ((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_UC_LEVEL_INTR_MASK) > 0UL) ? 1UL : 0UL;
            } else if (CACHE_INTR_MAINTENANCE_PERORMANCE_COMP == intrType) {
                *intrStatus = ((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_STATUS_LEVEL_INTR_MASK) > 0UL) ? 1UL : 0UL;
            } else if (CACHE_INTR_CORR_ERR_DET == intrType) {
                *intrStatus = ((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_C_SINGLE_PULSE_INTR_MASK) > 0UL) ? 1UL:0UL;
            } else if (CACHE_INTR_CORR_ERR_ABV_THOLD == intrType) {
                *intrStatus = ((CACHE_IntrRegs[hwId]->llc_func_intr_eipr & SDC_LLC_INTR_LFIE_P_IRQ_C_LEVEL_INTR_MASK) > 0UL) ? 1UL : 0UL;
            }
        }
#endif
        if(CACHE_INTR_EVENT_ALL == intrType) {
            CACHE_IntGetEventStatus(hwId, intrStatus);
       }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @} */


